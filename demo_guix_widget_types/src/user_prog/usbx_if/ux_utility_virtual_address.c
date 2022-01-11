/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Utility                                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"

/***********************************************************************************************************************
Constant macro definitions
***********************************************************************************************************************/
#define USB_MMU_VIRTUAL_PAGE_0  (0x82000000)  //Virtual address corresponding to 0x80000000
#define USB_MMU_VIRTUAL_PAGE_1  (0x82100000)  //Virtual address corresponding to 0x80100000
#define USB_MMU_VIRTUAL_PAGE_2  (0x82200000)  //Virtual address corresponding to 0x80200000
#define USB_MMU_VIRTUAL_PAGE_3  (0x82300000)  //Virtual address corresponding to 0x80300000


#define USB_MMU_MASK_TOP         (0xFFF00000)
#define USB_MMU_MASK_BOTTOM      (0x000FFFFF)

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_utility_virtual_address                         PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function returns a virtual address given the supplied          */ 
/*    physical address.                                                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    physical_address                      Virtual address               */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    virtual_address                       Physical address              */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    USBX Components                                                     */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
VOID  *_ux_utility_virtual_address(VOID *physical_address)
{

VOID    *virtual_address;

    /* Any code to translate the physical address into a virtual address
       will be below. If there is no translation, the physical address=
       the virtual address.  */

//    virtual_address =  physical_address;   

    /* Address translate for GR-MANGO. */
    switch(((ULONG)physical_address&USB_MMU_MASK_TOP))
    {
    case 0x80000000:
        virtual_address = (VOID*)((USB_MMU_VIRTUAL_PAGE_0 & USB_MMU_MASK_TOP) + ((ULONG)physical_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x80100000:
        virtual_address = (VOID*)((USB_MMU_VIRTUAL_PAGE_1 & USB_MMU_MASK_TOP) + ((ULONG)physical_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x80200000:
        virtual_address = (VOID*)((USB_MMU_VIRTUAL_PAGE_2 & USB_MMU_MASK_TOP) + ((ULONG)physical_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x80300000:
        virtual_address = (VOID*)((USB_MMU_VIRTUAL_PAGE_3 & USB_MMU_MASK_TOP) + ((ULONG)physical_address & USB_MMU_MASK_BOTTOM));
        break;
    default:
        virtual_address = physical_address;
    }

    return(virtual_address);
}

