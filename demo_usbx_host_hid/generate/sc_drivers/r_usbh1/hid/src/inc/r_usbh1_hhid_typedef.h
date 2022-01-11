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
 * File Name    : r_usbh1_hhid.h
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
 *                           "hid_suspended"->"usbh1_hid_suspended"
 *                           "hid_configured"->"usbh1_hid_configured"
 *                           "hid_detach"->"usbh1_hid_detach"
 *                           "hid_resume_complete"->"usbh1_hid_resume_complete"
 *                           "hid_class_request" is deleted
 *                           "usbh1_hhid_get_pipetbl" is added.
 *                           "usbh1_hhid_get_hid_protocol" is added.
 *                           "usbh1_hhid_set_pipe_registration" is added.
 *                           "usbh1_hhid_class_check" is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 *         : 16.11.2018 1.24 Supporting RTOS
 ***********************************************************************************************************************/
#ifndef R_USBH1_HHID_TYPEDEF_H
#define R_USBH1_HHID_TYPEDEF_H

#include  "r_usbh1_basic_local.h"


/*****************************************************************************
 Macro definitions
 ******************************************************************************/

typedef struct usbh1_utr usbh1_hcdinfo_t;
typedef struct usbh1_utr usbh1_mgrinfo_t;
typedef struct usbh1_utr usbh1_pcdinfo_t;
typedef struct usbh1_utr usbh1_clsinfo_t;

#define USBH1_DIRFIELD                        (0x0010u)   /* Transfer direction select */
#define USBH1_DIR_H_OUT                       (0x0010u)   /* HOST OUT */
#define USBH1_DIR_P_IN                        (0x0010u)   /* PERI IN */
#define USBH1_DIR_H_IN                        (0x0000u)   /* HOST IN */
#define USBH1_DIR_P_OUT                       (0x0000u)   /* PERI OUT */

#define USBH1_EVENT_MAX                           (10)
#define USBH1_MAXPIPE_NUM                         (32u)
#define USBH1_REPORT_LENGTH                       (8)

/* Pipe configuration table define */
#define USBH1_TYPFIELD_ISO                    (0xC000u)   /* Isochronous */
#define USBH1_TYPFIELD_INT                    (0x8000u)   /* Interrupt */
#define USBH1_TYPFIELD_BULK                   (0x4000u)   /* Bulk */
#define USBH1_SHTNAKFIELD                     (0x0080u)   /* Transfer end NAK */
#define USBH1_DEVADDRBIT                      (12u)

typedef enum usbh1_class
{
    USBH1_PCDC = 0, USBH1_PCDCC, USBH1_PHID, USBH1_PVND,

    USBH1_HCDC, USBH1_HCDCC, USBH1_HHID, USBH1_HVND,

    USBH1_HMSC, USBH1_PMSC, USBH1_REQUEST
} usbh1_class_t;

typedef enum usbh1_err
{
    USBH1_SUCCESS = 0,            /* Successful */
    USBH1_ERR_PARA,               /* Invalid parameter */
    USBH1_ERR_BUSY,               /* API is busy now */
    USBH1_ERR_NOT_OPEN,           /* Device is not opened */
    USBH1_ERR_NOT_SUSPEND,        /* Device is not the suspend state */
    USBH1_ERR_OVER,               /* Received data size over */
    USBH1_ERR_SHORT,              /* Received data size short */
    USBH1_ERR_NG,                 /* Operation failed */
    USBH1_ERR_TMOUT,              /* Timeout */
} usbh1_err_t;

typedef enum usbh1_status
{
    USBH1_STS_POWERED = 0,
    USBH1_STS_DEFAULT,
    USBH1_STS_ADDRESS,
    USBH1_STS_CONFIGURED,
    USBH1_STS_SUSPEND,
    USBH1_STS_RESUME,
    USBH1_STS_DETACH,
    USBH1_STS_REQUEST,
    USBH1_STS_REQUEST_COMPLETE,
    USBH1_STS_READ_COMPLETE,
    USBH1_STS_WRITE_COMPLETE,
    USBH1_STS_BC,
    USBH1_STS_OVERCURRENT,
    USBH1_STS_NOT_SUPPORT,
    USBH1_STS_NONE,
    USBH1_STS_MSC_CMD_COMPLETE,
} usbh1_status_t;


/* USB status */
typedef enum usbh1_onoff
{
    USBH1_OFF = 0, USBH1_ON,
} usbh1_onoff_t;

typedef struct usbh1_setup
{
    uint16_t        type;       /* USB standard/class request type */
    uint16_t        value;      /* Request value */
    uint16_t        index;      /* Request index */
    uint16_t        length;     /* Request length */
    uint16_t        devadr;     /* Device Address */
} usbh1_setup_t;

typedef struct usbh1_ctrl
{
    uint8_t         module;     /* USB module number (USBH1_IP0/USBH1_IP1) */
    uint8_t         address;    /* USB device address */
    uint8_t         pipe;       /* USB pipe number */
    uint8_t         type;       /* USB device class etc */
    uint8_t         status;     /* USB device state etc */
    uint32_t        size;       /* Read data size */
    usbh1_setup_t     setup;      /* usbh1_setup_t structure area */
} usbh1_ctrl_t;

typedef struct usbh1_info
{
    uint8_t type;               /* USB device class type */
    uint8_t speed;              /* USB speed */
    uint8_t status;             /* USB device status */
    uint8_t port;               /* Battery charging information */
} usbh1_info_t;

typedef struct usbh1_event
{
    uint8_t         write_pointer;       /* Write pointer */
    uint8_t         read_pointer;        /* Read pointer */
    usbh1_status_t    code[USBH1_EVENT_MAX]; /* Event code */
    usbh1_ctrl_t      ctrl[USBH1_EVENT_MAX]; /* Control Information */
} usbh1_event_t;


typedef struct usbh1_pipe_reg
{
    uint16_t    pipe_cfg;
    uint16_t    pipe_maxp;
    uint16_t    pipe_peri;
} usbh1_pipe_table_reg_t;

typedef struct usbh1_pipe_table
{
    uint16_t    use_flag;
    uint16_t    pipe_cfg;
    uint16_t    pipe_maxp;
    uint16_t    pipe_peri;
} usbh1_pipe_table_t;


typedef struct
{
    uint16_t                        devadr;
    uint16_t                        bRequestCode;
    void                            *tranadr;       /* Transfer data Start address */
    uint32_t                        tranlen;        /* Transfer data length */
    uint16_t                        index;          /* Descriptor index */
    uint16_t                        duration;       /* Report interval */
    uint8_t                         set_protocol;   /* Protocol(0:Boot,1:Report) */
    usbh1_utr_cb_t                  complete;       /* Call Back Function Info */
} USBH1_HHID_CLASS_REQUEST_PARM_t;

#endif  /* R_USBH1_HHID_H */

/******************************************************************************
 End  Of File
 ******************************************************************************/
