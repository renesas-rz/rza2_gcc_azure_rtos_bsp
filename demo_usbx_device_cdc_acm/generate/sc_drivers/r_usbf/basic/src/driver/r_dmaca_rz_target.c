/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_dmaca_rx_target.c
* Device(s)    : RX
* Tool-Chain   : Renesas RXC Toolchain v2.01.00
* OS           : not use
* H/W Platform : not use
* Description  : Functions for DMACA driver
*******************************************************************************/
/*******************************************************************************
* History      : DD.MM.YYYY Version  Description
*              : 30.06.2014 1.00     Initial revision
*              : 27.08.2014 1.01     Not change program due to updating application note only.
*              : 30.01.2015 1.02     Added RX71M.
*              : 13.04.2015 1.03     Added RX231.
*******************************************************************************/

/*******************************************************************************
Includes <System Includes>, "Project Includes"
*******************************************************************************/
/* Definitions for DMACA support */
#include "r_dmaca_rz_if.h"
#include "r_dmaca_rz_private.h"
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */

/*******************************************************************************
Macro definitions
*******************************************************************************/
typedef void (* DMACI_Callback)(void);

/*******************************************************************************
Private variables and functions
*******************************************************************************/

/* DMACA DMAC0I */
static void r_dmaca_intDMAC0I_isr(void);

/* DMACA DMAC1I */
static void r_dmaca_intDMAC1I_isr(void);

/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/


/*******************************************************************************
* Function Name: r_dmaca_channel_valid_check
* Description  : Checks whether channel is valid or not.
* Arguments    : channel -
*                    Which channel to use
* Return Value : true -
*                    Channel is valid.
*                false -
*                    Channel is invalid.
*******************************************************************************/
bool r_dmaca_channel_valid_check(uint8_t channel)
{
    bool ret = false;

    switch (channel)
    {
        case DMACA_CH0:
        case DMACA_CH1:
            /* The channel number is valid. */
            ret = true;
        break;

        default:
            /* The channel number is invalid. */
            ret = false;
        break;
    }

    return ret;
}


/************************************************************************************
* Outline      : Interrupt DMAC0I handler
* Function Name: r_dmaca_intDMAC0I_isr
* Description  : Types of interrupt requests from DMACA channel 0.
*                If the pointer to function is 'FIT_NO_FUNC' or 'NULL',
*                write a program under "do something" in this
*                r_dmaca_intDMAC0I_isr() function.
* Arguments    : None
* Return Value : None
************************************************************************************/
static void r_dmaca_intDMAC0I_isr(void)
{
	if ( ((uint32_t)NULL != (uint32_t)p_DMACI_Handlers[DMACA_CH0]) )
	{
        ((DMACI_Callback)p_DMACI_Handlers[DMACA_CH0])();
    }
    else
    {
        /* do something */
    }

    return;
}


/************************************************************************************
* Outline      : Interrupt DMAC1I handler
* Function Name: r_dmaca_intDMAC1I_isr
* Description  : Types of interrupt requests from DMACA channel1.
*                If the pointer to function is 'FIT_NO_FUNC' or 'NULL',
*                write a program under "do something" in this 
*                r_dmaca_intDMAC1I_isr() function.
* Arguments    : None
* Return Value : None
************************************************************************************/
static void r_dmaca_intDMAC1I_isr(void)
{
        if (((uint32_t)NULL != (uint32_t)p_DMACI_Handlers[DMACA_CH1]))
    {
        ((DMACI_Callback)p_DMACI_Handlers[DMACA_CH1])();
    }
    else
    {
        /* do something */
    }

    return;
}

/*******************************************************************************
* Function Name: r_dmaca_int_disable_set
* Description  : Disables DMACmI interrupt. (m = 0 to 3, or 74)
* Arguments    : channel -
*                    Which channel to use
* Return Value : DMACA_SUCCESS -
*                    DMACmI interrupt is disabled successfully.
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*******************************************************************************/
dmaca_return_t r_dmaca_int_disable_set(uint8_t channel)
{

    /* Clear the DMACmI interrupt Enable bit. */
    /* Clear the DMACmI priority level. */

	e_r_drv_intc_priority_t *pri;

    switch (channel)
    {
        case DMACA_CH0:
#if USB_CFG_USE_USBIP == USB_CFG_IP0
            R_INTC_Disable(INTC_ID_USB_USBFDMA00);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA00, 0);
#else
            R_INTC_Disable(INTC_ID_USB_USBFDMA10);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA10, 0);
#endif
        break;

        case DMACA_CH1:
#if USB_CFG_USE_USBIP == USB_CFG_IP0
         	R_INTC_Disable(INTC_ID_USB_USBFDMA01);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA01, 0);
#else
            R_INTC_Disable(INTC_ID_USB_USBFDMA11);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA11, 0);
#endif
        break;

        default:
            /* The channel number is invalid. */
            return DMACA_ERR_INVALID_CH;
        break;
    }

    return DMACA_SUCCESS;
}


/*******************************************************************************
* Function Name: r_dmaca_int_enable_set
* Description  : Enables DMACmI interrupt. (m = 0 to 3, or 74)
* Arguments    : channel -
*                    Which channel to use
*              : priority -
*                    DMACmI interrupt priority level
* Return Value : DMACA_SUCCESS -
*                    DMACmI interrupt is enabled successfully.
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*******************************************************************************/
dmaca_return_t r_dmaca_int_enable_set(uint8_t channel, uint8_t priority)
{
    /* Set the DMACmI priority level. */
    /* Set the DMACmI interrupt Enable bit. */
    switch (channel)
    {
        case DMACA_CH0:
#if USB_CFG_USE_USBIP == USB_CFG_IP0
        	R_INTC_RegistIntFunc(INTC_ID_USB_USBFDMA00, r_dmaca_intDMAC0I_isr);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA00, priority);
            R_INTC_Enable(INTC_ID_USB_USBFDMA00);
#else
            R_INTC_RegistIntFunc(INTC_ID_USB_USBFDMA10, r_dmaca_intDMAC0I_isr);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA10, priority);
            R_INTC_Enable(INTC_ID_USB_USBFDMA10);
#endif
        break;

        case DMACA_CH1:
#if USB_CFG_USE_USBIP == USB_CFG_IP0
        	R_INTC_RegistIntFunc(INTC_ID_USB_USBFDMA01, r_dmaca_intDMAC1I_isr);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA01, priority);
            R_INTC_Enable(INTC_ID_USB_USBFDMA01);
#else
            R_INTC_RegistIntFunc(INTC_ID_USB_USBFDMA11, r_dmaca_intDMAC1I_isr);
            R_INTC_SetPriority(INTC_ID_USB_USBFDMA11, priority);
            R_INTC_Enable(INTC_ID_USB_USBFDMA11);
#endif
        break;

        default:
            /* The channel number is invalid. */
            return DMACA_ERR_INVALID_CH;
        break;
    }

    return DMACA_SUCCESS;
}
/*******************************************************************************
* Function Name: r_intc_dummy_read_enable
* Description  : Dummy Read the enable clear of the ID specified by the int_id
* Arguments    : uint16_t int_id   : Interrupt ID
* Return Value : GICD_ISENABLERn dummy read value
*******************************************************************************/
int32_t r_intc_dummy_read_enable(uint16_t int_id)
{
    uint32_t            icdisen;
    uint32_t            mask;
    volatile uint32_t * addr;

    /* ==== Argument check ==== */
    if (int_id >= INTC_GIC_ID_TOTAL)  //20181221 CHANGE INTC_ID_TOTAL sawada
    {
        return INTC_ERR_INVALID;        /* Argument error */ //20181221 CHANGE DEVDRV_ERROR sawada
    }

    /* GICD_ISENABLERn has 32 sources in the 32 bits        */
    /* The n can be calculated by int_id / 32               */
    /* The bit field width is 1 bit                         */
    /* The target bit can be calclated by (int_id % 32) * 1 */
    /* GICD_ICENABLERn does not effect on writing "0"       */
    /* The bits except for the target write "0"             */
    addr = (volatile uint32_t *)&INTC.GICD_ISENABLER0;
    icdisen = *(addr + (int_id / 32)); /* Read GICD_ISENABLERn   */
    icdisen = icdisen >> (int_id % 32);
    icdisen &= 0x1;

    return (int32_t)icdisen;
}

/* End of File */

