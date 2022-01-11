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
 * File Name    : r_usbh1_hohci_def_usr.h
 * Version      : 1.0
 * Description  : OHCI User Define Header
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH1_HOHCIDEFUSR_H
#define R_USBH1_HOHCIDEFUSR_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* The maximum number of Endpoint memory */
#define USBH1_OHCI_NUM_ENDPOINT         (USBH1_HCI_TRREQ_NUM + USBH1_HCI_DEVICE_NUM)

/* The maximum number of Endpoint Descriptor memory */
#define	USBH1_OHCI_NUM_ED               ((USBH1_OHCI_ED_INTERRUPT_32ms * USBH1_OHCI_NUM_DEVICEDATA) + USBH1_OHCI_NUM_ENDPOINT)

/* The maximum number of Transfer Descriptor memory */
#define	USBH1_OHCI_NUM_TD               (0x100000 / USBH1_OHCI_MAXTDSIZE)

/* The maximum number of OHCI Isochronous Device */
#define USBH1_OHCI_ISO_MAXDEVICE        (4u)

/* The maximum number of OHCI Isochronous Packet Size */
#define USBH1_OHCI_ISO_MAX_PACKET_SIZE  (1023u)

/* Time-out setting (msec) */
#define USBH1_OHCI_TIMEOUT              (3000u)

#endif  /* R_USBH1_HOHCIDEFUSR_H */
