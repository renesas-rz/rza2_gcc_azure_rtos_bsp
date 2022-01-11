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
 * File Name    : r_usbh0_hhid.h
 * Description  : USB Host HID class driver header
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.21 hid_class_request_complete is deleted.
 *         : 30.09.2017 1.22 Change function name ""->""
 *                           "hid_suspended"->"usbh0_hid_suspended"
 *                           "hid_configured"->"usbh0_hid_configured"
 *                           "hid_detach"->"usbh0_hid_detach"
 *                           "hid_resume_complete"->"usbh0_hid_resume_complete"
 *                           "hid_class_request" is deleted
 *                           "usbh0_hhid_get_pipetbl" is added.
 *                           "usbh0_hhid_get_hid_protocol" is added.
 *                           "usbh0_hhid_set_pipe_registration" is added.
 *                           "usbh0_hhid_class_check" is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 *         : 16.11.2018 1.24 Supporting RTOS
 ***********************************************************************************************************************/
#ifndef R_USBH0_HHID_TYPEDEF_H
#define R_USBH0_HHID_TYPEDEF_H

#include  "r_usbh0_basic_local.h"


/*****************************************************************************
 Macro definitions
 ******************************************************************************/

typedef struct usbh0_utr usbh0_hcdinfo_t;
typedef struct usbh0_utr usbh0_mgrinfo_t;
typedef struct usbh0_utr usbh0_pcdinfo_t;
typedef struct usbh0_utr usbh0_clsinfo_t;

#define USBH0_DIRFIELD                        (0x0010u)   /* Transfer direction select */
#define USBH0_DIR_H_OUT                       (0x0010u)   /* HOST OUT */
#define USBH0_DIR_P_IN                        (0x0010u)   /* PERI IN */
#define USBH0_DIR_H_IN                        (0x0000u)   /* HOST IN */
#define USBH0_DIR_P_OUT                       (0x0000u)   /* PERI OUT */

#define USBH0_EVENT_MAX                           (10)
#define USBH0_MAXPIPE_NUM                         (32u)
#define USBH0_REPORT_LENGTH                       (8)

/* Pipe configuration table define */
#define USBH0_TYPFIELD_ISO                    (0xC000u)   /* Isochronous */
#define USBH0_TYPFIELD_INT                    (0x8000u)   /* Interrupt */
#define USBH0_TYPFIELD_BULK                   (0x4000u)   /* Bulk */
#define USBH0_SHTNAKFIELD                     (0x0080u)   /* Transfer end NAK */
#define USBH0_DEVADDRBIT                      (12u)

typedef enum usbh0_class
{
    USBH0_PCDC = 0, USBH0_PCDCC, USBH0_PHID, USBH0_PVND,

    USBH0_HCDC, USBH0_HCDCC, USBH0_HHID, USBH0_HVND,

    USBH0_HMSC, USBH0_PMSC, USBH0_REQUEST
} usbh0_class_t;

typedef enum usbh0_err
{
    USBH0_SUCCESS = 0,            /* Successful */
    USBH0_ERR_PARA,               /* Invalid parameter */
    USBH0_ERR_BUSY,               /* API is busy now */
    USBH0_ERR_NOT_OPEN,           /* Device is not opened */
    USBH0_ERR_NOT_SUSPEND,        /* Device is not the suspend state */
    USBH0_ERR_OVER,               /* Received data size over */
    USBH0_ERR_SHORT,              /* Received data size short */
    USBH0_ERR_NG,                 /* Operation failed */
    USBH0_ERR_TMOUT,              /* Timeout */
} usbh0_err_t;

typedef enum usbh0_status
{
    USBH0_STS_POWERED = 0,
    USBH0_STS_DEFAULT,
    USBH0_STS_ADDRESS,
    USBH0_STS_CONFIGURED,
    USBH0_STS_SUSPEND,
    USBH0_STS_RESUME,
    USBH0_STS_DETACH,
    USBH0_STS_REQUEST,
    USBH0_STS_REQUEST_COMPLETE,
    USBH0_STS_READ_COMPLETE,
    USBH0_STS_WRITE_COMPLETE,
    USBH0_STS_BC,
    USBH0_STS_OVERCURRENT,
    USBH0_STS_NOT_SUPPORT,
    USBH0_STS_NONE,
    USBH0_STS_MSC_CMD_COMPLETE,
} usbh0_status_t;


/* USB status */
typedef enum usbh0_onoff
{
    USBH0_OFF = 0, USBH0_ON,
} usbh0_onoff_t;

typedef struct usbh0_setup
{
    uint16_t        type;       /* USB standard/class request type */
    uint16_t        value;      /* Request value */
    uint16_t        index;      /* Request index */
    uint16_t        length;     /* Request length */
    uint16_t        devadr;     /* Device Address */
} usbh0_setup_t;

typedef struct usbh0_ctrl
{
    uint8_t         module;     /* USB module number (USBH0_IP0/USBH0_IP1) */
    uint8_t         address;    /* USB device address */
    uint8_t         pipe;       /* USB pipe number */
    uint8_t         type;       /* USB device class etc */
    uint8_t         status;     /* USB device state etc */
    uint32_t        size;       /* Read data size */
    usbh0_setup_t     setup;      /* usbh0_setup_t structure area */
} usbh0_ctrl_t;

typedef struct usbh0_info
{
    uint8_t type;               /* USB device class type */
    uint8_t speed;              /* USB speed */
    uint8_t status;             /* USB device status */
    uint8_t port;               /* Battery charging information */
} usbh0_info_t;

typedef struct usbh0_event
{
    uint8_t         write_pointer;       /* Write pointer */
    uint8_t         read_pointer;        /* Read pointer */
    usbh0_status_t    code[USBH0_EVENT_MAX]; /* Event code */
    usbh0_ctrl_t      ctrl[USBH0_EVENT_MAX]; /* Control Information */
} usbh0_event_t;


typedef struct usbh0_pipe_reg
{
    uint16_t    pipe_cfg;
    uint16_t    pipe_maxp;
    uint16_t    pipe_peri;
} usbh0_pipe_table_reg_t;

typedef struct usbh0_pipe_table
{
    uint16_t    use_flag;
    uint16_t    pipe_cfg;
    uint16_t    pipe_maxp;
    uint16_t    pipe_peri;
} usbh0_pipe_table_t;


typedef struct
{
    uint16_t                        devadr;
    uint16_t                        bRequestCode;
    void                            *tranadr;       /* Transfer data Start address */
    uint32_t                        tranlen;        /* Transfer data length */
    uint16_t                        index;          /* Descriptor index */
    uint16_t                        duration;       /* Report interval */
    uint8_t                         set_protocol;   /* Protocol(0:Boot,1:Report) */
    usbh0_utr_cb_t                  complete;       /* Call Back Function Info */
} USBH0_HHID_CLASS_REQUEST_PARM_t;

#endif  /* R_USBH0_HHID_H */

/******************************************************************************
 End  Of File
 ******************************************************************************/
