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
 * File Name :   r_vdc_shared_param.h
 * @file         r_vdc_shared_param.h
 * @version      1.12
 * @brief        RZ/A2M VDC driver shared parameter definitions
 ******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_vdc.h"
#include "r_vdc_user.h"

#ifndef R_VDC_SHARED_PARAM_H
#define R_VDC_SHARED_PARAM_H

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
/*! Color space */
typedef enum
{
    VDC_COLOR_SPACE_GBR    = 0,        /*!< GBR */
    VDC_COLOR_SPACE_YCBCR  = 1         /*!< YCbCr */
} vdc_color_space_t;

/*! Resource state */
typedef enum
{
    VDC_RESOURCE_ST_INVALID    = 0,    /*!< Invalid */
    VDC_RESOURCE_ST_VALID      = 1     /*!< Valid */
} vdc_resource_state_t;

/*! Resource type */
typedef enum
{
    VDC_RESOURCE_PANEL_CLK = 0,        /*!< Panel clock */
    VDC_RESOURCE_VIDEO_IN,             /*!< Input video */
    VDC_RESOURCE_VSYNC,                /*!< Vsync signal */
    VDC_RESOURCE_LCD_PANEL,            /*!< LCD panel (output video) */
    VDC_RESOURCE_LVDS_CLK,             /*!< LVDS PLL clock */
    VDC_RESOURCE_NUM
} vdc_resource_type_t;

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
 * @fn          VDC_ShrdPrmInit
 * @brief       Initializes variables of the VDC driver
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmInit(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetInitParam
 * @brief       Sets the initialization parameter
 * @param[in]   param                   : Initialization parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetInitParam(const vdc_init_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetTerminate
 * @brief       Sets the termination parameter
 * @param[in]   None
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetTerminate(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetInputParam
 * @brief       Sets the video input setup parameter
 * @param[in]   param                   : Video input setup parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetInputParam(const vdc_input_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetOutputParam
 * @brief       Sets the display output configuration parameter
 * @param[in]   param                   : Display output configuration parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetOutputParam(const vdc_output_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetWriteParam
 * @brief       Sets the data write control parameter
 * @param[in]   param                   : Data write control parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetWriteParam(const vdc_write_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetChgWriteParam
 * @brief       Sets the data write change parameter
 * @param[in]   param                   : Data write change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetChgWriteParam(const vdc_write_chg_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetReadParam
 * @brief       Sets the data read control parameter
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read control parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetReadParam(const vdc_graphics_type_t graphics_id, const vdc_read_t * const param);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetChgReadParam
 * @brief       Sets the data read change parameter
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read change parameter
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetChgReadParam(const vdc_graphics_type_t graphics_id, const vdc_read_chg_t * const param);


/******************************************************************************
 * @fn          VDC_ShrdPrmGetLvdsClkRef
 * @brief       Gets the reference to the LVDS PLL clock
 * @param[in]   void
 * @retval      Reference to the LVDS PLL clock
 *****************************************************************************/
vdc_onoff_t VDC_ShrdPrmGetLvdsClkRef(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetColorSpace
 * @brief       Gets the color space of the input video image signal
 * @param[in]   void
 * @retval      Color space of the input video image signal
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpace(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetBgColor
 * @brief       Get the background color
 * @param[in]   color_space             : Color space
 * @retval      Background color in 24-bit RGB color format or CrYCb format
 *****************************************************************************/
uint32_t VDC_ShrdPrmGetBgColor(const vdc_color_space_t color_space);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetWritingMode
 * @brief       Get the frame buffer writing mode for image processing
 * @param[in]   void
 * @retval      Frame buffer writing mode for image processing
 *****************************************************************************/
vdc_wr_md_t VDC_ShrdPrmGetWritingMode(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetInterlace
 * @brief
 * @param[in]   void
 * @retval      Field operating mode select
 *****************************************************************************/
vdc_res_inter_t VDC_ShrdPrmGetInterlace(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetColorSpaceFbWr
 * @brief       Gets the color space of the frame buffer writing format
 * @param[in]   void
 * @retval      Color space of the frame buffer writing format
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbWr(void);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetFrBuffBtm
 * @brief       Gets the frame buffer base address for bottom
 * @param[in]   void
 * @retval      Frame buffer base address for bottom
 *****************************************************************************/
void * VDC_ShrdPrmGetFrBuffBtm(void);


/******************************************************************************
 * @fn          VDC_ShrdPrmGetLineOfsAddrDir
 * @brief       Gets the line offset address direction of the frame buffer
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Line offset address direction of the frame buffer
 *****************************************************************************/
vdc_gr_ln_off_dir_t VDC_ShrdPrmGetLineOfsAddrDir(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetSelFbAddrSig
 * @brief       Gets the frame buffer address setting signal
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Frame buffer address setting signal
 *****************************************************************************/
vdc_gr_flm_sel_t VDC_ShrdPrmGetSelFbAddrSig(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetGraphicsFormat
 * @brief       Gets the format of the frame buffer read signal
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Format of the frame buffer read signal
 *****************************************************************************/
vdc_gr_format_t VDC_ShrdPrmGetGraphicsFormat(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetColorSpaceFbRd
 * @brief       Gets the color space of the frame buffer reading format
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Color space of the frame buffer reading format
 *****************************************************************************/
vdc_color_space_t VDC_ShrdPrmGetColorSpaceFbRd(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetMeasureFolding
 * @brief       Gets the folding handling
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Folding handling (on/off)
 *****************************************************************************/
vdc_onoff_t VDC_ShrdPrmGetMeasureFolding(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetDisplayArea
 * @brief       Gets the graphics display area
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Graphics display area
 *****************************************************************************/
vdc_period_rect_t * VDC_ShrdPrmGetDisplayArea(const vdc_graphics_type_t graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetFrBuffWidth_Rd
 * @brief       Gets the size of the frame buffer to be read
 * @param[in]   graphics_id             : Graphics type ID
 * @retval      Size of the frame buffer to be read
 *****************************************************************************/
vdc_width_read_fb_t * VDC_ShrdPrmGetFrBuffWidth_Rd(const vdc_graphics_type_t  graphics_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetResource
 * @brief       Updates the resource state
 * @param[in]   rsrc_type               : Resource type
 * @param[in]   rsrc_state              : Resource state
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetResource(const vdc_resource_type_t rsrc_type, const vdc_resource_state_t rsrc_state);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetLayerResource
 * @brief       Updates the layer resource state
 * @param[in]   layer_id                : Layer ID
 * @param[in]   rsrc_state              : Resource state
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetLayerResource(const vdc_layer_id_t layer_id, const vdc_resource_state_t rsrc_state);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetResource
 * @brief       Gets the resource state
 * @param[in]   rsrc_type               : Resource type
 * @retval      Resource state
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetResource(const vdc_resource_type_t rsrc_type);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetLayerResource
 * @brief       Gets the layer resource state
 * @param[in]   layer_id                : Layer ID
 * @retval      Layer resource state
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetLayerResource(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetRwProcEnable
 * @brief       Makes the data write/read processing enabled
 * @param[in]   layer_id                : Layer ID
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetRwProcEnable(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmSetRwProcDisable
 * @brief       Makes the data write/read processing disabled
 * @param[in]   layer_id                : Layer ID
 * @retval      None
 *****************************************************************************/
void VDC_ShrdPrmSetRwProcDisable(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetRwProcReady
 * @brief       Gets the state whether the specified layer is ready or not
 * @param[in]   layer_id                : Layer ID
 * @retval      Resource state
 *              - VDC_RESOURCE_ST_VALID: The layer resource state of the specified layer is valid and
 *                the data write/read processing in the layer is disabled.
 *              - VDC_RESOURCE_ST_INVALID: The specified layer is not ready.
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetRwProcReady(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetRwProcEnabled
 * @brief       Gets the state whether the specified layer is already run or not
 * @param[in]   layer_id                : Layer ID
 * @retval      Resource state
 *              - VDC_RESOURCE_ST_VALID: The layer resource state of the specified layer is valid and
 *                the data write/read processing in the layer is enabled.
 *              - VDC_RESOURCE_ST_INVALID: The specified layer is not enabled.
 *****************************************************************************/
vdc_resource_state_t VDC_ShrdPrmGetRwProcEnabled(const vdc_layer_id_t layer_id);

/******************************************************************************
 * @fn          VDC_ShrdPrmGetLvdsCh
 * @brief       Gets the VDC channel whose data is to be output through the LVDS
 * @param[in]   void
 * @retval      A channel in VDC whose data is to be output through the LVDS
 *****************************************************************************/
vdc_channel_t VDC_ShrdPrmGetLvdsCh(void);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_VDC_SHARED_PARAM_H */

