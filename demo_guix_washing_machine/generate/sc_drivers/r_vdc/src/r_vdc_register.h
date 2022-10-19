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
 * File Name :   r_vdc_register.h
 * @file         r_vdc_register.h
 * @version      1.12
 * @brief        RZ/A2M VDC driver register setup definitions
 ******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"
#include "r_vdc_user.h"

#ifndef R_VDC_REGISTER_H
#define R_VDC_REGISTER_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/
#define     VDC_GAM_LUT_REG_NUM    (16u)   /*!< The number of table setting register in gamma correction block */
#define     VDC_GAM_AREA_REG_NUM   (8u)    /*!< The number of area setting register in gamma correction block */

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! VDC input controller register address list */
typedef struct
{
    volatile uint32_t * inp_update; 
    volatile uint32_t * inp_sel_cnt; 
    volatile uint32_t * inp_ext_sync_cnt; 
    volatile uint32_t * inp_vsync_ph_adj; 
    volatile uint32_t * inp_dly_adj; 
    volatile uint32_t * imgcnt_update; 
    volatile uint32_t * imgcnt_nr_cnt0; 
    volatile uint32_t * imgcnt_nr_cnt1; 
} vdc_regaddr_input_ctrl_t;

/*! VDC scaler register address list */
typedef struct
{
    volatile uint32_t * scl0_update;   
    volatile uint32_t * scl0_frc1;     
    volatile uint32_t * scl0_frc2;     
    volatile uint32_t * scl0_frc3;     
    volatile uint32_t * scl0_frc4;     
    volatile uint32_t * scl0_frc5;     
    volatile uint32_t * scl0_frc6;     
    volatile uint32_t * scl0_frc7;     
    volatile uint32_t * scl0_frc9;     
    volatile uint16_t * scl0_mon0;     
    volatile uint16_t * scl0_int;      
    volatile uint32_t * scl0_ds1;      
    volatile uint32_t * scl0_ds2;      
    volatile uint32_t * scl0_ds3;      
    volatile uint32_t * scl0_ds4;      
    volatile uint32_t * scl0_ds5;      
    volatile uint32_t * scl0_ds6;      
    volatile uint32_t * scl0_ds7;      
    volatile uint32_t * scl0_us1;      
    volatile uint32_t * scl0_us2;      
    volatile uint32_t * scl0_us3;      
    volatile uint32_t * scl0_us4;      
    volatile uint32_t * scl0_us5;      
    volatile uint32_t * scl0_us6;      
    volatile uint32_t * scl0_us7;      
    volatile uint32_t * scl0_us8;      
    volatile uint32_t * scl0_ovr1;     
    volatile uint32_t * scl1_update;   
    volatile uint32_t * scl1_wr1;      
    volatile uint32_t * scl1_wr2;      
    volatile uint32_t * scl1_wr3;      
    volatile uint32_t * scl1_wr4;      
    volatile uint32_t * scl1_wr5;      
    volatile uint32_t * scl1_wr6;      
    volatile uint32_t * scl1_wr7;      
    volatile uint32_t * scl1_wr8;      
    volatile uint32_t * scl1_wr9;      
    volatile uint32_t * scl1_wr10;     
    volatile uint32_t * scl1_wr11;     
    volatile uint32_t * scl1_mon1;     
    volatile uint32_t * scl1_pbuf0;    
    volatile uint32_t * scl1_pbuf1;    
    volatile uint32_t * scl1_pbuf2;    
    volatile uint32_t * scl1_pbuf3;    
    volatile uint32_t * scl1_pbuf_fld; 
    volatile uint32_t * scl1_pbuf_cnt; 
} vdc_regaddr_scaler_t;

/*! VDC image quality improver register address list */
typedef struct
{
    volatile uint32_t * adj_update;    
    volatile uint32_t * adj_bkstr_set; 
    volatile uint32_t * adj_enh_tim1;  
    volatile uint32_t * adj_enh_tim2;  
    volatile uint32_t * adj_enh_tim3;  
    volatile uint32_t * adj_enh_shp1;  
    volatile uint32_t * adj_enh_shp2;  
    volatile uint32_t * adj_enh_shp3;  
    volatile uint32_t * adj_enh_shp4;  
    volatile uint32_t * adj_enh_shp5;  
    volatile uint32_t * adj_enh_shp6;  
    volatile uint32_t * adj_enh_lti1;  
    volatile uint32_t * adj_enh_lti2;  
} vdc_regaddr_img_qlty_imp_t;

/*! VDC color matrix register address list */
typedef struct
{
    volatile uint32_t * mtx_update;   
    volatile uint32_t * mtx_mode;     
    volatile uint32_t * mtx_yg_adj0;  
    volatile uint32_t * mtx_yg_adj1;  
    volatile uint32_t * mtx_cbb_adj0; 
    volatile uint32_t * mtx_cbb_adj1; 
    volatile uint32_t * mtx_crr_adj0; 
    volatile uint32_t * mtx_crr_adj1; 
} vdc_regaddr_color_matrix_t;

/*! VDC image synthesizer register address list */
typedef struct
{
    volatile uint32_t * gr_update; 
    volatile uint32_t * gr_flm_rd; 
    volatile uint32_t * gr_flm1;   
    volatile uint32_t * gr_flm2;   
    volatile uint32_t * gr_flm3;   
    volatile uint32_t * gr_flm4;   
    volatile uint32_t * gr_flm5;   
    volatile uint32_t * gr_flm6;   
    volatile uint32_t * gr_ab1;    
    volatile uint32_t * gr_ab2;    
    volatile uint32_t * gr_ab3;    
    volatile uint32_t * gr_ab4;    
    volatile uint32_t * gr_ab5;    
    volatile uint32_t * gr_ab6;    
    volatile uint32_t * gr_ab7;    
    volatile uint32_t * gr_ab8;    
    volatile uint32_t * gr_ab9;    
    volatile uint32_t * gr_ab10;   
    volatile uint32_t * gr_ab11;   
    volatile uint32_t * gr_base;   
    volatile uint32_t * gr_clut;   
    volatile uint32_t * gr_mon;    
} vdc_regaddr_img_synthesizer_t;   

/*! VDC gamma correction register address list */
typedef struct
{
    volatile uint32_t * gam_sw;                           
    volatile uint32_t * gam_g_update;                     
    volatile uint32_t * gam_g_lut[VDC_GAM_LUT_REG_NUM];   
    volatile uint32_t * gam_g_area[VDC_GAM_AREA_REG_NUM]; 
    volatile uint32_t * gam_b_update;                     
    volatile uint32_t * gam_b_lut[VDC_GAM_LUT_REG_NUM];   
    volatile uint32_t * gam_b_area[VDC_GAM_AREA_REG_NUM]; 
    volatile uint32_t * gam_r_update;                     
    volatile uint32_t * gam_r_lut[VDC_GAM_LUT_REG_NUM];   
    volatile uint32_t * gam_r_area[VDC_GAM_AREA_REG_NUM]; 
} vdc_regaddr_gamma_t;

/*! VDC output controller register address list */
typedef struct
{
    volatile uint32_t * tcon_update;     
    volatile uint32_t * tcon_tim;        
    volatile uint32_t * tcon_tim_stva1;  
    volatile uint32_t * tcon_tim_stva2;  
    volatile uint32_t * tcon_tim_stvb1;  
    volatile uint32_t * tcon_tim_stvb2;  
    volatile uint32_t * tcon_tim_sth1;   
    volatile uint32_t * tcon_tim_sth2;   
    volatile uint32_t * tcon_tim_stb1;   
    volatile uint32_t * tcon_tim_stb2;   
    volatile uint32_t * tcon_tim_cpv1;   
    volatile uint32_t * tcon_tim_cpv2;   
    volatile uint32_t * tcon_tim_pola1;  
    volatile uint32_t * tcon_tim_pola2;  
    volatile uint32_t * tcon_tim_polb1;  
    volatile uint32_t * tcon_tim_polb2;  
    volatile uint32_t * tcon_tim_de;     
    volatile uint32_t * out_update;      
    volatile uint32_t * out_set;         
    volatile uint32_t * out_bright1;     
    volatile uint32_t * out_bright2;     
    volatile uint32_t * out_contrast;    
    volatile uint32_t * out_pdtha;       
    volatile uint32_t * out_clk_phase;   
} vdc_regaddr_output_ctrl_t;

/*! VDC system controller register address list */
typedef struct
{
    volatile uint32_t * syscnt_int1;       
    volatile uint32_t * syscnt_int2;       
    volatile uint32_t * syscnt_int4;       
    volatile uint32_t * syscnt_int5;       
    volatile uint16_t * syscnt_panel_clk;  
    volatile uint16_t * syscnt_clut;       
} vdc_regaddr_system_ctrl_t;

/*! LVDS register address list */
typedef struct
{
    volatile uint32_t * lvds_update;  
    volatile uint32_t * lvdsfcl;      
    volatile uint32_t * lclkselr;     
    volatile uint32_t * lpllsetr;     
/*    volatile uint32_t * lphyacc;*/
} vdc_regaddr_lvds_t;

/******************************************************************************
Variable Externs
******************************************************************************/
extern const vdc_regaddr_input_ctrl_t
    vdc_regaddr_input_ctrl;                       
extern const vdc_regaddr_scaler_t
    vdc_regaddr_scaler;                           
extern const vdc_regaddr_img_qlty_imp_t
    vdc_regaddr_img_qlty_imp;                     
extern const vdc_regaddr_color_matrix_t
    vdc_regaddr_color_matrix[VDC_COLORMTX_NUM];   
extern const vdc_regaddr_img_synthesizer_t
    vdc_regaddr_img_synthesizer[VDC_GR_TYPE_NUM]; 
extern uint32_t * const
    vdc_regaddr_clut[VDC_GR_TYPE_NUM];            
extern const vdc_regaddr_output_ctrl_t
    vdc_regaddr_output_ctrl;                      
extern const vdc_regaddr_gamma_t
    vdc_regaddr_gamma;                            
extern const vdc_regaddr_system_ctrl_t
    vdc_regaddr_system_ctrl;                      
extern const vdc_regaddr_lvds_t
    vdc_regaddr_lvds;                             

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
 * @fn          VDC_Initialize
 * @brief       Sets registers for initialization
 * @param[in]   param                   : Initialization parameter
 * @retval      None
 *****************************************************************************/
void VDC_Initialize(const vdc_init_t * const param);

/******************************************************************************
 * @fn          VDC_Terminate
 * @brief       Sets registers for termination
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
void VDC_Terminate(void);

/******************************************************************************
 * @fn          VDC_VideoInput
 * @brief       Sets registers for video input
 * @param[in]   param                   : Video input setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_VideoInput(const vdc_input_t * const param);

/******************************************************************************
 * @fn          VDC_SyncControl
 * @brief       Sets registers for sync control
 * @param[in]   param                   : Sync signal control parameter
 * @retval      None
 *****************************************************************************/
void VDC_SyncControl(const vdc_sync_ctrl_t * const param);

/******************************************************************************
 * @fn          VDC_DisplayOutput
 * @brief       Sets registers for display output
 * @param[in]   param                   : Display output configuration parameter
 * @retval      None
 *****************************************************************************/
void VDC_DisplayOutput(const vdc_output_t * const param);

/******************************************************************************
 * @fn          VDC_WriteDataControl
 * @brief       Sets registers for data write control processing
 * @param[in]   param                   : Data write control parameter
 * @retval      None
 *****************************************************************************/
void VDC_WriteDataControl(const vdc_write_t * const param);

/******************************************************************************
 * @fn          VDC_ChangeWriteProcess
 * @brief       Sets registers for data write change processing
 * @param[in]   param                   : Data write change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ChangeWriteProcess(const vdc_write_chg_t * const param);

/******************************************************************************
 * @fn          VDC_ReadDataControl
 * @brief       Sets registers for data read control processing
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read control parameter
 * @retval      None
 *****************************************************************************/
void VDC_ReadDataControl(const vdc_graphics_type_t graphics_id, const vdc_read_t * const param);

/******************************************************************************
 * @fn          VDC_ChangeReadProcess
 * @brief       Sets registers for data read change processing
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ChangeReadProcess(const vdc_graphics_type_t graphics_id, const vdc_read_chg_t * const param);

/******************************************************************************
 * @fn          VDC_StartProcess
 * @brief       Sets registers for data write/read start processing
 * @param[in]   layer_id                : Layer ID
 * @param[in]   param                   : Data write/read start parameter
 * @retval      None
 *****************************************************************************/
void VDC_StartProcess(const vdc_layer_id_t layer_id, const vdc_start_t * const param);

/******************************************************************************
 * @fn          VDC_StopProcess
 * @brief       Sets registers for data write/read stop processing
 * @param[in]   layer_id                : Layer ID
 * @retval      None
 *****************************************************************************/
void VDC_StopProcess(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_VideoNoiseReduction
 * @param[in]   nr1d_on                 : Noise reduction ON/OFF setting
 * @param[in]   param                   : Noise reduction setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_VideoNoiseReduction(const vdc_onoff_t nr1d_on, const vdc_noise_reduction_t * const param);

/******************************************************************************
 * @fn          VDC_ImageColorMatrix
 * @brief       Sets registers for color matrix
 * @param[in]   param                   : Color matrix setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_ImageColorMatrix(const vdc_color_matrix_t * const param);

/******************************************************************************
 * @fn          VDC_ImageEnhancement
 * @brief       Sets registers for image enhancement processing
 * @param[in]   shp_h_on                : Sharpness ON/OFF setting
 * @param[in]   sharp_param             : Sharpness setup parameter
 * @param[in]   lti_h_on                : LTI ON/OFF setting
 * @param[in]   lti_param               : LTI setup parameter
 * @param[in]   enh_area                : Enhancer-enabled area setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_ImageEnhancement(
    const vdc_onoff_t                  shp_h_on,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_onoff_t                  lti_h_on,
    const vdc_enhance_lti_t    * const lti_param,
    const vdc_period_rect_t    * const enh_area);

/******************************************************************************
 * @fn          VDC_ImageBlackStretch
 * @brief       Sets registers for black stretch
 * @param[in]   bkstr_on                : Black stretch ON/OFF setting
 * @param[in]   param                   : Black stretch setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_ImageBlackStretch(
    const vdc_onoff_t          bkstr_on,
    const vdc_black_t  * const param);

/******************************************************************************
 * @fn          VDC_AlphaBlending
 * @brief       Sets registers for alpha blending
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Alpha blending setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_AlphaBlending(const vdc_graphics_type_t graphics_id, const vdc_alpha_blending_t * const param);

/******************************************************************************
 * @fn          VDC_AlphaBlendingRect
 * @brief       Sets registers for rectangle alpha blending
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   gr_arc_on               : ON/OFF setting for alpha blending in a rectangular area
 * @param[in]   param                   : Setup parameter for alpha blending in a rectangular area
 * @retval      None
 *****************************************************************************/
void VDC_AlphaBlendingRect(
    const vdc_graphics_type_t                  graphics_id,
    const vdc_onoff_t                          gr_arc_on,
    const vdc_alpha_blending_rect_t    * const param);

/******************************************************************************
 * @fn          VDC_Chromakey
 * @brief       Sets registers for chroma-key
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   gr_ck_on                : Chroma-key ON/OFF setting
 * @param[in]   param                   : Chroma-key setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_Chromakey(
    const vdc_graphics_type_t      graphics_id,
    const vdc_onoff_t              gr_ck_on,
    const vdc_chromakey_t  * const param);

/******************************************************************************
 * @fn          VDC_CLUT
 * @brief       Sets registers for CLUT
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : CLUT setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_CLUT(const vdc_graphics_type_t graphics_id, const vdc_clut_t * const param);

/******************************************************************************
 * @fn          VDC_DisplayCalibration
 * @brief       Sets registers for display calibration
 * @param[in]   param                   : Display calibration parameter
 * @retval      None
 *****************************************************************************/
void VDC_DisplayCalibration(const vdc_disp_calibration_t * const param);

/******************************************************************************
 * @fn          VDC_GammaCorrection
 * @brief       Sets registers for gamma correction
 * @param[in]   gam_on      : Gamma correction ON/OFF setting
 * @param[in]   param       : Gamma correction setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_GammaCorrection(const vdc_onoff_t gam_on, const vdc_gamma_correction_t * const param);

/******************************************************************************
 * @fn          VDC_Int_Disable
 * @brief       Disables all VDC interrupts
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
void VDC_Int_Disable(void);

/******************************************************************************
 * @fn          VDC_Int_SetInterrupt
 * @brief       Enables/disables the specified VDC interrupt
 * @param[in]   param                   : Interrupt callback setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_Int_SetInterrupt(const vdc_int_t * const param);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_VDC_REGISTER_H */

