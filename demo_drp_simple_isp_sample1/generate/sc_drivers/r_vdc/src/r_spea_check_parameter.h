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
 * File Name :   r_spea_check_parameter.h
 * @file         r_spea_check_parameter.h
 * @version      1.12
 * @brief        RZ/A2M SPEA driver parameter check definitions
 ******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_spea.h"
#include "r_vdc.h"

#ifndef R_SPEA_CHECK_PARAMETER_H
#define R_SPEA_CHECK_PARAMETER_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

#ifdef      R_SPEA_CHECK_PARAMETERS
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
/*--- SPEA function --------------------------------------*/

/******************************************************************************
 * @fn          SPEA_SetWindowCheckPrm
 * @brief       Checks on setting parameter for SPEA
 * @param[in]   layer_id        : VDC Layer ID (2 or 3)
 * @param[in]   window_id       : SPEA window ID
                                   - S0_WINDOW_00 - S0_WINDOW_15
                                   - S1_WINDOW_00 - S1_WINDOW_15
 * @param[in]   sken            : Window ON/OFF.
 * @param[in]   sklym           : Window size.
 * @param[in]   skpsm           : Window start coordinates.
 * @param[in]   buffer          : Window read buffer address
 * @retval      Error code
 *****************************************************************************/
spea_error_t SPEA_SetWindowCheckPrm (
    const vdc_layer_id_t       layer_id,
    const spea_window_id_t     window_id,
    const spea_onoff_t         sken,
    const spea_sklym_t       * sklym,
    const spea_skpsm_t       * skpsm,
    const void               * buffer);

/******************************************************************************
 * @fn          SPEA_WindowOffsetCheckPrm
 * @brief       Checks on offset parameter
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset_x     : x position
 * @param[in]   offset_y     : y position
 * @retval      Error code
 *****************************************************************************/
spea_error_t SPEA_WindowOffsetCheckPrm (
    const vdc_layer_id_t       layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y);

/******************************************************************************
 * @fn          SPEA_CreateSurfaceIDCheckPrm
 * @brief       Checks on layer ID parameter for SPEA
 * @param[in]   layer_id    : VDC Layer ID (2 or 3)
 * @retval      Error code
 *****************************************************************************/
spea_error_t SPEA_CreateSurfaceIDCheckPrm (
    const vdc_layer_id_t      layer_id);

/*--- RLE function --------------------------------------*/

/******************************************************************************
 * @fn          RLE_CreateSurfaceIDCheckPrm
 * @brief       Checks on layer ID parameter for RLE
 * @param[in]   layer_id    : VDC Layer ID (0)
 * @retval      Error code
 *****************************************************************************/
spea_error_t RLE_CreateSurfaceIDCheckPrm (
    const vdc_layer_id_t      layer_id);

/******************************************************************************
 * @fn          RLE_SetWindowCheckPrm
 * @brief       Checks on setting parameter for RLE
 * @param[in]   layer_id        : VDC Layer ID (0)
 * @param[in]   rle_cfg         : RLE configuration
 * @param[in]   buffer          : Window read buffer address
 * @retval      Error code
 *****************************************************************************/
spea_error_t RLE_SetWindowCheckPrm(
        const vdc_layer_id_t  layer_id,
        const rle_cfg_t     * rle_cfg,
        const void          * buffer);

#endif      /* R_SPEA_CHECK_PARAMETERS */

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_SPEA_CHECK_PARAMETER_H */
