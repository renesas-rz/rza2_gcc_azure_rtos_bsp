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
 * File Name :   r_spea.c
 * @file         r_spea.c
 * @version      1.12
 * @brief        RZ/A2M SPEA driver API function
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_spea.h"

#include    "r_spea_user.h"
#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_register.h"
#include    "r_vdc_shared_param.h"
#include    "r_vdc_check_parameter.h"

#include    "r_spea_register.h"
#include    "r_spea_check_parameter.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * Function Name:R_SPEA_SetWindow
 * @brief       Setting Window Parameters
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - SPEA Window ON / OFF
 *              - Set window starting coordinates and size and read buffer address to SPEA.
 *              - VDC frame buffer burst transfer mode setting (SPEA_ON: 128 bytes SPEA_OFF: 32 byte transfer).
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   window_id    : SPEA window ID
 *                              - S0_WINDOW_00 - S0_WINDOW_15
 *                              - S1_WINDOW_00 - S0_WINDOW_15
 * @param[in]   sken         : Window ON/OFF.
 * @param[in]   sklym        : Window size.
 * @param[in]   skpsm        : Window start coordinates.
 * @param[in]   buffer       : Window read buffer address.
 * @retval      Error code
 *****************************************************************************/
spea_error_t R_SPEA_SetWindow( 
    const vdc_layer_id_t       layer_id,
    const spea_window_id_t     window_id,
    const spea_onoff_t         sken,
    const spea_sklym_t       * sklym,
    const spea_skpsm_t       * skpsm,
    const void               * buffer)

{
    spea_error_t    spea_error    = SPEA_OK;
    spea_layer_id_t spea_layer_id = SPEA_LAYER_ID_0;

#ifdef  R_SPEA_CHECK_PARAMETERS
    spea_error = SPEA_SetWindowCheckPrm(layer_id, window_id, sken, sklym, skpsm, buffer );
#endif  /* R_SPEA_CHECK_PARAMETERS */

    if ( SPEA_OK == spea_error )
    {
        if( VDC_LAYER_ID_2_RD == layer_id )
        {
            spea_layer_id = SPEA_LAYER_ID_0;
        }
        else
        {
            spea_layer_id = SPEA_LAYER_ID_1;
        }
        SPEA_SetWindow(spea_layer_id, window_id, sken, sklym, skpsm, buffer);
    }
    else
    {
        spea_error = SPEA_ERR_PARAM;
    }
    return spea_error;
}   /* End of function R_SPEA_SetWindow() */

/**************************************************************************//**
 * Function Name:R_SPEA_WindowOffset
 * @brief       Setting the coordinate offset of the window
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              Set the arrangement of VDC (layers 2 and 3) display area on virtual frame of SPEA.
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset_x     : x position
 * @param[in]   offset_y     : y position
 * @retval      Error code
 *****************************************************************************/
spea_error_t R_SPEA_WindowOffset( 
        const vdc_layer_id_t       layer_id,
        const uint16_t             offset_x,
        const uint16_t             offset_y)
{
    spea_error_t    spea_error    = SPEA_OK;
    spea_layer_id_t spea_layer_id = SPEA_LAYER_ID_0;

#ifdef  R_SPEA_CHECK_PARAMETERS
    spea_error = SPEA_WindowOffsetCheckPrm( layer_id, offset_x, offset_y );
#endif  /* R_SPEA_CHECK_PARAMETERS */

    if ( SPEA_OK == spea_error )
    {
        if( VDC_LAYER_ID_2_RD == layer_id )
        {
            spea_layer_id = SPEA_LAYER_ID_0;
        }
        else
        {
            spea_layer_id = SPEA_LAYER_ID_1;
        }
        SPEA_WindowOffset(spea_layer_id, offset_x, offset_y );
    }
    else
    {
        spea_error = SPEA_ERR_PARAM;
    }
    return spea_error;
}   /* End of function R_SPEA_WindowOffset() */

/**************************************************************************//**
 * Function Name:R_SPEA_WindowUpdate
 * @brief       Window parameter update request
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              Request to update SPEA Window parameters.
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @retval      Error code
 *****************************************************************************/
spea_error_t R_SPEA_WindowUpdate( 
    const vdc_layer_id_t         layer_id)
{
    spea_error_t    spea_error    = SPEA_OK;
    spea_layer_id_t spea_layer_id = SPEA_LAYER_ID_0;

#ifdef  R_SPEA_CHECK_PARAMETERS
    spea_error = SPEA_CreateSurfaceIDCheckPrm(layer_id);
#endif  /* R_SPEA_CHECK_PARAMETERS */

    if ( SPEA_OK == spea_error )
    {
        if( VDC_LAYER_ID_2_RD == layer_id )
        {
            spea_layer_id = SPEA_LAYER_ID_0;
        }
        else
        {
            spea_layer_id = SPEA_LAYER_ID_1;
        }
        SPEA_WindowUpdate(spea_layer_id);
    }
    else
    {
        spea_error = SPEA_ERR_PARAM;
    }
    return spea_error;
}   /* End of function R_SPEA_WindowUpdate() */

/**************************************************************************//**
 * Function Name:R_RLE_SetWindow
 * @brief       Setting the RLE parameter
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - RLE ON /OFF.
 *              - Setting SPEA RLE parameters.
 * @param[in]   layer_id     : VDC Layer ID (0)
 * @param[in]   sken         : RLE ON / OFF
 * @param[in]   rle_cfg      : Set  NULL(TBD)
 * @param[in]   buffer       : Window read buffer address.
 * @retval      Error code
 *****************************************************************************/
spea_error_t R_RLE_SetWindow( 
    const vdc_error_t    layer_id,
    const rle_onoff_t    sken,
    const rle_cfg_t    * rle_cfg,
    const void         * buffer)
{
    spea_error_t    spea_error = SPEA_OK;

#ifdef  R_SPEA_CHECK_PARAMETERS
    spea_error = RLE_SetWindowCheckPrm(layer_id, rle_cfg, buffer);
#endif  /* R_SPEA_CHECK_PARAMETERS */

    if ( SPEA_OK == spea_error )
    {
        RLE_SetWindow( sken, rle_cfg, buffer);
    }
    else
    {
        spea_error = SPEA_ERR_PARAM;
    }
    return spea_error;
}   /* End of function R_RLE_SetWindow() */

/**************************************************************************//**
 * Function Name:R_RLE_WindowUpdate
 * @brief       RLE parameter update request
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              -Request to update RLE parameters.
 * @param[in]   layer_id     : VDC Layer ID (0)
 * @retval      Error code
 *****************************************************************************/
spea_error_t R_RLE_WindowUpdate( 
    const vdc_layer_id_t   layer_id)
{
    spea_error_t    spea_error = SPEA_OK;
#ifdef  R_SPEA_CHECK_PARAMETERS
    spea_error = RLE_CreateSurfaceIDCheckPrm(layer_id);
#endif  /* R_SPEA_CHECK_PARAMETERS */

    if ( SPEA_OK == spea_error )
    {
        RLE_WindowUpdate();
    }
    else
    {
        spea_error = SPEA_ERR_PARAM;
    }
    return spea_error;
}   /* End of function R_RLE_WindowUpdate() */
