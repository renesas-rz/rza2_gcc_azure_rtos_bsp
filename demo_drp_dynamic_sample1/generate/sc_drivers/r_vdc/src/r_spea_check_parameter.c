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
 * File Name :   r_spea_check_parameter.c
 * @file         r_spea_check_parameter.c
 * @version      1.12
 * @brief        RZ/A2M SPEA driver parameter check processing
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

#ifdef      R_SPEA_CHECK_PARAMETERS
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
 * Function Name:SPEA_SetWindowCheckPrm
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
spea_error_t 
    SPEA_SetWindowCheckPrm (  
        const vdc_layer_id_t       layer_id,
        const spea_window_id_t     window_id,
        const spea_onoff_t         sken,
        const spea_sklym_t       * sklym,
        const spea_skpsm_t       * skpsm,
        const void               * buffer)

{
    spea_error_t spea_error = SPEA_OK;
    spea_layer_id_t   spea_layer_id;

    spea_error = SPEA_CreateSurfaceIDCheckPrm(layer_id);
    if( SPEA_OK != spea_error)
    {
        goto END;
    }

    if( VDC_LAYER_ID_2_RD == layer_id )
    {
        spea_layer_id = SPEA_LAYER_ID_0;
    }
    else
    {
        spea_layer_id = SPEA_LAYER_ID_1;
    }

    if( (SPEA_OFF != sken) && (SPEA_ON != sken) )
    {
        spea_error = SPEA_ERR_PARAM;
        goto END;
    }

    if( window_id >= WINDOW_NUM )
    {
        spea_error = SPEA_ERR_PARAM;
        goto END;
    }

    /* pointer NULL check */
    if( NULL != skpsm )
    {
        int32_t x;
        int32_t y;

        spea_skpsm_t offset;
        SPEA_get_offset( spea_layer_id, &offset );

        x = skpsm->x + offset.x;
        y = skpsm->y + offset.y;

        if( (1 & x) == 1 )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }

        if( ( x >= 2048 ) || ( 0 > x ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }

        if( ( y >= 8192 ) || ( 0 > y ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }
    }

    /* pointer NULL check */
    if( NULL != sklym )
    {
        int32_t width;
        int32_t height;
        width  = sklym->width;

        if( (1 & width) == 1 )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }

        if( ( width >= 2048 ) || ( 0 > width ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }

        height = sklym->height;
        if( ( height >= 8192 ) || ( 0 > height ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }
    }

    /* pointer NULL check */
    if( NULL != buffer )
    {
        uint32_t address;

        /* Set the System Physical address */
        address = (uint32_t)R_SPEA_CPUVAddrToSysPAddr((uint32_t)buffer);
        if( (address & 0x00000007) != 0 )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }
    }

END:
    return spea_error; 
}   /* End of function SPEA_SetWindowCheckPrm() */

/**************************************************************************//**
 * Function Name:SPEA_WindowOffsetCheckPrm
 * @brief       Checks on offset parameter
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset_x     : x position
 * @param[in]   offset_y     : y position
 * @retval      Error code
 *****************************************************************************/
spea_error_t SPEA_WindowOffsetCheckPrm ( 
    const vdc_layer_id_t       layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y)
{
    spea_error_t spea_error = SPEA_OK;

    spea_error = SPEA_CreateSurfaceIDCheckPrm(layer_id);
    if( SPEA_OK != spea_error)
    {
        goto END;
    }

    if( (1u & offset_x) == 1 )
    {
        spea_error = SPEA_ERR_PARAM;
        goto END;
    }

    if( (2048-1) < offset_x )
    {
        spea_error = SPEA_ERR_PARAM;
        goto END;
    }

    if( (8192-1) < offset_y )
    {
        spea_error = SPEA_ERR_PARAM;
        goto END;
    }
END:
    return spea_error; 
}   /* End of function SPEA_WindowOffsetCheckPrm() */

/**************************************************************************//**
 * Function Name:SPEA_CreateSurfaceIDCheckPrm
 * @brief       Checks on layer ID parameter for SPEA
 * @param[in]   layer_id    : VDC Layer ID (2 or 3)
 * @retval      Error code
 *****************************************************************************/
spea_error_t SPEA_CreateSurfaceIDCheckPrm ( 
    const vdc_layer_id_t      layer_id)
{
    spea_error_t spea_error = SPEA_OK;

    if( (VDC_LAYER_ID_2_RD != layer_id) && (VDC_LAYER_ID_3_RD != layer_id) )
    {
        spea_error = SPEA_ERR_PARAM_LAYER_ID;
    }
    return spea_error;
}   /* End of function SPEA_CreateSurfaceIDCheckPrm() */

/**************************************************************************//**
 * Function Name:RLE_CreateSurfaceIDCheckPrm
 * @brief       Checks on layer ID parameter for RLE
 * @param[in]   layer_id    : VDC Layer ID (0)
 * @retval      Error code
 *****************************************************************************/
spea_error_t RLE_CreateSurfaceIDCheckPrm ( 
    const vdc_layer_id_t      layer_id)
{
    spea_error_t spea_error = SPEA_OK;

    if( VDC_LAYER_ID_0_RD != layer_id )
    {
        spea_error = SPEA_ERR_PARAM_LAYER_ID;
    }
    return spea_error;
}   /* End of function RLE_CreateSurfaceIDCheckPrm() */

/**************************************************************************//**
 * Function Name:RLE_SetWindowCheckPrm
 * @brief       Checks on setting parameter for RLE
 * @param[in]   layer_id        : VDC Layer ID (0)
 * @param[in]   rle_cfg         : RLE configuration
 * @param[in]   buffer          : Window read buffer address
 * @retval      Error code
 *****************************************************************************/
spea_error_t RLE_SetWindowCheckPrm( 
        const vdc_layer_id_t  layer_id,
        const rle_cfg_t     * rle_cfg,
        const void          * buffer)
{
    spea_error_t spea_error = SPEA_OK;

    spea_error = RLE_CreateSurfaceIDCheckPrm(layer_id);
    if( SPEA_OK != spea_error)
    {
        goto END;
    }

    /* pointer NULL check */
    if( NULL != rle_cfg )
    {
        int32_t rlen;
        int32_t rdth;
        rlen  = rle_cfg->rlen;

        if( ( rlen > 7 ) || ( 0 > rlen ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }

        rdth  = rle_cfg->rdth;
        if( ( rdth > 7 ) || ( 0 > rdth ) )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }
    }

    /* pointer NULL check */
    if( NULL != buffer )
    {
        uint32_t address;

        /* Set the System Physical address */
        address = (uint32_t)R_SPEA_CPUVAddrToSysPAddr((uint32_t)buffer);
        if( (address & 0x00000007) != 0 )
        {
            spea_error = SPEA_ERR_PARAM;
            goto END;
        }
    }

END:
    return spea_error; 
}   /* End of function RLE_SetWindowCheckPrm() */

#endif      /* R_SPEA_CHECK_PARAMETERS */
