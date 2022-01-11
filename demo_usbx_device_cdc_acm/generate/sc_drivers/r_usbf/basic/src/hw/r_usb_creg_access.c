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
 * File Name    : r_usb_reg_access.c
 * Description  : USB IP register access code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.20 hw_usb_write_fifo16(),hw_usb_write_fifo8(),hw_usb_get_fifosel_adr(),hw_usb_set_dreqe(),
 *         :                 hw_usb_set_curpipe() is fixed.
 *         : 30.09.2017 1.22 RX62N/RX630/RX63T-H is added. and Add "__evenaccess" for I/O reg access local variable.
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
#if (USB_CFG_DMA == USB_CFG_ENABLE)
#include "r_usb_dmac.h"
#endif /* (USB_CFG_DMA == USB_CFG_ENABLE) */

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void *hw_usb_get_fifosel_adr (usb_utr_t *ptr, uint16_t pipemode);
static void *hw_usb_get_fifoctr_adr (usb_utr_t *ptr, uint16_t pipemode);

/******************************************************************************
 Function Name   : hw_usb_read_syscfg
 Description     : Returns the specified port's SYSCFG register value.
 Arguments       : usb_utr_t    *ptr   : Pointer to usb_utr_t structure.
 Return value    : SYSCFG content.
 ******************************************************************************/
uint16_t hw_usb_read_syscfg (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return USB_M0.SYSCFG0.WORD;
#else
    return USB_M1.SYSCFG0.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_syscfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_cnen
 Description     : Enable single end receiver.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_set_cnen(void)
{
    /* Only USBA module */
#if USB_CFG_USE_USBIP == USB_CFG_IP1
    USB_M1.SYSCFG0.WORD |= USB_CNEN;
#else
    USB_M0.SYSCFG0.WORD |= USB_CNEN;
#endif
}
/******************************************************************************
 End of function hw_usb_set_cnen
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_cnen
 Description     : Disable single end receiver.
 Arguments       : usb_utr_t    *ptr   : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_cnen(usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= (~USB_CNEN);
#else
    USB_M1.SYSCFG0.WORD &= (~USB_CNEN);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_cnen
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_hse
 Description     : Not processed as the functionality is provided by R8A66597(ASSP).
 Arguments       : usb_utr_t    *ptr  : Pointer to usb_utr_t structure.
  Return value    : none
 ******************************************************************************/
void hw_usb_set_hse(usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD |= USB_HSE;
#else
    USB_M1.SYSCFG0.WORD |= USB_HSE;
#endif
}
/******************************************************************************
 End of function hw_usb_set_hse
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_hse
 Description     : Clears HSE bit of the specified port's SYSCFG register
 Arguments       : usb_utr_t    *ptr : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_hse(usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= (~USB_HSE);
#else
    USB_M1.SYSCFG0.WORD &= (~USB_HSE);
#endif
} /* End of function hw_usb_clear_hse */
/******************************************************************************
 End of function hw_usb_clear_hse
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_dcfm
 Description     : DCFM-bit set of register SYSCFG
                 : (USB Host mode is selected.)
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_set_dcfm(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD |= USB_DCFM;
#else
    USB_M1.SYSCFG0.WORD |= USB_DCFM;
#endif
}
/******************************************************************************
 End of function hw_usb_set_dcfm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_dcfm
 Description     : DCFM-bit clear of register SYSCFG.
                 : (USB Peripheral mode is selected.)
 Arguments       : usb_utr_t    *ptr  : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_dcfm (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= (~USB_DCFM);
#else
    USB_M1.SYSCFG0.WORD &= (~USB_DCFM);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_dcfm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_usbe
 Description     : Enable USB operation.
 Arguments       : usb_utr_t    *ptr  : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_usbe (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD |= USB_USBE;
#else
    USB_M1.SYSCFG0.WORD |= USB_USBE;
#endif
}
/******************************************************************************
 End of function hw_usb_set_usbe
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_usbe
 Description     : Enable USB operation.
 Arguments       : usb_utr_t    *ptr  : Pointer to usb_utr_t structure.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_usbe (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.SYSCFG0.WORD &= (~USB_USBE);
#else
    USB_M1.SYSCFG0.WORD &= (~USB_USBE);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_usbe
 ******************************************************************************/
#if 0
/******************************************************************************
Function Name   : hw_usb_set_bcctrl
Description     : Set BCCTRL's bits.
Arguments       : usb_utr_t     *ptr   : Pointer to usb_utr_t structure.
                : uint16_t      data   : Setting value
Return value    : none
******************************************************************************/
void    hw_usb_set_bcctrl (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP1
    USB_M1.BCCTRL.WORD |= data;
#else
    USB_M0.BCCTRL.WORD |= data;
#endif
}
/******************************************************************************
 End of function hw_usb_set_bcctrl
 ******************************************************************************/

/******************************************************************************
Function Name   : hw_usb_clear_bcctrl
Description     : Clear BCCTRL's bits.
Arguments       : usb_utr_t *ptr    : Pointer to usb_utr_t structure.
                : uint16_t  data    : Setting value
Return value    : none
******************************************************************************/
void    hw_usb_clear_bcctrl (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP1
    USB_M1.BCCTRL.WORD &= ~data;
#else
    USB_M0.BCCTRL.WORD &= ~data;
#endif
}
/******************************************************************************
 End of function hw_usb_clear_bcctrl
 ******************************************************************************/
#endif
/******************************************************************************
 Function Name   : hw_usb_read_syssts
 Description     : Returns the value of the specified port's SYSSTS register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
 Return value    : SYSSTS0 content
 ******************************************************************************/
uint16_t hw_usb_read_syssts (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)(USB_M0.SYSSTS0.WORD);
#else
    return (uint16_t)(USB_M1.SYSSTS0.WORD);
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_syssts
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_dvstctr
 Description     : Returns the specified port's DVSTCTR register content.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
 Return value    : DVSTCTR0 content
 ******************************************************************************/
uint16_t hw_usb_read_dvstctr (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)(USB_M0.DVSTCTR0.WORD);
#else
    return (uint16_t)(USB_M1.DVSTCTR0.WORD);
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_dvstctr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_utst
 Description     : Not processed as the functionality is provided by R8A66597(ASSP).
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  data  : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_set_utst(usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.TESTMODE.WORD = data;
#else
    USB_M1.TESTMODE.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_set_utst
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_fifo32
 Description     : Data is read from the specified pipemode's FIFO register, 32-bits 
                 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
 Return value    : CFIFO/D0FIFO/D1FIFO content (32-bit)
 ******************************************************************************/
uint32_t hw_usb_read_fifo32(usb_utr_t *ptr, uint16_t pipemode)
{
    uint32_t data;

    switch (pipemode)
    {
        case USB_CUSE:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
            data = USB_M1.CFIFO.LONG;
#else  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            data = USB_M0.CFIFO.LONG;
#endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        break;
        default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE2);
        break;
    }

    return data;
}
/******************************************************************************
 End of function hw_usb_read_fifo32
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_fifo32
 Description     : Data is written to the specified pipemode's FIFO register, 32-bits 
                 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
                 : uint32_t  data      : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_fifo32(usb_utr_t *ptr, uint16_t pipemode, uint32_t data)
{
    switch (pipemode)
    {
        case USB_CUSE:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
            USB_M1.CFIFO.LONG = data;
#else  /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
            USB_M0.CFIFO.LONG = data;
#endif /* USB_CFG_USE_USBIP == USB_CFG_IP0 */
        break;
        default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE3);
        break;
    }
}
/******************************************************************************
 End of function hw_usb_write_fifo32
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_fifo16
 Description     : Data is read from the specified pipemode's FIFO register, 16-bits 
                 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : CFIFO/D0FIFO/D1FIFO content (16-bit)
 ******************************************************************************/
uint16_t hw_usb_read_fifo16 (usb_utr_t *ptr, uint16_t pipemode)
{
    uint16_t data;

    switch (pipemode)
    {
        case USB_CUSE:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
            data = USB_M1.CFIFO.WORD.H;
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            data = USB_M0.CFIFO.WORD.H;
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
        break;
        default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE5);
        break;
    }
    return data;
}
/******************************************************************************
 End of function hw_usb_read_fifo16
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_fifo16
 Description     : Data is written to the specified pipemode's FIFO register, 16-bits 
                 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
                 : uint16_t  data      : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_fifo16 (usb_utr_t *ptr, uint16_t pipemode, uint16_t data)
{
    uint16_t    ip;

  #if USB_CFG_USE_USBIP == USB_CFG_IP1
        ip = USB_IP1;
  #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
        ip = USB_IP0;

  #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */

    if (USB_IP0 == ip)
    {
        switch (pipemode)
        {
            case USB_CUSE :
                USB0_CFIFO16 = data;
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE6);
            break;
        }
    }
#if USB_NUM_USBIP == 2
    else if (USB_IP1 == ip)
    {
        switch (pipemode)
        {
            case USB_CUSE :
                USB1_CFIFO16 = data;
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE7);
            break;
        }
    }
#endif  /* USB_NUM_USBIP == 2 */
    else
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE8);
    }
}
/******************************************************************************
 End of function hw_usb_write_fifo16
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_fifo8
 Description     : Data is written to the specified pipemode's FIFO register, 8-bits 
                 : wide, corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
                 : uint8_t  data       : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_fifo8 (usb_utr_t *ptr, uint16_t pipemode, uint8_t data)
{
    uint16_t    ip;

  #if USB_CFG_USE_USBIP == USB_CFG_IP1
        ip = USB_IP1;
  #else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
        ip = USB_IP0;
  #endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */

    if (USB_IP0 == ip)
    {
        switch (pipemode)
        {
            case USB_CUSE :
                USB0_CFIFO8 = data;
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE9);
            break;
        }
    }
#if USB_NUM_USBIP == 2
    else if (USB_IP1 == ip)
    {
        switch (pipemode)
        {
            case USB_CUSE :
                USB1_CFIFO8 = data;
            break;
            default :
                USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE10);
            break;
        }
    }
#endif  /* USB_NUM_USBIP == 2 */
    else
    {
        USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE11);
    }
}
/******************************************************************************
 End of function hw_usb_write_fifo8
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_get_fifosel_adr
 Description     : Returns the *address* of the FIFOSEL register corresponding to 
                 : specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
static void *hw_usb_get_fifosel_adr (usb_utr_t *ptr, uint16_t pipemode)
{
    void *p_reg;

        switch (pipemode)
        {
            case USB_CUSE:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
            p_reg = (void *)&(USB_M1.CFIFOSEL);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            p_reg = (void *)&(USB_M0.CFIFOSEL);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;

            case    USB_D0USE:
            case    USB_D0DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                p_reg = (void *)&(USB_M1.D0FIFOSEL);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                p_reg = (void *)&(USB_M0.D0FIFOSEL);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                break;

            case    USB_D1USE:
            case    USB_D1DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                p_reg = (void *)&(USB_M1.D1FIFOSEL);
#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                p_reg = (void *)&(USB_M0.D1FIFOSEL);
#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE12);
            break;
        }

    return p_reg;
}
/******************************************************************************
 End of function hw_usb_get_fifosel_adr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_fifosel
 Description     : Returns the value of the specified pipemode's FIFOSEL register.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : FIFOSEL content
 ******************************************************************************/
uint16_t hw_usb_read_fifosel (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifosel_adr(ptr, pipemode);

    return *p_reg;
}
/******************************************************************************
 End of function hw_usb_read_fifosel
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_rmw_fifosel
 Description     : Data is written to the specified pipemode's FIFOSEL register 
                 : (the FIFOSEL corresponding to the specified PIPEMODE), using 
                 : read-modify-write.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
                 : uint16_t  data      : Setting value
                 : uint16_t bitptn   : bitptn: Bit pattern to read-modify-write.
 Return value    : none
 ******************************************************************************/
void hw_usb_rmw_fifosel (usb_utr_t *ptr, uint16_t pipemode, uint16_t data, uint16_t bitptn)
{
    uint16_t buf;
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifosel_adr(ptr, pipemode);

    buf = *p_reg;
    buf &= (~bitptn);
    buf |= (data & bitptn);
    *p_reg = buf;
}
/******************************************************************************
 End of function hw_usb_rmw_fifosel
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_dclrm
 Description     : Set DCLRM-bits (FIFO buffer auto clear) of the FIFOSEL cor-
                 : responding to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_set_dclrm (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifosel_adr(ptr, pipemode);

    (*p_reg) |= USB_DCLRM;
}
/******************************************************************************
 End of function hw_usb_set_dclrm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_dclrm
 Description     : Reset DCLRM-bits (FIFO buffer not auto-cleared) of the FIFOSEL 
                 : corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_dclrm (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = hw_usb_get_fifosel_adr(ptr, pipemode);

    (*p_reg) &= (~USB_DCLRM);
}
/******************************************************************************
 End of function hw_usb_clear_dclrm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_dreqe
 Description     : Set DREQE-bits (to output signal DxREQ_Na) of the FIFOSEL cor-
                 : responding to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_set_dreqe (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = hw_usb_get_fifosel_adr(ptr, pipemode);

    if (true == usb_check_use_usba_module(ptr))
    {
        (*p_reg) &= (~(USB_DREQE | USB_CURPIPE));
        while(1)
        {
            if (0 == ((*p_reg) & USB_CURPIPE))
            {
                break;
            }
        }
    }
    else
    {
        (*p_reg) &= (~USB_DREQE);
    }

    (*p_reg) |= USB_DREQE;
}
/******************************************************************************
 End of function hw_usb_set_dreqe
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_dreqe
 Description     : Clear DREQE-bits (To prohibit the output of the signal DxREQ_N)
                 : of the FIFOSEL corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr      : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode  : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_dreqe (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = hw_usb_get_fifosel_adr(ptr, pipemode);

    (*p_reg) &= (~USB_DREQE);
}
/******************************************************************************
 End of function hw_usb_clear_dreqe
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_mbw
 Description     : Set MBW-bits (CFIFO Port Access Bit Width) of the FIFOSEL cor-
                 : responding to the specified PIPEMODE, to select 8 or 16-bit 
                 : wide FIFO port access.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
                 : uint16_t  data       : Setting value
                 : (data = 0x0400), 32 bit (data = 0x0800) access mode.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_mbw (usb_utr_t *ptr, uint16_t pipemode, uint16_t data)
{
    volatile uint16_t *p_reg;

    p_reg = hw_usb_get_fifosel_adr(ptr, pipemode);
    (*p_reg) &= (~USB_MBW);
    if (0 != data)
    {
        (*p_reg) |= data;
    }
}
/******************************************************************************
 End of function hw_usb_set_mbw
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_curpipe
 Description     : Set pipe to the number given; in the FIFOSEL corresponding 
                 : to specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
                 : uint16_t  pipeno     : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_curpipe (usb_utr_t *ptr, uint16_t pipemode, uint16_t pipeno)
{
    volatile uint16_t *p_reg;
    volatile uint16_t reg;

    p_reg = hw_usb_get_fifosel_adr(ptr, pipemode);
    reg = *p_reg;

    if ((USB_D0DMA == pipemode) || (USB_D1DMA == pipemode))
    {
        if (false == usb_check_use_usba_module(ptr))
        {
            reg &= (~USB_DREQE);
        }
    }

    reg &= (~USB_CURPIPE);
    *p_reg = reg;
    while(1)
    {
        if (0 == ((*p_reg) & USB_CURPIPE))
        {
            break;
        }
    }

    reg |= pipeno;

    *p_reg = reg;
}
/******************************************************************************
 End of function hw_usb_set_curpipe
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_get_fifoctr_adr
 Description     : Returns the *address* of the FIFOCTR register corresponding to 
                 : specified PIPEMODE.
                 : (FIFO Port Control Register.)
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
static void *hw_usb_get_fifoctr_adr (usb_utr_t *ptr, uint16_t pipemode)
{
    void *p_reg;

    if(USB_NULL == ptr)
    {
        switch (pipemode)
        {
            case USB_CUSE:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
            p_reg = (void *)&(USB_M1.CFIFOCTR);

#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            p_reg = (void *)&(USB_M0.CFIFOCTR);

#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;

            case    USB_D0USE:
            case    USB_D0DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                p_reg = (void *)&(USB_M1.D0FIFOCTR);

#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                p_reg = (void *)&(USB_M0.D0FIFOCTR);

#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                break;
            case    USB_D1USE:
            case    USB_D1DMA:
#if USB_CFG_USE_USBIP == USB_CFG_IP1
                p_reg = (void *)&(USB_M1.D1FIFOCTR);

#else   /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
                p_reg = (void *)&(USB_M0.D1FIFOCTR);

#endif  /* USB_CFG_USE_USBIP == USB_CFG_IP1 */
            break;
            default:
            USB_DEBUG_HOOK(USB_DEBUG_HOOK_STD | USB_DEBUG_HOOK_CODE13);
            break;
        }
    }
    else
    {
    }

    return p_reg;
}
/******************************************************************************
 End of function hw_usb_get_fifoctr_adr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_fifoctr
 Description     : Returns the value of the FIFOCTR register corresponding to 
                 : specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
 Return value    : FIFOCTR content
 ******************************************************************************/
uint16_t hw_usb_read_fifoctr (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifoctr_adr(ptr, pipemode);

    return *p_reg;
}
/******************************************************************************
 End of function hw_usb_read_fifoctr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_bval
 Description     : Set BVAL (Buffer Memory Valid Flag) to the number given; in 
                 : the FIFOCTR corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_set_bval (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifoctr_adr(ptr, pipemode);

    (*p_reg) |= USB_BVAL;
}
/******************************************************************************
 End of function hw_usb_set_bval
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_bclr
 Description     : Set BCLR (CPU Buffer Clear) to the number given; in the 
                 : FIFOCTR corresponding to the specified PIPEMODE.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipemode   : CUSE/D0DMA/D1DMA
 Return value    : none
 ******************************************************************************/
void hw_usb_set_bclr (usb_utr_t *ptr, uint16_t pipemode)
{
    volatile uint16_t *p_reg;

    p_reg = (uint16_t *) hw_usb_get_fifoctr_adr(ptr, pipemode);

    *p_reg = USB_BCLR;
}
/******************************************************************************
 End of function hw_usb_set_bclr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_intenb
 Description     : Data is written to INTENB register, 
                 : enabling/disabling the various USB interrupts.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void hw_usb_write_intenb (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD = data;
#else
    USB_M1.INTENB0.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_intenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_intenb
 Description     : Bit(s) to be set in INTENB register, 
                 : enabling the respective USB interrupt(s).
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  data  : Bit pattern: Respective interrupts with '1' 
                 :                   will be enabled.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_intenb (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTENB0.WORD |= data;
#else
    USB_M1.INTENB0.WORD |= data;
#endif
}
/******************************************************************************
 End of function hw_usb_set_intenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_brdyenb
 Description     : A bit is set in the specified pipe's BRDYENB, enabling the 
                 : respective pipe BRDY interrupt(s).
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_brdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BRDYENB.WORD |= (1 << pipeno);
#else
    USB_M1.BRDYENB.WORD |= (1 << pipeno);
#endif
}
/******************************************************************************
 End of function hw_usb_set_brdyenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_brdyenb
 Description     : Clear the PIPExBRDYE-bit of the specified pipe to prohibit 
                 : BRDY interrupts of that pipe.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_brdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BRDYENB.WORD &= (~(1 << pipeno));
#else
    USB_M1.BRDYENB.WORD &= (~(1 << pipeno));
#endif
}
/******************************************************************************
 End of function hw_usb_clear_brdyenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_nrdyenb
 Description     : A bit is set in the specified pipe's NRDYENB, enabling the 
                 : respective pipe NRDY interrupt(s).
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_nrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.NRDYENB.WORD |= (1 << pipeno);
#else
    USB_M1.NRDYENB.WORD |= (1 << pipeno);
#endif
}
/******************************************************************************
 End of function hw_usb_set_nrdyenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_nrdyenb
 Description     : Clear the PIPExNRDYE-bit of the specified pipe to prohibit 
                 : NRDY interrupts of that pipe.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_nrdyenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.NRDYENB.WORD &= (1 << pipeno);
#else
    USB_M1.NRDYENB.WORD &= (1 << pipeno);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_nrdyenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_bempenb
 Description     : A bit is set in the specified pipe's BEMPENB enabling the 
                 : respective pipe's BEMP interrupt(s).
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno  : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_bempenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BEMPENB.WORD |= (1 << pipeno);
#else
    USB_M1.BEMPENB.WORD |= (1 << pipeno);
#endif
}
/******************************************************************************
 End of function hw_usb_set_bempenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_bempenb
 Description     : Clear the PIPExBEMPE-bit of the specified pipe to prohibit 
                 : BEMP interrupts of that pipe.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_bempenb (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BEMPENB.WORD &= (~(1 << pipeno));
#else
    USB_M1.BEMPENB.WORD &= (~(1 << pipeno));
#endif
}
/******************************************************************************
 End of function hw_usb_clear_bempenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_sofcfg
 Description     : Set Bit pattern for SOFCFG
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  data  : The value to OR.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_sofcfg (usb_utr_t *ptr, uint16_t data)
{
}
/******************************************************************************
 End of function hw_usb_set_sofcfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_intsts
 Description     : Returns INTSTS0 register content.
 Arguments       : none
 Return value    : INTSTS0 content
 ******************************************************************************/
uint16_t hw_usb_read_intsts(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return USB_M0.INTSTS0.WORD;
#else
    return USB_M1.INTSTS0.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_intsts
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_intsts
 Description     : Data is written to INTSTS0 register.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  data  : The value to write.
 Return value    : none
 ******************************************************************************/
void hw_usb_write_intsts (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.INTSTS0.WORD = data;
#else
    USB_M1.INTSTS0.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_intsts
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_sts_brdy
 Description     : Clear the PIPExBRDY status bit of the specified pipe to clear 
                 : its BRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_sts_brdy (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BRDYSTS.WORD = (uint16_t) (~(1 << pipeno)) & BRDYSTS_MASK;
#else
    USB_M1.BRDYSTS.WORD = (uint16_t) (~(1 << pipeno)) & BRDYSTS_MASK;
#endif
}
/******************************************************************************
 End of function hw_usb_clear_sts_brdy
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_status_nrdy
 Description     : Clear the PIPExNRDY status bit of the specified pipe to clear 
                 : its NRDY interrupt status.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_status_nrdy (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.NRDYSTS.WORD = (uint16_t) ((~(1 << pipeno)) & NRDYSTS_MASK);
#else
    USB_M1.NRDYSTS.WORD = (uint16_t) ((~(1 << pipeno)) & NRDYSTS_MASK);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_status_nrdy
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_status_bemp
 Description     : Clear the PIPExBEMP status bit of the specified pipe to clear 
                 : its BEMP interrupt status.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno: Pipe number.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_status_bemp (usb_utr_t *ptr, uint16_t pipeno)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.BEMPSTS.WORD = (uint16_t) ((~(1 << pipeno)) & BEMPSTS_MASK);
#else
    USB_M1.BEMPSTS.WORD = (uint16_t) ((~(1 << pipeno)) & BEMPSTS_MASK);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_status_bemp
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_frmnum
 Description     : Returns FRMNUM register content.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
 Return value    : FRMNUM content
 ******************************************************************************/
uint16_t hw_usb_read_frmnum (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.FRMNUM.WORD;
#else
    return (uint16_t)USB_M1.FRMNUM.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_frmnum
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_usbreq
 Description     : Returns USBREQ register content.
 Arguments       : none
 Return value    : USBREQ content
 ******************************************************************************/
uint16_t hw_usb_read_usbreq(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBREQ.WORD;
#else
    return (uint16_t)USB_M1.USBREQ.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_usbreq
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_usbval
 Description     : Returns USBVAL register content.
 Arguments       : none
 Return value    : USBVAL content
 ******************************************************************************/
uint16_t hw_usb_read_usbval(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBVAL.WORD;
#else
    return (uint16_t)USB_M1.USBVAL.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_usbval
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_usbindx
 Description     : Returns USBINDX register content.
 Arguments       : none
 Return value    : USBINDX content
 ******************************************************************************/
uint16_t hw_usb_read_usbindx(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBINDX.WORD;
#else
    return (uint16_t)USB_M1.USBINDX.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_usbindx
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_usbleng
 Description     : Returns USBLENG register content.
 Arguments       : none
 Return value    : USBLENG content
 ******************************************************************************/
uint16_t hw_usb_read_usbleng(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.USBLENG.WORD;
#else
    return (uint16_t)USB_M1.USBLENG.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_usbleng
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_dcpcfg
 Description     : Returns DCPCFG register content.
 Arguments       : usb_utr_t *ptr  : Pointer to usb_utr_t structure.
 Return value    : DCPCFG content
 ******************************************************************************/
uint16_t hw_usb_read_dcpcfg (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.DCPCFG.WORD;
#else
    return (uint16_t)USB_M1.DCPCFG.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_dcpcfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_dcpcfg
 Description     : Specified data is written to DCPCFG register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_dcpcfg (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DCPCFG.WORD = data;
#else
    USB_M1.DCPCFG.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_dcpcfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_dcpmaxp
 Description     : Returns DCPMAXP register content.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
 Return value    : DCPMAXP content
 ******************************************************************************/
uint16_t hw_usb_read_dcpmaxp (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.DCPMAXP.WORD;
#else
    return (uint16_t)USB_M1.DCPMAXP.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_dcpmaxp
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_dcpmxps
 Description     : Specified data is written to DCPMAXP register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_dcpmxps (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.DCPMAXP.WORD = data;
#else
    USB_M1.DCPMAXP.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_dcpmxps
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_dcpctr
 Description     : Returns DCPCTR register content.
 Arguments       : none
 Return value    : DCPCTR content
 ******************************************************************************/
uint16_t hw_usb_read_dcpctr(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.DCPCTR.WORD;
#else
    return (uint16_t)USB_M1.DCPCTR.WORD;
#endif
}
/******************************************************************************
 End of function hw_usb_read_dcpctr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipesel
 Description     : Specified data is written to PIPESEL register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : The value to write.
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipesel (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.PIPESEL.WORD = data;
#else
    USB_M1.PIPESEL.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_pipesel
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_pipecfg
 Description     : Returns PIPECFG register content.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
 Return value    : PIPECFG content
 ******************************************************************************/
uint16_t hw_usb_read_pipecfg (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.PIPECFG.WORD;
#else
    return (uint16_t)USB_M1.PIPECFG.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_pipecfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipecfg
 Description     : Specified data is written to PIPECFG register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipecfg (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.PIPECFG.WORD = data;
#else
    USB_M1.PIPECFG.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_pipecfg
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipebuf
 Description     : Specified the value by 2nd argument is set to PIPEBUF register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipebuf(usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.PIPEBUF.WORD = data;
#else
    USB_M1.PIPEBUF.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_pipebuf
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_pipebuf
 Description     : Returns PIPECFG register content.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
 Return value    : PIPEBUF content
 ******************************************************************************/
uint16_t hw_usb_read_pipebuf(usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.PIPEBUF.WORD;
#else
    return (uint16_t)USB_M1.PIPEBUF.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_pipebuf
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_pipemaxp
 Description     : Returns PIPEMAXP register content.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
 Return value    : PIPEMAXP content
 ******************************************************************************/
uint16_t hw_usb_read_pipemaxp (usb_utr_t *ptr)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    return (uint16_t)USB_M0.PIPEMAXP.WORD;
#else
    return (uint16_t)USB_M1.PIPEMAXP.WORD;
#endif
    return 0;
}
/******************************************************************************
 End of function hw_usb_read_pipemaxp
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipemaxp
 Description     : Specified data is written to PIPEMAXP register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipemaxp (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.PIPEMAXP.WORD = data;
#else
    USB_M1.PIPEMAXP.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_pipemaxp
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipeperi
 Description     : Specified data is written to PIPEPERI register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  data   : Setting value
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipeperi (usb_utr_t *ptr, uint16_t data)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.PIPEPERI.WORD = data;
#else
    USB_M1.PIPEPERI.WORD = data;
#endif
}
/******************************************************************************
 End of function hw_usb_write_pipeperi
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_read_pipectr
 Description     : Returns DCPCTR or the specified pipe's PIPECTR register content.
                 : The Pipe Control Register returned is determined by the speci-
                 : fied pipe number.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number.
 Return value    : PIPExCTR content
 ******************************************************************************/
uint16_t hw_usb_read_pipectr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            p_reg = (uint16_t *)&(USB_M0.DCPCTR);
        }
        else
        {
            p_reg = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
        }
#else
        if (USB_PIPE0 == pipeno)
        {
            p_reg = (uint16_t *)&(USB_M1.DCPCTR);
        }
        else
        {
            p_reg = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
        }
#endif
    return *p_reg;
}
/******************************************************************************
 End of function hw_usb_read_pipectr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_csclr
 Description     : Set CSCLR bit in the specified pipe's PIPECTR register
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_csclr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
#endif
    (*p_reg) |= USB_CSCLR;
}
/******************************************************************************
 End of function hw_usb_set_csclr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_aclrm
 Description     : The ACLRM-bit (Auto Buffer Clear Mode) is set in the specified 
                 : pipe's control register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_aclrm (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
#endif
    (*p_reg) |= USB_ACLRM;
}
/******************************************************************************
 End of function hw_usb_set_aclrm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_aclrm
 Description     : Clear the ACLRM bit in the specified pipe's control register
                 : to disable Auto Buffer Clear Mode.
                 : its BEMP interrupt status.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_aclrm (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1);
#endif
    (*p_reg) &= (~USB_ACLRM);
}
/******************************************************************************
 End of function hw_usb_clear_aclrm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_sqclr
 Description     : The SQCLR-bit (Toggle Bit Clear) is set in the specified pipe's 
                 : control register. Setting SQSET to 1 makes DATA0 the expected 
                 : data in the pipe's next transfer.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_sqclr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            USB_M0.DCPCTR.WORD |= USB_SQCLR;
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
            (*p_reg) |= USB_SQCLR;
        }
#else
        if (USB_PIPE0 == pipeno)
        {
            USB_M1.DCPCTR.WORD |= USB_SQCLR;
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
            (*p_reg) |= USB_SQCLR;
        }
#endif
}
/******************************************************************************
 End of function hw_usb_set_sqclr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_sqset
 Description     : The SQSET-bit (Toggle Bit Set) is set in the specified pipe's 
                 : control register. Setting SQSET to 1 makes DATA1 the expected 
                 : data in the next transfer.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_sqset (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            USB_M0.DCPCTR.WORD |= USB_SQSET;
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
            (*p_reg) |= USB_SQSET;
        }
#else
        if (USB_PIPE0 == pipeno)
        {
            USB_M1.DCPCTR.WORD |= USB_SQSET;
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
            (*p_reg) |= USB_SQSET;
        }
#endif
}
/******************************************************************************
 End of function hw_usb_set_sqset
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_pid
 Description     : Set the specified PID of the specified pipe's DCPCTR/PIPECTR 
                 : register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
                 : uint16_t  data   : NAK/BUF/STALL.
 Return value    : none
 ******************************************************************************/
void hw_usb_set_pid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            p_reg = ((uint16_t *)&(USB_M0.DCPCTR));
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
        }
#else
        if (USB_PIPE0 == pipeno)
        {
            p_reg = ((uint16_t *)&(USB_M1.DCPCTR));
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
        }
#endif
    (*p_reg) &= (~USB_PID);
    (*p_reg) |= data;
}
/******************************************************************************
 End of function hw_usb_set_pid
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_pid
 Description     : Clear the specified PID-bits of the specified pipe's DCPCTR/
                 : PIPECTR register.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
                 : uint16_t  data  : NAK/BUF/STALL - to be cleared.
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_pid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
        if (USB_PIPE0 == pipeno)
        {
            p_reg = ((uint16_t *)&(USB_M0.DCPCTR));
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M0.PIPE1CTR) + (pipeno - 1));
        }
#else
        if (USB_PIPE0 == pipeno)
        {
            p_reg = ((uint16_t *)&(USB_M1.DCPCTR));
        }
        else
        {
            p_reg = ((uint16_t *)&(USB_M1.PIPE1CTR) + (pipeno - 1));
        }
#endif
    (*p_reg) &= (~data);
}
/******************************************************************************
 End of function hw_usb_clear_pid
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_trenb
 Description     : The TRENB-bit (Transaction Counter Enable) is set in the speci-
                 : fied pipe's control register, to enable the counter.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_trenb (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
#endif
    (*p_reg) |= USB_TRENB;
}
/******************************************************************************
 End of function hw_usb_set_trenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_trenb
 Description     : The TRENB-bit (Transaction Counter Enable) is cleared in the 
                 : specified pipe's control register, to disable the counter.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_trenb (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
#endif
    (*p_reg) &= (~USB_TRENB);
}
/******************************************************************************
 End of function hw_usb_clear_trenb
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_trclr
 Description     : The TRENB-bit (Transaction Counter Clear) is set in the speci-
                 : fied pipe's control register to clear the current counter 
                 : value.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
 Return value    : none
 ******************************************************************************/
void hw_usb_set_trclr (usb_utr_t *ptr, uint16_t pipeno)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1TRE) + ((pipeno - 1) * 2);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1TRE) + ((pipeno - 1) * 2);
#endif
    (*p_reg) |= USB_TRCLR;
}
/******************************************************************************
 End of function hw_usb_set_trclr
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_write_pipetrn
 Description     : Specified data is written to the specified pipe's PIPETRN reg-
                 : ister.
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t  pipeno : Pipe number
                 : uint16_t  data   : The value to write.
 Return value    : none
 ******************************************************************************/
void hw_usb_write_pipetrn (usb_utr_t *ptr, uint16_t pipeno, uint16_t data)
{
    volatile uint16_t *p_reg;

#if USB_CFG_USE_USBIP == USB_CFG_IP0
    p_reg = (uint16_t *)&(USB_M0.PIPE1TRN) + ((pipeno - 1) * 2);
#else
    p_reg = (uint16_t *)&(USB_M1.PIPE1TRN) + ((pipeno - 1) * 2);
#endif
    *p_reg = data;
}
/******************************************************************************
 End of function hw_usb_write_pipetrn
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_set_suspendm
 Description     : Set SUSPM bit.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_set_suspendm(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.LPSTS.WORD |= USB_SUSPENDM;
    /* Non */
#else
    USB_M1.LPSTS.WORD |= USB_SUSPENDM;
#endif
}
/******************************************************************************
 End of function hw_usb_set_suspendm
 ******************************************************************************/

/******************************************************************************
 Function Name   : hw_usb_clear_suspm
 Description     : Clear SUSPM bit.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void hw_usb_clear_suspm(void)
{
#if USB_CFG_USE_USBIP == USB_CFG_IP0
    USB_M0.LPSTS.WORD &= (~USB_SUSPENDM);
    /* Non */
#else
    USB_M1.LPSTS.WORD &= (~USB_SUSPENDM);
#endif
}
/******************************************************************************
 End of function hw_usb_clear_suspm
 ******************************************************************************/

/******************************************************************************
 End of file
 ******************************************************************************/
