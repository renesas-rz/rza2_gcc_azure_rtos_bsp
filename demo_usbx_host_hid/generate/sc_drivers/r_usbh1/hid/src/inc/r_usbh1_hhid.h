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
#ifndef R_USBH1_HHID_H
#define R_USBH1_HHID_H

#include  "r_usbh1_basic_local.h"
#include  "r_usbh1_hhid.h"
#include  "r_usbh1_hhid_if.h"
#include  "r_usbh1_hhid_typedef.h"


/*****************************************************************************
 Macro definitions
 ******************************************************************************/
/* ----- DESCRIPTOR Types ----- */
#define USBH1_HID_DT_TYPE_GETREPORT           ((uint8_t)0x01)     /* Report Type Input */
#define USBH1_HID_DT_TYPE_SETREPORT           ((uint8_t)0x02)     /* Report Type Output */
#define USBH1_HID_DT_TYPE_HIDDESCRIPTOR       ((uint8_t)0x21)     /* HID descriptor type */
#define USBH1_HID_DT_TYPE_RPTDESCRIPTOR       ((uint8_t)0x22)     /* Report descriptor type */

/* ----- Subclass Codes ----- */
#define USBH1_HID_IFSUB_NONE                  ((uint8_t)0x00)     /* No Subclass */
#define USBH1_HID_IFSUB_BOOT                  ((uint8_t)0x01)     /* Boot Interface Subclass */

/* Report Type set */
#define USBH1_HID_SET_DT_TYPE(v)              ((v) << 8)

#define USBH1_HHID_CLSDATASIZE                (512)

#define USBH1_MAX_DEVICE                      (5)                 /* 1-3 */

#if (BSP_CFG_RTOS_USED)
/* Host HID Task */
#define USBH1_HHID_TSK                        (USBH1_TID_9)         /* Task ID */
#define USBH1_HHID_MBX                        (USBH1_HHID_TSK)      /* Mailbox ID */
#define USBH1_HHID_MPL                        (USBH1_HHID_TSK)      /* Memorypool ID */
#define USBH1_HHID_MSG                        (USBH1_HHID_TSK)        /* Message ID */
#else /* (BSP_CFG_RTOS_USED) */
/* Host HID Task */
#define USBH1_HHID_TSK                        (USBH1_TID_7)         /* Task ID */
#define USBH1_HHID_MBX                        (USBH1_HHID_TSK)      /* Mailbox ID */
#define USBH1_HHID_MPL                        (USBH1_HHID_TSK)      /* Memorypool ID */
#define USBH1_HHID_MSG                        (USBH1_HHID_TSK)        /* Message ID */
#endif /* (BSP_CFG_RTOS_USED) */

#define USBH1_B_INTERFACE_PROTOCOL            (7)                 /* Interface Descriptor bInterfaceProtocol */

/* PIPE No. */
#define   USBH1_PIPE0   (0x0000u)	/* PIPE 0 */
#define   USBH1_PIPE1	(0x0001u)	/* PIPE 1 */
#define   USBH1_PIPE2	(0x0002u)	/* PIPE 2 */
#define   USBH1_PIPE3	(0x0003u)	/* PIPE 3 */
#define   USBH1_PIPE4	(0x0004u)	/* PIPE 4 */
#define   USBH1_PIPE5	(0x0005u)	/* PIPE 5 */
#define   USBH1_PIPE6	(0x0006u)	/* PIPE 6 */
#define   USBH1_PIPE7	(0x0007u)	/* PIPE 7 */
#define   USBH1_PIPE8	(0x0008u)	/* PIPE 8 */
#define   USBH1_PIPE9	(0x0009u)	/* PIPE 9 */
#define   USBH1_PIPE10	(0x000Au)	/* PIPE 10 */
#define   USBH1_PIPE11	(0x000Bu)	/* PIPE 11 */
#define   USBH1_PIPE12	(0x000Cu)	/* PIPE 12 */
#define   USBH1_PIPE13	(0x000Du)	/* PIPE 13 */
#define   USBH1_PIPE14	(0x000Eu)	/* PIPE 14 */

/*****************************************************************************
 Enumerated Types
 ******************************************************************************/
/* Host HID Task Command */
typedef enum
{
    USBH1_HHID_TCMD_OPEN
} usbh1_hhid_tcmd_t;

/* Enumeration Sequence */
typedef enum
{
    /* Enumeration Sequence Complete */
    USBH1_HHID_ENUM_COMPLETE = 0,

    /* Enumeration Sequence String Descriptor #0 receive request */
    USBH1_HHID_ENUM_STR_DT0_REQ,

    /* Enumeration Sequence String Descriptor #0 Receive complete */
    USBH1_HHID_ENUM_STR_DT0_WAIT,

    /* StringDescriptor iProduct Receive complete */
    USBH1_HHID_ENUM_STR_IPRODUCT_WAIT
} usbh1_hhid_enum_seq_t;

/*****************************************************************************
 Struct definition
 ******************************************************************************/

/******************************************************************************
 Exported global variables
 ******************************************************************************/
extern uint16_t g_usbh1_hhid_protocol[];                                    /* HID Device protocol(Mouse/Keyboard) */
extern uint16_t g_usbh1_hhid_num_endpoint[];                                /* Num Endpoints */
extern uint16_t g_usbh1_hhid_outpipe_support[];                             /* OUT pipe support */
extern uint16_t g_usbh1_hhid_devaddr;                                       /* Device Address */
extern uint16_t g_usbh1_hhid_speed;                                         /* Device speed */
extern uint16_t g_usbh1_hhid_enum_seq;                                      /* Enumeration Sequence */
extern uint8_t *g_p_usbh1_hhid_interface_table;                             /* Interface Descriptor Table */
extern uint8_t *g_p_usbh1_hhid_device_table;                                /* Device Descriptor Table */
extern uint8_t *g_p_usbh1_hhid_config_table;                                /* Configuration Descriptor Table */
extern uint16_t g_usbh1_hhid_init_seq;       
extern usbh1_event_t    g_usbh1_cstd_event;
extern USBH1_HHID_CLASS_REQUEST_PARM_t   usbh1_ghhid_class_request_parm;   /* Host HID Class Request Parametor */

/*****************************************************************************
 Public Functions
 ******************************************************************************/
/* Functions */
void        usbh1_hhid_task (void);
uint16_t    usbh1_hhid_pipe_info (st_usbh1_utr_t *ptr, uint8_t *table, uint16_t speed, uint16_t length);
uint16_t    usbh1_hhid_get_string_desc (st_usbh1_utr_t *ptr, uint16_t addr, uint16_t string, usbh1_utr_cb_t complete);
uint8_t     usbh1_hhid_get_hid_protocol (uint16_t devadr);
void        usbh1_hhid_class_check (uint16_t **table);
uint8_t     usbh1_hstd_make_pipe_reg_info (uint16_t address, uint16_t usbh1_class, uint16_t speed,
                                    uint8_t  *descriptor, usbh1_pipe_table_reg_t *pipe_table_work);
void        usbh1_hstd_set_pipe_info (uint16_t pipe_no, usbh1_pipe_table_reg_t *src_ep_tbl);
uint8_t     usbh1_hstd_get_pipe_no (uint16_t address, uint16_t usbh1_class, uint8_t type, uint8_t dir);

#endif  /* R_USBH1_HHID_H */

/******************************************************************************
 End  Of File
 ******************************************************************************/
