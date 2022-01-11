/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Example                                                          */
/**                                                                       */
/** This example illustrates USBX working on a Renesas RZ processor for   */
/** the GR-MANGO board.                                                   */
/**                                                                       */
/** This demo will show the device side only of USBX. It will make a      */
/** USB serial acm device visible on a host.                              */
/**                                                                       */
/** For this demo to work, the CDC_ACM_Template.inf file needs to be used.*/
/** This file is stored in the usbx_windows_host_files directory.         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */
#include <stdio.h>

#include "ux_api.h"
#include "ux_dcd_rz.h"
#include "ux_device_class_cdc_acm.h"

#include "ux_rz_dcd_hwsetup.h"

/* Define local constants.  */

#define UX_DEMO_STACK_SIZE      4096
#define UX_DEMO_BUFFER_SIZE     2048

/* Support USB High-speed */
//#define UX_RZ_DCD_SPEED_HS    1
#define UX_RZ_DCD_SPEED_HS  0

/* Define local prototypes and definitions.  */

VOID  demo_thread_entry(ULONG);
VOID  demo_cdc_instance_activate(VOID  *cdc_instance);             
VOID  demo_cdc_instance_deactivate(VOID *cdc_instance);


/* Define local prototypes and definitions.  */

UINT  ux_rz_dcd_cdc_acm_setup(UINT (*dcd_initialize)(ULONG dcd_io),
                                VOID (*cdc_instance_activate)(VOID *cdc_instance),
                                VOID (*cdc_instance_deactivate)(VOID *cdc_instance));


/* Define global variables.  */

UX_SLAVE_CLASS_CDC_ACM              *cdc = UX_NULL;
TX_THREAD                           demo_thread;
UCHAR                               buffer[UX_DEMO_BUFFER_SIZE];


/* Define what the initial system looks like.  */

void  tx_application_define(void *first_unused_memory)
{
UCHAR  *stack_pointer;

    /* Initialize the free memory pointer.  */
    stack_pointer =  (UCHAR *) first_unused_memory;

    /* Create the main demo thread.  */
    tx_thread_create(&demo_thread, "usbx device cdc-acm demo", demo_thread_entry, 0,  
            stack_pointer,UX_DEMO_STACK_SIZE, 
            20, 20, 1, TX_AUTO_START);
}


void  demo_thread_entry(ULONG arg)
{
UINT    status;
ULONG   actual_length;
ULONG   requested_length;

    tx_thread_sleep(300);
    printf("\r\nRZ/A2M USBX Device CDC ACM Demonstration Program.\r\n\r\n");

    /* Setup USB device controller of RZ/A2M(GR-MANGO). */
    uz_rz_dcd_pin_setting(); /* USB MCU pin setting */
    uz_rz_dcd_open(UX_RZ_DCD_SPEED_HS);

    /* Setup USBX Device for CDC/ACM operation on the RZ.  */
    status =  ux_rz_dcd_cdc_acm_setup(ux_dcd_rz_initialize, demo_cdc_instance_activate, demo_cdc_instance_deactivate);

    if (status)
        return;

    while(1)
    {
        /* Ensure the CDC class is mounted.  */
        if(cdc != UX_NULL)
        {
            /* Read from the CDC class.  */
            status = ux_device_class_cdc_acm_read(cdc, buffer, UX_DEMO_BUFFER_SIZE, &actual_length);

            /* The actual length becomes the requested length.  */
            requested_length = actual_length;

            /* Check the status.  If OK, we will write to the CDC instance.  */
            status = ux_device_class_cdc_acm_write(cdc, buffer, requested_length, &actual_length);

            /* Check for CR/LF.  */            
            if (buffer[requested_length - 1] == '\r') 
            {
                /* Copy LF value into user buffer.  */    
                ux_utility_memory_copy(buffer, "\n",  1);

                /* And send it again.  */
                status = ux_device_class_cdc_acm_write(cdc, buffer, 1, &actual_length);

                printf("    Send new-line character.\r\n");
            }

            /* And send 0 byte packet. Forced ZLP.  */
            status = ux_device_class_cdc_acm_write(cdc, buffer, 0, &actual_length);
        }
        else
        {
            tx_thread_sleep(50);
        }
    }
}


VOID  demo_cdc_instance_activate(VOID *cdc_instance)   
{
    /* Save the CDC instance.  */
    cdc = (UX_SLAVE_CLASS_CDC_ACM *) cdc_instance;

    printf("    Connected.\r\n");
}

VOID  demo_cdc_instance_deactivate(VOID *cdc_instance)
{

    /* Reset the CDC instance.  */
    cdc = UX_NULL;

    printf("    Disconnected.\r\n");
}

