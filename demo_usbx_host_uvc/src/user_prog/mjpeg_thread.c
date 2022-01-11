/* motion JPEG thread and functions */
#include "tx_api.h"
#include "mjpeg_defs.h"

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_gpio_drv_api.h"

/**********************************************************************************************************************
 Imported global variables and functions (from other files)
 *********************************************************************************************************************/
/* data received semaphore */
extern TX_SEMAPHORE g_data_received_semaphore;

/**********************************************************************************************************************
 Private global variables and functions
 *********************************************************************************************************************/
#define MAX_STREAM_BUFFER   2
#define MAX_STREAM_SIZE     (512*1024)

#define STREAM_STATUS_FIND          0
#define STREAM_STATUS_DATA_ACTIVE   0x01
#define STREAM_STATUS_DATA_EOF      0x02

#define MUTEX_WAIT_TIME             10

/* index of frame_data */
static int frame_check_index = 0;

/* Check status of stream data */
static int stream_status;

/* Structure that stores 1 frame of JPEG data */
typedef struct
{
    ULONG length;
    UCHAR image[MAX_STREAM_SIZE];
} STREAM_BUFFER;

/* JPEG data for MAX_STREAM_BUFFER frames */
STREAM_BUFFER stream_buffer[MAX_STREAM_BUFFER];

/* index of stream_buffer */
static int current_stream_index = 0;

/* JPEG image data for display */
STREAM_BUFFER* current_display_image = NULL;

/* JPEG image data for update */
STREAM_BUFFER* current_stream_image = &stream_buffer[0];

/* mutex for stream_buffer */
static TX_MUTEX mjpeg_mutex;

/* Value for searching for frame switching */
static int fid = -1;


static int_t s_my_gpio_handle;
static st_r_drv_gpio_pin_rw_t s_p82_hi =
{
    GPIO_PORT_8_PIN_2,
    GPIO_LEVEL_HIGH,
    GPIO_SUCCESS
};
static st_r_drv_gpio_pin_rw_t s_p82_lo =
{
    GPIO_PORT_8_PIN_2,
    GPIO_LEVEL_LOW,
    GPIO_SUCCESS
};
static const r_gpio_port_pin_t s_led_pin_list[] =
{
    GPIO_PORT_8_PIN_2,
};

/* prototypes */
void display_image_update(void);
int check_stream(FRAME_DATA* p_frame_data);


/* update image data from mjpeg thread */
void display_image_update(void)
{
    if(tx_mutex_get(&mjpeg_mutex, TX_NO_WAIT)==TX_SUCCESS)
    {
        /* Change current_display_image */
        current_display_image = current_stream_image;

        /* Change current_stream_image */
        current_stream_index++;
        if(MAX_STREAM_BUFFER<=current_stream_index)
        {
            current_stream_index = 0;
        }
        current_stream_image = &stream_buffer[current_stream_index];

        tx_mutex_put(&mjpeg_mutex);
    }
    /* If the mutex cannot be obtained, it will update the current image. */
}

/* get image data from display thread */
ULONG display_image_get(UCHAR** image)
{
    ULONG rtn = 0;

    if(tx_mutex_get(&mjpeg_mutex, MUTEX_WAIT_TIME)==TX_SUCCESS)
    {
        /* Returns the current JPEG image data. */
        if(current_display_image!=NULL)
        {
            rtn = current_display_image->length;

            if(image!=NULL)
            {
                *image = current_display_image->image;
            }
        }

        if(rtn==0)
        {
            tx_mutex_put(&mjpeg_mutex);
        }
    }

    return rtn;
}

/* release image data from display thread */
void display_image_release(void)
{
    current_display_image = NULL;
    tx_mutex_put(&mjpeg_mutex);
}

/* check image data and update */
int check_stream(FRAME_DATA* p_frame_data)
{
    uvc_stream_header_t* uvc_stream_header;
    char* jpeg_stream_data;
    int jpeg_stream_length;

    direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_hi);

    if(0<p_frame_data->length)
    {
        /* Get UVC stream header */
        uvc_stream_header = (uvc_stream_header_t *)p_frame_data->p_frame;
        jpeg_stream_data = (char*)&p_frame_data->p_frame[sizeof(uvc_stream_header_t)];
        jpeg_stream_length = (int)(p_frame_data->length - sizeof(uvc_stream_header_t));

        if (0 < jpeg_stream_length)
        {
            if(stream_status==STREAM_STATUS_FIND)
            {
                if(2<=jpeg_stream_length)
                {
                    /* check JPEG SOF */
                    if(((jpeg_stream_data[0]==(char)0xff) && (jpeg_stream_data[1]==(char)0xd8))
                       && (fid != (int)(uvc_stream_header->bmHeaderInfo&UVC_STREAM_HEADER_HEADERINFO_FID)))
                    {
                        /* stream data active */
                        stream_status = STREAM_STATUS_DATA_ACTIVE;

                        current_stream_image->length = 0;
                        fid = (int)(uvc_stream_header->bmHeaderInfo&UVC_STREAM_HEADER_HEADERINFO_FID);
                    }
                }
            }

            if((stream_status!=STREAM_STATUS_FIND)
               && (fid != (int)(uvc_stream_header->bmHeaderInfo&UVC_STREAM_HEADER_HEADERINFO_FID)))
            {
                /* The frame data has been lost. Start over from the frame start search. */
                stream_status = STREAM_STATUS_FIND;
                current_stream_image->length = 0;
                fid = -1;

                direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_lo);

                return 0;
            }

            if(stream_status!=STREAM_STATUS_FIND)
            {
                if(MAX_STREAM_SIZE<(current_stream_image->length+(ULONG)jpeg_stream_length))
                {
                    /* The storage area has been exceeded. Start over from the frame start search. */
                    stream_status = STREAM_STATUS_FIND;
                    current_stream_image->length = 0;

                    direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_lo);

                    return 0;
                }

                /* copy to stream buffer */
                memcpy(&current_stream_image->image[current_stream_image->length],
                        jpeg_stream_data, (size_t)jpeg_stream_length);
                current_stream_image->length += (ULONG)jpeg_stream_length;
            }

            if((uvc_stream_header->bmHeaderInfo&UVC_STREAM_HEADER_HEADERINFO_EOF)!=0)
            {
                display_image_update();
                stream_status = STREAM_STATUS_FIND;
                current_stream_image->length = 0;
                fid = -1;
            }
        }
    }

    direct_control(s_my_gpio_handle, CTL_GPIO_PIN_WRITE, &s_p82_lo);

    return 0;
}

/* Define the mjpeg thread.  */
void mjpeg_thread_entry(ULONG thread_input)
{
    int_t err;
    st_r_drv_gpio_pin_list_t pin_led;

    s_my_gpio_handle = open(DEVICE_INDENTIFIER "gpio", O_RDWR);

    /* On error */
    if (s_my_gpio_handle < 0)
    {
        /* stop execute */
        while (1)
        {
            ;
        }
    }
    /**************************************************
     * Initialise P6_0 pin parameterised in GPIO_SC_TABLE_MANUAL
     **************************************************/
    pin_led.p_pin_list = s_led_pin_list;
    pin_led.count = (sizeof(s_led_pin_list)) / (sizeof(s_led_pin_list[0]));
    err = direct_control(s_my_gpio_handle, CTL_GPIO_INIT_BY_PIN_LIST, &pin_led);

    /* On error */
    if (err < 0)
    {
        /* stop execute */
        while (1)
        {
            ;
        }
    }

    tx_mutex_create(&mjpeg_mutex, "mjpeg mutex", TX_NO_INHERIT);

    do
    {
        uvc_camera_stop();

        stream_status = STREAM_STATUS_FIND; /* find top stream data */
        frame_check_index = 0;

        current_stream_image = &stream_buffer[0];
        current_stream_image->length = 0;
        uvc_camera_start();

        while(tx_semaphore_get(&g_data_received_semaphore, 200)==TX_SUCCESS)
        {
            /* UVC data is received, the data is analyzed. */
            if(check_stream(&frame_data[frame_check_index])==0)
            {
                /* Release the UVC data. */
                uvc_frame_finish(frame_data[frame_check_index].p_frame); 

                frame_check_index++;
                if(MAX_NUM_BUFFERS<=frame_check_index)
                {
                    frame_check_index = 0;
                }
            }
        }
    } while(1);

    tx_mutex_delete(&mjpeg_mutex);
}

