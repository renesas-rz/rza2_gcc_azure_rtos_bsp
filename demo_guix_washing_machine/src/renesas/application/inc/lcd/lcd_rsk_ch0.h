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
* @file         lcd_rsk_ch0.h
* @version      0.01
* @brief        LCD panel for VDC channel 0 definition header
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"

#ifndef RSK_TFT_H
#define RSK_TFT_H
/******************************************************************************
Macro definitions
******************************************************************************/
/*  LCD WVGA signal 800x480
    Pixel clock frequency: 33.0 MHz
    Refresh rate: 59.523 Hz
    Polarity of horizontal sync pulse: Negative
    Polarity of vertical sync pulse: Negative
*/
#define     LCD_CH0_S_VSYNC         (0u)                     /* Vertical Pulse start position                       */
#define     LCD_CH0_W_VSYNC         (10u)                    /* Vertical Pulse Width(VPW)                           */
#define     LCD_CH0_DISP_VS         (10u + 13u)              /* LCD display area vertical start position
                                                                 = Vertical Pulse Width(VPW)
                                                                 + Vertical Back Porch(VBP)     */
#define     LCD_CH0_DISP_VW         (480u)                   /* Vertical Display Period(VDP)                        */

#define     LCD_CH0_S_HSYNC         (0u)                     /* Horizontal Pulse start position                     */
#define     LCD_CH0_W_HSYNC         (20u)                    /* Horizontal Pulse Width(HPW)                         */
#define     LCD_CH0_DISP_HS         (20u + 26u)              /* LCD display area horizontal start position
                                                                 = Horizontal Pulse Width(HPW)
                                                                 + Horizontal Back Porch(HBP) */
#define     LCD_CH0_DISP_HW         (800u)                   /* Horizontal Display Period(HDP)                      */

#define     LCD_CH0_SIG_FV          (525u - 1u)              /* Vertical Total Period(VTP)                          */
#define     LCD_CH0_SIG_FH          (1056u - 1u)             /* Horizontal Total Period(HTP)                        */

#define     LCD_CH0_TCON_PIN_VSYNC  (VDC_LCD_TCON_PIN_0)       /* Select TCON of a Vsync signal (Vsync_TCON_select) */
#define     LCD_CH0_TCON_PIN_HSYNC  (VDC_LCD_TCON_PIN_3)       /* Select TCON of a Hsync signal (Hsync_TCON_select) */
#define     LCD_CH0_TCON_PIN_DE     (VDC_LCD_TCON_PIN_4)       /* Select TCON of a DE signal    (DE_TCON_select)    */

#define     LCD_CH0_TOCN_POL_VSYNC  (VDC_SIG_POL_INVERTED)     /* Select polarity of a Vsync signal (Vsync_POL)     */
#define     LCD_CH0_TOCN_POL_HSYNC  (VDC_SIG_POL_INVERTED)     /* Select polarity of a Hsync signal (Hsync_POL)     */
#define     LCD_CH0_TOCN_POL_DE     (VDC_SIG_POL_NOT_INVERTED) /* Select polarity of a Hsync signal (DE_POL)        */

#define     LCD_CH0_TCON_HALF       (1056u)                  /* TCON reference timing, 1/2fH timing(HTP)            */
#define     LCD_CH0_TCON_OFFSET     (0u)                     /* TCON reference timing, offset Hsync signal timing   */

#define     LCD_CH0_OUT_EDGE        (VDC_EDGE_RISING) /* Output phase control of LCD_DATA signal(LCD_DATA_OUT_EDGE) */
#define     LCD_CH0_OUT_FORMAT      (VDC_LCD_OUTFORMAT_RGB888) /* LCD output format select (LCD_OUT_FORMAT)         */

#endif  /*  RSK_TFT_H */
