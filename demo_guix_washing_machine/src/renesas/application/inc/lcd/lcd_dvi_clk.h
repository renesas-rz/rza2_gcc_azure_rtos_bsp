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
* @file         lcd_dvi_clk.h
* @version      0.01
* @brief        Defines for VDC clock.
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>

#include    "r_typedefs.h"

#include    "pc_monitor.h"
#include    "r_vdc.h"

#ifndef DVI_CLK_H
#define DVI_CLK_H
/******************************************************************************
Macro definitions
******************************************************************************/
#if ( SELECT_MONITOR == SVGA_800_600 )
/* Clock setting
        DCDR  NFD   NRD  NODIV
        -     --    -    -      (40.000000)  40.0  MHz, SVGA Signal  800  x 600 (60 Hz)
*/
#define LCD_CH0_PANEL_CLK         (VDC_PANEL_ICKSEL_EXT_0)
#define LVDS_PLL_INPUT_CLK        (VDC_LVDS_INCLK_SEL_EXT_0)
#define LCD_CH0_PANEL_CLK_DIV     (VDC_PANEL_CLKDIV_1_1) /* DCDR  Not use */
#define LVDS_PLL_NFD              (29u-1u)               /* NFD   Not use */
#define LVDS_PLL_NRD              (4u-1u)                /* NRD   Not use */
#define LVDS_PLL_NODIV            (VDC_LVDS_NDIV_4)      /* ODIV  Not use */

#define LVDS_PLL_NIDV             (VDC_LVDS_NDIV_4)      /* Not use */
#define LVDS_PLL_NOD              (VDC_LVDS_PLL_NOD_4)   /* Not use */

#elif ( SELECT_MONITOR == XGA_1024_768 )
/* Clock setting
        DCDR  NFD   NRD  NODIV
        1     27    6    4      (74.250000)  75.00 MHz, XGA Signal  1024 x 768 (70 Hz)
*/
#define LCD_CH0_PANEL_CLK         (VDC_PANEL_ICKSEL_LVDS)
#define LVDS_PLL_INPUT_CLK        (VDC_LVDS_INCLK_SEL_PERI)
#define LCD_CH0_PANEL_CLK_DIV     (VDC_PANEL_CLKDIV_1_1)
#define LVDS_PLL_NFD              (27u-1u)
#define LVDS_PLL_NRD              (6u-1u)
#define LVDS_PLL_NODIV            (VDC_LVDS_NDIV_4)

#define LVDS_PLL_NIDV             (VDC_LVDS_NDIV_4)      /* Not use */
#define LVDS_PLL_NOD              (VDC_LVDS_PLL_NOD_4)   /* Not use */

#elif ( SELECT_MONITOR == FWXGA_1360_768 )
/* Clock setting
        DCDR  NFD   NRD  NODIV
        1     31    6    4      (85.250000)  85.50 MHz, WXGA Signal  1360 x 768 (60 Hz)
*/
#define LCD_CH0_PANEL_CLK         (VDC_PANEL_ICKSEL_LVDS)
#define LVDS_PLL_INPUT_CLK        (VDC_LVDS_INCLK_SEL_PERI)
#define LCD_CH0_PANEL_CLK_DIV     (VDC_PANEL_CLKDIV_1_1)
#define LVDS_PLL_NFD              (31u-1u)
#define LVDS_PLL_NRD              (6u-1u)
#define LVDS_PLL_NODIV            (VDC_LVDS_NDIV_4)

#define LVDS_PLL_NIDV             (VDC_LVDS_NDIV_4)      /* Not use */
#define LVDS_PLL_NOD              (VDC_LVDS_PLL_NOD_4)   /* Not use */

#else
    #error  Undefined LCD panel!
#endif

#endif  /* DVI_CLK_H */
