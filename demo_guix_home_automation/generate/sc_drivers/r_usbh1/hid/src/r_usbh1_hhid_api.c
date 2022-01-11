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
 * Copyright (C) 2014(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh1_hhid_api.c
 * Description  : Host HID class driver code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 30.09.2017 1.22 "R_USBH1_HhidDriverRelease","R_USBH1_HhidTransferEnd","R_USBH1_HhidChangeDeviceState"
 *                           "R_USBH1_HhidGetReportLength","R_USBH1_HhidClassRequest","R_USBH1_HhidPipeTransfer" is deleted.
 *         : 31.03.2018 1.23 Supporting Smart Configurator 
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usbh1_basic_if.h"
#include "r_usbh1_hhci_local.h"
#include "r_usbh1_hhid_if.h"
#include "r_usbh1_hhid.h"

/******************************************************************************
 Renesas Abstracted USB Driver functions
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USBH1_HhidGetType
Description     : Get HID protocol.(USB Mouse/USB Keyboard/Protocon non.)
Arguments       : usbh1_ctrl_t *p_ctrl    : Pointer to usbh1_ctrl_t structure
                : uint8_t *p_type       : Address for stored HID protocol value.
Return value    : usbh1_er_t error code  : USBH1_SUCCESS,USBH1_ERR_NG etc.
******************************************************************************/
usbh1_er_t R_USBH1_HhidGetType(uint16_t devadr, uint8_t *p_type)
{
    usbh1_er_t       err;
    usbh1_info_t     info;
    st_usbh1_utr_t   utr;
	
    if (USBH1_NULL == p_type)
    {
        return USBH1_ERR_PARA;
    }

    if(	USBH1_CONFIGURED == g_usbh1_hstd_mgr_mode[0] )
    {
        return USBH1_ERR_NG;
    }
    *p_type = usbh1_hhid_get_hid_protocol( devadr );
    return USBH1_SUCCESS;

}
/******************************************************************************
 End of function R_USBH1_HhidGetType
 ******************************************************************************/


/******************************************************************************
Function Name   : R_USBH1_HhidGetMxps
Description     : Obtains max packet size for the connected HID device
                : The max packet size is set to the area
                : Set the direction (USBH1_IN/USBH1_OUT)
Arguments       : usbh1_ctrl_t *p_ctrl    : Pointer to usbh1_ctrl_t structure area
                : uint16_t *p_mxps      : Pointer to the area to store the max package size
                : uint8_t dir           : Transfer direction
Return value    : usbh1_er_t error code  : USBH1_SUCCESS,USBH1_ERR_NG etc.
******************************************************************************/
uint32_t R_USBH1_HhidGetMxps(uint16_t pipe_id)
{
    return ( R_USBH1_HstdGetMaxPacketSize(pipe_id) );
}
/******************************************************************************
 End of function R_USBH1_HhidGetMxps
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USBH1_HhidClassCheck
Description     : check connected device
Arguments       : uint16_t **table :
Return value    : none
******************************************************************************/
void R_USBH1_HhidClassCheck(uint16_t **pp_table)
{
	usbh1_hhid_class_check (pp_table);
}   /* eof R_USBH1_HhidClassCheck() */


/******************************************************************************
Function Name   : R_USBH1_HhidTask
Description     : USB HMSC Task
Arguments       : none
Return value    : none
******************************************************************************/
void R_USBH1_HhidTask(void)
{
#if (BSP_CFG_RTOS_USED == 1)  //* not DEL *//
	st_usbh1_utr_t  *p_mess;
	while(1)
	{
		USBH1_RTOS_RCV_MSG(USBH1_HHID_MSG, (usbh1_msg_t **)&p_mess);
		usbh1_hhid_task();
		g_usbh1_msg_check &= ~(1<<USBH1_HHID_TSK);
	}
#else /* (BSP_CFG_RTOS_USED == 1) */
	usbh1_hhid_task();
#endif /* (BSP_CFG_RTOS_USED == 1) */
}   /* eof R_USBH1_HhidTask() */




/******************************************************************************
 End  Of File
 ******************************************************************************/
