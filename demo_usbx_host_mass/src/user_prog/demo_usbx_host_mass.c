/* This is a small demo of the high-performance USBX Host HID.  */

#include <stdio.h>

#include "tx_api.h"
#include "ux_api.h"

#include "fx_api.h"

#include "ux_host_class_hub.h"
#include "ux_host_class_storage.h"
#include "ux_host_stack.h"
#include "ux_system.h"

#include "ux_rz_driver.h"

#include "r_typedefs.h"
#include "driver.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


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

/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD               thread_0;
UX_HOST_CLASS_STORAGE           *storage;
UX_HOST_CLASS_STORAGE_MEDIA     *storage_media;
FX_MEDIA                        *media;

FX_FILE                         my_file;
CHAR                            file_name[64];
UCHAR                           local_buffer[UX_DEMO_BUFFER_SIZE];

static int_t gs_my_gpio_handle;

ULONG error_counter = 0;


/* Static memory pool allocation used by USBX system. */
CHAR g_ux_pool_memory[64*1024];
CHAR g_ux_uncache_pool_memory[48*1024] __attribute__ ((section ("USB_SMPL_0")));

/* Define prototypes.  */
static UINT _host_change_function(ULONG event, UX_HOST_CLASS * class, VOID * instance);

void    thread_0_entry(ULONG thread_input);


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer;
UINT    status;

    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

    /* Initialize FileX.  */
    fx_system_initialize();

    /* Initialize USBX memory.  */
    ux_system_initialize(g_ux_pool_memory, sizeof(g_ux_pool_memory),
            g_ux_uncache_pool_memory, sizeof(g_ux_uncache_pool_memory));
}

static UINT _host_change_function(ULONG event, UX_HOST_CLASS * class, VOID * instance)
{
UINT            status;
UX_HOST_CLASS   *host_class;

    /* Check if there is a device insertion.  */
    if (event == UX_DEVICE_INSERTION)
    {
        /* Get the storage class.  */
        status =  ux_host_stack_class_get(_ux_system_host_class_storage_name, &host_class);
        if (status != UX_SUCCESS)
            return(status);

        /* Check if we got a storage class device.  */
        if (host_class == class)
        {
            /* We only get the first media attached to the class container.  */
            if (media == UX_NULL)
            {
                storage = instance;
                storage_media = class -> ux_host_class_media;
                media = &storage_media -> ux_host_class_storage_media;

                printf("    Connected Mass Storage Device.\r\n");
            }
        }
    }
    /* Check if some device is removed.  */
    else if(event == UX_DEVICE_REMOVAL)
    {
        /*  Check if the storage device is removed.  */
        if (instance == storage)
        {
            /* Set pointers to null, so that the demo thread will not try to access the media any more.  */
            media = UX_NULL;
            storage_media = UX_NULL;
            storage = UX_NULL;

            printf("    Removed Mass Storage Device.\r\n");
        }
    }

    return(UX_SUCCESS);
}


/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{
UINT            status;
volatile ULONG       files_count = 0;
FX_FILE     my_file;
ULONG       requested_length;
UINT        file_attribute;

    tx_thread_sleep(300);
    printf("\r\nRZ/A2M USBX Host Mass Storage Demonstration Program.\r\n\r\n");

    ux_rz_usbh0_initialize();

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

    /* Register the MSC class.  */
    status =  ux_host_stack_class_register(_ux_system_host_class_storage_name, _ux_host_class_storage_entry);

    if(status != UX_SUCCESS)
        error_counter++;

    /* Register all the USB host controllers available in this system */
    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_ehci_name, ux_hcd_ehci_initialize, RZA2M_USB_EHCI_HOST0, 0x0);
    if (status != UX_SUCCESS)
        while(1);

    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_ohci_name, ux_hcd_ohci_initialize, RZA2M_USB_OHCI_HOST0, 0x0);
    if (status != UX_SUCCESS)
        while(1);

    ux_rz_usbh0_start();

    while(1)
    {
        /* Check if we have an available media instance.  */
        while (media == UX_NULL)
        {
            tx_thread_sleep(10);
        }

        /* Find first file on media.  */
        status =  fx_directory_first_entry_find(media, file_name);
        if (status != UX_SUCCESS)
        {
            tx_thread_sleep(20);
            continue;
        }

        /* We get here if there is at least a file in the directory.  */
        do
        {
            /* Loop through the files in the root directory.  */

            printf("    File name:<%s>.\r\n", file_name);

            /* Increment file count.  */
            files_count++;

            /* Reset file attributes.  */
            file_attribute = 0;

            /* Try to read the file attributes.  */
            status =  fx_file_attributes_read(media, file_name, &file_attribute);

            /* If this is a directory, pass.  */
            if(!(file_attribute & 0x18) && (status == UX_SUCCESS))
            {

                /* A file is present, attempt to read its contents.  */

                /* Try to open the file.  */
                status =  fx_file_open(media, &my_file, file_name, FX_OPEN_FOR_READ);
                if (status != UX_SUCCESS)
                    break;

                while(status == UX_SUCCESS)
                {

                    /* Read the file in blocks */
                    status = fx_file_read(&my_file, local_buffer, UX_DEMO_BUFFER_SIZE, &requested_length);

                    /* Check if status OK.  */
                    if (status != UX_SUCCESS)
                    {
                        error_counter++;
                        break;
                    }

                    /* Check if end of file.  */
                    if (requested_length != UX_DEMO_BUFFER_SIZE)
                        break;

                }

                /* Finished reading file either at the end or because of error. */
                fx_file_close(&my_file);
            }

            /* Find the next file.  */
            status =  fx_directory_next_entry_find(media, file_name);

        } while (status == UX_SUCCESS);

        printf("    Total file count:%d\r\n\r\n", files_count);
        files_count = 0;

        tx_thread_sleep(20);
    }
}

