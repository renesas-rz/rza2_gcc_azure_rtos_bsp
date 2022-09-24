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
 * File Name :   r_vdc_check_parameter.h
 * @file         r_vdc_check_parameter.h
 * @version      1.12
 * @brief        RZ/A2M VDC driver parameter check definitions
 ******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"
#include "r_vdc_user.h"

#ifndef R_VDC_CHECK_PARAMETER_H
#define R_VDC_CHECK_PARAMETER_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

#ifdef      R_VDC_CHECK_PARAMETERS
/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
 * @fn          VDC_InitializeCheckPrm
 * @brief       Checks on initialization parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Initialization parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_InitializeCheckPrm(const vdc_channel_t ch, const vdc_init_t * const param);

/******************************************************************************
 * @fn          VDC_TerminateCheckPrm
 * @brief       Checks on parameters in R_VDC_Terminate
 * @param[in]   ch          : Channel
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_TerminateCheckPrm(const vdc_channel_t ch);

/******************************************************************************
 * @fn          VDC_VideoInputCheckPrm
 * @brief       Checks on video input setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Video input setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_VideoInputCheckPrm(const vdc_channel_t ch, const vdc_input_t * const param);

/******************************************************************************
 * @fn          VDC_SyncControlCheckPrm
 * @brief       Checks on sync control parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Sync control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_SyncControlCheckPrm(const vdc_channel_t ch, const vdc_sync_ctrl_t * const param);

/******************************************************************************
 * @fn          VDC_DisplayOutputCheckPrm
 * @brief       Checks on display output configuration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display output configuration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_DisplayOutputCheckPrm(const vdc_channel_t ch, const vdc_output_t * const param);

/******************************************************************************
 * @fn          VDC_CallbackISRCheckPrm
 * @brief       Checks on interrupt callback setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Interrupt callback setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_CallbackISRCheckPrm(const vdc_channel_t ch, const vdc_int_t * const param);

/******************************************************************************
 * @fn          VDC_WriteDataControlCheckPrm
 * @brief       Checks on data write control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_WriteDataControlCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_write_t  * const param);

/******************************************************************************
 * @fn          VDC_ChangeWriteProcessCheckPrm
 * @brief       Checks on data write change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChangeWriteProcessCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_write_chg_t  * const param);

/******************************************************************************
 * @fn          VDC_ReadDataControlCheckPrm
 * @brief       Checks on data read control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Data read control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ReadDataControlCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_read_t   * const param);

/******************************************************************************
 * @fn          VDC_ChangeReadProcessCheckPrm
 * @brief       Checks on data read change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data read change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChangeReadProcessCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_read_chg_t   * const param);

/******************************************************************************
 * @fn          VDC_StartProcessCheckPrm
 * @brief       Checks on data write/read start parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write/read start parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_StartProcessCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_start_t  * const param);

/******************************************************************************
 * @fn          VDC_StopProcessCheckPrm
 * @brief       Checks on parameters in R_VDC_StopProcess
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_StopProcessCheckPrm(const vdc_channel_t ch, const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ReleaseDataControlCheckPrm
 * @brief       Checks on parameters in R_VDC_ReleaseDataControl
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ReleaseDataControlCheckPrm(const vdc_channel_t ch, const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_VideoNoiseReductCheckPrm
 * @brief       Checks on noise reduction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Noise reduction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_VideoNoiseReductCheckPrm(const vdc_channel_t ch, const vdc_noise_reduction_t * const param);

/******************************************************************************
 * @fn          VDC_ImageColorMatrixCheckPrm
 * @brief       Checks on color matrix setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Color matrix setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageColorMatrixCheckPrm(const vdc_channel_t ch, const vdc_color_matrix_t * const param);

/******************************************************************************
 * @fn          VDC_ImageEnhancementCheckPrm
 * @brief       Checks on sharpness setup parameter and enhancer-enabled area setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   sharp_param : Sharpness setup parameter
 * @param[in]   enh_area    : Enhancer-enabled area setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageEnhancementCheckPrm(
    const vdc_channel_t                ch,
    const vdc_imgimprv_id_t            imgimprv_id,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_period_rect_t    * const enh_area);

/******************************************************************************
 * @fn          VDC_ImageBlackStretchCheckPrm
 * @brief       Checks on black stretch setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   param       : Black stretch setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageBlackStretchCheckPrm(
    const vdc_channel_t        ch,
    const vdc_imgimprv_id_t    imgimprv_id,
    const vdc_black_t  * const param);

/******************************************************************************
 * @fn          VDC_AlphaBlendingCheckPrm
 * @brief       Checks on alpha blending setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Alpha blending setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_AlphaBlendingCheckPrm(
    const vdc_channel_t                ch,
    const vdc_layer_id_t               layer_id,
    const vdc_alpha_blending_t * const param);

/******************************************************************************
 * @fn          VDC_AlphaBlendingRectCheckPrm
 * @brief       Checks on setup parameter for alpha blending in a rectangular area
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Setup parameter for alpha blending in a rectangular area
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_AlphaBlendingRectCheckPrm(
    const vdc_channel_t                        ch,
    const vdc_layer_id_t                       layer_id,
    const vdc_alpha_blending_rect_t    * const param);

/******************************************************************************
 * @fn          VDC_ChromakeyCheckPrm
 * @brief       Checks on chroma-key setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Chroma-key setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChromakeyCheckPrm(
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_graphics_type_t      graphics_id,
    const vdc_chromakey_t  * const param);

/******************************************************************************
 * @fn          VDC_CLUTCheckPrm
 * @brief       Checks on CLUT setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : CLUT setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_CLUTCheckPrm(
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_clut_t   * const param);

/******************************************************************************
 * @fn          VDC_DisplayCalibrationCheckPrm
 * @brief       Checks on display calibration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display calibration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_DisplayCalibrationCheckPrm(const vdc_channel_t ch, const vdc_disp_calibration_t * const param);

/******************************************************************************
 * @fn          VDC_GammaCorrectionCheckPrm
 * @brief       Checks on gamma correction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Gamma correction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_GammaCorrectionCheckPrm(const vdc_channel_t ch, const vdc_gamma_correction_t * const param);

#endif      /* R_VDC_CHECK_PARAMETERS */

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_VDC_CHECK_PARAMETER_H */

