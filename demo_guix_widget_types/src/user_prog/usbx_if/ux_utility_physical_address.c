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
#define USB_MMU_PHYSICAL_PAGE_0  (0x80000000)  //physical address corresponding to 0x82000000
#define USB_MMU_PHYSICAL_PAGE_1  (0x80100000)  //physical address corresponding to 0x82100000
#define USB_MMU_PHYSICAL_PAGE_2  (0x80200000)  //physical address corresponding to 0x82200000
#define USB_MMU_PHYSICAL_PAGE_3  (0x80300000)  //physical address corresponding to 0x82300000


#define USB_MMU_MASK_TOP         (0xFFF00000)
#define USB_MMU_MASK_BOTTOM      (0x000FFFFF)

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_utility_physical_address                        PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This function returns a physical address given the supplied         */ 
/*    virtual address.                                                    */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    virtual_address                       Physical address              */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    physical_address                      Virtual address               */ 
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
VOID  *_ux_utility_physical_address(VOID *virtual_address)
{

VOID    *physical_address;

    /* Any code to translate the virtual address into a physical address
       will be below. If there is no translation, the physical address=
       the virtual address.  */

//    physical_address =  virtual_address;

    /* Address translate for GR-MANGO. */
    switch(((ULONG)virtual_address&USB_MMU_MASK_TOP))
    {
    case 0x82000000:
        physical_address = (VOID*)((USB_MMU_PHYSICAL_PAGE_0 & USB_MMU_MASK_TOP) + ((ULONG)virtual_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x82100000:
        physical_address = (VOID*)((USB_MMU_PHYSICAL_PAGE_1 & USB_MMU_MASK_TOP) + ((ULONG)virtual_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x82200000:
        physical_address = (VOID*)((USB_MMU_PHYSICAL_PAGE_2 & USB_MMU_MASK_TOP) + ((ULONG)virtual_address & USB_MMU_MASK_BOTTOM));
        break;
    case 0x82300000:
        physical_address = (VOID*)((USB_MMU_PHYSICAL_PAGE_3 & USB_MMU_MASK_TOP) + ((ULONG)virtual_address & USB_MMU_MASK_BOTTOM));
        break;
    default:
        physical_address = virtual_address;
    }

    return(physical_address);
}

