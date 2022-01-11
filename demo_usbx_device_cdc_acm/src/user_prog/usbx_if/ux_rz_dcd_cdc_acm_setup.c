/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Device CDC/ACM Initialize for Renesas RZ                         */ 
/**                                                                       */
/** This initialization function is for USBX Device CDC/ACM operation on  */ 
/** the Renesas RZ processor.                                             */
/**                                                                       */
/** This template assumes basic operation, any special configuration will */ 
/** require user modification or replacement of this file.                */ 
/**                                                                       */
/** For this demo to work, the CDC_ACM_Template.inf file needs to be used.*/
/** This file is stored in the usbx_windows_host_files directory.         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_device_class_cdc_acm.h"


/* Define local constants.  */

#define DEVICE_FRAMEWORK_LENGTH_FULL_SPEED      93 
#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED      103
#define STRING_FRAMEWORK_LENGTH                 47
#define LANGUAGE_ID_FRAMEWORK_LENGTH            2


/* Define local prototypes and definitions.  */

unsigned char device_framework_full_speed[] = {

    /* Device descriptor     18 bytes
       0x02 bDeviceClass:    CDC class code
       0x00 bDeviceSubclass: CDC class sub code
       0x00 bDeviceProtocol: CDC Device protocol
    
       idVendor & idProduct - http://www.linux-usb.org/usb.ids
    */
    0x12, 0x01, 0x10, 0x01,     
    0xEF, 0x02, 0x01,           
    0x08,                       
//    0x72, 0x05, 0x20, 0x13,     
    0x5B, 0x04, 0x00, 0x53,     

//    0x00, 0x01,                 
    0x00, 0x00,                 

//    0x01, 0x02, 03,             
    0x00, 0x00, 0x00,             
    0x01,                       

    /* Configuration 1 descriptor 9 bytes */
    0x09, 0x02, 0x4b, 0x00,     
    0x02, 0x01, 0x00,           
    0x40, 0x00,                 

    /* Interface association descriptor. 8 bytes.  */
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00,

    /* Communication Class Interface Descriptor Requirement. 9 bytes.   */
    0x09, 0x04, 0x00,           
    0x00,                       
    0x01,                       
    0x02, 0x02, 0x01,           
    0x00,                       

    /* Header Functional Descriptor 5 bytes */
    0x05, 0x24, 0x00,           
    0x10, 0x01,                 

    /* ACM Functional Descriptor 4 bytes */
    0x04, 0x24, 0x02,           
    0x0f,                       

    /* Union Functional Descriptor 5 bytes */
    0x05, 0x24, 0x06,           
    0x00,                          /* Master interface */
    0x01,                          /* Slave interface  */

    /* Call Management Functional Descriptor 5 bytes */
    0x05, 0x24, 0x01,           
    0x03,                       
    0x01,                          /* Data interface   */

    /* Endpoint 1 descriptor 7 bytes */
    0x07, 0x05, 0x86,           
    0x03,                       
    0x08, 0x00,                 
    0xFF,                       

    /* Data Class Interface Descriptor Requirement 9 bytes */
    0x09, 0x04, 0x01,           
    0x00,                       
    0x02,                       
    0x0A, 0x00, 0x00,           
    0x00,                       

    /* First alternate setting Endpoint 1 descriptor 7 bytes*/
    0x07, 0x05, 0x01,           
    0x02,                       
    0x40, 0x00,                 
    0x00,                       

    /* Endpoint 2 descriptor 7 bytes */
    0x07, 0x05, 0x82,           
    0x02,                       
    0x40, 0x00,                 
    0x00,                       

};

unsigned char device_framework_high_speed[] = {

    /* Device descriptor    
       0x02 bDeviceClass:    CDC class code
       0x00 bDeviceSubclass: CDC class sub code
       0x00 bDeviceProtocol: CDC Device protocol
    
       idVendor & idProduct - http://www.linux-usb.org/usb.ids
    */
    0x12, 0x01, 0x00, 0x02,     
    0xEF, 0x02, 0x01,           
    0x40,                       
//    0x84, 0x84, 0x00, 0x00,     
//    0x72, 0x05, 0x20, 0x13,     
    0x5B, 0x04, 0x00, 0x53,     
//    0x00, 0x01,                 
    0x00, 0x00,                 
//    0x01, 0x02, 03,             
    0x00, 0x00, 0x00,             
    0x01,                       

    /* Device qualifier descriptor */
    0x0a, 0x06, 0x00, 0x02,     
    0x02, 0x00, 0x00,           
    0x40,                       
    0x01,                       
    0x00,                       

    /* Configuration 1 descriptor */
    0x09, 0x02, 0x4b, 0x00,     
    0x02, 0x01, 0x00,           
    0x40, 0x00,                 

    /* Interface association descriptor. */
    0x08, 0x0b, 0x00, 0x02, 0x02, 0x02, 0x00, 0x00,

    /* Communication Class Interface Descriptor Requirement */
    0x09, 0x04, 0x00,           
    0x00,                       
    0x01,                       
    0x02, 0x02, 0x01,           
    0x00,                       

    /* Header Functional Descriptor */
    0x05, 0x24, 0x00,           
    0x10, 0x01,                 

    /* ACM Functional Descriptor */
    0x04, 0x24, 0x02,           
    0x0f,                       

    /* Union Functional Descriptor */
    0x05, 0x24, 0x06,           
    0x00,                       
    0x01,                       

    /* Call Management Functional Descriptor */
    0x05, 0x24, 0x01,           
    0x00,                       
    0x01,                       

    /* Endpoint 1 descriptor */
    0x07, 0x05, 0x86,           
    0x03,                       
    0x08, 0x00,                 
    0x80,                       

    /* Data Class Interface Descriptor Requirement */
    0x09, 0x04, 0x01,           
    0x00,                       
    0x02,                       
    0x0A, 0x00, 0x00,           
    0x00,                       

    /* First alternate setting Endpoint 1 descriptor */
    0x07, 0x05, 0x01,           
    0x02,                       
    0x40, 0x00,                 
    0x00,                       

    /* Endpoint 2 descriptor */
    0x07, 0x05, 0x82,           
    0x02,                       
    0x40, 0x00,                 
    0x00,                        

};

unsigned char string_framework[] = {

    /* Manufacturer string descriptor : Index 1 - "Express Logic" */
        0x09, 0x04, 0x01, 0x0c, 
        0x45, 0x78, 0x70, 0x72, 0x65, 0x73, 0x20, 0x4c, 
        0x6f, 0x67, 0x69, 0x63,

    /* Product string descriptor : Index 2 - "EL Composite device" */
        0x09, 0x04, 0x02, 0x13,
        0x45, 0x4c, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x6f,
        0x73, 0x69, 0x74, 0x65, 0x20, 0x64, 0x65, 0x76, 
        0x69, 0x63, 0x65,                              

    /* Serial Number string descriptor : Index 3 - "0001" */
        0x09, 0x04, 0x03, 0x04,
        0x30, 0x30, 0x30, 0x31
    };


    /* Multiple languages are supported on the device, to add
       a language besides english, the unicode language code must
       be appended to the language_id_framework array and the length
       adjusted accordingly. */
unsigned char language_id_framework[] = {

    /* English. */
        0x09, 0x04
    };


/* Define the memory area needed for USBX Device CDC/ACM.  */

UCHAR                                   ux_rz_dcd_memory_area[128 * 1024];     /* FIXME: This should be tuned based on memory used during testing.  */


UX_SLAVE_CLASS_CDC_ACM_PARAMETER        parameter;


/* Define the basic USBX Device CDC/ACM initialization.  Must be called from a thread context.  */

UINT    ux_rz_dcd_cdc_acm_setup(UINT (*dcd_initialize)(ULONG dcd_io),
                                VOID (*cdc_instance_activate)(VOID *cdc_instance),
                                VOID (*cdc_instance_deactivate)(VOID *cdc_instance))
{

UINT    status;
    

    /* Initialize USBX. */
    ux_system_initialize(ux_rz_dcd_memory_area, sizeof(ux_rz_dcd_memory_area), UX_NULL, 0);
    
    /* Initialize USBX Device for CDC/ACM operation. */
    status =  ux_device_stack_initialize(device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
                                       device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
                                       string_framework, STRING_FRAMEWORK_LENGTH,
                                       language_id_framework, LANGUAGE_ID_FRAMEWORK_LENGTH,UX_NULL);
    
    /* Check completion status.  */
    if (status != UX_SUCCESS)
        return(status);

    /* Set the parameters for callback when insertion/extraction of a CDC device.  */
    parameter.ux_slave_class_cdc_acm_instance_activate   =  cdc_instance_activate;
    parameter.ux_slave_class_cdc_acm_instance_deactivate =  cdc_instance_deactivate;

    /* Initialize the device cdc class. This class owns both interfaces starting with 1. */
    status =  ux_device_stack_class_register(_ux_system_slave_class_cdc_acm_name, ux_device_class_cdc_acm_entry, 
                                                1,0,  &parameter);

    /* Check completion status.  */
    if (status != UX_SUCCESS)
        return(status);

    /* Initialize/start the USBX device driver.  */
    status =  (dcd_initialize)(0x0);
    
    /* Check status.  */
    if (status != UX_SUCCESS)
        while(1);

    /* Return the completion status.  */
    return(status);
}

