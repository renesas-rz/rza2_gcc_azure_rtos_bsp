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
 * Copyright (C) 2015(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_cdataio.c
 * Description  : USB Host and Peripheral low level data I/O code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 30.09.2017 1.22 Move "g_usb_pcdc_serialstate_table[]" from "PCDC API file". and Change Function Name 
 *                           "pcdc_read_complete"->"usb_pcdc_read_complete"
 *                           "pcdc_write_complete"->"usb_pcdc_write_complete"
 *                           "phid_read_complete"->"usb_phid_read_complete"
 *                           "phid_write_complete"->"usb_phid_write_complete"
 *                           "pvndr_read_complete"->"usb_pvndr_read_complete"
 *                           "pnvdr_write_complete"->"usb_pnvdr_write_complete"
 *                           "hcdc_read_complete"->"usb_hcdc_read_complete"
 *                           "hcdc_write_complete"->"usb_hcdc_write_complete"
 *                           "hhid_read_complete"->"usb_hhid_read_complete"
 *                           "hhid_write_complete"->"usb_hhid_write_complete"
 *                           "hvndr_read_complete"->"usb_hvndr_read_complete"
 *                           "hnvdr_write_complete"->"usb_hnvdr_write_complete"
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"

#if defined(USB_CFG_PCDC_USE)
#include "r_usb_pcdc_if.h"
#endif /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_PMSC_USE)
#include "r_usb_pmsc_if.h"
#endif /* defined(USB_CFG_PMSC_USE) */

#if defined(USB_CFG_PHID_USE)
#include "r_usb_phid_if.h"
#endif /* defined(USB_CFG_PHID_USE) */

#if (USB_CFG_DMA == USB_CFG_ENABLE)
#include "r_usb_dmac.h"
#endif  /* (USB_CFG_DMA == USB_CFG_ENABLE) */

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
#if ((USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI)
static const uint8_t g_usb_pipe_peri[] =
{
    /* OUT pipe */          /* IN pipe */
  #if defined(USB_CFG_PCDC_USE)
    USB_CFG_PCDC_BULK_OUT,  USB_CFG_PCDC_BULK_IN,   /* USB_PCDC */
    USB_NULL,               USB_CFG_PCDC_INT_IN,    /* USB_PCDCC */
  #else   /* defined(USB_CFG_PCDC_USE) */
    USB_NULL,               USB_NULL,
    USB_NULL,               USB_NULL,
  #endif  /* defined(USB_CFG_PCDC_USE) */

  #if defined(USB_CFG_PHID_USE)
    USB_CFG_PHID_INT_OUT,   USB_CFG_PHID_INT_IN,    /* USB_PHID */
  #else   /* defined(USB_CFG_PHID_USE) */
    USB_NULL,               USB_NULL,
  #endif  /* defined(USB_CFG_PHID_USE) */
};
#endif  /* (USB_CFG_MODE & USB_CFG_PERI) == USB_CFG_PERI */
usb_utr_t g_usb_cstd_int[USB_INT_BUFSIZE] __attribute__ ((section ("USB_DATA_RAM")));
void usb_cstd_select_nak (usb_utr_t *ptr, uint16_t pipe);

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
extern const uint8_t g_usb_pipe_peri[];

/* USB data transfer */
/* PIPEn Buffer counter */
uint32_t g_usb_pstd_data_cnt[USB_MAX_PIPE_NO + 1u] __attribute__ ((section ("USB_DATA_RAM")));

/* PIPEn Buffer pointer(8bit) */
uint8_t *g_p_usb_pstd_data[USB_MAX_PIPE_NO + 1u] __attribute__ ((section ("USB_DATA_RAM")));

/* Message pipe */
usb_utr_t *g_p_usb_pstd_pipe[USB_MAX_PIPE_NO + 1u] __attribute__ ((section ("USB_DATA_RAM")));


/* Callback function of USB Read/USB Write */
void (*g_usb_callback[]) (usb_utr_t *, uint16_t, uint16_t) =  /*__attribute__ ((section ("USB_APL_RAM_INIT"))) = */
{
    /* PCDC, PCDCC */
#if defined(USB_CFG_PCDC_USE)
        usb_pcdc_read_complete, usb_pcdc_write_complete, /* USB_PCDC  (0) */
        USB_NULL, usb_pcdc_write_complete, /* USB_PCDCC (1) */
#else
        USB_NULL, USB_NULL, /* USB_PCDC  (0) */
        USB_NULL, USB_NULL, /* USB_PCDCC (1) */
#endif

        /* PHID */
#if defined(USB_CFG_PHID_USE)
        usb_phid_read_complete, usb_phid_write_complete, /* USB_PHID (2) */
#else
        USB_NULL, USB_NULL, /* USB_PHID (2) */
#endif

        /* PVNDR */
#if defined(USB_CFG_PVNDR_USE)
        usb_pvndr_read_complete, usb_pnvdr_write_complete, /* USB_PVND  (3) */
#else
        USB_NULL, USB_NULL, /* USB_PVND  (3) */
#endif

        /* HCDC, HCDCC */
        USB_NULL, USB_NULL, /* USB_HCDC  (4) */
        USB_NULL, USB_NULL, /* USB_HCDCC (5) */

        /* HHID */
        USB_NULL, USB_NULL, /* USB_HHID  (6) */

        /* HVNDR */
        USB_NULL, USB_NULL, /* USB_HVND  (7) */

        /* HMSC */
        USB_NULL, USB_NULL, /* USB_HMSC  (8) */

        /* PMSC */
        USB_NULL, USB_NULL, /* USB_PMSC  (9) */
}; /* const void (g_usb_callback[])(usb_utr_t *, uint16_t, uint16_t) */


#if defined(USB_CFG_PCDC_USE)

/* Abstract Control Model Notification - SerialState */
uint8_t g_usb_pcdc_serialstate_table[USB_CDC_SETUP_TBL_BSIZE] __attribute__ ((section ("USB_DATA_RAM_INIT"))) =
{
    0xA1,       /* bmRequestType */
    0x20,       /* bNotification:SERIAL_STATE */
    0x00, 0x00, /* wValue:Zero */
    0x00, 0x00, /* wIndex:Interface */
    0x02, 0x00, /* wLength:2 */
    0x00, 0x00, /* Data:UART State bitmap */
};

#endif /* defined(USB_CFG_PCDC_USE) */

usb_pipe_table_t    g_usb_pipe_table[USB_NUM_USBIP][USB_MAXPIPE_NUM+1] __attribute__ ((section ("USB_DATA_RAM")));

/******************************************************************************
 Renesas Abstracted common data I/O functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_select_nak
 Description     : Set the specified pipe PID to send a NAK if the transfer type 
                 : is BULK/INT. 
 Arguments       : usb_utr_t *ptr   : Pointer to usb_utr_t structure.
                 : uint16_t pipe    : Pipe number.
 Return value    : none
 ******************************************************************************/
void usb_cstd_select_nak (usb_utr_t *ptr, uint16_t pipe)
{
    if (USB_MAX_PIPE_NO < pipe)
    {
        return; /* Error */
    }

    /* Check PIPE TYPE */
    if (usb_cstd_get_pipe_type(ptr, pipe) != USB_TYPFIELD_ISO)
    {
        usb_cstd_set_nak(ptr, pipe);
    }
}
/******************************************************************************
 End of function usb_cstd_select_nak
 ******************************************************************************/

/******************************************************************************
 Function Name   : usb_cstd_debug_hook
 Description     : Debug hook
 Arguments       : uint16_t error_code          : error code
 Return value    : none
 ******************************************************************************/
void usb_cstd_debug_hook (uint16_t error_code)
{
    while (1)
    {
        /* Non */
    }
} /* End of function usb_cstd_debug_hook() */

/******************************************************************************
 Function Name   : usb_ctrl_read
 Description     : Receive process for Control transfer
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
                 : uint8_t *buf      : transfer data address
                 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_SUCCESS(USB_OK) / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_read (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    if (USB_PERI == g_usb_usbmode)
    {
        if (USB_ON == g_usb_pstd_pipe0_request)
        {
            return USB_QOVR;
        }
        g_usb_read_request_size[USB_CFG_USE_USBIP][USB_PIPE0] = size;
        usb_pstd_ctrl_write(size, buf);
    }
    return USB_SUCCESS;
} /* End of function usb_ctrl_read() */

/******************************************************************************
 Function Name   : usb_ctrl_write
 Description     : Send process for Control transfer
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
                 : uint8_t *buf      : transfer data address
                 : uint32_t size     : transfer length
 Return value    : usb_er_t          : USB_SUCCESS(USB_OK) / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_write (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_ctrl_t ctrl;

    if (USB_PERI == g_usb_usbmode)
    {
        if ((USB_NULL == buf) && (USB_NULL == size))
        {
            if (USB_ACK == p_ctrl->status)
            {
                usb_cstd_set_buf(USB_NULL, (uint16_t) USB_PIPE0);   /* Set BUF */
            }
            else /* USB_STALL */
            {
                usb_pstd_set_stall_pipe0();
            }

            ctrl.setup  = p_ctrl->setup;                            /* Save setup data. */
            ctrl.module = USB_CFG_USE_USBIP;
            ctrl.size   = 0;
            ctrl.status = USB_ACK;
            ctrl.type   = USB_REQUEST;
            usb_set_event(USB_STS_REQUEST_COMPLETE, &ctrl);

        }
        else
        {
            if (USB_ON == g_usb_pstd_pipe0_request)
            {
                return USB_QOVR;
            }
            usb_pstd_ctrl_read(size, buf);
        }
    }
    return USB_SUCCESS;
} /* End of function usb_ctrl_write() */

/******************************************************************************
 Function Name   : usb_ctrl_stop
 Description     : Stop of USB Control transfer.
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_ctrl_stop (usb_ctrl_t *p_ctrl)
{
    usb_er_t err;

    if ((p_ctrl->type) > USB_PVND)
    {
    }
    else
    {
        /* Peripheral only */
        err = usb_pstd_transfer_end(USB_PIPE0);
    }
    return err;
} /* End of function usb_ctrl_stop() */

/******************************************************************************
 Function Name   : usb_data_read
 Description     : USB data read processing
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
                 : uint8_t *buf      : Transfer data address
                 : uint32_t size     : Transfer length
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_read (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    uint8_t pipe;
    usb_er_t err;
    usb_utr_t   *p_tran_data;
    usb_utr_t   tran_data;

    pipe = usb_get_usepipe(p_ctrl, USB_READ);

    if ((p_ctrl->type) > USB_PVND)
    {
    }
    else
    {

        p_tran_data = (usb_utr_t *)&tran_data;

        g_usb_read_request_size[USB_CFG_USE_USBIP][pipe] = size;
        p_tran_data->keyword = pipe; /* Pipe No */
        p_tran_data->p_tranadr = buf; /* Data address */
        p_tran_data->tranlen = size; /* Data Size */
        p_tran_data->complete = (usb_cb_t)g_usb_callback[p_ctrl->type*2]; /* Callback function */
        err = usb_pstd_transfer_start(p_tran_data);
    }

    return err;
} /* End of function usb_data_read() */

/******************************************************************************
 Function Name   : usb_data_write
 Description     : USB data write processing
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
                 : uint8_t *buf      : Transfer data address
                 : uint32_t size     : Transfer length
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_write (usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    uint8_t pipe;
    usb_er_t err;
    usb_utr_t   *p_tran_data;
    usb_utr_t   tran_data;


    pipe = usb_get_usepipe(p_ctrl, USB_WRITE);

    if ((p_ctrl->type) > USB_PVND)
    {
    }
    else
    {

        p_tran_data = (usb_utr_t *)&tran_data;

  #if defined(USB_CFG_PCDC_USE)
        if (USB_CFG_PCDC_INT_IN != pipe)
        {
            p_tran_data->p_tranadr = buf; /* Data address */
            p_tran_data->tranlen = size; /* Data Size */
        }
        else
        {
            g_usb_pcdc_serialstate_table[8] = buf[0];
            g_usb_pcdc_serialstate_table[9] = buf[1];
            p_tran_data->p_tranadr = g_usb_pcdc_serialstate_table; /* Data address */
            p_tran_data->tranlen = 10; /* Data Size */
        }

  #else   /* defined(USB_CFG_PCDC_USE) */
        p_tran_data->p_tranadr = buf; /* Data address */
        p_tran_data->tranlen = size; /* Data Size */
  #endif  /* defined(USB_CFG_PCDC_USE) */
        p_tran_data->keyword = pipe; /* Pipe No */
        p_tran_data->complete = (usb_cb_t)g_usb_callback[(p_ctrl->type*2)+1]; /* Callback function */
        err = usb_pstd_transfer_start(p_tran_data);
    }

    return err;
} /* End of function usb_data_write() */

/******************************************************************************
 Function Name   : usb_data_stop
 Description     : USB data transfer stop processing
 Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
                 : uint16_t type     : Read(0)/Write(1)
 Return value    : usb_er_t          : USB_OK / USB_ERROR.
 ******************************************************************************/
usb_er_t usb_data_stop (usb_ctrl_t *p_ctrl, uint16_t type)
{
    uint8_t pipe;
    usb_er_t err = USB_ERROR;

    pipe = usb_get_usepipe(p_ctrl, type);

    if (USB_NULL == pipe)
    {
        return USB_ERROR;
    }

    if ((p_ctrl->type) > USB_PVND)
    {
        /* Host only */
    }
    else
    {
        /* Peripheral only */
        err = usb_pstd_transfer_end(pipe);
    }
    return err;
} /* End of function usb_data_stop() */

/******************************************************************************
 Function Name   : usb_get_usepipe
 Description     : Get pipe number for USB Read/USB Write
 Arguments       : usb_ctrl_t *p_ctrl: Control structure for USB API.
                 : uint8_t dir       : Read(0)/Write(1)
 Return value    : Bitmap of Use pipe
 ******************************************************************************/
uint8_t usb_get_usepipe (usb_ctrl_t *p_ctrl, uint8_t dir)
{
    uint8_t pipe = USB_NULL;
    uint8_t idx;

    if ((p_ctrl->type) > USB_PVND)
    {
    }
    else
    {
        /* Peripheral */
        idx = (p_ctrl->type * 2) + dir;
        pipe = g_usb_pipe_peri[idx];
    }
    return pipe;
} /* End of function usb_get_usepipe() */

/******************************************************************************
 Function Name   : usb_cstd_get_pipe_buf_value
 Description     : Get Value for USBA Module PIPE BUF REG.
 Arguments       : Pipe no.
 Return value    : PIPE BUF set value.
 ******************************************************************************/
uint16_t usb_cstd_get_pipe_buf_value (uint16_t pipe_no)
{
    uint16_t pipe_buf;

    switch (pipe_no)
    {
#if defined(USB_CFG_PCDC_USE)
        case USB_CFG_PCDC_BULK_IN:
            pipe_buf = (USB_BUF_SIZE(2048u) | USB_BUF_NUMB(8u));
        break;

        case USB_CFG_PCDC_BULK_OUT:
            pipe_buf = (USB_BUF_SIZE(2048u) | USB_BUF_NUMB(72u));
        break;
#endif  /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_PMSC_USE)
        case USB_CFG_PMSC_BULK_IN:
            pipe_buf = (USB_BUF_SIZE(2048u) | USB_BUF_NUMB(8u));
        break;

        case USB_CFG_PMSC_BULK_OUT:
            pipe_buf = (USB_BUF_SIZE(2048u) | USB_BUF_NUMB(72u));
        break;
#endif  /* defined(USB_CFG_PMSC_USE) */

#if defined(USB_CFG_PVND_USE) /*|| defined(USB_CFG_HVND_USE)*/
        case USB_PIPE1:
            pipe_buf = (USB_BUF_SIZE(512u) | USB_BUF_NUMB(8u));
        break;
        case USB_PIPE2:
            pipe_buf = (USB_BUF_SIZE(512u) | USB_BUF_NUMB(24u));
        break;
        case USB_PIPE3:
            pipe_buf = (USB_BUF_SIZE(512u) | USB_BUF_NUMB(40u));
        break;
        case USB_PIPE4:
            pipe_buf = (USB_BUF_SIZE(512u) | USB_BUF_NUMB(56u));
        break;
        case USB_PIPE5:
            pipe_buf = (USB_BUF_SIZE(512u) | USB_BUF_NUMB(72u));
        break;
#endif  /* defined(USB_CFG_PVND_USE) */

        case USB_PIPE6:
            pipe_buf = (USB_BUF_SIZE(64u) | USB_BUF_NUMB(4u));
        break;

        case USB_PIPE7:
            pipe_buf = (USB_BUF_SIZE(64u) | USB_BUF_NUMB(5u));
        break;

        case USB_PIPE8:
            pipe_buf = (USB_BUF_SIZE(64u) | USB_BUF_NUMB(6u));
        break;

        case USB_PIPE9:
            pipe_buf = (USB_BUF_SIZE(64u) | USB_BUF_NUMB(7u));
        break;

        default:
            /* Error */
        break;
    }

    return pipe_buf;
} /* End of function usb_cstd_get_pipe_buf_value() */


/******************************************************************************
 Function Name   : get_usb_int_buf
 Description     : USB interrupt routine. Analyze which USB interrupt occurred
                 : and send message to PCD task.
 Arguments       : none
 Return value    : Point to the area for usb_int_t structure
 ******************************************************************************/
usb_utr_t    *get_usb_int_buf(void)
{
    static uint16_t count = 0;
    usb_utr_t       *p;

    p = &g_usb_cstd_int[count];

    count = ((count + 1) % USB_INT_BUFSIZE);

    return p;
}
/******************************************************************************
 End of function get_usb_int_buf
 ******************************************************************************/


/******************************************************************************
 End  Of File
 ******************************************************************************/
