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
 * Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh0_rtos.h
 * Version      : 1.0
 * Description  : Header file USB basic
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 31.12.2018 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH0_RTOS_H
#define R_USBH0_RTOS_H

#include "r_usbh0_basic_local.h"
#if (BSP_CFG_RTOS_USED == 1)
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#endif /*(BSP_CFG_RTOS_USED == 1)*/
#include "r_os_abstraction_typedef.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/** Size of a queue **/
#define QUEUE_SIZE              (10)

/** USB task's priority **/
#define USBH0_HMGR_TSK_PRI             (8)
#define USBH0_HHUB_TSK_PRI             (10)
#define USBH0_HMSC_TSK_PRI             (10)
#define USBH0_HSTRG_TSK_PRI            (10)
#define USBH0_HHID_TSK_PRI             (10)

/** USB task stack size in words **/
#define USBH0_HMGR_STACK_SIZE          (512)
#define USBH0_HHUB_STACK_SIZE          (512)
#define USBH0_HMSC_STACK_SIZE          (512)
#define USBH0_HSTRG_STACK_SIZE         (512)
#define USBH0_HHID_STACK_SIZE          (512)

/***************************************(*******)*************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

typedef enum e_usbh0_rtos_err
{
    USBH0_RTOS_ERR_INIT_OK = 0,    // Successful
    USBH0_RTOS_ERR_INIT_MSG,       // Failure of Mailbox Creation
    USBH0_RTOS_ERR_INIT_TSK,       // Failure of Task Creation
} usbh0_rtos_err_t;

/** Declare a message handler for the created tasks **/
// QueueHandle_t g_usbh0_hmgr_msg_hdl;
// QueueHandle_t g_usbh0_hhub_msg_hdl;
// QueueHandle_t g_usbh0_hmsc_msg_hdl;
// QueueHandle_t g_usbh0_hstrg_msg_hdl;
// QueueHandle_t g_usbh0_hhid_msg_hdl;
extern p_os_msg_queue_handle_t g_usbh0_hmgr_msg_hdl;
extern p_os_msg_queue_handle_t g_usbh0_hhub_msg_hdl;
extern p_os_msg_queue_handle_t g_usbh0_hmsc_msg_hdl;
extern p_os_msg_queue_handle_t g_usbh0_hstrg_msg_hdl;
extern p_os_msg_queue_handle_t g_usbh0_hhid_msg_hdl;

usbh0_rtos_err_t usbh0_rtos_configration(void);

#endif /* R_USBH0_RTOS_H */
