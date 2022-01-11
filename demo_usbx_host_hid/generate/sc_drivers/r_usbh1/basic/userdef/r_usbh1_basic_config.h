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
 * File Name    : r_usbh1_basic_config.h
 * Version      : 1.0
 * Description  : USB Host Ch0 User definition
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH1_BASIC_CONFIG_H
#define R_USBH1_BASIC_CONFIG_H

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#include "r_usbh1_basic_if.h"

/***********************************************************************************************************************
 Macro definitions
 **********************************************************************************************************************/
/* Select PIPE NUM */
#define USBH1_MAXPIPE                         (32u)

/* defined: Host COMPLIANCE mode, undefined: Host NORMAL mode */
//#define USBH1_HOST_COMPLIANCE_MODE

#ifdef USBH1_HOST_COMPLIANCE_MODE
#define USBH1_COMPLIANCE_DISP(data1, data2)   
#endif /* USBH1_HOST_COMPLIANCE_MODE */

#define USBH1_OVERCURRENT(rootport)           

/** [Device class setting]
 *  #define USBH1_CFG_HHID_USE : Host Human Interface Device Class
 *  #define USBH1_CFG_HMSC_USE : Host Mass Storage Class
 * */
#define USBH1_CFG_HMSC_USE 0
#define USBH1_CFG_HHID_USE 1

/** [Target board setting]
 *  #define BSP_CFG_BOARD_RZA2_EVB : Target board is RZ/A2M Evaluation Board
 *  #define BSP_CFG_BOARD_GR_MANGO : Target board is GR-MANGO
 * */
#define BSP_CFG_BOARD_RZA2_EVB 0
#define BSP_CFG_BOARD_GR_MANGO 1

/***********************************************************************************************************************
 Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/

#endif  /* R_USBH1_BASIC_CONFIG_H */
