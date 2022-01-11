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
 * Copyright (C) 2014(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh0_hhid_driver.c
 * Description  : Host HID class driver code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.21 hid_class_request_complete()is deleted. usbh0_hhid_read_complete(),
 *                           usbh0_hhid_write_complete()is fixed.
 *         : 30.09.2017 1.22 Change function name ""->""
 *                           "hid_suspended"->"usbh0_hid_suspended"
 *                           "hid_configured"->"usbh0_hid_configured"
 *                           "hid_detach"->"usbh0_hid_detach"
 *                           "hid_resume_complete"->"usbh0_hid_resume_complete"
 *                           "hhid_read_complete"->"usbh0_hhid_read_complete"
 *                           "hhid_write_complete"->"usbh0_hhid_write_complete"
 *         : 31.03.2018 1.23 Supporting Smart Configurator 
 *         : 16.11.2018 1.24 Supporting RTOS.
 ***********************************************************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include <string.h>

#include "r_usbh0_basic_if.h"
#include "r_usbh0_hhci_local.h"
#include "r_usbh0_hhid_if.h"
#include "r_usbh0_hhid.h"

/******************************************************************************
 Exported global variables
 ******************************************************************************/


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* variables */
static uint16_t       g_usbh0_hhid_request_setup[5] __attribute__ ((section ("USB_CLASS_0")));   /* Control Transfer Request field */
static st_usbh0_utr_t usbh0_shhid_string_req __attribute__ ((section ("USB_CLASS_0")));               /* Request for String discriptor */

/* functions */
static uint16_t  usbh0_hhid_cmd_submit (st_usbh0_utr_t *ptr, usbh0_utr_cb_t complete);
static void      usbh0_hhid_check_result (st_usbh0_utr_t *ptr, uint16_t unused, uint16_t status);
static void      usbh0_hhid_enumeration (st_usbh0_utr_t *mess);
usbh0_err_t      usb_hhid_class_request_process(void);

/******************************************************************************
 Exported global variables (to be accessed by other files)
 ******************************************************************************/
uint16_t         g_usbh0_hhid_protocol[USBH0_MAXDEVADDR];               /* HID Device protocol(Mouse/Keyboard) */
uint16_t         g_usbh0_hhid_num_endpoint[USBH0_MAXDEVADDR];           /* Num Endpoints */
uint16_t         g_usbh0_hhid_outpipe_support[USBH0_MAXDEVADDR];        /* OUT pipe support */
static uint8_t   g_usbh0_hhid_string_data[USBH0_HHID_CLSDATASIZE] __attribute__ ((section ("USB_CLASS_0")));
uint16_t         g_usbh0_hhid_devaddr;                                /* Device Address */
uint16_t         g_usbh0_hhid_enum_seq;                               /* Enumeration Sequence */
uint16_t         g_usbh0_hhid_speed;                                  /* Device speed */
uint8_t         *g_p_usbh0_hhid_config_table __attribute__ ((section ("USB_CLASS_0")));                         /* Configuration Descriptor Table */
uint8_t         *g_p_usbh0_hhid_device_table __attribute__ ((section ("USB_CLASS_0")));                         /* Device Descriptor Table */
uint8_t         *g_p_usbh0_hhid_interface_table __attribute__ ((section ("USB_CLASS_0")));                      /* Interface Descriptor Table */
usbh0_pipe_table_t    g_usbh0_pipe_table[USBH0_MAXPIPE_NUM+1];

/******************************************************************************
 Renesas Abstracted USB Driver functions
 ******************************************************************************/

/******************************************************************************
 Function Name   : usbh0_hhid_check_result
 Description     : String Descriptor receive complete callback.
 Arguments       : st_usbh0_utr_t *ptr       : Pointer to st_usbh0_utr_t structure.
                 : uint16_t unused      : Not used
                 : uint16_t status      : Not used
 Return          : none
 ******************************************************************************/
static void usbh0_hhid_check_result (st_usbh0_utr_t *ptr, uint16_t unused, uint16_t status)
{
    st_usbh0_utr_t  *p_blf;
    usbh0_er_t      err;

    /* Get mem pool blk */
    if ( USBH0_PGET_BLK(USBH0_HHID_MPL, &p_blf) == USBH0_OK)
    {
        *p_blf = *ptr;
        /* Send message */
        err = USBH0_SND_MSG(USBH0_HHID_MBX, (usbh0_msg_t* )p_blf);
        if (USBH0_OK != err)
        {
            /* error */
            err = USBH0_REL_BLK(USBH0_HHID_MPL, &p_blf);
            USBH0_PRINTF0( "### usbh0_open_hstd function snd_msg error\n");
        }
    }
    else
    {
        /* Error */
        while (1)
        {
            /* error */
        }
    }
}
/******************************************************************************
 End of function usbh0_hhid_check_result
 ******************************************************************************/

/******************************************************************************
 Function Name   : usbh0_hhid_enumeration
 Description     : Enumeration( Get String Descriptor ) and Pipe tabele set.
 Arguments       : st_usbh0_utr_t *mess      : Pointer to st_usbh0_utr_t structure.
 Return value    : none
 ******************************************************************************/
static void usbh0_hhid_enumeration (st_usbh0_utr_t *mess)
{
    uint16_t retval;
    uint8_t string;
    uint8_t *p_desc;
    uint8_t *p_iftable;
    uint16_t desc_len;
    uint8_t protocol;
#ifdef USBH0_DEBUGPRINT_PP
    uint8_t *table;
    uint8_t pdata[32],j;
#endif /* #ifdef USBH0_DEBUGPRINT_PP */

    /* Branch for Enumeration Sequence */
    switch (g_usbh0_hhid_enum_seq)
    {
        /* Enumeration Sequence String Descriptor #0 receive request */
        case USBH0_HHID_ENUM_STR_DT0_REQ :

            /* Get String descriptor */
            usbh0_hhid_get_string_desc(mess, g_usbh0_hhid_devaddr, (uint16_t) 0,
                    (usbh0_utr_cb_t) &usbh0_hhid_check_result);

            /* String Descriptor #0 Receive wait */
            g_usbh0_hhid_enum_seq++;
        break;

            /* Enumeration Sequence String Descriptor #0 Receive complete */
        case USBH0_HHID_ENUM_STR_DT0_WAIT :
            /* String descriptor check */
            if ((usbh0_er_t) USBH0_CTRL_END == mess->result)
            {
                /* Set iProduct */
                string = g_p_usbh0_hhid_device_table[15];

                /* Get String descriptor */
                usbh0_hhid_get_string_desc(mess, g_usbh0_hhid_devaddr, (uint16_t) string,
                        (usbh0_utr_cb_t) &usbh0_hhid_check_result);
            }
            /* StringDescriptor iProduct Receive */
            g_usbh0_hhid_enum_seq++;
        break;

            /* StringDescriptor iProduct Receive complete */
        case USBH0_HHID_ENUM_STR_IPRODUCT_WAIT :
            /* String descriptor check */
            if ((usbh0_er_t) USBH0_CTRL_END == mess->result)
            {
#ifdef USBH0_DEBUGPRINT_PP
                /* String descriptor iProduct Address set */
                table = (uint8_t*) &g_usbh0_hhid_string_data;

                /* String descriptor bLength check */
                if (table[0] < (uint8_t) (32 * 2 + 2))
                {
                    /* Number of characters = bLength /2 -1  */
                    /* 1 character 16bit(UNICODE String) */
                    table[0] = (uint8_t) (table[0] / 2 - 1);
                }
                else
                {
                    /* Number of characters = 32 set */
                    table[0] = (uint8_t) 32;
                }
                for (j = 0; j < table[0]; j++)
                {
                    pdata[j] = table[j * 2 + 2];
                }
                pdata[table[0]] = 0;

                /* Display Product name */
                USBH0_PRINTF1("  %s\n", pdata);
#endif /* #ifdef USBH0_DEBUGPRINT_PP */
            }
            else
            {
                USBH0_PRINTF0("*** Product name error\n");
            }

            protocol = usbh0_hhid_get_hid_protocol(g_usbh0_hhid_devaddr);
            if ((USBH0_HID_KEYBOARD != protocol) && (USBH0_HID_MOUSE != protocol))
            {
                USBH0_PRINTF0("### Device information error !\n");

                /* Enumeration Sequence Complete */
                g_usbh0_hhid_enum_seq = USBH0_HHID_ENUM_COMPLETE;

                /* Enumeration class check error */
                retval = USBH0_ERROR;
            }
            p_desc = g_p_usbh0_hhid_config_table;

            desc_len = ((uint16_t) *(p_desc + 3)) << 8;
            desc_len += (uint16_t) *(p_desc + 2);

            /* Searching InterfaceDescriptor */
            p_iftable = g_p_usbh0_hhid_interface_table;

            desc_len = desc_len - (p_iftable - p_desc);

            /* pipe information table set */
            retval = usbh0_hhid_pipe_info(mess, p_iftable, g_usbh0_hhid_speed, desc_len);
            if (USBH0_ERROR == retval)
            {
                USBH0_PRINTF0("### Device information error !\n");
                /* Enumeration Sequence Complete */
                g_usbh0_hhid_enum_seq = USBH0_HHID_ENUM_COMPLETE;
                /* Enumeration class check error */
                retval = USBH0_ERROR;
            }
            else
            {
                /* Enumeration Sequence Complete */
                g_usbh0_hhid_enum_seq = USBH0_HHID_ENUM_COMPLETE;
                /* Enumeration class check OK */
                retval = USBH0_OK;
            }
            /* return to MGR */
            R_USBH0_HstdReturnEnuMGR(retval);
        break;
        default :
            g_usbh0_hhid_enum_seq = USBH0_HHID_ENUM_STR_DT0_REQ;
            R_USBH0_HstdReturnEnuMGR(USBH0_ERROR);
        break;
    }
}
/******************************************************************************
 End of function usbh0_hhid_enumeration
 ******************************************************************************/

/******************************************************************************
 Function Name   : usbh0_hhid_task
 Description     : Host HIT Task
 Argument        : usbh0_vp_int_t stacd   : Start code
 Return value    : none
 ******************************************************************************/
void usbh0_hhid_task (void)
{
    st_usbh0_utr_t *p_mess;
    usbh0_er_t err = 0l;

    err = USBH0_RCV_MSG(USBH0_HHID_MBX, (usbh0_msg_t** )&p_mess);
    if (USBH0_OK == err)
    {
        switch (p_mess->msginfo)
        {
            case USBH0_HHID_TCMD_OPEN :
                usbh0_hhid_enumeration(p_mess);
            break;

            default :
            break;
        }
        err = USBH0_REL_BLK(USBH0_HHID_MPL, p_mess);
        if (USBH0_OK != err)
        {
            USBH0_PRINTF0("### USB HHID Task rel_blk error\n");
        }

        /* Conditional compile dep. on OS usage. */
    }
    else
    {
        return;
    }
}
/******************************************************************************
 End of function usbh0_hhid_task
 ******************************************************************************/


/******************************************************************************
 Function Name   : usbh0_hhid_pipe_info
 Description     : pipe information table set
 Arguments       : st_usbh0_utr_t *mess         : Pointer to st_usbh0_utr_t structure.
                 : uint8_t *table          : Descriptor store address
                 : uint16_t speed          : Conect Dpeed(Hi/Full/...)
                 : uint16_t length         : Descriptor total lenght
 Return value    : uint16_t                : Error info
 ******************************************************************************/
uint16_t usbh0_hhid_pipe_info (st_usbh0_utr_t *ptr, uint8_t *table, uint16_t speed, uint16_t length)
{
    /* Offset for Descriptor Top Address */
    uint16_t ofdsc;
    uint16_t pipe_no;
    uint16_t detect_in_pipe = USBH0_OFF;
    uint16_t detect_out_pipe = USBH0_ON;

    usbh0_pipe_table_reg_t    ep_tbl;

    /* Check configuration descriptor */
    if (USBH0_DT_INTERFACE != table[1])
    {
        USBH0_PRINTF0("### Descriptor is not Interface descriptor.\n");
        return USBH0_ERROR;
    }

    /* Descriptor Address set */
    ofdsc = table[0];

    g_usbh0_hhid_outpipe_support[g_usbh0_hhid_devaddr] = USBH0_OFF;

    if (USBH0_HID_OTHER == g_usbh0_hhid_protocol[g_usbh0_hhid_devaddr])
    {
        if (2 == g_usbh0_hhid_num_endpoint[g_usbh0_hhid_devaddr])
        {
            detect_out_pipe = USBH0_OFF;
        }
    }

    /* Loop for Endpoint Descriptor search */
    while (ofdsc < length)
    {
        /* Endpoint Descriptor */
        if (USBH0_DT_ENDPOINT == table[ofdsc + USBH0_DEV_B_DESCRIPTOR_TYPE])
        {
            /* Check pipe information */
            pipe_no = usbh0_hstd_make_pipe_reg_info (g_usbh0_hhid_devaddr, USBH0_HHID,
                                                  speed, &table[ofdsc], &ep_tbl);
            if ( USBH0_NULL == pipe_no)
            {
                return USBH0_ERROR;
            }
            else
            {
                usbh0_hstd_set_pipe_info (pipe_no, &ep_tbl);
            }

            if (USBH0_DIR_H_IN == (ep_tbl.pipe_cfg & USBH0_DIRFIELD))
            {
                /* Get Max Packet size */
                detect_in_pipe = USBH0_ON;
            }
            else if (USBH0_DIR_H_OUT == (ep_tbl.pipe_cfg & USBH0_DIRFIELD))
            {
                g_usbh0_hhid_outpipe_support[g_usbh0_hhid_devaddr] = USBH0_ON;
                detect_out_pipe = USBH0_ON;
            }
            else
            {
                /* error */
            }

            if ((USBH0_ON == detect_in_pipe) && (USBH0_ON == detect_out_pipe))
            {
                    return USBH0_OK;
            }
        }
        ofdsc += table[ofdsc];/* Next descriptor point set */
    }
    return USBH0_ERROR;
}
/******************************************************************************
 End of function usbh0_hhid_pipe_info
 ******************************************************************************/


/******************************************************************************
 Function Name   : usbh0_hhid_get_string_desc
 Description     : Set GetDescriptor
 Arguments       : st_usbh0_utr_t    *ptr        : Pointer to st_usbh0_utr_t structure.
                 : uint16_t     addr        : device address
                 : uint16_t     string      : descriptor index
                 : usbh0_cb_t     complete    : callback function
 Return value    : uint16_t                 : error info
 ******************************************************************************/
uint16_t usbh0_hhid_get_string_desc (st_usbh0_utr_t *ptr, uint16_t addr, uint16_t string, usbh0_utr_cb_t complete)
{
    uint16_t i;

    if (0 ==  string)
    {
        g_usbh0_hhid_request_setup[2] = (uint16_t) 0x0000;
        g_usbh0_hhid_request_setup[3] = (uint16_t) 0x0004;
    }
    else
    {
        /* Set LanguageID */
        g_usbh0_hhid_request_setup[2] = (uint16_t) (g_usbh0_hhid_string_data[2]);
        g_usbh0_hhid_request_setup[2] |= (uint16_t) ((uint16_t) (g_usbh0_hhid_string_data[3]) << 8);
        g_usbh0_hhid_request_setup[3] = (uint16_t) USBH0_HHID_CLSDATASIZE;
    }
    g_usbh0_hhid_request_setup[0] = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
    g_usbh0_hhid_request_setup[1] = (uint16_t) (USBH0_STRING_DESCRIPTOR + string);
    g_usbh0_hhid_request_setup[4] = addr;

    for (i = 0; i < g_usbh0_hhid_request_setup[3]; i++)
    {
        g_usbh0_hhid_string_data[i] = (uint8_t) 0xFF;
    }

    return usbh0_hhid_cmd_submit(ptr, complete);
}
/******************************************************************************
 End of function usbh0_hhid_get_string_desc
 ******************************************************************************/

/******************************************************************************
 Function Name   : usbh0_hhid_cmd_submit
 Description     : command submit
 Arguments       : st_usbh0_utr_t    *ptr        : Pointer to st_usbh0_utr_t structure.
                 : usbh0_cb_t     complete    : callback function
 Return value    : uint16_t                  : USBH0_DONE
 ******************************************************************************/
static uint16_t usbh0_hhid_cmd_submit (st_usbh0_utr_t *ptr, usbh0_utr_cb_t complete)
{
    usbh0_er_t err;

    usbh0_shhid_string_req.p_tranadr = (void*) g_usbh0_hhid_string_data;
    usbh0_shhid_string_req.complete  = complete;
    usbh0_shhid_string_req.tranlen   = (uint32_t) g_usbh0_hhid_request_setup[3];
    usbh0_shhid_string_req.keyword   = USBH0_PIPE0;
    usbh0_shhid_string_req.p_setup   = &g_usbh0_hhid_request_setup;

    err = usbh0_hstd_transfer_start(&usbh0_shhid_string_req);

    if (USBH0_ERROR == err)
    {
        /* error */
        USBH0_PRINTF0( "### usbh0_hhid_cmd_submit() : USBH0_E_QOVR \n");
    }

    return USBH0_OK;
}
/******************************************************************************
 End of function usbh0_hhid_cmd_submit
 ******************************************************************************/

/******************************************************************************
 Function Name   : usbh0_hhid_get_hid_protocol
 Description     : Get HID Protocol
 Arguments       : st_usbh0_utr_t    *ptr        : Pointer to st_usbh0_utr_t structure.
                 : uint16_t     devadr      : Device address
 Return          : Hid Protocol
 ******************************************************************************/
uint8_t usbh0_hhid_get_hid_protocol (uint16_t devadr)
{
    /* table[7]:Interface Descriptor-bInterfaceProtocol */
    return g_usbh0_hhid_protocol[devadr];
}
/******************************************************************************
 End of function usbh0_hhid_get_hid_protocol
 ******************************************************************************/


/******************************************************************************
 Function Name   : usbh0_hhid_class_check
 Description     : Driver check.
 Arguments       : st_usbh0_utr_t    *ptr        : Pointer to st_usbh0_utr_t structure.
                 : uint16_t     **table     : Pointer to HID class information table
 Return value    : none
 ******************************************************************************/
void usbh0_hhid_class_check (uint16_t **table)
{
    st_usbh0_utr_t *p_blf;
    usbh0_er_t err;
    usbh0_clsinfo_t *cp;

    g_p_usbh0_hhid_device_table    = (uint8_t*) ((uint16_t*) table[0]);  /* Device Descriptor Table */
    g_p_usbh0_hhid_config_table    = (uint8_t*) ((uint16_t*) table[1]);  /* Configuration Descriptor Table */
    g_usbh0_hhid_speed             = (uint16_t) (*table[6]);             /* Device speed */
    g_usbh0_hhid_devaddr           = (uint16_t) (*table[7]);             /* Device Address */
    g_p_usbh0_hhid_interface_table = (uint8_t*) ((uint16_t*) table[2]);  /* Interface Descriptor Table */
    g_usbh0_hhid_protocol[g_usbh0_hhid_devaddr] =
            g_p_usbh0_hhid_interface_table[USBH0_B_INTERFACE_PROTOCOL];    /* Interface Protocol */

    g_usbh0_hhid_num_endpoint[g_usbh0_hhid_devaddr] =
            g_p_usbh0_hhid_interface_table[USBH0_IF_B_NUMENDPOINTS];       /* Num Endpoints */

    R_USBH0_HstdSetPipe(table);

    /* Enumeration Sequence String Descriptor #0 receive request */
    g_usbh0_hhid_enum_seq = (uint16_t) USBH0_HHID_ENUM_STR_DT0_REQ;

    /* Descriptor check result */
    *table[3] = USBH0_OK;

    /* Get mem block from pool. */
    if ( USBH0_PGET_BLK(USBH0_HHID_MPL, &p_blf) == USBH0_OK)
    {
        cp = (usbh0_clsinfo_t*) p_blf;
        cp->msginfo = USBH0_HHID_TCMD_OPEN; /* Set message information :USB transfer. */
        /* Send message */
        err = USBH0_SND_MSG(USBH0_HHID_MBX, (usbh0_msg_t* )cp);
        if (USBH0_OK != err)
        {
            /* Error */
            err = USBH0_REL_BLK(USBH0_HHID_MPL, (st_usbh0_utr_t* )p_blf);
            USBH0_PRINTF1("Host Sample snd_msg error %x\n", err);

            /* Transfer start request send NG */
        }
    }
    else
    {
        /* Error */
        while (1)
        {
            /* error */
        }
    }
}
/******************************************************************************
 End of function usbh0_hhid_class_check
 ******************************************************************************/


/******************************************************************************
Function Name   : usbh0_hstd_make_pipe_reg_info
Description     : Make value for USB PIPE registers set value.
Arguments       : uint16_t ip_no        : USB Module no.(USBH0_IP0/USBH0_IP1)
                : uint16_t address      : USB Device address
                : usbh0_class_t usbh0_class : USB Device class(USBH0_HVND/USBH0_HCDC/USBH0_HHID/USBH0_HMSC/USBH0_HUB)
                : uint16_t speed        : USB speed
                : uint8_t *descriptor   : Address for End Point Descriptor
                : usbh0_pipe_table_reg_t *pipe_table_work : Address for Store PIPE reg set value.
Return value    : Pipe no (USBH0_PIPE1->USBH0_PIPE9:OK, USBH0_NULL:Error)
******************************************************************************/
uint8_t usbh0_hstd_make_pipe_reg_info (uint16_t address, uint16_t usbh0_class, uint16_t speed,
                                         uint8_t *descriptor, usbh0_pipe_table_reg_t *pipe_table_work)
{
    uint8_t     pipe_no;
    uint16_t    pipe_cfg;
    uint16_t    pipe_maxp;
    uint16_t    pipe_peri = USBH0_NULL;

    /* Check Endpoint descriptor */
    if( descriptor[USBH0_DEV_B_DESCRIPTOR_TYPE] != USBH0_DT_ENDPOINT )
    {
        return USBH0_NULL;   /* Error */
    }

    /* set pipe configuration value */
    switch ((uint16_t)(descriptor[USBH0_EP_B_ATTRIBUTES] & USBH0_EP_TRNSMASK))
    {
        /* Bulk Endpoint */
        case USBH0_EP_BULK:
            /* Set pipe configuration table */
            if( (descriptor[USBH0_EP_B_EPADDRESS] & USBH0_EP_DIRMASK) == USBH0_EP_IN )
            {
                /* IN(receive) */
                pipe_cfg    = (uint16_t)(USBH0_TYPFIELD_BULK | USBH0_SHTNAKFIELD | USBH0_DIR_H_IN);
                pipe_no     = R_USBH0_HstdGetPipeID(address, USBH0_EP_BULK, USBH0_EP_IN, 0xFF);
            }
            else
            {
                /* OUT(send) */
                pipe_cfg    = (uint16_t)(USBH0_TYPFIELD_BULK | USBH0_DIR_H_OUT);
                pipe_no     = R_USBH0_HstdGetPipeID(address, USBH0_EP_BULK, USBH0_EP_OUT, 0xFF);
            }
        break;

        /* Interrupt Endpoint */
        case USBH0_EP_INT:
            /* Set pipe configuration table */
            if( (descriptor[USBH0_EP_B_EPADDRESS] & USBH0_EP_DIRMASK) == USBH0_EP_IN )
            {
                /* IN(receive) */
                pipe_cfg    = (uint16_t)(USBH0_TYPFIELD_INT | USBH0_DIR_H_IN);
                pipe_no     = R_USBH0_HstdGetPipeID(address, USBH0_EP_INT, USBH0_EP_IN, 0xFF);
            }
            else
            {
                /* OUT(send) */
                pipe_cfg    = (uint16_t)(USBH0_TYPFIELD_INT | USBH0_DIR_H_OUT);
                pipe_no     = R_USBH0_HstdGetPipeID(address, USBH0_EP_INT, USBH0_EP_OUT, 0xFF);
            }

            /* Get value for Interval Error Detection Interval  */
        break;

        default:
            return USBH0_NULL;   /* Error */
        break;
    }

    /* Check Pipe no. */
    if (USBH0_NULL != pipe_no)
    {
        /* Endpoint number set */
        pipe_cfg    |= (uint16_t)(descriptor[USBH0_EP_B_EPADDRESS] & USBH0_EP_NUMMASK);

        /* set max packet size */
        pipe_maxp   =  (uint16_t)descriptor[USBH0_EP_W_MPS_L] | (address << USBH0_DEVADDRBIT);
        pipe_maxp   |= ((uint16_t)descriptor[USBH0_EP_W_MPS_H] << 8);

        /* Store PIPE reg set value. */
        pipe_table_work->pipe_cfg  = pipe_cfg;
        pipe_table_work->pipe_maxp = pipe_maxp;
        pipe_table_work->pipe_peri = pipe_peri;
    }
    return (pipe_no);
} /* eof usbh0_hstd_make_pipe_reg_info() */

void usbh0_hstd_set_pipe_info (uint16_t pipe_no, usbh0_pipe_table_reg_t *src_ep_tbl)
{
    g_usbh0_pipe_table[pipe_no].use_flag  = USBH0_TRUE;
    g_usbh0_pipe_table[pipe_no].pipe_cfg  = src_ep_tbl->pipe_cfg;
    g_usbh0_pipe_table[pipe_no].pipe_maxp = src_ep_tbl->pipe_maxp;
    g_usbh0_pipe_table[pipe_no].pipe_peri = src_ep_tbl->pipe_peri;
}


/******************************************************************************
Function Name   : usbh0_hstd_get_pipe_no
Description     : Get PIPE No.
Arguments       : uint16_t ip_no     : USB Module no.(USBH0_IP0/USBH0_IP1)
                : uint16_t address   : USB Device address
                : uint16_t class     : USB Device class(USBH0_HVND/USBH0_HCDC/USBH0_HHID/USBH0_HMSC/USBH0_HUB)
                : uint8_t  type      : Transfer Type.(USBH0_EP_BULK/USBH0_EP_INT)
                : uint8_t  dir       : (USBH0_PIPE_DIR_IN/USBH0_PIPE_DIR_OUT)
Return value    : Pipe no (USBH0_PIPE1->USBH0_PIPE9:OK, USBH0_NULL:Error)
******************************************************************************/
uint8_t usbh0_hstd_get_pipe_no (uint16_t address, uint16_t usbh0_class, uint8_t type, uint8_t dir)
{
    uint8_t     pipe_no = USBH0_NULL;

    switch (usbh0_class)
    {
        case USBH0_HHID:
            if (USBH0_EP_INT == type)
            {
                if (USBH0_PIPE_DIR_IN == dir)
                {
                    switch (address)
                    {
                        case 1: /* Root port device1 */
                        	pipe_no     = USBH0_CFG_HHID_INT_IN;
							break;
                        case 2: /* Hub downport device1 */
                            pipe_no     = USBH0_CFG_HHID_INT_IN2;
                        break;
                        case 3:/* Hub downport device2 */
                            pipe_no     = USBH0_CFG_HHID_INT_IN3;
                        break;
                        case 4:/* Hub downport device3 */
                            pipe_no     = USBH0_CFG_HHID_INT_IN4;
                        break;
                        case 5:/* Hub downport device3 */
                            pipe_no     = USBH0_CFG_HHID_INT_IN5;
                        break;
                        default:
                        break;
                    }
                }
                else
                {   /* Check root port device1 or Hub downport device1 */
                    if ((1 == address) || (2 == address))
                    {
                        pipe_no     = USBH0_CFG_HHID_INT_OUT;
                    }
                }
            }
        break;

        default:
            return USBH0_NULL;
        break;
    }

    return pipe_no;
} /* eof usbh0_hstd_get_pipe_no() */




/******************************************************************************
End of function usb_hhid_class_request_process
/******************************************************************************

 End  Of File
 ******************************************************************************/
