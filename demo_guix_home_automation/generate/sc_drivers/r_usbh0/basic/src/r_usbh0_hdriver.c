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
* File Name    : r_usbh0_hdriver.c
* Description  : USB Host Control Driver
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

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
st_usbh0_pipe_t       g_usbh0_hstd_pipe[USBH0_MAXPIPE+1u];      /* pipe information */

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static uint16_t     gs_usbh0_hstd_dev_addr[USBH0_MAXDEVADDR];   /* DEVADD table */

uint16_t     usbh0_hstd_get_max_packet_size(uint16_t pipe_id);
static uint16_t     usbh0_hstd_get_pipe_type(uint16_t pipe_id);

/***********************************************************************************************************************
Description     : Get End point number from pipe number
Function Name   : uint16_t usbh0_hstd_get_epnum(uint16_t pipe_id)
Arguments       : uint16_t pipe_id  : Pipe Number
Return value    : uint16_t endpoint number
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_epnum(uint16_t pipe_id)
{
    return g_usbh0_hstd_pipe[pipe_id].epnum;
} /* End of function usbh0_hstd_get_epnum() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_max_packet_size
Description     : Fetch MaxPacketSize of the specified pipe.
Arguments       : uint16_t pipe_id  : Pipe number.
Return value    : uint16_t MaxPacketSize
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_max_packet_size(uint16_t pipe_id)
{
    return g_usbh0_hstd_pipe[pipe_id].mps;

} /* End of function usbh0_hstd_get_max_packet_size() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_dev_addr
Description     : Get device address from pipe number
Arguments       : uint16_t pipe_id  : Pipe number.
Return value    : uint16_t DEVSEL-bit status
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_dev_addr(uint16_t pipe_id)
{
    return g_usbh0_hstd_pipe[pipe_id].devaddr;
} /* End of function usbh0_hstd_get_dev_addr() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_pipe_dir
Description     : Get PIPE DIR
Arguments       : uint16_t pipe_id  : Pipe number.
Return value    : uint16_t pipe_id direction.
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_pipe_dir(uint16_t pipe_id)
{
    return g_usbh0_hstd_pipe[pipe_id].direction;
} /* End of function usbh0_hstd_get_pipe_dir() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_pipe_type
Description     : Fetch and return PIPE TYPE.
Arguments       : uint16_t pipe_id  : Pipe number.
Return value    : uint16_t pipe_id type
***********************************************************************************************************************/
static uint16_t usbh0_hstd_get_pipe_type(uint16_t pipe_id)
{
    return g_usbh0_hstd_pipe[pipe_id].type;
} /* End of function usbh0_hstd_get_pipe_type() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_dummy_function
Description     : dummy function
Arguments       : uint16_t data1    : Not used.
                : uint16_t data2    : Not used.
Return value    : none
***********************************************************************************************************************/
void usbh0_hstd_dummy_function(st_usbh0_utr_t *p_utr)
{
} /* End of function usbh0_hstd_dummy_function() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_set_rootport
Description     : Set Port Number of the connected USB Device.
Arguments       : uint16_t devaddr          : device address
                : uint16_t port             : root port
Return value    : none
***********************************************************************************************************************/
void usbh0_hstd_set_rootport(uint16_t devaddr, uint16_t rootport)
{
    gs_usbh0_hstd_dev_addr[devaddr] = rootport;
} /* End of function usbh0_hstd_set_rootport() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_rootport
Description     : Get USB port no. set in the USB register based on the speci-
                : fied USB Device address.
Arguments       : uint16_t devaddr          : device address
Return value    : uint16_t                  : root port number
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_rootport(uint16_t devaddr)
{
    return gs_usbh0_hstd_dev_addr[devaddr];
} /* End of function usbh0_hstd_get_rootport() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_mgr_snd_mbx
Description     : Send the message to MGR(Manager) task
Arguments       : uint16_t msginfo  : Message info.
                : uint16_t dat      : Port no.
                : uint16_t res      : Result
Return value    : none
***********************************************************************************************************************/
void usbh0_hstd_mgr_snd_mbx(uint16_t msginfo, uint16_t dat, uint16_t res)
{
    st_usbh0_utr_t    *p_blf;
    usbh0_er_t        err;

    /* Get mem pool blk */
    err = USBH0_PGET_BLK(USBH0_MGR_MPL, &p_blf);
    if (USBH0_OK == err)
    {
        p_blf->msginfo  = msginfo;
        p_blf->keyword  = dat;
        p_blf->result   = res;

        /* Send message */
        err = USBH0_SND_MSG(USBH0_MGR_MBX, (usbh0_msg_t *)p_blf);
        if (USBH0_OK != err)
        {
            USBH0_PRINTF1("### hMgrSndMbx snd_msg error (%ld)\n", err);
            err = USBH0_REL_BLK(USBH0_MGR_MPL, p_blf);
            if (USBH0_OK != err)
            {
                USBH0_PRINTF1("### hMgrSndMbx rel_blk error (%ld)\n", err);
            }
        }
    }
    else
    {
        USBH0_PRINTF1("### hMgrSndMbx pget_blk error (%ld)\n", err);
    }
} /* End of function usbh0_hstd_mgr_snd_mbx() */

/***********************************************************************************************************************
Function Name   : void usbh0_hstd_transfer_end
Description     : End of data transfer (IN/OUT)
Arguments       : uint16_t pipe_id         ; pipe Number
Return value    : usbh0_er_t  error code  : USBH0_OK/USBH0_ERROR.
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_transfer_end(uint16_t pipe_id)
{
    uint16_t    dev_addr;
    uint16_t    ep_num;
    uint16_t    pipe_type;
    uint16_t    pipe_dir;

    if (pipe_id > USBH0_MAXPIPE)
    {
        USBH0_PRINTF1("### R_USBH0_HstdTransferEnd PipeErr %d\n", pipe_id);
        return USBH0_ERROR;
    }

    dev_addr = usbh0_hstd_get_dev_addr(pipe_id);
    if ((USBH0_DEVICE_0 == dev_addr) && (USBH0_PIPE0 != pipe_id))
    {
        USBH0_PRINTF1("### R_USBH0_HstdTransferEnd AddrErr %d\n", pipe_id);
        return USBH0_ERROR;
    }

    ep_num      = usbh0_hstd_get_epnum(pipe_id);
    pipe_type   = usbh0_hstd_get_pipe_type(pipe_id);
    pipe_dir    = usbh0_hstd_get_pipe_dir(pipe_id);

    if (USBH0_PIPE0 == pipe_id)
    {
    }
    else
    {
        r_usbh0_hstd_hci_cancel_transfer_request(dev_addr, ep_num, pipe_type, pipe_dir);
    }

    return USBH0_OK;
} /* End of function usbh0_hstd_transfer_end() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_transfer_start
Description     : Request HCD to transfer data. HCD will transfer the data
                  based on the transfer information stored in utrmsg.
Arguments       : st_usbh0_utr_t *utr  : USB system internal structure.
Return value    : usbh0_er_t  error code  : USBH0_OK/USBH0_ERROR.
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_transfer_start(st_usbh0_utr_t *p_utr)
{
    usbh0_er_t    err;
    uint16_t    pipe_id;
    uint16_t    dev_addr;
    uint16_t    ep_num;
    uint16_t    pipe_type;
    uint16_t    pipe_dir;
    uint16_t    mps;

    pipe_id = p_utr->keyword;

    if (USBH0_PIPE0 == pipe_id)
    {
        dev_addr = (uint16_t)(p_utr->p_setup->devaddr);
    }
    else
    {
        dev_addr = usbh0_hstd_get_dev_addr(pipe_id);
    }

    if ((USBH0_DEVICE_0 == dev_addr) && (USBH0_PIPE0 != pipe_id))
    {
        USBH0_PRINTF1("### R_USBH0_HstdTransferStart not configured %x\n", dev_addr);
        return USBH0_ERROR;
    }

    if (USBH0_PIPE0 == pipe_id)
    {
        err = r_usbh0_hstd_hci_make_transfer_request(
                                                        p_utr,                                      /* utr */
                                                        p_utr->p_setup,                             /* setup */
                                                        p_utr->p_setup->devaddr,                    /* devaddr */
                                                        0,                                          /* epnum */
                                                        USBH0_EP_CNTRL,                             /* eptype */
                                                        (p_utr->p_setup->type & USBH0_DEV_TO_HOST), /* epdir */
                                                        p_utr->tranlen,                             /* tranlen */
                                                        (uint32_t)p_utr->p_tranadr,                 /* tranadr */
                                                        0                                           /* mps */
                                                    );
    }
    else
    {
        dev_addr    = usbh0_hstd_get_dev_addr(pipe_id);
        ep_num      = usbh0_hstd_get_epnum(pipe_id);
        pipe_type   = usbh0_hstd_get_pipe_type(pipe_id);
        pipe_dir    = usbh0_hstd_get_pipe_dir(pipe_id);
        mps         = usbh0_hstd_get_max_packet_size(pipe_id);

        err = r_usbh0_hstd_hci_make_transfer_request(
                                                        p_utr,                      /* utr */
                                                        (void*)USBH0_NULL,          /* setup */
                                                        dev_addr,                   /* devaddr */
                                                        ep_num,                     /* epnum */
                                                        pipe_type,                  /* eptype */
                                                        pipe_dir,                   /* epdir */
                                                        p_utr->tranlen,             /* tranlen */
                                                        (uint32_t)p_utr->p_tranadr, /* tranadr */
                                                        mps                         /* mps */
                                                    );
    }

    return err;
} /* End of function usbh0_hstd_transfer_start() */

/***********************************************************************************************************************
Function Name   : void usbh0_hstd_AttachCb
Description     : Attach / Detach Callback Function
Arguments       : uint16_t rootport         : rootport number
Return value    : void
***********************************************************************************************************************/
void usbh0_hstd_a_or_detach_cb(uint16_t rootport)
{
    uint16_t    connect_inf;
    uint16_t    dev_addr;

    
    dev_addr = r_usbh0_hstd_hci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE != dev_addr)   /* Decide USB Line state (ATTACH) */
    {
        connect_inf = USBH0_ATTACH;
    }
    else                                /* Decide USB Line state (DETACH) */
    {
        connect_inf = USBH0_DETACH;
        usbh0_hstd_set_rootport(USBH0_DEVICEADDR, 0x0000);
        USBH0_PRINTF0("*** Device address 1 clear.\n");
    }

    usbh0_hstd_mgr_snd_mbx((uint16_t)USBH0_MSG_MGR_AORDETACH, rootport, connect_inf);
} /* End of function usbh0_hstd_a_or_detach_cb() */

/***********************************************************************************************************************
Function Name   : void usbh0_hstd_port_reset_cb
Description     : Port Reset Callback Function
Arguments       : uint16_t rootport     : rootport number
Return value    : void
***********************************************************************************************************************/
void usbh0_hstd_port_reset_cb(uint16_t rootport)
{
    uint16_t    dev_addr;
    uint16_t    connect_inf;
    uint32_t    speed;

    /* support speed check */
    dev_addr = r_usbh0_hstd_hci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE == dev_addr)
    {
        connect_inf = USBH0_NOCONNECT;

        return;
    }

    speed = r_usbh0_hstd_hci_get_device_speed(dev_addr);

    switch (speed)
    {
        case USBH0_HCI_DEVSPEED_HS:
            connect_inf = USBH0_HSCONNECT;
        break;

        case USBH0_HCI_DEVSPEED_FS:
            connect_inf = USBH0_FSCONNECT;
        break;

        case USBH0_HCI_DEVSPEED_LS:
            connect_inf = USBH0_LSCONNECT;
        break;

        default:
            connect_inf = USBH0_NOCONNECT;
        break;
    }

    usbh0_hstd_mgr_snd_mbx((uint16_t)USBH0_MSG_MGR_STATUSRESULT, rootport, connect_inf);
} /* End of function usbh0_hstd_port_reset_cb() */

/***********************************************************************************************************************
Function Name   : void usbh0_hstd_over_current_cb
Description     : Over Current Callback Function
Arguments       : uint16_t rootport     : rootport number
Return value    : void
***********************************************************************************************************************/
void usbh0_hstd_over_current_cb(uint16_t rootport)
{
    usbh0_hstd_mgr_snd_mbx((uint16_t)USBH0_MSG_MGR_OVERCURRENT, rootport, 0u);
} /* End of function usbh0_hstd_over_current_cb() */

/***********************************************************************************************************************
Function Name   : void usbh0_hstd_transfer_end_cb
Description     : Transfer End Callback Function
Arguments       : void     *p_utr : UTR Pointer
                  uint16_t status : Transfer Status
Return value    : void
***********************************************************************************************************************/
void usbh0_hstd_transfer_end_cb(void *p_utr, uint32_t actual_size, uint16_t status)
{
    st_usbh0_utr_t    *p_mess;

    p_mess = (st_usbh0_utr_t *)p_utr;

    if (USBH0_DATA_SHT == status)
    {
        p_mess->tranlen -= actual_size;
    }

    /* Status setting */
    p_mess->result = status;

    /* Callback */
    p_mess->complete(p_mess);
} /* End of function usbh0_hstd_transfer_end_cb() */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
