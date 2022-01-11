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
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh0_basic_if.h
 * Version      : 1.0
 * Description  : Interface file for USB basic API for RZ/T1
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH0_BASIC_IF_H
#define R_USBH0_BASIC_IF_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#if 0
#ifdef   __ICCARM__
#include <stdint.h>
#endif
#ifdef   __CC_ARM
#include <stdint.h>
#endif
#ifdef __GNUC__
#include "platform.h"
#endif
#endif

#include <stdint.h>
#include <stddef.h>
#include "r_usbh0_basic_config.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define USBH0_NULL                          (0u)
#define USBH0_TRUE                          ((uint16_t)(1u))
#define USBH0_FALSE                         ((uint16_t)(0u))
#define USBH0_OK                            ((usbh0_er_t)(0x0000))
#define USBH0_ERROR                         ((usbh0_er_t)(0xFFFF))

/* Scheduler use define */
#define USBH0_TBLCLR                        (0u)                /* Table clear */
#define USBH0_CNTCLR                        (0u)                /* Counter clear */
#define USBH0_FLGCLR                        (0u)                /* Flag clear */
#define USBH0_FLGSET                        (1u)                /* Flag set */
#define USBH0_IDCLR                         (0xFFu)             /* Priority clear */

/* Task ID define */
#define USBH0_TID_0                         (0u)                /* Task ID 0 */
#define USBH0_TID_1                         (1u)                /* Task ID 1 */
#define USBH0_TID_2                         (2u)                /* Task ID 2 */
#define USBH0_TID_3                         (3u)                /* Task ID 3 */
#define USBH0_TID_4                         (4u)                /* Task ID 4 */
#define USBH0_TID_5                         (5u)                /* Task ID 5 */
#define USBH0_TID_6                         (6u)                /* Task ID 6 */
#define USBH0_TID_7                         (7u)                /* Task ID 7 */
#define USBH0_TID_8                         (8u)                /* Task ID 8 */
#define USBH0_TID_9                         (9u)                /* Task ID 9 */
#define USBH0_TID_10                        (10u)               /* Task ID 9 */

/* Task priority define */
#define USBH0_PRI_0                         (0u)                /* Priority 0 */
#define USBH0_PRI_1                         (1u)                /* Priority 1 */
#define USBH0_PRI_2                         (2u)                /* Priority 2 */
#define USBH0_PRI_3                         (3u)                /* Priority 3 */
#define USBH0_PRI_4                         (4u)                /* Priority 4 */
#define USBH0_PRI_5                         (5u)                /* Priority 5 */
#define USBH0_PRI_6                         (6u)                /* Priority 6 */
#define USBH0_PRI_7                         (7u)                /* Priority 7 */

/* nonOS Use */
#define USBH0_SEQ_0                         ((uint16_t)0x0000)  /* Sequence 0 */
#define USBH0_SEQ_1                         ((uint16_t)0x0001)  /* Sequence 1 */
#define USBH0_SEQ_2                         ((uint16_t)0x0002)  /* Sequence 2 */
#define USBH0_SEQ_3                         ((uint16_t)0x0003)  /* Sequence 3 */
#define USBH0_SEQ_4                         ((uint16_t)0x0004)  /* Sequence 4 */
#define USBH0_SEQ_5                         ((uint16_t)0x0005)  /* Sequence 5 */
#define USBH0_SEQ_6                         ((uint16_t)0x0006)  /* Sequence 6 */
#define USBH0_SEQ_7                         ((uint16_t)0x0007)  /* Sequence 7 */
#define USBH0_SEQ_8                         ((uint16_t)0x0008)  /* Sequence 8 */
#define USBH0_SEQ_9                         ((uint16_t)0x0009)  /* Sequence 9 */
#define USBH0_SEQ_10                        ((uint16_t)0x000a)  /* Sequence 10 */

/* Standard Device Descriptor Offset Define */
#define USBH0_DEV_B_LENGTH                  (0u)        /* Size of descriptor */
#define USBH0_DEV_B_DESCRIPTOR_TYPE         (1u)        /* Descriptor type */
#define USBH0_DEV_BCD_USB_L                 (2u)        /* USB Specification Release Number */
#define USBH0_DEV_BCD_USB_H                 (3u)        /* USB Specification Release Number */
#define USBH0_DEV_B_DEVICE_CLASS            (4u)        /* Class code */
#define USBH0_DEV_B_DEVICE_SUBCLASS         (5u)        /* Subclass code */
#define USBH0_DEV_B_DEVICE_PROTOCOL         (6u)        /* Protocol code */
#define USBH0_DEV_B_MAX_PACKET_SIZE_0       (7u)        /* Max packet size for EP0(only 8,16,32,64 are valid) */
#define USBH0_DEV_ID_VENDOR_L               (8u)        /* Vendor ID */
#define USBH0_DEV_ID_VENDOR_H               (9u)        /* Vendor ID */
#define USBH0_DEV_ID_PRODUCT_L              (10u)       /* Product ID */
#define USBH0_DEV_ID_PRODUCT_H              (11u)       /* Product ID */
#define USBH0_DEV_BCD_DEVICE_L              (12u)       /* Device relese number */
#define USBH0_DEV_BCD_DEVICE_H              (13u)       /* Device relese number */
#define USBH0_DEV_I_MANUFACTURER            (14u)       /* Index of string descriptor describing manufacturer */
#define USBH0_DEV_I_PRODUCT                 (15u)       /* Index of string descriptor describing product */
#define USBH0_DEV_I_SERIAL_NUMBER           (16u)       /* Device serial number */
#define USBH0_DEV_B_NUM_CONFIGURATION       (17u)       /* Number of possible configuration */

/* Standard Configuration Descriptor Offset Define */
#define USBH0_CON_W_TOTAL_LENGTH_L          (2u)        /* Total length of data returned for this configuration */
#define USBH0_CON_W_TOTAL_LENGTH_H          (3u)        /* Total length of data returned for this configuration */
#define USBH0_CON_B_NUM_INTERFACES          (4u)        /* Number of interfaces supported by this configuration */
#define USBH0_CON_B_CONFIGURATION_VALUE     (5u)        /* Configuration value */
#define USBH0_CON_I_CONFIGURATION           (6u)        /* Index of string descriptor describing this configuration */
#define USBH0_CON_BM_ATTRIBUTES             (7u)        /* Configuration characteristics */
#define USBH0_CON_B_MAX_POWER               (8u)        /* Max power consumption of the USB device from the bus */

/* Standard Interface Descriptor Offset Define */
#define USBH0_IF_B_INTERFACENUMBER          (2u)        /* bInterfaceNumber */
#define USBH0_IF_B_ALTERNATESETTING         (3u)        /* bAlternateSetting */
#define USBH0_IF_B_NUMENDPOINTS             (4u)        /* bNumEndpoints */
#define USBH0_IF_B_INTERFACECLASS           (5u)        /* bInterfaceClass */
#define USBH0_IF_B_INTERFACESUBCLASS        (6u)        /* bInterfaceSubClass */
#define USBH0_IF_B_INTERFACEPROTOCOL        (7u)        /* bInterfacePtorocol */
#define USBH0_IF_I_INTERFACE                (8u)        /* iInterface */

/* Standard Endpoint descriptor Offset Define */
#define USBH0_EP_B_EPADDRESS                (2u)        /* bEndpointAddress */
#define USBH0_EP_B_ATTRIBUTES               (3u)        /* bmAttributes */
#define USBH0_EP_W_MPS_L                    (4u)        /* wMaxPacketSize */
#define USBH0_EP_W_MPS_H                    (5u)        /* wMaxPacketSize */
#define USBH0_EP_B_INTERVAL                 (6u)        /* bInterval */

/* Descriptor type  Define */
#define USBH0_DT_DEVICE                     (0x01u)     /* Configuration Descriptor */
#define USBH0_DT_CONFIGURATION              (0x02u)     /* Configuration Descriptor */
#define USBH0_DT_STRING                     (0x03u)     /* Configuration Descriptor */
#define USBH0_DT_INTERFACE                  (0x04u)     /* Interface Descriptor */
#define USBH0_DT_ENDPOINT                   (0x05u)     /* Endpoint Descriptor */
#define USBH0_DT_DEVICE_QUALIFIER           (0x06u)     /* Device Qualifier Descriptor */
#define USBH0_DT_OTHER_SPEED_CONF           (0x07u)     /* Other Speed Configuration Descriptor */
#define USBH0_DT_INTERFACE_POWER            (0x08u)     /* Interface Power Descriptor */
#define USBH0_DT_OTGDESCRIPTOR              (0x09u)     /* OTG Descriptor */
#define USBH0_DT_HUBDESCRIPTOR              (0x29u)     /* HUB descriptor */

/* Interface class Define */
#define USBH0_IFCLS_NOT                     (0x00u)     /* Un corresponding Class */
#define USBH0_IFCLS_AUD                     (0x01u)     /* Audio Class */
#define USBH0_IFCLS_CDCC                    (0x02u)     /* CDC-Control Class */
#define USBH0_IFCLS_HID                     (0x03u)     /* HID Class */
#define USBH0_IFCLS_PHY                     (0x05u)     /* Physical Class */
#define USBH0_IFCLS_IMG                     (0x06u)     /* Image Class */
#define USBH0_IFCLS_PRN                     (0x07u)     /* Printer Class */
#define USBH0_IFCLS_MAS                     (0x08u)     /* Mass Storage Class */
#define USBH0_IFCLS_HUB                     (0x09u)     /* HUB Class */
#define USBH0_IFCLS_CDCD                    (0x0Au)     /* CDC-Data Class */
#define USBH0_IFCLS_CHIP                    (0x0Bu)     /* Chip/Smart Card Class */
#define USBH0_IFCLS_CNT                     (0x0Cu)     /* Content-Security Class */
#define USBH0_IFCLS_VID                     (0x0Du)     /* Video Class */
#define USBH0_IFCLS_DIAG                    (0xDCu)     /* Diagnostic Device */
#define USBH0_IFCLS_WIRE                    (0xE0u)     /* Wireless Controller */
#define USBH0_IFCLS_APL                     (0xFEu)     /* Application-Specific */
#define USBH0_IFCLS_VEN                     (0xFFu)     /* Vendor-Specific Class */

/* Endpoint Descriptor  Define */
#define USBH0_EP_DIRMASK                    (0x80u)     /* Endpoint direction mask [2] */
#define USBH0_EP_IN                         (0x80u)     /* In Endpoint */
#define USBH0_EP_OUT                        (0x00u)     /* Out Endpoint */
#define USBH0_EP_NUMMASK                    (0x0Fu)     /* Endpoint number mask [2] */
#define USBH0_EP_USGMASK                    (0x30u)     /* Usage type mask [2] */
#define USBH0_EP_SYNCMASK                   (0x0Cu)     /* Synchronization type mask [2] */
#define USBH0_EP_TRNSMASK                   (0x03u)     /* Transfer type mask [2] */
#define USBH0_EP_CNTRL                      (0x00u)     /* Control Transfer */
#define USBH0_EP_ISO                        (0x01u)     /* Isochronous Transfer */
#define USBH0_EP_BULK                       (0x02u)     /* Bulk Transfer */
#define USBH0_EP_INT                        (0x03u)     /* Interrupt Transfer */

/* Configuration descriptor bit define */
#define USBH0_CF_RESERVED                   (0x80u)     /* Reserved(set to 1) */
#define USBH0_CF_SELFP                      (0x40u)     /* Self Powered */
#define USBH0_CF_BUSP                       (0x00u)     /* Bus Powered */
#define USBH0_CF_RWUPON                     (0x20u)     /* Remote Wakeup ON */
#define USBH0_CF_RWUPOFF                    (0x00u)     /* Remote Wakeup OFF */

/* OTG descriptor bit define */
#define USBH0_OTG_HNP                       (0x02u)     /* HNP support */
#define USBH0_OTG_SRP                       (0x01u)     /* SRP support */

/* USB Standard request */
/* USB_BREQUEST         0xFF00u(b15-8) */
#define USBH0_GET_STATUS                    (0x0000u)
#define USBH0_CLEAR_FEATURE                 (0x0100u)
#define USBH0_REQRESERVED                   (0x0200u)
#define USBH0_SET_FEATURE                   (0x0300u)
#define USBH0_REQRESERVED1                  (0x0400u)
#define USBH0_SET_ADDRESS                   (0x0500u)
#define USBH0_GET_DESCRIPTOR                (0x0600u)
#define USBH0_SET_DESCRIPTOR                (0x0700u)
#define USBH0_GET_CONFIGURATION             (0x0800u)
#define USBH0_SET_CONFIGURATION             (0x0900u)
#define USBH0_GET_INTERFACE                 (0x0A00u)
#define USBH0_SET_INTERFACE                 (0x0B00u)
#define USBH0_SYNCH_FRAME                   (0x0C00u)

/* USB_BMREQUESTTYPEDIR 0x0080u(b7) */
#define USBH0_HOST_TO_DEV                   (0x0000u)
#define USBH0_DEV_TO_HOST                   (0x0080u)

/* USB_BMREQUESTTYPETYPE    0x0060u(b6-5) */
#define USBH0_STANDARD                      (0x0000u)
#define USBH0_CLASS                         (0x0020u)
#define USBH0_VENDOR                        (0x0040u)

/* USB_BMREQUESTTYPERECIP   0x001Fu(b4-0) */
#define USBH0_DEVICE                        (0x0000u)
#define USBH0_INTERFACE                     (0x0001u)
#define USBH0_ENDPOINT                      (0x0002u)
#define USBH0_OTHER                         (0x0003u)

/* GET_STATUS request information */
/* Standard Device status */
#define USBH0_GS_BUSPOWERD                  (0x0000u)
#define USBH0_GS_SELFPOWERD                 (0x0001u)
#define USBH0_GS_REMOTEWAKEUP               (0x0002u)

/* Endpoint status */
#define USBH0_GS_NOTHALT                    (0x0000u)
#define USBH0_GS_HALT                       (0x0001u)

/* CLEAR_FEATURE/GET_FEATURE/SET_FEATURE request information */
/* Standard Feature Selector */
#define USBH0_ENDPOINT_HALT                 (0x0000u)
#define USBH0_DEV_REMOTE_WAKEUP             (0x0001u)
#define USBH0_TEST_MODE                     (0x0002u)

/* GET_DESCRIPTOR/SET_DESCRIPTOR request information */
/* Standard Descriptor type */
#define USBH0_HUB_DESCRIPTOR                (0x0000u)
#define USBH0_DEV_DESCRIPTOR                (0x0100u)
#define USBH0_CONF_DESCRIPTOR               (0x0200u)
#define USBH0_STRING_DESCRIPTOR             (0x0300u)
#define USBH0_INTERFACE_DESCRIPTOR          (0x0400u)
#define USBH0_ENDPOINT_DESCRIPTOR           (0x0500u)
#define USBH0_DEV_QUALIFIER_DESCRIPTOR      (0x0600u)
#define USBH0_OTHER_SPEED_CONF_DESCRIPTOR   (0x0700u)
#define USBH0_INTERFACE_POWER_DESCRIPTOR    (0x0800u)

/* HUB CLASS REQUEST */
#define USBH0_HUB_CLEAR_TT_BUFFER           (0x0800u)
#define USBH0_HUB_RESET_TT                  (0x0900u)
#define USBH0_HUB_GET_TT_STATE              (0x0A00u)
#define USBH0_HUB_STOP_TT                   (0x0B00u)

/* HUB CLASS FEATURE SELECTER */
#define USBH0_HUB_C_HUB_LOCAL_POWER         (0x0000u)
#define USBH0_HUB_C_HUB_OVER_CURRENT        (0x0001u)
#define USBH0_HUB_PORT_CONNECTION           (0x0000u)
#define USBH0_HUB_PORT_ENABLE               (0x0001u)
#define USBH0_HUB_PORT_SUSPEND              (0x0002u)
#define USBH0_HUB_PORT_OVER_CURRENT         (0x0003u)
#define USBH0_HUB_PORT_RESET                (0x0004u)
#define USBH0_HUB_PORT_POWER                (0x0008u)
#define USBH0_HUB_PORT_LOW_SPEED            (0x0009u)
#define USBH0_HUB_PORT_HIGH_SPEED           (0x000Au)
#define USBH0_HUB_C_PORT_CONNECTION         (0x0010u)
#define USBH0_HUB_C_PORT_ENABLE             (0x0011u)
#define USBH0_HUB_C_PORT_SUSPEND            (0x0012u)
#define USBH0_HUB_C_PORT_OVER_CURRENT       (0x0013u)
#define USBH0_HUB_C_PORT_RESET              (0x0014u)
#define USBH0_HUB_PORT_TEST                 (0x0015u)
#define USBH0_HUB_PORT_INDICATOR            (0x0016u)

/* Root port */
#define USBH0_PORT0                         (0u)
#define USBH0_NOPORT                        (0xFFFFu)   /* Not connect */

/* Device connect information */
#define USBH0_ATTACH                        (0x0040u)
#define USBH0_DETACH                        (0x0043u)

/* Reset Handshake result */
#define USBH0_NOCONNECT                     (0x0000u)   /* Speed undecidedness */
#define USBH0_HSCONNECT                     (0x00C0u)   /* Hi-Speed connect */
#define USBH0_FSCONNECT                     (0x0080u)   /* Full-Speed connect */
#define USBH0_LSCONNECT                     (0x0040u)   /* Low-Speed connect */

/* Pipe define */
#define USBH0_PIPE0                         (0x0000u)   /* PIPE 0 */

/* Device Addr */
#define USBH0_DEVICE_0                      (0x0000u)   /* Device address 0 */
#define USBH0_NODEVICE                      (0xF000u)   /* No device */

/* Callback argument */
#define USBH0_NO_ARG                        (0u)

/* Device class Define  */
#define USBH0_NOVENDOR                      (0xFFFFu)   /* Vendor ID nocheck */
#define USBH0_NOPRODUCT                     (0xFFFFu)   /* Product ID nocheck */

/* Transfer status Type */
#define USBH0_CTRL_END                      (0x0000u)
#define USBH0_DATA_NONE                     (0x0001u)
#define USBH0_DATA_WAIT                     (0x0002u)
#define USBH0_DATA_OK                       (0x0003u)
#define USBH0_DATA_SHT                      (0x0004u)
#define USBH0_DATA_OVR                      (0x0005u)
#define USBH0_DATA_STALL                    (0x0006u)
#define USBH0_DATA_ERR                      (0x0007u)
#define USBH0_DATA_STOP                     (0x0008u)
#define USBH0_DATA_READING                  (0x0009u)
#define USBH0_DATA_WRITING                  (0x000Au)

/* CLS task message command */
#define USBH0_MSG_CLS_CHECKREQUEST          (0x0201u)
#define USBH0_MSG_CLS_INIT                  (0x0202u)
#define USBH0_MSG_CLS_TASK                  (0x0203u)
#define USBH0_MSG_CLS_WAIT                  (0x0204u)
#define USBH0_MSG_CLS_PROCESSRESULT         (0x0205u)

#define USBH0_DO_GLOBAL_SUSPEND             (0x0301u)   /* USB_MSG_HCD_SUSPEND */
#define USBH0_DO_GLOBAL_RESUME              (0x0302u)   /* USB_MSG_HCD_RESUME */
#define USBH0_DO_CLR_STALL                  (0x0303u)   /* USB_MSG_HCD_CLR_STALL */

/* Hub Task */
#define USBH0_HUB_TSK                       (USBH0_TID_2)       /* Task ID */
#define USBH0_HUB_MBX                       (USBH0_HUB_TSK)     /* Mailbox ID */
#define USBH0_HUB_MPL                       (USBH0_HUB_TSK)     /* Memorypool ID */
#define USBH0_HUB_MSG                       (USBH0_HUB_TSK)     /* Message ID */

#ifdef USBH0_HOST_COMPLIANCE_MODE
#define USBH0_COMP_ATTACH                   (0x00u)
#define USBH0_COMP_DETACH                   (0x01u)
#define USBH0_COMP_TPL                      (0x02u)
#define USBH0_COMP_NOTTPL                   (0x03u)
#define USBH0_COMP_HUB                      (0x04u)
#define USBH0_COMP_STOP                     (0x05u)
#define USBH0_COMP_NOTRESP                  (0x06u)
#define USBH0_COMP_VID                      (0x07u)
#define USBH0_COMP_PID                      (0x08u)
#define USBH0_COMP_ERR                      (0x09u)
#endif /* USBH0_HOST_COMPLIANCE_MODE */

#define R_USBH0_SND_MSG(ID, MESS)           R_USBH0_CstdSndMsg((uint8_t)ID, (usbh0_msg_t*)MESS)
#define R_USBH0_WAI_MSG(ID, MESS, TM)       R_USBH0_CstdWaiMsg((uint8_t)ID, (usbh0_msg_t*)MESS, (uint16_t)TM)
#define R_USBH0_RCV_MSG(ID, MESS)           R_USBH0_CstdRecMsg((uint8_t)ID, (usbh0_msg_t**)MESS)
#define R_USBH0_PGET_BLK(ID, BLK)           R_USBH0_CstdPgetBlk((uint8_t)ID, (st_usbh0_utr_t**)BLK)
#define R_USBH0_REL_BLK(ID, BLK)            R_USBH0_CstdRelBlk((uint8_t)ID,  (st_usbh0_utr_t*)BLK)

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/* USB message Type*/
typedef void                usbh0_msg_t;

/* USB error Type */
typedef uint16_t            usbh0_er_t;

/* USB ClassCheck Callback Type */
typedef void                (*usbh0_cb_check_t)(uint16_t**);

/* USB Standard Callback Type */
typedef void                (*usbh0_cb_t)(uint16_t devaddr);

/* USB Transfer Request Callback Type */
typedef struct usbh0_utr    st_usbh0_utr_t;
typedef void                (*usbh0_utr_cb_t)(st_usbh0_utr_t*);

/* Registration Structure */
typedef struct
{
    uint16_t            rootport;       /* Root port */
    uint16_t            devaddr;        /* Device address */
    uint16_t            devstate;       /* Device state */
    uint16_t            ifclass;        /* Interface Class */
    uint16_t            *p_tpl;         /* Target peripheral list (Vendor ID, Product ID) */
    usbh0_cb_check_t    classcheck;     /* Driver check */
    usbh0_cb_t          devconfig;      /* Device configured */
    usbh0_cb_t          devdetach;      /* Device detach */
    usbh0_cb_t          devsuspend;     /* Device suspend */
    usbh0_cb_t          devresume;      /* Device resume */
} st_usbh0_hcdreg_t;

/* Setup Structure */
typedef struct USBH0_SETUP
{
    uint16_t        type;           /* bRequest[b15-b8], bmRequestType[b7-b0] */
    uint16_t        value;          /* wValue */
    uint16_t        index;          /* wIndex */
    uint16_t        length;         /* wLength */
    uint16_t        devaddr;        /* Device Address */
} st_usbh0_setup_t;

/* Transfer Request Structure */
typedef struct usbh0_utr
{
    uint16_t            msginfo;        /* Message Info for F/W */
    uint16_t            keyword;        /* Rootport / Pipe number / Device Address */
    uint16_t            result;         /* Transfer Result */
    void                *p_tranadr;     /* Transfer data Start address */
    uint32_t            tranlen;        /* Transfer data length */
    st_usbh0_setup_t    *p_setup;       /* Setup packet(for control only) */
    usbh0_utr_cb_t      complete;       /* Call Back Function Info */
} st_usbh0_message_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/* USB API (Host) */
usbh0_er_t  R_USBH0_HstdTransferStart(st_usbh0_utr_t *utr);
usbh0_er_t  R_USBH0_HstdTransferEnd(uint16_t pipe_id);
usbh0_er_t  R_USBH0_HstdDriverRegistration(st_usbh0_hcdreg_t *callback);
void        R_USBH0_HstdReturnEnuMGR(uint16_t cls_result);
usbh0_er_t  R_USBH0_HstdChangeDeviceState(usbh0_utr_cb_t complete, uint16_t msginfo, uint16_t member);
usbh0_er_t  R_USBH0_HstdSetPipe(uint16_t **table);
uint16_t    R_USBH0_HstdGetPipeID(uint16_t devaddr, uint8_t type, uint8_t direction, uint8_t ifnum);
usbh0_er_t  R_USBH0_HstdClearPipe(uint16_t devaddr);
usbh0_er_t  R_USBH0_HstdMgrOpen(void);
void        R_USBH0_HstdMgrTask(void);
void        R_USBH0_HstdDelayXms(uint32_t time);
void        R_USBH0_HstdMgrClose(void);

/* USB API (Hub) */
void        R_USBH0_HhubTask(void);

/* NonOS Scheduler */
usbh0_er_t  R_USBH0_CstdRecMsg(uint8_t id, usbh0_msg_t** mess);
usbh0_er_t  R_USBH0_CstdSndMsg(uint8_t id, usbh0_msg_t* mess);
usbh0_er_t  R_USBH0_CstdWaiMsg(uint8_t id, usbh0_msg_t* mess, uint16_t tm);
usbh0_er_t  R_USBH0_CstdPgetBlk(uint8_t id, st_usbh0_utr_t** blk);
usbh0_er_t  R_USBH0_CstdRelBlk(uint8_t id, st_usbh0_utr_t* blk);
void        R_USBH0_CstdScheduler(void);
void        R_USBH0_CstdSetTaskPri(uint8_t tasknum, uint8_t pri);
uint8_t     R_USBH0_CstdCheckSchedule(void);
uint16_t    R_USBH0_HstdGetMaxPacketSize(uint16_t devadr);
uint16_t    R_USBH0_HstdGetDevAddr(uint16_t devadr);

void        R_USBH0_isr(uint32_t int_sense);
void        R_USBH0_Init(void);

#endif /* R_USBH0_BASIC_IF_H */
