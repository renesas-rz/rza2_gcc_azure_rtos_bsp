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
 * File Name :     r_rvapi_spea.h
 * @headerfile     r_rvapi_spea.h
 * @brief          RVAPI for SPEA function header
 * @version        1.12
 * H/W Platform    RZ/A2M
 *****************************************************************************/
/**************************************************************************//**
 * @ingroup R_SW_PKG_93_VIDEO_API
 * @defgroup R_SW_PKG_93_VIDEO_VDC Video SPEA
 * @brief SPEA Unit Module
 *
 * @anchor R_SW_PKG_93_VIDEO_SPEA_API_SUMMARY
 * @par Summary
 *
 * This middleware module provides functionality for a user to utilise the
 * Sprite Engine Controller (SPEA) for graphical/video output onto a display.
 * Configuration settings range from enabling interrupts to alpha blending.
 *
 * @anchor R_SW_PKG_93_VIDEO_SPEA_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_vdc.h"
#include "r_spea.h"

/* Multiple inclusion prevention macro */
#ifndef R_RVAPI_SPEA_H
#define R_RVAPI_SPEA_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
/* SPEA initialization */

/**
 * @fn          R_RVAPI_GraphCreateSurfaceSPEA
 * @brief       Graphics create surface processing
 * @param[in]   ch            : Channel
 * @param[in]   gr_disp_cnf   : Graphics surface read config
 * @retval      VDC driver error code
 */
vdc_error_t R_RVAPI_GraphCreateSurfaceSPEA(const vdc_channel_t ch,
        const gr_surface_disp_config_t * const gr_disp_cnf);

/**
 * @fn          R_RVAPI_WindowOffsetSPEA
 * @brief       Setting the coordinate offset of the window
 * @param[in]   layer_id  : VDC Layer ID (2 or 3)
 * @param[in]   offset_x  : x position
 * @param[in]   offset_y  : y position
 * @retval      SPEA driver error code
 */
spea_error_t R_RVAPI_WindowOffsetSPEA(const vdc_layer_id_t layer_id,
        const uint16_t offset_x, const uint16_t offset_y);

/**
 * @fn          R_RVAPI_SetWindowSPEA
 * @brief       Setting Window Parameters
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   window_id    : SPEA window ID
 *                              - S0_WINDOW_00 - S0_WINDOW_15
 *                              - S1_WINDOW_00 - S0_WINDOW_15
 * @param[in]   sken         : Window ON/OFF.
 * @param[in]   size         : Window size.
 * @param[in]   pos        : Window start coordinates.
 * @param[in]   buffer       : Window read buffer address.
 * @retval      SPEA driver error code
 */
spea_error_t R_RVAPI_SetWindowSPEA(const vdc_layer_id_t layer_id,
                                    const spea_window_id_t window_id,
                                    const spea_onoff_t sken,
                                    const spea_sklym_t * size,
                                    const spea_skpsm_t * pos,
                                    const void * buffer);

/**
 * @fn          R_RVAPI_WindowUpdateSPEA
 * @brief       Window parameters update request
 * @param[in]   layer_id        : VDC Layer ID (2 or 3)
 * @retval      SPEA driver error code
 */
spea_error_t R_RVAPI_WindowUpdateSPEA(const vdc_layer_id_t layer_id);

/**
 * @fn          R_RVAPI_GraphCreateSurfaceRLE
 * @brief       Graphics create surface processing
 * @param[in]   ch            : Channel
 * @param[in]   gr_disp_cnf   : Graphics surface read config
 * @retval      VDC driver error code
 */
vdc_error_t R_RVAPI_GraphCreateSurfaceRLE(const vdc_channel_t ch,
        const gr_surface_disp_config_t * const gr_disp_cnf);

/**
 * @fn          R_RVAPI_SetWindowRLE
 * @brief       Setting Window Parameters
 * @param[in]   ch            : Channel
 * @param[in]   sken          : RLE ON/OFF
 * @param[in]   rle_cfg       : Set NULL(TBD)
 * @param[in]   buffer        : Window read buffer address
 * @param[in]   g_rle_image   : image data
 * @param[in]   size_of_image : size of image
 * @retval      VDC driver error code
 */
vdc_error_t R_RVAPI_SetWindowRLE(const vdc_channel_t ch,
                                const rle_onoff_t sken,
                                const rle_cfg_t * rle_cfg,
                                const void * buffer,
                                const uint8_t * g_rle_image,
                                uint32_t size_of_image);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_RVAPI_SPEA_H */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
