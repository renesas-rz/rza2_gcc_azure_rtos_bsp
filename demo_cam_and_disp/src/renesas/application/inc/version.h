/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*******************************************************************************
* File Name    : version.h
* Device(s)    : RZ/A2M
* Tool-Chain   : e2Studio Ver 7.8.0
*              : GNU ARM Embedded 6.3.1.20170620
* OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
* H/W Platform : GR-MANGO(mbed-RZ/A2M) X71A-M01-B
* Description  : The version release and build numbers.
*                build version
*                APPLICATION_INFO_VERSION.APPLICATION_INFO_RELEASE.APPLICATION_INFO_BUILD
******************************************************************************/
#ifndef VERSION_H_INCLUDED
#define VERSION_H_INCLUDED

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
 * The following definitions are not surrounded by braces as the utility
 * buildinc does not recognise numbers surrounded in braces
******************************************************************************/
#define APPLICATION_INFO_VERSION          (3)
#define APPLICATION_INFO_RELEASE          (1)
#define APPLICATION_INFO_BUILD            (0)


#endif                              /* VERSION_H_INCLUDED */

/******************************************************************************
End  Of File
******************************************************************************/
