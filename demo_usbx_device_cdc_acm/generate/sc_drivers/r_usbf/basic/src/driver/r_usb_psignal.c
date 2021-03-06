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
 * File Name    : r_usb_psignal.c
 * Description  : USB Peripheral signal control code
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
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Renesas Abstracted Peripheral signal control functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_bus_reset
 Description     : A USB bus reset was issued by the host. Execute relevant pro-
                 : cessing.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_bus_reset (void)
{
    uint16_t connect_info;

    /* Bus Reset */
    usb_pstd_busreset_function();

    /* Memory clear */
    usb_pstd_clr_mem();
    connect_info = usb_cstd_port_speed(USB_NULL);

    /* Callback */
    if (USB_NULL != g_usb_pstd_driver.devdefault)
    {
        (*g_usb_pstd_driver.devdefault)(USB_NULL, connect_info, USB_NULL);
    }

    /* DCP configuration register  (0x5C) */
    hw_usb_write_dcpcfg(USB_NULL, 0);

    /* DCP maxpacket size register (0x5E) */
    hw_usb_write_dcpmxps(USB_NULL, g_usb_pstd_driver.p_devicetbl[USB_DEV_MAX_PKT_SIZE]);
}
/******************************************************************************
 End of function usb_pstd_bus_reset
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_attach_process
 Description     : USB attach setting.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_attach_process (void)
{
	usb_cpu_delay_xms((uint16_t) 10);
    hw_usb_pset_dprpu();

}
/******************************************************************************
 End of function usb_pstd_attach_process
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_detach_process
 Description     : Initialize USB registers for detaching, and call the callback
                 : function that completes the detach.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_detach_process (void)
{
    uint16_t i;

    hw_usb_clear_cnen(USB_NULL);
    /* Pull-up disable */
    hw_usb_pclear_dprpu();
    /* Configuration number */
    g_usb_pstd_config_num = 0;

    /* Remote wakeup enable flag */
    g_usb_pstd_remote_wakeup = USB_FALSE;

    /* INTSTS0 clear */
    /*g_usb_pstd_intsts0 = 0;*/

    for (i = USB_MIN_PIPE_NO; i < (USB_MAXPIPE_NUM +1); i++)
    {
        if (USB_TRUE == g_usb_pipe_table[USB_CFG_USE_USBIP][i].use_flag)
        {
            usb_pstd_forced_termination(i, (uint16_t) USB_DATA_STOP);
            usb_cstd_clr_pipe_cnfg(USB_NULL, i);
        }
    }

    /* Callback */
    if (USB_NULL != g_usb_pstd_driver.devdetach)
    {
        (*g_usb_pstd_driver.devdetach)(USB_NULL, USB_NO_ARG, USB_NULL);
    }
    usb_pstd_stop_clock();
}
/******************************************************************************
 End of function usb_pstd_detach_process
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_pstd_suspend_process
 Description     : Perform a USB peripheral suspend.
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void usb_pstd_suspend_process (void)
{
    uint16_t intsts0;
    uint16_t buf;

    /* Resume interrupt enable */
    hw_usb_pset_enb_rsme();

    intsts0 = hw_usb_read_intsts();
    buf = hw_usb_read_syssts(USB_NULL);
    if (((intsts0 & USB_DS_SUSP) != (uint16_t) 0) && ((buf & USB_LNST) == USB_FS_JSTS))
    {
        /* Suspend */
        usb_pstd_stop_clock();
        usb_pstd_suspend_function();

        /* Callback */
        if (USB_NULL != g_usb_pstd_driver.devsuspend)
        {
            (*g_usb_pstd_driver.devsuspend)(USB_NULL, g_usb_pstd_remote_wakeup, USB_NULL);
        }
    }

    /* --- SUSPEND -> RESUME --- */
    else
    {
        /* RESM status clear */
        hw_usb_pclear_sts_resm();

        /* RESM interrupt disable */
        hw_usb_pclear_enb_rsme();
    }
}
/******************************************************************************
 End of function usb_pstd_suspend_process
 ******************************************************************************/

/******************************************************************************
 End  Of File
 ******************************************************************************/
