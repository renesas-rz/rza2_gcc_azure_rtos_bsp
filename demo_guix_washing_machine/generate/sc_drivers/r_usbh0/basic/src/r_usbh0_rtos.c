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
* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_usb_rtos.c
* Version      : 1.0
* Description  : Create tasks, mailboxes, memory pool for USB in freeRTOS.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 31.12.2018 1.00     First Release
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usbh0_basic_local.h"
#if (BSP_CFG_RTOS_USED == 1)
#include "r_usbh0_rtos.h"
#include "r_usbh0_basic_if.h"
#if USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE
#include "r_usbh0_hmsc_if.h"
#endif /* USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE */
#if USBH0_CFG_CLASS == USBH0_CFG_HHID_USE
#include "r_usbh0_hhid_if.h"
#endif /* USBH0_CFG_CLASS == USBH0_CFG_HHID_USE */
#include "r_os_abstraction_typedef.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/** Declare a task handler for the created tasks. **/
static TaskHandle_t gs_usbh0_hmgr_tsk_hdl;
static TaskHandle_t gs_usbh0_hhub_tsk_hdl;
#if USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE
static TaskHandle_t gs_usbh0_hmsc_tsk_hdl;
static TaskHandle_t gs_usbh0_hstrg_tsk_hdl;
SemaphoreHandle_t    SemaphoreHandleRead;
#endif /*USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE*/
#if USBH0_CFG_CLASS == USBH0_CFG_HHID_USE
static TaskHandle_t gs_usbh0_hhid_tsk_hdl;
#endif /*USBH0_CFG_CLASS == USBH0_CFG_HHID_USE*/

p_os_msg_queue_handle_t g_usbh0_hmgr_msg_hdl;
p_os_msg_queue_handle_t g_usbh0_hhub_msg_hdl;
p_os_msg_queue_handle_t g_usbh0_hmsc_msg_hdl;
p_os_msg_queue_handle_t g_usbh0_hstrg_msg_hdl;
p_os_msg_queue_handle_t g_usbh0_hhid_msg_hdl;



/******************************************************************************
* Function Name: usbh0_rtos_configration
* Description  : Create mailboxes, memory pool using for the created tasks
*                in usb system.
* Arguments    : None
* Return Value : UsbRtos_Success -
*                   Successful.
*                UsbRtos_Err_Init_Mbx -
*                   Failed in mailbox allocation.
*                UsbRtos_Err_Init_Mpl -
*                   Failed in memory pool allocation.
******************************************************************************/
usbh0_rtos_err_t r_usbh0_rtos_configration(void)
{
    BaseType_t  ret;

    /** Create mailbox for each task. **/
    /** USB MGR task **/
    g_usbh0_hmgr_msg_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_usbh0_hmgr_msg_hdl)
    {
        return USBH0_RTOS_ERR_INIT_MSG;
    }

    /** USB HUB task **/
    g_usbh0_hhub_msg_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_usbh0_hhub_msg_hdl)
    {
        return USBH0_RTOS_ERR_INIT_MSG;
    }

#if USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE
    /** USB MSC task **/
    g_usbh0_hmsc_msg_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_usbh0_hmsc_msg_hdl)
    {
        return USBH0_RTOS_ERR_INIT_MSG;
    }

    /** USB String task **/
    g_usbh0_hstrg_msg_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_usbh0_hstrg_msg_hdl)
    {
        return USBH0_RTOS_ERR_INIT_MSG;
    }
#endif /*USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE*/

#if USBH0_CFG_CLASS == USBH0_CFG_HHID_USE
    /** USB MSC task **/
    g_usbh0_hhid_msg_hdl = xQueueCreate(QUEUE_SIZE, sizeof(void *));
    if (NULL == g_usbh0_hhid_msg_hdl)
    {
        return USBH0_RTOS_ERR_INIT_MSG;
    }
#endif /*USBH0_CFG_CLASS == USBH0_CFG_HHID_USE*/

    /** USB Tasks Creation **/
    ret = xTaskCreate(
        (TaskFunction_t)R_USBH0_HstdMgrTask,  /** Entry function of USB HMGR task  **/
        "USBH0_HMGR_TSK",                     /** Name of USB HMGR task            **/
        USBH0_HMGR_STACK_SIZE,                /** Stack size in words              **/
        NULL,                                 /** Task's parameter                 **/
        USBH0_HMGR_TSK_PRI,                   /** Task's priority                  **/
        &gs_usbh0_hmgr_tsk_hdl);              /** Task handler for use later       **/
    if (pdPASS != ret)
    {
        return USBH0_RTOS_ERR_INIT_TSK;
    }

    ret = xTaskCreate(
        (TaskFunction_t)R_USBH0_HhubTask,     /** Entry function of USB HHUB task  **/
        "USBH0_HHUB_TSK",                     /** Name of USB HHUB task            **/
        USBH0_HHUB_STACK_SIZE,                /** Stack size in words              **/
        NULL,                                 /** Task's parameter                 **/
        USBH0_HHUB_TSK_PRI,                   /** Task's priority                  **/
        &gs_usbh0_hhub_tsk_hdl);              /** Task handler for use later       **/
    if (pdPASS != ret)
    {
        return USBH0_RTOS_ERR_INIT_TSK;
    }

#if USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE
    ret = xTaskCreate(
        (TaskFunction_t)R_USBH0_HmscTask,     /** Entry function of USB HMSC task  **/
        "USBH0_HMSC_TSK",                     /** Name of USB HMSC task            **/
        USBH0_HMSC_STACK_SIZE,                /** Stack size in words              **/
        NULL,                                 /** Task's parameter                 **/
        USBH0_HMSC_TSK_PRI,                   /** Task's priority                  **/
        &gs_usbh0_hmsc_tsk_hdl);              /** Task handler for use later       **/
    if (pdPASS != ret)
    {
        return USBH0_RTOS_ERR_INIT_TSK;
    }

    ret = xTaskCreate(
        (TaskFunction_t)R_USBH0_HmscStrgDriveTask,     /** Entry function of USB HSTRG task  **/
        "USBH0_HSTRG_TSK",                             /** Name of USB HSTRG task            **/
        USBH0_HSTRG_STACK_SIZE,                        /** Stack size in words               **/
        NULL,                                          /** Task's parameter                  **/
        USBH0_HSTRG_TSK_PRI,                           /** Task's priority                   **/
        &gs_usbh0_hstrg_tsk_hdl);                      /** Task handler for use later        **/
    if (pdPASS != ret)
    {
        return USBH0_RTOS_ERR_INIT_TSK;
    }
#endif /* USBH0_CFG_CLASS == USBH0_CFG_HMSC_USE*/

#if USBH0_CFG_CLASS == USBH0_CFG_HHID_USE
    ret = xTaskCreate(
        (TaskFunction_t)R_USBH0_HhidTask,     /** Entry function of USB HHID task  **/
        "USBH0_HHID_TSK",                     /** Name of USB HHID task            **/
        USBH0_HHID_STACK_SIZE,                /** Stack size in words              **/
        NULL,                                 /** Task's parameter                 **/
        USBH0_HHID_TSK_PRI,                   /** Task's priority                  **/
        &gs_usbh0_hhid_tsk_hdl);              /** Task handler for use later       **/
    if (pdPASS != ret)
    {
        return USBH0_RTOS_ERR_INIT_TSK;
    }
#endif /*defined(USBH0_CFG_HHID_USE)*/

    return USBH0_RTOS_ERR_INIT_OK;

} /* End of function usb_rtos_configuration() */
#endif /* (BSP_CFG_RTOS_USED == 1) */
