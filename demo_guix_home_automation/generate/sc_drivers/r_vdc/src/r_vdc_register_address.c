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
 * File Name :   r_vdc_register_address.c
 * @file         r_vdc_register_address.c
 * @version      1.12
 * @brief        RZ/A2M VDC driver register address table
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_register.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define     VDC6_CH0_GR0_CLUT_TBL           (*(uint32_t*)0xFCFF6000u)
#define     VDC6_CH0_GR2_CLUT_TBL           (*(uint32_t*)0xFCFF6800u)
#define     VDC6_CH0_GR3_CLUT_TBL           (*(uint32_t*)0xFCFF6C00u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
/* VDC input controller register address list */
const vdc_regaddr_input_ctrl_t vdc_regaddr_input_ctrl = 
{
    /* Cast to an appropriate type */
    &VDC6.INP_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.INP_SEL_CNT.LONG,

    /* Cast to an appropriate type */
    &VDC6.INP_EXT_SYNC_CNT.LONG,

    /* Cast to an appropriate type */
    &VDC6.INP_VSYNC_PH_ADJ.LONG,

    /* Cast to an appropriate type */
    &VDC6.INP_DLY_ADJ.LONG,

    /* Cast to an appropriate type */
    &VDC6.IMGCNT_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.IMGCNT_NR_CNT0.LONG,

    /* Cast to an appropriate type */
    &VDC6.IMGCNT_NR_CNT1.LONG
};

/* VDC scaler register address list */
const vdc_regaddr_scaler_t vdc_regaddr_scaler = 
{
    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC3.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC4.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC5.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC6.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC7.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_FRC9.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_MON0.WORD,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_INT.WORD,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS3.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS4.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS5.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS6.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_DS7.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US3.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US4.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US5.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US6.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US7.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_US8.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL0_OVR1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR3.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR4.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR5.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR6.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR7.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR8.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR9.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR10.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_WR11.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_MON1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF0.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF3.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF_FLD.LONG,

    /* Cast to an appropriate type */
    &VDC6.SC0_SCL1_PBUF_CNT.LONG
};

/* VDC image quality improver register address list */
const vdc_regaddr_img_qlty_imp_t vdc_regaddr_img_qlty_imp = 
{
    /* Cast to an appropriate type */
    &VDC6.ADJ0_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_BKSTR_SET.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_TIM1.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_TIM2.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_TIM3.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP1.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP2.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP3.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP4.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP5.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_SHP6.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_LTI1.LONG,

    /* Cast to an appropriate type */
    &VDC6.ADJ0_ENH_LTI2.LONG
};

/* VDC color matrix register address list */
const vdc_regaddr_color_matrix_t
vdc_regaddr_color_matrix[VDC_COLORMTX_NUM] = 
{
    {
        /* Input Controller */
        &VDC6.IMGCNT_UPDATE.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_MODE.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_YG_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_YG_ADJ1.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_CBB_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_CBB_ADJ1.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_CRR_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.IMGCNT_MTX_CRR_ADJ1.LONG
    },
    {
        /* Image quality improver 0 */
        &VDC6.ADJ0_UPDATE.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_MODE.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_YG_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_YG_ADJ1.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_CBB_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_CBB_ADJ1.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_CRR_ADJ0.LONG,

        /* Cast to an appropriate type */
        &VDC6.ADJ0_MTX_CRR_ADJ1.LONG
    }
};

/* VDC image synthesizer register address list */
const vdc_regaddr_img_synthesizer_t
vdc_regaddr_img_synthesizer[VDC_GR_TYPE_NUM] = 
{
    {
        /* GR0 */
        &VDC6.GR0_UPDATE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM_RD.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_FLM6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB3.LONG,

        /* Cast to an appropriate type */
        NULL,

        /* Cast to an appropriate type */
        NULL,

        /* Cast to an appropriate type */
        NULL,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_AB11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_BASE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR0_CLUT.LONG,

        /* Cast to an appropriate type */
        NULL
    },
    {
        /* GR2 */
        &VDC6.GR2_UPDATE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM_RD.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_FLM6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_AB11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_BASE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_CLUT.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR2_MON.LONG
    },
    {
        /* GR3 */
        &VDC6.GR3_UPDATE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM_RD.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_FLM6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_AB11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_BASE.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_CLUT_INT.LONG,

        /* Cast to an appropriate type */
        &VDC6.GR3_MON.LONG
    }
};

/* VDC CLUT register address list */
uint32_t * const vdc_regaddr_clut[VDC_GR_TYPE_NUM] = 
{
    /* Cast to an appropriate type */
    &VDC6_CH0_GR0_CLUT_TBL,

    /* Cast to an appropriate type */
    &VDC6_CH0_GR2_CLUT_TBL,

    /* Cast to an appropriate type */
    &VDC6_CH0_GR3_CLUT_TBL
};

/* VDC output controller register address list */
const vdc_regaddr_output_ctrl_t vdc_regaddr_output_ctrl = 
{
    /* Cast to an appropriate type */
    &VDC6.TCON_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STVA1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STVA2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STVB1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STVB2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STH1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STH2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STB1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_STB2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_CPV1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_CPV2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_POLA1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_POLA2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_POLB1.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_POLB2.LONG,

    /* Cast to an appropriate type */
    &VDC6.TCON_TIM_DE.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_UPDATE.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_SET.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_BRIGHT1.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_BRIGHT2.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_CONTRAST.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_PDTHA.LONG,

    /* Cast to an appropriate type */
    &VDC6.OUT_CLK_PHASE.LONG
};

/* VDC gamma correction register address list */
const vdc_regaddr_gamma_t vdc_regaddr_gamma = 
{
    /* Cast to an appropriate type */
    &VDC6.GAM_SW.LONG,

    /* Cast to an appropriate type */
    &VDC6.GAM_G_UPDATE.LONG,
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT12.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT13.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT14.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT15.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_LUT16.LONG
    },
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_G_AREA8.LONG
    },

    /* Cast to an appropriate type */
    &VDC6.GAM_B_UPDATE.LONG,
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT12.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT13.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT14.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT15.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_LUT16.LONG
    },
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_B_AREA8.LONG
    },

    /* Cast to an appropriate type */
    &VDC6.GAM_R_UPDATE.LONG,
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT8.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT9.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT10.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT11.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT12.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT13.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT14.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT15.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_LUT16.LONG
    },
    {
        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA1.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA2.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA3.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA4.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA5.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA6.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA7.LONG,

        /* Cast to an appropriate type */
        &VDC6.GAM_R_AREA8.LONG
    }
};

/* VDC system controller register address list */
const vdc_regaddr_system_ctrl_t vdc_regaddr_system_ctrl = 
{
    /* Cast to an appropriate type */
    &VDC6.SYSCNT_INT1.LONG,

    /* Cast to an appropriate type */
    &VDC6.SYSCNT_INT2.LONG,

    /* Cast to an appropriate type */
    &VDC6.SYSCNT_INT4.LONG,

    /* Cast to an appropriate type */
    &VDC6.SYSCNT_INT5.LONG,

    /* Cast to an appropriate type */
    &VDC6.SYSCNT_PANEL_CLK.WORD,

    /* Cast to an appropriate type */
    &VDC6.SYSCNT_CLUT.WORD
};

/* LVDS register address list */
const vdc_regaddr_lvds_t vdc_regaddr_lvds = 
{
    /* Cast to an appropriate type */
    &LVDS.LVDS_UPDATE.LONG,

    /* Cast to an appropriate type */
    &LVDS.LVDSFCL.LONG,

    /* Cast to an appropriate type */
    &LVDS.LCLKSELR.LONG,

    /* Cast to an appropriate type */
    &LVDS.LPLLSETR.LONG,
};
