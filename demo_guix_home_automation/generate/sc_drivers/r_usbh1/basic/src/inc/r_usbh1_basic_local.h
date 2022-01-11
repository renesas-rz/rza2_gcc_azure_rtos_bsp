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
 * File Name    : r_usbh1_basic_local.h
 * Version      : 1.0
 * Description  : Header file USB basic
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef USBH1_BASIC_LOCAL_H
#define USBH1_BASIC_LOCAL_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usbh1_basic_if.h"
#include "r_usbh1_rtos_config.h"
#include "r_os_abstraction_typedef.h"
/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* USB Device address define */
#define USBH1_DEVICEADDR                (1u)    /* PORT0 USB Address (1 to 10) */

/* HUB Address */
#define USBH1_HUBDPADDR                 (USBH1_DEVICEADDR + 1u)

/* Descriptor size */
#define USBH1_DEVICESIZE                (20u)   /* Device Descriptor size */
#define USBH1_CONFIGSIZE                (256u)  /* Configuration Descriptor size */

/* HUB down port */
#define USBH1_HUBDOWNPORT               (4u)    /* HUB downport (MAX15) */

/* H/W function type */
#define USBH1_BIT0                      ((uint16_t)0x0001)
#define USBH1_BIT1                      ((uint16_t)0x0002)
#define USBH1_BIT2                      ((uint16_t)0x0004)
#define USBH1_BIT3                      ((uint16_t)0x0008)
#define USBH1_BIT4                      ((uint16_t)0x0010)
#define USBH1_BIT5                      ((uint16_t)0x0020)
#define USBH1_BIT6                      ((uint16_t)0x0040)
#define USBH1_BIT7                      ((uint16_t)0x0080)
#define USBH1_BIT8                      ((uint16_t)0x0100)
#define USBH1_BIT9                      ((uint16_t)0x0200)
#define USBH1_BIT10                     ((uint16_t)0x0400)
#define USBH1_BIT11                     ((uint16_t)0x0800)
#define USBH1_BIT12                     ((uint16_t)0x1000)
#define USBH1_BIT13                     ((uint16_t)0x2000)
#define USBH1_BIT14                     ((uint16_t)0x4000)
#define USBH1_BIT15                     ((uint16_t)0x8000)
#define USBH1_BITSET(x)                 ((uint16_t)((uint16_t)1 << (x)))

#define USBH1_SND_MSG(id, mess)         usbh1_hstd_snd_msg((uint8_t)id, (usbh1_msg_t*)mess)
#define USBH1_WAI_MSG(id, mess, tm)     usbh1_hstd_wai_msg((uint8_t)id, (usbh1_msg_t*)mess, (uint16_t)tm)
#define USBH1_RCV_MSG(id, mess)         usbh1_hstd_rec_msg((uint8_t)id, (usbh1_msg_t**)mess)
#define USBH1_PGET_BLK(id, blk)         usbh1_hstd_pget_blk((uint8_t)id, (st_usbh1_utr_t**)blk)
#define USBH1_REL_BLK(id, blk)          usbh1_hstd_rel_blk((uint8_t)id, (st_usbh1_utr_t*)blk)
#define USBH1_RTOS_SND_MSG(id, mess)    usbh1_hstd_rtos_snd_msg((uint8_t)id, (usbh1_msg_t*)mess)
#define USBH1_RTOS_RCV_MSG(id, mess)    usbh1_hstd_rtos_rec_msg((uint8_t)id, (usbh1_msg_t**)mess, 0)

/* USB Manager mode */
#define USBH1_PORTOFF                   (0u)        /* Disconnect(VBUSoff) */
#define USBH1_DETACHED                  (10u)       /* Disconnect(VBUSon) */
#define USBH1_ATTACHED                  (20u)       /* Disconnect(HUBdiconfigured) */
#define USBH1_POWERED                   (30u)       /* Start reset handshake */
#define USBH1_DEFAULT                   (40u)       /* Set device address */
#define USBH1_ADDRESS                   (50u)       /* Enumeration start */
#define USBH1_ENUMERATION               (60u)       /* Wait device enumeration */
#define USBH1_CONFIGURED                (70u)       /* Detach detected */
#define USBH1_SUSPENDED                 (80u)       /* Device suspended */
#define USBH1_DETACH_PROCESS            (101u)      /* Wait device detach */
#define USBH1_SUSPENDED_PROCESS         (102u)      /* Wait device suspend */
#define USBH1_RESUME_PROCESS            (103u)      /* Wait device resume */

/* USB Manager task message command */
#define USBH1_MSG_MGR_AORDETACH         (0x0121u)
#define USBH1_MSG_MGR_OVERCURRENT       (0x0122u)
#define USBH1_MSG_MGR_STATUSRESULT      (0x0123u)
#define USBH1_MSG_MGR_SUBMITRESULT      (0x0124u)

/* HCI Task */
#define USBH1_HCI_TSK                   (USBH1_TID_0)       /* Task ID */
#define USBH1_HCI_MBX                   (USBH1_HCI_TSK)     /* Mailbox ID */
#define USBH1_HCI_MPL                   (USBH1_HCI_TSK)     /* Memorypool ID */
#define USBH1_HCI_MSG                   (USBH1_HCI_TSK)     /* Message ID */

/* Host Manager Task */
#define USBH1_MGR_TSK                   (USBH1_TID_1)       /* Task ID */
#define USBH1_MGR_MBX                   (USBH1_MGR_TSK)     /* Mailbox ID */
#define USBH1_MGR_MPL                   (USBH1_MGR_TSK)     /* Memorypool ID */
#define USBH1_MGR_MSG                   (USBH1_MGR_TSK)     /* Message ID */

/* Application MailBox */
#define USBH1_INT                   (USBH1_TID_10)      /* Mailbox ID */

/* Select Device MAX */
#define USBH1_MAXDEVADDR                (12u)

/* Maximum Task ID +1 */
#define USBH1_IDMAX                     (11u)

/* Maximum Priority number +1 */
#define USBH1_PRIMAX                    (8u)

/* Maximum block */
#define USBH1_BLKMAX                    (20u)

/* Maximum priority table */
#define USBH1_TABLEMAX                  (USBH1_BLKMAX)

/* Output debugging message in a console of IDE. */
//#define USBH1_DEBUG_OUTPUT

#ifdef USBH1_DEBUG_OUTPUT
#include <stdlib.h>
#include <stdio.h>

#define USBH1_PRINTF0(FORM)                             (printf(FORM))
#define USBH1_PRINTF1(FORM,x1)                          (printf(FORM,x1))
#define USBH1_PRINTF2(FORM,x1,x2)                       (printf(FORM,x1,x2))
#define USBH1_PRINTF3(FORM,x1,x2,x3)                    (printf(FORM,x1,x2,x3))
#define USBH1_PRINTF4(FORM,x1,x2,x3,x4)                 (printf(FORM,x1,x2,x3,x4))
#define USBH1_PRINTF5(FORM,x1,x2,x3,x4,x5)              (printf(FORM,x1,x2,x3,x4,x5))
#define USBH1_PRINTF6(FORM,x1,x2,x3,x4,x5,x6)           (printf(FORM,x1,x2,x3,x4,x5,x6))
#define USBH1_PRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)        (printf(FORM,x1,x2,x3,x4,x5,x6,x7))
#define USBH1_PRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)     (printf(FORM,x1,x2,x3,x4,x5,x6,x7,x8))
#else
#define USBH1_PRINTF0(FORM)
#define USBH1_PRINTF1(FORM,x1)
#define USBH1_PRINTF2(FORM,x1,x2)
#define USBH1_PRINTF3(FORM,x1,x2,x3)
#define USBH1_PRINTF4(FORM,x1,x2,x3,x4)
#define USBH1_PRINTF5(FORM,x1,x2,x3,x4,x5)
#define USBH1_PRINTF6(FORM,x1,x2,x3,x4,x5,x6)
#define USBH1_PRINTF7(FORM,x1,x2,x3,x4,x5,x6,x7)
#define USBH1_PRINTF8(FORM,x1,x2,x3,x4,x5,x6,x7,x8)
#endif

/***************************************(*******)*************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
/* PIPE Information Structure */
typedef struct
{
    uint8_t         ifnum;          /* Interface Number */
    uint8_t         epnum;          /* Endpoint Number */
    uint8_t         direction;      /* Endpoint Direction */
    uint8_t         type;           /* Endpoint Type */
    uint16_t        mps;            /* Endpoint Max Packet Size */
    uint16_t        devaddr;        /* Device Address */
} st_usbh1_pipe_t;

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
/* r_usbh1_hManager.c */
extern st_usbh1_hcdreg_t    g_usbh1_hstd_device_drv[USBH1_MAXDEVADDR + 1u]; /* Device driver (registration) */
extern uint16_t             g_usbh1_hstd_mgr_mode[1u];                      /* Manager mode */
extern uint16_t             g_usbh1_hstd_device_addr;                       /* Device address */
extern uint16_t             g_usbh1_hstd_device_speed;                      /* Reset handshake result */
extern uint16_t             g_usbh1_hstd_device_num;                        /* Device driver number */
extern uint16_t             g_usbh1_hstd_check_enu_result;                  /* Enumeration result check */
extern uint16_t             g_usbh1_msg_check;

/* r_usbh1_hDriver.c */
extern st_usbh1_pipe_t      g_usbh1_hstd_pipe[USBH1_MAXPIPE+1u];            /* pipe information */

#if (BSP_CFG_RTOS_USED == 1)
/* RTOS semaphore */
extern uint32_t             g_usbh1_sem;
#endif

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/* r_usbh1_cScheduler.c */
usbh1_er_t  usbh1_hstd_rec_msg(uint8_t id, usbh1_msg_t **pp_mess);
usbh1_er_t  usbh1_hstd_snd_msg(uint8_t id, usbh1_msg_t *p_mess);
usbh1_er_t  usbh1_hstd_isnd_msg(uint8_t id, usbh1_msg_t *p_mess);
usbh1_er_t  usbh1_hstd_wai_msg(uint8_t id, usbh1_msg_t *p_mess, uint16_t times);
void        usbh1_hstd_wait_scheduler(void);
usbh1_er_t  usbh1_hstd_pget_blk(uint8_t id, st_usbh1_utr_t **pp_blk);
usbh1_er_t  usbh1_hstd_rel_blk(uint8_t id, st_usbh1_utr_t *p_blk);
void        usbh1_hstd_sche_init(void);

/* r_usbh1_hDriver.c */
uint16_t    usbh1_hstd_get_epnum(uint16_t pipe_id);
uint16_t    usbh1_hstd_get_dev_addr(uint16_t pipe_id);
uint16_t    usbh1_hstd_get_pipe_dir(uint16_t pipe_id);
void        usbh1_hstd_dummy_function(st_usbh1_utr_t *p_utr);
void        usbh1_hstd_set_rootport(uint16_t devaddr, uint16_t rootport);
uint16_t    usbh1_hstd_get_rootport(uint16_t devaddr);
void        usbh1_hstd_mgr_snd_mbx(uint16_t msginfo, uint16_t dat, uint16_t res);
usbh1_er_t  usbh1_hstd_transfer_end(uint16_t pipe_id);
usbh1_er_t  usbh1_hstd_transfer_start(st_usbh1_utr_t *p_utr);
void        usbh1_hstd_a_or_detach_cb(uint16_t rootport);
void        usbh1_hstd_port_reset_cb(uint16_t rootport);
void        usbh1_hstd_over_current_cb(uint16_t rootport );
void        usbh1_hstd_transfer_end_cb(void *p_utr, uint32_t actual_size, uint16_t status);
uint16_t     usbh1_hstd_get_max_packet_size(uint16_t pipe_id);

/* r_usbh1_hManager.c */
void        usbh1_hstd_enum_get_descriptor(uint16_t addr, uint16_t cnt_value);
uint16_t    usbh1_hstd_set_feature(uint16_t addr, uint16_t epnum, usbh1_utr_cb_t complete);
uint16_t    usbh1_hstd_clear_feature(uint16_t addr, uint16_t epnum, usbh1_utr_cb_t complete);
usbh1_er_t  usbh1_hstd_clear_stall(uint16_t pipe, usbh1_utr_cb_t complete);
uint16_t    usbh1_hstd_get_string_desc(uint16_t addr, uint16_t string, usbh1_utr_cb_t complete);
void        usbh1_hstd_mgr_task(void);

/* r_usbh1_hhubsys.c */
void        usbh1_hhub_registration(st_usbh1_hcdreg_t *p_callback);

#ifdef USBH1_HOST_COMPLIANCE_MODE
void        g_usbh1_hstd_test_packet_parameter_flag(uint16_t product_id, uint16_t port);
#endif /* USBH1_HOST_COMPLIANCE_MODE */

#endif /* USBH1_BASIC_LOCAL_H */
