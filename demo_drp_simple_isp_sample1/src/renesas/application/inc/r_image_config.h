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
* File Name    : r_image_config.h
* Description  : Defines for Image Correction
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"
#include    "lcd_panel.h"

#ifndef IMG_CONFIG_H
#define IMG_CONFIG_H
/******************************************************************************
Macro definitions
******************************************************************************/
/* Brightness Setting */
/* Contrast Setting */
#define IMGC_BRIGHT_INPUTCTL_MODULE          (VDC_COLORMTX_IMGCNT)       /* This value is fixed */
#define IMGC_BRIGHT_INPUTCTL_MODE            (VDC_COLORMTX_YCBCR_YCBCR)
#define IMGC_BRIGHT_INPUTCTL_OFFSET_YG       (128u)                      
#define IMGC_BRIGHT_INPUTCTL_OFFSET_B        (128u) /*This value is default. User can modify based on user settings.*/
#define IMGC_BRIGHT_INPUTCTL_OFFSET_R        (128u) /*This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_GG       (256u)                      
#define IMGC_CONTRAST_INPUTCTL_GAIN_GB       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_GR       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_BG       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_BB       (256u) /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_BR       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_RG       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_RB       (0u)   /* This value is default. User can modify based on user settings.*/
#define IMGC_CONTRAST_INPUTCTL_GAIN_RR       (256u) /* This value is default. User can modify based on user settings.*/

#define IMGC_OUTCTL_CALIB_ROUTE              (VDC_CALIBR_ROUTE_BCG)      /* This value is default. User can modify based on user settings. */
#define IMGC_BRIGHT_OUTCTL_OFFSET_G          (512u)
#define IMGC_BRIGHT_OUTCTL_OFFSET_B          (512u)
#define IMGC_BRIGHT_OUTCTL_OFFSET_R          (512u)
#define IMGC_CONTRAST_OUTCTL_GAIN_G          (128u)
#define IMGC_CONTRAST_OUTCTL_GAIN_B          (128u)
#define IMGC_CONTRAST_OUTCTL_GAIN_R          (128u)

/* Horizontal Mirroring and Rotation */
#define IMGC_ROTATION_MODE                   (0u)

/* Sharpness Setting */
#define IMGC_IMG_IMPRV_ID                    (VDC_IMG_IMPRV_0)           /* This value is fixed */
#define IMGC_SHARPNESS_ACTIVE                (VDC_OFF)
#define IMGC_SHARPNESS_H1_CLIP_O             (0u)
#define IMGC_SHARPNESS_H1_CLIP_U             (0u)
#define IMGC_SHARPNESS_H1_GAIN_O             (0u)
#define IMGC_SHARPNESS_H1_GAIN_U             (0u)
#define IMGC_SHARPNESS_H1_CORE               (0u)
#define IMGC_SHARPNESS_H2_LPF                (VDC_OFF)
#define IMGC_SHARPNESS_H2_CLIP_O             (0u)
#define IMGC_SHARPNESS_H2_CLIP_U             (0u)
#define IMGC_SHARPNESS_H2_GAIN_O             (0u)
#define IMGC_SHARPNESS_H2_GAIN_U             (0u)
#define IMGC_SHARPNESS_H2_CORE               (0u)
#define IMGC_SHARPNESS_H3_CLIP_O             (0u)
#define IMGC_SHARPNESS_H3_CLIP_U             (0u)
#define IMGC_SHARPNESS_H3_GAIN_O             (0u)
#define IMGC_SHARPNESS_H3_GAIN_U             (0u)
#define IMGC_SHARPNESS_H3_CORE               (0u)
#define IMGC_SHARPNESS_LTI_ACTIVE            (VDC_OFF)
#define IMGC_SHARPNESS_LTI_H2_LPF            (VDC_OFF)
#define IMGC_SHARPNESS_LTI_H2_ZERO           (10u)
#define IMGC_SHARPNESS_LTI_H2_GAIN           (0u)
#define IMGC_SHARPNESS_LTI_H2_CORE           (0u)
#define IMGC_SHARPNESS_LTI_H4_MEDTAP_SEL     (VDC_LTI_MDFIL_SEL_ADJ2)
#define IMGC_SHARPNESS_LTI_H4_ZERO           (10u)
#define IMGC_SHARPNESS_LTI_H4_GAIN           (0u)
#define IMGC_SHARPNESS_LTI_H4_CORE           (0u)
#define IMGC_SHARPNESS_VS                    (LCD_CH0_DISP_VS)           /* This value is fixed */
#define IMGC_SHARPNESS_VW                    (LCD_CH0_DISP_VW)           /* This value is fixed */
#define IMGC_SHARPNESS_HS                    (LCD_CH0_DISP_HS)           /* This value is fixed */
#define IMGC_SHARPNESS_HW                    (LCD_CH0_DISP_HW)           /* This value is fixed */

/* Gamma correction */
#define IMGC_GAMMA_ACTIVE                    (VDC_ON)
#define IMGC_GAMMA_G_GAIN_00                 (1024u)
#define IMGC_GAMMA_G_GAIN_01                 (1024u)
#define IMGC_GAMMA_G_GAIN_02                 (1024u)
#define IMGC_GAMMA_G_GAIN_03                 (1024u)
#define IMGC_GAMMA_G_GAIN_04                 (1024u)
#define IMGC_GAMMA_G_GAIN_05                 (1024u)
#define IMGC_GAMMA_G_GAIN_06                 (1024u)
#define IMGC_GAMMA_G_GAIN_07                 (1024u)
#define IMGC_GAMMA_G_GAIN_08                 (1024u)
#define IMGC_GAMMA_G_GAIN_09                 (1024u)
#define IMGC_GAMMA_G_GAIN_10                 (1024u)
#define IMGC_GAMMA_G_GAIN_11                 (1024u)
#define IMGC_GAMMA_G_GAIN_12                 (1024u)
#define IMGC_GAMMA_G_GAIN_13                 (1024u)
#define IMGC_GAMMA_G_GAIN_14                 (1024u)
#define IMGC_GAMMA_G_GAIN_15                 (1024u)
#define IMGC_GAMMA_G_GAIN_16                 (1024u)
#define IMGC_GAMMA_G_GAIN_17                 (1024u)
#define IMGC_GAMMA_G_GAIN_18                 (1024u)
#define IMGC_GAMMA_G_GAIN_19                 (1024u)
#define IMGC_GAMMA_G_GAIN_20                 (1024u)
#define IMGC_GAMMA_G_GAIN_21                 (1024u)
#define IMGC_GAMMA_G_GAIN_22                 (1024u)
#define IMGC_GAMMA_G_GAIN_23                 (1024u)
#define IMGC_GAMMA_G_GAIN_24                 (1024u)
#define IMGC_GAMMA_G_GAIN_25                 (1024u)
#define IMGC_GAMMA_G_GAIN_26                 (1024u)
#define IMGC_GAMMA_G_GAIN_27                 (1024u)
#define IMGC_GAMMA_G_GAIN_28                 (1024u)
#define IMGC_GAMMA_G_GAIN_29                 (1024u)
#define IMGC_GAMMA_G_GAIN_30                 (1024u)
#define IMGC_GAMMA_G_GAIN_31                 (1024u)
#define IMGC_GAMMA_G_TH_01                   (8u)
#define IMGC_GAMMA_G_TH_02                   (16u)
#define IMGC_GAMMA_G_TH_03                   (24u)
#define IMGC_GAMMA_G_TH_04                   (32u)
#define IMGC_GAMMA_G_TH_05                   (40u)
#define IMGC_GAMMA_G_TH_06                   (48u)
#define IMGC_GAMMA_G_TH_07                   (56u)
#define IMGC_GAMMA_G_TH_08                   (64u)
#define IMGC_GAMMA_G_TH_09                   (72u)
#define IMGC_GAMMA_G_TH_10                   (80u)
#define IMGC_GAMMA_G_TH_11                   (88u)
#define IMGC_GAMMA_G_TH_12                   (96u)
#define IMGC_GAMMA_G_TH_13                   (104u)
#define IMGC_GAMMA_G_TH_14                   (112u)
#define IMGC_GAMMA_G_TH_15                   (120u)
#define IMGC_GAMMA_G_TH_16                   (128u)
#define IMGC_GAMMA_G_TH_17                   (136u)
#define IMGC_GAMMA_G_TH_18                   (144u)
#define IMGC_GAMMA_G_TH_19                   (152u)
#define IMGC_GAMMA_G_TH_20                   (160u)
#define IMGC_GAMMA_G_TH_21                   (168u)
#define IMGC_GAMMA_G_TH_22                   (176u)
#define IMGC_GAMMA_G_TH_23                   (184u)
#define IMGC_GAMMA_G_TH_24                   (192u)
#define IMGC_GAMMA_G_TH_25                   (200u)
#define IMGC_GAMMA_G_TH_26                   (208u)
#define IMGC_GAMMA_G_TH_27                   (216u)
#define IMGC_GAMMA_G_TH_28                   (224u)
#define IMGC_GAMMA_G_TH_29                   (232u)
#define IMGC_GAMMA_G_TH_30                   (240u)
#define IMGC_GAMMA_G_TH_31                   (248u)
#define IMGC_GAMMA_B_GAIN_00                 (1024u)
#define IMGC_GAMMA_B_GAIN_01                 (1024u)
#define IMGC_GAMMA_B_GAIN_02                 (1024u)
#define IMGC_GAMMA_B_GAIN_03                 (1024u)
#define IMGC_GAMMA_B_GAIN_04                 (1024u)
#define IMGC_GAMMA_B_GAIN_05                 (1024u)
#define IMGC_GAMMA_B_GAIN_06                 (1024u)
#define IMGC_GAMMA_B_GAIN_07                 (1024u)
#define IMGC_GAMMA_B_GAIN_08                 (1024u)
#define IMGC_GAMMA_B_GAIN_09                 (1024u)
#define IMGC_GAMMA_B_GAIN_10                 (1024u)
#define IMGC_GAMMA_B_GAIN_11                 (1024u)
#define IMGC_GAMMA_B_GAIN_12                 (1024u)
#define IMGC_GAMMA_B_GAIN_13                 (1024u)
#define IMGC_GAMMA_B_GAIN_14                 (1024u)
#define IMGC_GAMMA_B_GAIN_15                 (1024u)
#define IMGC_GAMMA_B_GAIN_16                 (1024u)
#define IMGC_GAMMA_B_GAIN_17                 (1024u)
#define IMGC_GAMMA_B_GAIN_18                 (1024u)
#define IMGC_GAMMA_B_GAIN_19                 (1024u)
#define IMGC_GAMMA_B_GAIN_20                 (1024u)
#define IMGC_GAMMA_B_GAIN_21                 (1024u)
#define IMGC_GAMMA_B_GAIN_22                 (1024u)
#define IMGC_GAMMA_B_GAIN_23                 (1024u)
#define IMGC_GAMMA_B_GAIN_24                 (1024u)
#define IMGC_GAMMA_B_GAIN_25                 (1024u)
#define IMGC_GAMMA_B_GAIN_26                 (1024u)
#define IMGC_GAMMA_B_GAIN_27                 (1024u)
#define IMGC_GAMMA_B_GAIN_28                 (1024u)
#define IMGC_GAMMA_B_GAIN_29                 (1024u)
#define IMGC_GAMMA_B_GAIN_30                 (1024u)
#define IMGC_GAMMA_B_GAIN_31                 (1024u)
#define IMGC_GAMMA_B_TH_01                   (8u)
#define IMGC_GAMMA_B_TH_02                   (16u)
#define IMGC_GAMMA_B_TH_03                   (24u)
#define IMGC_GAMMA_B_TH_04                   (32u)
#define IMGC_GAMMA_B_TH_05                   (40u)
#define IMGC_GAMMA_B_TH_06                   (48u)
#define IMGC_GAMMA_B_TH_07                   (56u)
#define IMGC_GAMMA_B_TH_08                   (64u)
#define IMGC_GAMMA_B_TH_09                   (72u)
#define IMGC_GAMMA_B_TH_10                   (80u)
#define IMGC_GAMMA_B_TH_11                   (88u)
#define IMGC_GAMMA_B_TH_12                   (96u)
#define IMGC_GAMMA_B_TH_13                   (104u)
#define IMGC_GAMMA_B_TH_14                   (112u)
#define IMGC_GAMMA_B_TH_15                   (120u)
#define IMGC_GAMMA_B_TH_16                   (128u)
#define IMGC_GAMMA_B_TH_17                   (136u)
#define IMGC_GAMMA_B_TH_18                   (144u)
#define IMGC_GAMMA_B_TH_19                   (152u)
#define IMGC_GAMMA_B_TH_20                   (160u)
#define IMGC_GAMMA_B_TH_21                   (168u)
#define IMGC_GAMMA_B_TH_22                   (176u)
#define IMGC_GAMMA_B_TH_23                   (184u)
#define IMGC_GAMMA_B_TH_24                   (192u)
#define IMGC_GAMMA_B_TH_25                   (200u)
#define IMGC_GAMMA_B_TH_26                   (208u)
#define IMGC_GAMMA_B_TH_27                   (216u)
#define IMGC_GAMMA_B_TH_28                   (224u)
#define IMGC_GAMMA_B_TH_29                   (232u)
#define IMGC_GAMMA_B_TH_30                   (240u)
#define IMGC_GAMMA_B_TH_31                   (248u)
#define IMGC_GAMMA_R_GAIN_00                 (1024u)
#define IMGC_GAMMA_R_GAIN_01                 (1024u)
#define IMGC_GAMMA_R_GAIN_02                 (1024u)
#define IMGC_GAMMA_R_GAIN_03                 (1024u)
#define IMGC_GAMMA_R_GAIN_04                 (1024u)
#define IMGC_GAMMA_R_GAIN_05                 (1024u)
#define IMGC_GAMMA_R_GAIN_06                 (1024u)
#define IMGC_GAMMA_R_GAIN_07                 (1024u)
#define IMGC_GAMMA_R_GAIN_08                 (1024u)
#define IMGC_GAMMA_R_GAIN_09                 (1024u)
#define IMGC_GAMMA_R_GAIN_10                 (1024u)
#define IMGC_GAMMA_R_GAIN_11                 (1024u)
#define IMGC_GAMMA_R_GAIN_12                 (1024u)
#define IMGC_GAMMA_R_GAIN_13                 (1024u)
#define IMGC_GAMMA_R_GAIN_14                 (1024u)
#define IMGC_GAMMA_R_GAIN_15                 (1024u)
#define IMGC_GAMMA_R_GAIN_16                 (1024u)
#define IMGC_GAMMA_R_GAIN_17                 (1024u)
#define IMGC_GAMMA_R_GAIN_18                 (1024u)
#define IMGC_GAMMA_R_GAIN_19                 (1024u)
#define IMGC_GAMMA_R_GAIN_20                 (1024u)
#define IMGC_GAMMA_R_GAIN_21                 (1024u)
#define IMGC_GAMMA_R_GAIN_22                 (1024u)
#define IMGC_GAMMA_R_GAIN_23                 (1024u)
#define IMGC_GAMMA_R_GAIN_24                 (1024u)
#define IMGC_GAMMA_R_GAIN_25                 (1024u)
#define IMGC_GAMMA_R_GAIN_26                 (1024u)
#define IMGC_GAMMA_R_GAIN_27                 (1024u)
#define IMGC_GAMMA_R_GAIN_28                 (1024u)
#define IMGC_GAMMA_R_GAIN_29                 (1024u)
#define IMGC_GAMMA_R_GAIN_30                 (1024u)
#define IMGC_GAMMA_R_GAIN_31                 (1024u)
#define IMGC_GAMMA_R_TH_01                   (8u)
#define IMGC_GAMMA_R_TH_02                   (16u)
#define IMGC_GAMMA_R_TH_03                   (24u)
#define IMGC_GAMMA_R_TH_04                   (32u)
#define IMGC_GAMMA_R_TH_05                   (40u)
#define IMGC_GAMMA_R_TH_06                   (48u)
#define IMGC_GAMMA_R_TH_07                   (56u)
#define IMGC_GAMMA_R_TH_08                   (64u)
#define IMGC_GAMMA_R_TH_09                   (72u)
#define IMGC_GAMMA_R_TH_10                   (80u)
#define IMGC_GAMMA_R_TH_11                   (88u)
#define IMGC_GAMMA_R_TH_12                   (96u)
#define IMGC_GAMMA_R_TH_13                   (104u)
#define IMGC_GAMMA_R_TH_14                   (112u)
#define IMGC_GAMMA_R_TH_15                   (120u)
#define IMGC_GAMMA_R_TH_16                   (128u)
#define IMGC_GAMMA_R_TH_17                   (136u)
#define IMGC_GAMMA_R_TH_18                   (144u)
#define IMGC_GAMMA_R_TH_19                   (152u)
#define IMGC_GAMMA_R_TH_20                   (160u)
#define IMGC_GAMMA_R_TH_21                   (168u)
#define IMGC_GAMMA_R_TH_22                   (176u)
#define IMGC_GAMMA_R_TH_23                   (184u)
#define IMGC_GAMMA_R_TH_24                   (192u)
#define IMGC_GAMMA_R_TH_25                   (200u)
#define IMGC_GAMMA_R_TH_26                   (208u)
#define IMGC_GAMMA_R_TH_27                   (216u)
#define IMGC_GAMMA_R_TH_28                   (224u)
#define IMGC_GAMMA_R_TH_29                   (232u)
#define IMGC_GAMMA_R_TH_30                   (240u)
#define IMGC_GAMMA_R_TH_31                   (248u)

/* Dither Process */
#define IMGC_DITHER_MODE                     (VDC_PDTH_MD_TRU)
#define IMGC_DITHER_2X2_PA                   (3u) /* This value is default. User can modify based on user settings. */
#define IMGC_DITHER_2X2_PB                   (0u) /* This value is default. User can modify based on user settings. */
#define IMGC_DITHER_2X2_PC                   (2u) /* This value is default. User can modify based on user settings. */
#define IMGC_DITHER_2X2_PD                   (1u) /* This value is default. User can modify based on user settings. */

#endif  /* IMG_CONFIG_H */
