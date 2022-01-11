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
* File Name    : r_usbh1_hOhciTypedef.c
* Description  : OHCI Type Define Header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

#ifndef R_USBH1_HOHCI_TYPEDEF_H
#define R_USBH1_HOHCI_TYPEDEF_H

//========================================================================
//                      Definitions
//========================================================================
#define USBH1_OHCI_SHAREDMEMORYTYPE             (USBH1_OHCI_SHAREDMEMORYTYPE_PCI)   /* Shared Memory Type */
#define USBH1_OHCI_SHAREDMEMORY_OFFSET          (0x00000000)                        /* Shared Memory Offset */

#define USBH1_OHCI_NUM_REQUEST                  (USBH1_HCI_TRREQ_NUM)   /* The maximum number of Transfer Request memory */

#define USBH1_OHCI_MAXDEVICE                    (USBH1_HCI_DEVICE_NUM)  /* The maximum number of OHCI Device */

#define USBH1_OHCI_MAXENDPOINT                  (16u)           /* The maximum number of OHCI Endpoint No. */

#define USBH1_OHCI_NUM_DEVICEDATA               (1u)            /* The maximum number of Dvice Data */

#define USBH1_OHCI_MAXROOTPORTS                 (1u)            /* The maximum number of OHCI Rootport */

#define USBH1_OHCI_MAXTDSIZE                    (0x00002000)    /* Max Transfer Descriptor Size */

#define USBH1_OHCI_MAXIMUM_OVERHEAD             (210u)          /* The maximum number of OHCI Isochronous Overhead */

#define USBH1_OHCI_ISO_MAX_FRAME                (8u)            /* The maximum number of OHCI Isochronous Frame Size */

#define USBH1_OHCI_SHAREDMEMORYTYPE_EXCLUSIVE   (0u)            /* Exclusive Shared Memory Type */
#define USBH1_OHCI_SHAREDMEMORYTYPE_COMMOM      (1u)            /* Common Shared Memory Type */
#define USBH1_OHCI_SHAREDMEMORYTYPE_PCI         (2u)            /* PCI Shared Memory Type */

/* TD->td_status */
#define USBH1_OHCI_TD_NOTFILLED                 (0u)            /* NOTFILLED */
#define USBH1_OHCI_TD_CANCELED                  (1u)            /* CANCELED */
#define USBH1_OHCI_TD_COMPLETED                 (2u)            /* COMPLETED */
#define USBH1_OHCI_TD_PENDING                   (3u)            /* PENDING */

/* TD ConditionCode */
#define USBH1_OHCI_CC_NOERROR                   (0x0)           /* NOERROR */
#define USBH1_OHCI_CC_CRC                       (0x1)           /* CRC */
#define USBH1_OHCI_CC_BITSTUFFING               (0x2)           /* BITSTUFFING */
#define USBH1_OHCI_CC_DATATOGGLEMISMATCH        (0x3)           /* DATATOGGLEMISMATCH */
#define USBH1_OHCI_CC_STALL                     (0x4)           /* STALL */
#define USBH1_OHCI_CC_DEVICENOTRESPOND          (0x5)           /* DEVICENOTRESPOND */
#define USBH1_OHCI_CC_PIDCHECKFAILURE           (0x6)           /* PIDCHECKFAILURE */
#define USBH1_OHCI_CC_UNEXPECTEDPID             (0x7)           /* UNEXPECTEDPID */
#define USBH1_OHCI_CC_DATAOVERRUN               (0x8)           /* DATAOVERRUN */
#define USBH1_OHCI_CC_DATAUNDERRUN              (0x9)           /* DATAUNDERRUN */
#define USBH1_OHCI_CC_BUFFEROVERRUN             (0xC)           /* BUFFEROVERRUN */
#define USBH1_OHCI_CC_BUFFERUNDERRUN            (0xD)           /* BUFFERUNDERRUN */
#define USBH1_OHCI_CC_NOTACCESSED               (0xF)           /* NOTACCESSED */

/* Request->Status */
#define USBH1_OHCI_RS_NORMAL_COMPLETION         (0x00)          /* NORMAL COMPLETION */
#define USBH1_OHCI_RS_NOT_DONE                  (0x10)          /* NOT DONE */
#define USBH1_OHCI_RS_NOT_REQUESTED             (0x11)          /* NOT REQUESTED */
#define USBH1_OHCI_RS_CANCELING                 (0x12)          /* CANCELING */
#define USBH1_OHCI_RS_CANCELED                  (0x13)          /* CANCELED */

/* HC_Transfer_Control.DP */
#define USBH1_OHCI_PID_SETUP                    (0u)            /* SETUP */
#define USBH1_OHCI_PID_OUT                      (1u)            /* OUT */
#define USBH1_OHCI_PID_IN                       (2u)            /* IN */

/* Regs */
#define USBH1_OHCI_CONTROLLISTFILLED            (0x00000002)    /* Control */
#define USBH1_OHCI_BULKLISTFILLED               (0x00000004)    /* Bulk */

/* For usbh1_hstd_ohci_scan_containing_record() */
#define USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR       (0u)            /* ENDPOINT DESCRIPTOR */
#define USBH1_OHCI_CR_TRANSFER_DESCRIPTOR       (1u)            /* TRANSFER DESCRIPTOR */

#define USBH1_OHCI_CR_LINK                      (0u)            /* LINK */
#define USBH1_OHCI_CR_REQUESTLIST               (1u)            /* REQUEST LIST */
#define USBH1_OHCI_CR_PAUSEDLINK                (2u)            /* PAUSED LINK */

/* Others */
#define USBH1_OHCI_HC_INT_SOF                   (0x00000004)    /* SOF */

/* InterruptStatus */
#define USBH1_OHCI_IS_SCHEDULINGOVERRUN         (0x00000001)    /* SCHEDULINGOVERRUN */
#define USBH1_OHCI_IS_WRITEBACKDONEHEAD         (0x00000002)    /* WRITEBACKDONEHEAD */
#define USBH1_OHCI_IS_STARTOFFRAME              (0x00000004)    /* STARTOFFRAME */
#define USBH1_OHCI_IS_RESUMEDETECTED            (0x00000008)    /* RESUMEDETECTED */
#define USBH1_OHCI_IS_UNRECOVERABLERROR         (0x00000010)    /* UNRECOVERABLERROR */
#define USBH1_OHCI_IS_FRAMENUMBEROVERRUN        (0x00000020)    /* FRAMENUMBEROVERRUN */
#define USBH1_OHCI_IS_ROOTHUBSTATUSCHANGE       (0x00000040)    /* ROOTHUBSTATUSCHANGE */
#define USBH1_OHCI_IS_OWNERSHIPCHANGE           (0x40000000)    /* OWNERSHIPCHANGE */
#define USBH1_OHCI_IS_MASTERINTENABLE           (0x80000000)    /* MASTERINTENABLE */

#define USBH1_OHCI_ISOCHRONOUSENABLE            (0x00000008)    /* ISOCHRONOUS ENABLE */
#define USBH1_OHCI_BULKLISTENABLE               (0x00000020)    /* BULK LIST ENABLE */
#define USBH1_OHCI_CONTROLLISTENABLE            (0x00000010)    /* CONTROL LIST ENABLE */
#define USBH1_OHCI_INTERRUPTLISTENABLE          (0x00000004)    /* PERIODIC LIST ENABLE */
#define USBH1_OHCI_PERIODICLISTENABLE           (0x0000000C)    /* PERIODIC+ISO LIST ENABLE */

/* The different ED lists are as follows. */
#define USBH1_OHCI_ED_INTERRUPT_1ms             (0u)            /* For Interrupt(Polling rate = 1ms) */
#define USBH1_OHCI_ED_INTERRUPT_2ms             (1u)            /* For Interrupt(Polling rate = 2ms) */
#define USBH1_OHCI_ED_INTERRUPT_4ms             (3u)            /* For Interrupt(Polling rate = 4ms) */
#define USBH1_OHCI_ED_INTERRUPT_8ms             (7u)            /* For Interrupt(Polling rate = 8ms) */
#define USBH1_OHCI_ED_INTERRUPT_16ms            (15u)           /* For Interrupt(Polling rate = 16ms) */
#define USBH1_OHCI_ED_INTERRUPT_32ms            (31u)           /* For Interrupt(Polling rate = 32ms) */
#define USBH1_OHCI_ED_CONTROL                   (63u)           /* For Control */
#define USBH1_OHCI_ED_BULK                      (64u)           /* For Bulk */
#define USBH1_OHCI_ED_ISOCHRONOUS               (0u)            /* For Isochronous( same as 1ms interrupt queue ) */
#define USBH1_OHCI_NO_ED_LISTS                  (65u)           /* No ED Lists */
#define USBH1_OHCI_ED_EOF                       (0xff)          /* ED EOF */

#define USBH1_OHCI_HCED_HEADP_HALT              (0x00000001)    /* hardware stopped bit */
#define USBH1_OHCI_HCED_HEADP_CARRY             (0x00000002)    /* hardware toggle carry bit */


/************************************************************************/
/*                      Typedefs for OHCI                               */
/************************************************************************/
#define boolean_t       uint8_t

/************************************/
/*  5.2.2 Miscellaneous Definitions */
/************************************/
/* Doubly linked list */
typedef struct usbh1_ohci_list_entry
{
    struct usbh1_ohci_list_entry   *f_link;
    struct usbh1_ohci_list_entry   *b_link;
} st_usbh1_ohci_list_entry_t, *st_usbh1_ohci_list_entry_p_t;

/****************************************************/
/*  5.2.3 Host Controller Descriptors Definitions   */
/****************************************************/
typedef struct  usbh1_ohci_hc_endpoint_control
{
    uint32_t        fa:7;       /*  Function Address    7bit    0-127   */
    uint32_t        en:4;       /*  EndPoint No.        4bit    0-15    */
    uint32_t        d:2;        /*  Direction           2bit    D=0,3:get direction from TD ,   */
                                /*                              D=1:OUT , D=2:IN                */
    uint32_t        s:1;        /*  SPEED               1bit    S=0:full speed,S=1:low speed    */
    uint32_t        k:1;        /*  SKIP                1bit    K=1:go to the next ED           */
    uint32_t        f:1;        /*  Format              1bit    F=1:isochronous,F=0:others      */
    uint32_t        mps:16;     /*  MaxPacketSize       11bit   0-2047  */
} st_usbh1_ohci_hc_endpoint_control_t, *st_usbh1_ohci_hc_endpoint_control_p_t;

typedef struct  usbh1_ohci_hc_transfer_control
{
    uint32_t        dummy1:16;  /*  dummy   */
    uint32_t        dummy:2;    /*  dummy   */
    uint32_t        r:1;        /*  Buffer Rounding  0:must exactly define buffer               */
                                /*                   1: smaller packet than buffer not error    */
    uint32_t        dp:2;       /*  Direction PID  00:SETUP,    */
                                /*                 01:OUT ,     */
                                /*                 10:IN  ,     */
                                /*                 11:reserved  */
    uint32_t        int_d:3;     /*  Delay Interrupt    111:no interrupt */
    uint32_t        toggle:2;   /*  Data toggle, T= 0x:acquied from ED toggle carry, */
                                /*                  10:DATA0,   */
                                /*                  11:DATA1    */
    uint32_t        ec:2;       /*  Error count */
    uint32_t        cc:4;       /*  condition code  */
} st_usbh1_ohci_hc_transfer_control_t, *st_usbh1_ohci_hc_transfer_control_p_t;

/* Host Controller Endpoint Descriptor, refer to Section 4.2, Endpoint Descriptor */
typedef struct usbh1_ohci_hc_endpoint_descriptor
{
    st_usbh1_ohci_hc_endpoint_control_t control;    /* dword 0 */
    uint32_t                            tail_p;     /* physical pointer to USBH1_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t                            head_p;     /* flags + phys ptr to USBH1_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t                            next_ed;    /* phys ptr to USBH1_OHCI_HC_ENDPOINT_DESCRIPTOR */
} st_usbh1_ohci_hc_endpoint_descriptor_t, *st_usbh1_ohci_hc_endpoint_descriptor_p_t;

/* Host Controller Transfer Descriptor, refer to Section 4.3, Transfer Descriptors */
typedef struct usbh1_ohci_hc_transfer_descriptor
{
    struct usbh1_ohci_hc_transfer_control   control;    /* dword 0 */
    uint32_t                                cbp;        /* current buffer pointer */
    uint32_t                                next_td;    /* phys ptr to USBH1_OHCI_HC_TRANSFER_DESCRIPTOR */
    uint32_t                                be;         /* buffer end */
} st_usbh1_ohci_hc_transfer_descriptor_t, *st_usbh1_ohci_hc_transfer_descriptor_p_t;

/********************************************************/
/*  5.2.6 Host Controller Driver Internal Definitions   */
/********************************************************/
/* OHCI USBD Transfer Request */
typedef struct usbh1_ohci_request
{
    uint8_t                     *buffer;            /* Pointer to data to be transferred */
    uint32_t                    buffer_length;      /* Length of data buffer in bytes */
    uint8_t                     xferinfo;           /* Direction (In/Out) for control and bulk */
    uint8_t                     max_int_delay;      /* Maximum allowable delay from completion to completion notification to USBD */
    uint8_t                     shortxferok;        /* Transfer of less than BufferLength is to be treated as an error unless this is TRUE */
    uint8_t                     using_flag;         /* Using Flag */
    uint8_t                     *setup;             /* Data for setup packet (control endpoints only) */
    uint32_t                    status;             /* Completion status from HCD to USBD */
    st_usbh1_ohci_list_entry_t  hcd_list;           /* List of all HCD_TRANSFER_DESCRIPTORs in use for this request */
    uint32_t                    *driver_request;    /* Driver Request Infomation pointer */
} st_usbh1_ohci_request_t, *st_usbh1_ohci_request_p_t;

/* Each Host Controller Endpoint Descriptor is also doubly linked into a list tracked by HCD. */
/* Each ED queue is managed via an USBH1_OHCI_HCD_ED_LIST */
typedef struct usbh1_ohci_hcd_ed_list
{
    st_usbh1_ohci_list_entry_t  head;               /* Head of an HCD_ENDPOINT_DESCRIPTOR list */
    uint32_t                    *physical_head;     /* Address of location to put the physical head pointer when it changes */
    uint16_t                    bandwidth;          /* Allocated bandwidth on this timeslice. */
    uint8_t                     next;               /* Index to the next HCD_ED_LIST for this timeslice */
    uint8_t                     reserved;
} st_usbh1_ohci_hcd_ed_list_t, *st_usbh1_ohci_hcd_ed_list_p_t;


/* Refer to Section 4.4 Host Controller Communications Area */
typedef struct usbh1_ohci_hcca_block
{
    uint32_t        hcca_interrupt_list[32];
    uint32_t        hcca_frame_number:16;
    uint32_t        hcca_pad1:16;
    uint32_t        hcca_done_head;
    uint32_t        reserve[30];
} st_usbh1_ohci_hcca_block_t, *st_usbh1_ohci_hcca_block_p_t;

/* HCD Device Data (Refer to Section 5.2.6 Host Controller Driver Internal Definitions) */
typedef struct usbh1_ohci_hcd_device_data
{

    st_usbh1_ohci_hcca_block_p_t    hcca;
    st_usbh1_ohci_list_entry_t      endpoints;
    st_usbh1_ohci_list_entry_t      freeed;
    st_usbh1_ohci_list_entry_t      freetd;
    st_usbh1_ohci_list_entry_t      stalled_ed_reclamation;
    st_usbh1_ohci_list_entry_t      running_ed_reclamation;
    st_usbh1_ohci_list_entry_t      paused_ed_restart;
    st_usbh1_ohci_hcd_ed_list_t     ed_list[USBH1_OHCI_NO_ED_LISTS];
    uint32_t                        frame_high_part;
    uint32_t                        available_bandwidth;
    uint32_t                        max_bandwidth_inuse;
    uint32_t                        socount;
    uint32_t                        sostallframe;
    uint32_t                        solimitframe;
    uint8_t                         solimithit;
    uint8_t                         sostallhit;
    uint8_t                         root_device[USBH1_OHCI_MAXROOTPORTS];
    uint8_t                         using_flag;
} st_usbh1_ohci_hcd_device_data_t, *st_usbh1_ohci_hcd_device_data_p_t;

/************************************/
/*  5.2.5 Host Controller Endpoints */
/************************************/
/* Ext. struct */
typedef struct usbh1_ohci_iso_transfer_info
{
    uint16_t        size;               /* Transfer Size */
    uint8_t         control_cc;         /* Control CC */
    uint8_t         psw_cc;             /* PSW CC */
    uint8_t         *buffer;            /* Data Buffer pointer */
} st_usbh1_ohci_iso_transfer_info_t, *st_usbh1_ohci_iso_transfer_info_p_t;

/* Ext. struct */
typedef struct usbh1_ohci_iso_info
{
    uint8_t                             using_flag;         /* Using Flag */
    uint8_t                             copy_cnt;           /* Data copy count (for out) */
    uint8_t                             start_cnt;          /* Transfer start count (for in/out) */
    uint8_t                             end_cnt;            /* Transfer end count (for in/out) */
    uint8_t                             copy_p;             /* Data copy infomaition pointer (for out) */
    uint8_t                             start_p;            /* Transfer start infomaition pointer (for in/out) */
    uint8_t                             end_p;              /* Transfer end infomaition pointer (for in/out) */
    uint8_t                             complete_p;         /* Transfer complete information pointer (for in/out) */
    st_usbh1_ohci_request_p_t           usb_drequest;     /* USBD Transfer Request pointer */
    st_usbh1_ohci_iso_transfer_info_t   transfer_info[USBH1_OHCI_ISO_MAX_FRAME];    /* Transfer Infomation */
} st_usbh1_ohci_iso_info_t, *st_usbh1_ohci_iso_info_p_t;

typedef struct usbh1_ohci_hcd_endpoint
{
    uint8_t                                     type;
    uint8_t                                     list_index;
    uint8_t                                     using_flag;
    uint8_t                                     reserved[1];
    st_usbh1_ohci_hcd_device_data_p_t           device_data;
    st_usbh1_ohci_hc_endpoint_control_t         control;
    struct usbh1_ohci_hcd_endpoint_descriptor   *hcd_ed;
    struct usbh1_ohci_hcd_transfer_descriptor   *hcd_head_p;
    struct usbh1_ohci_hcd_transfer_descriptor   *hcd_tail_p;
    uint32_t                                    rate;
    uint32_t                                    bandwidth;
    uint32_t                                    max_packet;
    st_usbh1_ohci_list_entry_t                  link;           /* Ext. */
    st_usbh1_ohci_iso_info_p_t                  iso_info;       /* Ext. */
} st_usbh1_ohci_hcd_endpoint_t, *st_usbh1_ohci_hcd_endpoint_p_t;

/********************************************************/
/*  5.2.4 Host Controller Driver Descriptor Definitions */
/********************************************************/
/* HCD Endpoint Descriptor */
typedef struct usbh1_ohci_hcd_endpoint_descriptor
{
    uint8_t                                 list_index;
    uint8_t                                 paused_flag;
    uint8_t                                 using_flag;
    uint8_t                                 reserved[1];
    uint32_t                                physical_address;
    st_usbh1_ohci_list_entry_t              link;
    st_usbh1_ohci_hcd_endpoint_p_t          endpoint;
    uint32_t                                reclamation_frame;
    st_usbh1_ohci_list_entry_t              paused_link;
    st_usbh1_ohci_hc_endpoint_descriptor_t  hc_ed;
} st_usbh1_ohci_hcd_endpoint_descriptor_t, *st_usbh1_ohci_hcd_endpoint_descriptor_p_t;

/* HCD Transfer Descriptor */
typedef struct usbh1_ohci_hcd_transfer_descriptor
{
    uint8_t                                     td_status;
    uint8_t                                     using_flag:4;
    uint8_t                                     cancel_pending:4;
    uint32_t                                    physical_address;
    struct usbh1_ohci_hcd_transfer_descriptor   *next_hcd_td;
    st_usbh1_ohci_list_entry_t                  request_list;
    struct usbh1_ohci_request                   *usb_drequest;
    st_usbh1_ohci_hcd_endpoint_p_t              endpoint;
    uint32_t                                    transfer_count;
    st_usbh1_ohci_hc_transfer_descriptor_t      hc_td;
    uint32_t                                    dummy[4];
} st_usbh1_ohci_hcd_transfer_descriptor_t, *st_usbh1_ohci_hcd_transfer_descriptor_p_t;

/* HCD Transfer Descriptor for Isochronous */
typedef struct usbh1_ohci_hc_iso_transfer_control
{
    uint32_t    sf:16;      /*  StartingFrame   */
    uint32_t    dummy2:5;
    uint32_t    int_d:3;    /*  Delay Interrupt    111:no interrupt */
    uint32_t    fc:3;       /*  Frame count */
    uint32_t    dummy:1;
    uint32_t    cc:4;       /*  condition code  */
} st_usbh1_ohci_hc_iso_transfer_control_t, *st_usbh1_ohci_hc_iso_transfer_control_p_t;

typedef union usbh1_ohci_offset_psw
{
    struct
    {
        uint16_t    size:11;
        uint16_t    dummy:1;
        uint16_t    cc:4;
    } psw;
    struct
    {
        uint16_t    offset:13;
        uint16_t    cc:3;
    } offset;
} u_usbh1_ohci_offset_psw_t, *u_usbh1_ohci_offset_psw_p_t;

typedef struct usbh1_ohci_hc_iso_transfer_descriptor
{
    struct usbh1_ohci_hc_iso_transfer_control   control;
    uint32_t                                    bp0;
    uint32_t                                    next_td;
    uint32_t                                    be;
    u_usbh1_ohci_offset_psw_t                   usb_ohci_offset_psw[8];
} st_usbh1_ohci_hc_iso_transfer_descriptor_t, *st_usbh1_ohci_hc_iso_transfer_descriptor_p_t;

#endif  /* R_USBH1_HOHCI_TYPEDEF_H */
