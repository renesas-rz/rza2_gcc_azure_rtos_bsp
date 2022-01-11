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
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/*******************************************************************************
* File Name    : r_usb_cstd_rtos.c
* Version      : 1.0
* Description  : Create tasks, mailboxes, memory pool for USB in freeRTOS.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 15.06.2016 1.00     First Release
*         : 16.11.2018 1.24     Supporting RTOS Thread safe
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"

#if defined(USB_CFG_HMSC_USE)
#include "r_usb_hmsc_if.h"
#endif /* defined(USB_CFG_HMSC_USE) */

#if defined(USB_CFG_HCDC_USE)
#include "r_usb_hcdc.h"
#endif /* defined(USB_CFG_HCDC_USE) */

#if defined(USB_CFG_HHID_USE)
#include "r_usb_hhid.h"
#endif /* defined(USB_CFG_HHID_USE) */


#include "r_usb_cstd_rtos.h"
#include "r_os_abstraction_typedef.h"
#include "r_os_abstraction_api.h"
#include "r_os_private_vector.h"
#if BSP_CFG_RTOS_USED  ==  1
#include "r_task_priority.h"
#endif /* BSP_CFG_RTOS_USED == 1 */

/******************************************************************************
 Macro definitions
 *****************************************************************************/
#define USB_TMO_VAL     (3000)

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/** Declare a task handler for the created tasks. **/
#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
static os_task_t * g_hcd_tsk_hdl;
static os_task_t * g_mgr_tsk_hdl;
  #if USB_CFG_HUB == USB_CFG_ENABLE
static os_task_t g_hub_tsk_hdl;
  #endif  /* USB_CFG_HUB == USB_CFG_ENABLE */
#endif /* ( (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST ) */

#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
/*static*/ os_task_t * g_pcd_tsk_hdl;
#endif /* ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI) */

#if defined(USB_CFG_PMSC_USE)
static os_task_t g_pmsc_tsk_hdl;
#endif /* defined(USB_CFG_PMSC_USE) */

#if defined(USB_CFG_HMSC_USE)
SemaphoreHandle_t   SemaphoreHandleRead;
#endif /* defined(USB_CFG_HMSC_USE) */

/** Declare a mailbox handler for the created tasks **/
static p_os_msg_queue_handle_t g_hcd_mbx_hdl;
static p_os_msg_queue_handle_t g_mgr_mbx_hdl;
static p_os_msg_queue_handle_t g_hub_mbx_hdl;
static p_os_msg_queue_handle_t g_pcd_mbx_hdl;
static p_os_msg_queue_handle_t g_cls_mbx_hdl;
static p_os_msg_queue_handle_t g_hmsc_mbx_hdl;
static p_os_msg_queue_handle_t g_hmsc_req_mbx_hdl;
static p_os_msg_queue_handle_t g_hcdc_mbx_hdl;
static p_os_msg_queue_handle_t g_hhid_mbx_hdl;
static p_os_msg_queue_handle_t g_pmsc_mbx_hdl;

static p_os_msg_queue_handle_t g_pipe0_hdl[USB_NUM_USBIP][USB_MAXDEVADDR];       /* USB Transfer MBX for PIPE0 wait que */
static p_os_msg_queue_handle_t g_pipe_hdl[USB_NUM_USBIP][USB_MAXPIPE_NUM + 1];   /* USB Transfer MBX for PIPE1-9 wait que */

/** Declare an array of mailbox handlers. **/
p_os_msg_queue_handle_t *g_mbx_table[] = {
    NULL,                   /* Not used */
    &g_hcd_mbx_hdl,         /* A mailbox handler of USB HCD task */
    &g_mgr_mbx_hdl,         /* A mailbox handler of USB MGR task */
    &g_hub_mbx_hdl,         /* A mailbox handler of USB HUB task */
    &g_cls_mbx_hdl,         /* A mailbox handler of USB internal communication */
    &g_pcd_mbx_hdl,         /* A mailbox handler of USB PCD task */
    &g_hmsc_mbx_hdl,        /* A mailbox handler of USB HMSC */
    &g_hmsc_req_mbx_hdl,    /* A mailbox handler for class request (via PIPE0) of USB HMSC */
    &g_hcdc_mbx_hdl,        /* A mailbox handler of USB HCDC task */
    &g_hhid_mbx_hdl,        /* A mailbox handler of USB HHID task */
    &g_pmsc_mbx_hdl,        /* A mailbox handler of USB PMSC task */
};

/** Declare a memory handler for the created tasks **/
static p_os_msg_queue_handle_t g_hcd_mpl_hdl;
static p_os_msg_queue_handle_t g_mgr_mpl_hdl;
static p_os_msg_queue_handle_t g_hub_mpl_hdl;
static p_os_msg_queue_handle_t g_cls_mpl_hdl;
static p_os_msg_queue_handle_t g_hmsc_mpl_hdl;
static p_os_msg_queue_handle_t g_hmsc_req_mpl_hdl;
static p_os_msg_queue_handle_t g_hcdc_mpl_hdl;
static p_os_msg_queue_handle_t g_hhid_mpl_hdl;
static p_os_msg_queue_handle_t g_pmsc_mpl_hdl;

/** Declare an array of memory pool handlers. **/
p_os_msg_queue_handle_t *g_mpl_table[] = {
    NULL,                   /* Not used */
    &g_hcd_mpl_hdl,         /* A memory pool handler of USB HCD task */
    &g_mgr_mpl_hdl,         /* A memory pool handler of USB MGR task */
    &g_hub_mpl_hdl,         /* A memory pool handler of USB HUB task */
    &g_cls_mpl_hdl,         /* A memory pool handler of USB internal communication */
    NULL,
    &g_hmsc_mpl_hdl,        /* A memory pool handler of USB HMSC */
    &g_hmsc_req_mpl_hdl,    /* A memory pool handler for class request (via PIPE0) of USB HMSC */
    &g_hcdc_mpl_hdl,        /* A memory pool handler of USB HCDC task */
    &g_hhid_mpl_hdl,        /* A memory pool handler of USB HHID task */
    &g_pmsc_mpl_hdl,        /* A memory pool handler of USB PMSC task */
};

/******************************************************************************
* Function Name: usb_rtos_configuration
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
usb_rtos_err_t usb_rtos_configuration(void)
{
    usb_rtos_err_t err = UsbRtos_Success;
    BaseType_t  ret;
    uint16_t ip_loop;
    uint16_t pipe_loop;
    uint16_t addr_loop;
    

    /** Create mailbox **/
    /** USB PCD task **/
    R_OS_MessageQueueCreate(&g_pcd_mbx_hdl, QUEUE_SIZE);
    if (NULL == g_pcd_mbx_hdl)
    {
        err = UsbRtos_Err_Init_Mbx;
        return err;
    }

    /** USB Tasks Creation **/
#if BSP_CFG_RTOS_USED == 1
	g_pcd_tsk_hdl = R_OS_TaskCreate("PCD_TSK",              /** Name of USB HCD task            **/
                                      usb_pstd_pcd_task,    /** Entry function of USB PMSC task  **/
                                      NULL,                  /** Task's parameter                **/
                                      PCD_STACK_SIZE,       /** Stack size in words             **/
                                      PCD_TSK_PRI          /** Task's priority                 **/
                                      );
    if (NULL == g_pcd_tsk_hdl)
    {
        err = UsbRtos_Err_Init_Tsk;
        return err;
    }
#endif // BSP_CFG_RTOS_USED == 1

//    printf("STACK:%d\r\n",uxTaskGetStackHighWaterMark((TaskHandle_t)g_pcd_tsk_hdl));

#if defined(USB_CFG_PMSC_USE)
    /** USB Tasks Creation **/
    g_pmsc_tsk_hdl = R_OS_TaskCreate("PMSC_TSK",                       /** Name of USB HCD task            **/
                                     (TaskFunction_t)usb_pmsc_task,    /** Entry function of USB PMSC task  **/
                                      NULL,                  /** Task's parameter                **/
                                      PMSC_STACK_SIZE,       /** Stack size in words             **/
                                      PMSC_TSK_PRI          /** Task's priority                 **/
                                      );
    if (NULL == g_pmsc_tsk_hdl)
    {
        err = UsbRtos_Err_Init_Tsk;
        return err;
    }

    /** Create mailbox **/
    /** USB PMSC task **/
    R_OS_MessageQueueCreate(&g_pmsc_mbx_hdl, QUEUE_SIZE);
    if (NULL == g_pmsc_mbx_hdl)
    {
        err = UsbRtos_Err_Init_Mbx;
        return err;
    }

#endif  /* defined(USB_CFG_PMSC_USE) */

    for (ip_loop = 0; ip_loop <USB_NUM_USBIP; ip_loop++)
    {
        /** Create memory pool using for pipe process **/
        for (addr_loop = 0; addr_loop <USB_MAXDEVADDR; addr_loop++)
        {
            R_OS_MessageQueueCreate(&g_pipe0_hdl[ip_loop][addr_loop],QUEUE_SIZE);
            if (NULL == g_pipe0_hdl[ip_loop][addr_loop])
            {
                err = UsbRtos_Err_Init_Mpl;
                return err;
            }
        }
        for (pipe_loop = 0; pipe_loop <(USB_MAXPIPE_NUM+1); pipe_loop++)
        {
            R_OS_MessageQueueCreate(&g_pipe_hdl[ip_loop][pipe_loop], QUEUE_SIZE);
            if (NULL == g_pipe_hdl[ip_loop][pipe_loop])
            {
                err = UsbRtos_Err_Init_Mpl;
                return err;
            }
        }
    }
    return err;
} /* End of function usb_rtos_configuration() */

/******************************************************************************
 Function Name   : usb_cstd_rec_msg
 Description     : Receive a message to the specified id (mailbox).
 Argument        : uint8_t      id      : ID number (mailbox).
                 : p_os_msg_t**  mess    : Message pointer
                 : usb_tm_t     tm      : Timeout Value
 Return          : uint16_t             : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_rec_msg (uint8_t id, p_os_msg_t** mess, usb_tm_t tm)
{
    BaseType_t err;
    p_os_msg_queue_handle_t handle;

    (void *)tm;

    if (NULL == mess)
    {
        return USB_ERROR;
    }

    handle = (p_os_msg_queue_handle_t)( *(g_mbx_table[id]) );
    *mess  = NULL;

    if (0 == tm)
    {
        tm = USB_TMO_VAL;
    }

    /** Receive message from queue specified by *(mbx_table[id]) **/
    err = R_OS_MessageQueueGet(handle, (p_os_msg_t **)mess, (uint32_t)(tm), false);
    if ((pdTRUE == err) && (NULL != (*mess)))
    {
        return USB_OK;
    }
    else
    {
        return USB_ERROR;
    }
}
/******************************************************************************
 End of function usb_cstd_rec_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_snd_msg
 Description     : Send a message to the specified id (mailbox).
 Argument        : uint8_t      id      : ID number (mailbox).
                 : p_os_msg_t*   mess    : Message pointer
 Return          : usb_er_t             : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_snd_msg (uint8_t id, p_os_msg_t* mess)
{
    BaseType_t err;
    p_os_msg_queue_handle_t handle;

    if (NULL == mess)
    {
        return USB_ERROR;
    }

    handle = (p_os_msg_queue_handle_t)( *(g_mbx_table[id]) );

    /** Send message to queue specified by *(mbx_table[id]) **/
//    err = R_OS_MessageQueuePut(handle, (const p_os_msg_t)&mess);
    err = R_OS_MessageQueuePut(handle, (const p_os_msg_t)mess);
    if (pdTRUE == err)
    {
        return USB_OK;
    }
    else
    {
        return USB_ERROR;
    }
}
/******************************************************************************
 End of function usb_cstd_snd_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_isnd_msg
 Description     : Send a message to the specified id (mailbox) while executing 
                 : an interrupt.
 Argument        : uint8_t      id      : ID number (mailbox).
                 : p_os_msg_t*   mess    : Message pointer
 Return          : usb_er_t             : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_isnd_msg (uint8_t id, p_os_msg_t* mess)
{
    BaseType_t err;
    p_os_msg_queue_handle_t handle;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    BaseType_t xHigherPriorityTaskWoken = pdTRUE;

    if (NULL == mess)
    {
        return USB_ERROR;
    }

    handle = (p_os_msg_queue_handle_t)( *(g_mbx_table[id]) );

    /** Send message to queue specified by *(mbx_table[id]) from ISR **/
//    err = R_OS_MessageQueuePut(handle, (const void *)&mess, &xHigherPriorityTaskWoken);
    err = R_OS_MessageQueuePut(handle, (const p_os_msg_t)mess);
    if (pdTRUE == err)
    {
#if BSP_CFG_RTOS_USED  ==  1
        portYIELD_FROM_ISR ( xHigherPriorityTaskWoken );
#endif // BSP_CFG_RTOS_USED  ==  1
        return USB_OK;
    }
    else
    {
        return USB_ERROR;
    }
}
/******************************************************************************
 End of function usb_cstd_isnd_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pget_blk
 Description     : Get a memory block for the caller.
 Argument        : uint8_t      id      : ID number (mailbox).
                 : usb_utr_t**  blk     : Memory block pointer.
 Return          : usb_er_t             : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_pget_blk (uint8_t id, usb_utr_t** blk)
{
    BaseType_t err;
    p_os_msg_queue_handle_t handle;

    if (NULL == blk)
    {
        return USB_ERR_NG;
    }

    handle = (p_os_msg_queue_handle_t)( *(g_mpl_table[id]) );
    *blk  = NULL;

    /* Look for a free memory block */
    err = R_OS_MessageQueueGet(handle, (p_os_msg_t)blk, (uint32_t)(0), false);
    if ((pdTRUE == err) && (NULL != (*blk)))
    {
        return USB_SUCCESS;
    }
    else
    {
        return USB_ERR_NG;
    }
}
/******************************************************************************
 End of function usb_cstd_pget_blk
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_rel_blk
 Description     : Release a memory block.
 Argument        : uint8_t      id      : ID number (mailbox).
                 : usb_utr_t*   blk     : Memory block pointer.
 Return          : usb_er_t             : USB_OK / USB_ERROR
 ******************************************************************************/
usb_er_t usb_cstd_rel_blk (uint8_t id, usb_utr_t* blk)
{
    BaseType_t err;
    p_os_msg_queue_handle_t handle;

    if (NULL == blk)
    {
        return USB_ERR_NG;
    }

    handle = (p_os_msg_queue_handle_t)( *(g_mpl_table[id]) );

    /* Release a memory block specified by id */
    err = R_OS_MessageQueuePut(handle, (const p_os_msg_t)&blk);
    if (pdTRUE == err)
    {
        return USB_SUCCESS;
    }
    else
    {
        return USB_ERR_NG;
    }
}
/******************************************************************************
 End of function usb_cstd_rel_blk
 ******************************************************************************/

#if ((USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST)
/******************************************************************************
 Function Name   : usb_cstd_wai_msg
 Description     : Runs USB_SND_MSG after running the scheduler the specified 
                 : number of times.
 Argument        : uint8_t      id      : ID number (mailbox).
                 : p_os_msg_t    *mess   : Message pointer.
                 : uint16_t     times   : Timeout value.
 Return          : usb_er_t             : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_err_t usb_cstd_wai_msg (uint8_t id, p_os_msg_t* mess, usb_tm_t times)
{
    usb_er_t err;

    R_OS_TaskSleep((uint32_t)(times/portTICK_PERIOD_MS));
    err = usb_cstd_snd_msg(id, mess);

    if (USB_OK == err)
    {
        return USB_SUCCESS;
    }
    else
    {
        return USB_ERR_NG;
    }
}
/******************************************************************************
 End of function usb_cstd_wai_msg
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_wait_scheduler
 Description     : Schedules a wait request.
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_wait_scheduler (void)
{
}
/******************************************************************************
 End of function usb_cstd_wait_scheduler
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_sche_init
 Description     : Scheduler initialization.
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_sche_init (void)
{
}
/******************************************************************************
 End of function usb_cstd_sche_init
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_set_task_pri
 Description     : Set a task's priority.
 Argument        : uint8_t tasknum   : Task id.
                 : uint8_t pri       : The task priority to be set.
 Return          : none
 ******************************************************************************/
void usb_cstd_set_task_pri (uint8_t tasknum, uint8_t pri)
{
}
/******************************************************************************
 End of function usb_cstd_set_task_pri
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_check_schedule
 Description     : Check schedule flag to see if caller's "time has come", then 
                 : clear it.
 Argument        : none
 Return          : flg   : usb_scheduler_schedule_flag
 ******************************************************************************/
uint8_t usb_cstd_check_schedule (void)
{
    return USB_FLGSET;
}
/******************************************************************************
 End of function usb_cstd_check_schedule
 ******************************************************************************/

#endif  /* (USB_CFG_MODE & USB_CFG_HOST) == USB_CFG_HOST */

/******************************************************************************
 Function Name   : usb_cstd_pipe_msg_clear
 Description     : Message clear for PIPE Transfer wait que.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipe_no    : Pipe no.
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe_msg_clear (usb_utr_t *ptr, uint16_t pipe_no)
{
    BaseType_t  err;
    p_os_msg_t   *mess;
    uint16_t    ip;

    ip = ptr->ip;

//    R_OS_Free (ptr);
	R_OS_Free((void **)ptr);
    if ((USB_MIN_PIPE_NO > pipe_no) || (USB_MAXPIPE_NUM < pipe_no))
    {
        return;
    }

    do
    {
        /** Receive message from queue specified by *(mbx_table[id]) **/
        err = R_OS_MessageQueueGet(g_pipe_hdl[ip][pipe_no], (p_os_msg_t)&mess, (uint32_t)(0), false);
        if ((pdTRUE == err) && (NULL != (mess)))
        {
//            R_OS_Free (mess);
        	R_OS_Free((void **)mess);
        }
        else
        {
            break;
        }
    } while (pdTRUE == err);
}
/******************************************************************************
 End of function usb_cstd_pipe_msg_clear
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pipe0_msg_clear
 Description     : Message clear for PIPE0 Transfer wait que.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  dev_addr   : device address
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe0_msg_clear (usb_utr_t *ptr, uint16_t dev_addr)
{
    BaseType_t  err;
    p_os_msg_t   *mess;
    uint16_t    ip;

    ip = ptr->ip;

//    vPortFree (ptr);
    R_OS_Free((void **)ptr);
    if ((USB_DEVICEADDR > dev_addr) || (USB_MAXDEVADDR < dev_addr))
    {
        return;
    }

    do
    {
        /** Receive message from queue specified by *(mbx_table[id]) **/
        err = R_OS_MessageQueueGet(g_pipe0_hdl[ip][dev_addr], (p_os_msg_t)&mess, (uint32_t)(0) ,false);
        if ((pdTRUE == err) && (NULL != (mess)))
        {
            R_OS_Free ((void **)mess);
        }
        else
        {
            break;
        }
    } while (pdTRUE == err);
}
/******************************************************************************
 End of function usb_cstd_pipe0_msg_clear
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pipe_msg_re_forward
 Description     : Get PIPE Transfer wait que and Message send to HCD/PCD
 Argument        : uint16_t  pipe_no    : Pipe no.
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe_msg_re_forward (uint16_t ip_no, uint16_t pipe_no)
{
    BaseType_t      err;
    p_os_msg_t       *mess;
    p_os_msg_queue_handle_t   handle;
    uint8_t         id;

    if ((USB_MIN_PIPE_NO > pipe_no) || (USB_MAXPIPE_NUM < pipe_no))
    {
        return;
    }

    /** Receive message from queue specified by *(mbx_table[id]) **/
    err = R_OS_MessageQueueGet(g_pipe_hdl[ip_no][pipe_no], (p_os_msg_t)&mess, (uint32_t)0, false);
    if ((pdTRUE == err) && (NULL != (mess)))
    {
    	id = USB_PCD_MBX;

        handle = (p_os_msg_queue_handle_t)(*(g_mbx_table[id]));

        /** Send message to queue specified by *(mbx_table[id]) **/
        R_OS_MessageQueuePut(handle, (const p_os_msg_t)&mess);
    }
}
/******************************************************************************
 End of function usb_cstd_pipe_msg_re_forward
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pipe0_msg_re_forward
 Description     : Get PIPE0 Transfer wait que and Message send to HCD/PCD
 Argument        : none
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe0_msg_re_forward (uint16_t ip_no)
{
    BaseType_t      err;
    p_os_msg_t       *mess;
    p_os_msg_queue_handle_t   handle;
    uint8_t         id;
    uint8_t         dev_addr;

    for (dev_addr =0; dev_addr <USB_MAXDEVADDR; dev_addr++)
    {
        /** Receive message from queue specified by *(mbx_table[id]) **/
        err = R_OS_MessageQueueGet(g_pipe0_hdl[ip_no][dev_addr], (void *)&mess, (uint32_t)(0), false);
        if ((pdTRUE == err) && (NULL != (mess)))
        {
            id = USB_PCD_MBX;

            handle = (p_os_msg_queue_handle_t)(*(g_mbx_table[id]));

            /** Send message to queue specified by *(mbx_table[id]) **/
            R_OS_MessageQueuePut(handle, (const p_os_msg_t)&mess);
            return;
        }
    }
}
/******************************************************************************
 End of function usb_cstd_pipe0_msg_re_forward
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pipe_msg_forward
 Description     : Message foward to PIPE Transfer wait que.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  pipe_no    : Pipe no.
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe_msg_forward (usb_utr_t *ptr, uint16_t pipe_no)
{
    if ((USB_MIN_PIPE_NO > pipe_no) || (USB_MAXPIPE_NUM < pipe_no))
    {
        return;
    }

    /** Send message to queue specified by *(mbx_table[id]) **/
    R_OS_MessageQueuePut(g_pipe_hdl[ptr->ip][pipe_no], (const p_os_msg_t)&ptr);
}
/******************************************************************************
 End of function usb_cstd_pipe_msg_forward
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_pipe0_msg_forward
 Description     : Message foward to PIPE0 Transfer wait que.
 Arguments       : usb_utr_t *ptr       : Pointer to usb_utr_t structure.
                 : uint16_t  dev_addr   : device address
 Return          : none
 ******************************************************************************/
void usb_cstd_pipe0_msg_forward (usb_utr_t *ptr, uint16_t dev_addr)
{
    if ((USB_DEVICEADDR > dev_addr) || (USB_MAXDEVADDR < dev_addr))
    {
        return;
    }

    /** Send message to queue specified by *(mbx_table[id]) **/
    R_OS_MessageQueuePut(g_pipe0_hdl[ptr->ip][dev_addr], (const p_os_msg_t)&ptr);
}
/******************************************************************************
 End of function usb_cstd_pipe0_msg_forward
 ******************************************************************************/

