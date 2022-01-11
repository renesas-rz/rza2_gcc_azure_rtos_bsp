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
 * File Name    : r_usbh1_hehci_typedef.h
 * Version      : 1.0
 * Description  : EHCI Type Define Header
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH1_HEHCITYPEDEF_H
#define R_USBH1_HEHCITYPEDEF_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
#define USBH1_EHCI_VERSON               (0x0110u)       /* HCIVERSION */

#define USBH1_EHCI_MAXROOTPORTS         (1u)            /* The maximum number of route ports */

#define USBH1_EHCI_LINE_STATE_SE0       (0u)            /* USB State : SE0 */
#define USBH1_EHCI_LINE_STATE_J         (2u)            /* USB State : J-state */
#define USBH1_EHCI_LINE_STATE_K         (1u)            /* USB State : K-state */

#define USBH1_EHCI_TYP_ITD              (0u)            /* Isochronous Transfer Descriptor */
#define USBH1_EHCI_TYP_QH               (1u)            /* Queue Head */
#define USBH1_EHCI_TYP_SITD             (2u)            /* Split Transaction Isochronous Transfer Descriptor */
#define USBH1_EHCI_TYP_FSTN             (3u)            /* Periodic Frame Span Traversal Node */

#define USBH1_EHCI_TP_ALL               (0u)            /* Transaction position : All */
#define USBH1_EHCI_TP_BEGIN             (1u)            /* Transaction position : Begin */
#define USBH1_EHCI_TP_MID               (2u)            /* Transaction position : Middle */
#define USBH1_EHCI_TP_END               (3u)            /* Transaction position : End */

#define USBH1_EHCI_PID_OUT              (0u)            /* PID : OUT */
#define USBH1_EHCI_PID_IN               (1u)            /* PID : IN */
#define USBH1_EHCI_PID_SETUP            (2u)            /* PID : SETUP */

#define USBH1_EHCI_MAXQTDSIZE           (20480u)        /* The maximum transfer size of QTD */

#define USBH1_EHCI_SITD_DATA_SIZE       (188u)          /* Maximum size that can be transed with one micro frame of SITD */

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/* Refer to EHCI Spec.3.1 */
typedef union usbh1_ehci_flep_tag
{
    struct
    {
        uint32_t            t : 1;
        uint32_t            typ : 2;
        uint32_t            : 2;
        uint32_t            pointer : 27;
    } bit;
    union usbh1_ehci_flep_tag *pointer;
    uint32_t                address;
} u_usbh1_ehci_flep_t;


/* Refer to EHCI Spec.3.5.3 */
typedef union usbh1_ehci_transfer_info_tag
{
    struct
    {
        uint32_t            status_ping_state : 1;
        uint32_t            status_split_transaction_state : 1;
        uint32_t            status_missed_micro_frame : 1;
        uint32_t            status_transaction_error : 1;
        uint32_t            status_babble_detected : 1;
        uint32_t            status_data_buffer_error : 1;
        uint32_t            status_halted : 1;
        uint32_t            status_active : 1;
        uint32_t            pid_code : 2;
        uint32_t            cerr : 2;
        uint32_t            c_page : 3;
        uint32_t            ioc : 1;
        uint32_t            total_bytes_to_transfer : 15;
        uint32_t            data_toggle : 1;
    } bit;
    uint32_t                dword;
} u_usbh1_ehci_transfer_info_t;


/* Queue Head */
typedef struct usbh1_ehci_qh_tag
{
    /* Refer to EHCI Spec.3.6.1 */
    u_usbh1_ehci_flep_t           queue_head_horizontal_link;

    /* Refer to EHCI Spec.3.6.2 */
    union endpoint1_tag
    {
        struct
        {
            uint32_t        device_address : 7;
            uint32_t        i : 1;
            uint32_t        endpt : 4;
            uint32_t        eps : 2;
            uint32_t        dtc : 1;
            uint32_t        h : 1;
            uint32_t        maximum_packet_length : 11;
            uint32_t        c : 1;
            uint32_t        rl : 4;
        } bit;
        uint32_t            dword;
    } endpoint1;

    /* Refer to EHCI Spec.3.6.2 */
    union endpoint2_tag
    {
        struct
        {
            uint32_t        mframe_smask : 8;
            uint32_t        mframe_cmask : 8;
            uint32_t        hub_addr : 7;
            uint32_t        port_number : 7;
            uint32_t        mult : 2;
        } bit;
        uint32_t            dword;
    } endpoint2;

    union
    {
        struct
        {
            uint32_t    : 5;
            uint32_t    pointer : 27;
        } bit;
        struct usbh1_ehci_qtd_tag   *pointer;
        uint32_t                    address;
    } current_qtd;

    union
    {
        struct
        {
            uint32_t    t : 1;
            uint32_t    : 4;
            uint32_t    pointer : 27;
        } bit;
        struct usbh1_ehci_qtd_tag   *pointer;
        uint32_t                    address;
    } next_qtd;

    union
    {
        struct
        {
            uint32_t    t : 1;
            uint32_t    nakcnt : 4;
            uint32_t    pointer : 27;
        } bit;
        struct usbh1_ehci_qtd_tag   *pointer;
        uint32_t                    address;
    } alternate_next_qtd;

    /* Refer to EHCI Spec.3.5.3 */
    u_usbh1_ehci_transfer_info_t    transfer_info;

    /* Refer to EHCI Spec.3.6.3 */
    union
    {
        struct
        {
            uint32_t    current_offset : 12;
            uint32_t    pointer : 20;
        } bit;
        struct
        {
            uint32_t    c_prog_mask : 8;
            uint32_t    : 4;
            uint32_t    pointer : 20;
        } bit1;
        struct
        {
            uint32_t    frame_tag : 5;
            uint32_t    s_bytes : 7;
            uint32_t    pointer : 20;
        } bit2;
        uint32_t        address;
    } buffer[5];

//  hcd-related
    uint32_t            reserve[1];
    struct
    {
        uint32_t        pollrate        : 14;   /* pollrate */
        uint32_t        direction       : 2;    /* direction */
        uint32_t        pollrate_offset : 14;   /* pollrate offset */
        uint32_t        tr_req_flag     : 1;    /* Transfer Request flag */
        uint32_t        enable          : 1;    /* QH enable flag */
    } info;
    struct usbh1_ehci_qtd_tag   *qtd_head;          /* QTD head pointer */
    struct usbh1_ehci_qtd_tag   *qtd_end;           /* QTD end pointer */
} st_usbh1_ehci_qh_t;


/* Queue Element Transfer Descriptor */
typedef struct usbh1_ehci_qtd_tag
{
    /* Refer to EHCI Spec.3.5.1 */
    union
    {
        struct
        {
            uint32_t    t : 1;
            uint32_t    : 4;
            uint32_t    pointer : 27;
        } bit;
        struct usbh1_ehci_qtd_tag   *pointer;
        uint32_t                    address;
    } next_qtd;

    /* Refer to EHCI Spec.3.5.2 */
    union {
        struct {
            uint32_t    t : 1;
            uint32_t    : 4;
            uint32_t    pointer : 27;
        } bit;
        struct usbh1_ehci_qtd_tag   *pointer;
        uint32_t                    address;
    } alternate_next_qtd;

    /* Refer to EHCI Spec.3.5.3 */
    u_usbh1_ehci_transfer_info_t    transfer_info;

    /* Refer to EHCI Spec.3.5.4 */
    union
    {
        struct
        {
            uint32_t    current_offset : 12;
            uint32_t    pointer : 20;
        } bit;
        uint32_t        address;
    } buffer[5];
} st_usbh1_ehci_qtd_t;


/* Isochronous Transfer Descriptor */
typedef struct  usbh1_ehci_itd_tag
{
    /* Refer to EHCI Spec.3.3.1 */
    u_usbh1_ehci_flep_t     next_link;

    /* Refer to EHCI Spec.3.3.2 */
    union
    {
        struct
        {
            uint32_t    offset : 12;
            uint32_t    pg : 3;
            uint32_t    ioc : 1;
            uint32_t    length : 12;
            uint32_t    status_transaction_error : 1;
            uint32_t    status_babble_detected : 1;
            uint32_t    status_data_buffer_error : 1;
            uint32_t    status_active : 1;
        } bit;
        uint32_t        dword;
    } transaction[8];

    /* Refer to EHCI Spec.3.3.3 */
    union
    {
        struct
        {
            uint32_t    device_address : 7;
            uint32_t    : 1;
            uint32_t    endpt : 4;
            uint32_t    pointer : 20;
        } bit0;
        struct
        {
            uint32_t    maximum_packet_length : 11;
            uint32_t    direction : 1;
            uint32_t    pointer : 20;
        } bit1;
        struct
        {
            uint32_t    multi : 2;
            uint32_t    : 10;
            uint32_t    pointer : 20;
        } bit2;
        uint32_t        address;
    } buffer[7];

//  hcd-related
    uint32_t        reserve[2];
    struct
    {
        uint32_t    pollrate        : 14;   /* pollrate */
        uint32_t    direction       : 2;    /* direction */
        uint32_t    pollrate_offset : 14;   /* pollrate offset */
        uint32_t    tr_req_flag     : 1;    /* Transfer Request flag */
        uint32_t    enable          : 1;    /* QH enable flag */
    } info;

    uint32_t        setup_cnt;              /* setup count */
    uint32_t        ini_setup_cnt;          /* initialize setup count */
    uint32_t        next_setup_uframe;      /* next setup micro frame no */
    uint32_t        next_active_uframe;     /* next active micro frame no */
    uint32_t        next_trend_uframe;      /* next transfer end micro frame no */

    uint8_t         tmp_buffer[8][USBH1_EHCI_ITD_DATA_SIZE];  /* transfe temporary buffer for ITD */
} st_usbh1_ehci_itd_t;


/* Split Transaction Isochronous Transfer Descriptor */
typedef struct usbh1_ehci_sitd_tag
{
    /* Refer to EHCI Spec.3.4.1 */
    u_usbh1_ehci_flep_t     next_link;

    /* Refer to EHCI Spec.3.4.2 */
    union
    {
        struct
        {
            uint32_t    device_address : 7;
            uint32_t    : 1;
            uint32_t    endpt : 4;
            uint32_t    : 4;
            uint32_t    hub_addr : 7;
            uint32_t    : 1;
            uint32_t    port_number : 7;
            uint32_t    direction : 1;
        } bit;
        uint32_t        dword;
    } endpoint1;

    /* Refer to EHCI Spec.3.4.2 */
    union
    {
        struct
        {
            uint32_t    mframe_smask : 8;
            uint32_t    mframe_cmask : 8;
            uint32_t    : 16;
        } bit;
        uint32_t        dword;
    } endpoint2;

    /* Refer to EHCI Spec.3.4.3 */
    union state_tag
    {
        struct
        {
            uint32_t    : 1;
            uint32_t    status_split_transaction_state : 1;
            uint32_t    status_missed_micro_frame : 1;
            uint32_t    status_transaction_error : 1;
            uint32_t    status_babble_detected : 1;
            uint32_t    status_data_buffer_error : 1;
            uint32_t    status_err : 1;
            uint32_t    status_active : 1;
            uint32_t    c_prog_mask : 8;
            uint32_t    total_bytes_to_transfer : 10;
            uint32_t    : 4;
            uint32_t    page_select : 1;
            uint32_t    ioc : 1;
        } bit;
        uint32_t        dword;
    } state;

    /* Refer to EHCI Spec.3.4.4 */
    union
    {
        struct
        {
            uint32_t    current_offset : 12;
            uint32_t    pointer : 20;
        } bit0;
        struct
        {
            uint32_t    t_count : 3;
            uint32_t    tp : 2;
            uint32_t    : 7;
            uint32_t    pointer : 20;
        } bit1;
        uint32_t        address;
    } buffer[2];

    /* Refer to EHCI Spec.3.4.5 */
    u_usbh1_ehci_flep_t     back;

//  hcd-related
    uint32_t                reserved[7];
    uint32_t                mps;                    /* Max Packet Size */
    struct
    {
        uint32_t    pollrate        : 14;   /* pollrate */
        uint32_t    direction       : 2;    /* direction */
        uint32_t    pollrate_offset : 14;   /* pollrate offset */
        uint32_t    tr_req_flag     : 1;    /* Transfer Request flag */
        uint32_t    enable          : 1;    /* QH enable flag */
    } info;
} st_usbh1_ehci_sitd_t;

#endif  /* R_USBH1_HEHCITYPEDEF_H */
