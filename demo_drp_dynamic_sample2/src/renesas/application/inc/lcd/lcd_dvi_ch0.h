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
/****************************************************************************
* @file        : lcd_dvi_ch0.h
* Device(s)    : RZ/A2M
* Tool-Chain   : e2Studio Ver 7.8.0
*              : GNU ARM Embedded 6.3.1.20170620
* OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
* H/W Platform : GR-MANGO(mbed-RZ/A2M) X71A-M01-B
* @brief       : LCD panel for VDC channel 0 definition header
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "pc_monitor.h"
#include    "r_vdc.h"

#ifndef DVI_H
#define DVI_H
/******************************************************************************
Macro definitions
******************************************************************************/
#define     LCD_CH0_S_VSYNC         (0u)                  /* Vertical Pulse start position */
#define     LCD_CH0_W_VSYNC         (LCD_V_SYNC_WIDTH)    /* Vertical Pulse Width(VPW) */
#define     LCD_CH0_DISP_VS         (LCD_V_SYNC_WIDTH + LCD_V_BACK_PORCH) /* LCD display area vertical start position
                                                                            = Vertical Pulse Width(VPW)
                                                                            + Vertical Back Porch(VBP)     */
#define     LCD_CH0_DISP_VW         (LCD_V_VISIBLE_AREA)  /* Vertical Display Period(VDP) */

#define     LCD_CH0_S_HSYNC         (0u)                /* Horizontal Pulse start position */
#define     LCD_CH0_W_HSYNC         (LCD_H_SYNC_WIDTH)    /* Horizontal Pulse Width(HPW) */
#define     LCD_CH0_DISP_HS         (LCD_H_SYNC_WIDTH + LCD_H_BACK_PORCH) /* LCD display area horizontal start position
                                                                                = Horizontal Pulse Width(HPW)
                                                                                + Horizontal Back Porch(HBP) */
#define     LCD_CH0_DISP_HW         (LCD_H_VISIBLE_AREA)  /* Horizontal Display Period(HDP) */

#define     LCD_CH0_SIG_FV          (LCD_V_TOTAL - 1u)  /* Vertical Total Period(VTP) */
#define     LCD_CH0_SIG_FH          (LCD_H_TOTAL - 1u)  /* Horizontal Total Period(HTP)  */

#define     LCD_CH0_TCON_PIN_VSYNC  (VDC_LCD_TCON_PIN_1)  /* Select TCON of a Vsync signal (Vsync_TCON_select) */
#define     LCD_CH0_TCON_PIN_HSYNC  (VDC_LCD_TCON_PIN_0)  /* Select TCON of a Hsync signal (Hsync_TCON_select) */
#define     LCD_CH0_TCON_PIN_DE     (VDC_LCD_TCON_PIN_2)  /* Select TCON of a DE signal    (DE_TCON_select)    */

#define     LCD_CH0_TOCN_POL_VSYNC  (LCD_V_POLARITY)      /* Select polarity of a Vsync signal (Vsync_POL)     */
#define     LCD_CH0_TOCN_POL_HSYNC  (LCD_H_POLARITY)      /* Select polarity of a Hsync signal (Hsync_POL)     */
#define     LCD_CH0_TOCN_POL_DE     (VDC_SIG_POL_NOT_INVERTED)/* Select polarity of a Hsync signal (DE_POL)    */

#define     LCD_CH0_TCON_HALF       (LCD_H_TOTAL)         /* TCON reference timing, 1/2fH timing(HTP)          */
#define     LCD_CH0_TCON_OFFSET     (0u)                /* TCON reference timing, offset Hsync signal timing   */

#define     LCD_CH0_OUT_EDGE        (VDC_EDGE_FALLING) /* Output phase control of LCD_DATA signal(LCD_DATA_OUT_EDGE) */
#define     LCD_CH0_OUT_FORMAT      (VDC_LCD_OUTFORMAT_RGB888) /* LCD output format select (LCD_OUT_FORMAT)    */

#endif  /*  DVI_H */
