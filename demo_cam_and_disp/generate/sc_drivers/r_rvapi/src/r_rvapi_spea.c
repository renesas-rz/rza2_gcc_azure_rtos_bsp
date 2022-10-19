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
 * File Name :   r_rvapi_spea.c
 * @file         r_rvapi_spea.c
 * @version      1.12
 * @brief        RVAPI for SPEA function
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include    <string.h>

#include    "r_vdc.h"
#include    "r_spea.h"
#include    "r_rvapi_vdc.h"
#include    "r_rvapi_spea.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define RLE_STRIDE    (4)

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global variables
 ******************************************************************************/
extern vdc_period_rect_t full_screen; 

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphCreateSurfaceSPEA
 * @brief       Graphics create surface processing
 * @param[in]   ch            : Channel
 * @param[in]   gr_disp_cnf   : Graphics surface read config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_GraphCreateSurfaceSPEA(const vdc_channel_t ch, 
            const gr_surface_disp_config_t * const gr_disp_cnf)
{
    vdc_error_t error;
    vdc_read_t spea_cnf;
    vdc_start_t start;
    vdc_gr_disp_sel_t disp_mode;

    /* pointer_NULL_check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Read data parameter */
        spea_cnf.gr_ln_off_dir
            = VDC_GR_LN_OFF_DIR_INC;       /* Line offset address direction of the frame buffer */
        spea_cnf.gr_flm_sel     = VDC_GR_FLM_SEL_FLM_NUM;          /* Selects a frame buffer address setting signal */
        spea_cnf.gr_imr_flm_inv = VDC_OFF;                         /* Frame buffer number for distortion correction */
        spea_cnf.gr_bst_md      = VDC_BST_MD_32BYTE;               /* Frame buffer burst transfer mode */
        spea_cnf.gr_base        = (void *) VIRTUAL_FRAME_BASE_ADD; /* Frame buffer base address */
        spea_cnf.gr_ln_off      = VIRTUAL_FRAME_STRAID;            /* Frame buffer line offset address */
        spea_cnf.width_read_fb  = NULL;                            /* Width of the image read from frame buffer */
        spea_cnf.adj_sel
            = VDC_ON;                      /* Measures to decrease the influence by folding pixels/lines (on/off) */
        spea_cnf.gr_format      = VDC_GR_FORMAT_ARGB8888;          /* Format of the frame buffer read signal */
        spea_cnf.gr_ycc_swap
            = VDC_GR_YCCSWAP_CBY0CRY1;         /* Controls swapping of data read from buffer in the YCbCr422 format */
        spea_cnf.gr_rdswa       = VDC_WR_RD_WRSWA_32BIT;           /* Frame buffer swap setting */

        /* Display area */
        spea_cnf.gr_grc.vs = (uint16_t) (gr_disp_cnf->disp_area.vs_rel + full_screen.vs); /* vertical start position */
        spea_cnf.gr_grc.vw = (uint16_t) gr_disp_cnf->disp_area.vw_rel;                    /* vertical display size */
        spea_cnf.gr_grc.hs = (uint16_t) (gr_disp_cnf->disp_area.hs_rel + full_screen.hs); /*horizontal start position*/
        spea_cnf.gr_grc.hw = (uint16_t) gr_disp_cnf->disp_area.hw_rel;                    /* horizontal display size */

        disp_mode = VDC_DISPSEL_BLEND;
        start.gr_disp_sel = &disp_mode;

        error = R_VDC_ReadDataControl(ch, gr_disp_cnf->layer_id, &spea_cnf);
    }

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if(error == VDC_OK)
        {
            error = R_VDC_StartProcess(ch, gr_disp_cnf->layer_id, &start);
        }
    }

    return error;
} /* End of function R_RVAPI_GraphCreateSurfaceSPEA() */

/**************************************************************************//**
 * Function Name : R_RVAPI_WindowOffsetSPEA
 * @brief       Setting the coordinate offset of the window
 * @param[in]   layer_id  : VDC Layer ID (2 or 3)
 * @param[in]   offset_x  : x position
 * @param[in]   offset_y  : y position
 * @retval      SPEA driver error code
 ******************************************************************************/
spea_error_t R_RVAPI_WindowOffsetSPEA
            (const vdc_layer_id_t layer_id, const uint16_t offset_x, const uint16_t offset_y)
{
    spea_error_t spea_error;

    spea_error = R_SPEA_WindowOffset(layer_id, offset_x, offset_y);

    return spea_error;
} /* End of function R_RVAPI_WindowOffsetSPEA() */

/**************************************************************************//**
 * Function Name : R_RVAPI_SetWindowSPEA
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
 ******************************************************************************/
spea_error_t R_RVAPI_SetWindowSPEA(const vdc_layer_id_t layer_id, 
                const spea_window_id_t window_id,
                const spea_onoff_t sken,
                const spea_sklym_t * size,
                const spea_skpsm_t * pos,
                const void * buffer)
{
    spea_error_t spea_error;

    spea_error = R_SPEA_SetWindow(layer_id, window_id, sken, size, pos, buffer);

    return spea_error;
} /* End of function R_RVAPI_SetWindowSPEA() */

/**************************************************************************//**
 * Function Name : R_RVAPI_WindowUpdateSPEA
 * @brief       Window parameters update request
 * @param[in]   layer_id        : VDC Layer ID (2 or 3)
 * @retval      SPEA driver error code
 ******************************************************************************/
spea_error_t R_RVAPI_WindowUpdateSPEA(const vdc_layer_id_t layer_id) 
{
    spea_error_t spea_error;

    spea_error = R_SPEA_WindowUpdate(layer_id);

    return spea_error;
} /* End of function R_RVAPI_WindowUpdateSPEA() */

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphCreateSurfaceRLE
 * @brief       Graphics create surface processing
 * @param[in]   ch            : Channel
 * @param[in]   gr_disp_cnf   : Graphics surface read config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_GraphCreateSurfaceRLE(const vdc_channel_t ch, 
                const gr_surface_disp_config_t * const gr_disp_cnf)
{
    vdc_error_t error;
    vdc_read_t spea_cnf;
    vdc_start_t start;
    vdc_gr_disp_sel_t disp_mode;

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Read data parameter */
        spea_cnf.gr_ln_off_dir  = VDC_GR_LN_OFF_DIR_INC;  /* Line offset address direction of the frame buffer */
        spea_cnf.gr_flm_sel     = VDC_GR_FLM_SEL_FLM_NUM; /* Selects a frame buffer address setting signal */
        spea_cnf.gr_imr_flm_inv = VDC_OFF;                /* Frame buffer number for distortion correction */
        spea_cnf.gr_bst_md      = VDC_BST_MD_32BYTE;      /* Frame buffer burst transfer mode */
        spea_cnf.gr_base        = (void *)gr_disp_cnf->fb_buff;     /* Frame buffer base address */
        spea_cnf.gr_ln_off /* Cast to an appropriate type */
            = ((uint16_t)gr_disp_cnf->disp_area.hw_rel * RLE_STRIDE); /* Frame buffer line offset address */

        /* Cast to an appropriate type */
        spea_cnf.width_read_fb  = NULL;                   /* Width of the image read from frame buffer */
        spea_cnf.adj_sel
            = VDC_ON;                 /* Measures to decrease the influence by folding pixels/lines (on/off) */
        spea_cnf.gr_format      = VDC_GR_FORMAT_RGB888;   /* Format of the frame buffer read signal */
        spea_cnf.gr_ycc_swap
            = VDC_GR_YCCSWAP_CBY0CRY1;/* Controls swapping of data read from buffer in the YCbCr422 format */
        spea_cnf.gr_rdswa       = VDC_WR_RD_WRSWA_32BIT;  /* Frame buffer swap setting */

        /* Display area */
        spea_cnf.gr_grc.vs = (uint16_t) (gr_disp_cnf->disp_area.vs_rel + full_screen.vs);/* vertical start position */
        spea_cnf.gr_grc.vw = (uint16_t) gr_disp_cnf->disp_area.vw_rel;                   /* vertical display size */
        spea_cnf.gr_grc.hs = (uint16_t) (gr_disp_cnf->disp_area.hs_rel + full_screen.hs);/* horizontal start position*/
        spea_cnf.gr_grc.hw = (uint16_t) gr_disp_cnf->disp_area.hw_rel;                   /* horizontal display size */

        disp_mode = VDC_DISPSEL_CURRENT;
        start.gr_disp_sel = &disp_mode;

        error = R_VDC_ReadDataControl(ch, VDC_LAYER_ID_0_RD, &spea_cnf);
    }

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if(VDC_OK == error)
        {
            error = R_VDC_StartProcess(ch, VDC_LAYER_ID_0_RD, &start);
        }
    }

    return error;
}  /* End of function R_RVAPI_GraphCreateSurfaceRLE() */

/**************************************************************************//**
 * Function Name : R_RVAPI_SetWindowRLE
 * @brief       Setting Window Parameters
 * @param[in]   ch            : Channel
 * @param[in]   sken          : RLE ON/OFF
 * @param[in]   rle_cfg       : Set NULL(TBD)
 * @param[in]   buffer        : Window read buffer address
 * @param[in]   g_rle_image   : image data
 * @param[in]   size_of_image : size of image
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_SetWindowRLE(const vdc_channel_t ch, 
                                const rle_onoff_t sken,
                                const rle_cfg_t * rle_cfg,
                                const void * buffer,
                                const uint8_t * g_rle_image,
                                uint32_t size_of_image)
{
    vdc_error_t error = VDC_OK;
    spea_error_t spea_error = SPEA_OK;
    vdc_read_chg_t read_chg;
    uint32_t image_size;
    uint16_t width_check;

    /* Set read_chg */
    read_chg.gr_base = (void *)buffer;  /* Set buffer */
    read_chg.gr_disp_sel = NULL;        /* Set NULL */
    read_chg.gr_grc = NULL;             /* Set NULL */
    read_chg.width_read_fb = NULL;      /* Set NULL */

    /* alignment check 32pixel */
    width_check = (uint16_t)((g_rle_image[13] << 0x08)|g_rle_image[12]);
    if((width_check % 32) != 0)
    {
        spea_error = SPEA_ERR_PARAM;
    }

    /* image type check fullcolor(RLE) */
    if(g_rle_image[2] != 0x0A)
    {
        spea_error = SPEA_ERR_PARAM;
    }

    /* pixel size check 24bit(RGB888) */
    if(g_rle_image[16] != 0x18)
    {
        spea_error = SPEA_ERR_PARAM;
    }

    if(SPEA_OK == spea_error)
    {
        image_size = size_of_image - 18;

        /* Copy Targa image file */
        memcpy((void *)buffer, &g_rle_image[18], image_size);

        spea_error = R_RLE_SetWindow(VDC_LAYER_ID_0_RD, sken, rle_cfg, buffer);

        if(SPEA_OK == spea_error)
        {
            spea_error = R_RLE_WindowUpdate(VDC_LAYER_ID_0_RD);

            if(SPEA_OK == spea_error)
            {
                error = R_VDC_ChangeReadProcess(ch, VDC_LAYER_ID_0_RD, &read_chg);
            }
            else
            {
                error = VDC_ERR_PARAM_CONDITION;
            }
        }
        else
        {
            error = VDC_ERR_PARAM_CONDITION;
        }
    }
    else
    {
        error = VDC_ERR_PARAM_CONDITION;
    }

    return error;
} /* End of function R_RVAPI_SetWindowRLE() */

