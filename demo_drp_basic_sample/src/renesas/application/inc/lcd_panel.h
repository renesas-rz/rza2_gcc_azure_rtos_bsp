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
 * File Name :   lcd_panel.h
 * @file         lcd_panel.h
 * @version      0.01
 * @brief        Defines for selecting the LCD panel type which is connected.
 ******************************************************************************/
#ifndef LCD_PANEL_H
#define LCD_PANEL_H

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define     LCD_PANEL_LVDS       (1)
#define     LCD_PANEL_RSK        (2)
#define     LCD_PANEL_DVI        (3)
#define     LCD_PANEL            (LCD_PANEL_DVI)

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#if     (LCD_PANEL==1)
#include "lcd/lcd_lvds_clk.h"
#include "lcd/lcd_lvds_ch0.h"
#elif     (LCD_PANEL==2)
#include "lcd/lcd_rsk_clk.h"
#include "lcd/lcd_rsk_ch0.h"
#elif     (LCD_PANEL==3)
#include "lcd/lcd_dvi_clk.h"
#include "lcd/lcd_dvi_ch0.h"
#else
    #error  Undefined LCD panel!
#endif

#endif  /*  LCD_PANEL_H */
