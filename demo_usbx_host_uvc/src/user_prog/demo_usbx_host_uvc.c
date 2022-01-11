/* This is a small demo of the high-performance USBX Host Video class.  */

#include <stdio.h>

#include "tx_api.h"
#include "ux_api.h"

#include "fx_api.h"

#include "ux_host_class_hub.h"
#include "ux_host_class_video.h"
#include "ux_host_stack.h"
#include "ux_system.h"

#include "mjpeg_defs.h"

#include "ux_rz_driver.h"

#include "r_typedefs.h"
#include "driver.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define SHOW_VIDEO_INFOMATION

/******************************************************************************
Private global variables and functions
******************************************************************************/

#define RZA2M_USB_HOST0 (ULONG)(0xE8218000)
#define RZA2M_USB_OHCI_HOST0    (RZA2M_USB_HOST0+0x0000)
#define RZA2M_USB_EHCI_HOST0    (RZA2M_USB_HOST0+0x0100)

#define RZA2M_USB_HOST1 (ULONG)(0xE821A000)
#define RZA2M_USB_OHCI_HOST1    (RZA2M_USB_HOST1+0x0000)
#define RZA2M_USB_EHCI_HOST1    (RZA2M_USB_HOST1+0x0100)


#define DEMO_STACK_SIZE         2048
#define UX_DEMO_BUFFER_SIZE     512

#define EVENTFLAG_USB_DEVICE_INSERTED   0x01

/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD thread_0;
TX_THREAD http_thread;
TX_THREAD mjpeg_thread;

ULONG error_counter = 0;

/* device insert flag */
TX_EVENT_FLAGS_GROUP g_device_insert_eventflag;

/* data received semaphore */
TX_SEMAPHORE g_data_received_semaphore;

/* video class instance */
UX_HOST_CLASS_VIDEO* volatile video_host_class;

/* Definition of the process of adding a video buffer. */
/* Please enable either HIGH_BANDWIDTH_EHCI or NORMAL_BANDWIDTH_OHCI. */
#define HIGH_BANDWIDTH_EHCI 1
//#define NORMAL_BANDWIDTH_OHCI 1

#define VIDEO_BUFFER_SIZE   (4*1024)
UCHAR video_buffer[MAX_NUM_BUFFERS][VIDEO_BUFFER_SIZE] __attribute__ ((section ("USB_SMPL_0")));


FRAME_DATA frame_data[MAX_NUM_BUFFERS];
int frame_index = 0;

/* Camera spec data */
SPEC_OF_CAMERA camera_spec[200];
int camera_spec_count = 0;
int camera_spec_select = 0;

/* First select camera spec */
#define FIRST_SELECT_WIDTH  320
#define FIRST_SELECT_HEIGHT 240
#define FIRST_SELECT_INTERVAL   2000000


/* Static memory pool allocation used by USBX system. */
CHAR g_ux_pool_memory[64*1024];
CHAR g_ux_uncache_pool_memory[64*1024] __attribute__ ((section ("USB_SMPL_0")));

/* Define prototypes.  */
static UINT _host_change_function(ULONG event, UX_HOST_CLASS * class, VOID * instance);
VOID uvc_transfer_request_done_callback(UX_TRANSFER * transfer_request);
VOID uvc_parameter_interval_list(UX_HOST_CLASS_VIDEO *video, int width, int height);
UINT uvc_parameter_frame_list(UX_HOST_CLASS_VIDEO *video);
VOID uvc_parameter_list(UX_HOST_CLASS_VIDEO *video, int format);
VOID uvc_process_function(UX_HOST_CLASS_VIDEO* video);
VOID uvc_frame_finish(UCHAR* p_frame);
int uvc_camera_spec_get(SPEC_OF_CAMERA** p_camera_spec, int* p_select);
void uvc_camera_spec_set(int select);
void uvc_camera_stop(void);
void uvc_camera_start(void);

/* Thread entry prototype and stack definition. */
void thread_0_entry(ULONG thread_input);
CHAR thread_0_stack[DEMO_STACK_SIZE];

extern void http_thread_entry(ULONG thread_input);
CHAR http_thread_stack[DEMO_STACK_SIZE];

extern void mjpeg_thread_entry(ULONG thread_input);
CHAR mjpeg_thread_stack[4*1024];

/* Define what the initial system looks like.  */
void tx_application_define(void *first_unused_memory)
{

UINT    status;

    /* Create the flags and the semaphore for video control.  */
    status = tx_event_flags_create (&g_device_insert_eventflag, (CHAR *)"Device Insert Event Flags");
    if(status!=TX_SUCCESS)
    {
        error_counter++;
    }

    status = tx_semaphore_create (&g_data_received_semaphore, (CHAR *) "Data Receive Semaphore", 0);
    if(status!=TX_SUCCESS)
    {
        error_counter++;
    }

    /* Create the main thread.  */
    status = tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            thread_0_stack, sizeof(thread_0_stack), 
            10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
    if(status!=TX_SUCCESS)
    {
        error_counter++;
    }

    /* Initialize FileX.  */
    fx_system_initialize();

    /* Initialize USBX memory.  */
    ux_system_initialize(g_ux_pool_memory, sizeof(g_ux_pool_memory),
            g_ux_uncache_pool_memory, sizeof(g_ux_uncache_pool_memory));

    /* Create the http thread.  */
    status = tx_thread_create (&http_thread, (CHAR *) "HTTP Thread", http_thread_entry, (ULONG)NULL,
                        &http_thread_stack, sizeof(http_thread_stack), 30, 30, 1, TX_AUTO_START);
    if(status!=TX_SUCCESS)
    {
        error_counter++;
    }
}

static UINT _host_change_function(ULONG event, UX_HOST_CLASS * class, VOID * instance)
{
    if (UX_SUCCESS
            == _ux_utility_memory_compare (_ux_system_host_class_video_name, class,
                                           _ux_utility_string_length_get(_ux_system_host_class_video_name)))
    {
        if (UX_DEVICE_INSERTION == event) /* Check if there is a device insertion. */
        {
            video_host_class = instance;

            /* Set the event flag to let application know the device insertion. */
            tx_event_flags_set (&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR);
        }
        else if(UX_DEVICE_REMOVAL == event)
        {
            /* Clear the event flag in case the camera was removed before the application could clear it. */
            tx_event_flags_set (&g_device_insert_eventflag, (ULONG)~EVENTFLAG_USB_DEVICE_INSERTED, TX_AND);

            video_host_class = NULL;
        }
    }

    return (UX_SUCCESS);
}

/* Video data received callback function. */ 
VOID uvc_transfer_request_done_callback(UX_TRANSFER * transfer_request)
{
    /* This is the callback function invoked by UVC class after a packet of 
       data is received. */

    /* The actual number of bytes being received into the data buffer is 
       recorded in transfer_request -> ux_transfer_request_actual_length. */ 

    /* Since this callback function executes in the USB host controller
       thread, a semaphore is released so the application can pick up the 
       video data in application thread. */

    frame_data[frame_index].length = transfer_request->ux_transfer_request_actual_length;

    frame_index++;
    if(MAX_NUM_BUFFERS<=frame_index)
    {
        frame_index = 0;
    }

    tx_semaphore_put(&g_data_received_semaphore); 
} 

/* Show the interval types */
VOID uvc_parameter_interval_list(UX_HOST_CLASS_VIDEO *video, int width, int height)
{
    UX_HOST_CLASS_VIDEO_FRAME_DESCRIPTOR frame_descriptor;

    ULONG min_frame_interval;
    ULONG max_frame_interval;
    ULONG frame_interval_step;
    int i;

    /* Make the descriptor machine independent.  */
    _ux_utility_descriptor_parse(video -> ux_host_class_video_current_frame_address,
                                 _ux_system_class_video_frame_descriptor_structure,
                                 UX_HOST_CLASS_VIDEO_FRAME_DESCRIPTOR_ENTRIES, (UCHAR *) &frame_descriptor);

    /* Check the frame interval type.  */
    if (frame_descriptor.bFrameIntervalType == 0)
    {
        if(camera_spec_count<(int)(sizeof(camera_spec)/sizeof(camera_spec[0])))
        {
            /* Frame interval type is continuous.  */
            min_frame_interval = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 26);
            max_frame_interval = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 30);
            frame_interval_step = _ux_utility_long_get(video -> ux_host_class_video_current_frame_address + 34);

            camera_spec[camera_spec_count].width = width;
            camera_spec[camera_spec_count].height = height;
            camera_spec[camera_spec_count].interval = (int)min_frame_interval;

            if(0<camera_spec_count)
            {
                /* select low-level spec */
                if(width<camera_spec[camera_spec_select].width)
                {
                    camera_spec_select = camera_spec_count;
                }
            }

            camera_spec_count++;
        }
    }
    else
    {
        /* Frame interval type is discrete.  */
        for(i = 0; i < (int)frame_descriptor.bFrameIntervalType; i++)
        {
            if(camera_spec_count<(int)(sizeof(camera_spec)/sizeof(camera_spec[0])))
            {
                camera_spec[camera_spec_count].width = width;
                camera_spec[camera_spec_count].height = height;
                camera_spec[camera_spec_count].interval = (int)_ux_utility_long_get(video -> ux_host_class_video_current_frame_address +
                                                            26 + (unsigned int)i * sizeof(ULONG));

                if(0<camera_spec_count)
                {
                    /* select low-level spec */
                    if(width<camera_spec[camera_spec_select].width)
                    {
                        camera_spec_select = camera_spec_count;
                    }
                }

                camera_spec_count++;
            }
            else
            {
                break;
            }
        }
    }
}

/* Show the frame resolutions */
UINT uvc_parameter_frame_list(UX_HOST_CLASS_VIDEO *video)
{
    ULONG frames_index;
    UX_HOST_CLASS_VIDEO_PARAMETER_FRAME_DATA frame_parameter;

    UINT status = UX_SUCCESS;

    /* frame resolutions */
    for (frames_index = 1; frames_index <= video -> ux_host_class_video_number_frames; frames_index++)
    {
        /* Get frame data for current frame index.  */
        frame_parameter.ux_host_class_video_parameter_frame_requested = frames_index;
        status = _ux_host_class_video_frame_data_get(video, &frame_parameter);
        if (status != UX_SUCCESS)
        {
            return(status);
        }

        /* Save the current frame index.  */
        video -> ux_host_class_video_current_frame = frames_index;

        uvc_parameter_interval_list(video,
            (int)frame_parameter.ux_host_class_video_parameter_frame_width,
            (int)frame_parameter.ux_host_class_video_parameter_frame_height);
    }

    return(status);
}


/* Show the device parameters */
VOID uvc_parameter_list(UX_HOST_CLASS_VIDEO *video, int format)
{
    ULONG format_index;
    UX_HOST_CLASS_VIDEO_PARAMETER_FORMAT_DATA format_parameter;

    UINT status = UX_SUCCESS;

    /* format types */
    for (format_index = 1; format_index <= video -> ux_host_class_video_number_formats; format_index++)
    {
        /* Get format data for current format index.  */
        format_parameter.ux_host_class_video_parameter_format_requested = format_index;
        status = _ux_host_class_video_format_data_get(video, &format_parameter);
        if (status == UX_SUCCESS)
        {
            if((int)format_parameter.ux_host_class_video_parameter_format_subtype==format)
            {
                /* Save number of frames in the video instance.  */
                video -> ux_host_class_video_number_frames = format_parameter.ux_host_class_video_parameter_number_frame_descriptors;

                uvc_parameter_frame_list(video);
                break;
            }
        }
    }
}

VOID uvc_process_function(UX_HOST_CLASS_VIDEO* video)
{
    UINT status;
    ULONG actual_flags;

    /* List parameters */
    camera_spec_count = 0;
    camera_spec_select = 0;
    uvc_parameter_list(video, UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG);

    /* Create the mjpeg thread.  */
    status = tx_thread_create (&mjpeg_thread, (CHAR *) "MJPEG Thread", mjpeg_thread_entry, (ULONG)NULL,
                        &mjpeg_thread_stack, sizeof(mjpeg_thread_stack), 22, 22, 0, TX_AUTO_START);
    if(status!=TX_SUCCESS)
    {
        error_counter++;
    }

    while (1)
    {
        /* Check camera status */
        status = tx_event_flags_get(&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR, (ULONG *)&actual_flags, 100);
        if(status!=TX_SUCCESS)
        {
            /* Stop video transfer.  */
            break;
        }

        if((actual_flags&EVENTFLAG_USB_DEVICE_INSERTED)==0)
        {
            /* camera removed.  */
            break;
        }

        tx_thread_sleep(10);
    }

    camera_spec_count = 0;
}

VOID uvc_frame_finish(UCHAR* p_frame)
{
    UINT old_threshold;

    tx_thread_preemption_change(tx_thread_identify(), 0, &old_threshold);

    if(video_host_class)
    {
        /* Add the buffer back for video transfer. */
        ux_host_class_video_transfer_buffer_add(video_host_class, p_frame); 
    }
    tx_thread_preemption_change(tx_thread_identify(), old_threshold, &old_threshold);
}

int uvc_camera_spec_get(SPEC_OF_CAMERA** p_camera_spec, int* p_select)
{
    if(p_camera_spec!=NULL)
    {
        *p_camera_spec = camera_spec;
    }
    if(p_select!=NULL)
    {
        *p_select = camera_spec_select;
    }

    return camera_spec_count;
}

void uvc_camera_spec_set(int select)
{
    if(select<camera_spec_count)
    {
        camera_spec_select = select;
    }
}

void uvc_camera_stop(void)
{
    if(video_host_class!=NULL)
    {
        /* Stop video transfer.  */
        ux_host_class_video_stop(video_host_class);
    }
}

void uvc_camera_start(void)
{
    UINT status;

    /* Maximum buffer requirement reported by the video device. */ 
    ULONG max_buffer_size; 

    UX_HOST_CLASS_VIDEO_PARAMETER_CHANNEL channel;

    /* Index variable keeping track of the current buffer being used by the video device. */ 
    ULONG buffer_index; 

    if(video_host_class!=NULL)
    {
        /* Set video parameters. This setting value is a dummy. 
           Depending on the application, set the necessary parameters. */
        if(camera_spec_select<camera_spec_count)
        {
            status = ux_host_class_video_frame_parameters_set(video_host_class,
                        UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG,
                        (ULONG)camera_spec[camera_spec_select].width, camera_spec[camera_spec_select].height,
                        (ULONG)camera_spec[camera_spec_select].interval);
        }
        else
        {
            status = ux_host_class_video_frame_parameters_set(video_host_class,
                        UX_HOST_CLASS_VIDEO_VS_FORMAT_MJPEG,
                        FIRST_SELECT_WIDTH, FIRST_SELECT_HEIGHT,
                        FIRST_SELECT_INTERVAL);
        }

        /* Set the user callback function of video class. */
        ux_host_class_video_transfer_callback_set(video_host_class, uvc_transfer_request_done_callback);

        /* Find out the maximum memory buffer size for the video configuration
           set above. */
        max_buffer_size = ux_host_class_video_max_payload_get(video_host_class);

        /* Clear semaphore to zero */
        while (1)
        {
            if(tx_semaphore_get(&g_data_received_semaphore, 0)==TX_NO_INSTANCE)
            {
                break;
            }
        }

        /* Start video transfer.  */
        status = ux_host_class_video_start(video_host_class);
        if(status!=UX_SUCCESS)
        {
            /* Setting these to zero is a hack since we're mixing old and new APIs (new API does this and is required for reads). */
            video_host_class -> ux_host_class_video_transfer_request_start_index = 0;
            video_host_class -> ux_host_class_video_transfer_request_end_index = 0;

            channel.ux_host_class_video_parameter_format_requested = video_host_class -> ux_host_class_video_current_format;
            channel.ux_host_class_video_parameter_frame_requested = video_host_class -> ux_host_class_video_current_frame;
            channel.ux_host_class_video_parameter_frame_interval_requested = video_host_class -> ux_host_class_video_current_frame_interval;
            channel.ux_host_class_video_parameter_channel_bandwidth_selection = 1024;

            status = ux_host_class_video_ioctl(video_host_class, UX_HOST_CLASS_VIDEO_IOCTL_CHANNEL_START, &channel);
        }

#if HIGH_BANDWIDTH_EHCI /* Driver HCD must support adding requests list.  */
        {
        UCHAR* video_buffers[MAX_NUM_BUFFERS];

        for(buffer_index = 0; buffer_index < MAX_NUM_BUFFERS; buffer_index++)
        {
            frame_data[buffer_index].p_frame = video_buffer[buffer_index];
            video_buffers[buffer_index] = frame_data[buffer_index].p_frame;
        }

        /* Issue transfer request list to start streaming.  */
        status = ux_host_class_video_transfer_buffers_add(video_host_class, video_buffers, MAX_NUM_BUFFERS);
        }
#elif NORMAL_BANDWIDTH_OHCI /* Driver adds request one by one.  */
        /* Allocate space for video buffer. */     
        for(buffer_index = 0; buffer_index < MAX_NUM_BUFFERS; buffer_index++)
        {
            frame_data[buffer_index].p_frame = video_buffer[buffer_index];

            /* Add buffer to the video device for video streaming data. */
            ux_host_class_video_transfer_buffer_add(video_host_class, frame_data[buffer_index].p_frame);
        }
#endif
    }

    frame_index = 0;
}

/* Define the test threads.  */
void    thread_0_entry(ULONG thread_input)
{
UINT            status;
    ULONG actual_flags;

    tx_thread_sleep(300);

    ux_rz_usbh1_initialize();   /* initialize USB1 */

    /* The code below is required for installing the host portion of USBX */
    status =  ux_host_stack_initialize(_host_change_function);
    if (status != UX_SUCCESS)
        error_counter++;

#if 1
    /* Register the hub class.  */
    status =  ux_host_stack_class_register(_ux_system_host_class_hub_name, _ux_host_class_hub_entry);
    if (status != UX_SUCCESS)
        error_counter++;
#endif

    /* Register the Video class.  */
    status =  ux_host_stack_class_register(_ux_system_host_class_video_name, ux_host_class_video_entry);

    if(status != UX_SUCCESS)
        error_counter++;

    /* Register all the USB host controllers available in this system */
    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_ehci_name, ux_hcd_ehci_initialize, RZA2M_USB_EHCI_HOST1, 0x0);
    if (status != UX_SUCCESS)
        while(1);

    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_ohci_name, ux_hcd_ohci_initialize, RZA2M_USB_OHCI_HOST1, 0x0);
    if (status != UX_SUCCESS)
        while(1);

    ux_rz_usbh1_start();

    while(1)
    {
        /* Suspend here until a USBX Host Class Instance gets ready. */
        tx_event_flags_get(&g_device_insert_eventflag, EVENTFLAG_USB_DEVICE_INSERTED, TX_OR, (ULONG *)&actual_flags, TX_WAIT_FOREVER);

        /* This delay is required for now to get valid ISO IN UX_ENDPOINT instance. */
        tx_thread_sleep(100);

        if(video_host_class!=NULL)
        {
            uvc_process_function(video_host_class);
        }
    }
}

