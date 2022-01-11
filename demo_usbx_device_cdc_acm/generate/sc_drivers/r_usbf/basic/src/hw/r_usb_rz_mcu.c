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
 * Copyright (C) 2015(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_rx_mcu.c
 * Description  : RX MCU processing
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.21 Support DMAC Technical Update for RX71M/RX64M USBA.
 *         : 30.09.2017 1.22 RX62N/RX630/RX63T-H is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"
#include "r_intc_lld_rza2m.h"
#include "stdbool.h"
#include "r_usb_cstd_rtos.h"

#if (USB_CFG_DMA == USB_CFG_ENABLE)
#include "r_usb_dmac.h"
#endif  /* (USB_CFG_DMA == USB_CFG_ENABLE) */

/******************************************************************************
 Macro definitions
 ******************************************************************************/

#if defined(USB_CFG_PHID_USE)
  #if USB_CFG_DMA == USB_CFG_ENABLE
    #error  In HID class, can not set USB_CFG_ENABLE to USB_CFG_DMA in r_usb_basic_config.h.
  #endif /* USB_CFG_DMA == USB_CFG_ENABLE */
#endif /* defined(USB_CFG_PHID_USE) */


#if USB_CFG_USE_USBIP == USB_CFG_MULTI
  #error  Can not set USB_CFG_MULTI to USB_CFG_USE_USBIP when setting USB_CFG_PERI to USB_CFG_MODE \
       in r_usb_basic_config.h.
#endif /* USB_CFG_USE_USBIP == USB_CFG_MULTI */


#if USB_CFG_COMPLIANCE == USB_CFG_DISABLE
  #if USB_CFG_ELECTRICAL == USB_CFG_ENABLE
    #error  Can not set USB_CFG_DISABLE to USB_CFG_COMPLIANCE when setting USB_CFG_ENABLE to USB_CFG_ELECTRICAL \
         in r_usb_basic_config.h.

  #endif /* USB_CFG_ELECTRICAL == USB_CFG_ENABLE */
#endif /* USB_CFG_COMPLIANCE == USB_CFG_DISABLE */


#if USB_CFG_BC == USB_CFG_DISABLE
  #if USB_CFG_DCP == USB_CFG_ENABLE
    #error  Can not set USB_CFG_DISABLE to USB_CFG_BC when setting USB_CFG_ENABLE to USB_CFG_DCP \
         in r_usb_basic_config.h.

  #endif /* USB_CFG_DCP == USB_CFG_ENABLE */
#endif /* USB_CFG_BC == USB_CFG_DISABLE */

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
static void usbfs_usbi_isr (void);
static void usbhs_usbir_isr (void);
/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Renesas Abstracted RSK functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_module_start
 Description     : USB module start
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_start (uint8_t ip_type)
{
	uint8_t dummy_read;

    if (USB_IP0 == ip_type)
    {
        if (0 == CPG.STBCR6.BIT.MSTP61)
        {
            return USB_ERR_BUSY;
        }
        /* Enable power for USB0 */
        CPG.STBCR6.BIT.MSTP61 = 0;

        dummy_read = CPG.STBCR6.BYTE;
#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
        USB00.PHYCLK_CTRL.LONG = 0x00000001;    /* USBPHY select : USB_X1 */
#else
        USB00.PHYCLK_CTRL.LONG = 0x00000000;    /* USBPHY select : EXTAL */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */
        usb_cpu_delay_1us((uint16_t) 500);      /* 500us wait */
        USB00.COMMCTRL.BIT.OTG_PERI = 1;        /* 0 : Host, 1 : Peri */
        USB00.PHYIF_CTRL.LONG = 0x00000001;     /* It can be recovered from the deep-stanby by signal change of D+,D-. */
        USB00.USBCTR.LONG &= ~0x00000002;       /* UTMI+PHY reset OFF */
        usb_cpu_delay_1us((uint16_t) 100);      /* 100us wait */
    }
    else
    {
        if (0 == CPG.STBCR6.BIT.MSTP60)
        {
            return USB_ERR_BUSY;
        }
        /* Enable power for USBA */
        CPG.STBCR6.BIT.MSTP60 = 0;

        dummy_read = CPG.STBCR6.BYTE;
#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
        USB10.PHYCLK_CTRL.LONG = 0x00000001;    /* USBPHY select : USB_X1 */
#else
        USB10.PHYCLK_CTRL.LONG = 0x00000000;    /* USBPHY select : EXTAL */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */
        usb_cpu_delay_1us((uint16_t) 500);      /* 500us wait */
        USB10.COMMCTRL.BIT.OTG_PERI = 1;        /* 0 : Host, 1 : Peri */
        USB10.PHYIF_CTRL.LONG = 0x00000001;     /* It can be recovered from the deep-stanby by signal change of D+,D-. */
        USB10.USBCTR.LONG &= ~0x00000002;       /* UTMI+PHY reset OFF */
        usb_cpu_delay_1us((uint16_t) 100);      /* 100us wait */
    }

    return USB_SUCCESS;
}
/******************************************************************************
End of function usb_module_start
******************************************************************************/

/******************************************************************************
 Function Name   : usb_module_stop
 Description     : USB module stop
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : none
 ******************************************************************************/
usb_err_t usb_module_stop (uint8_t ip_type)
{
    if (USB_IP0 == ip_type)
    {
        if (0 != CPG.STBCR6.BIT.MSTP61)
        {
            return USB_ERR_NOT_OPEN;
        }
        /* Disable power for USB0 */
        CPG.STBCR6.BIT.MSTP61 = 1;
    }
    else if (USB_IP1 == ip_type)
    {
        if (0 != CPG.STBCR6.BIT.MSTP60)
        {
            return USB_ERR_NOT_OPEN;
        }
        /* Disable power for USBA */
        CPG.STBCR6.BIT.MSTP60 = 1;
    }
    else
    {
        return USB_ERR_PARA;
    }
    return USB_SUCCESS;
}
/******************************************************************************
End of function usb_module_stop
******************************************************************************/

/******************************************************************************
 Function Name   : usb_cpu_usbint_init
 Description     : USB interrupt Initialize
 Arguments       : uint8_t ip_type  : USB_IP0/USB_IP1
 Return value    : void
 ******************************************************************************/
void usb_cpu_usbint_init (uint8_t ip_type)
{
    if ( USB_IP0 == ip_type)
    {
#if (USB_CFG_DMA == USB_CFG_ENABLE)
    	R_INTC_Enable(INTC_ID_USB_USBFDMA00); /* D0FIFO0 Enable */
        R_INTC_Enable(INTC_ID_USB_USBFDMA01); /* D1FIFO0 Enable */
#endif  /* (USB_CFG_DMA == USB_CFG_ENABLE) */
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBFI0, usb_pstd_usb_handler);
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBHI0, usb_pstd_usb_handler);
        R_INTC_Enable(INTC_ID_USB_USBFI0); /* USBR0 Enable */
        R_INTC_Enable(INTC_ID_USB_USBHI0); /* USBR0 Enable */
        R_INTC_SetPriority(INTC_ID_USB_USBFI0, USB_CFG_INTERRUPT_PRIORITY);
        R_INTC_SetPriority(INTC_ID_USB_USBHI0, USB_CFG_INTERRUPT_PRIORITY);
    }

    if (USB_IP1 == ip_type)
    {
#if (USB_CFG_DMA == USB_CFG_ENABLE)
        R_INTC_Enable(INTC_ID_USB_USBFDMA10); /* Enable D0FIFO interrupt */
        R_INTC_Enable(INTC_ID_USB_USBFDMA11); /* Enable D1FIFO interrupt */
#endif  /* (USB_CFG_DMA == USB_CFG_ENABLE) */
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBFI1, usb_pstd_usb_handler);
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBHI1, usb_pstd_usb_handler);
        R_INTC_Enable(INTC_ID_USB_USBFI1); /* Enable USB11 interrupt */
        R_INTC_Enable(INTC_ID_USB_USBHI1); /* Enable USB11 interrupt */
        R_INTC_SetPriority(INTC_ID_USB_USBFI1, USB_CFG_INTERRUPT_PRIORITY);
        R_INTC_SetPriority(INTC_ID_USB_USBHI1, USB_CFG_INTERRUPT_PRIORITY);
    }
}
/******************************************************************************
 End of function usb_cpu_usbint_init
 ******************************************************************************/

/******************************************************************************
 TIMER function
 ******************************************************************************/
/******************************************************************************
 Function Name   : usb_cpu_delay_1us
 Description     : 1us Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1us)
 Return value    : none
 Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_1us (uint16_t time)
{
    volatile uint32_t   i;
    volatile uint8_t   tmp;

    /* Wait 1us (Please change for your MCU) */
    for (i = 0; i < (3 * time); ++i)  /* 要変更 */
    {
        tmp = CPG.CPUSTS.BYTE;
    };
}
/******************************************************************************
 End of function usb_cpu_delay_1us
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cpu_delay_xms
 Description     : xms Delay timer
 Arguments       : uint16_t  time        ; Delay time(*1ms)
 Return value    : void
 Note            : Please change for your MCU
 ******************************************************************************/
void usb_cpu_delay_xms (uint16_t time)
{
    R_OS_TaskSleep(time/portTICK_PERIOD_MS);
}
/******************************************************************************
 End of function usb_cpu_delay_xms
 ******************************************************************************/

/*******************************************************************************
 * Function Name: usbfs_usbi_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
static void usbfs_usbi_isr (void)
{
    /* Call USB interrupt routine */
    usb_pstd_usb_handler(); /* Call interrupt routine */
}
/******************************************************************************
 End of function usbfs_usbi_isr
 ******************************************************************************/


/*******************************************************************************
 * Function Name: usbhs_usbir_isr
 * Description  : Interrupt service routine of USBF
 * Arguments    : none
 * Return Value : none
 *******************************************************************************/
static void usbhs_usbir_isr (void)
{
    /* Condition compilation by the difference of USB function */
    usb_pstd_usb_handler();
}
/******************************************************************************
 End of function usbhs_usbir_isr
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cpu_usb_int_hand_isr
 Description     :
 Arguments       : none
 Return value    : none
 ******************************************************************************/
static void usb_cpu_usb_int_hand_isr (void)
{
    hw_usb_pclear_sts_resm();
    R_INTC_SetPriority(INTC_ID_USB_USBFI0, 0x00);
} /* End of function usb_cpu_usb_int_hand_isr */

/******************************************************************************
Function Name   : usb_check_use_usba_module
Description     : 
Arguments       : usb_utr_t *ptr     : Pointer to usb_utr_t structure
Return value    : 
******************************************************************************/
bool usb_check_use_usba_module(usb_utr_t *ptr)
{
    bool    ret_code = false;

    ret_code = true;
    return ret_code;
}   /* End of function usb_check_use_usba_module */

/******************************************************************************
 End  Of File
 ******************************************************************************/
