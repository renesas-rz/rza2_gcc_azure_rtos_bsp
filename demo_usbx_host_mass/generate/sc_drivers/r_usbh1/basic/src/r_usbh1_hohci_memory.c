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
* File Name    : r_usbh1_hOhciMemory.c
* Description  : OHCI Memory Control Code
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
#ifdef __ICCARM__
#pragma location="OHCI_HCCA_1"
static st_usbh1_ohci_hcca_block_t               gs_usbh1_hstd_ohci_hcca;
/* 256 byte-boundary Area */
#pragma location="OHCI_TD_1"
static st_usbh1_ohci_hcd_transfer_descriptor_t  gs_usbh1_hstd_ohci_td_memory[USBH1_OHCI_NUM_TD];    /* 32 bit-boundary Area */
#pragma location="OHCI_ED_1"
static st_usbh1_ohci_hcd_endpoint_descriptor_t  gs_usbh1_hstd_ohci_ed_memory[USBH1_OHCI_NUM_ED];    /* 16 bit-boundary Area */
#pragma location="USB_HOST_1"
uint8_t      gs_usbh1_hstd_ohci_iso_buffer[USBH1_OHCI_ISO_MAXDEVICE][USBH1_OHCI_ISO_MAX_FRAME][USBH1_OHCI_ISO_MAX_PACKET_SIZE];
#endif  /* __ICCARM__ */

#ifdef __GNUC__
st_usbh1_ohci_hcca_block_t                  gs_usbh1_hstd_ohci_hcca
        __attribute__ ((section ("OHCI_HCCA_1")));
/* 256 byte-boundary Area */
st_usbh1_ohci_hcd_transfer_descriptor_t     gs_usbh1_hstd_ohci_td_memory[USBH1_OHCI_NUM_TD]    /* 32 bit-boundary Area */
        __attribute__ ((section ("OHCI_TD_1")));

st_usbh1_ohci_hcd_endpoint_descriptor_t     gs_usbh1_hstd_ohci_ed_memory[USBH1_OHCI_NUM_ED]    /* 16 bit-boundary Area */
        __attribute__ ((section ("OHCI_ED_1")));

uint8_t      gs_usbh1_hstd_ohci_iso_buffer[USBH1_OHCI_ISO_MAXDEVICE][USBH1_OHCI_ISO_MAX_FRAME][USBH1_OHCI_ISO_MAX_PACKET_SIZE]
        __attribute__ ((section ("USB_HOST_1")));

st_usbh1_ohci_request_t             gs_usbh1_hstd_ohci_request_array[USBH1_OHCI_NUM_REQUEST]
        __attribute__ ((section ("USB_HOST_1")));

st_usbh1_ohci_hcd_device_data_t     gs_usbh1_hstd_ohci_device_data_memory[USBH1_OHCI_NUM_DEVICEDATA]
        __attribute__ ((section ("USB_HOST_1")));

st_usbh1_ohci_hcd_endpoint_t        gs_usbh1_hstd_ohci_endpoint_memory[USBH1_OHCI_NUM_ENDPOINT]
        __attribute__ ((section ("OHCI_ED2_1")));

st_usbh1_ohci_hcd_endpoint_p_t      gs_usbh1_hstd_ohci_enable_endpoint[USBH1_OHCI_MAXDEVICE][USBH1_OHCI_MAXENDPOINT][2]
        __attribute__ ((section ("USB_HOST_1")));

st_usbh1_ohci_iso_info_t            gs_usbh1_hstd_ohci_iso_endpoint[USBH1_OHCI_ISO_MAXDEVICE]
        __attribute__ ((section ("USB_HOST_1")));

static uint32_t     gs_usbh1_hstd_ohci_remain_dd __attribute__ ((section ("USB_HOST_INIT_1"))) = USBH1_OHCI_NUM_DEVICEDATA;   /* Remaining DeviceData */
static uint32_t     gs_usbh1_hstd_ohci_remain_ep __attribute__ ((section ("USB_HOST_INIT_1"))) = USBH1_OHCI_NUM_ENDPOINT;     /* Remaining Endpoint */
static uint32_t     gs_usbh1_hstd_ohci_remain_ed __attribute__ ((section ("USB_HOST_INIT_1"))) = USBH1_OHCI_NUM_ED;           /* Remaining EndpointDescriptor */
static uint32_t     gs_usbh1_hstd_ohci_remain_td __attribute__ ((section ("USB_HOST_INIT_1"))) = USBH1_OHCI_NUM_TD;           /* Remaining TransferDescriptor */

#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "OHCI_HCCA_1"
static st_usbh1_ohci_hcca_block_t                  gs_usbh1_hstd_ohci_hcca;
#pragma arm section zidata
/* 256 byte-boundary Area */
#pragma arm section zidata = "OHCI_TD_1"
static st_usbh1_ohci_hcd_transfer_descriptor_t     gs_usbh1_hstd_ohci_td_memory[USBH1_OHCI_NUM_TD];    /* 32 bit-boundary Area */
#pragma arm section zidata
#pragma arm section zidata = "OHCI_ED_1"
static st_usbh1_ohci_hcd_endpoint_descriptor_t     gs_usbh1_hstd_ohci_ed_memory[USBH1_OHCI_NUM_ED];    /* 16 bit-boundary Area */
#pragma arm section zidata
#pragma arm section zidata = "USB_HOST_1"
uint8_t      gs_usbh1_hstd_ohci_iso_buffer[USBH1_OHCI_ISO_MAXDEVICE][USBH1_OHCI_ISO_MAX_FRAME][USBH1_OHCI_ISO_MAX_PACKET_SIZE];
#pragma arm section zidata
#pragma arm section zidata
#endif  /* __CC_ARM */

/***********************************************************************************************************************
 * Function     : OHCI Getting of HCCA Address
 * Declaration  : st_usbh1_ohci_hcca_block_p_t usbh1_hstd_ohci_get_hcca_address( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_hcca_block_p_t ; HCCA Address pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcca_block_p_t usbh1_hstd_ohci_get_hcca_address(void)
{
    return &gs_usbh1_hstd_ohci_hcca;
} /* End of function usbh1_hstd_ohci_get_hcca_address() */


/***********************************************************************************************************************
 * Function     : OHCI Initialize Request-Memory
 * Declaration  : void usbh1_hstd_ohci_init_request_memory( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_init_request_memory( void )
{
    usbh1_hstd_hci_lock_resouce();

    memset(&gs_usbh1_hstd_ohci_request_array[0], 0, sizeof(gs_usbh1_hstd_ohci_request_array));

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_init_request_memory() */

/***********************************************************************************************************************
 * Function     : OHCI Initialize Device Data Memory
 * Declaration  : void usbh1_hstd_ohci_init_device_data( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_init_device_data(void)
{
    usbh1_hstd_hci_lock_resouce();

    /*  Clear all DeviceData */
    memset(&gs_usbh1_hstd_ohci_device_data_memory[0], 0, sizeof(gs_usbh1_hstd_ohci_device_data_memory));

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_init_device_data() */

/***********************************************************************************************************************
 * Function     : OHCI Clear List
 * Declaration  : void usbh1_hstd_ohci_clear_list( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_clear_list(void)
{
    usbh1_hstd_hci_lock_resouce();

    /*  Clear all Endpoints */
    memset(&gs_usbh1_hstd_ohci_endpoint_memory[0], 0, sizeof(gs_usbh1_hstd_ohci_endpoint_memory));
    
    /*  Clear all EndpointDescriptors */
    memset(&gs_usbh1_hstd_ohci_ed_memory[0], 0, sizeof(gs_usbh1_hstd_ohci_ed_memory));
    
    /*  Clear all TransferDescriptors */
    memset(&gs_usbh1_hstd_ohci_td_memory[0], 0, sizeof(gs_usbh1_hstd_ohci_td_memory));
    
    /*  Clear gs_usbh1_hstd_ohci_enable_endpoint[][] */
    memset(&gs_usbh1_hstd_ohci_enable_endpoint[0], 0, sizeof(gs_usbh1_hstd_ohci_enable_endpoint));

    gs_usbh1_hstd_ohci_remain_ep = USBH1_OHCI_NUM_ENDPOINT;
    gs_usbh1_hstd_ohci_remain_ed = USBH1_OHCI_NUM_ED;
    gs_usbh1_hstd_ohci_remain_td = USBH1_OHCI_NUM_TD;

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_clear_list() */

/***********************************************************************************************************************
 * Function     : OHCI Create Transfer Request
 * Declaration  : st_usbh1_ohci_request_p_t usbh1_hstd_ohci_alloc_transfer_request( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_request_p_t usbh1_hstd_ohci_alloc_transfer_request(void)
{
    st_usbh1_ohci_request_p_t   ohci_req;
    st_usbh1_ohci_request_p_t   ret_req = NULL;
    uint32_t                    n;

    usbh1_hstd_hci_lock_resouce();

    /* Allocate Request */
    ohci_req = &gs_usbh1_hstd_ohci_request_array[0];
    for (n = 0; n < USBH1_OHCI_NUM_REQUEST; n++, ohci_req++)
    {
        if (FALSE == ohci_req->using_flag)
        {
            memset(ohci_req, 0, sizeof(st_usbh1_ohci_request_t));
            ohci_req->using_flag = TRUE;
            ret_req = ohci_req;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_req;
} /* End of function usbh1_hstd_ohci_alloc_transfer_request() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Device Data Memory
 * Declaration  : st_usbh1_ohci_hcd_device_data_p_t usbh1_hstd_ohci_alloc_device_data( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_hcd_device_data_p_t ; Device Data Pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcd_device_data_p_t usbh1_hstd_ohci_alloc_device_data(void)
{
    st_usbh1_ohci_hcd_device_data_p_t   device_data;
    st_usbh1_ohci_hcd_device_data_p_t   ret_device_data = NULL;
    uint32_t                            n;
    uint32_t                            m;

    usbh1_hstd_hci_lock_resouce();

    /* Allocate device_data */
    device_data = &gs_usbh1_hstd_ohci_device_data_memory[0];
    for (n = 0; n < USBH1_OHCI_NUM_DEVICEDATA; n++, device_data++)
    {
        if (FALSE == device_data->using_flag)
        {
            /* 0 clear */
            memset(device_data, 0, sizeof(st_usbh1_ohci_hcd_device_data_t));
            ret_device_data = device_data;
            
            /* Initialize Device Data */
            device_data->using_flag             = TRUE;
            device_data->available_bandwidth    = 0x2778;

            for (m = 0; m < USBH1_OHCI_MAXROOTPORTS; m++)
            {
                device_data->root_device[m] = USBH1_HCI_NODEVICE;
            }

            gs_usbh1_hstd_ohci_remain_dd--;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_device_data;
} /* End of function usbh1_hstd_ohci_alloc_device_data() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Endpoint Memory
 * Declaration  : st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_alloc_endpoint( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_hcd_endpoint_p_t ; Endpoint Pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_alloc_endpoint(void)
{
    st_usbh1_ohci_hcd_endpoint_p_t  endpoint;
    st_usbh1_ohci_hcd_endpoint_p_t  ret_endpoint = NULL;
    uint32_t                        n;

    usbh1_hstd_hci_lock_resouce();

    /*  Allocate endpoint */
    endpoint = &gs_usbh1_hstd_ohci_endpoint_memory[0];
    for (n = 0; n < USBH1_OHCI_NUM_ENDPOINT; n++, endpoint++)
    {
        if (FALSE == endpoint->using_flag)
        {
            ret_endpoint = endpoint;
            
            /* Initialize Endpoint */
            memset(endpoint, 0, sizeof(st_usbh1_ohci_hcd_endpoint_t));
            endpoint->using_flag = TRUE;
            gs_usbh1_hstd_ohci_remain_ep--;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_endpoint;
} /* End of function usbh1_hstd_ohci_alloc_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate EndpointDescriptor Memory
 * Declaration  : st_usbh1_ohci_hcd_endpoint_descriptor_p_t usbh1_hstd_ohci_allocate_endpoint_descriptor( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_hcd_endpoint_descriptor_p_t ; Endpoint Descriptor Pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcd_endpoint_descriptor_p_t usbh1_hstd_ohci_allocate_endpoint_descriptor(void)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ret_ed = NULL;
    uint32_t                                    i;

    usbh1_hstd_hci_lock_resouce();

    /* Allocate EndpointDescriptor */
    ed = &gs_usbh1_hstd_ohci_ed_memory[0];
    for (i = 0; i < USBH1_OHCI_NUM_ED; i++, ed++)
    {
        if (FALSE == ed->using_flag)
        {
            ret_ed = ed;
            
            /* Initialize EndpointDescriptor */
            memset(ed, 0, sizeof(st_usbh1_ohci_hcd_endpoint_descriptor_t));
            ed->using_flag = TRUE;
            gs_usbh1_hstd_ohci_remain_ed--;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_ed;
} /* End of function usbh1_hstd_ohci_allocate_endpoint_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate TransferDescriptor Memory
 * Declaration  : st_usbh1_ohci_hcd_transfer_descriptor_p_t usbh1_hstd_ohci_allocate_transfer_descriptor( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_hcd_transfer_descriptor_p_t ; Transfer Descriptor Pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcd_transfer_descriptor_p_t usbh1_hstd_ohci_allocate_transfer_descriptor(void)
{
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   ret_td = NULL;
    uint32_t                                    i;

    usbh1_hstd_hci_lock_resouce();

    /* Allocate TransferDescriptor */
    td = &gs_usbh1_hstd_ohci_td_memory[0];
    for (i = 0; i < USBH1_OHCI_NUM_TD; i++, td++)
    {
        if (FALSE == td->using_flag)
        {
            ret_td = td;

            /* Initialize TransferDescriptor */
            memset(td, 0, sizeof(st_usbh1_ohci_hcd_transfer_descriptor_t));
            td->using_flag = TRUE;
            gs_usbh1_hstd_ohci_remain_td--;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_td;
} /* End of function usbh1_hstd_ohci_allocate_transfer_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Allocate Iso Endpoint
 * Declaration  : st_usbh1_ohci_iso_info_p_t usbh1_hstd_ohci_alloc_endpoint_iso( void )
 * Argument     : void
 * Return       : st_usbh1_ohci_iso_info_p_t ; Iso Endpoint pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_iso_info_p_t usbh1_hstd_ohci_alloc_endpoint_iso(void)
{
    st_usbh1_ohci_iso_info_p_t  ret_iso_info = NULL;
    st_usbh1_ohci_iso_info_p_t  iso_info;
    uint32_t                    n;
    uint32_t                    i;
    
    usbh1_hstd_hci_lock_resouce();

    iso_info = &gs_usbh1_hstd_ohci_iso_endpoint[0];
    for (n = 0; n < USBH1_OHCI_ISO_MAXDEVICE; n++, iso_info++)
    {
        if (FALSE == iso_info->using_flag)
        {
            memset(iso_info, 0, sizeof(st_usbh1_ohci_iso_info_t));
            for (i = 0; i < USBH1_OHCI_ISO_MAX_FRAME; i++)
            {
                iso_info->transfer_info[i].buffer = usbh1_hstd_ohci_physical_address_of(&gs_usbh1_hstd_ohci_iso_buffer[n][i][0]);
            }
            iso_info->using_flag = TRUE;
            ret_iso_info = iso_info;
            break;
        }
    }

    usbh1_hstd_hci_unlock_resouce();

    return ret_iso_info;
} /* End of function usbh1_hstd_ohci_alloc_endpoint_iso() */

/***********************************************************************************************************************
 * Function     : OHCI Free Transfer Request
 * Declaration  : usbh1_hstd_ohci_free_transfer_request(st_usbh1_ohci_request_p_t ohci_req)
 * Argument     : st_usbh1_ohci_request_p_t ohci_req ; OHCI transfer request pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_free_transfer_request(st_usbh1_ohci_request_p_t ohci_req)
{
    usbh1_hstd_hci_lock_resouce();

    memset(ohci_req, 0, sizeof(st_usbh1_ohci_request_t));

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_free_transfer_request() */

/***********************************************************************************************************************
 * Function     : OHCI Free TransferDescriptor Memory
 * Declaration  : void usbh1_hstd_ohci_free_transfer_descriptor( st_usbh1_ohci_hcd_transfer_descriptor_p_t td )
 * Argument     : st_usbh1_ohci_hcd_transfer_descriptor_p_t ; Transfer Descriptor Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_free_transfer_descriptor(st_usbh1_ohci_hcd_transfer_descriptor_p_t td)
{
    st_usbh1_ohci_list_entry_p_t    tmp_hcd_list_f_link;
    st_usbh1_ohci_list_entry_p_t    tmp_hcd_list_b_link;
    st_usbh1_ohci_request_p_t       tmp_usb_drequest;

    if (NULL == td)
    {
        return;
    }

    usbh1_hstd_hci_lock_resouce();

    if(td->usb_drequest != NULL)
    {
        tmp_usb_drequest    = (st_usbh1_ohci_request_p_t)r_usbh1_pa_to_va((uint32_t)td->usb_drequest);
        tmp_hcd_list_f_link = (st_usbh1_ohci_list_entry_p_t)r_usbh1_pa_to_va((uint32_t)tmp_usb_drequest->hcd_list.f_link);
        tmp_hcd_list_b_link = (st_usbh1_ohci_list_entry_p_t)r_usbh1_pa_to_va((uint32_t)tmp_usb_drequest->hcd_list.b_link);

        if (tmp_usb_drequest->hcd_list.f_link == &td->request_list)
        {
            /* Removing TD is located in head of the HcdList */
            tmp_hcd_list_f_link = tmp_hcd_list_f_link->f_link;
        }
        else if (tmp_usb_drequest->hcd_list.b_link == &td->request_list)
        {
            /* Removing TD is located in tail of the HcdList */
            tmp_hcd_list_b_link = tmp_hcd_list_b_link->b_link;
        }
    }

    /* clear */
    td = (st_usbh1_ohci_hcd_transfer_descriptor_p_t)r_usbh1_pa_to_va((uint32_t)td);
    memset(td, NULL, sizeof(st_usbh1_ohci_hcd_transfer_descriptor_t));

    gs_usbh1_hstd_ohci_remain_td++;

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_free_transfer_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Free Endpoint
 * Declaration  : void usbh1_hstd_ohci_free_endpoint( st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_free_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
	if(endpoint == NULL)
	{
		return;
	}

#if 1
    if ((NULL != endpoint->hcd_head_p) && (endpoint->hcd_tail_p == endpoint->hcd_head_p))
    {
        /* Remove NullTD (if remained) */
        usbh1_hstd_ohci_free_transfer_descriptor(endpoint->hcd_head_p);
    }
#else
    struct usbh1_ohci_hcd_transfer_descriptor   * volatile tp;
    struct usbh1_ohci_hcd_transfer_descriptor   * volatile tp_next;

    tp = endpoint->hcd_head_p;
    if( 0 == tp->hc_td.next_td )
    {
        tp_next = tp->hc_td.next_td;
    }
    else
    {
        tp_next = tp->hc_td.next_td - 0x20;
    }
    while( NULL != tp )
    {
        usbh1_hstd_ohci_free_transfer_descriptor(tp);
        tp = tp_next;
        if( 0 == tp->hc_td.next_td )
        {
            tp_next = tp->hc_td.next_td;
        }
        else
        {
            tp_next = tp->hc_td.next_td - 0x20;
        }
    }
#endif

    usbh1_hstd_hci_lock_resouce();

    if (NULL != endpoint->iso_info)
    {
        endpoint->iso_info->using_flag = FALSE;
    }

    if(TRUE == endpoint->using_flag)
    {
		memset(endpoint, NULL, sizeof(st_usbh1_ohci_hcd_endpoint_t));
		gs_usbh1_hstd_ohci_remain_ep++;
    }

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_free_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Free Endpoint Descriptor
 * Declaration  : void usbh1_hstd_ohci_free_endpoint_descriptor( st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed )
 * Argument     : st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed ; Endpoint Descriptor Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_free_endpoint_descriptor(st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed)
{
    usbh1_hstd_ohci_free_endpoint(r_usbh1_pa_to_va(ed->endpoint));

    usbh1_hstd_hci_lock_resouce();

    memset(ed, NULL , sizeof(st_usbh1_ohci_hcd_endpoint_descriptor_t));

    gs_usbh1_hstd_ohci_remain_ed++;

    usbh1_hstd_hci_unlock_resouce();
} /* End of function usbh1_hstd_ohci_free_endpoint_descriptor() */

/***********************************************************************************************************************
 * Function     : OHCI Scan Endpoint info.
 * Declaration  : st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_scan_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 * Return       : st_usbh1_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_hcd_endpoint_p_t usbh1_hstd_ohci_scan_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction)
{
    return gs_usbh1_hstd_ohci_enable_endpoint[device_address][endpoint_number][direction];
} /* End of function usbh1_hstd_ohci_scan_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Store Endpoint info.
 * Declaration  : void usbh1_hstd_ohci_store_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction, st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 *              : st_usbh1_ohci_hcd_endpoint_p_t ; endpoint pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_store_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction, st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
    gs_usbh1_hstd_ohci_enable_endpoint[device_address][endpoint_number][direction] = endpoint;
} /* End of function usbh1_hstd_ohci_store_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Scan Containing Record( ED/TD )
 * Declaration  : uint32_t usbh1_hstd_ohci_scan_containing_record( st_usbh1_ohci_list_entry_p_t list, uint8_t type, uint8_t field )
 * Argument     : st_usbh1_ohci_list_entry_p_t list ; Scanning list pointer
 *              : uint8_t type               ; list type
 *              : uint8_t field              ; list field
 * Return       : uint32_t                   ; scaned record address
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_scan_containing_record(st_usbh1_ohci_list_entry_p_t list, uint8_t type, uint8_t field)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   td;
    uint32_t                                    i;

    if (NULL == list)
    {
        return 0;
    }
    
    if (USBH1_OHCI_CR_TRANSFER_DESCRIPTOR == type)
    {
        if (USBH1_OHCI_CR_REQUESTLIST == field)
        {
            td = &gs_usbh1_hstd_ohci_td_memory[0];
            for (i = 0; i < USBH1_OHCI_NUM_TD; i++, td++)
            {
                if ((TRUE == td->using_flag) && (&td->request_list == list))
                {
                    return (uint32_t)td;
                }
            }
        }
    }
    else if (USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR == type)
    {
        if (USBH1_OHCI_CR_LINK == field)
        {
            ed = &gs_usbh1_hstd_ohci_ed_memory[0];
            for (i = 0; i < USBH1_OHCI_NUM_ED; i++, ed++)
            {
                if ((TRUE == ed->using_flag) && (&ed->link == list))
                {
                    return (uint32_t)ed;
                }
            }
        }
        else if (USBH1_OHCI_CR_PAUSEDLINK == field)
        {
            ed = &gs_usbh1_hstd_ohci_ed_memory[0];
            for (i = 0; i < USBH1_OHCI_NUM_ED; i++, ed++)
            {
                if ((TRUE == ed->using_flag) && (&ed->paused_link == list))
                {
                    return (uint32_t)ed;
                }
            }
        }
    }

    return 0;
} /* End of function usbh1_hstd_ohci_scan_containing_record() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining TDs
 * Declaration  : uint32_t usbh1_hstd_ohci_check_remained_td( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_check_remained_td(void)
{
    if (gs_usbh1_hstd_ohci_remain_td > 4)
    {
        return TRUE;
    }

    return FALSE;
} /* End of function usbh1_hstd_ohci_check_remained_td() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining EDs
 * Declaration  : uint32_t usbh1_hstd_ohci_check_remained_ed( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_check_remained_ed(void)
{
    if (gs_usbh1_hstd_ohci_remain_ed > 1)
    {
        return TRUE;
    }

    return FALSE;
} /* End of function usbh1_hstd_ohci_check_remained_ed() */

/***********************************************************************************************************************
 * Function     : OHCI Check remaining EPs
 * Declaration  : uint32_t usbh1_hstd_ohci_check_remained_ed( void )
 * Argument     : void
 * Return       : uint32_t ; check result( TRUE:remaining, FALSE:not remaining )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_check_remained_ep(void)
{
    if (gs_usbh1_hstd_ohci_remain_ep > 1)
    {
        return TRUE;
    }

    return FALSE;
} /* End of function usbh1_hstd_ohci_check_remained_ep() */

/***********************************************************************************************************************
 * Function     : OHCI ED descriptor 0 clear
 * Declaration  : void usbh1_hstd_ohci_release_ed( uint32_t )
 * Argument     : ED address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
#if 0
void usbh1_hstd_ohci_release_ed(uint32_t hcd_ed_tmp)
{
    memset(hcd_ed_tmp, 0 , sizeof(st_usbh1_ohci_hcd_endpoint_descriptor_t));
    gs_usbh1_hstd_ohci_remain_ed++;
}
#endif
/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
