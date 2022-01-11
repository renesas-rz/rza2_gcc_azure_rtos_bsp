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

#include "gx_api.h"
#include "demo_guix_home_automation_resources.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


extern uint32_t usbh0_hstd_ohci_physical_address_of(void *data);

static GX_EVENT mouse_event;
extern GX_WINDOW_ROOT    *root;

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

static int_t gs_my_gpio_handle;

ULONG error_counter = 0;
ULONG mouse_event_count = 0;
UX_HOST_CLASS_HID* g_mouse = UX_NULL;

/* Static memory pool allocation used by USBX system. */
CHAR g_ux_pool_memory[64*1024];
CHAR g_ux_uncache_pool_memory[48*1024] __attribute__ ((section ("USB_SMPL_0")));

/* Define prototypes.  */
static UINT _host_change_function(ULONG event, UX_HOST_CLASS * class, VOID * instance);


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
        }
    }
    /* Check if some device is removed.  */
    else if(event == UX_DEVICE_REMOVAL)
    {
        /* Check if we got an HID class device.  */
        if (g_mouse==instance)
        {
            g_mouse = NULL;
        }
    }

    return(UX_SUCCESS);
}


/* Define the mouse threads.  */
void mouse_thread_entry(ULONG thread_input)
{
    UINT status;
    UX_HOST_CLASS_HID_CLIENT* hid_client;
    UINT rtn;

    ULONG mouse_buttons;
    SLONG mouse_x;
    SLONG mouse_y;

    ULONG mouse_left = 0;
    ULONG mouse_right = 0;
    ULONG mouse_middle = 0;

    ULONG current_type;
    SLONG org_x;
    SLONG org_y;
    SLONG offset_x;
    SLONG offset_y;
    SLONG current_x;
    SLONG current_y;

    int_t gpio_handle;

    org_x = 0;
    org_y = 0;

    mouse_event.gx_event_type = GX_EVENT_REDRAW;

    mouse_event.gx_event_display_handle = 0;
    mouse_event.gx_event_target = 0;
    mouse_event.gx_event_sender = GX_ID_NONE;
    mouse_event.gx_event_payload.gx_event_pointdata.gx_point_x = DISPLAY_1_X_RESOLUTION/2;
    mouse_event.gx_event_payload.gx_event_pointdata.gx_point_y = DISPLAY_1_Y_RESOLUTION/2;

    /* For information only
     * Use stdio calls to open drivers once  the kernel is initialised
     *
     * ie
     * int_t ostm3_handle;
     * ostm3_handle = open (DEVICE_INDENTIFIER "ostm2", O_RDWR);
     * close (ostm3_handle);
     */

    gs_my_gpio_handle = open (DEVICE_INDENTIFIER "gpio", O_RDWR);

    /* On error */
    if ( gs_my_gpio_handle < 0 )
    {
        /* stop execute */
        while(1)
        {
            /* Spin here forever.. */
        }
    }

    /* Initialize USBX memory.  */
    ux_system_initialize(g_ux_pool_memory, sizeof(g_ux_pool_memory),
            usbh0_hstd_ohci_physical_address_of(g_ux_uncache_pool_memory), sizeof(g_ux_uncache_pool_memory));

    ux_rz_usbh_initialize();

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

                    if(mouse_event.gx_event_type==GX_EVENT_REDRAW)
                    {
                        /* show mouse cursor */
                        gx_canvas_mouse_show(root->gx_window_root_canvas);

                        org_x = 0;
                        org_y = 0;
                        mouse_event.gx_event_type = GX_EVENT_PEN_MOVE;
                    }

                    mouse_left =  mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_1_PRESSED;
                    mouse_right = mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_2_PRESSED;
                    mouse_middle = mouse_buttons & UX_HOST_CLASS_HID_MOUSE_BUTTON_3_PRESSED;

                    /* Adjustment of scale and direction */
#if 0
                    mouse_x /= 2;
                    mouse_y /= 2;
#endif
#if 1
                    mouse_x = -mouse_x;
                    mouse_y = -mouse_y;
#endif

                    /* Calculate the next point and adjust the range */
                    offset_x = org_x - mouse_x;
                    offset_y = org_y - mouse_y;

                    current_x = mouse_event.gx_event_payload.gx_event_pointdata.gx_point_x + offset_x;
                    current_y = mouse_event.gx_event_payload.gx_event_pointdata.gx_point_y + offset_y;
                    if(current_x<0)
                    {
                        current_x = 0;
                    }
                    else if(DISPLAY_1_X_RESOLUTION<current_x)
                    {
                        current_x = DISPLAY_1_X_RESOLUTION;
                    }
                    if(current_y<0)
                    {
                        current_y = 0;
                    }
                    else if(DISPLAY_1_Y_RESOLUTION<current_y)
                    {
                        current_y = DISPLAY_1_Y_RESOLUTION;
                    }

                    /* Create type for the next point */
                    current_type = GX_EVENT_PEN_MOVE;
                    if(mouse_left!=0)
                    {
                        if(mouse_event.gx_event_type==GX_EVENT_PEN_MOVE)
                        {
                            current_type = GX_EVENT_PEN_DOWN;
                        }
                        else
                        {
                            current_type = GX_EVENT_PEN_DRAG;
                        }
                    }
                    else
                    {
                        if((mouse_event.gx_event_type==GX_EVENT_PEN_DOWN) ||
                            (mouse_event.gx_event_type==GX_EVENT_PEN_DRAG))
                        {
                            current_type = GX_EVENT_PEN_UP;
                        }
                    }

                    if((mouse_event.gx_event_payload.gx_event_pointdata.gx_point_x!=current_x) ||
                        (mouse_event.gx_event_payload.gx_event_pointdata.gx_point_y!=current_y) ||
                        (mouse_event.gx_event_type!=current_type))
                    {
                        /* If the status has changed, send a event */
                        mouse_event.gx_event_type = current_type;
                        mouse_event.gx_event_payload.gx_event_pointdata.gx_point_x = current_x;
                        mouse_event.gx_event_payload.gx_event_pointdata.gx_point_y = current_y;

                        gx_system_event_send(&mouse_event);
                    }

                    /* Save the current point for the next point calculation */
                    org_x = mouse_x;
                    org_y = mouse_y;
                }
            }

            tx_thread_sleep(5);
        }
        else
        {
            if(mouse_event.gx_event_type!=GX_EVENT_REDRAW)
            {
                /* hide mouse cursor */
                gx_canvas_mouse_hide(root->gx_window_root_canvas);

                mouse_event.gx_event_type = GX_EVENT_REDRAW;
                gx_system_event_send(&mouse_event);
            }

            tx_thread_sleep(100);
        }
    }
}

