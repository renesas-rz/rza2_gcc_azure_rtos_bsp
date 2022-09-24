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
* @file         lcd_analog_rgb.h
* @version      1.00
* @brief        LCD panel  definition header
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>

#include    "r_typedefs.h"

#include    "r_vdc.h"

#ifndef LCD_ANALOG_RGB_H
#define LCD_ANALOG_RGB_H

#define     SVGA_800_600     (0u)
#define     XGA_1024_768     (1u)
#define     FWXGA_1360_768   (2u)
#define     SELECT_MONITOR   (FWXGA_1360_768)

/******************************************************************************
Macro definitions
******************************************************************************/
#if ( SELECT_MONITOR == SVGA_800_600)
/* SVGA signal 800x600
    Pixel clock frequency: 40.0 MHz
    Refresh rate: 60.317 Hz
    Polarity of horizontal sync pulse: Positive
    Polarity of vertical sync pulse: Positive
*/
#define     LCD_H_VISIBLE_AREA     (800u)                       /* Horizontal visible area [pixel]     */
#define     LCD_H_FRONT_PORCH      (40u)                        /* Horizontal front porch [pixel]      */
#define     LCD_H_SYNC_WIDTH       (128u)                       /* Horizontal sync pulse width [pixel] */
#define     LCD_H_BACK_PORCH       (88u)                        /* Horizontal back porch [pixel]       */
                                                                /* Horizontal total (one line) [pixel] */
#define     LCD_H_TOTAL            (LCD_H_VISIBLE_AREA +\
                                    LCD_H_FRONT_PORCH +\
                                    LCD_H_SYNC_WIDTH +\
                                    LCD_H_BACK_PORCH)
#define     LCD_H_POLARITY         (VDC_SIG_POL_NOT_INVERTED)   /* Polarity of horizontal sync pulse */

#define     LCD_V_VISIBLE_AREA     (600u)                       /* Vertical visible area [line]      */
#define     LCD_V_FRONT_PORCH      (4u)                         /* Vertical front porch [line]       */
#define     LCD_V_SYNC_WIDTH       (4u)                         /* Vertical sync pulse width [line]  */
#define     LCD_V_BACK_PORCH       (20u)                        /* Vertical back porch [line]        */
                                                                /* Vertical total (one frame) [line] */
#define     LCD_V_TOTAL            (LCD_V_VISIBLE_AREA +\
                                    LCD_V_FRONT_PORCH +\
                                    LCD_V_SYNC_WIDTH +\
                                    LCD_V_BACK_PORCH)
#define     LCD_V_POLARITY         (VDC_SIG_POL_NOT_INVERTED)   /* Polarity of vertical sync pulse   */

#elif ( SELECT_MONITOR == XGA_1024_768)
/* XGA signal 1024x768
    Pixel clock frequency: 75.0 MHz
    Refresh rate: 70.0 Hz
    Polarity of horizontal sync pulse: Negative
    Polarity of vertical sync pulse: Negative
*/
#define     LCD_H_VISIBLE_AREA     (1024u)                     /* Horizontal visible area [pixel] */
#define     LCD_H_FRONT_PORCH      (24u)                       /* Horizontal front porch [pixel] */
#define     LCD_H_SYNC_WIDTH       (136u)                      /* Horizontal sync pulse width [pixel] */
#define     LCD_H_BACK_PORCH       (144u)                      /* Horizontal back porch [pixel] */
/* Horizontal total (one line) [pixel] */
#define     LCD_H_TOTAL            (LCD_H_VISIBLE_AREA +\
                                    LCD_H_FRONT_PORCH +\
                                    LCD_H_SYNC_WIDTH +\
                                    LCD_H_BACK_PORCH)
#define     LCD_H_POLARITY         (VDC_SIG_POL_INVERTED)       /* Polarity of horizontal sync pulse */

#define     LCD_V_VISIBLE_AREA     (768u)                       /* Vertical visible area [line] */
#define     LCD_V_FRONT_PORCH      (4u)                         /* Vertical front porch [line] */
#define     LCD_V_SYNC_WIDTH       (6u)                         /* Vertical sync pulse width [line] */
#define     LCD_V_BACK_PORCH       (28u)                        /* Vertical back porch [line] */
                                                                /* Vertical total (one frame) [line] */
#define     LCD_V_TOTAL            (LCD_V_VISIBLE_AREA +\
                                    LCD_V_FRONT_PORCH +\
                                    LCD_V_SYNC_WIDTH +\
                                    LCD_V_BACK_PORCH)
#define     LCD_V_POLARITY         (VDC_SIG_POL_INVERTED)       /* Polarity of vertical sync pulse */

#elif ( SELECT_MONITOR == FWXGA_1360_768 )
/* FWXGA signal 1360x768
    Pixel clock frequency: 85.5 MHz
    Refresh rate: 60.015 Hz
    Polarity of horizontal sync pulse: Positive
    Polarity of vertical sync pulse: Positive
*/
#define     LCD_H_VISIBLE_AREA      (1360u)                     /* Horizontal visible area [pixel] */
#define     LCD_H_FRONT_PORCH       (64u)                       /* Horizontal front porch [pixel] */
#define     LCD_H_SYNC_WIDTH        (112u)                      /* Horizontal sync pulse width [pixel] */
#define     LCD_H_BACK_PORCH        (256u)                      /* Horizontal back porch [pixel] */
                                                                /* Horizontal total (one line) [pixel] */
#define     LCD_H_TOTAL             (LCD_H_VISIBLE_AREA +\
                                    LCD_H_FRONT_PORCH +\
                                    LCD_H_SYNC_WIDTH +\
                                    LCD_H_BACK_PORCH)
#define     LCD_H_POLARITY          (VDC_SIG_POL_NOT_INVERTED)  /* Polarity of horizontal sync pulse */

#define     LCD_V_VISIBLE_AREA      (768u)                      /* Vertical visible area [line] */
#define     LCD_V_FRONT_PORCH       (4u)                        /* Vertical front porch [line] */
#define     LCD_V_SYNC_WIDTH        (6u)                        /* Vertical sync pulse width [line] */
#define     LCD_V_BACK_PORCH        (17u)                       /* Vertical back porch [line] */
                                                                /* Vertical total (one frame) [line] */
#define     LCD_V_TOTAL             (LCD_V_VISIBLE_AREA +\
                                    LCD_V_FRONT_PORCH +\
                                    LCD_V_SYNC_WIDTH +\
                                    LCD_V_BACK_PORCH)
#define     LCD_V_POLARITY          (VDC_SIG_POL_NOT_INVERTED)  /* Polarity of vertical sync pulse */

#else
    #error  Undefined LCD panel!
#endif

#endif  /* LCD_ANALOG_RGB_H */
