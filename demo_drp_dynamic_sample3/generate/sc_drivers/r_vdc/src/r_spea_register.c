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
 * File Name :   r_spea_register.c
 * @file         r_spea_register.c
 * @version      1.12
 * @brief        RZ/A2M SPEA driver register setup processing
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_spea.h"
#include    "r_spea_register.h"

#include    "r_spea.h"
#include    "r_spea_user.h"
#include    "r_vdc.h"
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
static int32_t g_offset_x[SPEA_UNIT_NUMBER_INDEX_MAX] = { 0, 0 };
static int32_t g_offset_y[SPEA_UNIT_NUMBER_INDEX_MAX] = { 0, 0 };

/**************************************************************************//**
 * Function Name: SPEA_set_offset
 * @brief       Setting offset position
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset       : offset position
 * @retval      None
 *****************************************************************************/
void SPEA_set_offset( 
    const spea_layer_id_t   layer_id,
    const spea_skpsm_t    * offset )
{
    g_offset_x[layer_id] = offset->x;
    g_offset_y[layer_id] = offset->y;
}   /* End of function SPEA_set_offset() */

/**************************************************************************//**
 * Function Name: SPEA_get_offset
 * @brief       Getting offset position
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset       : offset position
 * @retval      None
 *****************************************************************************/
void SPEA_get_offset( 
    const spea_layer_id_t   layer_id,
    spea_skpsm_t    * offset )
{
    offset->x = g_offset_x[layer_id];
    offset->y = g_offset_y[layer_id];
}   /* End of function SPEA_get_offset() */

/**************************************************************************//**
 * Function Name: SPEA_SetWindow
 * @brief       Setting Window Parameters
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   window_id    : SPEA window ID
 *                              - S0_WINDOW_00 - S0_WINDOW_15
 *                              - S1_WINDOW_00 - S0_WINDOW_15
 * @param[in]   sken         : Window ON/OFF.
 * @param[in]   sklym        : Window size.
 * @param[in]   skpsm        : Window start coordinates.
 * @param[in]   buffer       : Window read buffer address.
 * @retval      None
 *****************************************************************************/
void SPEA_SetWindow( 
    const spea_layer_id_t      layer_id,
    const spea_window_id_t     window_id,
    const spea_onoff_t         sken,
    const spea_sklym_t       * sklym,
    const spea_skpsm_t       * skpsm,
    const void               * buffer)
{
    volatile uint32_t temp;

    /* pointer NULL check */
    if( NULL != buffer )
    {
        /* SPEAnSkDAm
         * **** **** **** **** **** **** **** **** : SPEAnSkDAm[31:0] : dst_address
         */
        *spea_regaddr_unit_ctr[layer_id][window_id].dst_address = R_SPEA_CPUVAddrToSysPAddr((uint32_t)buffer);
    }

    /* pointer NULL check */
    if( NULL != skpsm )
    {
        int32_t temp_x;
        int32_t temp_y;
        spea_skpsm_t offset;

        SPEA_get_offset( layer_id, &offset );

        temp_x = skpsm->x + offset.x;
        temp_y = skpsm->y + offset.y;

        /* Set display position */
        temp = (uint32_t)( (temp_x << 16u )|(temp_y) );

        /* SPEAnSkPSm
         * ---- -*** **** **** ---- ---- ---- ---- : SPEAnSkPSXm[9:0]  : x
         * ---- ---- ---- ---- ---* **** **** **** : SPEAnSkPSYm[12:0] : y
         */
        *spea_regaddr_unit_ctr[layer_id][window_id].position = temp;
    }

    /* pointer NULL check */
    if( NULL != sklym )
    {
        /* SPEAnSkLYm
         * ---- -*** **** **** ---- ---- ---- ---- : SPEAnSkLYWm[9:0]  : width
         * ---- ---- ---- ---- ---- -*** **** **** : SPEAnSkLYHm[10:0] : height
         */
        temp = (uint32_t)( (sklym->width << 16u )|(sklym->height) );
        *spea_regaddr_unit_ctr[layer_id][window_id].width_hight = temp;
    }

    if( SPEA_ON == sken )
    {
        /* SPEAnSkEN */
        (*spea_regaddr_unit_enable_ctrl[layer_id].enable) |= (uint32_t)(0x00000001 << window_id);
    }
    else
    {
        /* SPEAnSkDS */
        (*spea_regaddr_unit_enable_ctrl[layer_id].disable) |= ((uint32_t)(0x00000001 << window_id));
    }

}   /* End of function SPEA_SetWindow() */

/**************************************************************************//**
 * Function Name: SPEA_WindowUpdate
 * @brief       Window parameters update request
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @retval      None
 *****************************************************************************/
void SPEA_WindowUpdate( 
    const spea_layer_id_t      layer_id)
{
    /* SPEAnSkUP
     * ---- ---- ---- ---- ---- ---- ---- ---* : SPEAnSkUP0 : 1
     */
    *spea_regaddr_unit_update_ctrl[layer_id].update = 0x00000001u;

    /* GR_UPDATE
     * ---- ---- ---- ---- ---- ---- ---* ---- : GR_P_VEN    : 1
     * ---- ---- ---- ---- ---- ---- ---- ---* : GR_IBUS_VEN : 1
     */
    *spea_regaddr_img_synthesizer[layer_id].gr_update = 0x00000011;

}   /* End of function SPEA_WindowUpdate() */

/**************************************************************************//**
 * Function Name: SPEA_WindowOffset
 * @brief       Setting offset position
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @param[in]   offset_x     : x position
 * @param[in]   offset_y     : y position
 * @retval      None
 *****************************************************************************/
void SPEA_WindowOffset( 
    const spea_layer_id_t      layer_id,
    const uint16_t             offset_x,
    const uint16_t             offset_y)
{
    volatile uint32_t temp_address;
    spea_skpsm_t offset;

    /* Set frame base address */
    temp_address = (uint32_t)(VIRTUAL_FRAME_BASE_ADD + ((VIRTUAL_FRAME_STRAID * offset_y) + (uint32_t)(offset_x *4) ));

    /* GR_FLM2
     * **** **** **** ****  **** **** **** **** : GR_BASE[31:0]
     */
    *spea_regaddr_img_synthesizer[layer_id].gr_flm2 = temp_address;

    /* save off set */
    offset.x = offset_x;
    offset.y = offset_y;
    SPEA_set_offset(layer_id, &offset);

}   /* End of function SPEA_WindowOffset() */

/**************************************************************************//**
 * Function Name: RLE_SetWindow
 * @brief       Setting Window Parameters
 * @param[in]   rbussel      : RLE ON/OFF.
 * @param[in]   rle_cfg      : RLE configuration.
 * @param[in]   buffer       : Window read buffer address.
 * @retval      None
 *****************************************************************************/
void RLE_SetWindow( 
    const rle_onoff_t    rbussel,
    const rle_cfg_t    * rle_cfg,
    const void         * buffer)
{
    volatile uint32_t temp;
    volatile uint32_t temp_address;

    /* pointer NULL check */
    if( NULL != buffer )
    {
        /* Set frame base address */
        temp_address = R_SPEA_CPUVAddrToSysPAddr((uint32_t)buffer);

        /* SPEAnSTAi
         * **** **** **** **** **** **** **** **** : SPEAnRSTA[31:0] : start_address
         */
        *rle_regaddr.start_address = temp_address;

        /* SPEAnPHAi
         * **** **** **** **** **** **** **** **** : SPEAnRPHA[31:0] : physical_address
         */
        *rle_regaddr.physical_address = temp_address;

        /* GR_FLM2
         * **** **** **** ****  **** **** **** **** : GR_BASE[31:0]
         */
        *rle_regaddr_img_synthesizer.gr_flm2 = temp_address;

        /* SPEAnRCMi
         * ---- ---- ---- ---- ---- ---- ---- --** : SPEAnRCMi[1:0] : color
         */
        *rle_regaddr.color = 0x00000002;
    }

    /* pointer NULL check */
    if( NULL != rle_cfg )
    {
        /* SPEAnSkLYm
         * ---- -*** **** **** ---- ---- ---- ---- : SPEAnSkLYWm[9:0]  : width
         * ---- ---- ---- ---- ---- -*** **** **** : SPEAnSkLYHm[10:0] : height
         */
        temp = (uint32_t)( (rle_cfg->rlen << 4u )|(rle_cfg->rdth) );
        *rle_regaddr.rlen_rdth = temp;
    }

    if( RLE_ON == rbussel )
    {
        /* SPEAnRLSL
         * ---- ---- ---- ---- ---- ---- ---- ---* : SPEAnRBUSSEL : enable
         */
        (*rle_regaddr.enable) |= 0x00000001;

        /* GR_FLM1
         * ---- ---- ---- ---- ---- ---- ---- ---* : GR0_BST_MD
         */
        (*rle_regaddr_img_synthesizer.gr_flm1) |= 0x00000001;
    }
    else
    {
        /* SPEAnRLSL
         * ---- ---- ---- ---- ---- ---- ---- ---* : SPEAnRBUSSEL : disable
         */
        (*rle_regaddr.enable) &= (uint32_t)~(0x00000001);

        /* GR_FLM1
         * ---- ---- ---- ---- ---- ---- ---- ---* : GR0_BST_MD
         */
        (*rle_regaddr_img_synthesizer.gr_flm1) &= (uint32_t)~(0x00000001);
    }

}   /* End of function RLE_SetWindow() */

/**************************************************************************//**
 * Function Name: RLE_WindowUpdate
 * @brief       RLE parameters update request
 * @param[in]   layer_id     : VDC Layer ID (2 or 3)
 * @retval      None
 *****************************************************************************/
void RLE_WindowUpdate(void) 
{
    /* SPEAnRUP
     * ---- ---- ---- ---- ---- ---- ---- ---* : SPEAnRUP0 : 1
     */
    *rle_regaddr.update = 0x00000001u;

    /* GR_UPDATE
     * ---- ---- ---- ---- ---- ---- ---* ---- : GR_P_VEN    : 1
     * ---- ---- ---- ---- ---- ---- ---- ---* : GR_IBUS_VEN : 1
     */
    *rle_regaddr_img_synthesizer.gr_update = 0x00000011;
    *rle_regaddr_img_synthesizer.sc0_scl1_update = 0x00000010;

}   /* End of function RLE_WindowUpdate() */
