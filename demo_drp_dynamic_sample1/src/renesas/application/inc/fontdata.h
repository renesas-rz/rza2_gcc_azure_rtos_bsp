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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**************************************************************************//**
 * File Name :   fontdata.h
 * @file         fontdata.h
 * @version      0.01
 * @brief        Defines for selecting the LCD panel type which is connected.
 ******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>

#ifndef R_BCD_FONTDATA_H
#define R_BCD_FONTDATA_H

/******************************************************************************
Macro definitions
******************************************************************************/
/*#define FONT_10x20 */
#define FONT_12x24
/*#define FONT_32x32 */

#if defined(FONT_10x20)
#define R_BCD_FONTDATA_WIDTH    (10)
#define R_BCD_FONTDATA_HEIGHT   (20)
#elif defined(FONT_12x24)
#define R_BCD_FONTDATA_WIDTH    (12)
#define R_BCD_FONTDATA_HEIGHT   (24)
#elif defined(FONT_32x32)
#define R_BCD_FONTDATA_WIDTH    (32)
#define R_BCD_FONTDATA_HEIGHT   (32)
#else
#define R_BCD_FONTDATA_WIDTH    (10)
#define R_BCD_FONTDATA_HEIGHT   (20)
#endif

/******************************************************************************
Variable Externs
******************************************************************************/
extern uint8_t font_array[][R_BCD_FONTDATA_HEIGHT][(R_BCD_FONTDATA_WIDTH/8)+1];

#endif /* R_BCD_FONTDATA_10X20_H */
