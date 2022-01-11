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
 * File Name    : r_usbh1_hhci_local.h
 * Version      : 1.0
 * Description  : HCI Local Common Header
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#ifndef R_USBH1_HHCILOCAL_H
#define R_USBH1_HHCILOCAL_H

#include <stdio.h>
#include <string.h>
#include "iodefine.h"
#include "r_usbh1_hhci.h"
#include "r_usbh1_hehci_def_usr.h"
#include "r_usbh1_hehci_typedef.h"
#include "r_usbh1_hehci_extern.h"
#include "r_usbh1_hohci_def_usr.h"
#include "r_usbh1_hohci_typedef.h"
#include "r_usbh1_hohci_extern.h"

#include "r_mmu_lld_rza2m.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
//#define USBH1_DEBUG_HCI

#ifndef NULL
#define NULL                            (0u)
#endif
#ifndef FALSE
#define FALSE                           (0u)
#endif
#ifndef TRUE
#define TRUE                            (1u)
#endif

#define USBH1_HCI_NONCACHE_ADDR(x)      (x)
#define USBH1_HCI_CACHE_ADDR(x)         (x | 0x02000000)

#define USBH1_PID                       (0x0003u)   /* b1-0: Response PID */
#define USBH1_PID_STALL                 (0x0002u)   /* STALL */
#define USBH1_PID_BUF                   (0x0001u)   /* BUF */
#define USBH1_PID_NAK                   (0x0000u)   /* NAK */

//========================================================================
//                      Definitions for HCI Common
//========================================================================
/* Type of HCI task */
typedef enum
{
    USBH1_HCI_TASK,
    USBH1_HCI_TASK_MAX
} e_usbh1_hci_task_enum_t;

/* Type of HCI task message */
typedef enum
{
    USBH1_HCI_MESS_EHCI_USBINT,                         /* EHCI Interrupt on complte message */
    USBH1_HCI_MESS_EHCI_USBERRINT,                      /* EHCI Error Interrupt message */
    USBH1_HCI_MESS_EHCI_PORT_CHANGE_DETECT,             /* EHCI Port Change Detect message */
    USBH1_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER,            /* EHCI Frame List Rollover message */
    USBH1_HCI_MESS_EHCI_HOST_SYSTEM_ERROR,              /* EHCI Host System Error message */
    USBH1_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE,     /* EHCI Interrupt on Asnc Advance message */
    USBH1_HCI_MESS_OHCI_INT,                            /* OHCI Interrupt message */
    USBH1_HCI_MESS_OHCI_CONTROL_ROOTHUB,                /* OHCI Control Roothub message */
    USBH1_HCI_MESS_PORT_RESET_REQUEST,                  /* Port Reset Request message */
    USBH1_HCI_MESS_MAKE_TRANSFER_REQUEST,               /* Transfer Request message */
    USBH1_HCI_MESS_DISCONNECT_REQUEST,                  /* Disconnect Request message */
    USBH1_HCI_MESS_CANCEL_TRANSFER_REQUEST_S,           /* Cancel Transfer Request for Sytem task */
    USBH1_HCI_MESS_CANCEL_TRANSFER_REQUEST_T,           /* Cancel Transfer Request for Transfer task */
    USBH1_HCI_MESS_MAX
} e_usbh1_hci_mess_enum_t;

#ifdef USBH1_DEBUG_HCI
#define USBH1_HCI_PRINTF0(FORM)             printf("\033[36m"FORM"\033[0m")
#define USBH1_HCI_PRINTF1(FORM,x)           printf("\033[36m"FORM"\033[0m",x)
#define USBH1_HCI_PRINTF2(FORM,x,y)         printf("\033[36m"FORM"\033[0m",x,y)
#define USBH1_HCI_PRINTF3(FORM,x,y,z)       printf("\033[36m"FORM"\033[0m",x,y,z)
#define USBH1_HCI_PRINTF0_R(FORM)           printf("\033[31m"FORM"\033[0m")
#define USBH1_HCI_PRINTF1_R(FORM,x)         printf("\033[31m"FORM"\033[0m",x)
#define USBH1_HCI_PRINTF2_R(FORM,x,y)       printf("\033[31m"FORM"\033[0m",x,y)
#define USBH1_HCI_PRINTF3_R(FORM,x,y,z)     printf("\033[31m"FORM"\033[0m",x,y,z)
#define USBH1_HCI_PRINTF0_G(FORM)           printf("\033[32m"FORM"\033[0m")
#define USBH1_HCI_PRINTF1_G(FORM,x)         printf("\033[32m"FORM"\033[0m",x)
#define USBH1_HCI_PRINTF2_G(FORM,x,y)       printf("\033[32m"FORM"\033[0m",x,y)
#define USBH1_HCI_PRINTF3_G(FORM,x,y,z)     printf("\033[32m"FORM"\033[0m",x,y,z)
#define USBH1_HCI_PRINTF0_Y(FORM)           printf("\033[33m"FORM"\033[0m")
#define USBH1_HCI_PRINTF1_Y(FORM,x)         printf("\033[33m"FORM"\033[0m",x)
#define USBH1_HCI_PRINTF2_Y(FORM,x,y)       printf("\033[33m"FORM"\033[0m",x,y)
#define USBH1_HCI_PRINTF3_Y(FORM,x,y,z)     printf("\033[33m"FORM"\033[0m",x,y,z)
#else
#define USBH1_HCI_PRINTF0(FORM)
#define USBH1_HCI_PRINTF1(FORM,x)
#define USBH1_HCI_PRINTF2(FORM,x,y)
#define USBH1_HCI_PRINTF3(FORM,x,y,z)
#define USBH1_HCI_PRINTF0_R(FORM)
#define USBH1_HCI_PRINTF1_R(FORM,x)
#define USBH1_HCI_PRINTF2_R(FORM,x,y)
#define USBH1_HCI_PRINTF3_R(FORM,x,y,z)
#define USBH1_HCI_PRINTF0_G(FORM)
#define USBH1_HCI_PRINTF1_G(FORM,x)
#define USBH1_HCI_PRINTF2_G(FORM,x,y)
#define USBH1_HCI_PRINTF3_G(FORM,x,y,z)
#define USBH1_HCI_PRINTF0_Y(FORM)
#define USBH1_HCI_PRINTF1_Y(FORM,x)
#define USBH1_HCI_PRINTF2_Y(FORM,x,y)
#define USBH1_HCI_PRINTF3_Y(FORM,x,y,z)
#endif

/***************************************(*******)*************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
typedef struct
{
    uint8_t     blength;
    uint8_t     bdescriptor_type;
} st_usbh1_hci_descriptor_header_t;


/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern st_usbh1_hci_tr_req_t    g_usbh1_hstd_hci_transfer_request[USBH1_HCI_TRREQ_NUM];
extern st_usbh1_hci_cb_info_t   g_usbh1_hstd_hci_callback;

 /***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
void                    usbh1_hstd_hci_send_message_from_int(int32_t task_no, int8_t message, uint32_t data1, uint32_t data2, uint32_t data3);
st_usbh1_hci_device_t   *usbh1_hstd_hci_alloc_dev_info(void);
uint32_t                usbh1_hstd_hci_get_pollrate(st_usbh1_hci_tr_req_t *tr_req);
void                    usbh1_hstd_hci_lock_resouce(void);
void                    usbh1_hstd_hci_unlock_resouce(void);

#endif  /* R_USBH1_HHCILOCAL_H */
