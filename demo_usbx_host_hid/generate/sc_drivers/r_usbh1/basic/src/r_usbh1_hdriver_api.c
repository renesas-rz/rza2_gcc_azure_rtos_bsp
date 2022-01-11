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
* File Name    : r_usbh1_hdriverapi.c
* Description  : USB Host Control Driver API. USB Host transfer level commands.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh1_basic_local.h"
#include "r_usbh1_hhci.h"
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */

#if (BSP_CFG_RTOS_USED == 1)
#include "r_os_abstraction_api.h"
#include "r_usbh1_rtos.h"
#endif

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Constant macro definitions
***********************************************************************************************************************/
#define USBH1_DRV_MAXIMUM_ACCESS_PRV (1)

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
/* Variable */
static st_usbh1_hci_cb_info_t cb =
{
    &usbh1_hstd_a_or_detach_cb,
    &usbh1_hstd_a_or_detach_cb,
    &usbh1_hstd_over_current_cb,
    &usbh1_hstd_port_reset_cb,
    &usbh1_hstd_transfer_end_cb
};

uint32_t  g_usbh1_sem;

/***********************************************************************************************************************
Renesas USB Host Driver API functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdTransferStart
Description     : Request HCD to transfer data. HCD will transfer the data
                  based on the transfer information stored in ptr.
Arguments       : st_usbh1_utr_t *utrmsg    : USB system internal structure.
Return          : usbh1_er_t  error code : USBH1_OK/USBH1_ERROR/USBH1_ERROR.
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdTransferStart(st_usbh1_utr_t *p_utr)
{
    usbh1_er_t    err;

    if(p_utr->keyword == 0 && g_usbh1_hstd_mgr_mode[p_utr->keyword] == USBH1_DEFAULT)
    {
    	return USBH1_ERROR;
    }

    err = usbh1_hstd_transfer_start(p_utr);

    return err;
} /* End of function R_USBH1_HstdTransferStart() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdTransferEnd
Description     : Request HCD to force termination of data transfer.
Arguments       : uint16_t pipe_id      : PipeID
Return          : usbh1_er_t error code   : USBH1_OK etc
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdTransferEnd(uint16_t pipe_id)
{
    usbh1_er_t    err;

    err = usbh1_hstd_transfer_end(pipe_id);

    return err;
} /* End of function R_USBH1_HstdTransferEnd() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdDriverRegistration
Description     : The HDCD information registered in the class driver structure
                : is registered in HCD.
Arguments       : st_usbh1_hcdreg_t *p_callback    : class driver structure
Return          : none
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdDriverRegistration(st_usbh1_hcdreg_t *p_callback)
{
    st_usbh1_hcdreg_t     *p_driver;

    if (g_usbh1_hstd_device_num <= USBH1_MAXDEVADDR)
    {
        p_driver = &g_usbh1_hstd_device_drv[g_usbh1_hstd_device_num];

        p_driver->rootport      = USBH1_NOPORT;             /* Root port */
        p_driver->devaddr       = USBH1_NODEVICE;           /* Device address */
        p_driver->devstate      = USBH1_DETACHED;           /* Device state */
        if(USBH1_NULL == p_callback)
		{
			p_driver->ifclass       = USBH1_NULL;      /* Interface Class */
			p_driver->p_tpl         = USBH1_NULL;        /* Target peripheral list */
			p_driver->classcheck    = USBH1_NULL;   /* Driver check */
			p_driver->devconfig     = USBH1_NULL;    /* Device configured */
			p_driver->devdetach     = USBH1_NULL;    /* Device detach */
			p_driver->devsuspend    = USBH1_NULL;   /* Device suspend */
			p_driver->devresume     = USBH1_NULL;    /* Device resume */
		}
		else
		{
			p_driver->ifclass       = p_callback->ifclass;      /* Interface Class */
			p_driver->p_tpl         = p_callback->p_tpl;        /* Target peripheral list */
			p_driver->classcheck    = p_callback->classcheck;   /* Driver check */
			p_driver->devconfig     = p_callback->devconfig;    /* Device configured */
			p_driver->devdetach     = p_callback->devdetach;    /* Device detach */
			p_driver->devsuspend    = p_callback->devsuspend;   /* Device suspend */
			p_driver->devresume     = p_callback->devresume;    /* Device resume */
		}

        g_usbh1_hstd_device_num++;

        USBH1_PRINTF1("*** Registration driver 0x%02x\n",p_driver->ifclass);

        return USBH1_OK;
    }
    else
    {
        USBH1_PRINTF0("### Registration device driver over\n");
        return USBH1_ERROR;
    }
} /* End of function R_USBH1_HstdDriverRegistration() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdReturnEnuMGR
Description     : Continuous enumeration is requested to MGR task (API for nonOS)
Arguments       : uint16_t cls_result   : class check result
Return          : none
***********************************************************************************************************************/
void R_USBH1_HstdReturnEnuMGR(uint16_t cls_result)
{
    g_usbh1_hstd_check_enu_result = cls_result;
    usbh1_hstd_mgr_snd_mbx(USBH1_MSG_MGR_SUBMITRESULT, USBH1_PIPE0, USBH1_CTRL_END);
} /* End of function R_USBH1_HstdReturnEnuMGR() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdChangeDeviceState
Description     : Request to change the status of the connected USB Device
Arguments       : usbh1_utr_cb_t complete    : callback function pointer
                : uint16_t msginfo         : Message Info
                : uint16_t member          : Device address/pipe number
Return          : usbh1_er_t                 : USBH1_OK etc
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdChangeDeviceState(usbh1_utr_cb_t complete, uint16_t msginfo, uint16_t member)
{
    st_usbh1_utr_t    *p_blf;
    usbh1_er_t        err;

    /* Get mem pool blk */
    err = USBH1_PGET_BLK(USBH1_MGR_MPL, &p_blf);
    if (USBH1_OK == err)
    {
        USBH1_PRINTF2("*** member%d : msginfo=%d ***\n", member, msginfo);
        p_blf->msginfo  = msginfo;
        p_blf->keyword  = member;
        p_blf->complete = complete;

        /* Send message */
        err = USBH1_SND_MSG(USBH1_MGR_MBX, (usbh1_msg_t*)p_blf);
        if (USBH1_OK != err)
        {
            USBH1_PRINTF1("### hMgrChangeDeviceState snd_msg error (%ld)\n", err);
            err = USBH1_REL_BLK(USBH1_MGR_MPL, p_blf);
            if (USBH1_OK != err)
            {
                USBH1_PRINTF1("### hMgrChangeDeviceState rel_blk error (%ld)\n", err);
            }
        }
    }
    else
    {
        USBH1_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }

    return err;
} /* End of function R_USBH1_HstdChangeDeviceState() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdSetPipe
Description     : SetPipe
Argument        : uint16_t **pp_table : Data table pointer.
                    pp_table[0] : Device Descriptor
                    pp_table[1] : Configuration Descriptor
                    pp_table[2] : Interface Descriptor
                    pp_table[3] : Class check result
                    pp_table[4] : HUB device speed
                    pp_table[5] : Root port
                    pp_table[6] : Device speed
                    pp_table[7] : Device address
Return          : usbh1_er_t                   : USBH1_OK etc
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdSetPipe(uint16_t **pp_table)
{
    uint8_t         *p_condesc;
    uint8_t         *p_desc_offset;
    uint8_t         *p_descriptor;
    uint16_t        total_len;
    uint16_t        index;
    st_usbh1_pipe_t *p_pipe;
    uint16_t        pipe_id = 0;
    uint8_t         ifnum;

    /* g_usbh1_hstd_pipe[0] is Default Control Pipe */
    p_pipe = &g_usbh1_hstd_pipe[1];

    /* Done ClassCheck */
    *pp_table[3] = USBH1_OK;

    p_condesc = (uint8_t*)((uint16_t*)pp_table[1]);

    /* Configuration Descriptor + Interface Descriptor + Endpoint Descriptor */
    total_len = (uint16_t)(p_condesc[USBH1_CON_W_TOTAL_LENGTH_H] << 8) | (uint16_t)(p_condesc[USBH1_CON_W_TOTAL_LENGTH_L]);
    index     = USBH1_DEV_B_LENGTH;

    p_desc_offset = p_condesc;

    while (index < total_len)
    {
        /* index will always point to the beginning of the Descriptor */
        p_descriptor = &p_desc_offset[index];

        /* Check Descriptor Type */
        switch (p_descriptor[USBH1_DEV_B_DESCRIPTOR_TYPE])
        {
            case USBH1_DT_INTERFACE:
                ifnum = p_descriptor[USBH1_IF_B_INTERFACENUMBER];
            break;

            case USBH1_DT_ENDPOINT:
                /* Search Empty Pipe */
                while (0 != p_pipe->epnum)
                {
                    p_pipe++;
                    pipe_id++;
                    if (pipe_id > USBH1_MAXPIPE)
                    {
                        USBH1_PRINTF0("PIPE_FULL ERROR\n");
                        return USBH1_ERROR;
                    }
                }

                /* Get value from Endpoint Descriptor */
                p_pipe->epnum       = (p_descriptor[USBH1_EP_B_EPADDRESS] & USBH1_EP_NUMMASK);
                p_pipe->direction   = (p_descriptor[USBH1_EP_B_EPADDRESS] & USBH1_EP_DIRMASK);
                p_pipe->type        = (p_descriptor[USBH1_EP_B_ATTRIBUTES] & USBH1_EP_TRNSMASK);
                p_pipe->mps         = (uint16_t)(p_descriptor[USBH1_EP_W_MPS_H] << 8)
                                         | (uint16_t)(p_descriptor[USBH1_EP_W_MPS_L]);

                /* Get value from Attach Info */
                p_pipe->devaddr     = *pp_table[7];
                p_pipe->ifnum       = ifnum;
            break;

            default:
            break;
        }

        /* Next Descriptor */
        index += p_desc_offset[index];
    }

    return USBH1_OK;
} /* End of function R_USBH1_HstdSetPipe() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdGetPipeID
Description     : Return PipeID from PipeTable
Argument        : uint16_t devaddr  : Device Address
                  uint8_t type      : Endpoint Type
                  uint8_t direction : Endpoint Direction
                  uint8_t ifnum     : Interface Number
Return          : uint16_t : PipeID
***********************************************************************************************************************/
uint16_t R_USBH1_HstdGetPipeID(uint16_t devaddr, uint8_t type, uint8_t direction, uint8_t ifnum)
{
    uint16_t    pipe_id;

    for (pipe_id = 0; pipe_id < USBH1_MAXPIPE; pipe_id++)
    {
        if (g_usbh1_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            if (g_usbh1_hstd_pipe[pipe_id].type == type)
            {
                if (g_usbh1_hstd_pipe[pipe_id].direction == direction)
                {
                    if (0xFF == ifnum)
                    {
                        return pipe_id;
                    }
                    else
                    {
                        if (g_usbh1_hstd_pipe[pipe_id].ifnum == ifnum)
                        {
                            return pipe_id;
                        }
                    }
                }
            }
        }
    }

    return 0;
} /* End of function R_USBH1_HstdGetPipeID() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdClearPipe
Description     : ClearPipe
Argument        : uint16_t devaddr : Device Address
Return          : usbh1_er_t                   : USBH1_OK etc
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdClearPipe(uint16_t devaddr)
{
    uint16_t    pipe_id;

    for (pipe_id = 1; pipe_id < USBH1_MAXPIPE; pipe_id++)
    {
        if (g_usbh1_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            g_usbh1_hstd_pipe[pipe_id].ifnum      = 0;
            g_usbh1_hstd_pipe[pipe_id].epnum      = 0;
            g_usbh1_hstd_pipe[pipe_id].direction  = 0;
            g_usbh1_hstd_pipe[pipe_id].type       = 0;
            g_usbh1_hstd_pipe[pipe_id].mps        = 0;
            g_usbh1_hstd_pipe[pipe_id].devaddr    = 0;
        }
    }

    return USBH1_OK;
} /* End of function R_USBH1_HstdClearPipe() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdMgrOpen
Description     : Initialize Manager(MGR) task
Arguments       : none
Return          : none
***********************************************************************************************************************/
usbh1_er_t R_USBH1_HstdMgrOpen(void)
{
    usbh1_er_t          err = USBH1_OK;
    st_usbh1_hcdreg_t   *p_driver;
    uint16_t            i;

    /* Scheduler init */
    usbh1_hstd_sche_init();
#if (BSP_CFG_RTOS_USED == 1)
    /* create access control semaphore */
    R_OS_SemaphoreCreate( &g_usbh1_sem, USBH1_DRV_MAXIMUM_ACCESS_PRV);
#endif

    /* Manager Mode */
    g_usbh1_hstd_mgr_mode[USBH1_PORT0]  = USBH1_DETACHED;
    g_usbh1_hstd_device_speed           = USBH1_NOCONNECT;
    g_usbh1_hstd_device_addr            = USBH1_DEVICE_0;   /* Device address */
    g_usbh1_hstd_device_num             = 0;                /* Device driver number */

    for (i = 0; i < (USBH1_MAXDEVADDR + 1u); i++)
    {
        p_driver  = &g_usbh1_hstd_device_drv[i];

        p_driver->rootport  = USBH1_NOPORT;                 /* Root port */
        p_driver->devaddr   = USBH1_NODEVICE;               /* Device address */
        p_driver->devstate  = USBH1_DETACHED;               /* Device state */
        p_driver->ifclass   = (uint16_t)USBH1_IFCLS_NOT;    /* Interface Class : NO class */
    }

    R_USBH1_CstdSetTaskPri(USBH1_MGR_TSK, USBH1_PRI_2);

    /* EHCI/OHCI init */
    r_usbh1_hstd_hci_init(&cb);

    R_USBH1_CstdSetTaskPri(USBH1_HCI_TSK, USBH1_PRI_1);

    /* HUB Open */
    usbh1_hhub_registration(USBH1_NULL);
    R_USBH1_CstdSetTaskPri(USBH1_HUB_TSK, USBH1_PRI_3);

    return err;
} /* End of function R_USBH1_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdMgrTask
Description     : Call MGR(Manager) task (API for nonOS)
Argument        : none
Return          : none
***********************************************************************************************************************/
void R_USBH1_HstdMgrTask(void)
{
#if (BSP_CFG_RTOS_USED == 1)
    st_usbh1_utr_t  *p_mess;
    while(1)
    {
        USBH1_RTOS_RCV_MSG(USBH1_MGR_MSG, (usbh1_msg_t **)&p_mess);
        usbh1_hstd_mgr_task();
        g_usbh1_msg_check &= ~(1<<USBH1_MGR_TSK);
        g_usbh1_msg_check &= ~(1<<USBH1_HCI_TSK);
    }
#else /* (BSP_CFG_RTOS_USED == 1) */
    usbh1_hstd_mgr_task();
#endif /* (BSP_CFG_RTOS_USED == 1) */

} /* End of function R_USBH1_HstdMgrTask() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdDelayXms
Description     : Software Wait Timer
Arguments       : uint32_t ms   : msec
Return          : none
***********************************************************************************************************************/
void R_USBH1_HstdDelayXms(uint32_t ms)
{
    r_usbh1_hstd_hci_wait_time(ms);
} /* End of function R_USBH1_HstdDelayXms() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdMgrClose
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
void R_USBH1_HstdMgrClose(void)
{
#if (BSP_CFG_RTOS_USED == 1)
    /* create access control semaphore */
    R_OS_SemaphoreDelete( &g_usbh1_sem);
#endif
} /* End of function R_USBH1_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH1_Init
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
void R_USBH1_Init(void)
{
    usbh1_module_init();        /* Start USB Port0 module */

    R_USBH1_HstdMgrOpen();      /* USB0 MGR Open */

    R_USBH1_CstdSetTaskPri(USBH1_HUB_TSK, USBH1_PRI_3); /* USB0 HUB Open */
    R_USBH1_HstdDriverRegistration(USBH1_NULL);

#if(BSP_CFG_RTOS_USED == 1)
    r_usbh1_rtos_configration();  /* USB0 FreeRTOS Setting */
#endif /* (BSP_CFG_RTOS_USED == 1) */

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_GR_MANGO)
    r_usbh1_tusb320_host_setting();    /* USB Port1 Setting */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_GR_MANGO) */

    R_INTC_Enable(INTC_ID_USB_USBHI1);

    g_usbh1_msg_check = 0;

} /* End of function R_USBH1_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdGetMaxPacketSize
Description     : Get max packet size
Arguments       : uint32_t devadr : device address
Return          : max packet size
***********************************************************************************************************************/
uint16_t R_USBH1_HstdGetMaxPacketSize(uint16_t devadr)
{
    return usbh1_hstd_get_max_packet_size(devadr);
} /* End of function R_USBH1_HstdDelayXms() */

/***********************************************************************************************************************
Function Name   : R_USBH1_HstdGetDevAddr
Description     : Get device address
Arguments       : uint32_t devadr : device address
Return          : device address
***********************************************************************************************************************/
uint16_t R_USBH1_HstdGetDevAddr(uint16_t devadr)
{
    return usbh1_hstd_get_dev_addr(devadr);
} /* End of function R_USBH1_HstdDelayXms() */


/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
