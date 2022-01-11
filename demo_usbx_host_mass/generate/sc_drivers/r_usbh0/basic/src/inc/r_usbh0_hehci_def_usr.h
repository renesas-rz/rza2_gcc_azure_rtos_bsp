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
 * File Name    : r_usbh0_hEhciDefUsr.h
 * Version      : 1.0
 * Description  : EHCI User Define Header
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH0_HEHCIDEFUSR_H
#define R_USBH0_HEHCIDEFUSR_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* Periodic Frame List Size */
#define USBH0_EHCI_PFL_SIZE             (256u)
//#define USBH0_EHCI_PFL_SIZE             (512u)
//#define USBH0_EHCI_PFL_SIZE             (1024u)

/* The maximum number of QH memory resources */
#define USBH0_EHCI_NUM_QH               (USBH0_HCI_TRREQ_NUM + USBH0_HCI_DEVICE_NUM)

/* The maximum number of QTD memory resources */
#define USBH0_EHCI_NUM_QTD              (2048u)

/* The maximum number of ITD memory resources */
#define USBH0_EHCI_NUM_ITD              (4u)

/* The maximum number of SITD memory resources */
#define USBH0_EHCI_NUM_SITD             (4u)

/* Maximum size that can be transed with one micro frame of ITD */
#define USBH0_EHCI_ITD_DATA_SIZE        (1024u)

/* Time-out setting (msec) */
#define USBH0_EHCI_TIMEOUT              (3000u)

#endif  /* R_USBH0_HEHCIDEFUSR_H */
