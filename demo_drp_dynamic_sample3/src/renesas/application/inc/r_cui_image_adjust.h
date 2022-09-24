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
/******************************************************************************
 * @headerfile     r_cui_image_adjust.h
 * @brief         This module provides function of register value changing
                  regarding image quality adjustment.
 * @version        1.00
 * @date           27.06.2018
 * H/W Platform    RZ/A2M
 *****************************************************************************/
/*****************************************************************************
* History: DD.MM.YYYY Ver. Description
*        : 11.27.2018 1.01 Improved CUI operation (operated with CR data)
*        : 09.14.2018 1.00 first version
 *****************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "iodefine.h"

/* Multiple inclusion prevention macro */
#ifndef R_CUI_H_
#define R_CUI_H_
/******************************************************************************
Macro definitions
******************************************************************************/
#define INPUT_CMOS_CAM_1                (0)  /** VDC  IN */
#define INPUT_CMOS_CAM_2                (1)  /** CEU  IN */
#define INPUT_CMOS_CAM_3                (2)  /** MIPI IN */
#define TARGET_INPUT                    (INPUT_CMOS_CAM_3)

#define CR_MODE

/* State ID */
#define CUI_STATE_TOP                   (0xFFFFFFFF)
#define CUI_STATE_BRIGHT                (0x00FFFFFF)
#define CUI_STATE_BRIGHT_IN             (0x0000FFFF)
#define CUI_STATE_BRIGHT_OUT            (0x0001FFFF)
#define CUI_STATE_BRIGHT_IN_C           (0x00000CFF)
#define CUI_STATE_BRIGHT_IN_C_0         (0x00000C00)
#define CUI_STATE_BRIGHT_OUT_C          (0x00010CFF)
#define CUI_STATE_BRIGHT_OUT_C_0        (0x00010C00)
#define CUI_STATE_BRIGHT_OUT_C_1        (0x00010C01)
#define CUI_STATE_BRIGHT_OUT_C_2        (0x00010C02)
#define CUI_STATE_CONTRAST              (0x01FFFFFF)
#define CUI_STATE_CONTRAST_IN           (0x0100FFFF)
#define CUI_STATE_CONTRAST_OUT          (0x0101FFFF)
#define CUI_STATE_CONTRAST_IN_C         (0x01000CFF)
#define CUI_STATE_CONTRAST_IN_C_0       (0x01000C00)
#define CUI_STATE_CONTRAST_OUT_C        (0x01010CFF)
#define CUI_STATE_CONTRAST_OUT_C_0      (0x01010C00)
#define CUI_STATE_CONTRAST_OUT_C_1      (0x01010C01)
#define CUI_STATE_CONTRAST_OUT_C_2      (0x01010C02)
#define CUI_STATE_SHARPNESS             (0x02FFFFFF)
#define CUI_STATE_SHARPNESS_C           (0x02FF0CFF)
#define CUI_STATE_SHARPNESS_C_0         (0x02FF0C00)
#define CUI_STATE_SHARPNESS_C_1         (0x02FF0C01)
#define CUI_STATE_SHARPNESS_C_2         (0x02FF0C02)
#define CUI_STATE_SHARPNESS_C_3         (0x02FF0C03)
#define CUI_STATE_SHARPNESS_C_4         (0x02FF0C04)
#define CUI_STATE_SHARPNESS_C_5         (0x02FF0C05)
#define CUI_STATE_SHARPNESS_C_6         (0x02FF0C06)
#define CUI_STATE_SHARPNESS_C_7         (0x02FF0C07)
#define CUI_STATE_SHARPNESS_C_8         (0x02FF0C08)
#define CUI_STATE_SHARPNESS_C_9         (0x02FF0C09)
#define CUI_STATE_SHARPNESS_C_10        (0x02FF0C0A)
#define CUI_STATE_SHARPNESS_C_11        (0x02FF0C0B)
#define CUI_STATE_SHARPNESS_C_12        (0x02FF0C0C)
#define CUI_STATE_SHARPNESS_C_13        (0x02FF0C0D)
#define CUI_STATE_SHARPNESS_C_14        (0x02FF0C0E)
#define CUI_STATE_SHARPNESS_C_15        (0x02FF0C0F)
#define CUI_STATE_SHARPNESS_C_16        (0x02FF0C10)
#define CUI_STATE_SHARPNESS_C_17        (0x02FF0C11)
#define CUI_STATE_SHARPNESS_C_18        (0x02FF0C12)
#define CUI_STATE_SHARPNESS_C_19        (0x02FF0C13)
#define CUI_STATE_SHARPNESS_C_20        (0x02FF0C14)
#define CUI_STATE_SHARPNESS_C_21        (0x02FF0C15)
#define CUI_STATE_SHARPNESS_C_22        (0x02FF0C16)
#define CUI_STATE_SHARPNESS_C_23        (0x02FF0C17)
#define CUI_STATE_SHARPNESS_C_24        (0x02FF0C18)
#define CUI_STATE_SHARPNESS_C_25        (0x02FF0C19)
#define CUI_STATE_GAMMA                 (0x03FFFFFF)
#define CUI_STATE_DITHER                (0x04FFFFFF)
#define CUI_STATE_ROTATION              (0x05FFFFFF)
#define CUI_STATE_ROTATION_C            (0x05FF0CFF)
#define CUI_STATE_ROTATION_C_0          (0x05FF0C00)

/* Menu ID */
#define CUI_MAX_MENUID                  (52)
#define CUI_MAX_INPUT_NUM               (10)

/* Menu Select Number */
#define CUI_SELNUM_TOP                  (6)
#define CUI_SELNUM_BRIGHT               (2)
#define CUI_SELNUM_HML                  (3)
#define CUI_SELNUM_CONTRAST             (2)
#define CUI_SELNUM_SHARP                (3)
#define CUI_SELNUM_GAMMA                (4)
#define CUI_SELNUM_DITHER               (4)
#define CUI_SELNUM_ROTATION             (2)

/* Register Number */
#define CUI_REGNUM_BRIGHT_IN            (1)
#define CUI_REGNUM_BRIGHT_OUT           (3)
#define CUI_REGNUM_CONTRAST_IN          (1)
#define CUI_REGNUM_CONTRAST_OUT         (3)
#define CUI_REGNUM_SHARP                (26)
#define CUI_REGNUM_GAMMA                (190)
#define CUI_REGNUM_DITHER               (1)
#define CUI_REGNUM_ROTATION             (1)
#define CUI_REGNUM_IN_MTX_MODE          (1)

/* Judge */
#define RESULT_OK                       (0)
#define RESULT_NG                       (1)

/* ASCII CODE */
#define ASCII_BS                        (0x08)
#define ASCII_LF                        (0x0A)
#define ASCII_CR                        (0x0D)
#define ASCII_SP                        (' ')
#define ASCII_HYPHEN                    ('-')
#define ASCII_0                         ('0')
#define ASCII_1                         ('1')
#define ASCII_2                         ('2')
#define ASCII_3                         ('3')
#define ASCII_4                         ('4')
#define ASCII_5                         ('5')
#define ASCII_6                         ('6')
#define ASCII_7                         ('7')
#define ASCII_8                         ('8')
#define ASCII_9                         ('9')
#define ASCII_B                         ('B')
#define ASCII_C                         ('C')
#define ASCII_D                         ('D')
#define ASCII_R                         ('R')
#define ASCII_T                         ('T')
#define ASCII_P                         ('P')
#define ASCII_b                         ('b')
#define ASCII_c                         ('c')
#define ASCII_d                         ('d')
#define ASCII_r                         ('r')
#define ASCII_t                         ('t')
#define ASCII_p                         ('p')
#define ASCII_TILDE                     ('~')
#define ASCII_DEL                       (0x7F)

/* Menu pattern */
#define CUI_PATTERN_TRANSITION   (1)    /** transition menu      */
#define CUI_PATTERN_PRESET       (2)    /** preset menu          */
#define CUI_PATTERN_REGISTERLIST (3)    /** register select menu */
#define CUI_PATTERN_REGISTERSET  (4)    /** register setup menu  */

/**
 * @brief Creates touch screen task
 * @param pCom
 */

#endif /* R_CUI_H_ */
