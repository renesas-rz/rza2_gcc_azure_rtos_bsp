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
 * File Name    : r_usb_preg_access.c
 * Description  : USB IP Peripheral control register access code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_reg_access.h"
#include "r_usb_bitdefine.h"
#include "r_usb_extern.h"


/******************************************************************************
 Function Name   : hw_usb_pclear_drpd
 Description     : Clear DRPD-bit SYSCFG0 register.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pclear_drpd(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= ~USB_DRPD;
#else
    USB_M1.SYSCFG0.WORD &= ~USB_DRPD;
#endif
}
/******************************************************************************
 Function Name   : hw_usb_pset_dprpu
 Description     : Set DPRPU-bit SYSCFG0 register.
                 : (Enable D+Line pullup when PeripheralController function is selected)
 Arguments       : usb_utr_t *ptr  : USB internal structure. Selects USB channel.
 Return value    : none
 ******************************************************************************/
void hw_usb_pset_dprpu(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD |= USB_DPRPU;
#else
    USB_M1.SYSCFG0.WORD |= USB_DPRPU;
#endif
}
/******************************************************************************
 End of function hw_usb_pset_dprpu
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pclear_dprpu
 Description     : Clear DPRPU-bit of the SYSCFG0 register.
                 : (Disable D+Line pullup when PeripheralController function is
                 : selected.)
 Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_pclear_dprpu(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= (~USB_DPRPU);
#else
    USB_M1.SYSCFG0.WORD &= (~USB_DPRPU);
#endif
}
/******************************************************************************
 End of function hw_usb_pclear_dprpu
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pset_wkup
 Description     : Set WKUP-bit DVSTCTR register.
                 : (Output Remote wakeup signal when PeripheralController function is selected)
 Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_pset_wkup(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DVSTCTR0.WORD |= USB_WKUP;
#else
    USB_M1.DVSTCTR0.WORD |= USB_WKUP;
#endif
}
/******************************************************************************
 End of function hw_usb_pset_wkup
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pset_enb_rsme
 Description     : Enable interrupt from RESUME
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pset_enb_rsme(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD |= USB_RSME;
#else
    USB_M1.INTENB0.WORD |= USB_RSME;
#endif
}
/******************************************************************************
 End of function hw_usb_pset_enb_rsme
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pclear_enb_rsme
 Description     : Disable interrupt from RESUME
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pclear_enb_rsme(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD &= (~USB_RSME);
#else
    USB_M1.INTENB0.WORD &= (~USB_RSME);
#endif
}
/******************************************************************************
 End of function hw_usb_pclear_enb_rsme
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pclear_sts_resm
 Description     : Clear interrupt status of RESUME.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pclear_sts_resm(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTSTS0.WORD = (uint16_t)~USB_RESM;
#else
    USB_M1.INTSTS0.WORD = (uint16_t)~USB_RESM;
#endif
}
/******************************************************************************
 End of function hw_usb_pclear_sts_resm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pclear_sts_valid
 Description     : Clear the Setup Packet Reception interrupt flag.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pclear_sts_valid(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTSTS0.WORD = (uint16_t)~USB_VALID;
#else
    USB_M1.INTSTS0.WORD = (uint16_t)~USB_VALID;
#endif
}
/******************************************************************************
 End of function hw_usb_pclear_sts_valid
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pset_ccpl
 Description     : Enable termination of control transfer status stage.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pset_ccpl(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DCPCTR.WORD |= USB_CCPL;
#else
    USB_M1.DCPCTR.WORD |= USB_CCPL;
#endif
}
/******************************************************************************
 End of function hw_usb_pset_ccpl
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_pmodule_init
 Description     : 
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_pmodule_init( void )
{

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD |= USB_USBE;
    USB_M0.LPSTS.WORD |= USB_SUSPENDM;
    usb_cpu_delay_xms(1); /* wait 1msec */

    USB_M0.SYSCFG1.WORD = (USB_CFG_BUSWAIT & 0x003f);

    USB_M0.CFIFOSEL.WORD  = USB0_CFIFO_MBW;
    USB_M0.D0FIFOSEL.WORD = USB0_D0FIFO_MBW;
    USB_M0.D1FIFOSEL.WORD = USB0_D1FIFO_MBW;

#if USB_CFG_ENDIAN == USB_CFG_BIG
    USB_M0.CFIFOSEL.WORD  |= USB_BIGEND;
    USB_M0.D0FIFOSEL.WORD |= USB_BIGEND;
    USB_M0.D1FIFOSEL.WORD |= USB_BIGEND;

#endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

    USB_M0.INTENB0.WORD = (USB_BEMPE | USB_BRDYE | USB_VBSE | USB_DVSE | USB_CTRE);

#else   /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
    USB_M1.SYSCFG0.WORD |= USB_USBE;
    USB_M1.LPSTS.WORD |= USB_SUSPENDM;
    usb_cpu_delay_xms(1); /* wait 1msec */

    USB_M1.SYSCFG1.WORD = (USB_CFG_BUSWAIT & 0x003f);

    USB_M1.CFIFOSEL.WORD  = USB1_CFIFO_MBW;
    USB_M1.D0FIFOSEL.WORD = USB1_D0FIFO_MBW;
    USB_M1.D1FIFOSEL.WORD = USB1_D1FIFO_MBW;

#if USB_CFG_ENDIAN == USB_CFG_BIG
    USB_M1.CFIFOSEL.WORD  |= USB_BIGEND;
    USB_M1.D0FIFOSEL.WORD |= USB_BIGEND;
    USB_M1.D1FIFOSEL.WORD |= USB_BIGEND;

#endif  /* USB_CFG_ENDIAN == USB_CFG_BIG */

    USB_M1.INTENB0.WORD = (USB_BEMPE | USB_BRDYE | USB_VBSE | USB_DVSE | USB_CTRE);

#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
}
/******************************************************************************
 End of function hw_usb_pmodule_init
 ******************************************************************************/

/******************************************************************************
 End of file
 ******************************************************************************/

