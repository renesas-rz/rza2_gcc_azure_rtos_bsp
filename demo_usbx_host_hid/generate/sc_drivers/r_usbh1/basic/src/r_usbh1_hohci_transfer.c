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
* File Name    : r_usbh1_hOhciTransfer.c
* Description  : OHCI Transfer Code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh1_hhci_local.h"

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static void         usbh1_hstd_ohci_setup_request(st_usbh1_hci_tr_req_t *p_tr_req, st_usbh1_ohci_request_p_t ohci_req);
static uint32_t     usbh1_hstd_ohci_do_transfer_request(st_usbh1_ohci_hcd_device_data_p_t device_data,
                                                        uint8_t device_address, uint8_t endpoint_number,
                                                        uint8_t endpoint_type, uint16_t mps, uint8_t speed,
                                                        st_usbh1_ohci_request_p_t ohci_req, uint32_t pollrate);
static uint32_t     usbh1_hstd_ohci_check_ep(st_usbh1_ohci_hcd_endpoint_p_t endpoint, uint8_t endpoint_type,
                                             uint16_t mps, uint8_t speed);
static uint32_t     usbh1_hstd_ohci_queue_general_request(st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                                                          st_usbh1_ohci_request_p_t ohci_req);
static uint32_t     usbh1_hstd_ohci_queue_iso_request(st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                                                      st_usbh1_ohci_request_p_t ohci_req);
static uint32_t     usbh1_hstd_ohci_queue_iso_request_in(st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                                                         st_usbh1_ohci_request_p_t ohci_req);
static uint32_t     usbh1_hstd_ohci_queue_iso_request_out(st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                                                          st_usbh1_ohci_request_p_t ohci_req);
static uint32_t     usbh1_hstd_ohci_insert_ed_for_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint);
static void         usbh1_hstd_ohci_complete_transfer_request_iso(st_usbh1_ohci_hcd_endpoint_p_t endpoint);
static uint32_t     usbh1_hstd_ohci_open_periodic_pipe(st_usbh1_ohci_hcd_endpoint_p_t endpoint);
static uint32_t     usbh1_hstd_ohci_setup_endpoint(st_usbh1_ohci_hcd_device_data_p_t device_data,
                                                   st_usbh1_ohci_hcd_endpoint_p_t endpoint, uint8_t endpoint_type,
                                                   uint16_t mps, uint8_t devaddr, uint8_t epnum, uint8_t speed,
                                                   uint8_t direction, uint32_t pollrate);
static uint8_t      usbh1_hstd_ohci_min_delay(uint8_t delay1, uint8_t delay2);
static uint32_t     usbh1_hstd_ohci_check_transfer_request_end(st_usbh1_ohci_request_p_t ohci_req, uint32_t mps, uint16_t size);
static uint32_t     usbh1_hstd_ohci_cancel_request(st_usbh1_ohci_request_p_t ohci_req);
static st_usbh1_ohci_hcd_endpoint_p_t   usbh1_hstd_ohci_initialize_endpoint(st_usbh1_ohci_hcd_device_data_p_t device_data,
                                                                            uint8_t endpoint_type, uint16_t max_packet_size,
                                                                            uint8_t device_address, uint8_t endpoint_number,
                                                                            uint8_t speed, uint8_t direction, uint32_t pollrate);
static st_usbh1_ohci_list_entry_p_t     usbh1_hstd_ohci_virtual_address_of(uint32_t data);


static const uint32_t   gs_usbh1_hstd_ohci_cc_table[16] =
{
    USBH1_OHCI_RS_NORMAL_COMPLETION,     /* NoError */
    USBH1_OHCI_CC_CRC,                   /* CRC */
    USBH1_OHCI_CC_BITSTUFFING,           /* BitStuffing */
    USBH1_OHCI_CC_DATATOGGLEMISMATCH,    /* DataToggleMismatch */
    USBH1_OHCI_CC_STALL,                 /* Stall */
    USBH1_OHCI_CC_DEVICENOTRESPOND,      /* DeviceNotResponding */
    USBH1_OHCI_CC_PIDCHECKFAILURE,       /* PIDCheckFailure */
    USBH1_OHCI_CC_UNEXPECTEDPID,         /* UnexpectedPID */
    USBH1_OHCI_CC_DATAOVERRUN,           /* DataOverrun */
    USBH1_OHCI_CC_DATAUNDERRUN,          /* DataUnderrun */
    USBH1_OHCI_RS_NOT_DONE,
    USBH1_OHCI_RS_NOT_DONE,
    USBH1_OHCI_CC_BUFFEROVERRUN,         /* BufferOverrun */
    USBH1_OHCI_CC_BUFFERUNDERRUN,        /* BufferUnderrun */
    USBH1_OHCI_RS_NOT_DONE,              /* NotAccessed */
    USBH1_OHCI_RS_NOT_DONE               /* NotAccessed */
};

static const uint8_t    gs_usbh1_hstd_ohci_offset_psw_table[2][8] =
{
    { 0, 1, 2, 3, 4, 5, 6, 7 }, /* little endian */
    { 1, 0, 3, 2, 5, 4, 7, 6 }  /* big endian */
};

/***********************************************************************************************************************
 * Function     : OHCI Make Transfer Request
 * Declaration  : usbh1_hstd_ohci_make_transfer_request( st_usbh1_hci_tr_req_t *tr_req )
 * Argument     : st_usbh1_hci_tr_req_t *tr_req    ; Transfer Request Infomation
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_make_transfer_request(st_usbh1_hci_tr_req_t *p_tr_req)
{
    st_usbh1_ohci_request_p_t   ohci_req;
    uint32_t                    result;

    /* pollrate setting */
    if (USBH1_EP_INT == p_tr_req->bit.eptype)
    {
        p_tr_req->pollrate = usbh1_hstd_hci_get_pollrate(p_tr_req);
    }

    /* Make USBD_Request */
    ohci_req = usbh1_hstd_ohci_alloc_transfer_request();
    if (NULL == ohci_req)
    {
        return;
    }

    R_MMU_VAtoPA((uint32_t)ohci_req,&p_tr_req->hci_info);

    /* Convert Driver_Request to USBD_Request */
    usbh1_hstd_ohci_setup_request(p_tr_req, ohci_req);

    /* Send Request to HCD */
    result = usbh1_hstd_ohci_do_transfer_request(
                                                    (st_usbh1_ohci_hcd_device_data_p_t)(p_tr_req->devinfo->hci_device),
                                                    p_tr_req->bit.devadrs,
                                                    p_tr_req->bit.epnum,
                                                    p_tr_req->bit.eptype,
                                                    p_tr_req->bit.mps,
                                                    p_tr_req->devinfo->bit.devspeed,
                                                    ohci_req,
                                                    p_tr_req->pollrate
                                                );
    if (FALSE == result)
    {
        usbh1_hstd_ohci_free_transfer_request(ohci_req);
    }
}

/***********************************************************************************************************************
 * Function     : OHCI Chancel Transfer Request
 * Declaration  : usbh1_hstd_ohci_cancel_transfer_request( st_usbh1_hci_tr_req_t *tr_req )
 * Argument     : st_usbh1_hci_tr_req_t *tr_req ; Transfer Request Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_cancel_transfer_request(st_usbh1_hci_tr_req_t *p_tr_req)
{
    if (NULL == p_tr_req)
    {
        return;
    }

    if (TRUE == p_tr_req->bit.enable)
    {
        if (NULL != r_usbh1_pa_to_va((uint32_t)p_tr_req->hci_info))
        {
            p_tr_req->bit.cancel_req = TRUE;

            /* Cancel Transfer Reques */
            usbh1_hstd_ohci_cancel_request((st_usbh1_ohci_request_p_t)r_usbh1_pa_to_va(p_tr_req->hci_info));
        }
    }
}

/***********************************************************************************************************************
 * Function     : OHCI Process DoneQueue.
 * Declaration  : void usbh1_hstd_ohci_process_done_queue( uint32_t phys_hc_td )
 * Argument     : uint32_t phys_hc_td ; Physical Addres Hcca Done Head
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_process_done_queue(uint32_t phys_hc_td)
{
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tn;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       td_list = NULL;
    st_usbh1_ohci_request_p_t                       ohci_req;
    st_usbh1_ohci_hcd_endpoint_p_t                  endpoint;
    st_usbh1_ohci_hc_iso_transfer_descriptor_p_t    hc_iso_td;
    st_usbh1_ohci_iso_transfer_info_p_t             iso_tr_info;
    uint32_t                                        err;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tmp_hcd_head_p;
    uint32_t                                        tmp_hc_ed_head_p;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t       tmp_hcd_ed;

    /* Reverse the queue passed from controller while virtualizing addresses */
    /* NOTE: The following code assumes that a uint32_t and a pointer are the same size */

    if (phys_hc_td & 1)
    {
        phys_hc_td &= ~(uint32_t)1;
    }

    if(0 == phys_hc_td)
    {
        return;
    }

#if USBH1_OHCI_SHAREDMEMORYTYPE == USBH1_OHCI_SHAREDMEMORYTYPE_EXCLUSIVE
    phys_hc_td += USBH1_OHCI_SHAREDMEMORY_OFFSET;
    do
    {
        td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
            usbh1_hstd_ohci_virtual_address_of(phys_hc_td), USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);

        phys_hc_td  = td->hc_td.next_td;
        phys_hc_td += USBH1_OHCI_SHAREDMEMORY_OFFSET;

        if (NULL == td_list)
        {
            td->hc_td.next_td = NULL;
        }
        else
        {
            td->hc_td.next_td = (uint32_t)(td_list->physicaladdress + USBH1_OHCI_SHAREDMEMORY_OFFSET);
        }

        td_list = td;
    } while (USBH1_OHCI_SHAREDMEMORY_OFFSET != phys_hc_td);

#elif USBH1_OHCI_SHAREDMEMORYTYPE == USBH1_OHCI_SHAREDMEMORYTYPE_PCI
    phys_hc_td = r_usbh1_pa_to_va((uint32_t)phys_hc_td);
    do
    {
        td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
            usbh1_hstd_ohci_virtual_address_of(phys_hc_td), USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);

        if (0 != td->hc_td.next_td)
        {
            phys_hc_td = r_usbh1_pa_to_va((uint32_t)td->hc_td.next_td);
        }
        else
        {
            phys_hc_td = 0;
        }

        if (NULL == td_list)
        {
            td->hc_td.next_td = 0;
        }
        else
        {
            td->hc_td.next_td = r_usbh1_pa_to_va((uint32_t)td_list->physical_address);
        }

        td_list = td;
    } while (0 != phys_hc_td);

#else
    do
    {
        td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
            usbh1_hstd_ohci_virtual_address_of(phys_hc_td), USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);

        phys_hc_td = r_usbh1_pa_to_va((uint32_t)td->hc_td.next_td);
        if (NULL == td_list)
        {
            td->hc_td.next_td = NULL;
        }
        else
        {
            td->hc_td.next_td = (uint32_t)r_usbh1_pa_to_va((uint32_t)td_list->physical_address);
        }

        td_list = td;
    } while (0 != phys_hc_td);

#endif

    /* Process list that is now reordered to completion order */
    while (NULL != td_list)
    {
        td = td_list;
        if (0 != td->hc_td.next_td)
        {
            td_list = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)r_usbh1_pa_to_va(((td->hc_td.next_td) - 0x20));
        }
        else
        {
            td_list = NULL;
        }

        if (USBH1_OHCI_TD_CANCELED == td->td_status)
        {
            usbh1_hstd_ohci_free_transfer_descriptor(td);
            continue;
        }

        ohci_req = r_usbh1_pa_to_va(td->usb_drequest);
        endpoint = r_usbh1_pa_to_va(td->endpoint);

        if (USBH1_EP_ISO != endpoint->type)
        {
            if (0 != td->hc_td.cbp)
            {
                td->transfer_count -=
                    (((td->hc_td.be ^ td->hc_td.cbp) & 0xFFFFF000) ? 0x00001000 : 0) +
                    (td->hc_td.be & 0x00000FFF) - (td->hc_td.cbp & 0x00000FFF) + 1;
            }

            if (USBH1_OHCI_PID_SETUP != td->hc_td.control.dp)
            {
                ohci_req->buffer_length += td->transfer_count;
            }

            if (USBH1_OHCI_CC_NOERROR == td->hc_td.control.cc)
            {
                /* td completed without error, remove it from USBH1_OHCI_REQUEST list, */
                /* if USBH1_OHCI_REQUEST list is now empty, then complete the request. */

                endpoint->hcd_head_p = usbh1_hstd_ohci_physical_address_of(td->next_hcd_td);
                usbh1_hstd_ohci_remove_list_entry(&td->request_list);
                usbh1_hstd_ohci_free_transfer_descriptor(td);
                err = usbh1_hstd_ohci_is_list_empty(&ohci_req->hcd_list);
                if (TRUE == err)
                {
                    if (USBH1_OHCI_RS_CANCELING != ohci_req->status)
                    {
                        ohci_req->status = USBH1_OHCI_RS_NORMAL_COMPLETION;
                    }
                    else
                    {
                        ohci_req->status = USBH1_OHCI_RS_CANCELED;
                    }

                    usbh1_hstd_ohci_complete_transfer_request(ohci_req);
                }
            }
            else
            {
                /* TD completed with an error, remove it and other TDs for same request, */
                /* set appropriate status in USBH1_OHCI_REQUEST and then complete it. There */
                /* are two special cases: 1) error is DataUnderun on Bulk or Interrupt and */
                /* ShortXferOk is true; for this do not report error to USBD and restart */
                /* endpoint. 2) error is DataUnderrun on Control and ShortXferOk is true; */
                /* for this the final status TD for the ohci_req should not be canceled, the */
                /* ohci_req should not be completed, and the endpoint should be restarted. */
                /* NOTE: The endpoint has been halted by the controller */

                for (tn = r_usbh1_pa_to_va(endpoint->hcd_head_p); tn != r_usbh1_pa_to_va(endpoint->hcd_tail_p); tn = r_usbh1_pa_to_va(tn->next_hcd_td))
                {
                    if ((ohci_req != r_usbh1_pa_to_va(tn->usb_drequest))
                        || ((USBH1_OHCI_CC_DATAUNDERRUN == td->hc_td.control.cc)
                            && (0 != ohci_req->shortxferok)
                            && (USBH1_OHCI_RS_CANCELING != ohci_req->status)
                            && (td->hc_td.control.dp != tn->hc_td.control.dp))
                      )
                    {
                        break;
                    }
                }

                endpoint->hcd_head_p = usbh1_hstd_ohci_physical_address_of(tn);

                tmp_hcd_ed = r_usbh1_pa_to_va(endpoint->hcd_ed);
                tmp_hcd_ed->hc_ed.head_p = tn->physical_address |
                    (tmp_hcd_ed->hc_ed.head_p & (USBH1_OHCI_HCED_HEADP_HALT | USBH1_OHCI_HCED_HEADP_CARRY));

                /*  Delete TD about failed request  */
                while ((tn = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                            usbh1_hstd_ohci_remove_list_head(&ohci_req->hcd_list),
                            USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST)) != NULL)
                {
                    if ((tn != td) && (tn != r_usbh1_pa_to_va(endpoint->hcd_head_p)))
                    {
                        usbh1_hstd_ohci_free_transfer_descriptor(tn);
                    }
                }

                tmp_hc_ed_head_p = r_usbh1_pa_to_va((uint32_t)tmp_hcd_ed->hc_ed.head_p);
                tmp_hcd_head_p = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_head_p);

                if (tmp_hcd_head_p->usb_drequest == ohci_req)
                {
                    usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &tmp_hcd_head_p->request_list);
                    tmp_hcd_ed->hc_ed.head_p = usbh1_hstd_ohci_physical_address_of((tmp_hc_ed_head_p & ~USBH1_OHCI_HCED_HEADP_HALT));
                }
                else
                {
                    if ((0 != ohci_req->shortxferok) && (USBH1_OHCI_CC_DATAUNDERRUN == td->hc_td.control.cc))
                    {
                        if (USBH1_OHCI_RS_CANCELING != ohci_req->status)
                        {
                            ohci_req->status = USBH1_OHCI_RS_NORMAL_COMPLETION;
                        }
                        else
                        {
                            ohci_req->status = USBH1_OHCI_RS_CANCELED;
                        }

                        tmp_hcd_ed->hc_ed.head_p &= usbh1_hstd_ohci_physical_address_of((tmp_hc_ed_head_p & ~USBH1_OHCI_HCED_HEADP_HALT));
                    }
                    else if (USBH1_OHCI_RS_CANCELING != ohci_req->status)
                    {
                        ohci_req->status = gs_usbh1_hstd_ohci_cc_table[td->hc_td.control.cc];
                    }
                    else
                    {
                        ohci_req->status = USBH1_OHCI_RS_CANCELED;
                    }

                    usbh1_hstd_ohci_complete_transfer_request(ohci_req);
                }

                usbh1_hstd_ohci_free_transfer_descriptor(td);

                switch (endpoint->type)
                {
                    case USBH1_EP_CNTRL:
                        USB10.HCCOMMANDSTATUS.LONG = USBH1_OHCI_CONTROLLISTFILLED;
                    break;

                    case USBH1_EP_BULK:
                        USB10.HCCOMMANDSTATUS.LONG = USBH1_OHCI_BULKLISTFILLED;
                    break;

                    default:
                    break;
                }
            }
        }
        else
        {
            /* Code for Isochronous */
            hc_iso_td = (st_usbh1_ohci_hc_iso_transfer_descriptor_p_t)r_usbh1_pa_to_va(&td->hc_td);

            iso_tr_info = r_usbh1_pa_to_va(&endpoint->iso_info->transfer_info[endpoint->iso_info->end_p]);
            endpoint->iso_info->end_p++;
            endpoint->iso_info->end_p &= USBH1_OHCI_ISO_MAX_FRAME - 1;

            iso_tr_info->control_cc = hc_iso_td->control.cc;
            if (endpoint->control.d == (USBH1_HCI_DIRECTION_IN + 1))
            {
                if (1 == USBH1_HCI_GET_ENDIAN)
                {
                    iso_tr_info->size   = hc_iso_td->usb_ohci_offset_psw[1].psw.size;
                    iso_tr_info->psw_cc = hc_iso_td->usb_ohci_offset_psw[1].psw.cc;
                }
                else
                {
                    iso_tr_info->size   = hc_iso_td->usb_ohci_offset_psw[0].psw.size;
                    iso_tr_info->psw_cc = hc_iso_td->usb_ohci_offset_psw[0].psw.cc;
                }
            }
            else
            {
                if (1 == USBH1_HCI_GET_ENDIAN)
                {
                    iso_tr_info->size   -= hc_iso_td->usb_ohci_offset_psw[1].psw.size;
                    iso_tr_info->psw_cc  = hc_iso_td->usb_ohci_offset_psw[1].psw.cc;
                }
                else
                {
                    iso_tr_info->size   -= hc_iso_td->usb_ohci_offset_psw[0].psw.size;
                    iso_tr_info->psw_cc  = hc_iso_td->usb_ohci_offset_psw[0].psw.cc;
                }
            }

            endpoint->iso_info->start_cnt--;
            endpoint->iso_info->end_cnt++;

            endpoint->hcd_head_p = usbh1_hstd_ohci_physical_address_of(td->next_hcd_td);
            usbh1_hstd_ohci_remove_list_entry(&td->request_list);
            usbh1_hstd_ohci_free_transfer_descriptor(td);

            if (NULL != endpoint->iso_info->usb_drequest)
            {
                usbh1_hstd_ohci_complete_transfer_request_iso(endpoint);
            }
        }
    }
}

/***********************************************************************************************************************
 * Function     : OHCI Complete Transfer Request
 * Declaration  : void usbh1_hstd_ohci_complete_transfer_request( st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_complete_transfer_request(st_usbh1_ohci_request_p_t ohci_req)
{
    st_usbh1_hci_tr_req_t   *p_tr_req;
    uint16_t                status;

    if (FALSE == ohci_req->using_flag)
    {
        return;
    }

    p_tr_req = (st_usbh1_hci_tr_req_t *)r_usbh1_pa_to_va(ohci_req->driver_request);
    if (FALSE == p_tr_req->bit.enable)
    {
        return;
    }

    p_tr_req->status        = ohci_req->status;
    p_tr_req->actual_size  += ohci_req->buffer_length;

    if (USBH1_OHCI_RS_NORMAL_COMPLETION != p_tr_req->status)
    {
        USBH1_HCI_PRINTF0_R("Condition Code : ");
        if (USBH1_OHCI_RS_CANCELED == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("Request is Canceled\n");
        }
        else if (USBH1_OHCI_CC_CRC == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("CRC Error\n");
        }
        else if (USBH1_OHCI_CC_BITSTUFFING == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("BitStuffing Error\n");
        }
        else if (USBH1_OHCI_CC_DATATOGGLEMISMATCH == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("DataToggleMismatch Error\n");
        }
        else if (USBH1_OHCI_CC_STALL == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("Endpoint is Stalled\n");
        }
        else if (USBH1_OHCI_CC_DEVICENOTRESPOND == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("DeviceNotRespond\n");
        }
        else if (USBH1_OHCI_CC_PIDCHECKFAILURE == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("PIDCheckFailured\n");
        }
        else if (USBH1_OHCI_CC_UNEXPECTEDPID == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("UnexpectedPID\n");
        }
        else if (USBH1_OHCI_CC_DATAOVERRUN == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R("DataOverrun\n");
        }
        else if (USBH1_OHCI_CC_DATAUNDERRUN == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R( "DataUnderrun\n" );
        }
        else if (USBH1_OHCI_CC_BUFFEROVERRUN == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R( "BufferOverrun\n" );
        }
        else if (USBH1_OHCI_CC_BUFFERUNDERRUN == p_tr_req->status)
        {
            USBH1_HCI_PRINTF0_R( "BufferUnderrun\n" );
        }

        if (USBH1_EP_ISO != p_tr_req->bit.eptype)
        {
            usbh1_hstd_ohci_pause_endpoint(p_tr_req->bit.devadrs, p_tr_req->bit.epnum, ohci_req->xferinfo);
        }
    }

    /* Clear completed USBD_Request */
    usbh1_hstd_ohci_free_transfer_request(ohci_req);

    /* Set Transfer Status */
    status = usbh1_hstd_ohci_get_transfer_end_status(p_tr_req);

    p_tr_req->bit.enable = FALSE;

    g_usbh1_hstd_hci_callback.tr_end_cb(p_tr_req->utr_p, p_tr_req->actual_size, status);
}


/* Static Function */
/***********************************************************************************************************************
 * Function     : OHCI Setup Request
 * Declaration  : void usbh1_hstd_ohci_setup_request( st_usbh1_hci_tr_req_t *tr_req, st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_hci_tr_req_t *tr_req      ; HCI transfer request pointer
 *              : st_usbh1_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_setup_request(st_usbh1_hci_tr_req_t *p_tr_req, st_usbh1_ohci_request_p_t ohci_req)
{
    ohci_req->buffer        = (uint8_t *)p_tr_req->databuf;
    ohci_req->buffer_length = p_tr_req->trsize;
    ohci_req->xferinfo      = p_tr_req->bit.direction;
    ohci_req->max_int_delay = 0x00;
    ohci_req->shortxferok   = 0x01;

    if (0x00 == p_tr_req->bit.epnum)
    {
        ohci_req->setup = (uint8_t *)usbh1_hstd_ohci_physical_address_of(p_tr_req->setupbuf);
    }

    ohci_req->driver_request    = (uint32_t *)usbh1_hstd_ohci_physical_address_of(p_tr_req);
    ohci_req->status            = USBH1_OHCI_RS_NOT_REQUESTED;
}

/***********************************************************************************************************************
 * Function     : OHCI Do Transfer Request
 * Declaration  : uint32_t usbh1_hstd_ohci_do_transfer_request(
 *              : st_usbh1_ohci_hcd_device_data_p_t device_data,
 *              : uint8_t device_address,
 *              : uint8_t endpoint_number,
 *              : uint8_t endpoint_type,
 *              : uint16_t mps,
 *              : uint8_t speed,
 *              : st_usbh1_ohci_request_p_t ohci_req,
 *              : uint32_t pollrate )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data pointer
 *              : uint8_t device_address                 ; devide address
 *              : uint8_t endpoint_number                ; endpoint no
 *              : uint8_t endpoint_type                  ; endpoint type
 *              : uint16_t mps                           ; Max Packet Size
 *              : uint8_t speed                          ; device speed
 *              : st_usbh1_ohci_request_p_t ohci_req            ; Driver Request Infomation pointer
 *              : uint32_t pollrate                      ; polling rate
 * Return       : uint32_t                               ; result( TRUE:Nomal End, FALSE:Request Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_do_transfer_request(
    st_usbh1_ohci_hcd_device_data_p_t device_data,
    uint8_t device_address,
    uint8_t endpoint_number,
    uint8_t endpoint_type,
    uint16_t mps,
    uint8_t speed,
    st_usbh1_ohci_request_p_t ohci_req,
    uint32_t pollrate
)
{
    st_usbh1_ohci_hcd_endpoint_p_t  endpoint;
    uint32_t                        ret = FALSE;
    uint32_t                        err1;
    uint32_t                        err2;

    endpoint = usbh1_hstd_ohci_scan_endpoint(device_address, endpoint_number, ohci_req->xferinfo);
    if (NULL != endpoint)
    {
        err1 =usbh1_hstd_ohci_check_ep(endpoint, endpoint_type, mps, speed);
        if (FALSE == err1)
        {
            usbh1_hstd_ohci_remove_endpoint(device_address, endpoint_number, ohci_req->xferinfo, FALSE);
            endpoint = NULL;
        }
    }

    err1 = usbh1_hstd_ohci_check_remained_td();
    if (TRUE == err1)
    {
        if (NULL == endpoint)
        {
            err1 = usbh1_hstd_ohci_check_remained_ep();
            err2 = usbh1_hstd_ohci_check_remained_ed();
            if ((TRUE == err1) && (TRUE == err2))
            {
                endpoint = usbh1_hstd_ohci_initialize_endpoint(
                                                                device_data,        /* Device Data pointer */
                                                                endpoint_type,      /* endpoint type */
                                                                mps,                /* Max Packet Size */
                                                                device_address,     /* devide address */
                                                                endpoint_number,    /* endpoint no */
                                                                speed,              /* device speed */
                                                                ohci_req->xferinfo, /* direction */
                                                                pollrate            /* polling rate */
                                                             );
            }
        }

        if (NULL != endpoint)
        {
            if (USBH1_EP_ISO == endpoint_type)
            {
                ret = usbh1_hstd_ohci_queue_iso_request(endpoint, ohci_req);
            }
            else
            {
                ret = usbh1_hstd_ohci_queue_general_request(endpoint, ohci_req);
            }
        }
    }

    return ret;
}

/***********************************************************************************************************************
 * Function     : OHCI Check difference between received information
 * Declaration  : uint8_t usbh1_hstd_ohci_check_ep( st_usbh1_ohci_hcd_endpoint_p_t endpoint, uint8_t endpoint_type, uint16_t mps, uint8_t speed )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t ; endpoint pointer
 *              : uint8_t endpoint_type   ; endpoint type
 *              : uint16_t mps            ; max packet size
 *              : uint8_t speed           ; device speed
 * Return       : uint32_t                ; check result( TRUE:same, FALSE:difference )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_check_ep(st_usbh1_ohci_hcd_endpoint_p_t endpoint, uint8_t endpoint_type, uint16_t mps, uint8_t speed)
{
    uint8_t     ret = FALSE;

    if (NULL != endpoint)
    {
        if ((mps == endpoint->max_packet) && (speed == endpoint->control.s) && (endpoint_type == endpoint->type))
        {
            if (USBH1_EP_INT == endpoint_type)
            {
                switch (endpoint->rate)
                {
                    case 1:
                    case 2:
                    case 4:
                    case 8:
                    case 16:
                    case 32:
                        ret = TRUE;
                    break;

                    default:
                    break;
                }
            }
            else
            {
                ret = TRUE;
            }
        }
    }

    return ret;
}

/***********************************************************************************************************************
 * Function     : Generate TD
 * Declaration  : uint32_t usbh1_hstd_ohci_queue_general_request( st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 *              : st_usbh1_ohci_request_p_t ohci_req      ; Driver ohci_req Infomation pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_queue_general_request(st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   list_td = NULL;
    uint32_t                                    remaining_length;
    uint32_t                                    count;
    uint8_t                                     *p_current_buffer_pointer;

    st_usbh1_ohci_hcd_transfer_descriptor_p_t   tmp_next_hcd_td;

    ed = r_usbh1_pa_to_va(endpoint->hcd_ed);

    if ((NULL == ed) || (USBH1_OHCI_ED_EOF == ed->list_index))
    {
        /* endpoint has been removed from schedule */
        return FALSE;
    }

    td = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_tail_p);  /* hcd_head_p => hcd_tail_p */
    ohci_req->status = USBH1_OHCI_RS_NOT_DONE;
    remaining_length = ohci_req->buffer_length;
    ohci_req->buffer_length  = 0;                           /* Report back bytes transferred so far */
    p_current_buffer_pointer = r_usbh1_pa_to_va((uint32_t)ohci_req->buffer);
    usbh1_hstd_ohci_initialize_list_head(&ohci_req->hcd_list);

    if (USBH1_EP_CNTRL == endpoint->type)
    {
        /* Setup a TD for setup packet */
        usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &td->request_list);

        td->usb_drequest            = ohci_req;
        td->endpoint                = endpoint;
        td->cancel_pending          = FALSE;
        td->hc_td.cbp               = usbh1_hstd_ohci_physical_address_of(ohci_req->setup);
        td->hc_td.be                = usbh1_hstd_ohci_physical_address_of(ohci_req->setup + 7);
        td->transfer_count          = 0;
        td->hc_td.control.dp        = USBH1_OHCI_PID_SETUP;
        td->hc_td.control.toggle    = 2;
        td->hc_td.control.r         = TRUE;
        td->hc_td.control.int_d     = 7;    /* Specify no interrupt */
        td->hc_td.control.cc        = USBH1_OHCI_CC_NOTACCESSED;
        tmp_next_hcd_td             = usbh1_hstd_ohci_allocate_transfer_descriptor();
        td->next_hcd_td             = usbh1_hstd_ohci_physical_address_of(tmp_next_hcd_td);
        tmp_next_hcd_td->physical_address = td->hc_td.next_td = usbh1_hstd_ohci_physical_address_of(&tmp_next_hcd_td->hc_td);

        list_td = td;
        td = tmp_next_hcd_td;
    }

    /* Setup TD(s) for data packets */
    while ((0 != remaining_length) || (NULL == list_td))
    {
        usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &td->request_list);
        td->usb_drequest    = usbh1_hstd_ohci_physical_address_of(ohci_req);
        td->endpoint        = usbh1_hstd_ohci_physical_address_of(endpoint);
        td->cancel_pending  = FALSE;

        if (0 != remaining_length)
        {
            td->hc_td.cbp = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer);
            count = USBH1_OHCI_MAXTDSIZE - (td->hc_td.cbp & 0x00000FFF);
            if (count < remaining_length)
            {
            }
            else
            {
                count = remaining_length;
            }
            p_current_buffer_pointer += count - 1;
            td->hc_td.be = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer++);
        }
        else
        {
            td->hc_td.cbp = td->hc_td.be = count = 0;
        }

        td->transfer_count   = count;
        td->hc_td.control.dp = ohci_req->xferinfo + 1;

        if (USBH1_EP_CNTRL == endpoint->type)
        {
            td->hc_td.control.toggle = 3;
        }
        else
        {
            td->hc_td.control.toggle = 0;
        }

        if ((0 == (remaining_length -= count)) && (0 != ohci_req->shortxferok))
        {
            td->hc_td.control.r = TRUE;
        }
        else
        {
            td->hc_td.control.r = FALSE;
        }

        td->hc_td.control.int_d = 7;    /* Specify no interrupt */
        td->hc_td.control.cc    = USBH1_OHCI_CC_NOTACCESSED;

        tmp_next_hcd_td = usbh1_hstd_ohci_allocate_transfer_descriptor();
        td->next_hcd_td = usbh1_hstd_ohci_physical_address_of(tmp_next_hcd_td);
        tmp_next_hcd_td->physical_address = td->hc_td.next_td = usbh1_hstd_ohci_physical_address_of(&tmp_next_hcd_td->hc_td);

        list_td = td;
        td = tmp_next_hcd_td;
    }

    if (USBH1_EP_CNTRL == endpoint->type)
    {
        /* Setup TD for status phase */
        usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &td->request_list);

        td->endpoint        = usbh1_hstd_ohci_physical_address_of(endpoint);
        td->usb_drequest    = usbh1_hstd_ohci_physical_address_of(ohci_req);
        td->cancel_pending  = FALSE;
        td->hc_td.cbp       = 0;
        td->hc_td.be        = 0;
        td->transfer_count  = 0;

        if (USBH1_HCI_DIRECTION_IN == ohci_req->xferinfo)
        {
            td->hc_td.control.dp = USBH1_OHCI_PID_OUT;
        }
        else
        {
            td->hc_td.control.dp = USBH1_OHCI_PID_IN;
        }

        td->hc_td.control.toggle    = 3;
        td->hc_td.control.r         = FALSE;
        td->hc_td.control.int_d     = 7;    /* Specify no interrupt */
        td->hc_td.control.cc        = USBH1_OHCI_CC_NOTACCESSED;
        tmp_next_hcd_td             = usbh1_hstd_ohci_allocate_transfer_descriptor();
        td->next_hcd_td             = usbh1_hstd_ohci_physical_address_of(tmp_next_hcd_td);
        tmp_next_hcd_td->physical_address = td->hc_td.next_td = usbh1_hstd_ohci_physical_address_of(&tmp_next_hcd_td->hc_td);

        list_td = td;
        td = tmp_next_hcd_td;
    }

    /* Setup interrupt delay */
    list_td->hc_td.control.int_d = usbh1_hstd_ohci_min_delay(6, ohci_req->max_int_delay);

    /* Set new TailP in ED */
    td->usb_drequest = NULL;
    endpoint->hcd_tail_p = usbh1_hstd_ohci_physical_address_of(td);
    ed->hc_ed.tail_p = td->physical_address;   /* Add null TD to the tail */

    switch (endpoint->type)
    {
        case USBH1_EP_CNTRL:
            USB10.HCCOMMANDSTATUS.LONG = USBH1_OHCI_CONTROLLISTFILLED;
        break;

        case USBH1_EP_BULK:
            USB10.HCCOMMANDSTATUS.LONG = USBH1_OHCI_BULKLISTFILLED;
        break;

        case USBH1_EP_INT:
        break;

        default:
            return FALSE;
        break;
    }

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Isochronous TD
 * Declaration  : uint32_t usbh1_hstd_ohci_queue_iso_request( st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 *              ; st_usbh1_ohci_request_p_t ohci_req      ; Driver ohci_req Infomation pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_queue_iso_request(st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req)
{
    if (USBH1_HCI_DIRECTION_IN == ohci_req->xferinfo)
    {
        return usbh1_hstd_ohci_queue_iso_request_in(endpoint, ohci_req);
    }
    else
    {
        return usbh1_hstd_ohci_queue_iso_request_out(endpoint, ohci_req);
    }
}

/***********************************************************************************************************************
 * Function     : OHCI Isochronous TD for IN Transfer
 * Declaration  : uint32_t usbh1_hstd_ohci_queue_iso_request_in( st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 *              ; st_usbh1_ohci_request_p_t ohci_req      ; Driver ohci_req Infomation pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_queue_iso_request_in(st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t       ed;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       td;
    uint32_t                                        remaining_length;
    uint8_t                                         *p_current_buffer_pointer;
    uint32_t                                        buffer_pointer_tmp;
    st_usbh1_ohci_iso_transfer_info_p_t             iso_tr_info;
    st_usbh1_ohci_hc_iso_transfer_descriptor_p_t    hc_iso_td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       last_valid_iso_td;
    st_usbh1_ohci_hc_iso_transfer_descriptor_p_t    last_iso_hctd;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tmp_next_hcd_td_2;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tmp_td_next_hcd_td;

    ed = r_usbh1_pa_to_va(endpoint->hcd_ed);

    if (NULL == ed)
    {
        return FALSE;
    }

    if (USBH1_OHCI_ED_EOF == ed->list_index)
    {
        /* Endpoint has been removed from schedule */
        return FALSE;
    }

    if (NULL == endpoint->iso_info->usb_drequest)
    {
        /* The relations between Endpoint and usb_drequest are linked here */
        endpoint->iso_info->usb_drequest = ohci_req;
    }
    else
    {
        /* Error when new transfer request comes */
        return FALSE;
    }

    if (0 != endpoint->iso_info->end_cnt)
    {
        usbh1_hstd_ohci_complete_transfer_request_iso(endpoint);
        return TRUE;
    }

    td = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_tail_p);        /* hcd_head_p => hcd_tail_p */
    if (NULL == td)
    {
        return FALSE;
    }

    ohci_req->status = USBH1_OHCI_RS_NOT_DONE;
    ohci_req->buffer_length = 0;        /* report back bytes transferred so far */
    remaining_length = (USBH1_OHCI_ISO_MAX_FRAME - (endpoint->iso_info->start_cnt + endpoint->iso_info->end_cnt)) * endpoint->max_packet;
    usbh1_hstd_ohci_initialize_list_head(&ohci_req->hcd_list);

    /* Setup TD(s) for data packets */
    while (0 != remaining_length)
    {
        usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &td->request_list);
        td->usb_drequest    = NULL;
        td->endpoint        = usbh1_hstd_ohci_physical_address_of(endpoint);
        td->cancel_pending  = FALSE;

        hc_iso_td = (st_usbh1_ohci_hc_iso_transfer_descriptor_p_t)r_usbh1_pa_to_va((uint32_t)&td->hc_td);

        iso_tr_info             = &endpoint->iso_info->transfer_info[endpoint->iso_info->start_p];
        iso_tr_info->size       = 0;
        iso_tr_info->control_cc = 0;
        iso_tr_info->psw_cc     = 0;

        p_current_buffer_pointer = r_usbh1_pa_to_va(&iso_tr_info->buffer[0]);
        endpoint->iso_info->start_cnt++;
        endpoint->iso_info->start_p++;
        endpoint->iso_info->start_p &= USBH1_OHCI_ISO_MAX_FRAME - 1;

        hc_iso_td->bp0 = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer);
        if (remaining_length > endpoint->max_packet)
        {
            hc_iso_td->be = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer + endpoint->max_packet -1);
        }
        else
        {
            hc_iso_td->be = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer + remaining_length -1);
        }

        buffer_pointer_tmp = (uint32_t)p_current_buffer_pointer & 0x00000FFF;
        hc_iso_td->usb_ohci_offset_psw[gs_usbh1_hstd_ohci_offset_psw_table[USBH1_HCI_GET_ENDIAN][0]].offset.offset = usbh1_hstd_ohci_physical_address_of(buffer_pointer_tmp & 0x00001FFF);
        hc_iso_td->usb_ohci_offset_psw[gs_usbh1_hstd_ohci_offset_psw_table[USBH1_HCI_GET_ENDIAN][0]].offset.cc = 7;

        if(remaining_length > endpoint->max_packet)
        {
            td->transfer_count += endpoint->max_packet;
            remaining_length -= endpoint->max_packet;
        }
        else
        {
            td->transfer_count += remaining_length;
            remaining_length = 0;
        }

        hc_iso_td->control.int_d    = 7;    /* specify no interrupt */
        hc_iso_td->control.cc       = USBH1_OHCI_CC_NOTACCESSED;
        hc_iso_td->control.fc       = 0;
        hc_iso_td->control.int_d    = usbh1_hstd_ohci_min_delay(6, ohci_req->max_int_delay);    /* Setup interrupt delay */

        last_valid_iso_td = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_head_p);
        if (0 == last_valid_iso_td->next_hcd_td)       /* Valid IsoTD is Empty */
        {
            hc_iso_td->control.sf = USB10.HCFMNUMBER.LONG + 2;
        }
        else
        {
            tmp_next_hcd_td_2 = r_usbh1_pa_to_va((uint32_t)last_valid_iso_td->next_hcd_td);
            while (0 != tmp_next_hcd_td_2->next_hcd_td)
            {
                last_valid_iso_td = r_usbh1_pa_to_va(last_valid_iso_td->next_hcd_td);
                tmp_next_hcd_td_2 = r_usbh1_pa_to_va(last_valid_iso_td->next_hcd_td);
            }
            last_iso_hctd = (st_usbh1_ohci_hc_iso_transfer_descriptor_p_t)r_usbh1_pa_to_va(&last_valid_iso_td->hc_td);
            hc_iso_td->control.sf = last_iso_hctd->control.sf + last_iso_hctd->control.fc + 1;
        }

        td->next_hcd_td = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_allocate_transfer_descriptor());
        tmp_td_next_hcd_td = r_usbh1_pa_to_va((uint32_t)td->next_hcd_td);
        tmp_td_next_hcd_td->physical_address = td->hc_td.next_td = usbh1_hstd_ohci_physical_address_of(&tmp_td_next_hcd_td->hc_td);
        td = r_usbh1_pa_to_va((uint32_t)td->next_hcd_td);
    }

    /* Set new TailP in ED */
    td->usb_drequest = NULL;
    endpoint->hcd_tail_p = usbh1_hstd_ohci_physical_address_of(td);
    ed->hc_ed.tail_p = td->physical_address;   /* Add null TD to the tail */

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Isochronous TD for OUT Transfer
 * Declaration  : uint32_t usbh1_hstd_ohci_queue_iso_request_out( st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 *              ; st_usbh1_ohci_request_p_t ohci_req      ; Driver ohci_req Infomation pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_queue_iso_request_out(st_usbh1_ohci_hcd_endpoint_p_t endpoint, st_usbh1_ohci_request_p_t ohci_req)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t       ed;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       td;
    uint32_t                                        remaining_count;
    uint8_t                                         *p_current_buffer_pointer;
    uint32_t                                        buffer_pointer_tmp;
    uint32_t                                        total_size;
    st_usbh1_ohci_iso_transfer_info_p_t             iso_tr_info;
    st_usbh1_ohci_hc_iso_transfer_descriptor_p_t    hc_iso_td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       last_valid_iso_td;
    st_usbh1_ohci_hc_iso_transfer_descriptor_p_t    last_iso_hctd;
    uint8_t                                         *tmp_iso_tr_info_buffer;
    uint8_t                                         *tmp_ohci_req_buffer;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tmp_next_hcd_td_2;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t       tmp_td_next_hcd_td;

    ed = r_usbh1_pa_to_va(endpoint->hcd_ed);

    if (NULL == ed)
    {
        return FALSE;
    }

    if (USBH1_OHCI_ED_EOF == ed->list_index)
    {
        /* endpoint has been removed from schedule */
        return FALSE;
    }

    if (NULL == endpoint->iso_info->usb_drequest)
    {
        /* The relations between Endpoint and usb_drequest are linked here */
        endpoint->iso_info->usb_drequest = ohci_req;
    }
    else
    {
        /* Error when new transfer request comes */
        return FALSE;
    }

    /* Buffering */
    total_size = 0;
    while ((endpoint->iso_info->copy_cnt < USBH1_OHCI_ISO_MAX_FRAME) && (0 != ohci_req->buffer_length))
    {
        iso_tr_info = &endpoint->iso_info->transfer_info[endpoint->iso_info->copy_p];

        if (ohci_req->buffer_length > endpoint->max_packet)
        {
            iso_tr_info->size = endpoint->max_packet;
        }
        else
        {
            iso_tr_info->size = ohci_req->buffer_length;
        }

        iso_tr_info->control_cc = 0;
        iso_tr_info->psw_cc     = 0;

        tmp_iso_tr_info_buffer = r_usbh1_pa_to_va(&iso_tr_info->buffer[0]);
        tmp_ohci_req_buffer = r_usbh1_pa_to_va(ohci_req->buffer);
        memcpy(tmp_iso_tr_info_buffer, tmp_ohci_req_buffer, iso_tr_info->size);

        tmp_ohci_req_buffer += iso_tr_info->size;
        ohci_req->buffer = usbh1_hstd_ohci_physical_address_of(tmp_ohci_req_buffer);
        ohci_req->buffer_length  -= iso_tr_info->size;
        total_size += iso_tr_info->size;

        endpoint->iso_info->copy_cnt++;
        endpoint->iso_info->copy_p++;
        endpoint->iso_info->copy_p &= USBH1_OHCI_ISO_MAX_FRAME - 1;
    }

    /* The transfer request size buffering is completed */
    /* However, the transfer completion is notified when not having arrived at USBH1_OHCI_ISO_MAX_FRAME yet */
    /* As a result, the following forwarding is requested */
    if ((endpoint->iso_info->copy_cnt < USBH1_OHCI_ISO_MAX_FRAME)
            && (0 == ohci_req->buffer_length)
            && (0 == endpoint->iso_info->start_cnt))
    {
        ohci_req->buffer_length = total_size;

        /* The relation between Endpoint and usb_drequest is released here */
        endpoint->iso_info->usb_drequest = NULL;
        if (USBH1_OHCI_RS_CANCELING != ohci_req->status)
        {
            ohci_req->status = USBH1_OHCI_RS_NORMAL_COMPLETION;
        }
        else
        {
            ohci_req->status = USBH1_OHCI_RS_CANCELED;
        }

        usbh1_hstd_ohci_complete_transfer_request(ohci_req);
        return TRUE;
    }

    if (endpoint->iso_info->end_cnt)
    {
        usbh1_hstd_ohci_complete_transfer_request_iso(endpoint);
        return TRUE;
    }

    td = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_tail_p);    /* hcd_head_p => hcd_tail_p */
    ohci_req->status = USBH1_OHCI_RS_NOT_DONE;
    remaining_count = USBH1_OHCI_ISO_MAX_FRAME - endpoint->iso_info->start_cnt;
    usbh1_hstd_ohci_initialize_list_head(&ohci_req->hcd_list);

    /* Setup TD(s) for data packets */
    while (0 != remaining_count)
    {
        usbh1_hstd_ohci_insert_tail_list(&ohci_req->hcd_list, &td->request_list);
        td->usb_drequest    = NULL;
        td->endpoint        = usbh1_hstd_ohci_physical_address_of(endpoint);
        td->cancel_pending  = FALSE;

        hc_iso_td = (st_usbh1_ohci_hc_iso_transfer_descriptor_p_t)r_usbh1_pa_to_va((uint32_t)&td->hc_td);

        iso_tr_info = &endpoint->iso_info->transfer_info[endpoint->iso_info->start_p];
        p_current_buffer_pointer = r_usbh1_pa_to_va(&iso_tr_info->buffer[0]);
        endpoint->iso_info->start_cnt++;
        endpoint->iso_info->start_p++;
        endpoint->iso_info->start_p &= USBH1_OHCI_ISO_MAX_FRAME - 1;

        hc_iso_td->bp0  = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer);
        hc_iso_td->be   = usbh1_hstd_ohci_physical_address_of(p_current_buffer_pointer + iso_tr_info->size -1);

        buffer_pointer_tmp = (uint32_t)p_current_buffer_pointer & 0x00000FFF;
        hc_iso_td->usb_ohci_offset_psw[gs_usbh1_hstd_ohci_offset_psw_table[USBH1_HCI_GET_ENDIAN][0]].offset.offset = usbh1_hstd_ohci_physical_address_of(buffer_pointer_tmp & 0x00001FFF);
        hc_iso_td->usb_ohci_offset_psw[gs_usbh1_hstd_ohci_offset_psw_table[USBH1_HCI_GET_ENDIAN][0]].offset.cc = 7;
        td->transfer_count += iso_tr_info->size;

        endpoint->iso_info->copy_cnt--;
        remaining_count--;

        hc_iso_td->control.int_d    = 7;    /* specify no interrupt */
        hc_iso_td->control.cc       = USBH1_OHCI_CC_NOTACCESSED;
        hc_iso_td->control.fc       = 0;
        hc_iso_td->control.int_d    = usbh1_hstd_ohci_min_delay(6, ohci_req->max_int_delay);    /* Setup interrupt delay */

        last_valid_iso_td = r_usbh1_pa_to_va(endpoint->hcd_head_p);
        if (0 == last_valid_iso_td->next_hcd_td)    /* Valid IsoTD is Empty */
        {
            hc_iso_td->control.sf = USB10.HCFMNUMBER.LONG + 2;
        }
        else
        {
            tmp_next_hcd_td_2 = r_usbh1_pa_to_va((uint32_t)last_valid_iso_td->next_hcd_td);
            while (0 != tmp_next_hcd_td_2->next_hcd_td)
            {
                last_valid_iso_td = r_usbh1_pa_to_va(last_valid_iso_td->next_hcd_td);
                tmp_next_hcd_td_2 = r_usbh1_pa_to_va(last_valid_iso_td->next_hcd_td);
            }
            last_iso_hctd = (st_usbh1_ohci_hc_iso_transfer_descriptor_p_t)r_usbh1_pa_to_va(&last_valid_iso_td->hc_td);
            hc_iso_td->control.sf = last_iso_hctd->control.sf + last_iso_hctd->control.fc + 1;
        }

        td->next_hcd_td = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_allocate_transfer_descriptor());
        tmp_td_next_hcd_td = r_usbh1_pa_to_va((uint32_t)td->next_hcd_td);
        tmp_td_next_hcd_td->physical_address = td->hc_td.next_td = usbh1_hstd_ohci_physical_address_of(&tmp_td_next_hcd_td->hc_td);
        td = r_usbh1_pa_to_va((uint32_t)td->next_hcd_td);
    }

    /* Set new TailP in ED */
    td->usb_drequest = NULL;
    endpoint->hcd_tail_p = usbh1_hstd_ohci_physical_address_of(td);
    ed->hc_ed.tail_p = td->physical_address;   /* Add null TD to the tail */

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Inserting ED to Endpoint structure.
 * Declaration  : void usbh1_hstd_ohci_insert_ed_for_endpoint( st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_insert_ed_for_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
    st_usbh1_ohci_hcd_device_data_p_t           device_data;
    st_usbh1_ohci_hcd_ed_list_p_t               list;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   tail_ed;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   tmp_hcd_head_p;
    uint32_t                                    *tmp_list_physical_head;

    device_data = r_usbh1_pa_to_va(endpoint->device_data);
    list = r_usbh1_pa_to_va(&device_data->ed_list[endpoint->list_index]);

    /* Initialize an endpoint descriptor for this endpoint */
    ed = usbh1_hstd_ohci_allocate_endpoint_descriptor();
    if (NULL == ed)
    {
        return FALSE;
    }

    ed->endpoint            = usbh1_hstd_ohci_physical_address_of(endpoint);
    ed->list_index          = endpoint->list_index;
    ed->physical_address    = usbh1_hstd_ohci_physical_address_of(&ed->hc_ed);
    ed->hc_ed.control       = endpoint->control;

    endpoint->hcd_head_p = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_allocate_transfer_descriptor()); /* Make Dummy TD */
    if (NULL == endpoint->hcd_head_p)
    {
        usbh1_hstd_ohci_free_endpoint_descriptor(ed);
        return FALSE;
    }
    endpoint->hcd_tail_p = endpoint->hcd_head_p;

    tmp_hcd_head_p = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_head_p);  /* debug */
    ed->hc_ed.head_p = usbh1_hstd_ohci_physical_address_of(&tmp_hcd_head_p->hc_td);

    tmp_hcd_head_p->physical_address = ed->hc_ed.tail_p = ed->hc_ed.head_p;

    endpoint->hcd_ed = usbh1_hstd_ohci_physical_address_of(ed);
    tmp_hcd_head_p->usb_drequest = NULL;

    /* Link endpoint descriptor into HCD tracking queue */
    if ((USBH1_EP_ISO != endpoint->type) || (TRUE == usbh1_hstd_ohci_is_list_empty(&list->head)))
    {
        /* Control/Bulk/Interrupt(any case) , Iso(List is empty) */

        /* Link ED into head of ED list */
        usbh1_hstd_ohci_insert_head_list(&list->head, &ed->link);
        tmp_list_physical_head = r_usbh1_pa_to_va(list->physical_head);
        ed->hc_ed.next_ed = *tmp_list_physical_head;
        *tmp_list_physical_head = ed->physical_address;
    }
    else
    {
        /* Iso type ( List is not empty) */

        /* Link ED into tail of ED list */
        tail_ed = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(r_usbh1_pa_to_va(list->head.b_link),
            USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);

        usbh1_hstd_ohci_insert_tail_list(&list->head, &ed->link);
        ed->hc_ed.next_ed = 0;
        tail_ed->hc_ed.next_ed = ed->physical_address;
    }

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Complete Transfer Request for Isochronous
 * Declaration  : void usbh1_hstd_ohci_complete_transfer_request_iso( st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_complete_transfer_request_iso(st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
    st_usbh1_ohci_iso_transfer_info_p_t iso_tr_info;
    st_usbh1_ohci_request_p_t           ohci_req;
    uint32_t                            err;
    uint8_t                             *tmp_iso_tr_info_buffer;
    uint8_t                             *tmp_ohci_req_buffer;

    ohci_req = r_usbh1_pa_to_va((uint32_t)endpoint->iso_info->usb_drequest);

    iso_tr_info = &endpoint->iso_info->transfer_info[endpoint->iso_info->complete_p];
    endpoint->iso_info->complete_p++;
    endpoint->iso_info->complete_p &= USBH1_OHCI_ISO_MAX_FRAME - 1;
    if (USBH1_HCI_DIRECTION_IN == ohci_req->xferinfo)
    {
        tmp_iso_tr_info_buffer = r_usbh1_pa_to_va(iso_tr_info->buffer);
        tmp_ohci_req_buffer = r_usbh1_pa_to_va(ohci_req->buffer);
        memcpy(tmp_ohci_req_buffer, tmp_iso_tr_info_buffer, iso_tr_info->size);
    }

    ohci_req->buffer_length += iso_tr_info->size;
    endpoint->iso_info->end_cnt--;

    /* Check Transfer Request End */
    err = usbh1_hstd_ohci_check_transfer_request_end(ohci_req, endpoint->max_packet, iso_tr_info->size);
    if (TRUE == err)
    {
        /* The relation between Endpoint and usb_drequest is released here */
        endpoint->iso_info->usb_drequest = NULL;
        if (USBH1_OHCI_RS_CANCELING != ohci_req->status)
        {
            if (USBH1_OHCI_CC_NOERROR == iso_tr_info->control_cc)
            {
                ohci_req->status = USBH1_OHCI_RS_NORMAL_COMPLETION;
            }
            else
            {
                ohci_req->status = gs_usbh1_hstd_ohci_cc_table[iso_tr_info->control_cc];
            }
        }
        else
        {
            ohci_req->status = USBH1_OHCI_RS_CANCELED;
        }

        usbh1_hstd_ohci_complete_transfer_request(ohci_req);
    }
}

/***********************************************************************************************************************
 * Function     : OHCI Open periodic pipe
 * Declaration  : uint32_t usbh1_hstd_ohci_open_periodic_pipe( st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_open_periodic_pipe(st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
    uint32_t                            which_list;
    uint32_t                            junk;
    st_usbh1_ohci_hcd_device_data_p_t   device_data;

    device_data = r_usbh1_pa_to_va(endpoint->device_data);

    /* Determine the scheduling period  */
    which_list = USBH1_OHCI_ED_INTERRUPT_32ms;
    while ((which_list >= endpoint->rate) && (which_list >>= 1))
    {
        continue;
    }

    /* Commit this endpoints bandwidth to the proper scheduling slot */
    if (USBH1_OHCI_ED_ISOCHRONOUS == which_list)
    {
        device_data->ed_list[USBH1_OHCI_ED_ISOCHRONOUS].bandwidth += endpoint->bandwidth;
        device_data->max_bandwidth_inuse += endpoint->bandwidth;
    }
    else
    {
        /* Determine which scheduling list has the least bandwidth */
        usbh1_hstd_ohci_check_bandwidth(device_data, which_list, &which_list);
        device_data->ed_list[which_list].bandwidth += endpoint->bandwidth;

        /* Recalculate the max bandwidth which is in use */
        /* This allows 1ms (isoc) pipe opens to occur with few calculation */
        device_data->max_bandwidth_inuse = usbh1_hstd_ohci_check_bandwidth(device_data, USBH1_OHCI_ED_INTERRUPT_32ms, &junk);
    }

    /* Verify the new max has not overcomitted the USB */
    if (device_data->max_bandwidth_inuse > device_data->available_bandwidth)
    {
        /* Too much, back this bandwidth out and fail the request */
        device_data->ed_list[which_list].bandwidth -= endpoint->bandwidth;
        device_data->max_bandwidth_inuse = usbh1_hstd_ohci_check_bandwidth(device_data, USBH1_OHCI_ED_INTERRUPT_32ms, &junk);

        return FALSE;
    }

    /* Assign endpoint to list and open pipe */
    endpoint->list_index = which_list;

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Initialize Endpoint
 * Declaration  : st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_initialize_endpoint(
                : st_usbh1_ohci_hcd_device_data_p_t device_data,
                : uint8_t endpoint_type,
                : uint16_t max_packet_size,
                : uint8_t device_address,
                : uint8_t endpoint_number,
                : uint8_t speed,
                : uint8_t direction,
                : uint32_t pollrate )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data pointer
 *              : uint8_t endpoint_type                  ; endpoint type
 *              : uint16_t max_packet_size               ; Max Packet Size
 *              : uint8_t device_address                 ; devide address
 *              : uint8_t endpoint_number                ; endpoint no
 *              : uint8_t speed                          ; device speed
 *              : uint8_t direction                      ; direction
 *              : uint32_t pollrate                      ; polling rate
 * Return       : st_usbh1_ohci_hcd_endpoint_p_t endpoint       ; endpoint pointer
 * Note         :
***********************************************************************************************************************/
static st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_initialize_endpoint(
    st_usbh1_ohci_hcd_device_data_p_t device_data,
    uint8_t endpoint_type,
    uint16_t max_packet_size,
    uint8_t device_address,
    uint8_t endpoint_number,
    uint8_t speed,
    uint8_t direction,
    uint32_t pollrate
)
{
    st_usbh1_ohci_hcd_endpoint_p_t  endpoint;
    uint32_t                        ret;

    /* Create EndPoint */
    endpoint = usbh1_hstd_ohci_alloc_endpoint();
    if (NULL == endpoint)
    {
        return NULL;
    }

    ret = usbh1_hstd_ohci_setup_endpoint(device_data, endpoint, endpoint_type, max_packet_size,
                                            device_address, endpoint_number, speed, direction, pollrate);

    if (FALSE == ret)
    {
        usbh1_hstd_ohci_free_endpoint(endpoint);
        return NULL;
    }

    if ((USBH1_EP_INT == endpoint->type) || (USBH1_EP_ISO == endpoint->type))
    {
        ret = usbh1_hstd_ohci_open_periodic_pipe(endpoint);
        if (FALSE == ret)
        {
            return NULL;
        }
    }

    /* Add to Host Controller schedule processing */
    ret = usbh1_hstd_ohci_insert_ed_for_endpoint(endpoint);
    if (TRUE == ret)
    {
        usbh1_hstd_ohci_store_endpoint(device_address, endpoint_number, direction, endpoint);
    }
    else
    {
        return NULL;
    }

    return endpoint;
}

/***********************************************************************************************************************
 * Function     : OHCI Setup Endpoint Memory
 * Declaration  : st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_setup_endpoint(
                :   st_usbh1_ohci_hcd_device_data_p_t device_data,
                :   st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                :   uint8_t endpoint_type,
                :   uint16_t mps,
                :   uint8_t devaddr,
                :   uint8_t epnum,
                :   uint8_t speed,
                :   uint8_t direction,
                :   uint32_t pollrate )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data pointer
 *              : st_usbh1_ohci_hcd_endpoint_p_t endpoint       ; endpoint pointer
 *              : uint8_t endpoint_type                  ; endpoint type
 *              : uint16_t mps                           ; Max Packet Size
 *              : uint8_t devaddr                        ; devide address
 *              : uint8_t epnum                          ; endpoint no
 *              : uint8_t speed                          ; device speed
 *              : uint8_t direction                      ; direction
 *              : uint32_t pollrate                      ; polling rate
 * Return       : uint32_t                               ; status
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_setup_endpoint(
    st_usbh1_ohci_hcd_device_data_p_t device_data,
    st_usbh1_ohci_hcd_endpoint_p_t endpoint,
    uint8_t endpoint_type,
    uint16_t mps,
    uint8_t devaddr,
    uint8_t epnum,
    uint8_t speed,
    uint8_t direction,
    uint32_t pollrate
)
{
    uint8_t     endpoint_descriptor_type;

    endpoint->device_data = usbh1_hstd_ohci_physical_address_of((uint32_t)device_data);

    switch (endpoint_type)
    {
        case USBH1_EP_CNTRL:
            endpoint_descriptor_type = USBH1_OHCI_ED_CONTROL;
            endpoint->type = endpoint_type;
        break;

        case USBH1_EP_BULK:
            endpoint_descriptor_type = USBH1_OHCI_ED_BULK;
            endpoint->type = endpoint_type;
        break;

        case USBH1_EP_ISO:
            endpoint_descriptor_type = USBH1_OHCI_ED_ISOCHRONOUS;
            endpoint->type      = endpoint_type;
            endpoint->iso_info  = usbh1_hstd_ohci_alloc_endpoint_iso();
            if (NULL == endpoint->iso_info)
            {
                return FALSE;
            }
        break;

        case USBH1_EP_INT:
            endpoint->type = USBH1_EP_INT;
            if (pollrate < (2 * 8))
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_1ms;
                endpoint->rate = 1;
            }
            else if(pollrate < (4 * 8))
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_2ms;
                endpoint->rate = 2;
            }
            else if(pollrate < (8 * 8))
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_4ms;
                endpoint->rate = 4;
            }
            else if(pollrate < (16 * 8))
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_8ms;
                endpoint->rate = 8;
            }
            else if(pollrate < (32 * 8))
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_16ms;
                endpoint->rate = 16;
            }
            else
            {
                endpoint_descriptor_type = USBH1_OHCI_ED_INTERRUPT_32ms;
                endpoint->rate = 32;
            }
            break;

        default:
            return FALSE;
    }

    endpoint->list_index    = endpoint_descriptor_type;
    endpoint->max_packet    = mps;
    endpoint->control.mps   = mps;
    endpoint->control.fa    = devaddr;
    endpoint->control.en    = epnum;
    endpoint->control.s     = speed;

    if (USBH1_EP_ISO == endpoint_type)
    {
        endpoint->control.d = direction + 1;
        endpoint->control.f = 1;
    }
    else
    {
        endpoint->bandwidth = mps;
    }

    return TRUE;
}

/***********************************************************************************************************************
 * Function     : OHCI Return VirtualAddress
 * Declaration  : st_usbh1_ohci_list_entry_p_t usbh1_hstd_ohci_virtual_address_of( uint32_t data )
 * Argument     : uint32_t data         ; address
 * Return       : st_usbh1_ohci_list_entry_p_t ; Virtual Address
 * Note         :
***********************************************************************************************************************/
static st_usbh1_ohci_list_entry_p_t usbh1_hstd_ohci_virtual_address_of(uint32_t data)
{
    return (st_usbh1_ohci_list_entry_p_t)(data - 20);
}

/***********************************************************************************************************************
 * Function     : OHCI Judge minimum value
 * Declaration  : uint8_t usbh1_hstd_ohci_min_delay( uint8_t delay1, uint8_t delay2 )
 * Argument     : uint8_t delay1 ; value1
 *              : uint8_t delay2 ; value2
 * Return       : uint8_t        ; Minimum value
 * Note         :
***********************************************************************************************************************/
static uint8_t usbh1_hstd_ohci_min_delay(uint8_t delay1, uint8_t delay2)
{
    if (delay1 <= delay2)
    {
        return delay1;
    }

    return delay2;
}

/***********************************************************************************************************************
 * Function     : OHCI Check Transfer Request End
 * Declaration  : uint32_t usbh1_hstd_ohci_check_transfer_request_end( st_usbh1_ohci_request_p_t ohci_req, uint32_t mps, uint16_t size )
 * Argument     : st_usbh1_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 *              : uint32_t mps                ; max packet size
 *              : uint16_t size               ; transfer size
 * Return       : uint32_t                    ; check flag( TRUE:End, FALSE:Not End )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_check_transfer_request_end(st_usbh1_ohci_request_p_t ohci_req, uint32_t mps, uint16_t size)
{
    st_usbh1_hci_tr_req_t   *p_tr_req;

    if (FALSE == ohci_req->using_flag)
    {
        return FALSE;
    }

    p_tr_req = (st_usbh1_hci_tr_req_t *)r_usbh1_pa_to_va((uint32_t)ohci_req->driver_request);

    /* check short packet */
    if (mps > size)
    {
        return TRUE;
    }

    /* check transfer request size */
    if (p_tr_req->trsize <= (p_tr_req->actual_size + size))
    {
        return TRUE;
    }

    return FALSE;
}

/***********************************************************************************************************************
 * Function     : OHCI Cancel Transfer Request
 * Declaration  : uint32_t usbh1_hstd_ohci_cancel_request( st_usbh1_ohci_request_p_t ohci_req )
 * Argument     : st_usbh1_ohci_request_p_t ohci_req      ; Driver ohci_req Infomation pointer
 * Return       : uint32_t                         ; result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_cancel_request(st_usbh1_ohci_request_p_t ohci_req)
{
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   td;
    st_usbh1_ohci_hcd_endpoint_p_t              endpoint;
    st_usbh1_ohci_list_entry_p_t                tmp_request_list;
    st_usbh1_ohci_list_entry_p_t                tmp_b_link;

    /* If request status is 'not done' set status to 'canceling' */
    if (USBH1_OHCI_RS_NOT_DONE != ohci_req->status)
    {
        return FALSE;   /* cannot cancel a completed request */
    }

    ohci_req->status = USBH1_OHCI_RS_CANCELING;

//    td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
//            ohci_req->hcd_list.f_link, USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);
    td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
            r_usbh1_pa_to_va(ohci_req->hcd_list.f_link), USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);

    if (NULL == td)
    {
        return TRUE;
    }

    while (1)
    {
        tmp_request_list = r_usbh1_pa_to_va((uint32_t)&td->request_list);
        tmp_b_link = r_usbh1_pa_to_va((uint32_t)ohci_req->hcd_list.b_link);

        td->cancel_pending = TRUE;
        if (tmp_request_list == tmp_b_link)
        {
            break;
        }

//        td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
//                td->request_list.f_link, USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);
        td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
        		r_usbh1_pa_to_va(td->request_list.f_link), USBH1_OHCI_CR_TRANSFER_DESCRIPTOR, USBH1_OHCI_CR_REQUESTLIST);

        if (NULL == td)
        {
            return TRUE;
        }

    }

    endpoint = td->endpoint;
    usbh1_hstd_ohci_pause_ed(endpoint);    /* stop endpoint, schedule cleanup (see Section 5.2.7.1.3) */

    return TRUE;
}

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
