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
 * File Name :   r_vdc_check_parameter.c
 * @file         r_vdc_check_parameter.c
 * @version      1.12
 * @brief        RZ/A2M VDC driver parameter check processing
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_shared_param.h"
#include    "r_vdc_check_parameter.h"

#ifdef      R_VDC_CHECK_PARAMETERS
/******************************************************************************
Macro definitions
******************************************************************************/
/* Valid bit range */
#define VDC_CPARA_RANGE_0X00000001 (0x00000001u)
#define VDC_CPARA_RANGE_0X00000003 (0x00000003u)
#define VDC_CPARA_RANGE_0X00000007 (0x00000007u)
#define VDC_CPARA_RANGE_0X0000000F (0x0000000Fu)
#define VDC_CPARA_RANGE_0X0000001F (0x0000001Fu)
#define VDC_CPARA_RANGE_0X0000003F (0x0000003Fu)
#define VDC_CPARA_RANGE_0X0000007F (0x0000007Fu)
#define VDC_CPARA_RANGE_0X000000FF (0x000000FFu)
#define VDC_CPARA_RANGE_0X000003FF (0x000003FFu)
#define VDC_CPARA_RANGE_0X000007FF (0x000007FFu)
#define VDC_CPARA_RANGE_0X00000FFF (0x00000FFFu)
#define VDC_CPARA_RANGE_0X00007FFF (0x00007FFFu)
#define VDC_CPARA_RANGE_0X0000FFFF (0x0000FFFFu)
#define VDC_CPARA_RANGE_0X007FFFFF (0x007FFFFFu)
#define VDC_CPARA_RANGE_0X00FFFFFF (0x00FFFFFFu)
/* Valid range for signal delay amount */
#define VDC_CPARA_INP_FLD_DLY_MAX (254u)
#define VDC_CPARA_INP_VS_DLY_MAX  (254u)
#define VDC_CPARA_INP_HS_DLY_MAX  (254u)
/* Valid range for HS + HW */
#define VDC_CPARA_HS_HW_MAX       (2015u)
/* Valid range for HS */
#define VDC_CPARA_HS_MIN          (16u)
/* Valid range for VS + VW */
#define VDC_CPARA_VS_VW_MAX       (2039u)
/* Valid range for VS */
#define VDC_CPARA_VS_MIN          (4u)
/* The max number of CLUT data values */
#define VDC_CPARA_CLUT8_NUM       (256u)
#define VDC_CPARA_CLUT4_NUM       (16u)
#define VDC_CPARA_CLUT1_NUM       (2u)
/* Valid range for the alpha coefficient */
#define VDC_CPARA_GR_ARC_COEF_MIN (-255)
#define VDC_CPARA_GR_ARC_COEF_MAX (255)
/* Valid range for HS of enhancer area */
#define VDC_CPARA_ENH_HS_LIMIT    (4u)
/* Valid range for VS of enhancer area */
#define VDC_CPARA_ENH_VS_LIMIT    (2u)

#define VDC_BKSTR_T2_PROHIBITED    (31u)

#define VDC_MULTIPLE_4_MASK        (0x00000003u)
#define VDC_MULTIPLE_32_MASK       (0x0000001Fu)
#define VDC_MULTIPLE_128_MASK      (0x0000007Fu)

#define VDC_LVDSPLL_FD_MIN        (22u)
#define VDC_LVDSPLL_FD_MAX        (62u)
#define VDC_LVDSPLL_RD_MAX        (7u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static vdc_error_t CheckChannel(const vdc_channel_t ch);
static vdc_error_t CheckLayerId(const vdc_layer_id_t layer_id);
static vdc_error_t CheckGraphicsTypeLayerId(const vdc_layer_id_t layer_id);
static vdc_error_t CheckRect(const vdc_period_rect_t * const rect);
static vdc_error_t CheckRect_WriteProcess (const vdc_period_rect_t * const rect);
static vdc_error_t CheckTcon_Vertical(const vdc_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc_error_t CheckTcon_Horizontal(const vdc_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc_error_t CheckTcon_HorizontalPol(const vdc_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc_error_t CheckTcon_DataEnable(const vdc_lcd_tcon_timing_t * const lcd_tcon_timing);
static vdc_error_t CheckGammaCrct_GainAdjustment(const uint16_t * gam_gain);

/**************************************************************************//**
 * @fn          VDC_InitializeCheckPrm
 * @brief       Checks on initialization parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Initialization parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_InitializeCheckPrm   
        (const vdc_channel_t ch, const vdc_init_t * const param)
{
    vdc_error_t error;
    const vdc_lvds_t     * lvds_tmp; 
    vdc_resource_state_t   rsrc_state;
    vdc_channel_t          lvds_vdc_sel_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Panel clock select */
    if (param->panel_icksel >= VDC_PANEL_ICKSEL_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if ((param->panel_icksel != VDC_PANEL_ICKSEL_LVDS) && (param->panel_icksel != VDC_PANEL_ICKSEL_LVDS_DIV7))
    {
        /* Clock frequency division ratio */
        if (param->panel_dcdr >= VDC_PANEL_CLKDIV_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }

    /* LVDS-related parameter */
    lvds_tmp = param->lvds;

    /* NULL check */
    if (lvds_tmp == NULL)
    {
        rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_LVDS_CLK);
        if (rsrc_state == VDC_RESOURCE_ST_INVALID)
        {
            /* The LVDS PLL clock has not been set yet. */
            if ((param->panel_icksel == VDC_PANEL_ICKSEL_LVDS) || (param->panel_icksel == VDC_PANEL_ICKSEL_LVDS_DIV7))
            {
                error = VDC_ERR_PARAM_NULL;
                goto END;
            }
        }
        else
        {
            if (param->panel_icksel == VDC_PANEL_ICKSEL_LVDS_DIV7)
            {
                lvds_vdc_sel_tmp = VDC_ShrdPrmGetLvdsCh();
                if (ch != lvds_vdc_sel_tmp)
                {
                    /* Channel mismatch error */
                    error = VDC_ERR_PARAM_CONDITION;
                    goto END;
                }
            }
        }
    }
    else
    {
        /* The clock input to frequency divider 1 */
        if (lvds_tmp->lvds_in_clk_sel >= VDC_LVDS_INCLK_SEL_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }

        /* The frequency dividing value (NIDIV and NODIV) for frequency divider 1 and 2 */
        if (lvds_tmp->lvds_odiv_set >= VDC_LVDS_NDIV_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }

        /* A channel in VDC whose data is to be output through the LVDS */
        if (lvds_tmp->lvds_vdc_sel >= VDC_CHANNEL_NUM)
        {
            error = VDC_ERR_PARAM_CHANNEL;
            goto END;
        }
        if (param->panel_icksel != VDC_PANEL_ICKSEL_LVDS_DIV7)
        {
            /* When using the clock output from the LVDS PLL as the clock for the LVDS output interface,
               both channel number should match. */
            if (ch != lvds_tmp->lvds_vdc_sel)
            {
                /* Channel mismatch error */
                error = VDC_ERR_PARAM_CONDITION;
                goto END;
            }
        }

        /* The frequency dividing value (NFD) for the feedback frequency */
        if (((uint32_t)lvds_tmp->lvdspll_fd > VDC_LVDSPLL_FD_MAX) ||
            /* Cast to an appropriate type */
            ((uint32_t)lvds_tmp->lvdspll_fd < VDC_LVDSPLL_FD_MIN) )
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* The frequency dividing value (NRD) for the input frequency */
        if ((uint32_t)lvds_tmp->lvdspll_rd > VDC_LVDSPLL_RD_MAX )
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_InitializeCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_TerminateCheckPrm
 * @brief       Checks on parameters in R_VDC_Terminate
 * @param[in]   ch          : Channel
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_TerminateCheckPrm (const vdc_channel_t ch) 
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_TerminateCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_VideoInputCheckPrm
 * @brief       Checks on video input setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Video input setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_VideoInputCheckPrm 
            (const vdc_channel_t ch, const vdc_input_t * const param)
{
    vdc_error_t                error;
    const vdc_sync_delay_t   * dly_tmp;
    const vdc_ext_in_sig_t   * ext_sig_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Vsync signal 1/2fH phase timing */
    if (((uint32_t)param->inp_fh50 & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Vsync signal 1/4fH phase timing */
    if (((uint32_t)param->inp_fh25 & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Sync signal delay adjustment */
    dly_tmp = param->dly;

    /* NULL check */
    if (dly_tmp != NULL)
    {
        /* Number of lines for delaying Vsync signal and field differentiation signal */
        if (((uint32_t)dly_tmp->inp_vs_dly_l & (uint32_t)~VDC_CPARA_RANGE_0X00000007) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Field differentiation signal delay amount */
        if (((uint32_t)dly_tmp->inp_fld_dly & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)dly_tmp->inp_fld_dly > (uint32_t)VDC_CPARA_INP_FLD_DLY_MAX)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Vsync signal delay amount */
        if (((uint32_t)dly_tmp->inp_vs_dly & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)dly_tmp->inp_vs_dly > (uint32_t)VDC_CPARA_INP_VS_DLY_MAX)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Hsync signal delay amount */
        if (((uint32_t)dly_tmp->inp_hs_dly & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)dly_tmp->inp_hs_dly > (uint32_t)VDC_CPARA_INP_HS_DLY_MAX)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* External input signal */
    ext_sig_tmp = param->ext_sig;

    /* NULL check */
    if (ext_sig_tmp == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* External input format select */
    if (ext_sig_tmp->inp_format >= VDC_EXTIN_FORMAT_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    if (ext_sig_tmp->inp_h_pos >= VDC_EXTIN_H_POS_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if ((ext_sig_tmp->inp_h_pos == VDC_EXTIN_H_POS_YCRYCB) || (ext_sig_tmp->inp_h_pos == VDC_EXTIN_H_POS_CRYCBY))
    {
        if (ext_sig_tmp->inp_format == VDC_EXTIN_FORMAT_YCBCR422)
        {
            error = VDC_ERR_PARAM_CONDITION;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_VideoInputCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_SyncControlCheckPrm
 * @brief       Checks on sync control parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Sync control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_SyncControlCheckPrm  
            (const vdc_channel_t ch, const vdc_sync_ctrl_t * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    if (param->res_vs_sel != VDC_OFF)
    {
        /* Free-running Vsync period setting */
        if (((uint32_t)param->res_fv & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }

    /* Hsync period setting */
    if (((uint32_t)param->res_fh & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Vsync signal delay control */
    if (((uint32_t)param->res_vsdly & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Full-screen enable control */
    error = CheckRect(&param->res_f);
    if (error != VDC_OK)
    {
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_SyncControlCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_DisplayOutputCheckPrm
 * @brief       Checks on display output configuration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display output configuration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_DisplayOutputCheckPrm  
            (const vdc_channel_t ch, const vdc_output_t * const param)
{
    vdc_error_t                    error;
    uint32_t                        tcon_sig_index;
    static vdc_error_t    (* const tcon_function_tbl[VDC_LCD_TCONSIG_NUM])(
                                        const vdc_lcd_tcon_timing_t * const lcd_tcon_timing) =
    {
        &CheckTcon_Vertical,        /* STVA/VS */
        &CheckTcon_Vertical,        /* STVB/VE */
        &CheckTcon_Horizontal,      /* STH/HS */
        &CheckTcon_Horizontal,      /* STB/HE */
        &CheckTcon_Horizontal,      /* CPV/GCK */
        &CheckTcon_HorizontalPol,   /* POLA */
        &CheckTcon_HorizontalPol,   /* POLB */
        &CheckTcon_DataEnable       /* DE */
    };

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* 1/2fH timing */
    if (((uint32_t)param->tcon_half & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Offset Hsync signal timing */
    if (((uint32_t)param->tcon_offset & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* LCD TCON timing setting */
    for (tcon_sig_index = 0; tcon_sig_index < VDC_LCD_TCONSIG_NUM; tcon_sig_index++)
    {
        /* NULL check */
        if (param->outctrl[tcon_sig_index] != NULL)
        {
            error = tcon_function_tbl[tcon_sig_index](param->outctrl[tcon_sig_index]);
            if (error != VDC_OK)
            {
                goto END;
            }
        }
    }

    /* LCD output format select */
    if (param->out_format >= VDC_LCD_OUTFORMAT_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if (param->out_format == VDC_LCD_OUTFORMAT_SERIAL_RGB)
    {
        /* Clock frequency control */
        if (param->out_frq_sel >= VDC_LCD_SERIAL_CLKFRQ_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (param->out_frq_sel == VDC_LCD_PARALLEL_CLKFRQ_1)
        {
            error = VDC_ERR_PARAM_CONDITION;
            goto END;
        }

        /* Clock phase adjustment */
        if (param->out_phase >= VDC_LCD_SERIAL_CLKPHASE_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (param->out_frq_sel == VDC_LCD_SERIAL_CLKFRQ_3)
        {
            if (param->out_phase == VDC_LCD_SERIAL_CLKPHASE_3)
            {
                error = VDC_ERR_PARAM_CONDITION;
                goto END;
            }
        }
    }

    /* Background color in 24-bit RGB color format */
    if ((param->bg_color & (uint32_t)~VDC_CPARA_RANGE_0X00FFFFFF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_DisplayOutputCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_CallbackISRCheckPrm
 * @brief       Checks on interrupt callback setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Interrupt callback setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_CallbackISRCheckPrm  
            (const vdc_channel_t ch, const vdc_int_t * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* VDC interrupt type */
    if (param->type >= VDC_INT_TYPE_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Interrupt callback function pointer */
    if (param->callback != 0)
    {
        if ((param->type == VDC_INT_TYPE_VLINE) || (param->type == VDC_INT_TYPE_S0_WLINE))
        {
            /* Line interrupt set */
            if (((uint32_t)param->line_num & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }
    }
END:
    return error; 
}   /* End of function VDC_CallbackISRCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_WriteDataControlCheckPrm
 * @brief       Checks on data write control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_WriteDataControlCheckPrm ( 
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_write_t  * const param)
{
    vdc_error_t                    error;
    const vdc_scalingdown_rot_t  * scldw_rt_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    if (layer_id != VDC_LAYER_ID_0_WR)
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Scaling-down and rotation parameter */
    scldw_rt_tmp = &param->scalingdown_rot;

    /* Image area to be captured */
    error = CheckRect_WriteProcess(&scldw_rt_tmp->res);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* Number of valid lines in vertical direction output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)VDC_MULTIPLE_4_MASK) != 0u)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Number of valid horizontal pixels output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)VDC_MULTIPLE_4_MASK) != 0u)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Frame buffer writing mode for image processing */
    if (scldw_rt_tmp->res_ds_wr_md >= VDC_WR_MD_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Swap setting in frame buffer writing */
    if (param->res_wrswa >= VDC_WR_RD_WRSWA_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Frame buffer video-signal writing format */
    if (param->res_md >= VDC_RES_MD_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Writing rate */
    if (param->res_fs_rate >= VDC_RES_FS_RATE_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Frame buffer base address */
    if (param->base == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Frame buffer line offset address [byte] */
    if ((param->ln_off & (uint32_t)~VDC_CPARA_RANGE_0X00007FFF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (param->res_bst_md == VDC_BST_MD_32BYTE)
    {
        /* 32-byte transfer */
        if ((param->ln_off & (uint32_t)VDC_MULTIPLE_32_MASK) != 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    else
    {
        /* 128-byte transfer */
        if ((param->ln_off & (uint32_t)VDC_MULTIPLE_128_MASK) != 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* Number of frames of buffer to be written to (res_flm_num + 1) */
    if ((param->flm_num & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Frame buffer frame offset address [byte] */
    if (param->flm_num != 0u)
    {
        /* Cast to an appropriate type */
        if ((param->flm_off & (uint32_t)~VDC_CPARA_RANGE_0X007FFFFF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
        if (param->res_bst_md == VDC_BST_MD_32BYTE)
        {
            /* 32-byte transfer */
            if ((param->flm_off & (uint32_t)VDC_MULTIPLE_32_MASK) != 0u)
            {
                error = VDC_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
        else
        {
            /* 128-byte transfer */
            if ((param->flm_off & (uint32_t)VDC_MULTIPLE_128_MASK) != 0u)
            {
                error = VDC_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
    }
END:
    return error; 
}   /* End of function VDC_WriteDataControlCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ChangeWriteProcessCheckPrm
 * @brief       Checks on data write change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChangeWriteProcessCheckPrm ( 
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_write_chg_t  * const param)
{
    vdc_error_t                    error;
    const vdc_scalingdown_rot_t  * scldw_rt_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    if (layer_id != VDC_LAYER_ID_0_WR)
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Scaling-down and rotation parameter */
    scldw_rt_tmp = &param->scalingdown_rot;

    /* Image area to be captured */
    error = CheckRect_WriteProcess(&scldw_rt_tmp->res);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* Number of valid lines in vertical direction output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)scldw_rt_tmp->res_out_vw & (uint32_t)VDC_MULTIPLE_4_MASK) != 0u)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Number of valid horizontal pixels output by scaling-down control block */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)scldw_rt_tmp->res_out_hw & (uint32_t)VDC_MULTIPLE_4_MASK) != 0u)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Frame buffer writing mode */
    if (scldw_rt_tmp->res_ds_wr_md >= VDC_WR_MD_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_ChangeWriteProcessCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ReadDataControlCheckPrm
 * @brief       Checks on data read control parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Data read control parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ReadDataControlCheckPrm (  
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_read_t   * const param)
{
    vdc_error_t            error;
    void                 * btm_base_tmp;
    vdc_resource_state_t   rsrc_state;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Frame buffer address setting signal */
    if (param->gr_flm_sel >= VDC_GR_FLM_SEL_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* initialize NULL */
    btm_base_tmp    = NULL;
    rsrc_state      = VDC_RESOURCE_ST_INVALID;
    switch (graphics_id)
    {
        case VDC_GR_TYPE_GR0:
            btm_base_tmp    = VDC_ShrdPrmGetFrBuffBtm();
            rsrc_state      = VDC_ShrdPrmGetLayerResource(VDC_LAYER_ID_0_WR);
        break;

        /* The following two cases are intentionally combined. */
        case VDC_GR_TYPE_GR2:
        case VDC_GR_TYPE_GR3:
            if (param->gr_flm_sel != VDC_GR_FLM_SEL_FLM_NUM)
            {
                error = VDC_ERR_PARAM_CONDITION;
                goto END;
            }
        break;
        default:

            /* DO NOTHING */
        break;
    }

    /* NULL check */
    if ((btm_base_tmp != NULL) && (rsrc_state != VDC_RESOURCE_ST_INVALID))
    {
        /* btm_base_tmp is valid only when rsrc_state is valid. */
        if (param->gr_flm_sel == VDC_GR_FLM_SEL_SCALE_DOWN)
        {
            error = VDC_ERR_PARAM_CONDITION;
            goto END;
        }
    }

    /* Frame buffer base address */
    if (param->gr_base == NULL)
    {
        if (param->gr_flm_sel != VDC_GR_FLM_SEL_POINTER_BUFF)
        {
            error = VDC_ERR_PARAM_NULL;
            goto END;
        }
    }

    /* Frame buffer line offset address */
    if ((param->gr_ln_off & (uint32_t)~VDC_CPARA_RANGE_0X00007FFF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }
    if (param->gr_bst_md == VDC_BST_MD_32BYTE)
    {
        /* 32-byte transfer */
        if ((param->gr_ln_off & (uint32_t)VDC_MULTIPLE_32_MASK) != 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    else
    {
        /* 128-byte transfer */
        if ((param->gr_ln_off & (uint32_t)VDC_MULTIPLE_128_MASK) != 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* NULL check */
    if (param->width_read_fb != NULL)
    {
        /* Number of lines in a frame */
        if (((uint32_t)param->width_read_fb->in_vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)param->width_read_fb->in_vw == 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Width of the horizontal valid period */
        if (((uint32_t)param->width_read_fb->in_hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)param->width_read_fb->in_hw < (uint32_t)VDC_CPARA_RANGE_0X00000003)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* Format of the frame buffer read signal */
    if (param->gr_format >= VDC_GR_FORMAT_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if ((param->gr_format == VDC_GR_FORMAT_YCBCR422) || (param->gr_format == VDC_GR_FORMAT_YCBCR444))
    {
        /* YCbCr422 and YCbCr444 are supported for the graphics 0 and 1. */
        if (graphics_id != VDC_GR_TYPE_GR0)
        {
            error = VDC_ERR_PARAM_CONDITION;
            goto END;
        }
    }
    if (param->gr_format == VDC_GR_FORMAT_YCBCR422)
    {
        /* Swapping of data read from buffer in the YCbCr422 format */
        if (param->gr_ycc_swap >= VDC_GR_YCCSWAP_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }

    /* Swap setting in frame buffer reading */
    if (param->gr_rdswa >= VDC_WR_RD_WRSWA_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Graphics display area */
    error = CheckRect(&param->gr_grc);
    if (error != VDC_OK)
    {
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_ReadDataControlCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ChangeReadProcessCheckPrm
 * @brief       Checks on data read change parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data read change parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChangeReadProcessCheckPrm ( 
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_read_chg_t   * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Width of the image read from frame buffer */
    if (param->width_read_fb != NULL)
    {
        /* Number of lines in a frame */
        if (((uint32_t)param->width_read_fb->in_vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)param->width_read_fb->in_vw == 0u)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Width of the horizontal valid period */
        if (((uint32_t)param->width_read_fb->in_hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)param->width_read_fb->in_hw < (uint32_t)VDC_CPARA_RANGE_0X00000003)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* Graphics display area */
    if (param->gr_grc != NULL)
    {
        error = CheckRect(param->gr_grc);
        if (error != VDC_OK)
        {
            goto END;
        }
    }

    /* Graphics display mode */
    if (param->gr_disp_sel != NULL)
    {
        if (((*(param->gr_disp_sel)) < VDC_DISPSEL_IGNORED) || ((*(param->gr_disp_sel)) >= VDC_DISPSEL_NUM))
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_ChangeReadProcessCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_StartProcessCheckPrm
 * @brief       Checks on data write/read start parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Data write/read start parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_StartProcessCheckPrm ( 
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_start_t  * const param)
{
    vdc_error_t                error;
    uint32_t                   graphics_types;
    const vdc_gr_disp_sel_t  * gr_disp_sel_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    gr_disp_sel_tmp = param->gr_disp_sel;

    /* NULL check */
    if (gr_disp_sel_tmp == NULL)
    {
        error = VDC_OK;
        goto END;
    }

    /* Graphics display mode */
    if (layer_id == VDC_LAYER_ID_ALL)
    {
        for (graphics_types = 0; graphics_types < VDC_GR_TYPE_NUM; graphics_types++)
        {
            if ((gr_disp_sel_tmp[graphics_types] < VDC_DISPSEL_IGNORED) ||
                (gr_disp_sel_tmp[graphics_types] >= VDC_DISPSEL_NUM))
            {
                error = VDC_ERR_PARAM_UNDEFINED;
                goto END;
            }
        }
    }
    else
    {
        if (((*gr_disp_sel_tmp) < VDC_DISPSEL_IGNORED) || ((*gr_disp_sel_tmp) >= VDC_DISPSEL_NUM))
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_StartProcessCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_StopProcessCheckPrm
 * @brief       Checks on parameters in R_VDC_StopProcess
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_StopProcessCheckPrm  
            (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_StopProcessCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ReleaseDataControlCheckPrm
 * @brief       Checks on parameters in R_VDC_ReleaseDataControl
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ReleaseDataControlCheckPrm  
            (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_ReleaseDataControlCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_VideoNoiseReductCheckPrm
 * @brief       Checks on noise reduction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Noise reduction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_VideoNoiseReductCheckPrm 
            (const vdc_channel_t ch, const vdc_noise_reduction_t * const param)
{
    vdc_error_t            error;
    const vdc_nr_param_t * nr_param;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_OK;
        goto END;
    }

    /* Y/G signal noise reduction parameter */
    nr_param = &param->y;

    /* TAP select */
    if (nr_param->nr1d_tap >= VDC_NR_TAPSEL_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC_NR_GAIN_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Cb/B signal noise reduction parameter */
    nr_param = &param->cb;

    /* TAP select */
    if (nr_param->nr1d_tap >= VDC_NR_TAPSEL_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC_NR_GAIN_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Cr/R signal noise reduction parameter */
    nr_param = &param->cr;

    /* TAP select */
    if (nr_param->nr1d_tap >= VDC_NR_TAPSEL_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Maximum value of coring (absolute value) */
    if ((nr_param->nr1d_th & (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Noise reduction gain adjustment */
    if (nr_param->nr1d_gain >= VDC_NR_GAIN_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_VideoNoiseReductCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ImageColorMatrixCheckPrm
 * @brief       Checks on color matrix setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Color matrix setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageColorMatrixCheckPrm  
            (const vdc_channel_t ch, const vdc_color_matrix_t * const param)
{
    vdc_error_t    error;
    int32_t        colormtx_index;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Color matrix module */
    if (param->module >= VDC_COLORMTX_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Operating mode */
    if (param->mtx_mode >= VDC_COLORMTX_MODE_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
    if (param->module != VDC_COLORMTX_IMGCNT)
    {
        if ((param->mtx_mode == VDC_COLORMTX_GBR_YCBCR) || (param->mtx_mode == VDC_COLORMTX_YCBCR_YCBCR))
        {
            error = VDC_ERR_PARAM_CONDITION;
            goto END;
        }
    }

    /* Offset (DC) adjustment of Y/G, B, and R signal */
    for (colormtx_index = 0; colormtx_index < VDC_COLORMTX_OFFST_NUM; colormtx_index++)
    {
        /* Cast to an appropriate type */
        if (((uint32_t)param->offset[colormtx_index] & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }

    /* GG, GB, GR, BG, BB, BR, RG, RB, and RR signal gain adjustment */
    for (colormtx_index = 0; colormtx_index < VDC_COLORMTX_GAIN_NUM; colormtx_index++)
    {
        /* Cast to an appropriate type */
        if (((uint32_t)param->gain[colormtx_index] & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_ImageColorMatrixCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ImageEnhancementCheckPrm
 * @brief       Checks on sharpness setup parameter and enhancer-enabled area setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   sharp_param : Sharpness setup parameter
 * @param[in]   enh_area    : Enhancer-enabled area setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageEnhancementCheckPrm ( 
    const vdc_channel_t                ch,
    const vdc_imgimprv_id_t            imgimprv_id,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_period_rect_t    * const enh_area)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* Image quality improver ID */
    if (imgimprv_id >= VDC_IMG_IMPRV_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* Sharpness parameter */
    if (sharp_param != NULL)
    {
        /* Sharpness control parameter (H1, H2, and H3) */
        if (((uint32_t)sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H1].shp_core &

            /* Cast to an appropriate type */
            (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if (((uint32_t)sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H2].shp_core &

            /* Cast to an appropriate type */
            (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if (((uint32_t)sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H3].shp_core &

        /* Cast to an appropriate type */
        (uint32_t)~VDC_CPARA_RANGE_0X0000007F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }

    /* Enhance area */
    if (enh_area != NULL)
    {
        /* Vertical start position */
        if (((uint32_t)enh_area->vs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)enh_area->vs < (uint32_t)VDC_CPARA_ENH_VS_LIMIT)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Height (vertical width) */
        if (((uint32_t)enh_area->vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Horizontal start position */
        if (((uint32_t)enh_area->hs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)enh_area->hs < (uint32_t)VDC_CPARA_ENH_HS_LIMIT)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

        /* Horizontal width */
        if (((uint32_t)enh_area->hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_ImageEnhancementCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ImageBlackStretchCheckPrm
 * @brief       Checks on black stretch setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   imgimprv_id : Image quality improver ID
 * @param[in]   param       : Black stretch setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ImageBlackStretchCheckPrm ( 
    const vdc_channel_t        ch,
    const vdc_imgimprv_id_t    imgimprv_id,
    const vdc_black_t  * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* Image quality improver ID */
    if (imgimprv_id >= VDC_IMG_IMPRV_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* NULL check */
    if (param != NULL)
    {
        /* Black stretch start point */
        if (((uint32_t)param->bkstr_st & (uint32_t)~VDC_CPARA_RANGE_0X0000000F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Black stretch depth */
        if (((uint32_t)param->bkstr_d & (uint32_t)~VDC_CPARA_RANGE_0X0000000F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Black stretch time constant (T1) */
        if (((uint32_t)param->bkstr_t1 & (uint32_t)~VDC_CPARA_RANGE_0X0000001F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Black stretch time constant (T2) */
        if (((uint32_t)param->bkstr_t2 & (uint32_t)~VDC_CPARA_RANGE_0X0000001F) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Cast to an appropriate type */
        if ((uint32_t)param->bkstr_t2 == VDC_BKSTR_T2_PROHIBITED)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }

    }
END:
    return error; 
}   /* End of function VDC_ImageBlackStretchCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_AlphaBlendingCheckPrm
 * @brief       Checks on alpha blending setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Alpha blending setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_AlphaBlendingCheckPrm ( 
    const vdc_channel_t                ch,
    const vdc_layer_id_t               layer_id,
    const vdc_alpha_blending_t * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    if ((layer_id != VDC_LAYER_ID_2_RD) && (layer_id != VDC_LAYER_ID_3_RD))
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_AlphaBlendingCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_AlphaBlendingRectCheckPrm
 * @brief       Checks on setup parameter for alpha blending in a rectangular area
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   param       : Setup parameter for alpha blending in a rectangular area
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_AlphaBlendingRectCheckPrm ( 
    const vdc_channel_t                        ch,
    const vdc_layer_id_t                       layer_id,
    const vdc_alpha_blending_rect_t    * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    if ((layer_id != VDC_LAYER_ID_2_RD) && (layer_id != VDC_LAYER_ID_3_RD))
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }

    /* NULL check */
    if (param != NULL)
    {
        /* Rectangular area subjected to alpha blending */
        if (param->gr_arc != NULL)
        {
            /* Vertical start position */
            if (((uint32_t)param->gr_arc->vs_rel & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Height (vertical width) */
            if (((uint32_t)param->gr_arc->vw_rel & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Horizontal start position */
            if (((uint32_t)param->gr_arc->hs_rel & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Horizontal width */
            if (((uint32_t)param->gr_arc->hw_rel & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }

        /* Alpha Blending in a rectangular area */
        if (param->alpha_rect != NULL)
        {
            /* Frame rate for alpha blending in a rectangular area */
            if ((param->alpha_rect->gr_arc_coef < (int16_t)VDC_CPARA_GR_ARC_COEF_MIN) ||
            
            /* Cast to an appropriate type */
                (param->alpha_rect->gr_arc_coef > (int16_t)VDC_CPARA_GR_ARC_COEF_MAX))
            {
                error = VDC_ERR_PARAM_EXCEED_RANGE;
                goto END;
            }
        }
    }
END:
    return error; 
}   /* End of function VDC_AlphaBlendingRectCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_ChromakeyCheckPrm
 * @brief       Checks on chroma-key setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : Chroma-key setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_ChromakeyCheckPrm ( 
    const vdc_channel_t            ch,
    const vdc_layer_id_t           layer_id,
    const vdc_graphics_type_t      graphics_id,
    const vdc_chromakey_t  * const param)
{
    vdc_error_t        error;
    vdc_gr_format_t    gr_format_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param != NULL)
    {
        gr_format_tmp = VDC_ShrdPrmGetGraphicsFormat(graphics_id);
        switch (gr_format_tmp)
        {
            /* The following four cases are intentionally combined,
             * since the four types of graphics format are represented as 16 bit unsigned integer. */
            case VDC_GR_FORMAT_RGB565:
            case VDC_GR_FORMAT_ARGB1555:
            case VDC_GR_FORMAT_ARGB4444:
            case VDC_GR_FORMAT_RGBA5551:

                /* parameter check */
                if (((param->ck_color & (uint32_t)~VDC_CPARA_RANGE_0X0000FFFF) != 0u) ||

                    /* Cast to an appropriate type */
                    ((param->rep_color & (uint32_t)~VDC_CPARA_RANGE_0X0000FFFF) != 0u))
                {
                    error = VDC_ERR_PARAM_BIT_WIDTH;
                }
            break;
            case VDC_GR_FORMAT_RGB888:

                /* parameter check */
                if (((param->ck_color & (uint32_t)~VDC_CPARA_RANGE_0X00FFFFFF) != 0u) ||

                    /* Cast to an appropriate type */
                    ((param->rep_color & (uint32_t)~VDC_CPARA_RANGE_0X00FFFFFF) != 0u))
                {
                    error = VDC_ERR_PARAM_BIT_WIDTH;
                }
            break;
            case VDC_GR_FORMAT_CLUT8:

                /* Cast to an appropriate type */
                if ((param->ck_color & (uint32_t)~VDC_CPARA_RANGE_0X000000FF) != 0u)
                {
                    error = VDC_ERR_PARAM_BIT_WIDTH;
                }
            break;
            case VDC_GR_FORMAT_CLUT4:

                /* Cast to an appropriate type */
                if ((param->ck_color & (uint32_t)~VDC_CPARA_RANGE_0X0000000F) != 0u)
                {
                    error = VDC_ERR_PARAM_BIT_WIDTH;
                }
            break;
            case VDC_GR_FORMAT_CLUT1:

                /* Cast to an appropriate type */
                if ((param->ck_color & (uint32_t)~VDC_CPARA_RANGE_0X00000001) != 0u)
                {
                    error = VDC_ERR_PARAM_BIT_WIDTH;
                }
            break;
            default:

                /* DO NOTHING */
            break;
        }
    }
END:
    return error; 
}   /* End of function VDC_ChromakeyCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_CLUTCheckPrm
 * @brief       Checks on CLUT setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   layer_id    : Layer ID
 * @param[in]   graphics_id : Graphics type ID
 * @param[in]   param       : CLUT setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_CLUTCheckPrm ( 
    const vdc_channel_t        ch,
    const vdc_layer_id_t       layer_id,
    const vdc_graphics_type_t  graphics_id,
    const vdc_clut_t   * const param)
{
    vdc_error_t        error;
    vdc_gr_format_t    gr_format_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }
    error = CheckGraphicsTypeLayerId(layer_id);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* The number of colors in CLUT */
    if (param->color_num == 0u)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
    gr_format_tmp = VDC_ShrdPrmGetGraphicsFormat(graphics_id);
    if (gr_format_tmp == VDC_GR_FORMAT_CLUT1)
    {
        /* CLUT1 */
        if (param->color_num > (uint32_t)VDC_CPARA_CLUT1_NUM)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    else if (gr_format_tmp == VDC_GR_FORMAT_CLUT4)
    {
        /* CLUT4 */
        if (param->color_num > (uint32_t)VDC_CPARA_CLUT4_NUM)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }
    else
    {
        /* CLUT8 */
        if (param->color_num > (uint32_t)VDC_CPARA_CLUT8_NUM)
        {
            error = VDC_ERR_PARAM_EXCEED_RANGE;
            goto END;
        }
    }

    /* Address of the area storing the CLUT data (in ARGB8888 format) */
    if (param->clut == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }
END:
    return error; 
}   /* End of function VDC_CLUTCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_DisplayCalibrationCheckPrm
 * @brief       Checks on display calibration parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Display calibration parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_DisplayCalibrationCheckPrm  
            (const vdc_channel_t ch, const vdc_disp_calibration_t * const param)
{
    vdc_error_t                    error;
    const vdc_calibr_bright_t    * bright_tmp;
    const vdc_calibr_dither_t    * panel_dither_tmp;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param == NULL)
    {
        error = VDC_ERR_PARAM_NULL;
        goto END;
    }

    /* Brightness (DC) adjustment */
    bright_tmp = param->bright;

    /* NULL check */
    if (bright_tmp != NULL)
    {
        /* Brightness (DC) adjustment of G signal */
        if (((uint32_t)bright_tmp->pbrt_g & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Brightness (DC) adjustment of B signal */
        if (((uint32_t)bright_tmp->pbrt_b & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }

        /* Brightness (DC) adjustment of R signal */
        if (((uint32_t)bright_tmp->pbrt_r & (uint32_t)~VDC_CPARA_RANGE_0X000003FF) != 0u)
        {
            error = VDC_ERR_PARAM_BIT_WIDTH;
            goto END;
        }
    }

    /* Panel dithering */
    panel_dither_tmp = param->panel_dither;

    /* NULL check */
    if (panel_dither_tmp != NULL)
    {
        /* Panel dither operation mode */
        if (panel_dither_tmp->pdth_sel >= VDC_PDTH_MD_NUM)
        {
            error = VDC_ERR_PARAM_UNDEFINED;
            goto END;
        }
        if (panel_dither_tmp->pdth_sel == VDC_PDTH_MD_2X2)
        {
            /* Pattern value (A)-(D) of 2x2 pattern dither */
            if (((uint32_t)panel_dither_tmp->pdth_pa & (uint32_t)~VDC_CPARA_RANGE_0X00000003) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Cast to an appropriate type */
            if (((uint32_t)panel_dither_tmp->pdth_pb & (uint32_t)~VDC_CPARA_RANGE_0X00000003) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Cast to an appropriate type */
            if (((uint32_t)panel_dither_tmp->pdth_pc & (uint32_t)~VDC_CPARA_RANGE_0X00000003) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }

            /* Cast to an appropriate type */
            if (((uint32_t)panel_dither_tmp->pdth_pd & (uint32_t)~VDC_CPARA_RANGE_0X00000003) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
        }
    }
END:
    return error; 
}   /* End of function VDC_DisplayCalibrationCheckPrm() */

/**************************************************************************//**
 * @fn          VDC_GammaCorrectionCheckPrm
 * @brief       Checks on gamma correction setup parameter
 * @param[in]   ch          : Channel
 * @param[in]   param       : Gamma correction setup parameter
 * @retval      Error code
 *****************************************************************************/
vdc_error_t VDC_GammaCorrectionCheckPrm  
            (const vdc_channel_t ch, const vdc_gamma_correction_t * const param)
{
    vdc_error_t error;

    error = CheckChannel(ch);
    if (error != VDC_OK)
    {
        goto END;
    }

    /* NULL check */
    if (param != NULL)
    {
        /* Gain adjustment of area 0 to 31 of G signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_g_gain);
        if (error != VDC_OK)
        {
            goto END;
        }

        /* Gain adjustment of area 0 to 31 of B signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_b_gain);
        if (error != VDC_OK)
        {
            goto END;
        }

        /* Gain adjustment of area 0 to 31 of R signal */
        error = CheckGammaCrct_GainAdjustment(param->gam_r_gain);
        if (error != VDC_OK)
        {
            goto END;
        }
    }
END:
    return error; 
}   /* End of function VDC_GammaCorrectionCheckPrm() */

/******************************************************************************
Local Functions
******************************************************************************/
/**************************************************************************//**
 * @fn          CheckChannel
 * @brief       Checks on VDC channel
 * @param[in]   ch          : Channel
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckChannel (const vdc_channel_t ch) 
{
    vdc_error_t error;

    error = VDC_OK;

    if (ch >= VDC_CHANNEL_NUM)
    {
        error = VDC_ERR_PARAM_CHANNEL;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckChannel() */

/**************************************************************************//**
 * @fn          CheckLayerId
 * @brief       Checks on layer ID
 * @param[in]   layer_id        : Layer ID
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckLayerId (const vdc_layer_id_t layer_id) 
{
    vdc_error_t error;

    error = VDC_OK;

    if ((layer_id < VDC_LAYER_ID_ALL) || (layer_id >= VDC_LAYER_ID_NUM))
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckLayerId() */

/**************************************************************************//**
 * @fn          CheckGraphicsTypeLayerId
 * @brief       Checks on layer ID related to graphics
 * @param[in]   layer_id        : Layer ID
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckGraphicsTypeLayerId (const vdc_layer_id_t layer_id) 
{
    vdc_error_t error;

    error = VDC_OK;

    if ((layer_id != VDC_LAYER_ID_0_RD) && (layer_id != VDC_LAYER_ID_2_RD) && (layer_id != VDC_LAYER_ID_3_RD))
    {
        error = VDC_ERR_PARAM_LAYER_ID;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckGraphicsTypeLayerId() */

/**************************************************************************//**
 * @fn          CheckRect
 * @brief       Checks on horizontal/vertical timing of the VDC signals
 * @param[in]   rect            : Horizontal/vertical timing of the VDC signals
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckRect (const vdc_period_rect_t * const rect) 
{
    vdc_error_t error;

    error = VDC_OK;

    /* Vertical start position */
    if (((uint32_t)rect->vs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if ((uint32_t)rect->vs < (uint32_t)VDC_CPARA_VS_MIN)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Height (vertical width) */
    if (((uint32_t)rect->vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)rect->vs + (uint32_t)rect->vw) > (uint32_t)VDC_CPARA_VS_VW_MAX)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Horizontal start position */
    if (((uint32_t)rect->hs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if ((uint32_t)rect->hs < (uint32_t)VDC_CPARA_HS_MIN)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Horizontal width */
    if (((uint32_t)rect->hw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)rect->hs + (uint32_t)rect->hw) > (uint32_t)VDC_CPARA_HS_HW_MAX)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckRect() */

/**************************************************************************//**
 * @fn          CheckRect_WriteProcess
 * @brief       Checks on horizontal/vertical timing of the VDC signals
 * @param[in]   rect            : Horizontal/vertical timing of the VDC signals
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckRect_WriteProcess 
                    (const vdc_period_rect_t * const rect)
{
    vdc_error_t error;

    error = VDC_OK;

    /* Vertical start position */
    if (((uint32_t)rect->vs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if ((uint32_t)rect->vs < (uint32_t)VDC_CPARA_VS_MIN)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Height (vertical width) */
    if (((uint32_t)rect->vw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)rect->vs + (uint32_t)rect->vw) > (uint32_t)VDC_CPARA_VS_VW_MAX)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Horizontal start position */
    if (((uint32_t)rect->hs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if ((uint32_t)rect->hs < (uint32_t)VDC_CPARA_HS_MIN)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }

    /* Horizontal width */
    if (((uint32_t)rect->hw & (uint32_t)~VDC_CPARA_RANGE_0X00000FFF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Cast to an appropriate type */
    if (((uint32_t)rect->hs + (uint32_t)rect->hw) > (uint32_t)VDC_CPARA_HS_HW_MAX)
    {
        error = VDC_ERR_PARAM_EXCEED_RANGE;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckRect_WriteProcess() */

/**************************************************************************//**
 * @fn          CheckTcon_Horizontal
 * @brief       Checks on LCD TCON timing signal parameter for horizontal panel driver signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckTcon_Horizontal 
                    (const vdc_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc_error_t error;

    error = VDC_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC_LCD_TCON_PIN_NUM))
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckTcon_Horizontal() */

/**************************************************************************//**
 * @fn          CheckTcon_HorizontalPol
 * @brief       Checks on LCD TCON timing signal parameter for horizontal panel driver signal (POLA/POLB)
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckTcon_HorizontalPol  
                    (const vdc_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc_error_t error;

    error = VDC_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* POLA/POLB signal generation mode select */
    if (lcd_tcon_timing->tcon_md >= VDC_LCD_TCON_POLMD_NUM)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }

    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC_LCD_TCON_PIN_NUM))
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckTcon_HorizontalPol() */

/**************************************************************************//**
 * @fn          CheckTcon_Vertical
 * @brief       Checks on LCD TCON timing signal parameter for vertical panel driver signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckTcon_Vertical  
                    (const vdc_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc_error_t error;

    error = VDC_OK;

    /* Signal pulse start position */
    if (((uint32_t)lcd_tcon_timing->tcon_hsvs & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* Pulse width */
    if (((uint32_t)lcd_tcon_timing->tcon_hwvw & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
    {
        error = VDC_ERR_PARAM_BIT_WIDTH;
        goto END;
    }

    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC_LCD_TCON_PIN_NUM))
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckTcon_Vertical() */

/**************************************************************************//**
 * @fn          CheckTcon_DataEnable
 * @brief       Checks on LCD TCON timing signal parameter for DE timing signal
 * @param[in]   lcd_tcon_timing : LCD TCON timing signal parameter
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckTcon_DataEnable  
                    (const vdc_lcd_tcon_timing_t * const lcd_tcon_timing)
{
    vdc_error_t error;

    error = VDC_OK;

    /* LCD TCON output pin select */
    if ((lcd_tcon_timing->tcon_pin < VDC_LCD_TCON_PIN_NON) || (lcd_tcon_timing->tcon_pin >= VDC_LCD_TCON_PIN_NUM))
    {
        error = VDC_ERR_PARAM_UNDEFINED;
        goto END;
    }
END:
    return error; 
}   /* End of function CheckTcon_DataEnable() */

/**************************************************************************//**
 * @fn          CheckGammaCrct_GainAdjustment
 * @brief       Checks on gain adjustment of area 0 to 31 of G/B/R signal
 * @param[in]   gam_gain        : Gain adjustment of area 0 to 31 of G/B/R signal
 * @retval      Error code
******************************************************************************/
static vdc_error_t CheckGammaCrct_GainAdjustment (const uint16_t * gam_gain) 
{
    uint32_t        gain_index;
    vdc_error_t    error;

    error = VDC_OK;

    /* NULL check */
    if (gam_gain != NULL)
    {
        for (gain_index = 0; gain_index < VDC_GAM_GAIN_ADJ_NUM; gain_index++)
        {
            /* Cast to an appropriate type */
            if (((uint32_t)*gam_gain & (uint32_t)~VDC_CPARA_RANGE_0X000007FF) != 0u)
            {
                error = VDC_ERR_PARAM_BIT_WIDTH;
                goto END;
            }
            gam_gain++;
        }
    }
END:
    return error; 
}   /* End of function CheckGammaCrct_GainAdjustment() */

#endif      /* R_VDC_CHECK_PARAMETERS */

