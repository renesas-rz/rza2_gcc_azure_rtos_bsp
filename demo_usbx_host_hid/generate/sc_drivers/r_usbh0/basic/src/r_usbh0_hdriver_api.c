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
* File Name    : r_usbh0_hdriverapi.c
* Description  : USB Host Control Driver API. USB Host transfer level commands.
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh0_basic_local.h"
#include "r_usbh0_hhci.h"
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */

#if (BSP_CFG_RTOS_USED == 1)
#include "r_os_abstraction_api.h"
#include "r_usbh0_rtos.h"
#endif

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Constant macro definitions
***********************************************************************************************************************/
#define USBH0_DRV_MAXIMUM_ACCESS_PRV (1)

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
/* Variable */
static st_usbh0_hci_cb_info_t cb =
{
    &usbh0_hstd_a_or_detach_cb,
    &usbh0_hstd_a_or_detach_cb,
    &usbh0_hstd_over_current_cb,
    &usbh0_hstd_port_reset_cb,
    &usbh0_hstd_transfer_end_cb
};

uint32_t  g_usbh0_sem;

/***********************************************************************************************************************
Renesas USB Host Driver API functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdTransferStart
Description     : Request HCD to transfer data. HCD will transfer the data
                  based on the transfer information stored in ptr.
Arguments       : st_usbh0_utr_t *utrmsg    : USB system internal structure.
Return          : usbh0_er_t  error code : USBH0_OK/USBH0_ERROR/USBH0_ERROR.
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdTransferStart(st_usbh0_utr_t *p_utr)
{
    usbh0_er_t    err;

    if(p_utr->keyword == 0 && g_usbh0_hstd_mgr_mode[p_utr->keyword] == USBH0_DEFAULT)
    {
    	return USBH0_ERROR;
    }

    err = usbh0_hstd_transfer_start(p_utr);

    return err;
} /* End of function R_USBH0_HstdTransferStart() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdTransferEnd
Description     : Request HCD to force termination of data transfer.
Arguments       : uint16_t pipe_id      : PipeID
Return          : usbh0_er_t error code   : USBH0_OK etc
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdTransferEnd(uint16_t pipe_id)
{
    usbh0_er_t    err;

    err = usbh0_hstd_transfer_end(pipe_id);

    return err;
} /* End of function R_USBH0_HstdTransferEnd() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdDriverRegistration
Description     : The HDCD information registered in the class driver structure
                : is registered in HCD.
Arguments       : st_usbh0_hcdreg_t *p_callback    : class driver structure
Return          : none
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdDriverRegistration(st_usbh0_hcdreg_t *p_callback)
{
    st_usbh0_hcdreg_t     *p_driver;

    if (g_usbh0_hstd_device_num <= USBH0_MAXDEVADDR)
    {
        p_driver = &g_usbh0_hstd_device_drv[g_usbh0_hstd_device_num];

        p_driver->rootport      = USBH0_NOPORT;             /* Root port */
        p_driver->devaddr       = USBH0_NODEVICE;           /* Device address */
        p_driver->devstate      = USBH0_DETACHED;           /* Device state */
        if(USBH0_NULL == p_callback)
		{
			p_driver->ifclass       = USBH0_NULL;      /* Interface Class */
			p_driver->p_tpl         = USBH0_NULL;        /* Target peripheral list */
			p_driver->classcheck    = USBH0_NULL;   /* Driver check */
			p_driver->devconfig     = USBH0_NULL;    /* Device configured */
			p_driver->devdetach     = USBH0_NULL;    /* Device detach */
			p_driver->devsuspend    = USBH0_NULL;   /* Device suspend */
			p_driver->devresume     = USBH0_NULL;    /* Device resume */
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

        g_usbh0_hstd_device_num++;

        USBH0_PRINTF1("*** Registration driver 0x%02x\n",p_driver->ifclass);

        return USBH0_OK;
    }
    else
    {
        USBH0_PRINTF0("### Registration device driver over\n");
        return USBH0_ERROR;
    }
} /* End of function R_USBH0_HstdDriverRegistration() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdReturnEnuMGR
Description     : Continuous enumeration is requested to MGR task (API for nonOS)
Arguments       : uint16_t cls_result   : class check result
Return          : none
***********************************************************************************************************************/
void R_USBH0_HstdReturnEnuMGR(uint16_t cls_result)
{
    g_usbh0_hstd_check_enu_result = cls_result;
    usbh0_hstd_mgr_snd_mbx(USBH0_MSG_MGR_SUBMITRESULT, USBH0_PIPE0, USBH0_CTRL_END);
} /* End of function R_USBH0_HstdReturnEnuMGR() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdChangeDeviceState
Description     : Request to change the status of the connected USB Device
Arguments       : usbh0_utr_cb_t complete    : callback function pointer
                : uint16_t msginfo         : Message Info
                : uint16_t member          : Device address/pipe number
Return          : usbh0_er_t                 : USBH0_OK etc
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdChangeDeviceState(usbh0_utr_cb_t complete, uint16_t msginfo, uint16_t member)
{
    st_usbh0_utr_t    *p_blf;
    usbh0_er_t        err;

    /* Get mem pool blk */
    err = USBH0_PGET_BLK(USBH0_MGR_MPL, &p_blf);
    if (USBH0_OK == err)
    {
        USBH0_PRINTF2("*** member%d : msginfo=%d ***\n", member, msginfo);
        p_blf->msginfo  = msginfo;
        p_blf->keyword  = member;
        p_blf->complete = complete;

        /* Send message */
        err = USBH0_SND_MSG(USBH0_MGR_MBX, (usbh0_msg_t*)p_blf);
        if (USBH0_OK != err)
        {
            USBH0_PRINTF1("### hMgrChangeDeviceState snd_msg error (%ld)\n", err);
            err = USBH0_REL_BLK(USBH0_MGR_MPL, p_blf);
            if (USBH0_OK != err)
            {
                USBH0_PRINTF1("### hMgrChangeDeviceState rel_blk error (%ld)\n", err);
            }
        }
    }
    else
    {
        USBH0_PRINTF1("### hMgrChangeDeviceState pget_blk error (%ld)\n", err);
    }

    return err;
} /* End of function R_USBH0_HstdChangeDeviceState() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdSetPipe
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
Return          : usbh0_er_t                   : USBH0_OK etc
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdSetPipe(uint16_t **pp_table)
{
    uint8_t         *p_condesc;
    uint8_t         *p_desc_offset;
    uint8_t         *p_descriptor;
    uint16_t        total_len;
    uint16_t        index;
    st_usbh0_pipe_t *p_pipe;
    uint16_t        pipe_id = 0;
    uint8_t         ifnum;

    /* g_usbh0_hstd_pipe[0] is Default Control Pipe */
    p_pipe = &g_usbh0_hstd_pipe[1];

    /* Done ClassCheck */
    *pp_table[3] = USBH0_OK;

    p_condesc = (uint8_t*)((uint16_t*)pp_table[1]);

    /* Configuration Descriptor + Interface Descriptor + Endpoint Descriptor */
    total_len = (uint16_t)(p_condesc[USBH0_CON_W_TOTAL_LENGTH_H] << 8) | (uint16_t)(p_condesc[USBH0_CON_W_TOTAL_LENGTH_L]);
    index     = USBH0_DEV_B_LENGTH;

    p_desc_offset = p_condesc;

    while (index < total_len)
    {
        /* index will always point to the beginning of the Descriptor */
        p_descriptor = &p_desc_offset[index];

        /* Check Descriptor Type */
        switch (p_descriptor[USBH0_DEV_B_DESCRIPTOR_TYPE])
        {
            case USBH0_DT_INTERFACE:
                ifnum = p_descriptor[USBH0_IF_B_INTERFACENUMBER];
            break;

            case USBH0_DT_ENDPOINT:
                /* Search Empty Pipe */
                while (0 != p_pipe->epnum)
                {
                    p_pipe++;
                    pipe_id++;
                    if (pipe_id > USBH0_MAXPIPE)
                    {
                        USBH0_PRINTF0("PIPE_FULL ERROR\n");
                        return USBH0_ERROR;
                    }
                }

                /* Get value from Endpoint Descriptor */
                p_pipe->epnum       = (p_descriptor[USBH0_EP_B_EPADDRESS] & USBH0_EP_NUMMASK);
                p_pipe->direction   = (p_descriptor[USBH0_EP_B_EPADDRESS] & USBH0_EP_DIRMASK);
                p_pipe->type        = (p_descriptor[USBH0_EP_B_ATTRIBUTES] & USBH0_EP_TRNSMASK);
                p_pipe->mps         = (uint16_t)(p_descriptor[USBH0_EP_W_MPS_H] << 8)
                                         | (uint16_t)(p_descriptor[USBH0_EP_W_MPS_L]);

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

    return USBH0_OK;
} /* End of function R_USBH0_HstdSetPipe() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdGetPipeID
Description     : Return PipeID from PipeTable
Argument        : uint16_t devaddr  : Device Address
                  uint8_t type      : Endpoint Type
                  uint8_t direction : Endpoint Direction
                  uint8_t ifnum     : Interface Number
Return          : uint16_t : PipeID
***********************************************************************************************************************/
uint16_t R_USBH0_HstdGetPipeID(uint16_t devaddr, uint8_t type, uint8_t direction, uint8_t ifnum)
{
    uint16_t    pipe_id;

    for (pipe_id = 0; pipe_id < USBH0_MAXPIPE; pipe_id++)
    {
        if (g_usbh0_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            if (g_usbh0_hstd_pipe[pipe_id].type == type)
            {
                if (g_usbh0_hstd_pipe[pipe_id].direction == direction)
                {
                    if (0xFF == ifnum)
                    {
                        return pipe_id;
                    }
                    else
                    {
                        if (g_usbh0_hstd_pipe[pipe_id].ifnum == ifnum)
                        {
                            return pipe_id;
                        }
                    }
                }
            }
        }
    }

    return 0;
} /* End of function R_USBH0_HstdGetPipeID() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdClearPipe
Description     : ClearPipe
Argument        : uint16_t devaddr : Device Address
Return          : usbh0_er_t                   : USBH0_OK etc
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdClearPipe(uint16_t devaddr)
{
    uint16_t    pipe_id;

    for (pipe_id = 1; pipe_id < USBH0_MAXPIPE; pipe_id++)
    {
        if (g_usbh0_hstd_pipe[pipe_id].devaddr == devaddr)
        {
            g_usbh0_hstd_pipe[pipe_id].ifnum      = 0;
            g_usbh0_hstd_pipe[pipe_id].epnum      = 0;
            g_usbh0_hstd_pipe[pipe_id].direction  = 0;
            g_usbh0_hstd_pipe[pipe_id].type       = 0;
            g_usbh0_hstd_pipe[pipe_id].mps        = 0;
            g_usbh0_hstd_pipe[pipe_id].devaddr    = 0;
        }
    }

    return USBH0_OK;
} /* End of function R_USBH0_HstdClearPipe() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdMgrOpen
Description     : Initialize Manager(MGR) task
Arguments       : none
Return          : none
***********************************************************************************************************************/
usbh0_er_t R_USBH0_HstdMgrOpen(void)
{
    usbh0_er_t          err = USBH0_OK;
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            i;

    /* Scheduler init */
    usbh0_hstd_sche_init();
#if (BSP_CFG_RTOS_USED == 1)
    /* create access control semaphore */
    R_OS_SemaphoreCreate( &g_usbh0_sem, USBH0_DRV_MAXIMUM_ACCESS_PRV);
#endif

    /* Manager Mode */
    g_usbh0_hstd_mgr_mode[USBH0_PORT0]  = USBH0_DETACHED;
    g_usbh0_hstd_device_speed           = USBH0_NOCONNECT;
    g_usbh0_hstd_device_addr            = USBH0_DEVICE_0;   /* Device address */
    g_usbh0_hstd_device_num             = 0;                /* Device driver number */

    for (i = 0; i < (USBH0_MAXDEVADDR + 1u); i++)
    {
        p_driver  = &g_usbh0_hstd_device_drv[i];

        p_driver->rootport  = USBH0_NOPORT;                 /* Root port */
        p_driver->devaddr   = USBH0_NODEVICE;               /* Device address */
        p_driver->devstate  = USBH0_DETACHED;               /* Device state */
        p_driver->ifclass   = (uint16_t)USBH0_IFCLS_NOT;    /* Interface Class : NO class */
    }

    R_USBH0_CstdSetTaskPri(USBH0_MGR_TSK, USBH0_PRI_2);

    /* EHCI/OHCI init */
    r_usbh0_hstd_hci_init(&cb);

    R_USBH0_CstdSetTaskPri(USBH0_HCI_TSK, USBH0_PRI_1);

    /* HUB Open */
    usbh0_hhub_registration(USBH0_NULL);
    R_USBH0_CstdSetTaskPri(USBH0_HUB_TSK, USBH0_PRI_3);

    return err;
} /* End of function R_USBH0_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdMgrTask
Description     : Call MGR(Manager) task (API for nonOS)
Argument        : none
Return          : none
***********************************************************************************************************************/
void R_USBH0_HstdMgrTask(void)
{
#if (BSP_CFG_RTOS_USED == 1)
    st_usbh0_utr_t  *p_mess;
    while(1)
    {
        USBH0_RTOS_RCV_MSG(USBH0_MGR_MSG, (usbh0_msg_t **)&p_mess);
        usbh0_hstd_mgr_task();
        g_usbh0_msg_check &= ~(1<<USBH0_MGR_TSK);
        g_usbh0_msg_check &= ~(1<<USBH0_HCI_TSK);
    }
#else /* (BSP_CFG_RTOS_USED == 1) */
    usbh0_hstd_mgr_task();
#endif /* (BSP_CFG_RTOS_USED == 1) */

} /* End of function R_USBH0_HstdMgrTask() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdDelayXms
Description     : Software Wait Timer
Arguments       : uint32_t ms   : msec
Return          : none
***********************************************************************************************************************/
void R_USBH0_HstdDelayXms(uint32_t ms)
{
    r_usbh0_hstd_hci_wait_time(ms);
} /* End of function R_USBH0_HstdDelayXms() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdMgrClose
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
void R_USBH0_HstdMgrClose(void)
{
#if (BSP_CFG_RTOS_USED == 1)
    /* create access control semaphore */
    R_OS_SemaphoreDelete( &g_usbh0_sem);
#endif
} /* End of function R_USBH0_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH0_Init
Description     :
Arguments       : none
Return          : none
***********************************************************************************************************************/
void R_USBH0_Init(void)
{
    usbh0_module_init();        /* Start USB Port0 module */

    R_USBH0_HstdMgrOpen();      /* USB0 MGR Open */

    R_USBH0_CstdSetTaskPri(USBH0_HUB_TSK, USBH0_PRI_3); /* USB0 HUB Open */
    R_USBH0_HstdDriverRegistration(USBH0_NULL);

#if(BSP_CFG_RTOS_USED == 1)
    r_usbh0_rtos_configration();  /* USB0 FreeRTOS Setting */
#endif /* (BSP_CFG_RTOS_USED == 1) */

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    r_usbh0_tusb320hi_host_setting();    /* USB Port0 Setting */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    R_INTC_Enable(INTC_ID_USB_USBHI0);

    g_usbh0_msg_check = 0;

} /* End of function R_USBH0_HstdMgrOpen() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdGetMaxPacketSize
Description     : Get max packet size
Arguments       : uint32_t devadr : device address
Return          : max packet size
***********************************************************************************************************************/
uint16_t R_USBH0_HstdGetMaxPacketSize(uint16_t devadr)
{
    return usbh0_hstd_get_max_packet_size(devadr);
} /* End of function R_USBH0_HstdDelayXms() */

/***********************************************************************************************************************
Function Name   : R_USBH0_HstdGetDevAddr
Description     : Get device address
Arguments       : uint32_t devadr : device address
Return          : device address
***********************************************************************************************************************/
uint16_t R_USBH0_HstdGetDevAddr(uint16_t devadr)
{
    return usbh0_hstd_get_dev_addr(devadr);
} /* End of function R_USBH0_HstdDelayXms() */



/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
