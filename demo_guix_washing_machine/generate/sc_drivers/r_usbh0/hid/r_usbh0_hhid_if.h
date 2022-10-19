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
 * File Name    : r_usbh0_hhid_if.h
 * Description  : Interface file for USB Host HID class API for RX
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 30.09.2017 1.22 Deleted functions
 *                           "R_USBH0_HhidTask"
 *                           "usbh0_hhid_class_check"
 *                           "usbh0_hhid_driver_start"
 *                           "R_USBH0_HhidDriverRelease"
 *                           "usbh0_hhid_set_pipe_registration"
 *                           "R_USBH0_HhidPipeTransfer"
 *                           "R_USBH0_HhidTransferEnd"
 *                           "R_USBH0_HhidClassRequest"
 *                           "R_usbh0_hhid_device_information"
 *                           "R_USBH0_HhidChangeDeviceState"
 *                           "R_USBH0_HhidGetReportLength"
 *                           "usbh0_hhid_get_hid_protocol"
 *                           "usbh0_hhid_get_pipetbl"
 *                           "R_USBH0_HhidGetType" is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator 
 ***********************************************************************************************************************/

#ifndef USBH0_HHID_IF_H
#define USBH0_HHID_IF_H

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include <r_usbh0_hhid_config.h>
#include "r_usbh0_hhid.h"
#include "r_usbh0_hhid_typedef.h"


/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define USBH0_HHID_ECHO_MODE                  (0)   /* Host HID Application echo mode */
#define USBH0_HHID_MOUSE_KEYBOARD_MODE        (1)   /* Host HID Application mouse keyboard mode */

/* ----- HID Mouse Class Request Defines -------- */
/* HID Class Request IDs */
/* It is used by the classification judging of a class request. */
#define USBH0_HID_GET_REPORT                  (0x0100u)
#define USBH0_HID_GET_IDLE                    (0x0200u)
#define USBH0_HID_GET_PROTOCOL                (0x0300u)
#define USBH0_HID_SET_IDLE                    (0x0A00u)
#define USBH0_HID_SET_REPORT                  (0x0900u)
#define USBH0_HID_SET_PROTOCOL                (0x0B00u)
#define USBH0_HID_GET_HID_DESCRIPTOR          (0x2100u)
#define USBH0_HID_GET_REPORT_DESCRIPTOR       (0x2200u)
#define USBH0_HID_GET_PHYSICAL_DESCRIPTOR     (0x2300u)

/* ----- Protocol Codes ----- */
#define USBH0_HID_OTHER                       ((uint8_t)0x00)
#define USBH0_HID_KEYBOARD                    ((uint8_t)0x01)   /* Keyboard */
#define USBH0_HID_MOUSE                       ((uint8_t)0x02)   /* Mouse */

#define USBH0_IN                              (0u)
#define USBH0_OUT                             (1u)
#define USBH0_PIPE_DIR_IN                     (USBH0_IN)
#define USBH0_PIPE_DIR_OUT                    (USBH0_OUT)

/* API Function */
usbh0_er_t       R_USBH0_HhidGetType(uint16_t devadr, uint8_t *p_type);
uint32_t         R_USBH0_HhidGetMxps(uint16_t pipe_id);
void             R_USBH0_HhidClassCheck(uint16_t **pp_table);
void             R_USBH0_HhidTask(void);

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

#endif /* USBH0_VENDOR_IF_H */
