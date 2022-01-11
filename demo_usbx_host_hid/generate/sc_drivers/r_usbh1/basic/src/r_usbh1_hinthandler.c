/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh1_hinthandler.c
 * Version      : 1.0
 * Description  : This module solves all the world's problems
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2007 1.00     First Release
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include <stdint.h>
#include "r_usbh1_basic_if.h"
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */
#include "iodefine.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define USB10_WAKEON_INT                0x00000010  /* Interrupt */
#define USB10_UCOM_INT                  0x00000008  /* Interrupt */
#define USB10_USBH_INTB                 0x00000004  /* EHCI interrupt */
#define USB10_USBH_INTA                 0x00000002  /* OHCI interrupt */
#define USB10_AHB_INT                   0x00000001  /* Bus master error interrupt */

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables (to be accessed by other files)
 **********************************************************************************************************************/
extern void     usbh1_hstd_hci_interrupt_handler(void);

/***********************************************************************************************************************
 Private global variables and functions
 **********************************************************************************************************************/
void        R_USBH1_HstdIntInit(void);
void        R_USBH1_isr(uint32_t int_sense);

/***********************************************************************************************************************
 * Function Name: usbh1_hstd_int_enable
 * Description  : USB host interrupt enable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usbh1_hstd_int_enable(void)
{
    R_INTC_Enable(INTC_ID_USB_USBHI1);
} /* End of function usbh1_hstd_int_enable() */

/***********************************************************************************************************************
 * Function Name: usbh1_hstd_int_disable
 * Description  : USB host interrupt disable
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void usbh1_hstd_int_disable(void)
{
    R_INTC_Disable(INTC_ID_USB_USBHI1);
} /* End of function usbh1_hstd_int_disable() */

/***********************************************************************************************************************
 * Function Name: R_USBH1_isr
 * Description  : Interrupt service routine of USB host
 * Arguments    : none
 * Return Value : none
 **********************************************************************************************************************/
void R_USBH1_isr(uint32_t int_sense)
{
    uint32_t    int_state;

    int_state = USB10.INT_STATUS.LONG;
    if (USB10_AHB_INT == (int_state & USB10_AHB_INT))
    {
        USB10.INT_STATUS.LONG |= USB10_AHB_INT;
    }
    else if ((USB10_USBH_INTA == (int_state & USB10_USBH_INTA)) || (USB10_USBH_INTB == (int_state & USB10_USBH_INTB)))  /* EHCI or OHCI interrupt */
    {
        usbh1_hstd_hci_interrupt_handler();
    }
    else if (USB10_UCOM_INT == (int_state & USB10_UCOM_INT))
    {
    }
    else if (USB10_WAKEON_INT == (int_state & USB10_WAKEON_INT))
    {
        USB10.INT_STATUS.LONG |= USB10_WAKEON_INT;
    }
    else
    {
    }
} /* End of function R_USBH1_isr() */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
