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
* File Name    : r_usbh0_hHci.c
* Description  : HCI Common Code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <fcntl.h>

#include "r_usbh0_hhci_local.h"
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */
#include "r_riic_drv_api.h"

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
#define USBH0_HCI_MESS_MAX              (32u)   /* Maximum number of HCI message buffers */
/* For RIIC */
#define TUSB320LI_DEV_ADDRESS (0x47 << 1)

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/
extern void     usbh0_hstd_int_enable(void);
extern void     usbh0_hstd_int_disable(void);
#ifdef USBH0_HOST_COMPLIANCE_MODE
extern void     usbh0_hstd_electrical_test_mode( uint16_t product_id, uint16_t port );
#endif /* USBH0_HOST_COMPLIANCE_MODE */

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
typedef void (*USBH0_HCI_TASK_FUNC)(void);

typedef struct
{
    int16_t         sem_id;
    int16_t         tsk_id;
    uint32_t        mess_write;
    uint32_t        mess_read;
    uint32_t        mess_cnt;
    uint32_t        data1_buff[USBH0_HCI_MESS_MAX];
    uint32_t        data2_buff[USBH0_HCI_MESS_MAX];
    uint32_t        data3_buff[USBH0_HCI_MESS_MAX];
    int8_t          mess_buff[USBH0_HCI_MESS_MAX];
} st_usbh0_hci_task_info_t;

typedef struct {
    st_usbh0_hci_task_info_t  task[USBH0_HCI_TASK_MAX];
} st_usbh0_hci_manage_info_t;


void                usbh0_hstd_hci_interrupt_handler(void);
void                usbh0_hstd_hci_lock_resouce(void);

/* global */
st_usbh0_hci_cb_info_t            g_usbh0_hstd_hci_callback;

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static void                                 usbh0_hstd_hci_free_dev_info(uint32_t devadd);
static st_usbh0_hci_tr_req_t                *usbh0_hstd_hci_alloc_transefer_request(void);
static void                                 usbh0_hstd_hci_set_disconnect_flag(uint32_t devadd);
static st_usbh0_hci_endpoint_descriptor_t   *usbh0_hstd_hci_search_endpoint_descriptor(st_usbh0_hci_tr_req_t *tr_req);
static void                                 usbh0_hstd_hci_deinit(void);
static uint32_t                             usbh0_hstd_hci_get_port_current_connect_status(uint16_t rootport);
static uint16_t                             usbh0_hstd_hci_get_pid_status(uint16_t devaddr, uint16_t epnum);
static void                                 usbh0_hub_diconnect_delay(void);
#ifdef RZA2_USBH0_OHCI_IP
static void                                 r_usbh0_port_clean_seq(void);
#endif

static st_usbh0_hci_device_t        usbh0_hci_device_info[USBH0_HCI_DEVICE_NUM];
static st_usbh0_hci_manage_info_t   usbh0_hci_manage_info;
static uint16_t                     usbh0_hci_tmp_addr;

/* save register */
uint32_t usbh0_hcrh_port_status1;
uint32_t usbh0_hcrh_descriptor_a;
uint32_t usbh0_hcrh_descriptor_b;
uint32_t usbh0_portsc1;

#ifdef __ICCARM__
#pragma location="USB_HOST_0"
st_usbh0_hci_tr_req_t       g_usbh0_hstd_hci_transfer_request[USBH0_HCI_TRREQ_NUM];
static uint32_t             usbh0_hci_setup_buffer[USBH0_HCI_TRREQ_NUM][USBH0_HCI_SETUP_DATA_SIZE / sizeof(uint32_t)];
#endif  /* __ICCARM__ */

#ifdef __GNUC__
st_usbh0_hci_tr_req_t       g_usbh0_hstd_hci_transfer_request[USBH0_HCI_TRREQ_NUM] __attribute__ ((section ("USB_HOST_0")));
static uint32_t             usbh0_hci_setup_buffer[USBH0_HCI_TRREQ_NUM][USBH0_HCI_SETUP_DATA_SIZE / sizeof(uint32_t)] __attribute__ ((section ("USB_HOST_0")));
#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "USB_HOST_0"
st_usbh0_hci_tr_req_t       g_usbh0_hstd_hci_transfer_request[USBH0_HCI_TRREQ_NUM];
static uint32_t             usbh0_hci_setup_buffer[USBH0_HCI_TRREQ_NUM][USBH0_HCI_SETUP_DATA_SIZE / sizeof(uint32_t)];
#endif  /* __CC_ARM */


/***********************************************************************************************************************
 * Function     : HCI Task
 * Declaration  : void r_usbh0_hstd_hci_task(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_task(void)
{
    usbh0_er_t                  err;
    st_usbh0_hci_task_info_t    *p_task_info;
    uint32_t                    message;
    uint32_t                    data1;
    uint32_t                    data2;
    uint32_t                    data3;
    st_usbh0_utr_t              dummy;

    /* receive message */
    err = USBH0_RCV_MSG(USBH0_HCI_MBX, (usbh0_msg_t**)&dummy);
    if (err != USBH0_OK)
    {
        return;
    }

    p_task_info = &usbh0_hci_manage_info.task[USBH0_HCI_TASK];

    /* Taking out of message */
    if (p_task_info->mess_cnt)
    {
        usbh0_hstd_int_disable();

        message = p_task_info->mess_buff[p_task_info->mess_read];
        data1    = p_task_info->data1_buff[p_task_info->mess_read];
        data2    = p_task_info->data2_buff[p_task_info->mess_read];
        data3    = p_task_info->data3_buff[p_task_info->mess_read];
        p_task_info->mess_read++;
        if (USBH0_HCI_MESS_MAX <= p_task_info->mess_read)
        {
            p_task_info->mess_read = 0ul;
        }

        p_task_info->mess_cnt--;

        usbh0_hstd_int_enable();

        /* Processing of message */
        switch(message)
        {
            case USBH0_HCI_MESS_EHCI_PORT_CHANGE_DETECT:
                USBH0_HCI_PRINTF0( "USBH0_HCI_MESS_EHCI_PORT_CHANGE_DETECT \n" );
                usbh0_hstd_ehci_int_port_change();
            break;

            case USBH0_HCI_MESS_EHCI_HOST_SYSTEM_ERROR:
                USBH0_HCI_PRINTF0_R("USBH0_HCI_MESS_EHCI_HOST_SYSTEM_ERROR \n");
            break;

            case USBH0_HCI_MESS_EHCI_USBINT:
                USBH0_HCI_PRINTF0("USBH0_HCI_MESS_EHCI_USBINT \n");
                usbh0_hstd_ehci_int_transfer_end();
            break;

            case USBH0_HCI_MESS_EHCI_USBERRINT:
                USBH0_HCI_PRINTF0_R("USBH0_HCI_MESS_EHCI_USBERRINT \n");
                usbh0_hstd_ehci_int_transfer_end();
            break;

            case USBH0_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER:
                USBH0_HCI_PRINTF0("USBH0_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER \n");
            break;

            case USBH0_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE:
                USBH0_HCI_PRINTF0("USBH0_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE \n");
            break;

            case USBH0_HCI_MESS_OHCI_INT:
                USBH0_HCI_PRINTF3("USBH0_HCI_MESS_OHCI_INT : %08x %08x %08x\n", data1, data2, data3);
                usbh0_hstd_OhciMainRoutine(data1,data2,data3);
            break;

            default:
                USBH0_HCI_PRINTF0_R("Error: HCI transfer task unknown message \n");
            break;
        }
    }
} /* End of function r_usbh0_hstd_hci_task() */

/***********************************************************************************************************************
 * Function     : Interrupt Routine for HCI
 * Declaration  : void usbh0_hstd_hci_interrupt_handler(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_hci_interrupt_handler(void)
{
    g_usbh0_msg_check |= (1<<USBH0_INT);

    if(USB00.HCINTERRUPTSTATUS.BIT.RHSC && USB00.HCINTERRUPTENABLE.BIT.RHSCE)
    {
		usbh0_hcrh_descriptor_a = USB00.HCRHDESCRIPTORA.LONG;
		usbh0_hcrh_descriptor_b = USB00.HCRHDESCRIPTORB.LONG;
		usbh0_portsc1           = USB00.PORTSC1.LONG;
		usbh0_hcrh_port_status1 = USB00.HCRHPORTSTATUS1.LONG;
    	if(0 == USB00.HCRHPORTSTATUS1.BIT.CCS)
    	{
#ifdef RZA2_USBH0_OHCI_IP
    		r_usbh0_port_clean_seq();
#endif /* RZA2_USBH0_OHCI_IP */
    		/* R_USBH0_HstdDelayXms((uint16_t)30); */
    		usbh0_hub_diconnect_delay();
    	}
    	else
    	{
    	}
    }
    else if(USB00.USBSTS.BIT.PortChangeDetect && USB00.USBINTR.BIT.PortChangeDetectEnable)
    {
    	if(0 == USB00.HCRHPORTSTATUS1.BIT.CCS)
    	{
    		/* R_USBH0_HstdDelayXms((uint16_t)30); */
    		usbh0_hub_diconnect_delay();
    	}
    	else
    	{
    	}
    }
    else
    {
    }

    /* Check ohci */
    usbh0_hstd_ohci_interrupt_handler();

    /* Check ehci */
    usbh0_hstd_ehci_interrupt_handler();

    g_usbh0_msg_check &= ~(1<<USBH0_INT);

} /* End of function usbh0_hstd_hci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : Message sending from interrupt
 * Declaration  : void usbh0_hstd_hci_send_message_from_int( int32_t task_no, int8_t message, uint32_t data )
 * Argument     : int32_t task_no ; task no
 *              : int8_t message  ; message type
 *              : void *data      ; data pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_hci_send_message_from_int(int32_t task_no, int8_t message, uint32_t data1, uint32_t data2, uint32_t data3)
{
    st_usbh0_hci_task_info_t    *p_task_info;

    p_task_info = &usbh0_hci_manage_info.task[task_no];

    /* Registration of message */
    p_task_info->mess_buff[p_task_info->mess_write] = message;
    p_task_info->data1_buff[p_task_info->mess_write] = data1;
    p_task_info->data2_buff[p_task_info->mess_write] = data2;
    p_task_info->data3_buff[p_task_info->mess_write] = data3;
    p_task_info->mess_write++;

    if (USBH0_HCI_MESS_MAX <= p_task_info->mess_write)
    {
        p_task_info->mess_write = 0ul;
    }

    p_task_info->mess_cnt++;

    if (p_task_info->mess_cnt > USBH0_HCI_MESS_MAX)
    {
        /* Message buffer overflow  */
        USBH0_HCI_PRINTF1_R( "Error: HCI send message buffer overflow( task_no:%d ) \n", task_no );
    }

    /* Notification of message sending */
    USBH0_SND_MSG(USBH0_HCI_MBX, (usbh0_msg_t *)NULL);
} /* End of function usbh0_hstd_hci_send_message_from_int() */

/***********************************************************************************************************************
 * Function     : Allocate Device-Information-Memory
 * Declaration  : st_usbh0_hci_device_t *usbh0_hstd_hci_alloc_dev_info(void)
 * Argument     : void
 * Return       : st_usbh0_hci_device_t * ; Device-Information-Memory pointer
 * Note         :
***********************************************************************************************************************/
st_usbh0_hci_device_t *usbh0_hstd_hci_alloc_dev_info(void)
{
    st_usbh0_hci_device_t   *p_ret = NULL;
    st_usbh0_hci_device_t   *p_devinfo;
    uint32_t                i;

    usbh0_hstd_hci_lock_resouce();

    p_devinfo = &usbh0_hci_device_info[0];
    for (i = 0; i < USBH0_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if(FALSE == p_devinfo->bit.enable)
        {
            memset(p_devinfo, 0, sizeof(st_usbh0_hci_device_t));
            p_devinfo->bit.enable = TRUE;
            p_ret = p_devinfo;
            break;
        }
    }

    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret)
    {
        USBH0_HCI_PRINTF0_R( "--------------------------------------------------\n" );
        USBH0_HCI_PRINTF0_R( "Memory Allocation Error: usbh0_hstd_hci_alloc_dev_info\n" );
        USBH0_HCI_PRINTF0_R( "--------------------------------------------------\n" );
    }

    return p_ret;
} /* End of function usbh0_hstd_hci_alloc_dev_info() */

/***********************************************************************************************************************
 * Function     : Free Device-Information-Memory
 * Declaration  : void usbh0_hstd_hci_free_dev_info(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         : When the device is HUB, the disconnection process of the down port is executed.
***********************************************************************************************************************/
void usbh0_hstd_hci_free_dev_info(uint32_t devadd)
{
    st_usbh0_hci_device_t   *p_devinfo;
    uint32_t                n;
    uint32_t                i;

    p_devinfo = &usbh0_hci_device_info[0];
    for (i = 0; i < USBH0_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (p_devinfo->bit.enable == TRUE)
        {
            if (p_devinfo->bit.devnum == devadd)
            {
                /* When the device is HUB, the disconnection process of the down port is executed.  */
                for (n = 0; n < USBH0_HCI_HUBPORT_NUM; n++)
                {
                    if(p_devinfo->hubdevinfo.devnum[n] != 0)
                    {
                        r_usbh0_hstd_hci_disconnect(p_devinfo->hubdevinfo.devnum[n]);
                    }
                }
                usbh0_hstd_hci_lock_resouce();
                p_devinfo->bit.enable = FALSE;  /* clear device info */
                usbh0_hstd_hci_unlock_resouce();
            }
            else
            {
                /* The HUB registered information is deleted for the device connected with HUB. */
                for (n = 0; n < USBH0_HCI_HUBPORT_NUM; n++)
                {
                    if (p_devinfo->hubdevinfo.devnum[n] == devadd)
                    {
                        p_devinfo->hubdevinfo.devnum[n]     = 0;
                        p_devinfo->hubdevinfo.devspeed[n]   = 0;
                    }
                }
            }
        }
    }
} /* End of function usbh0_hstd_hci_free_dev_info() */

/***********************************************************************************************************************
 * Function     : Getting of Pollrate
 * Declaration  : uint32_t usbh0_hstd_hci_get_pollrate(st_usbh0_hci_tr_req_t *tr_req)
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : uint32_t               ; pollrate( uFrame unit )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_hci_get_pollrate(st_usbh0_hci_tr_req_t *p_tr_req)
{
    st_usbh0_hci_endpoint_descriptor_t  *p_ep_desc;
    uint32_t                            ep_interbal;
    uint32_t                            pollrate = 0;

    p_ep_desc = usbh0_hstd_hci_search_endpoint_descriptor(p_tr_req);
    if (NULL != p_ep_desc)
    {
        ep_interbal = p_ep_desc->bInterval;
        switch (p_tr_req->bit.eptype)
        {
            case USBH0_EP_INT:
                if (USBH0_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
                {
                    pollrate = 1;
                    for (ep_interbal--; ep_interbal != 0; ep_interbal--)
                    {
                        pollrate *= 2;
                    }
                }
                else
                {
                    pollrate = ep_interbal * 8;
                }
            break;

            case USBH0_EP_ISO:
                if (USBH0_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
                {
                    pollrate = 1;
                    for (ep_interbal--; ep_interbal != 0; ep_interbal--)
                    {
                        pollrate *= 2;
                    }
                }
                else
                {
                    if (0x0200 == p_tr_req->devinfo->devicedesc.bcd_usb)
                    {
                        pollrate = 1;
                        for(ep_interbal--; ep_interbal != 0; ep_interbal--)
                        {
                            pollrate *= 2;
                        }
                        pollrate *= 8;
                    }
                    else
                    {
                        pollrate = 8;
                    }
                }
            break;

            default:
            break;
        }
    }

    return(pollrate);
} /* End of function usbh0_hstd_hci_get_pollrate() */

/***********************************************************************************************************************
 * Function     : HCI Lock resource
 * Declaration  : void usbh0_hstd_hci_lock_resouce(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_hci_lock_resouce(void)
{
} /* End of function usbh0_hstd_hci_lock_resouce() */

/***********************************************************************************************************************
 * Function     : HCI Lock resource
 * Declaration  : void usbh0_hstd_hci_unlock_resouce( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_hci_unlock_resouce(void)
{
} /* End of function usbh0_hstd_hci_unlock_resouce() */

/***********************************************************************************************************************
 * Function     : Device Disconnect Flag Set
 * Declaration  : void usbh0_hstd_hci_set_disconnect_flag(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh0_hstd_hci_set_disconnect_flag(uint32_t devadd)
{
    st_usbh0_hci_device_t *p_devinfo;
    uint32_t            n;

    p_devinfo = r_usbh0_hstd_hci_get_device_infomation(devadd);

    /* Disconnect Request Flag Set */
    p_devinfo->bit.disconnect_req = TRUE;

    /* Hub Port Device Disconnect */
    for (n = 0; n < USBH0_HCI_HUBPORT_NUM; n++)
    {
        if (p_devinfo->hubdevinfo.devnum[n] != 0)
        {
            usbh0_hstd_hci_set_disconnect_flag(p_devinfo->hubdevinfo.devnum[n]);
        }
    }
} /* End of function usbh0_hstd_hci_set_disconnect_flag() */

/***********************************************************************************************************************
 * Function     : Searching of endpoint descriptor address
 * Declaration  : st_usbh0_hci_endpoint_descriptor_t* usbh0_hstd_hci_search_endpoint_descriptor(st_usbh0_hci_tr_req_t *tr_req)
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : st_usbh0_hci_endpoint_descriptor_t * ; endpoint descriptor pointer
 * Note         :
***********************************************************************************************************************/
static st_usbh0_hci_endpoint_descriptor_t* usbh0_hstd_hci_search_endpoint_descriptor(st_usbh0_hci_tr_req_t *p_tr_req)
{
    st_usbh0_hci_endpoint_descriptor_t  *p_epdesc;
    st_usbh0_hci_descriptor_header_t    *p_desc_head;
    st_usbh0_hci_device_t               *p_devinfo;
    uint32_t                            n;
    uint8_t                             ep_direction;

    n = 0;
    p_devinfo = p_tr_req->devinfo;

    if (USBH0_HCI_DIRECTION_IN == p_tr_req->bit.direction)
    {
        ep_direction = USBH0_EP_IN;
    }
    else
    {
        ep_direction = USBH0_EP_OUT;
    }

    while (n < USBH0_HCI_CONFIGDESC_BUFFSIZE)
    {
        p_desc_head = (st_usbh0_hci_descriptor_header_t *)((uint8_t *)&p_devinfo->rawconfigdesc[0] + n);
        if (USBH0_DT_ENDPOINT == p_desc_head->bdescriptor_type)
        {
            p_epdesc = (st_usbh0_hci_endpoint_descriptor_t *)p_desc_head;
            if (((p_epdesc->bmAttributes & USBH0_EP_TRNSMASK) == p_tr_req->bit.eptype)
                && ((p_epdesc->bEndpointAddress & USBH0_EP_DIRMASK) == ep_direction)
                && ((p_epdesc->bEndpointAddress & USBH0_EP_NUMMASK) == p_tr_req->bit.epnum))
            {
                return p_epdesc;
            }
        }
        n += p_desc_head->blength;
    }

    return NULL;
} /* End of function st_usbh0_hci_endpoint_descriptor_t() */

/***********************************************************************************************************************
 * Function     : Allocate Transfer-Request-Memory
 * Declaration  : st_usbh0_hci_tr_req_t* usbh0_hstd_hci_alloc_transefer_request( void )
 * Argument     : void
 * Return       : st_usbh0_hci_tr_req_t * ; Transfer-Request-Memory pointer
 * Note         :
***********************************************************************************************************************/
st_usbh0_hci_tr_req_t *usbh0_hstd_hci_alloc_transefer_request(void)
{
    st_usbh0_hci_tr_req_t   *p_ret = NULL;
    st_usbh0_hci_tr_req_t   *p_tr_req;
    uint32_t                i;

    usbh0_hstd_hci_lock_resouce();

    p_tr_req = &g_usbh0_hstd_hci_transfer_request[0];
    for (i = 0; i < USBH0_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (p_tr_req->bit.enable == FALSE)
        {
            memset(p_tr_req, 0, sizeof(st_usbh0_hci_tr_req_t));

            p_tr_req->bit.enable    = TRUE;
            R_MMU_VAtoPA((uint32_t)&usbh0_hci_setup_buffer[i][0],&p_tr_req->setupbuf);
            p_ret                   = p_tr_req;
            break;
        }
    }

    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret)
    {
        USBH0_HCI_PRINTF0_R( "-----------------------------------------------------------\n" );
        USBH0_HCI_PRINTF0_R( "Memory Allocation Error: usbh0_hstd_hci_alloc_transefer_request\n" );
        USBH0_HCI_PRINTF0_R( "-----------------------------------------------------------\n" );
    }

    return p_ret;
} /* End of function usbh0_hstd_hci_alloc_transefer_request() */

/* API function */
/***********************************************************************************************************************
 * Function     : Initialize HCI
 * Declaration  : int32_t r_usbh0_hstd_hci_init(st_usbh0_hci_cb_info_t *cb_info)
 * Argument     : st_usbh0_hci_cb_info_t *cb_info ; callback function infomation
 * Return       : uint32_t                  ; status
 * Note         :
***********************************************************************************************************************/
int32_t r_usbh0_hstd_hci_init(st_usbh0_hci_cb_info_t *p_cb_info)
{
    uint32_t    err;

    if (NULL == p_cb_info)
    {
        return USBH0_ERROR;
    }
    if ((NULL == p_cb_info->attach_cb)
        || (NULL == p_cb_info->detach_cb)
        || (NULL == p_cb_info->over_current_cb)
        || (NULL == p_cb_info->tr_end_cb))
    {
        return USBH0_ERROR;
    }

    /* set callback function infomation */
    g_usbh0_hstd_hci_callback = *p_cb_info;

    /* HCI management memory initialization */
    memset(&usbh0_hci_manage_info, 0, sizeof(usbh0_hci_manage_info));

    usbh0_hstd_hci_lock_resouce();

    /* Initialize Transfer-Request Array */
    memset(&g_usbh0_hstd_hci_transfer_request[0], 0, sizeof(g_usbh0_hstd_hci_transfer_request));

    /* Initialize Device-Info Array */
    memset(&usbh0_hci_device_info[0], 0, sizeof(usbh0_hci_device_info));

    usbh0_hstd_hci_unlock_resouce();

    /* init ehci */
    err = usbh0_hstd_ehci_init();
    if (USBH0_OK != err)
    {
        return USBH0_ERROR;
    }

    /* init ohci */
    err = usbh0_hstd_ohci_init();
    if (USBH0_OK != err)
    {
        return USBH0_ERROR;
    }

    /* interrupt enable */
//    usbh0_hstd_UsbintInit();

    return(USBH0_OK);
} /* End of function r_usbh0_hstd_hci_init() */

/***********************************************************************************************************************
 * Function     : Deinitialize HCI
 * Declaration  : void usbh0_hstd_hci_deinit(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh0_hstd_hci_deinit(void)
{
    st_usbh0_hci_device_t   *p_devinfo;
    uint8_t                 i;

    /* All transfers are stopped, and all devices are disconnect. */
    p_devinfo = &usbh0_hci_device_info[0];
    for (i = 0; i < USBH0_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (FALSE == p_devinfo->bit.enable)
        {
            r_usbh0_hstd_hci_disconnect(p_devinfo->bit.devnum);
        }
    }

    usbh0_hstd_ehci_deinit();   /* EHCI H/W reset */
    usbh0_hstd_ohci_deinit();   /* OHCI H/W reset */

    memset(&usbh0_hci_manage_info, 0, sizeof(usbh0_hci_manage_info));
} /* End of function usbh0_hstd_hci_deinit() */

/***********************************************************************************************************************
 * Function     : Making of Transfer Request
 * Declaration  : void usbh0_hstd_HciMakeTransferRequest(st_usbh0_hci_tr_req_t *tr_req)
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; transfer request infomation
 * Return       : uint16_t
 * Note         :
***********************************************************************************************************************/
uint16_t r_usbh0_hstd_hci_make_transfer_request(void *p_utr, void *p_setup, uint32_t devaddr, uint32_t epnum,
                                                uint32_t eptype, uint32_t epdir, uint32_t tranlen, uint32_t tranadr,
                                                uint32_t mps)
{
    st_usbh0_hci_tr_req_t   *p_tr_req;
    uint8_t                 *p_dst;
    uint8_t                 *p_src;
    uint32_t                i;

    p_tr_req = usbh0_hstd_hci_alloc_transefer_request();
    if (NULL == p_tr_req)
    {
        return USBH0_ERROR;
    }

    /* Device Address */
    p_tr_req->bit.devadrs = devaddr;

    /* Get Device Information */
    if (0 == devaddr)
    {
        devaddr = usbh0_hci_tmp_addr;
    }

    p_tr_req->devinfo  = r_usbh0_hstd_hci_get_device_infomation( devaddr );

    /* Check Devinfo Address */
    if (NULL == p_tr_req->devinfo)
    {
        p_tr_req->bit.enable = FALSE;
        USBH0_HCI_PRINTF0_R( "-------------\n" );
        USBH0_HCI_PRINTF0_R( " Devinfo NULL\n" );
        USBH0_HCI_PRINTF0_R( "-------------\n" );
        return USBH0_ERROR;
    }

    /* Check Connection */
    if (TRUE == p_tr_req->devinfo->bit.disconnect_req)
    {
        p_tr_req->bit.enable = FALSE;
        USBH0_HCI_PRINTF0_R( "-------------\n" );
        USBH0_HCI_PRINTF0_R( " Disconnected\n" );
        USBH0_HCI_PRINTF0_R( "-------------\n" );
        return USBH0_ERROR;
    }

    /* Endpoint Type */
    p_tr_req->bit.eptype = eptype;

    /* Direction */
    if (USBH0_EP_OUT == epdir)
    {
        p_tr_req->bit.direction = USBH0_HCI_DIRECTION_OUT;
    }
    else
    {
        p_tr_req->bit.direction = USBH0_HCI_DIRECTION_IN;
    }

    p_tr_req->bit.epnum = epnum;        /* Endpoint Number */
    p_tr_req->trsize    = tranlen;      /* Transfer Size */
    R_MMU_VAtoPA((uint32_t)tranadr,&p_tr_req->databuf);      /* Transfer Data Buffer */
    p_tr_req->utr_p     = p_utr;        /* Set UTR Pointer */

    /* Control Transfer */
    if(USBH0_EP_CNTRL == eptype)
    {
        /* Setup Buffer */
        /* Because the data format is different, location is converted. */
        p_dst = (uint8_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->setupbuf);
        p_src = (uint8_t *)r_usbh0_pa_to_va((uint32_t)p_setup);
        for (i = 0; i < USBH0_HCI_SETUP_DATA_SIZE; i++)
        {
            *p_dst++ = *p_src++;
        }

        /* MaxPacket Size */
        if (0 == p_tr_req->devinfo->devicedesc.bmax_packet_size_0)
        {
            if (USBH0_HCI_DEVSPEED_LS == p_tr_req->devinfo->bit.devspeed)
            {
                p_tr_req->bit.mps = 0x08;
            }
            else
            {
                p_tr_req->bit.mps = 0x40;
            }
        }
        else
        {
            p_tr_req->bit.mps = p_tr_req->devinfo->devicedesc.bmax_packet_size_0;
        }
    }
    /* For Bulk, Interrupt and  Isochronous Transfer */
    else if ((eptype == USBH0_EP_ISO) || (eptype == USBH0_EP_BULK) || (eptype == USBH0_EP_INT))
    {
        p_tr_req->bit.mps = mps;
    }

    switch (p_tr_req->devinfo->bit.hctype)
    {
        case USBH0_HCI_HCTYPE_EHCI:
            usbh0_hstd_ehci_make_transfer_request(p_tr_req);
        break;

        case USBH0_HCI_HCTYPE_OHCI:
            usbh0_hstd_ohci_make_transfer_request(p_tr_req);
        break;

        default:
        break;
    }

    return USBH0_OK;
} /* End of function r_usbh0_hstd_hci_make_transfer_request() */

/***********************************************************************************************************************
 * Function     : Message sending of transfer request
 * Declaration  : void usbh0_hstd_HciSendCancelTransferRequest(uint32_t devadrs, uint32_t epnum, uint32_t direction)
 * Argument     : uint32_t devadrs   ; device address
 *              : uint32_t epnum     ; endpoint no
 *              : uint32_t direction ; direction
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_cancel_transfer_request(uint32_t devaddr, uint32_t epnum, uint32_t eptype, uint32_t epdir)
{
    st_usbh0_hci_tr_req_t   *p_tr_req;
    uint32_t                i;

    p_tr_req = &g_usbh0_hstd_hci_transfer_request[0];
    for (i = 0; i < USBH0_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (p_tr_req->bit.enable == TRUE)
        {
            if (epdir == USBH0_EP_OUT)
            {
                epdir = USBH0_HCI_DIRECTION_OUT;
            }
            else
            {
                epdir = USBH0_HCI_DIRECTION_IN;
            }

            if ((p_tr_req->bit.devadrs == devaddr) && (p_tr_req->bit.epnum == epnum) &&
                (p_tr_req->bit.direction == epdir) &&  (p_tr_req->bit.eptype == eptype))
            {
                if (NULL != p_tr_req->hci_info)
                {
                    p_tr_req->devinfo  = r_usbh0_hstd_hci_get_device_infomation( devaddr );
                    if (USBH0_HCI_HCTYPE_EHCI == p_tr_req->devinfo->bit.hctype)
                    {
                        usbh0_hstd_ehci_cancel_transfer_request(p_tr_req);
                    }
                    else if (USBH0_HCI_HCTYPE_OHCI == p_tr_req->devinfo->bit.hctype)
                    {
                        usbh0_hstd_ohci_cancel_transfer_request(p_tr_req);
                    }
                }

                return;
            }
        }
    }
} /* End of function r_usbh0_hstd_hci_cancel_transfer_request() */

/***********************************************************************************************************************
 * Function     : Device Disconnect
 * Declaration  : void r_usbh0_hstd_hci_disconnect(uint32_t devadd)
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_disconnect(uint32_t devadd)
{
    st_usbh0_hci_device_t   *p_devinfo;

    p_devinfo = r_usbh0_hstd_hci_get_device_infomation(devadd);
    if (NULL != p_devinfo)
    {
        /* Disconnect Request Flag Set */
        usbh0_hstd_hci_set_disconnect_flag(devadd);

        switch (p_devinfo->bit.hctype)
        {
            case USBH0_HCI_HCTYPE_EHCI:
                usbh0_hstd_ehci_disconnect(devadd);
                usbh0_hstd_hci_free_dev_info(devadd);
            break;

            case USBH0_HCI_HCTYPE_OHCI:
                USB00.HCCONTROL.LONG = USB00.HCCONTROL.LONG | 0x000000c0 & 0xffffffcf;

                usbh0_hstd_ohci_disconnect(0);
                usbh0_hstd_hci_free_dev_info(0);

                usbh0_hstd_ohci_disconnect(devadd);
                usbh0_hstd_hci_free_dev_info(devadd);

                USB00.HCCONTROL.LONG = (USB00.HCCONTROL.LONG & ~0x000000C0) | 0x00000080;
            break;

            default:
            break;
        }
    }
} /* End of function r_usbh0_hstd_hci_disconnect() */

/***********************************************************************************************************************
 * Function     : Getting of device address of rootpoot
 * Declaration  : uint16_t r_usbh0_hstd_hci_get_device_address_of_rootpoot(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t r_usbh0_hstd_hci_get_device_address_of_rootpoot(uint16_t rootport)
{
    uint16_t    devaddr;

    devaddr = usbh0_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE == devaddr)
    {
        devaddr = usbh0_hstd_ohci_get_device_address_of_rootpoot(rootport);
    }

    return devaddr;
} /* End of function r_usbh0_hstd_hci_get_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : The device address of the route port is set.
 * Declaration  : uint16_t r_usbh0_hstd_hci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t devaddr)
 * Argument     : uint16_t rootport ; rootport no
 *              : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t r_usbh0_hstd_hci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t devaddr)
{
    uint16_t    ret_devaddr;

    usbh0_hci_tmp_addr = devaddr;

    ret_devaddr = usbh0_hstd_ehci_set_device_address_of_rootpoot(rootport, devaddr);
    if(USBH0_HCI_NODEVICE == ret_devaddr)
    {
        ret_devaddr = usbh0_hstd_ohci_set_device_address_of_rootpoot(rootport, devaddr);
    }
    return(ret_devaddr);
} /* End of function r_usbh0_hstd_hci_set_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : Getting of device speed
 * Declaration  : uint32_t r_usbh0_hstd_hci_get_device_speed(uint16_t devaddr)
 * Argument     : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device speed
 * Note         :
***********************************************************************************************************************/
uint32_t r_usbh0_hstd_hci_get_device_speed(uint16_t devaddr)
{
    st_usbh0_hci_device_t   *devinfo;
    uint32_t                i;

    devinfo = &usbh0_hci_device_info[0];
    for (i = 0; i < USBH0_HCI_DEVICE_NUM; i++, devinfo++)
    {
        if (TRUE == devinfo->bit.enable)
        {
            if (devinfo->bit.devnum == devaddr)
            {
                return devinfo->bit.devspeed;
            }
        }
    }

    return 0xFF;   /* Error */
} /* End of function r_usbh0_hstd_hci_get_device_speed() */

/***********************************************************************************************************************
 * Function     : Getting of device information
 * Declaration  : st_usbh0_hci_device_t *r_usbh0_hstd_hci_get_device_infomation(uint16_t devaddr)
 * Argument     : uint16_t devaddr  ; device address
 * Return       : st_usbh0_hci_device_t *  ; device information
 * Note         :
***********************************************************************************************************************/
st_usbh0_hci_device_t *r_usbh0_hstd_hci_get_device_infomation(uint16_t devaddr)
{
    st_usbh0_hci_device_t   *p_devinfo;
    uint32_t                i;

    p_devinfo = &usbh0_hci_device_info[0];
    for (i = 0; i < USBH0_HCI_DEVICE_NUM; i++, p_devinfo++)
    {
        if (TRUE == p_devinfo->bit.enable)
        {
            if (p_devinfo->bit.devnum == devaddr)
            {
                return p_devinfo;
            }
        }
    }

    return NULL;    /* Error */
} /* End of function r_usbh0_hstd_hci_get_device_infomation() */

/***********************************************************************************************************************
 * Function     :
 * Declaration  : uint32_t usbh0_hstd_hci_get_port_current_connect_status(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint32_t          ; Current Connect Status( 1:Connected, 0:Not Connect );
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh0_hstd_hci_get_port_current_connect_status(uint16_t rootport)
{
    return usbh0_hstd_ehci_get_port_current_connect_status(rootport);
} /* End of function usbh0_hstd_hci_get_port_current_connect_status() */

/***********************************************************************************************************************
 * Function     : Port Reset
 * Declaration  : void usbh0_hstd_HciPortReset(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_port_reset(uint16_t rootport)
{
    uint16_t    devaddr;

    devaddr = usbh0_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE != devaddr)
    {
        usbh0_hstd_ehci_port_reset(rootport);
        g_usbh0_hstd_hci_callback.port_reset_cb(rootport);
    }
    else
    {
        devaddr = usbh0_hstd_ohci_get_device_address_of_rootpoot(rootport);
        if (USBH0_HCI_NODEVICE != devaddr)
        {
            usbh0_hstd_ohci_port_reset(rootport);
            g_usbh0_hstd_hci_callback.port_reset_cb(rootport);
        }
    }
} /* End of function r_usbh0_hstd_hci_port_reset() */

/***********************************************************************************************************************
 * Function     : Port Suspend
 * Declaration  : void r_usbh0_hstd_hci_port_suspend(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_port_suspend(uint16_t rootport)
{
    uint16_t    devaddr;

    devaddr = usbh0_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE != devaddr)
    {
        usbh0_hstd_ehci_port_suspend(rootport);
    }
    else
    {
        devaddr = usbh0_hstd_ohci_get_device_address_of_rootpoot(rootport);
        if (USBH0_HCI_NODEVICE != devaddr)
        {
            usbh0_hstd_ohci_port_suspend(rootport);
        }
    }
} /* End of function r_usbh0_hstd_hci_port_suspend() */

/***********************************************************************************************************************
 * Function     : Port Resume
 * Declaration  : void r_usbh0_hstd_hci_port_resume(uint16_t rootport)
 * Argument     : uint16_t rootport ; rootport no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_port_resume(uint16_t rootport)
{
    uint16_t    devaddr;

    devaddr = usbh0_hstd_ehci_get_device_address_of_rootpoot(rootport);
    if (USBH0_HCI_NODEVICE != devaddr)
    {
        usbh0_hstd_ehci_port_resume(rootport);
    }
    else
    {
        devaddr = usbh0_hstd_ohci_get_device_address_of_rootpoot(rootport);
        if (USBH0_HCI_NODEVICE != devaddr)
        {
            usbh0_hstd_ohci_port_resume(rootport);
        }
    }
} /* End of function r_usbh0_hstd_hci_port_resume() */

/***********************************************************************************************************************
 * Function     : Getting of PID status
 * Declaration  : uint16_t usbh0_hstd_hci_get_pid_status(uint16_t devaddr, uint16_t epnum)
 * Argument     : uint16_t devaddr ; device address
 * Note         :
***********************************************************************************************************************/
static uint16_t usbh0_hstd_hci_get_pid_status(uint16_t devaddr, uint16_t epnum)
{
    st_usbh0_hci_tr_req_t   *p_tr_req;
    uint32_t                i;
    uint16_t                sts;

    p_tr_req = &g_usbh0_hstd_hci_transfer_request[0];
    for (i = 0; i < USBH0_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (TRUE == p_tr_req->bit.enable)
        {
            if ((p_tr_req->bit.devadrs == devaddr) && (p_tr_req->bit.epnum == epnum))
            {
                if (USBH0_HCI_HCTYPE_EHCI == p_tr_req->devinfo->bit.hctype)
                {
                    sts = usbh0_hstd_ehci_get_pid_status(p_tr_req);
                    return sts;
                }
                else if (p_tr_req->devinfo->bit.hctype == USBH0_HCI_HCTYPE_OHCI)
                {
                    sts = usbh0_hstd_ohci_get_pid_status(p_tr_req);
                    return sts;
                }

                break;
            }
        }
    }

    return USBH0_PID_NAK;
} /* End of function usbh0_hstd_hci_get_pid_status() */

/***********************************************************************************************************************
 * Function     : HUB information setting when new device is attached.
 * Declaration  : void r_usbh0_hstd_hci_set_hub_info(uint32_t hubaddr, uint32_t hubportnum, uint32_t devaddr, uint32_t devspeed)
 * Argument     : uint32_t hubaddr    ; HUB device address
 *              : uint32_t hubportnum ; HUB port no
 *              : uint32_t devaddr    ; new device address
 *              : uint32_t devspeed   ; new device speed
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_set_hub_info(uint32_t hubaddr, uint32_t hubportnum, uint32_t devaddr, uint32_t devspeed)
{
    st_usbh0_hci_device_t   *p_new_devinfo;
    st_usbh0_hci_device_t   *p_hub_devinfo;

    p_new_devinfo = usbh0_hstd_hci_alloc_dev_info();
    if (NULL != p_new_devinfo)
    {
        /* Getting of device information on connected HUB */
        p_hub_devinfo = r_usbh0_hstd_hci_get_device_infomation(hubaddr);
        if (NULL != p_hub_devinfo)
        {
            p_new_devinfo->bit.devnum   = devaddr;
            p_new_devinfo->bit.devspeed = devspeed;
            p_new_devinfo->bit.hctype   = p_hub_devinfo->bit.hctype;

            /* The split transaction is checked. */
            if (TRUE == p_hub_devinfo->bit.sprit_flag)
            {
                p_new_devinfo->bit.sprit_flag   = TRUE;
                p_new_devinfo->bit.hubaddr      = p_hub_devinfo->bit.hubaddr;
                p_new_devinfo->bit.hubportnum   = p_hub_devinfo->bit.hubportnum;
            }
            else
            {
                if((USBH0_HCI_DEVSPEED_HS != devspeed) && (USBH0_HCI_DEVSPEED_HS == p_hub_devinfo->bit.devspeed))
                {
                    /* The split transaction is set. */
                    p_new_devinfo->bit.sprit_flag = TRUE;
                }
                else
                {
                    p_new_devinfo->bit.sprit_flag = FALSE;
                }

                p_new_devinfo->bit.hubaddr      = hubaddr;
                p_new_devinfo->bit.hubportnum   = hubportnum;
            }

            /* Device information on HCI is succeeded. */
            p_new_devinfo->hci_device = p_hub_devinfo->hci_device;
        }
    }
} /* End of function r_usbh0_hstd_hci_set_hub_info() */

/***********************************************************************************************************************
 * Function     : The device address of the HUB port is set.
 * Declaration  : void r_usbh0_hstd_hci_set_device_address_of_hub_port(uint16_t hubaddr, uint16_t devaddr)
 * Argument     : uint16_t hubaddr ; HUB device address
 *              : uint16_t devaddr ; HUB down port device address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void r_usbh0_hstd_hci_set_device_address_of_hub_port(uint16_t hubaddr, uint16_t devaddr)
{
    st_usbh0_hci_device_t   *p_hub_devinfo;
    st_usbh0_hci_device_t   *p_devinfo;
    uint32_t                cnt;

    /* Getting of HUB device information */
    p_hub_devinfo = r_usbh0_hstd_hci_get_device_infomation(hubaddr);

    /* Getting of HUB downport device information */
    p_devinfo = r_usbh0_hstd_hci_get_device_infomation(devaddr);

    if ((p_hub_devinfo != NULL) && (p_devinfo != NULL))
    {
        for (cnt = 0; cnt < USBH0_HCI_HUBPORT_NUM; cnt++)
        {
            if (0 == p_hub_devinfo->hubdevinfo.devnum[cnt])
            {
                p_hub_devinfo->hubdevinfo.devnum[cnt]     = (uint8_t)devaddr;
                p_hub_devinfo->hubdevinfo.devspeed[cnt]   = (uint8_t)p_devinfo->bit.devspeed;
                usbh0_hci_tmp_addr = devaddr;
                break;
            }
        }
    }
    else
    {
        /* Error */
        USBH0_HCI_PRINTF2_R("Error: unknown hubaddr( %d ) or devaddr( %d )\n", hubaddr, devaddr);
    }
} /* End of function r_usbh0_hstd_hci_set_device_address_of_hub_port() */

/*********************/
/* utility function  */
/*********************/
#ifdef USBH0_HOST_COMPLIANCE_MODE
void r_usbh0_hstd_hci_electrical_test(uint16_t rootport, uint8_t mode)
{
    switch (mode)
    {
        case 0:    /* Test SE0_NAK */
            USB00.PORTSC1.BIT.PortTestControl = 3;
            while(1);   /* This loops infinitely until it's reset. */
        break;

        case 1:    /* Test J */
            USB00.PORTSC1.BIT.PortTestControl = 1;
            while(1);   /* This loops infinitely until it's reset. */
        break;

        case 2:    /* Test K */
            USB00.PORTSC1.BIT.PortTestControl = 2;
            while(1);   /* This loops infinitely until it's reset. */
        break;

        case 3:    /* Signal Quality */
            USB00.PORTSC1.BIT.PortTestControl = 4;
            while(1);   /* This loops infinitely until it's reset. */
        break;

        case 4:    /* Suspend/Resume */
            r_usbh0_hstd_hci_wait_time(15000);      /* wait 15sec */
            USB00.PORTSC1.BIT.Suspend = 1;          /* Suspend */
            r_usbh0_hstd_hci_wait_time(15000);      /* wait 15sec */
            USB00.PORTSC1.BIT.ForcePortResume = 1;  /* Resume start */
            r_usbh0_hstd_hci_wait_time(20);         /* wait 20ms */
            USB00.PORTSC1.BIT.ForcePortResume = 0;  /* Resume end */
        break;

        case 5:   /* Packet Parameters */
            usbh0_hstd_electrical_test_mode(0x0108, rootport);
        break;

        case 6:   /* Chirp Timing */
            USB00.PORTSC1.BIT.Suspend = 1;
            if (1 == USB00.PORTSC1.BIT.CurrentConnectStatus)
            {
                USB00.USBINTR.LONG = 0x00000000;    /* Disable interrupt */
                r_usbh0_hstd_hci_port_reset(rootport);
                r_usbh0_hstd_hci_wait_time(15000);  /* wait 15sec */
            }

            if (1 == USB00.PORTSC1.BIT.CurrentConnectStatus)
            {
                r_usbh0_hstd_hci_port_reset(rootport);
            }
            USB00.USBINTR.LONG = 0x00000007;  /* PortChange, USBError, USBInt */
        break;

        default:
        break;
    }
} /* End of function r_usbh0_hstd_hci_electrical_test() */
#endif /* USBH0_HOST_COMPLIANCE_MODE */

/* waittimer_function for usbhstd */
void r_usbh0_hstd_hci_wait_time(uint32_t ms)
{
    uint32_t    prv = USB00.HCFMNUMBER.LONG;

//    USBH0_HCI_PRINTF1("Wait %d[ms]\n",ms);

    while(ms)
    {
        if ((USB00.HCFMNUMBER.LONG - prv) != 0)
        {
            ms--;
            prv = USB00.HCFMNUMBER.LONG;
        }
    }
} /* End of function r_usbh0_hstd_hci_wait_time() */


/***********************************************************************************************************************
* Function Name: usbh0_module_init
* Description  :
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void usbh0_module_init(void)
{
    volatile unsigned long  dummy;
    volatile uint8_t        dummy_buf;

    CPG.STBCR6.BYTE  &= 0xFD;                       /* Release module standby */
    dummy_buf   = CPG.STBCR6.BYTE;

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    USB00.PHYCLK_CTRL.LONG = 0x00000001;            /* USBPHY select : USB_X1 */
#else
    USB00.PHYCLK_CTRL.LONG = 0x00000000;            /* USBPHY select : EXTAL */
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    for(dummy = 0; dummy < (450 * 500); dummy++);   /* Wait 500us */

    USB00.COMMCTRL.BIT.OTG_PERI = 0;                /* Operation mode setting (0 : Host, 1 : Peri) */

    USB00.USBCTR.LONG &= ~0x00000002;               /* UTMI+PHY reset release */

    for(dummy = 0; dummy < (450 * 100); dummy++);   /* Wait 100us */

    /* AHB-PCI bridge Register setting */
    usbh0_ahb_pci_bridge_init();

    /* Core defined register setting */
    USB00.REGEN_CG_CTRL.BIT.NONUSE_CLK_MSK  = 1;
    USB00.REGEN_CG_CTRL.BIT.HOST_CLK_MSK    = 0;
    USB00.REGEN_CG_CTRL.BIT.PERI_CLK_MSK    = 0;

    USB00.SPD_CTRL.BIT.SUSPENDM_ENABLE      = 0;
    USB00.SPD_CTRL.BIT.SLEEPM_ENABLE        = 0;
    USB00.SPD_CTRL.BIT.WKCNNT_ENABLE        = 0;
    USB00.SPD_CTRL.BIT.GLOBAL_SUSPENDM_P1   = 0;

    /* Interrupt setting */
    R_INTC_RegistIntFunc(INTC_ID_USB_USBHI0, R_USBH0_isr);
    R_INTC_SetPriority(INTC_ID_USB_USBHI0, 5);
} /* End of function usbh0_module_init() */

/***********************************************************************************************************************
* Function Name: usbh0_hub_diconnect_delay
* Description  :
* Arguments    : none
* Return Value : none
***********************************************************************************************************************/
void usbh0_hub_diconnect_delay(void)
{
	uint16_t i=0;
	volatile static uint16_t usbh0_delay_flag = 0;

	while(i < USBH0_MAXDEVADDR)
	{
		if(USBH0_IFCLS_HUB == g_usbh0_hstd_device_drv[i].ifclass)
		{
			usbh0_delay_flag++;
			break;
		}
		i++;
	}

	if(2 <= usbh0_delay_flag)
	{
		R_USBH0_HstdDelayXms((uint16_t)66);
		usbh0_delay_flag = 0;
	}
}

/***********************************************************************************************************************
 Function Name   : r_usbh0_tusb320hi_host_setting
 Description     : tusb320hi Setting for Peri
 Arguments       : none
 Return value    : none
***********************************************************************************************************************/
void r_usbh0_tusb320hi_host_setting(void)
{

    int_t handle_riic2;
    st_r_drv_riic_transfer_t i2c_write;
    uint8_t write_buf = 0x20;

    handle_riic2 = open(DEVICE_INDENTIFIER "riic2", O_RDWR);
    if (0 > handle_riic2)
    {
        printf("riic2 open failed.\r\n");
    }

    i2c_write.device_address = TUSB320LI_DEV_ADDRESS;
    i2c_write.sub_address_type = RIIC_SUB_ADDR_WIDTH_8_BITS;
    i2c_write.sub_address = (uint32_t)0x0A;
    i2c_write.number_of_bytes = 1;
    i2c_write.p_data_buffer = (uint8_t *)&write_buf;

    if (DRV_ERROR >= control(handle_riic2, CTL_RIIC_WRITE, &i2c_write))
    {
        printf("riic2 read failed.\r\n");
    }

} /* End of function usbh0_tusb320hi_host_setting() */

#ifdef RZA2_USBH0_OHCI_IP
void r_usbh0_port_clean_seq(void)
{
	if(0 == USB00.HCRHDESCRIPTORA.BIT.NPS)
	{
		if(USB00.HCRHDESCRIPTORA.BIT.PSM & USB00.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* A */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
		}
		else
		{
			/* B */
			USB00.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
		}
	}
	else
	{
		if(USB00.HCRHDESCRIPTORA.BIT.PSM & USB00.HCRHDESCRIPTORB.BIT.PPCM)
		{
			/* C */
			USB00.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000200; /* LSDA = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000100; /* POCI = 1 */
			USB00.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}
		else
		{
			/* D */
			USB00.HCRHDESCRIPTORA.LONG &= 0xFFFFFDFF; /* NPS  = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00000001; /* PPS  = 1 */
			USB00.HCRHPORTSTATUS1.LONG |= 0x00000008; /* POCI = 1 */
			USB00.PORTSC1.LONG         |= 0x00002000; /* PortOwner = 1 */
			USB00.PORTSC1.LONG         &= 0xFFFFDFFF; /* PortOwner = 0 */
			USB00.HCRHSTATUS.LONG      |= 0x00010000; /* LPSC = 1 */
			USB00.HCRHDESCRIPTORA.LONG |= 0x00000200; /* NPS  = 0 */
		}
	}
}
#endif /* RZA2_USBH0_OHCI_IP */



/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
