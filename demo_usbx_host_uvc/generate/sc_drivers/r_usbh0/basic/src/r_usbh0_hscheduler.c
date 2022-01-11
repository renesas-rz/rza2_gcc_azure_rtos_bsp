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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usbh0_cscheduler.c
* Description  : USB Host scheduler code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

#include "r_usbh0_basic_local.h"
#if (BSP_CFG_RTOS_USED == 1)
#include "r_usbh0_rtos.h"
#endif /* (BSP_CFG_RTOS_USED == 1) */

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/
extern void     usbh0_hstd_int_enable(void);
extern void     usbh0_hstd_int_disable(void);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

usbh0_er_t usbh0_hstd_rtos_snd_msg (uint8_t id, usbh0_msg_t* mess);
usbh0_er_t usbh0_hstd_rtos_rec_msg (uint8_t id, usbh0_msg_t** mess, uint16_t tm);

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
/* Priority Table */
static usbh0_msg_t      *gsp_usbh0_hstd_table_addr[USBH0_PRIMAX][USBH0_TABLEMAX];
static uint8_t          gs_usbh0_hstd_table_id[USBH0_PRIMAX][USBH0_TABLEMAX];
static uint8_t          gs_usbh0_hstd_pri_r[USBH0_PRIMAX];
static uint8_t          gs_usbh0_hstd_pri_w[USBH0_PRIMAX];
static uint8_t          gs_usbh0_hstd_pri[USBH0_IDMAX];

/* Schedule Set Flag  */
static uint8_t          gs_usbh0_hstd_schedule_flag;

/* Fixed-sized memory pools */
static st_usbh0_utr_t   gs_usbh0_hstd_block[USBH0_BLKMAX];
static uint8_t          gs_usbh0_hstd_blk_flg[USBH0_BLKMAX];

static usbh0_msg_t      *gsp_usbh0_hstd_add_use;
static uint8_t          gs_usbh0_hstd_id_use;

/* Wait MSG */
static usbh0_msg_t      *gsp_usbh0_hstd_wait_add[USBH0_IDMAX];
static uint16_t         gs_usbh0_hstd_wait_counter[USBH0_IDMAX];

#if (BSP_CFG_RTOS_USED == 1)
/** Declare an array of mailbox handlers. **/
p_os_msg_queue_handle_t *g_usbh0_msg_table[] = {
    &g_usbh0_hmgr_msg_hdl,         // A message handler of USB HCI task ... MGR Handler
    &g_usbh0_hmgr_msg_hdl,         // A message handler of USB MGR task
    &g_usbh0_hhub_msg_hdl,         // A message handler of USB HUB task
    NULL,                     // Not used
    NULL,                     // Not used
    NULL,                     // Not used
    &g_usbh0_hmsc_msg_hdl,         // A message handler of USB
    &g_usbh0_hstrg_msg_hdl,        // A message handler of USB
    NULL,         // Not used
    &g_usbh0_hhid_msg_hdl,         // A message handler of USB
    NULL,                     // Not used
    NULL,                     // Not used
    NULL,                     // Not used
    NULL,                     // Not used
	NULL,                     // Not used
};
#endif /* (BSP_CFG_RTOS_USED == 1) */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_rec_msg
Description     : Receive a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t** mess  : Message pointer
Return          : uint16_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_rec_msg(uint8_t id, usbh0_msg_t **pp_mess)
{
    if ((id < USBH0_IDMAX) && (gs_usbh0_hstd_id_use < USBH0_IDMAX))
    {
        if(gs_usbh0_hstd_id_use == id)
        {
            *pp_mess = gsp_usbh0_hstd_add_use;
            return  USBH0_OK;
        }
    }

    return  USBH0_ERROR;
} /* End of function usbh0_hstd_rec_msg() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdRecMsg
Description     : Receive a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t** mess  : Message pointer
Return          : uint16_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdRecMsg(uint8_t id, usbh0_msg_t **pp_mess)
{
    usbh0_er_t  err;

    err = usbh0_hstd_rec_msg(id,  pp_mess);

    return err;
} /* End of function R_USBH0_CstdRecMsg() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_isnd_msg
Description     : Send a message to the specified id (mailbox) while executing
                : an interrupt.
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t* mess   : Message pointer
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_isnd_msg(uint8_t id, usbh0_msg_t *p_mess)
{
    uint8_t     usb_pri;    /* Task Priority */
    uint8_t     usb_write;  /* Priority Table Writing pointer */

    if(id < USBH0_IDMAX)
    {
        /* Read priority and table pointer */
        usb_pri     = gs_usbh0_hstd_pri[id];
        usb_write   = gs_usbh0_hstd_pri_w[usb_pri];
        if(usb_pri < USBH0_PRIMAX)
        {
            /* Renewal write pointer */
            usb_write++;
            if(usb_write >= USBH0_TABLEMAX)
            {
                usb_write = USBH0_TBLCLR;
            }
            /* Check pointer */
            if(usb_write == gs_usbh0_hstd_pri_r[usb_pri])
            {
                return  USBH0_ERROR;
            }
            /* Save message */
            /* Set priority table */
            gs_usbh0_hstd_table_id[usb_pri][usb_write]    = id;
            gsp_usbh0_hstd_table_addr[usb_pri][usb_write] = p_mess;
            gs_usbh0_hstd_pri_w[usb_pri]                  = usb_write;
            return  USBH0_OK;
        }
    }

    USBH0_PRINTF0("SND_MSG ERROR !!\n");

    return  USBH0_ERROR;
} /* End of function usbh0_hstd_isnd_msg() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdiSndMsg
Description     : Send a message to the specified id (mailbox) while executing
                : an interrupt.
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t* mess   : Message pointer
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdiSndMsg(uint8_t id, usbh0_msg_t *p_mess)
{
    usbh0_er_t    err;

    err = usbh0_hstd_isnd_msg(id, p_mess);

    return err;
} /* End of function R_USBH0_CstdiSndMsg() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_snd_msg
Description     : Send a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t* mess   : Message pointer
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_snd_msg(uint8_t id, usbh0_msg_t *p_mess)
{
    usbh0_er_t  status;

    usbh0_hstd_int_disable();     /* USBH0_ interrupt disable */

    status = usbh0_hstd_isnd_msg(id, p_mess);

    usbh0_hstd_int_enable();      /* USBH0_ interrupt enable */

    return status;
} /* End of function usbh0_hstd_snd_msg() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdSndMsg
Description     : Send a message to the specified id (mailbox).
Argument        : uint8_t id        : ID number (mailbox).
                : usbh0_msg_t* mess   : Message pointer
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdSndMsg(uint8_t id, usbh0_msg_t *p_mess)
{
    usbh0_er_t  status;

    status = usbh0_hstd_snd_msg(id, p_mess);

    return status;
} /* End of function R_USBH0_CstdSndMsg() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_pget_blk
Description     : Get a memory block for the caller.
Argument        : uint8_t id        : ID number (mailbox).
                : st_usbh0_utr_t** blk   : Memory block pointer.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_pget_blk(uint8_t id, st_usbh0_utr_t **pp_blk)
{
    uint8_t     usb_s_pblk_c;

    if(id < USBH0_IDMAX)
    {
        usb_s_pblk_c = USBH0_CNTCLR;

        while(USBH0_BLKMAX != usb_s_pblk_c)
        {
            if(USBH0_FLGCLR == gs_usbh0_hstd_blk_flg[usb_s_pblk_c])
            {
                /* Acquire fixed-size memory block */
                *pp_blk = &gs_usbh0_hstd_block[usb_s_pblk_c];
                gs_usbh0_hstd_blk_flg[usb_s_pblk_c] = USBH0_FLGSET;

                return  USBH0_OK;
            }
            usb_s_pblk_c++;
        }
        /* Error of BLK Table Full !!  */
        USBH0_PRINTF1("usbh0_scBlkFlg[%d][] Full !!\n",id);
    }
    return USBH0_ERROR;
} /* End of function usbh0_hstd_pget_blk() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdPgetBlk
Description     : Call the get memory block function to get a memory block for
                : the caller.
Argument        : uint8_t id        : ID number (mailbox).
                : st_usbh0_utr_t** blk   : Memory block pointer.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdPgetBlk(uint8_t id, st_usbh0_utr_t **pp_blk)
{
    usbh0_er_t  err;

    err = usbh0_hstd_pget_blk(id, pp_blk);

    return err;
} /* End of function R_USBH0_CstdPgetBlk() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_rel_blk
Description     : Release a memory block.
Argument        : uint8_t id        : ID number (mailbox).
                : st_usbh0_utr_t* blk    : Memory block pointer.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_rel_blk(uint8_t id, st_usbh0_utr_t *p_blk)
{
    uint16_t    usb_s_rblk_c;
    
    if(id < USBH0_IDMAX)
    {
        usb_s_rblk_c = USBH0_CNTCLR;
        while(USBH0_BLKMAX != usb_s_rblk_c)
        {
            if(p_blk == &gs_usbh0_hstd_block[usb_s_rblk_c])
            {
                /* Release fixed-size memory block */
                gs_usbh0_hstd_blk_flg[usb_s_rblk_c] = USBH0_FLGCLR;

                return  USBH0_OK;
            }
            usb_s_rblk_c++;
        }
        /* Error of BLK Flag is not CLR !!  */
        USBH0_PRINTF0("TskBlk NO CLR !!\n");
    }
    return USBH0_ERROR;
} /* End of function usbh0_hstd_rel_blk() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdRelBlk
Description     : Call the function to release a memory block.
Argument        : uint8_t id        : ID number (mailbox).
                : st_usbh0_utr_t* blk    : Memory block pointer.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdRelBlk(uint8_t id,  st_usbh0_utr_t *p_blk)
{
    usbh0_er_t  err;

    err = usbh0_hstd_rel_blk(id, p_blk);

    return err;
} /* End of function R_USBH0_CstdRelBlk() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_wai_msg
Description     : Runs USBH0_SND_MSG after running the scheduler the specified
                : number of times.
Argument        : uint8_t  id       : ID number (mailbox).
                : usbh0_msg_t *mess   : Message pointer.
                : uint16_t  times   : Timeout value.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR.
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_wai_msg(uint8_t id, usbh0_msg_t *p_mess, uint16_t times)
{
    if(id < USBH0_IDMAX)
    {
        if(0 == gs_usbh0_hstd_wait_counter[id])
        {
            gsp_usbh0_hstd_wait_add[id]       = p_mess;
            gs_usbh0_hstd_wait_counter[id]    = times;
            return  USBH0_OK;
        }
    }

    /* Error !!  */
    USBH0_PRINTF0("WAI_MSG ERROR !!\n");

    return USBH0_ERROR;
} /* End of function usbh0_hstd_wai_msg() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdWaiMsg
Description     : Will run USBH0_SND_MSG after running the scheduler the specified
                : number of times.
Argument        : uint8_t  id       : ID number (mailbox).
                : usbh0_msg_t *mess   : Message pointer.
                : uint16_t  times   : Timeout value.
Return          : usbh0_er_t          : USBH0_OK / USBH0_ERROR.
***********************************************************************************************************************/
usbh0_er_t R_USBH0_CstdWaiMsg(uint8_t id, usbh0_msg_t *p_mess, uint16_t times)
{
    usbh0_er_t  err;

    err = usbh0_hstd_wai_msg(id, p_mess, times);

    return err;
} /* End of function R_USBH0_CstdWaiMsg() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_wait_scheduler
Description     : Schedules a wait request.
Argument        : none
Return          : none
***********************************************************************************************************************/
void usbh0_hstd_wait_scheduler(void)
{
    usbh0_er_t  err;
    uint8_t     id;

    for (id=0; id < USBH0_IDMAX; id++)
    {
        if (0 != gs_usbh0_hstd_wait_counter[id])
        {
            gs_usbh0_hstd_wait_counter[id]--;
            if (0 == gs_usbh0_hstd_wait_counter[id])
            {
                err = usbh0_hstd_snd_msg(id, gsp_usbh0_hstd_wait_add[id]);
                if (USBH0_OK != err)
                {
                    gs_usbh0_hstd_wait_counter[id]++;
                }
            }
        }
    }
} /* End of function usbh0_hstd_wait_scheduler() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_sche_init
Description     : Scheduler initialization.
Argument        : none
Return          : none
***********************************************************************************************************************/
void usbh0_hstd_sche_init(void)
{
    uint8_t     i;
    uint8_t     j;

    /* Initial Scheduler */
    gs_usbh0_hstd_id_use          = 0;
    gs_usbh0_hstd_schedule_flag   = 0;

    /* Initialize  priority table pointer and priority table */
    for (i = 0; i != USBH0_PRIMAX; i++)
    {
        gs_usbh0_hstd_pri_r[i] = 0;
        gs_usbh0_hstd_pri_w[i] = 0;
        for (j = 0; j != USBH0_TABLEMAX; j++)
        {
            gs_usbh0_hstd_table_id[i][j] = USBH0_IDMAX;
        }
    }

    /* Initialize block table */
    for (i = 0; i != USBH0_BLKMAX; i++)
    {
        gs_usbh0_hstd_blk_flg[i] = 0;
    }

    /* Initialize priority */
    for (i = 0; i != USBH0_IDMAX; i++)
    {
        gs_usbh0_hstd_pri[i]          = (uint8_t)USBH0_IDCLR;
        gsp_usbh0_hstd_wait_add[i]    = (usbh0_msg_t *)USBH0_NULL;
        gs_usbh0_hstd_wait_counter[i] = 0;
    }
} /* End of function usbh0_hstd_sche_init() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdScheduler
Description     : The scheduler.
Argument        : none
Return          : none
***********************************************************************************************************************/
void R_USBH0_CstdScheduler(void)
{
    uint8_t     usb_pri;        /* Priority Counter */
    uint8_t     usb_read;       /* Priority Table read pointer */

#if(BSP_CFG_RTOS_USED == 1)
    if (0 == g_usbh0_msg_check)
    {
#endif /* (BSP_CFG_RTOS_USED == 1) */
        /* wait msg */
        usbh0_hstd_wait_scheduler();

        /* Priority Table reading */
        usb_pri = USBH0_CNTCLR;
        while (usb_pri < USBH0_PRIMAX)
        {
            usb_read = gs_usbh0_hstd_pri_r[usb_pri];
            if (usb_read != gs_usbh0_hstd_pri_w[usb_pri])
            {
                /* Priority Table read pointer increment */
                usb_read++;
                if (usb_read >= USBH0_TABLEMAX)
                {
                    usb_read = USBH0_TBLCLR;
                }
                /* Set practise message */
                gs_usbh0_hstd_id_use                        = gs_usbh0_hstd_table_id[usb_pri][usb_read];
                gsp_usbh0_hstd_add_use                      = gsp_usbh0_hstd_table_addr[usb_pri][usb_read];
                gs_usbh0_hstd_table_id[usb_pri][usb_read]   = USBH0_IDMAX;
                gs_usbh0_hstd_pri_r[usb_pri]                = usb_read;
                gs_usbh0_hstd_schedule_flag                 = USBH0_FLGSET;

                usb_pri = USBH0_PRIMAX;

#if (BSP_CFG_RTOS_USED == 1)
                g_usbh0_msg_check |= (1<<gs_usbh0_hstd_id_use);
                USBH0_RTOS_SND_MSG (gs_usbh0_hstd_id_use, gsp_usbh0_hstd_add_use);
#endif /* (BSP_CFG_RTOS_USED == 1) */
            }
            else
            {
                usb_pri++;
            }
        }
#if(BSP_CFG_RTOS_USED == 1)
    }
#endif /* (BSP_CFG_RTOS_USED == 1) */
} /* End of function R_USBH0_CstdScheduler() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdSetTaskPri
Description     : Set a task's priority.
Argument        : uint8_t tasknum   : Task id.
                : uint8_t pri       : The task priority to be set.
Return          : none
***********************************************************************************************************************/
void R_USBH0_CstdSetTaskPri(uint8_t tasknum, uint8_t pri)
{
    if (tasknum < USBH0_IDMAX)
    {
        if (pri < USBH0_PRIMAX)
        {
            gs_usbh0_hstd_pri[tasknum] = pri;
        }
        else if (USBH0_IDCLR == pri)
        {
            gs_usbh0_hstd_pri[tasknum] = USBH0_IDCLR;
        }
        else
        {
        }
    }
} /* End of function R_USBH0_CstdSetTaskPri() */

/***********************************************************************************************************************
Function Name   : R_USBH0_CstdCheckSchedule
Description     : Check schedule flag to see if caller's "time has come", then
                : clear it.
Argument        : none
Return          : flg   : gs_usbh0_hstd_schedule_flag
***********************************************************************************************************************/
uint8_t R_USBH0_CstdCheckSchedule(void)
{
    uint8_t     flg;

    flg = gs_usbh0_hstd_schedule_flag;
    gs_usbh0_hstd_schedule_flag = USBH0_FLGCLR;

    return flg;
} /* End of function R_USBH0_CstdCheckSchedule() */

#if (BSP_CFG_RTOS_USED == 1)
/***********************************************************************************************************************
Function Name   : usbh0_hstd_rtos_snd_msg
Description     :
Argument        :
Return          :
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_rtos_snd_msg (uint8_t id, usbh0_msg_t* mess)
{
    BaseType_t err;
    QueueHandle_t handle;

    if ((NULL == mess) && (USBH0_TID_0 != id))
    {
        return USBH0_ERROR;
    }

    handle = (QueueHandle_t)( *(g_usbh0_msg_table[id]) );
    if(NULL == handle)
    {
        return USBH0_ERROR;
    }

    /** Send message to queue specified by *(msg_table[id]) **/
    err = xQueueSend(handle, (const void *)&mess, (TickType_t)(0));
    if (pdTRUE == err)
    {
        return USBH0_OK;
    }
    else
    {
        return USBH0_ERROR;
    }
}

/***********************************************************************************************************************
Function Name   : usbh0_hstd_rtos_rec_msg
Description     :
Argument        :
Return          :
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_rtos_rec_msg (uint8_t id, usbh0_msg_t** mess, uint16_t tm)
{
    BaseType_t err = pdFALSE;
    QueueHandle_t handle;

    (void *)tm;

    if (NULL == mess)
    {
        return USBH0_ERROR;
    }

    handle = (QueueHandle_t)( *(g_usbh0_msg_table[id]) );
    *mess  = NULL;

    /** Receive message from queue specified by *(mbx_table[id]) **/
    if (0 == tm)
    {
        tm = portMAX_DELAY;
    }

    /* err = xQueueReceive(handle, (void *)mess, (TickType_t)(tm)); */
    while( pdFALSE == err )
    {
    	err = xQueueReceive(handle, (void *)mess, (TickType_t)(tm));
    }

    if (USBH0_HCI_TSK == id && NULL == *mess)
    {
        return USBH0_OK;
    }
    if ((pdTRUE == err) && (NULL != (*mess)))
    {
        return USBH0_OK;
    }
    else
    {
        return USBH0_ERROR;
    }
}

#endif /* (BSP_CFG_RTOS_USED == 1) */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
