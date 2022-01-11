/* This is a small demo of the high-performance USBX Host HID.  */

#include <stdio.h>

#include "tx_api.h"
#include "ux_api.h"

#include "ux_host_class_hub.h"
#include "ux_host_stack.h"
#include "ux_system.h"
#include "ux_host_class_hid.h"
#include "ux_host_class_hid_mouse.h"

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

#define MOUSE_DEVICE    (2)

/* Define the ThreadX and NetX object control blocks...  */

TX_THREAD               thread_0;

static int_t gs_my_gpio_handle;

ULONG error_counter = 0;
ULONG mouse_event_count = 0;
UX_HOST_CLASS_HID* g_mouse = UX_NULL;

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
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

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
        /* Check mouse device.  */
        status =  ux_host_stack_class_get(_ux_system_host_class_hid_name, &host_class);
        if (status != UX_SUCCESS)
        {
            return(status);
        }

        if(((UX_HOST_CLASS_HID *)instance)->ux_host_class_hid_interface->ux_interface_descriptor.bInterfaceProtocol == MOUSE_DEVICE)
        {
            g_mouse = (UX_HOST_CLASS_HID*)instance;

            printf("    Connected Mouse Device.\r\n");
        }
    }
    /* Check if some device is removed.  */
    else if(event == UX_DEVICE_REMOVAL)
    {
        /* Check if we got an HID class device.  */
        if (g_mouse==instance)
        {
            g_mouse = NULL;

            printf("    Removed Mouse Device.\r\n");
        }
    }

    return(UX_SUCCESS);
}


/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{
    UINT status;
    UX_HOST_CLASS_HID_CLIENT* hid_client;
    UINT rtn;

    ULONG mouse_buttons;
    SLONG mouse_x;
    SLONG mouse_y;

    tx_thread_sleep(300);
    printf("\r\nRZ/A2M USBX Host HID Demonstration Program.\r\n\r\n");

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

    /* Register the HID class.  */
    status =  ux_host_stack_class_register(_ux_system_host_class_hid_name, _ux_host_class_hid_entry);

    if(status != UX_SUCCESS)
        error_counter++;

    /* Register the HID Mouse client. */
    status = ux_host_class_hid_client_register (
            _ux_system_host_class_hid_client_mouse_name, ux_host_class_hid_mouse_entry);

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
        if(g_mouse!=UX_NULL)
        {
            hid_client = g_mouse->ux_host_class_hid_client;

            rtn = ux_host_class_hid_mouse_buttons_get(
                            (UX_HOST_CLASS_HID_MOUSE*)(hid_client->ux_host_class_hid_client_local_instance),
                            &mouse_buttons);
            if(rtn==UX_SUCCESS)
            {
                rtn = ux_host_class_hid_mouse_position_get (
                            (UX_HOST_CLASS_HID_MOUSE*)(hid_client->ux_host_class_hid_client_local_instance),
                            &mouse_x, &mouse_y);

                if(rtn==UX_SUCCESS)
                {
                    /* mouse event received */
                    mouse_event_count++;

                    printf("    Mouse Position<%d/%d>\r\n", mouse_x, mouse_y);
                }
            }

            tx_thread_sleep(50);
        }
        else
        {
            tx_thread_sleep(100);
        }
    }
}

