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
 * File Name :   r_vdc_register.c
 * @file         r_vdc_register.c
 * @version      1.12
 * @brief        RZ/A2M VDC driver register setup processing
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_vdc_register.h"

#include    "r_vdc.h"
#include    "r_vdc_user.h"
#include    "r_vdc_shared_param.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     INITIAL_SCALING_PHASE          ((uint32_t)2048u)
#define     SCALING_RATIO_SAME_SIZE        (4096u)
/* Vsync signal delay control for OIR */
#define     OIR_VSYNC_DELAY                 ((uint32_t)1u)

/* Panel clock frequency division ratio */
#define     VDC_REG_CLKDIV_1_1            (1u)
#define     VDC_REG_CLKDIV_1_2            (2u)
#define     VDC_REG_CLKDIV_1_3            (3u)
#define     VDC_REG_CLKDIV_1_4            (4u)
#define     VDC_REG_CLKDIV_1_5            (5u)
#define     VDC_REG_CLKDIV_1_6            (6u)
#define     VDC_REG_CLKDIV_1_7            (7u)
#define     VDC_REG_CLKDIV_1_8            (8u)
#define     VDC_REG_CLKDIV_1_9            (9u)
#define     VDC_REG_CLKDIV_1_12           (12u)
#define     VDC_REG_CLKDIV_1_16           (16u)
#define     VDC_REG_CLKDIV_1_24           (24u)
#define     VDC_REG_CLKDIV_1_32           (32u)
/* Panel clock select */
#define     VDC_REG_ICKSEL_IMG_DV         (0x0000u)
#define     VDC_REG_ICKSEL_EXT_0          (0x1000u)
#define     VDC_REG_ICKSEL_PERI           (0x3000u)
#define     VDC_REG_ICKSEL_LVDS           (0x0400u)
#define     VDC_REG_ICKSEL_LVDS_DIV7      (0x0800u)
/* Output pin edge select */
#define     VDC_REG_TCON0_EDGE            (0x0040u)
#define     VDC_REG_TCON1_EDGE            (0x0020u)
#define     VDC_REG_TCON2_EDGE            (0x0010u)
#define     VDC_REG_TCON3_EDGE            (0x0008u)
#define     VDC_REG_TCON4_EDGE            (0x0004u)
#define     VDC_REG_TCON5_EDGE            (0x0002u)
#define     VDC_REG_TCON6_EDGE            (0x0001u)
/* Panel dither output format select */
#define     VDC_REG_PATH_FORMAT_RGB888    (0x00000000u)
#define     VDC_REG_PATH_FORMAT_RGB666    (0x00010000u)
#define     VDC_REG_PATH_FORMAT_RGB565    (0x00020000u)
#define     VDC_REG_PATH_FORMAT_NON       (0x00000000u)
/* Calculate scaling ratio use value */
#define     VDC_REG_FLOAT_1_0 (1.0f)
#define     VDC_REG_FLOAT_0_5 (0.5f)
/* bits per pixel */
#define     VDC_REG_BIT_PER_PIXEL_RGB565    (16u)
#define     VDC_REG_BIT_PER_PIXEL_RGB888    (32u)
#define     VDC_REG_BIT_PER_PIXEL_ARGB1555  (16u)
#define     VDC_REG_BIT_PER_PIXEL_ARGB4444  (16u)
#define     VDC_REG_BIT_PER_PIXEL_ARGB8888  (32u)
#define     VDC_REG_BIT_PER_PIXEL_CLUT8     (8u)
#define     VDC_REG_BIT_PER_PIXEL_CLUT4     (4u)
#define     VDC_REG_BIT_PER_PIXEL_CLUT1     (1u)
#define     VDC_REG_BIT_PER_PIXEL_YCBCR422  (32u)
#define     VDC_REG_BIT_PER_PIXEL_YCBCR444  (32u)
#define     VDC_REG_BIT_PER_PIXEL_RGBA5551  (16u)
#define     VDC_REG_BIT_PER_PIXEL_RGBA8888  (32u)
#define     VDC_REG_BIT_PER_PIXEL_VALUE_8   (8u)
/* Convert the color data from 4 bits value into 8 bits value */
#define     VDC_REG_EXTENSION_VALUE_17      (17u)
/* Convert the color data from 5 bits value into 8 bits value */
#define     VDC_REG_EXTENSION_VALUE_263     (263u)
#define     VDC_REG_EXTENSION_VALUE_32      (32u)
/* Convert the color data from 6 bits value into 8 bits value */
#define     VDC_REG_EXTENSION_VALUE_259     (259u)
#define     VDC_REG_EXTENSION_VALUE_64      (64u)
/* Round off */
#define     VDC_REG_ROUND_OFF_VALUE_1       (1u)
#define     VDC_REG_ROUND_OFF_VALUE_2       (2u)
/* Color mask data */
#define     VDC_REG_ALPHA_8BIT              (0xFF000000u)
#define     VDC_REG_RGB_24BIT_MASK          (0x00FFFFFFu)
#define     VDC_REG_COLOR_8BIT_MASK         (0x000000FFu)
#define     VDC_REG_COLOR_6BIT_MASK         (0x0000003Fu)
#define     VDC_REG_COLOR_5BIT_MASK         (0x0000001Fu)
#define     VDC_REG_COLOR_4BIT_MASK         (0x0000000Fu)
#define     VDC_REG_ARGB8888_GB_MASK        (0x00FFFF00u)

/* RGB565 bits mask data */
#define     VDC_REG_RGB565_R_MASK           (0x0000F800u)
#define     VDC_REG_RGB565_G_MASK           (0x000007E0u)
#define     VDC_REG_RGB565_B_MASK           (0x0000001Fu)
/* ARGB4444 bits mask data */
#define     VDC_REG_ARGB4444_R_MASK         (0x00000F00u)
#define     VDC_REG_ARGB4444_G_MASK         (0x000000F0u)
#define     VDC_REG_ARGB4444_B_MASK         (0x0000000Fu)
/* ARGB1555 bits mask data */
#define     VDC_REG_ARGB1555_R_MASK         (0x00007C00u)
#define     VDC_REG_ARGB1555_G_MASK         (0x000003E0u)
#define     VDC_REG_ARGB1555_B_MASK         (0x0000001Fu)
/* RGBA5551 bits mask data */
#define     VDC_REG_RGBA5551_R_MASK         (0x0000F800u)
#define     VDC_REG_RGBA5551_G_MASK         (0x000007C0u)
#define     VDC_REG_RGBA5551_B_MASK         (0x0000003Eu)

/* bit pattern */
#define     VDC_REG_BIT31          (0x80000000u)
#define     VDC_REG_BIT29          (0x20000000u)
#define     VDC_REG_BIT28          (0x10000000u)
#define     VDC_REG_BIT24          (0x01000000u)
#define     VDC_REG_BIT20          (0x00100000u)
#define     VDC_REG_BIT16          (0x00010000u)
#define     VDC_REG_BIT15          (0x00008000u)
#define     VDC_REG_BIT14          (0x00004000u)
#define     VDC_REG_BIT13          (0x00002000u)
#define     VDC_REG_BIT12          (0x00001000u)
#define     VDC_REG_BIT8           (0x00000100u)
#define     VDC_REG_BIT4           (0x00000010u)
#define     VDC_REG_BIT2           (0x00000004u)
#define     VDC_REG_BIT1           (0x00000002u)
#define     VDC_REG_BIT0           (0x00000001u)

/* shift value */
#define     VDC_REG_SHIFT_28       (28u)
#define     VDC_REG_SHIFT_24       (24u)
#define     VDC_REG_SHIFT_20       (20u)
#define     VDC_REG_SHIFT_16       (16u)
#define     VDC_REG_SHIFT_13       (13u)
#define     VDC_REG_SHIFT_12       (12u)
#define     VDC_REG_SHIFT_11       (11u)
#define     VDC_REG_SHIFT_10       (10u)
#define     VDC_REG_SHIFT_8        (8u)
#define     VDC_REG_SHIFT_6        (6u)
#define     VDC_REG_SHIFT_5        (5u)
#define     VDC_REG_SHIFT_4        (4u)
#define     VDC_REG_SHIFT_2        (2u)
#define     VDC_REG_SHIFT_1        (1u)

/* register set value */
#define VDC_REG_BIT_0XFFFF0000     (0xFFFF0000u) /* scaler_0->scl0_frc2                     */
/* register mask value */
#define VDC_REG_MASK_0XF7FF1C3F    (0xF7FF1C3Fu) /* mask img_synthesizer->gr_flm6           */
#define VDC_REG_MASK_0X0000D000    (0x0000D000u) /* mask img_synthesizer->gr_ab1            */
#define VDC_REG_MASK_0X01FF00FF    (0x01FF00FFu) /* mask img_synthesizer->gr_ab6            */
#define VDC_REG_MASK_0X0000007F    (0x0000007Fu) /* mask output_ctrl->out_clk_phase         */
#define VDC_REG_MASK_0X11003313    (0x11003313u) /* mask output_ctrl->out_set               */
#define VDC_REG_MASK_0X00030000    (0x00030000u) /* mask output_ctrl->out_pdtha             */
#define VDC_REG_MASK_0X0007000D    (0x0007000Du) /* mask scaler->scl1_wr1                   */
#define VDC_REG_MASK_0X00000300    (0x00000300u) /* mask scaler->scl1_wr5                   */
#define VDC_REG_MASK_0X0300FF00    (0x0300FF00u) /* mask regaddr_lvds->lclkselr             */
#define VDC_REG_MASK_0X003F0730    (0x003F0730u) /* mask regaddr_lvds->lpllsetr             */
#define VDC_REG_MASK_0X00010301    (0x00010301u) /* mask img_synthesizer->gr_flm1           */
#define VDC_REG_MASK_0X7FFF03FF    (0x7FFF03FFu) /* mask img_synthesizer->gr_flm3           */
#define VDC_REG_MASK_0X0000E000    (0x0000E000u) /* mask img_synthesizer->gr_flm6           */
#define VDC_REG_MASK_0X00007F33    (0x00007F33u) /* mask input_ctrl->imgcnt_nr_cnt0         */
#define VDC_REG_MASK_0X00FF0000    (0x00FF0000u) /* mask img_synthesizer->gr_ab7            */
#define VDC_REG_MASK_0X00300000    (0x00300000u) /* mask output_ctrl->out_pdtha             */
#define VDC_REG_MASK_0X00003333    (0x00003333u) /* mask output_ctrl->out_pdtha             */
#define VDC_REG_MASK_0X00007111    (0x00007111u) /* mask input_ctrl->inp_sel_cnt            */
#define VDC_REG_MASK_0X11110113    (0x11110113u) /* mask input_ctrl->inp_ext_sync_cnt       */
#define VDC_REG_MASK_0X000007FF    (0x000007FFu) /* mask img_synthesizer->gr_flm5           */
#define VDC_REG_MASK_0X0000FFFF    (0x0000FFFFu) /* mask scaler->scl0_ds4                   */
/*      caler->scl0_us5                    */
#define VDC_REG_MASK_0X00000011    (0x00000011u) /* mask scaler->scl0_ds1                   */
/*      scaler->scl0_us1                   */
#define VDC_REG_MASK_0X00000070    (0x00000070u) /* mask scaler->scl1_wr1                   */
#define VDC_REG_MASK_0X00000080    (0x00000080u) /* mask scaler->scl1_wr1                   */
#define VDC_REG_MASK_0X00003110    (0x00003110u) /* mask output_ctrl->tcon_tim_pola2        */
/*      output_ctrl->tcon_tim_polb2        */
#define VDC_REG_MASK_0X000000FF    (0x000000FFu) /* mask scaler_0->scl0_frc5                */
#define VDC_REG_MASK_0X0000003F    (0x0000003Fu) /* mask img_synthesizer->gr_flm6           */
/*      img_qlty_imp->adj_enh_shp1         */
#define VDC_REG_MASK_0XFF000000    (0xFF000000u) /* mask img_synthesizer->gr_ab10           */
/*      img_synthesizer->gr_ab11           */
#define VDC_REG_MASK_0X00000110    (0x00000110u) /* mask output_ctrl->tcon_tim_sth2         */
/*      output_ctrl->tcon_tim_stb2         */
/*      output_ctrl->tcon_tim_cpv2         */
#define VDC_REG_MASK_0X00000007    (0x00000007u) /* mask *(output_pin_regaddr[tcon_pin])    */
/*      img_synthesizer->gr_flm2           */
/*      img_synthesizer->gr_flm2           */
#define VDC_REG_MASK_0X07FF0000    (0x07FF0000u) /* mask scaler_0->scl0_frc4                */
/*      img_synthesizer->gr_flm5           */
/*      img_synthesizer->gr_flm6           */
#define VDC_REG_MASK_0X0FFF0FFF    (0x0FFF0FFFu) /* mask                                    */
/*      scaler->scl0_ds5, scaler->scl0_us6 */
/*      scaler->scl0_ds5, scaler->scl0_us6 */
/*      scaler->scl0_ds5, scaler->scl0_us6 */
/*      scaler->scl0_ds5, scaler->scl0_us6 */
/*      scaler->scl0_ds5, scaler->scl0_us6 */
#define VDC_REG_MASK_0X00000003    (0x00000003u) /* mask img_synthesizer->gr_ab1            */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void SetVideoExternalInput(
    const vdc_ext_in_sig_t         * const ext_sig_tmp,
    const vdc_regaddr_input_ctrl_t * const input_ctrl);
static void SetInitialConnection(void);
static void SetGrapicsBackgroundColor(void);
static void SetScalerBackgroundColor(const vdc_color_space_t color_space);
static void SetLcdTcon_STVA_VS(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_STVB_VE(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_STH_HS(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_STB_HE(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_CPV_GCK(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_POLA(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_POLB(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetLcdTcon_DE(
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl);
static void SetScalingDown(
    const vdc_scalingdown_rot_t    * const scldw_rot,
    const vdc_res_inter_t                  res_inter,
    const vdc_regaddr_scaler_t     * const scaler);
static void SetScalingUp(
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc,
    const vdc_onoff_t                  adj_sel,
    const vdc_onoff_t                  ip_conversion,
    const vdc_wr_md_t                  res_ds_wr_md,
    const vdc_gr_ln_off_dir_t          ln_off_dir,
    const vdc_regaddr_scaler_t * const scaler);
static uint32_t CalcScalingRatio(
    const uint32_t      before_scl,
    const uint32_t      after_scl,
    const vdc_onoff_t  adj_sel,
    const vdc_onoff_t  round_up);
static void SetInputCntrlColorMtx(void);
static vdc_onoff_t ConfirmGraphicsEnlargement(
    const vdc_graphics_type_t          graphics_id,
    const vdc_gr_flm_sel_t             gr_flm_sel,
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc);
static void SetScalerGraphics(
    const vdc_graphics_type_t          graphics_id,
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc,
    const vdc_gr_flm_sel_t             gr_flm_sel,
    const vdc_onoff_t                  gr_enlarge);
static uint32_t DisplayStartPixelSetting(const uint32_t gr_base, const vdc_gr_format_t gr_format);
static void SetImgQaImproverColorMtx(const vdc_color_space_t color_space);

static void SetRwProcAbility_Write_0(const vdc_onoff_t ability);
static void SetRwProcAbility_Read_0(const vdc_onoff_t ability);
static void SetRwProcAbility_Read_2(const vdc_onoff_t ability);
static void SetRwProcAbility_Read_3(const vdc_onoff_t ability);
static void SetRegUpdateRwEnable(const vdc_layer_id_t layer_id);

static void SetGraphicsDisplayMode(const vdc_layer_id_t layer_id, const vdc_gr_disp_sel_t * const gr_disp_sel);
static void SetColorMatrix(
    const vdc_colormtx_mode_t                  mtx_mode,
    const uint16_t                      * const offset,
    const uint16_t                      * const gain,
    const vdc_regaddr_color_matrix_t   * const color_matrix);
static void SetImageEnhancementSharpness(
    const vdc_onoff_t                          shp_h_on,
    const vdc_enhance_sharp_t          * const sharp_param,
    const vdc_regaddr_img_qlty_imp_t   * const img_qlty_imp);
static void SetImageEnhancementLti(
    const vdc_onoff_t                          lti_h_on,
    const vdc_enhance_lti_t            * const lti_param,
    const vdc_regaddr_img_qlty_imp_t   * const img_qlty_imp);
static void SetAlphaRectArea(
    const vdc_pd_disp_rect_t               * const pd_disp_rect,
    const vdc_regaddr_img_synthesizer_t    * const img_synthesizer);
static uint32_t ColorConversion(const vdc_gr_format_t gr_format, const uint32_t input_color);
static uint32_t ColorConversionFrom4to8(const uint32_t color_value);
static uint32_t ColorConversionFrom5to8(const uint32_t color_value);
static uint32_t ColorConversionFrom6to8(const uint32_t color_value);
static uint32_t ColorConversionIntoAgbr(const uint32_t color_value);
static void Set_Clut(const vdc_clut_t * const param, volatile uint32_t * regaddr_clut);
static void Set_StartThreshold_Gamma(const uint8_t * gam_th, volatile uint32_t * const * const gam_area);
static void Set_GainAdjustment_Gamma(const uint16_t * gam_gain, volatile uint32_t * const * const gam_lut);
static void Wait_200_usec(void);

/*! Color matrix offset (DC) adjustment (YG, B, and R) */
static const uint16_t colormtx_offset_adj[VDC_COLORMTX_OFFST_NUM] =
{
    /* Cast to an appropriate type */
    (uint16_t)VDC_COLORCONV_DC_OFFSET, (uint16_t)VDC_COLORCONV_DC_OFFSET, (uint16_t)VDC_COLORCONV_DC_OFFSET
};

/*! Color matrix signal gain adjustment (GG, GB, GR, BG, BB, BR, RG, RB, and RR) */
static const uint16_t colormtx_gain_adj[VDC_COLORMTX_MODE_NUM][VDC_COLORMTX_GAIN_NUM] =
{
    {
            /* GBR to GBR */
        (uint16_t)VDC_COLORCONV_1TIMES_GAIN, 0, 0,

        /* Cast to an appropriate type */
        0, (uint16_t)VDC_COLORCONV_1TIMES_GAIN, 0,

        /* Cast to an appropriate type */
        0, 0, (uint16_t)VDC_COLORCONV_1TIMES_GAIN
    },
    {
        /* GBR to YCbCr */
        (uint16_t)VDC_COLORCONV_Y_G,  (uint16_t)VDC_COLORCONV_Y_B,  (uint16_t)VDC_COLORCONV_Y_R,

        /* Cast to an appropriate type */
        (uint16_t)VDC_COLORCONV_CB_G, (uint16_t)VDC_COLORCONV_CB_B, (uint16_t)VDC_COLORCONV_CB_R,

        /* Cast to an appropriate type */
        (uint16_t)VDC_COLORCONV_CR_G, (uint16_t)VDC_COLORCONV_CR_B, (uint16_t)VDC_COLORCONV_CR_R
    },
    {
        /* YCbCr to GBR */
        (uint16_t)VDC_COLORCONV_G_Y, (uint16_t)VDC_COLORCONV_G_CB, (uint16_t)VDC_COLORCONV_G_CR,

        /* Cast to an appropriate type */
        (uint16_t)VDC_COLORCONV_B_Y, (uint16_t)VDC_COLORCONV_B_CB, (uint16_t)VDC_COLORCONV_B_CR,

        /* Cast to an appropriate type */
        (uint16_t)VDC_COLORCONV_R_Y, (uint16_t)VDC_COLORCONV_R_CB, (uint16_t)VDC_COLORCONV_R_CR
    },
    {
        /* YCbCr to YCbCr */
        (uint16_t)VDC_COLORCONV_1TIMES_GAIN, 0, 0,

        /* Cast to an appropriate type */
        0, (uint16_t)VDC_COLORCONV_1TIMES_GAIN, 0,

        /* Cast to an appropriate type */
        0, 0, (uint16_t)VDC_COLORCONV_1TIMES_GAIN
    }
};

static void (* const rw_proc_function_tbl[VDC_LAYER_ID_NUM])(const vdc_onoff_t ability) =
{
    &SetRwProcAbility_Write_0,          /* Write scaler 0 */
    &SetRwProcAbility_Read_0,           /* Read graphics 0 */
    &SetRwProcAbility_Read_2,           /* Read graphics 2 */
    &SetRwProcAbility_Read_3            /* Read graphics 3 */
};

/**************************************************************************//**
 * @fn          VDC_Initialize
 * @brief       Sets registers for initialization
 * @param[in]   param                   : Initialization parameter
 * @retval      None
 *****************************************************************************/
void VDC_Initialize (const vdc_init_t * const param) 
{
    const vdc_lvds_t         * lvds_tmp; 
    const vdc_regaddr_lvds_t * regaddr_lvds;
    volatile uint32_t           counter;
    uint16_t                    syscnt_panel_clk_tmp;
    volatile uint16_t         * syscnt_panel_clk_reg;
    volatile uint32_t         * inp_sel_cnt_reg;
    static const uint16_t       panel_dcdr_tbl[VDC_PANEL_CLKDIV_NUM] =
    {
        /* Panel clock frequency division ratio */
        (uint16_t)VDC_REG_CLKDIV_1_1, (uint16_t)VDC_REG_CLKDIV_1_2,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_3,  (uint16_t)VDC_REG_CLKDIV_1_4,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_5, (uint16_t)VDC_REG_CLKDIV_1_6,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_7,  (uint16_t)VDC_REG_CLKDIV_1_8,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_9, (uint16_t)VDC_REG_CLKDIV_1_12,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_16, (uint16_t)VDC_REG_CLKDIV_1_24,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_CLKDIV_1_32
    };
    static const uint16_t       panel_icksel_tbl[VDC_PANEL_ICKSEL_NUM] =
    {
        /* Panel clock select */
        (uint16_t)VDC_REG_ICKSEL_IMG_DV, (uint16_t)VDC_REG_ICKSEL_EXT_0,

        /* Cast to an appropriate type */
        (uint16_t)VDC_REG_ICKSEL_PERI, (uint16_t)VDC_REG_ICKSEL_LVDS,   (uint16_t)VDC_REG_ICKSEL_LVDS_DIV7
    };

    syscnt_panel_clk_reg = vdc_regaddr_system_ctrl.syscnt_panel_clk;
    /* Disable */
    syscnt_panel_clk_tmp      = (uint16_t)((uint32_t)*syscnt_panel_clk_reg & (~VDC_REG_BIT8));
    (*syscnt_panel_clk_reg)   = syscnt_panel_clk_tmp;

    /* LVDS */
    lvds_tmp = param->lvds;

    /* NULL check */
    if (lvds_tmp != NULL)
    {
        regaddr_lvds = &vdc_regaddr_lvds;

        /* Output from the LVDS PLL is disabled. */
        (*(regaddr_lvds->lclkselr)) &= (uint32_t)~VDC_REG_BIT4;

        /* Controls power-down for the LVDS PLL: Power-down state */
        (*(regaddr_lvds->lpllsetr)) |= (uint32_t)VDC_REG_BIT0;

        /* Cast to an appropriate type */
        for (counter = 0; counter < (uint32_t)VDC_LVDS_PLL_WAIT_CYCLE; counter++)
        {
            /* This is a delay (1 usec) while waiting for PLL PD to settle. */
            ;
        }

        /* LCLKSELR: LVDS clock select register */
        (*(regaddr_lvds->lclkselr)) &= (uint32_t)~VDC_REG_MASK_0X0300FF00;

        /* The clock input to frequency divider 1 */
        (*(regaddr_lvds->lclkselr)) |= (uint32_t)(lvds_tmp->lvds_in_clk_sel << VDC_REG_SHIFT_24);

        /* The frequency dividing value (NIDIV) for frequency divider 1 */
            /* The frequency dividing value (NODIV) for frequency divider 2 */
        (*(regaddr_lvds->lclkselr)) |= (uint32_t)(lvds_tmp->lvds_odiv_set << VDC_REG_SHIFT_8);

        /* LPLLSETR: LVDS PLL setting register */
        (*(regaddr_lvds->lpllsetr)) &= (uint32_t)~VDC_REG_MASK_0X003F0730;

        /* The frequency dividing value (NFD) for the feedback frequency */
        (*(regaddr_lvds->lpllsetr)) |= (uint32_t)(lvds_tmp->lvdspll_fd << VDC_REG_SHIFT_16);

        /* The frequency dividing value (NRD) for the input frequency */
        (*(regaddr_lvds->lpllsetr)) |= (uint32_t)(lvds_tmp->lvdspll_rd << VDC_REG_SHIFT_8);

        /* The frequency dividing value (NOD) for the output frequency */
        (*(regaddr_lvds->lpllsetr)) |= (uint32_t)(1u << VDC_REG_SHIFT_4);

        /* Cast to an appropriate type */
        for (counter = 0; counter < (uint32_t)VDC_LVDS_PLL_WAIT_CYCLE; counter++)
        {
            /* This is a delay (1 usec) while waiting for PLL PD to settle. */
            ;
        }

        /* Controls power-down for the LVDS PLL: Normal operation */
        (*(regaddr_lvds->lpllsetr)) &= (uint32_t)~VDC_REG_BIT0;

        /* Wait for 200 usec. */
        Wait_200_usec();

        /* Output from the LVDS PLL is enabled. */
        (*(regaddr_lvds->lclkselr)) |= (uint32_t)VDC_REG_BIT4;
    }

    inp_sel_cnt_reg = vdc_regaddr_input_ctrl.inp_sel_cnt;

    /* Input select */
    if (param->panel_icksel == VDC_PANEL_ICKSEL_IMG_DV)
    {
        /* Cast to an appropriate type */
        (*inp_sel_cnt_reg) |= (uint32_t)VDC_REG_BIT20;
    }
    else
    {
        /* Cast to an appropriate type */
        (*inp_sel_cnt_reg) &= (uint32_t)~VDC_REG_BIT20;
    }
    syscnt_panel_clk_tmp    = panel_dcdr_tbl[param->panel_dcdr];

    /* Cast to an appropriate type */
    syscnt_panel_clk_tmp    = (uint16_t)((uint32_t)syscnt_panel_clk_tmp |

            /* Cast to an appropriate type */
                                        (uint32_t)panel_icksel_tbl[param->panel_icksel]);
    *syscnt_panel_clk_reg   = syscnt_panel_clk_tmp;

    /* Enable */
    syscnt_panel_clk_tmp    = (uint16_t)((uint32_t)syscnt_panel_clk_tmp | VDC_REG_BIT8);
    (*syscnt_panel_clk_reg)   = syscnt_panel_clk_tmp;

}   /* End of function VDC_Initialize() */

/**************************************************************************//**
 * @fn          VDC_Terminate
 * @brief       Sets registers for termination
 * @param[in]   void
 * @retval      None
******************************************************************************/
void VDC_Terminate (void) 
{
    volatile uint16_t         * syscnt_panel_clk_reg;
    uint16_t                    syscnt_panel_clk_tmp;
    vdc_resource_state_t       rsrc_state;
    vdc_onoff_t                lvds_ref;
    const vdc_regaddr_lvds_t * regaddr_lvds;
    volatile uint32_t           counter;

    syscnt_panel_clk_reg = vdc_regaddr_system_ctrl.syscnt_panel_clk;

    /* Disable */
    syscnt_panel_clk_tmp      = (uint16_t)((uint32_t)*syscnt_panel_clk_reg & (~VDC_REG_BIT8));
    (*syscnt_panel_clk_reg)   = syscnt_panel_clk_tmp;

    rsrc_state = VDC_ShrdPrmGetResource(VDC_RESOURCE_LVDS_CLK);
    if (rsrc_state != VDC_RESOURCE_ST_INVALID)         /* LVDS PLL clock is set. */
    {
        lvds_ref = VDC_ShrdPrmGetLvdsClkRef();
        if (lvds_ref == VDC_OFF)       /* LVDS PLL clock is not referred. */
        {
            regaddr_lvds = &vdc_regaddr_lvds;

            /* Output from the LVDS PLL is disabled. */
            (*(regaddr_lvds->lclkselr)) &= (uint32_t)~VDC_REG_BIT4;

            /* Controls power-down for the LVDS PLL: Power-down state */
            (*(regaddr_lvds->lpllsetr)) |= (uint32_t)VDC_REG_BIT0;

            /* Cast to an appropriate type */
            for (counter = 0; counter < (uint32_t)VDC_LVDS_PLL_WAIT_CYCLE; counter++)
            {
                /* This is a delay (1 usec) while waiting for PLL PD to settle. */
                ;
            }
        }
    }
}   /* End of function VDC_Terminate() */

/**************************************************************************//**
 * @fn          VDC_VideoInput
 * @brief       Sets registers for video input
 * @param[in]   param                   : Video input setup parameter
 * @retval      None
******************************************************************************/
void VDC_VideoInput (const vdc_input_t * const param) 
{
    const vdc_regaddr_input_ctrl_t   * input_ctrl;
    const vdc_sync_delay_t           * dly_tmp;
    const vdc_ext_in_sig_t           * ext_sig_tmp;

    input_ctrl = &vdc_regaddr_input_ctrl;

    /* Input select */
    (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)VDC_REG_BIT20;

    /* Vsync signal 1/2fH and 1/4fH phase timing */
    (*(input_ctrl->inp_vsync_ph_adj))  = (uint32_t)param->inp_fh50 << VDC_REG_SHIFT_16;

    /* Cast to an appropriate type */
    (*(input_ctrl->inp_vsync_ph_adj)) |= (uint32_t)param->inp_fh25;

    /* Sync signal delay adjustment */
    dly_tmp = param->dly;

    /* NULL check */
    if (dly_tmp != NULL)
    {
        /* Number of lines for delaying Vsync signal and field differentiation signal */
        (*(input_ctrl->inp_dly_adj))  = (uint32_t)(dly_tmp->inp_vs_dly_l << VDC_REG_SHIFT_24);

        /* Field differentiation signal delay amount */
        (*(input_ctrl->inp_dly_adj)) |= (uint32_t)(dly_tmp->inp_fld_dly << VDC_REG_SHIFT_16);

        /* Vsync signal delay amount */
        (*(input_ctrl->inp_dly_adj)) |= (uint32_t)(dly_tmp->inp_vs_dly << VDC_REG_SHIFT_8);

        /* Hsync signal delay amount */
        (*(input_ctrl->inp_dly_adj)) |= (uint32_t)(dly_tmp->inp_hs_dly);
    }

    /* Horizontal noise reduction operating mode, Y/Cb/Cr mode */
    (*(input_ctrl->imgcnt_nr_cnt0)) |= (uint32_t)VDC_REG_BIT20;

    /* External input signal */
    ext_sig_tmp = param->ext_sig;

    /* NULL check */
    if (ext_sig_tmp != NULL)
    {
        /* Setting external input video signal */
        SetVideoExternalInput(ext_sig_tmp, input_ctrl);

        /* Horizontal noise reduction operating mode, G/B/R mode */
        if ((ext_sig_tmp->inp_format == VDC_EXTIN_FORMAT_RGB888) ||
            (ext_sig_tmp->inp_format == VDC_EXTIN_FORMAT_RGB666) ||
            (ext_sig_tmp->inp_format == VDC_EXTIN_FORMAT_RGB565))
        {
            /* Cast to an appropriate type */
            (*(input_ctrl->imgcnt_nr_cnt0)) &= (uint32_t)~VDC_REG_BIT20;
        }
    }

    /* Register update control register (INP_UPDATE)
       b4       INP_EXT_UPDATE
       b0       INP_IMG_UPDATE */
    (*(input_ctrl->inp_update)) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);

    /* Register update control register (IMGCNT_UPDATE)
       b0       IMGCNT_VEN */
    (*(input_ctrl->imgcnt_update)) |= (uint32_t)VDC_REG_BIT0;
}   /* End of function VDC_VideoInput() */

/**************************************************************************//**
 * @fn          VDC_SyncControl
 * @brief       Sets registers for sync control
 * @param[in]   param                   : Sync signal control parameter
 * @retval      None
******************************************************************************/
void VDC_SyncControl (const vdc_sync_ctrl_t * const param) 
{
    const vdc_regaddr_scaler_t   * scaler_0;
    uint32_t                        full_scr;

    scaler_0    = &vdc_regaddr_scaler;

    /* NULL check */
    if (param->vsync_cpmpe != NULL)
    {
        /* Frequent Vsync signal masking period */
        (*(scaler_0->scl0_frc1)) = (uint32_t)(param->vsync_cpmpe->res_vmask << VDC_REG_SHIFT_16);

        /* Missing-Sync compensating pulse output wait time */
        (*(scaler_0->scl0_frc2)) = (uint32_t)(param->vsync_cpmpe->res_vlack << VDC_REG_SHIFT_16);

        /* Frequent Vsync signal masking and missing Vsync signal compensation */
        (*(scaler_0->scl0_frc1)) |= (uint32_t)VDC_REG_BIT0;

        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc2)) |= (uint32_t)VDC_REG_BIT0;
    }
    else
    {
        /* Frequent Vsync signal masking and missing Vsync signal compensation are disabled, if unnecessary. */
        (*(scaler_0->scl0_frc1)) &= (uint32_t)~VDC_REG_BIT0;

        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc2)) &= (uint32_t)~VDC_REG_BIT0;

        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc2)) |= (uint32_t)VDC_REG_BIT_0XFFFF0000;
    }

    /* Hsync period setting and free-running Vsync period initialization */
    (*(scaler_0->scl0_frc4))      = (uint32_t)param->res_fh;

    /* Vsync signal output select (free-running Vsync on/off control) */
    if (param->res_vs_sel == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc3)) &= (uint32_t)~VDC_REG_BIT0;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc3)) |= (uint32_t)VDC_REG_BIT0;

        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc4)) &= (uint32_t)~VDC_REG_MASK_0X07FF0000;

        /* Cast to an appropriate type */
        (*(scaler_0->scl0_frc4)) |= (uint32_t)(param->res_fv << VDC_REG_SHIFT_16);
    }

    /* Vsync signal delay control */
    (*(scaler_0->scl0_frc5)) &= (uint32_t)~VDC_REG_MASK_0X000000FF;

    /* Cast to an appropriate type */
    (*(scaler_0->scl0_frc5)) |= (uint32_t)param->res_vsdly;

    /* Full-screen enable signal */
    full_scr  = (uint32_t)param->res_f.vs << VDC_REG_SHIFT_16;

    /* Cast to an appropriate type */
    full_scr |= (uint32_t)param->res_f.vw;

    (*(scaler_0->scl0_frc6))   = full_scr;

    /* Cast to an appropriate type */
    full_scr  = (uint32_t)param->res_f.hs << VDC_REG_SHIFT_16;

    /* Cast to an appropriate type */
    full_scr |= (uint32_t)param->res_f.hw;

    (*(scaler_0->scl0_frc7))   = full_scr;

    /* Register update control register (SC0_SCL0_UPDATE, SC1_SCL0_UPDATE, OIR_SCL0_UPDATE)
       b8       SCL0_UPDATE
       b4       SCL0_VEN_B */
    (*(scaler_0->scl0_update))   |= (uint32_t)(VDC_REG_BIT8|VDC_REG_BIT4);

    SetInitialConnection();
}   /* End of function VDC_SyncControl() */

/**************************************************************************//**
 * @fn          VDC_DisplayOutput
 * @brief       Sets registers for display output
 * @param[in]   param                   : Display output configuration parameter
 * @retval      None
******************************************************************************/
void VDC_DisplayOutput (const vdc_output_t * const param) 
{
    const vdc_regaddr_output_ctrl_t      * output_ctrl;
    volatile uint32_t                     * output_pin_regaddr[VDC_LCD_TCON_PIN_NUM];
    uint32_t                                tcon_sig_index;
    const vdc_lcd_tcon_timing_t  * const * lcd_tcon_timing;
    static void                    (* const tcon_function_tbl[VDC_LCD_TCONSIG_NUM])(
        const vdc_lcd_tcon_timing_t        * const lcd_tcon,
        const vdc_regaddr_output_ctrl_t    * const output_ctrl) =
    {
        &SetLcdTcon_STVA_VS,    /* STVA/VS */
        &SetLcdTcon_STVB_VE,    /* STVB/VE */
        &SetLcdTcon_STH_HS,     /* STH_HS */
        &SetLcdTcon_STB_HE,     /* STB_HE */
        &SetLcdTcon_CPV_GCK,    /* CPV_GCK */
        &SetLcdTcon_POLA,       /* POLA */
        &SetLcdTcon_POLB,       /* POLB */
        &SetLcdTcon_DE          /* DE */
    };
    const uint32_t                      output_pin_edge[VDC_LCD_TCON_PIN_NUM] =
    {
        VDC_REG_TCON0_EDGE, VDC_REG_TCON1_EDGE, VDC_REG_TCON2_EDGE, VDC_REG_TCON3_EDGE,
        VDC_REG_TCON4_EDGE, VDC_REG_TCON5_EDGE, VDC_REG_TCON6_EDGE
    };
    vdc_lcd_tcon_pin_t                 tcon_pin_tmp;
    const vdc_gr_disp_sel_t            gr_disp_sel_tbl[VDC_GR_TYPE_NUM] =
    {
            /* Graphics display mode for initialization */
        VDC_DISPSEL_BACK,              /* Graphics 0 */
        VDC_DISPSEL_LOWER,             /* Graphics 2 */
        VDC_DISPSEL_LOWER              /* Graphics 3 */
    };
    const uint32_t                      dither_format[VDC_LCD_OUTFORMAT_NUM] =
    {
        /* Panel dither output format select */
        VDC_REG_PATH_FORMAT_RGB888, VDC_REG_PATH_FORMAT_RGB666,
        VDC_REG_PATH_FORMAT_RGB565, VDC_REG_PATH_FORMAT_NON
    };

    output_ctrl = &vdc_regaddr_output_ctrl;

    /* 1/2fH timing */
    (*(output_ctrl->tcon_tim))  = (uint32_t)param->tcon_half << VDC_REG_SHIFT_16;

    /* Offset Hsync signal timing */
    (*(output_ctrl->tcon_tim)) |= (uint32_t)param->tcon_offset;

    /* Create output pin register address table */
    output_pin_regaddr[VDC_LCD_TCON_PIN_0] = output_ctrl->tcon_tim_stva2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_1] = output_ctrl->tcon_tim_stvb2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_2] = output_ctrl->tcon_tim_sth2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_3] = output_ctrl->tcon_tim_stb2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_4] = output_ctrl->tcon_tim_cpv2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_5] = output_ctrl->tcon_tim_pola2;
    output_pin_regaddr[VDC_LCD_TCON_PIN_6] = output_ctrl->tcon_tim_polb2;

    /* Clear output phase control of LCT_TCON0~6 pin */
    (*(output_ctrl->out_clk_phase)) &= (uint32_t)~VDC_REG_MASK_0X0000007F;

    /* LCD TCON */
    lcd_tcon_timing = param->outctrl;
    for (tcon_sig_index = 0; tcon_sig_index < VDC_LCD_TCONSIG_NUM; tcon_sig_index++)
    {
        /* NULL check */
        if ((*lcd_tcon_timing) != NULL)
        {
            /* Setting LCD TCON */
            tcon_function_tbl[tcon_sig_index](*lcd_tcon_timing, output_ctrl);

            tcon_pin_tmp = (*lcd_tcon_timing)->tcon_pin;
            if (tcon_pin_tmp != VDC_LCD_TCON_PIN_NON)
            {
                /* LCD TCON output pin select */
                (*(output_pin_regaddr[tcon_pin_tmp])) &= (uint32_t)~VDC_REG_MASK_0X00000007;
                (*(output_pin_regaddr[tcon_pin_tmp])) |= tcon_sig_index;

                /* Output phase control of LCT_TCON pin */
                if ((*lcd_tcon_timing)->outcnt_edge != VDC_EDGE_RISING)
                {
                    (*(output_ctrl->out_clk_phase)) |= output_pin_edge[tcon_pin_tmp];
                }
            }
        }
        lcd_tcon_timing++;
    }

    /* Cast to an appropriate type */
    (*(output_ctrl->out_set)) &= (uint32_t)~VDC_REG_MASK_0X11003313;

    /* Bit endian change ON/OFF control */
    if (param->out_endian_on != VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_set)) |= (uint32_t)VDC_REG_BIT28;
    }

    /* B/R signal swap ON/OFF control */
    if (param->out_swap_on != VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_set)) |= (uint32_t)VDC_REG_BIT24;
    }

    /* Output format select */
    (*(output_ctrl->out_set)) |= (uint32_t)(param->out_format << VDC_REG_SHIFT_12);

    /* Panel dither output format select */
    (*(output_ctrl->out_pdtha)) &= (uint32_t)~VDC_REG_MASK_0X00030000;
    (*(output_ctrl->out_pdtha)) |= dither_format[param->out_format];

    if (param->out_format == VDC_LCD_OUTFORMAT_SERIAL_RGB)
    {
        /* Clock frequency control */
        (*(output_ctrl->out_set)) |= (uint32_t)(param->out_frq_sel << VDC_REG_SHIFT_8);

        /* Scan direction select */
        if (param->out_dir_sel != VDC_LCD_SERIAL_SCAN_FORWARD)
        {
            /* Cast to an appropriate type */
            (*(output_ctrl->out_set)) |= (uint32_t)VDC_REG_BIT4;
        }

        /* Clock phase adjustment */
        (*(output_ctrl->out_set)) |= (uint32_t)param->out_phase;
    }
    else
    {
        /* Clock frequency control */
        (*(output_ctrl->out_set)) |= (uint32_t)(VDC_LCD_PARALLEL_CLKFRQ_1 << VDC_REG_SHIFT_8);
    }

    /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
    if (param->outcnt_lcd_edge == VDC_EDGE_RISING)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_clk_phase)) &= (uint32_t)~VDC_REG_BIT8;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_clk_phase)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Set background color */
    SetGrapicsBackgroundColor();

    /* Initialize graphics display mode */
    SetGraphicsDisplayMode(VDC_LAYER_ID_ALL, gr_disp_sel_tbl);

    /* TCON register update control register (TCON_UPDATE)
       b0       TCON_VEN */
    (*(output_ctrl->tcon_update)) |= (uint32_t)VDC_REG_BIT0;

    /* Register update control register (OUT_UPDATE)
       b0       OUTCNT_VEN */
    (*(output_ctrl->out_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function VDC_DisplayOutput() */

/**************************************************************************//**
 * @fn          VDC_WriteDataControl
 * @brief       Sets registers for data write control processing
 * @param[in]   param                   : Data write control parameter
 * @retval      None
******************************************************************************/
void VDC_WriteDataControl (const vdc_write_t * const param) 
{
    const vdc_regaddr_scaler_t   * scaler;
    const vdc_scalingdown_rot_t  * scldw_rot;

    scaler      = &vdc_regaddr_scaler;
    scldw_rot   = &param->scalingdown_rot;

    /* Image area to be captured */
    (*(scaler->scl0_ds2))  = (uint32_t)(scldw_rot->res.vs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds2)) |= (uint32_t)scldw_rot->res.vw;

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds3))  = (uint32_t)(scldw_rot->res.hs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds3)) |= (uint32_t)scldw_rot->res.hw;

    /* Cast to an appropriate type */
    (*(scaler->scl1_wr1)) &= (uint32_t)~VDC_REG_MASK_0X0007000D;

    /* Swap setting in frame buffer writing */
    (*(scaler->scl1_wr1)) |= (uint32_t)(param->res_wrswa << VDC_REG_SHIFT_16);

    /* Frame buffer video-signal writing format */
    (*(scaler->scl1_wr1)) |= (uint32_t)(param->res_md << VDC_REG_SHIFT_2);

    /* Transfer burst length for frame buffer writing */
    if (param->res_bst_md != VDC_BST_MD_32BYTE)
    {
        /* Cast to an appropriate type */
        (*(scaler->scl1_wr1)) |= (uint32_t)VDC_REG_BIT0;
    }

    /* Frame buffer base address */
    (*(scaler->scl1_wr2)) = R_VDC_CPUVAddrToSysPAddr( (uint32_t)param->base) ;

    /* Frame buffer line offset address[byte] */
    (*(scaler->scl1_wr3)) = (param->ln_off << VDC_REG_SHIFT_16);

    /* Number of frames of buffer to be written to (res_flm_num + 1) */
    (*(scaler->scl1_wr3)) |= param->flm_num;

    /* Frame buffer frame offset address [byte] */
    if (param->flm_num != 0u)
    {
        (*(scaler->scl1_wr4)) = param->flm_off;
    }

    /* Writing rate */
    (*(scaler->scl1_wr5)) &= (uint32_t)~VDC_REG_MASK_0X00000300;

    /* Cast to an appropriate type */
    (*(scaler->scl1_wr5)) |= (uint32_t)(param->res_fs_rate << VDC_REG_SHIFT_8);

    /* Dither correction on/off */
    if (param->res_dth_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(scaler->scl1_wr6)) &= (uint32_t)~VDC_REG_BIT4;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(scaler->scl1_wr6)) |= (uint32_t)VDC_REG_BIT4;
    }

    /* Bit Reduction */
    if (param->res_md == VDC_RES_MD_RGB565)
    {
        /* On */
        (*(scaler->scl1_wr6)) |= (uint32_t)VDC_REG_BIT0;
    }
    else
    {
        /* Off */
        (*(scaler->scl1_wr6)) &= (uint32_t)~VDC_REG_BIT0;
    }

    /* Scaler 0 or 1 */
    /* Field determination signal delay control */
    if (param->flm_num == 0u)
    {
        /* Frame buffer: One plane */
        (*(scaler->scl0_frc5)) &= (uint32_t)~VDC_REG_BIT8;
    }
    else
    {
        /* Frame buffer: Two planes or more */
        (*(scaler->scl0_frc5)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Horizontal prefilter */
    if (scldw_rot->res_pfil_sel == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds4)) &= (uint32_t)~VDC_REG_BIT29;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds4)) |= (uint32_t)VDC_REG_BIT29;
    }

    /* Number of valid lines in vertical direction output by scaling-down control block */
    (*(scaler->scl0_ds7))  = (uint32_t)(scldw_rot->res_out_vw << VDC_REG_SHIFT_16);

    /* Number of valid horizontal pixels output by scaling-down control block */
    (*(scaler->scl0_ds7)) |= (uint32_t)scldw_rot->res_out_hw;

    /* Scaling-down */
    SetScalingDown(scldw_rot, param->res_inter, scaler);

    /* Frame buffer writing mode for image processing */
    (*(scaler->scl1_wr1)) &= (uint32_t)~VDC_REG_MASK_0X00000070;

    /* Cast to an appropriate type */
    (*(scaler->scl1_wr1)) |= (uint32_t)(scldw_rot->res_ds_wr_md << VDC_REG_SHIFT_4);

    /* Field operating mode select */
    if (param->res_inter == VDC_RES_INTER_PROGRESSIVE)
    {
        /* Progressive */
        (*(scaler->scl1_wr5)) &= (uint32_t)~VDC_REG_BIT12;
    }
    else
    {
        /* Interlace */
        (*(scaler->scl1_wr5)) |= (uint32_t)VDC_REG_BIT12;
    }

    /* Write field select */
    if (param->res_fld_sel == VDC_RES_FLD_SEL_TOP)
    {
        /* Top field */
        (*(scaler->scl1_wr5)) &= (uint32_t)~VDC_REG_BIT4;
    }
    else
    {
        /* Bottom field */
        (*(scaler->scl1_wr5)) |= (uint32_t)VDC_REG_BIT4;
    }

    /* Frame buffer for bottom */
    if (param->btm_base != NULL)
    {
        /* SC_RES_TB_ADD_MOD */
        (*(scaler->scl1_wr1)) |= (uint32_t)VDC_REG_MASK_0X00000080;

        /* Frame buffer base address */
        (*(scaler->scl1_wr8)) = (uint32_t)param->btm_base;

        /* Frame buffer line offset address[byte] */
        (*(scaler->scl1_wr9))  = (param->ln_off << VDC_REG_SHIFT_16);

        /* The number of frames of buffer (defined by res_flm_num + 1) */
        (*(scaler->scl1_wr9)) |= param->flm_num;

        /* Frame buffer frame offset address */
        if (param->flm_num != 0u)
        {
            (*(scaler->scl1_wr10)) = param->flm_off;
        }
    }
    else
    {
        /* SC_RES_TB_ADD_MOD */
        (*(scaler->scl1_wr1)) &= (uint32_t)~VDC_REG_MASK_0X00000080;
    }

    /* Register update control register (SC0_SCL0_UPDATE)
       b12      SCL0_VEN_C
       b4       SCL0_VEN_B
       b0       SCL0_VEN_A */
    (*(scaler->scl0_update))   |= (uint32_t)(VDC_REG_BIT12 | VDC_REG_BIT4 | VDC_REG_BIT0);

    /* Register update control register (SC0_SCL1_UPDATE)
       b16      SCL1_UPDATE_A
       b4       SCL1_VEN_B
       b0       SCL1_VEN_A */
    (*(scaler->scl1_update))   |= (uint32_t)(VDC_REG_BIT16 | VDC_REG_BIT4 | VDC_REG_BIT0);

    SetInputCntrlColorMtx();
}   /* End of function VDC_WriteDataControl() */

/**************************************************************************//**
 * @fn          VDC_ChangeWriteProcess
 * @brief       Sets registers for data write change processing
 * @param[in]   param                   : Data write change parameter
 * @retval      None
******************************************************************************/
void VDC_ChangeWriteProcess (const vdc_write_chg_t * const param) 
{
    const vdc_regaddr_scaler_t   * scaler;
    const vdc_scalingdown_rot_t  * scldw_rot;
    vdc_res_inter_t                res_inter_tmp;

    scaler      = &vdc_regaddr_scaler;
    scldw_rot   = &param->scalingdown_rot;

    /* Image area to be captured */
    (*(scaler->scl0_ds2))  = (uint32_t)(scldw_rot->res.vs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds2)) |= (uint32_t)scldw_rot->res.vw;

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds3))  = (uint32_t)(scldw_rot->res.hs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(scaler->scl0_ds3)) |= (uint32_t)scldw_rot->res.hw;

    /* Horizontal prefilter */
    if (scldw_rot->res_pfil_sel == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds4)) &= (uint32_t)~VDC_REG_BIT29;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds4)) |= (uint32_t)VDC_REG_BIT29;
    }

    /* Number of valid lines in vertical direction output by scaling-down control block */
    (*(scaler->scl0_ds7))  = (uint32_t)(scldw_rot->res_out_vw << VDC_REG_SHIFT_16);

    /* Number of valid horizontal pixels output by scaling-down control block */
    (*(scaler->scl0_ds7)) |= (uint32_t)scldw_rot->res_out_hw;

    /* Progressive or interlace */
    res_inter_tmp = VDC_ShrdPrmGetInterlace();

    /* Scaling-down */
    SetScalingDown(scldw_rot, res_inter_tmp, scaler);

    /* Frame buffer writing mode for image processing */
    (*(scaler->scl1_wr1)) &= (uint32_t)~VDC_REG_MASK_0X00000070;

    /* Cast to an appropriate type */
    (*(scaler->scl1_wr1)) |= (uint32_t)(scldw_rot->res_ds_wr_md << VDC_REG_SHIFT_4);

    /* Register update control register (SC0_SCL1_UPDATE)
       b4       SCL1_VEN_B
       b0       SCL1_VEN_A */
    (*(scaler->scl1_update))   |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);

    /* Register update control register (SC0_SCL0_UPDATE)
       b12      SCL0_VEN_C
       b4       SCL0_VEN_B
       b0       SCL0_VEN_A */
    (*(scaler->scl0_update))   |= (uint32_t)(VDC_REG_BIT12 | VDC_REG_BIT4 | VDC_REG_BIT0);

}   /* End of function VDC_ChangeWriteProcess() */

/**************************************************************************//**
 * @fn          VDC_ReadDataControl
 * @brief       Sets registers for data read control processing
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read control parameter
 * @retval      None
******************************************************************************/
void VDC_ReadDataControl  
    (const vdc_graphics_type_t graphics_id, const vdc_read_t * const param)
{
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;
    const vdc_regaddr_img_qlty_imp_t     * img_qlty_imp;
    vdc_width_read_fb_t                  * w_read_fb;
    vdc_onoff_t                            graphics_enlargement;
    vdc_color_space_t                      color_space;
    uint32_t                                reg_data;

    w_read_fb = VDC_ShrdPrmGetFrBuffWidth_Rd(graphics_id);

    graphics_enlargement = ConfirmGraphicsEnlargement(graphics_id, param->gr_flm_sel, w_read_fb, &param->gr_grc);

    if (graphics_id == VDC_GR_TYPE_GR0)
    {
        SetScalerGraphics(graphics_id, w_read_fb, &param->gr_grc, param->gr_flm_sel, graphics_enlargement);
    }

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

    /* Cast to an appropriate type */
    (*(img_synthesizer->gr_flm1)) &= (uint32_t)~VDC_REG_MASK_0X00010301;

    /* Line offset address direction of the frame buffer */
    if (param->gr_ln_off_dir != VDC_GR_LN_OFF_DIR_INC)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_flm1)) |= (uint32_t)VDC_REG_BIT16;
    }

    /* Frame buffer address setting signal */
    (*(img_synthesizer->gr_flm1)) |= (uint32_t)(param->gr_flm_sel << VDC_REG_SHIFT_8);

    /* Frame buffer burst transfer mode */
    if (param->gr_bst_md != VDC_BST_MD_32BYTE)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_flm1)) |= (uint32_t)VDC_REG_BIT0;
    }

    /* Sets the frame buffer number for distortion correction */
    if (param->gr_flm_sel == VDC_GR_FLM_SEL_DISTORTION)
    {
        if (param->gr_imr_flm_inv == VDC_OFF)
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_flm1)) &= (uint32_t)~VDC_REG_BIT4;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_flm1)) |= (uint32_t)VDC_REG_BIT4;
        }
    }

    /* Frame buffer base address */
    (*(img_synthesizer->gr_flm2))

        /* Cast to an appropriate type */
        = (uint32_t)(R_VDC_CPUVAddrToSysPAddr((uint32_t)param->gr_base) & (uint32_t)~VDC_REG_MASK_0X00000007);

    /* Frame buffer line offset address[byte] */
    (*(img_synthesizer->gr_flm3)) &= (uint32_t)~VDC_REG_MASK_0X7FFF03FF;
    (*(img_synthesizer->gr_flm3)) |= (param->gr_ln_off << VDC_REG_SHIFT_16);

    /* The number of lines when reading the addresses repeatedly */
    (*(img_synthesizer->gr_flm5)) = (uint32_t)VDC_REG_MASK_0X000007FF;

    /* Number of lines in a frame */
    (*(img_synthesizer->gr_flm5)) |= (((uint32_t)w_read_fb->in_vw - 1u) << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(img_synthesizer->gr_flm6)) &= (uint32_t)~VDC_REG_MASK_0XF7FF1C3F;

    /* Format of the frame buffer read signal */
    (*(img_synthesizer->gr_flm6)) |= ((uint32_t)param->gr_format << VDC_REG_SHIFT_28);

    /* Width of the horizontal valid period */
    (*(img_synthesizer->gr_flm6)) |= (((uint32_t)w_read_fb->in_hw - 1u) << VDC_REG_SHIFT_16);

    /* Swap setting in frame buffer reading */
    (*(img_synthesizer->gr_flm6)) |= ((uint32_t)param->gr_rdswa << VDC_REG_SHIFT_10);

    if ((param->gr_flm_sel == VDC_GR_FLM_SEL_SCALE_DOWN) || (param->gr_flm_sel == VDC_GR_FLM_SEL_FLM_NUM))
    {
        /* Sets the amount of data to be skipped through */
        (*(img_synthesizer->gr_flm6)) |= DisplayStartPixelSetting((uint32_t)param->gr_base, param->gr_format);
    }

    /* Graphics display mode */
    (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;
    if (graphics_id == VDC_GR_TYPE_GR0)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_DISPSEL_BACK;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_DISPSEL_LOWER;
    }

    /* Graphics display area */
    (*(img_synthesizer->gr_ab2))  = ((uint32_t)param->gr_grc.vs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(img_synthesizer->gr_ab2)) |= (uint32_t)param->gr_grc.vw;

    /* Cast to an appropriate type */
    (*(img_synthesizer->gr_ab3))  = ((uint32_t)param->gr_grc.hs << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(img_synthesizer->gr_ab3)) |= (uint32_t)param->gr_grc.hw;

    /* Initialize alpha blending in a rectangular area */
    if ((graphics_id == VDC_GR_TYPE_GR2) || (graphics_id == VDC_GR_TYPE_GR3))
    {
        /* Turns off alpha blending in a rectangular area */
        (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X0000D000;

        /* The valid image area for alpha blending in a rectangular area */
        reg_data                     = (*(img_synthesizer->gr_ab2));
        (*(img_synthesizer->gr_ab4)) = reg_data;
        reg_data                     = (*(img_synthesizer->gr_ab3));
        (*(img_synthesizer->gr_ab5)) = reg_data;

        /* Disable fade-in and fade-out */
        (*(img_synthesizer->gr_ab6)) &= (uint32_t)~VDC_REG_MASK_0X01FF00FF;

        /* Set the initial alpha value to '255' */
        (*(img_synthesizer->gr_ab7)) |= (uint32_t)VDC_REG_MASK_0X00FF0000;
    }

    /* Turns off  chroma-key processing */
    (*(img_synthesizer->gr_ab7)) &= (uint32_t)~VDC_REG_BIT0;

    if (graphics_id == VDC_GR_TYPE_GR0)
    {
        /* Alpha value is set to 255. */
        (*(img_synthesizer->gr_ab10)) |= (uint32_t)VDC_REG_ALPHA_8BIT;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab11)) |= (uint32_t)VDC_REG_ALPHA_8BIT;

        /* Swapping of data read from buffer in the YCbCr422 format */
        (*(img_synthesizer->gr_flm6)) &= (uint32_t)~VDC_REG_MASK_0X0000E000;
        if (param->gr_format == VDC_GR_FORMAT_YCBCR422)
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_flm6)) |= ((uint32_t)param->gr_ycc_swap << VDC_REG_SHIFT_13);

            /* The interpolation mode for YCC422 to YCbCr444 conversion is fixed to average interpolation */
            (*(img_synthesizer->gr_flm6)) |= (uint32_t)VDC_REG_BIT8;
        }
        else
        {
            /* If the format of the frame buffer read signal is not YCbCr422,
               initialize the swapping of data read from buffer in the YCbCr422 format. */
            (*(img_synthesizer->gr_flm6)) |= ((uint32_t)VDC_GR_YCCSWAP_CBY0CRY1 << VDC_REG_SHIFT_13);
        }

        color_space = VDC_ShrdPrmGetColorSpaceFbRd(graphics_id);

        SetScalerBackgroundColor(color_space);
        SetImgQaImproverColorMtx(color_space);

        img_qlty_imp = &vdc_regaddr_img_qlty_imp;

        /* Operating mode */
        if (color_space == VDC_COLOR_SPACE_GBR)
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_tim1)) &= (uint32_t)~VDC_REG_BIT4;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_tim1)) |= (uint32_t)VDC_REG_BIT4;
        }

        /* Register update control register (ADJx_UPDATE)
           b0       ADJx_VEN */
        (*(img_qlty_imp->adj_update)) |= (uint32_t)VDC_REG_BIT0;
    }

    /* Graphics register update control register (GRx_UPDATE)
       b8       GRx_UPDATE
       b4       GRx_P_VEN
       b0       GRx_IBUS_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)(VDC_REG_BIT8 | VDC_REG_BIT4 | VDC_REG_BIT0);

}   /* End of function VDC_ReadDataControl() */

/**************************************************************************//**
 * @fn          VDC_ChangeReadProcess
 * @brief       Sets registers for data read change processing
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Data read change parameter
 * @retval      None
******************************************************************************/
void VDC_ChangeReadProcess  
    (const vdc_graphics_type_t graphics_id, const vdc_read_chg_t * const param)
{
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;
    vdc_gr_flm_sel_t                       gr_flm_sel_tmp;
    vdc_period_rect_t                    * gr_grc_tmp;
    vdc_width_read_fb_t                  * w_read_fb;
    vdc_onoff_t                            graphics_enlargement;
    vdc_gr_format_t                        gr_format_tmp;

    gr_flm_sel_tmp  = VDC_ShrdPrmGetSelFbAddrSig(graphics_id);
    gr_grc_tmp      = VDC_ShrdPrmGetDisplayArea(graphics_id);
    w_read_fb       = VDC_ShrdPrmGetFrBuffWidth_Rd(graphics_id);

    graphics_enlargement = ConfirmGraphicsEnlargement(graphics_id, gr_flm_sel_tmp, w_read_fb, gr_grc_tmp);

    if (graphics_id == VDC_GR_TYPE_GR0)
    {
        SetScalerGraphics(graphics_id, w_read_fb, gr_grc_tmp, gr_flm_sel_tmp, graphics_enlargement);
    }

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

    /* Frame buffer base address */
    if (param->gr_base != NULL)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_flm2))

            /* Cast to an appropriate type */
            = (uint32_t)(R_VDC_CPUVAddrToSysPAddr((uint32_t)param->gr_base) & (uint32_t)~VDC_REG_MASK_0X00000007);

        if ((gr_flm_sel_tmp == VDC_GR_FLM_SEL_SCALE_DOWN) || (gr_flm_sel_tmp == VDC_GR_FLM_SEL_FLM_NUM))
        {
            gr_format_tmp = VDC_ShrdPrmGetGraphicsFormat(graphics_id);

            /* Sets the amount of data to be skipped through */
            (*(img_synthesizer->gr_flm6)) &= (uint32_t)~VDC_REG_MASK_0X0000003F;

            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_flm6)) |= DisplayStartPixelSetting((uint32_t)param->gr_base, gr_format_tmp);
        }
    }

    /* Size of the frame buffer to be read */
    if (param->width_read_fb != NULL)
    {
        /* Number of lines in a frame */
        (*(img_synthesizer->gr_flm5)) &= (uint32_t)~VDC_REG_MASK_0X07FF0000;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_flm5)) |= (((uint32_t)param->width_read_fb->in_vw - 1u) << VDC_REG_SHIFT_16);

        /* Width of the horizontal valid period */
        (*(img_synthesizer->gr_flm6)) &= (uint32_t)~VDC_REG_MASK_0X07FF0000;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_flm6)) |= (((uint32_t)param->width_read_fb->in_hw - 1u) << VDC_REG_SHIFT_16);
    }

    /* Graphics display mode */
    if (param->gr_disp_sel != NULL)
    {
        if ((*(param->gr_disp_sel)) != VDC_DISPSEL_IGNORED)
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) |= (uint32_t)*(param->gr_disp_sel);
        }
    }

    /* Graphics display area */
    if (param->gr_grc != NULL)
    {

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab2))  = ((uint32_t)param->gr_grc->vs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab2)) |= (uint32_t)param->gr_grc->vw;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab3))  = ((uint32_t)param->gr_grc->hs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab3)) |= (uint32_t)param->gr_grc->hw;
    }

    /* Graphics register update control register (GRx_UPDATE)
       b4       GRx_P_VEN
       b0       GRx_IBUS_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
}   /* End of function VDC_ChangeReadProcess() */

/**************************************************************************//**
 * @fn          VDC_StartProcess
 * @brief       Sets registers for data write/read start processing
 * @param[in]   layer_id                : Layer ID
 * @param[in]   param                   : Data write/read start parameter
 * @retval      None
******************************************************************************/
void VDC_StartProcess  
    (const vdc_layer_id_t layer_id, const vdc_start_t * const param)
{
    uint32_t                   layer_id_num;
    vdc_resource_state_t       resource_state;

    /* NULL check */
    if (param->gr_disp_sel != NULL)
    {
        /* Set graphics display mode */
        SetGraphicsDisplayMode(layer_id, param->gr_disp_sel);
    }

    if (layer_id != VDC_LAYER_ID_ALL)
    {
        rw_proc_function_tbl[layer_id](VDC_ON);
    }
    else
    {
        /* Cast to an appropriate type */
        for (layer_id_num = 0; layer_id_num < (uint32_t)VDC_LAYER_ID_NUM; layer_id_num++)
        {
            /* Cast to an appropriate type */
            resource_state = VDC_ShrdPrmGetRwProcReady((vdc_layer_id_t)layer_id_num);
            if (resource_state != VDC_RESOURCE_ST_INVALID)
            {
                rw_proc_function_tbl[layer_id_num](VDC_ON);
            }
        }
    }
    SetRegUpdateRwEnable(layer_id);
}   /* End of function VDC_StartProcess() */

/**************************************************************************//**
 * @fn          VDC_StopProcess
 * @brief       Sets registers for data write/read stop processing
 * @param[in]   layer_id                : Layer ID
 * @retval      None
******************************************************************************/
void VDC_StopProcess (const vdc_layer_id_t layer_id) 
{
    uint32_t                   layer_id_num;
    vdc_resource_state_t       resource_state;

    if (layer_id != VDC_LAYER_ID_ALL)
    {
        rw_proc_function_tbl[layer_id](VDC_OFF);
    }
    else
    {
        /* Cast to an appropriate type */
        for (layer_id_num = 0; layer_id_num < (uint32_t)VDC_LAYER_ID_NUM; layer_id_num++)
        {
            /* Cast to an appropriate type */
            resource_state = VDC_ShrdPrmGetRwProcEnabled((vdc_layer_id_t)layer_id_num);
            if (resource_state != VDC_RESOURCE_ST_INVALID)
            {
                rw_proc_function_tbl[layer_id_num](VDC_OFF);
            }
        }
    }
    SetRegUpdateRwEnable(layer_id);
}   /* End of function VDC_StopProcess() */

/**************************************************************************//**
 * @fn          VDC_VideoNoiseReduction
 * @brief       Sets registers for noise reduction
 * @param[in]   nr1d_on                 : Noise reduction ON/OFF setting
 * @param[in]   param                   : Noise reduction setup parameter
 * @retval      None
******************************************************************************/
void VDC_VideoNoiseReduction  
    (const vdc_onoff_t nr1d_on, const vdc_noise_reduction_t * const param)
{
    const vdc_regaddr_input_ctrl_t   * input_ctrl;
    const vdc_nr_param_t             * nr_param;

    input_ctrl = &vdc_regaddr_input_ctrl;

    /* NULL check */
    if (param != NULL)
    {
        /* Y/G signal noise reduction parameter */
        nr_param = &param->y;

        /* Cast to an appropriate type */
        (*(input_ctrl->imgcnt_nr_cnt0)) &= (uint32_t)~VDC_REG_MASK_0X00007F33;

        /* Maximum value of coring (absolute value) */
        (*(input_ctrl->imgcnt_nr_cnt0)) |= (nr_param->nr1d_th << VDC_REG_SHIFT_8);

        /* TAP select */
        (*(input_ctrl->imgcnt_nr_cnt0)) |= ((uint32_t)nr_param->nr1d_tap << VDC_REG_SHIFT_4);

        /* Noise reduction gain adjustment */
        (*(input_ctrl->imgcnt_nr_cnt0)) |= (uint32_t)nr_param->nr1d_gain;

        /* Cb/B signal noise reduction parameter */
        nr_param = &param->cb;

        /* Maximum value of coring (absolute value) */
        (*(input_ctrl->imgcnt_nr_cnt1))  = (nr_param->nr1d_th << VDC_REG_SHIFT_24);

        /* TAP select */
        (*(input_ctrl->imgcnt_nr_cnt1)) |= ((uint32_t)nr_param->nr1d_tap << VDC_REG_SHIFT_20);

        /* Noise reduction gain adjustment */
        (*(input_ctrl->imgcnt_nr_cnt1)) |= ((uint32_t)nr_param->nr1d_gain << VDC_REG_SHIFT_16);

        /* Cr/R signal noise reduction parameter */
        nr_param = &param->cr;

        /* Maximum value of coring (absolute value) */
        (*(input_ctrl->imgcnt_nr_cnt1)) |= (nr_param->nr1d_th << VDC_REG_SHIFT_8);

        /* TAP select */
        (*(input_ctrl->imgcnt_nr_cnt1)) |= ((uint32_t)nr_param->nr1d_tap << VDC_REG_SHIFT_4);

        /* Noise reduction gain adjustment */
        (*(input_ctrl->imgcnt_nr_cnt1)) |= (uint32_t)nr_param->nr1d_gain;
    }

    if (nr1d_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->imgcnt_nr_cnt0)) &= (uint32_t)~VDC_REG_BIT16;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->imgcnt_nr_cnt0)) |= (uint32_t)VDC_REG_BIT16;
    }

    /* Register update control register (IMGCNT_UPDATE)
       b0       IMGCNT_VEN */
    (*(input_ctrl->imgcnt_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function VDC_VideoNoiseReduction() */

/**************************************************************************//**
 * @fn          VDC_ImageColorMatrix
 * @brief       Sets registers for color matrix
 * @param[in]   param                   : Color matrix setup parameter
 * @retval      None
******************************************************************************/
void VDC_ImageColorMatrix (const vdc_color_matrix_t * const param) 
{
    SetColorMatrix(param->mtx_mode, param->offset, param->gain, &vdc_regaddr_color_matrix[param->module]);
}   /* End of function VDC_ImageColorMatrix() */

/**************************************************************************//**
 * @fn          VDC_ImageEnhancement
 * @brief       Sets registers for image enhancement processing
 * @param[in]   shp_h_on                : Sharpness ON/OFF setting
 * @param[in]   sharp_param             : Sharpness setup parameter
 * @param[in]   lti_h_on                : LTI ON/OFF setting
 * @param[in]   lti_param               : LTI setup parameter
 * @param[in]   enh_area                : Enhancer-enabled area setup parameter
 * @retval      None
******************************************************************************/
void VDC_ImageEnhancement ( 
    const vdc_onoff_t                  shp_h_on,
    const vdc_enhance_sharp_t  * const sharp_param,
    const vdc_onoff_t                  lti_h_on,
    const vdc_enhance_lti_t    * const lti_param,
    const vdc_period_rect_t    * const enh_area)
{
    const vdc_regaddr_img_qlty_imp_t * img_qlty_imp;

    img_qlty_imp = &vdc_regaddr_img_qlty_imp;

    /* Sharpness */
    SetImageEnhancementSharpness(shp_h_on, sharp_param, img_qlty_imp);

    /* LTI */
    SetImageEnhancementLti(lti_h_on, lti_param, img_qlty_imp);

    /* Enhance area */
    if (enh_area != NULL)
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_tim2))  = ((uint32_t)enh_area->vs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_tim2)) |= (uint32_t)enh_area->vw;

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_tim3))  = ((uint32_t)enh_area->hs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_tim3)) |= (uint32_t)enh_area->hw;
    }

    /* Register update control register (ADJx_UPDATE)
       b0       ADJx_VEN */
    (*(img_qlty_imp->adj_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function VDC_ImageEnhancement() */

/**************************************************************************//**
 * @fn          VDC_ImageBlackStretch
 * @brief       Sets registers for black stretch
 * @param[in]   bkstr_on                : Black stretch ON/OFF setting
 * @param[in]   param                   : Black stretch setup parameter
 * @retval      None
******************************************************************************/
void VDC_ImageBlackStretch ( 
    const vdc_onoff_t          bkstr_on,
    const vdc_black_t  * const param)
{
    const vdc_regaddr_img_qlty_imp_t * img_qlty_imp;

    img_qlty_imp = &vdc_regaddr_img_qlty_imp;

    /* NULL check */
    if (param != NULL)
    {
        /* Black stretch start point */
        (*(img_qlty_imp->adj_bkstr_set))  = (uint32_t)((uint32_t)param->bkstr_st << VDC_REG_SHIFT_20);

        /* Black stretch depth */
        (*(img_qlty_imp->adj_bkstr_set)) |= (uint32_t)((uint32_t)param->bkstr_d << VDC_REG_SHIFT_16);

        /* Black stretch time constant (T1) */
        (*(img_qlty_imp->adj_bkstr_set)) |= (uint32_t)((uint32_t)param->bkstr_t1 << VDC_REG_SHIFT_8);

        /* Black stretch time constant (T2) */
        (*(img_qlty_imp->adj_bkstr_set)) |= (uint32_t)param->bkstr_t2;
    }

    if (bkstr_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_bkstr_set)) &= (uint32_t)~VDC_REG_BIT24;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_bkstr_set)) |= (uint32_t)VDC_REG_BIT24;
    }

    /* Register update control register (ADJx_UPDATE)
       b0       ADJx_VEN */
    (*(img_qlty_imp->adj_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function VDC_ImageBlackStretch() */

/**************************************************************************//**
 * @fn          VDC_AlphaBlending
 * @brief       Sets registers for alpha blending
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : Alpha blending setup parameter
 * @retval      None
******************************************************************************/
void VDC_AlphaBlending 
    (const vdc_graphics_type_t graphics_id, const vdc_alpha_blending_t * const param)
{
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

    /* Alpha signal of the ARGB1555/ARGB5551 format */
    if (param->alpha_1bit != NULL)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab10)) &= (uint32_t)~VDC_REG_MASK_0XFF000000;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab10)) |= (uint32_t)(param->alpha_1bit->gr_a0 << VDC_REG_SHIFT_24);

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab11)) &= (uint32_t)~VDC_REG_MASK_0XFF000000;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab11)) |= (uint32_t)(param->alpha_1bit->gr_a1 << VDC_REG_SHIFT_24);
    }

    /* Premultiplication processing at alpha blending in one-pixel */
    if (param->alpha_pixel != NULL)
    {
        if (param->alpha_pixel->gr_acalc_md == VDC_OFF)
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_BIT14;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_REG_BIT14;
        }
    }

    /* Graphics register update control register (GRx_UPDATE)
       b4       GRx_P_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;

}   /* End of function VDC_AlphaBlending() */

/**************************************************************************//**
 * @fn          VDC_AlphaBlendingRect
 * @brief       Sets registers for rectangle alpha blending
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   gr_arc_on               : ON/OFF setting for alpha blending in a rectangular area
 * @param[in]   param                   : Setup parameter for alpha blending in a rectangular area
 * @retval      None
******************************************************************************/
void VDC_AlphaBlendingRect ( 
    const vdc_graphics_type_t                  graphics_id,
    const vdc_onoff_t                          gr_arc_on,
    const vdc_alpha_blending_rect_t    * const param)
{
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;
    const vdc_alpha_rect_t               * alpha_rect_tmp;
    uint32_t                                arc_coef;

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

    /* Alpha blending on/off control in a rectangular area */
    if (gr_arc_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_BIT12;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_REG_BIT12;
    }

    /* NULL check */
    if (param != NULL)
    {
        /* Rectangular area subjected to alpha blending */
        SetAlphaRectArea(param->gr_arc, img_synthesizer);

        /* Alpha blending in a rectangular area */
        alpha_rect_tmp = param->alpha_rect;

        /* NULL check */
        if (alpha_rect_tmp != NULL)
        {
            /* Multiplication processing with current alpha at alpha blending in a rectangular area (on/off) */
            if (alpha_rect_tmp->gr_arc_mul == VDC_OFF)
            {
                /* Cast to an appropriate type */
                (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_BIT15;
            }
            else
            {
                /* Cast to an appropriate type */
                (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_REG_BIT15;
            }

            /* Alpha coefficient for alpha blending in a rectangular area (-255 to 255) */
            if (alpha_rect_tmp->gr_arc_coef < 0)
            {
                /* Cast to an appropriate type */
                arc_coef = (uint32_t)(alpha_rect_tmp->gr_arc_coef * (-1));  /* Conversion into absolute value */

                /* Subtraction of the alpha coefficient */
                (*(img_synthesizer->gr_ab6)) = (uint32_t)VDC_REG_BIT24;
            }
            else
            {
                /* Cast to an appropriate type */
                arc_coef = (uint32_t)alpha_rect_tmp->gr_arc_coef;

                /* Addition of the alpha coefficient */
                (*(img_synthesizer->gr_ab6)) = (uint32_t)0x00000000u;
            }
            (*(img_synthesizer->gr_ab6)) |= (arc_coef << VDC_REG_SHIFT_16);

            /* Frame rate for alpha blending in a rectangular area (gr_arc_rate + 1) */
            (*(img_synthesizer->gr_ab6)) |= (uint32_t)alpha_rect_tmp->gr_arc_rate;

            /* Initial alpha value for alpha blending in a rectangular area */
            (*(img_synthesizer->gr_ab7)) &= (uint32_t)~VDC_REG_MASK_0X00FF0000;

            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab7)) |= (uint32_t)(alpha_rect_tmp->gr_arc_def << VDC_REG_SHIFT_16);
        }
    }

    /* Graphics register update control register (GRx_UPDATE)
       b4       GRx_P_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;

}   /* End of function VDC_AlphaBlendingRect() */

/**************************************************************************//**
 * @fn          VDC_Chromakey
 * @brief       Sets registers for chroma-key
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   gr_ck_on                : Chroma-key ON/OFF setting
 * @param[in]   param                   : Chroma-key setup parameter
 * @retval      None
******************************************************************************/
void VDC_Chromakey ( 
    const vdc_graphics_type_t      graphics_id,
    const vdc_onoff_t              gr_ck_on,
    const vdc_chromakey_t  * const param)
{
    vdc_gr_format_t                        gr_format_tmp;
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];


    /* NULL check */
    if (param != NULL)
    {
        gr_format_tmp = VDC_ShrdPrmGetGraphicsFormat(graphics_id);
        if ((gr_format_tmp == VDC_GR_FORMAT_CLUT1) ||
            (gr_format_tmp == VDC_GR_FORMAT_CLUT4) ||
            (gr_format_tmp == VDC_GR_FORMAT_CLUT8))
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab8))  = (uint32_t)(param->ck_color << VDC_REG_SHIFT_24);
            (*(img_synthesizer->gr_ab9))  = ColorConversionIntoAgbr(param->rep_color);
        }
        else
        {
            /* Color conversion from gr_format_tmp into RGB888 format */
            (*(img_synthesizer->gr_ab8))  = ColorConversion(gr_format_tmp, param->ck_color);
            (*(img_synthesizer->gr_ab9))  = ColorConversion(gr_format_tmp, param->rep_color);
        }
        if (graphics_id == VDC_GR_TYPE_GR0)
        {
            /* Alpha value is set to 255. */
            (*(img_synthesizer->gr_ab9)) |= (uint32_t)VDC_REG_ALPHA_8BIT;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab9)) |= (uint32_t)(param->rep_alpha << VDC_REG_SHIFT_24);
        }
    }

    if (gr_ck_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab7)) &= (uint32_t)~VDC_REG_BIT0;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab7)) |= (uint32_t)VDC_REG_BIT0;
    }

    /* Graphics register update control register (GRx_UPDATE)
       b4       GRx_P_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;

}   /* End of function VDC_Chromakey() */

/**************************************************************************//**
 * @fn          VDC_CLUT
 * @brief       Sets registers for CLUT
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   param                   : CLUT setup parameter
 * @retval      None
******************************************************************************/
void VDC_CLUT  
    (const vdc_graphics_type_t graphics_id, const vdc_clut_t * const param)
{
    vdc_gr_format_t                        gr_format_tmp;
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;

    img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

    gr_format_tmp = VDC_ShrdPrmGetGraphicsFormat(graphics_id);
    if (gr_format_tmp == VDC_GR_FORMAT_CLUT1)
    {
        /* Cast to an appropriate type */
        if (param->color_num == ((uint32_t)1u))
        {
            (*(img_synthesizer->gr_ab10)) = ColorConversionIntoAgbr(param->clut[0]);
            if (graphics_id == VDC_GR_TYPE_GR0)
            {
                /* Alpha value is set to 255. */
                (*(img_synthesizer->gr_ab10)) |= (uint32_t)VDC_REG_ALPHA_8BIT;
            }
        }
        else
        {
            (*(img_synthesizer->gr_ab10)) = ColorConversionIntoAgbr(param->clut[0]);
            (*(img_synthesizer->gr_ab11)) = ColorConversionIntoAgbr(param->clut[1]);
            if (graphics_id == VDC_GR_TYPE_GR0)
            {
                /* Alpha value is set to 255. */
                (*(img_synthesizer->gr_ab10)) |= (uint32_t)VDC_REG_ALPHA_8BIT;

                /* Cast to an appropriate type */
                (*(img_synthesizer->gr_ab11)) |= (uint32_t)VDC_REG_ALPHA_8BIT;
            }
        }
    }
    else        /* CLUT4 or CLUT8 */
    {
        Set_Clut(param, vdc_regaddr_clut[graphics_id]);

        /* dummy read*/
        {
            volatile uint32_t wk = (*(vdc_regaddr_clut[graphics_id]));
        }

        /* CLUT table control register (GRx_CLUT)
           b16      GRx_CLT_SEL - CLUT table select signal - XOR, flip signal */
        (*(img_synthesizer->gr_clut)) ^= (uint32_t)VDC_REG_BIT16;
    }

    /* Graphics register update control register (GRx_UPDATE)
       b4       GRx_P_VEN */
    (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;

}   /* End of function VDC_CLUT() */

/**************************************************************************//**
 * @fn          VDC_DisplayCalibration
 * @brief       Sets registers for display calibration
 * @param[in]   param                   : Display calibration parameter
 * @retval      None
******************************************************************************/
void VDC_DisplayCalibration (const vdc_disp_calibration_t * const param) 
{
    const vdc_calibr_bright_t        * bright_tmp;
    const vdc_calibr_contrast_t      * contrast_tmp;
    const vdc_calibr_dither_t        * panel_dither_tmp;
    const vdc_regaddr_output_ctrl_t  * output_ctrl;

    output_ctrl = &vdc_regaddr_output_ctrl;

    /* Correction circuit sequence control */
    if (param->route == VDC_CALIBR_ROUTE_BCG)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_clk_phase)) &= (uint32_t)~VDC_REG_BIT12;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_clk_phase)) |= (uint32_t)VDC_REG_BIT12;
    }

    /* Brightness (DC) adjustment */
    bright_tmp = param->bright;

    /* NULL check */
    if (bright_tmp != NULL)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_bright1))  = (uint32_t)bright_tmp->pbrt_g;

        /* Cast to an appropriate type */
        (*(output_ctrl->out_bright2))  = (uint32_t)(bright_tmp->pbrt_b << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(output_ctrl->out_bright2)) |= (uint32_t)bright_tmp->pbrt_r;
    }

    /* Contrast (gain) adjustment */
    contrast_tmp = param->contrast;

    /* NULL check */
    if (contrast_tmp != NULL)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_contrast))  = (uint32_t)(contrast_tmp->cont_g << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(output_ctrl->out_contrast)) |= (uint32_t)(contrast_tmp->cont_b << VDC_REG_SHIFT_8);

        /* Cast to an appropriate type */
        (*(output_ctrl->out_contrast)) |= (uint32_t)contrast_tmp->cont_r;
    }

    /* Panel dithering */
    panel_dither_tmp = param->panel_dither;

    /* NULL check */
    if (panel_dither_tmp != NULL)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->out_pdtha)) &= (uint32_t)~VDC_REG_MASK_0X00300000;

        /* Cast to an appropriate type */
        (*(output_ctrl->out_pdtha)) |= (uint32_t)(panel_dither_tmp->pdth_sel << VDC_REG_SHIFT_20);
        if (panel_dither_tmp->pdth_sel == VDC_PDTH_MD_2X2)
        {
            /* Cast to an appropriate type */
            (*(output_ctrl->out_pdtha)) &= (uint32_t)~VDC_REG_MASK_0X00003333;

            /* Cast to an appropriate type */
            (*(output_ctrl->out_pdtha)) |= (uint32_t)(panel_dither_tmp->pdth_pa << VDC_REG_SHIFT_12);

            /* Cast to an appropriate type */
            (*(output_ctrl->out_pdtha)) |= (uint32_t)(panel_dither_tmp->pdth_pb << VDC_REG_SHIFT_8);

            /* Cast to an appropriate type */
            (*(output_ctrl->out_pdtha)) |= (uint32_t)(panel_dither_tmp->pdth_pc << VDC_REG_SHIFT_4);

            /* Cast to an appropriate type */
            (*(output_ctrl->out_pdtha)) |= (uint32_t)panel_dither_tmp->pdth_pd;
        }
    }

    /* Register update control register (OUT_UPDATE)
       b0       OUTCNT_VEN */
    (*(output_ctrl->out_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function VDC_DisplayCalibration() */

/**************************************************************************//**
 * @fn          VDC_GammaCorrection
 * @brief       Sets registers for gamma correction
 * @param[in]   gam_on      : Gamma correction ON/OFF setting
 * @param[in]   param       : Gamma correction setup parameter
 * @retval      None
******************************************************************************/
void VDC_GammaCorrection  
    (const vdc_onoff_t gam_on, const vdc_gamma_correction_t * const param)
{
    const vdc_regaddr_gamma_t  * gamma;

    gamma = &vdc_regaddr_gamma;

    /* Gamma correction on/off control */
    (*(gamma->gam_sw)) = (gam_on == VDC_OFF) ? 0x0000u : 0x0001u;

    /* NULL check */
    if (param != NULL)
    {
        /* Start threshold of area 1 to 31 of G signal */
        Set_StartThreshold_Gamma(param->gam_g_th, gamma->gam_g_area);

        /* Gain adjustment of area 0 to 31 of G signal */
        Set_GainAdjustment_Gamma(param->gam_g_gain, gamma->gam_g_lut);

        /* Register update control register G (GAM_G_UPDATE)
           b0       GAM_G_VEN */
        (*(gamma->gam_g_update)) |= (uint32_t)VDC_REG_BIT0;

        /* Start threshold of area 1 to 31 of B signal */
        Set_StartThreshold_Gamma(param->gam_b_th, gamma->gam_b_area);

        /* Gain adjustment of area 0 to 31 of B signal */
        Set_GainAdjustment_Gamma(param->gam_b_gain, gamma->gam_b_lut);

        /* Register update control register B (GAM_B_UPDATE)
           b0       GAM_B_VEN */
        (*(gamma->gam_b_update)) |= (uint32_t)VDC_REG_BIT0;

        /* Start threshold of area 1 to 31 of R signal */
        Set_StartThreshold_Gamma(param->gam_r_th, gamma->gam_r_area);

        /* Gain adjustment of area 0 to 31 of R signal */
        Set_GainAdjustment_Gamma(param->gam_r_gain, gamma->gam_r_lut);

        /* Register update control register R (GAM_R_UPDATE)
           b0       GAM_R_VEN */
        (*(gamma->gam_r_update)) |= (uint32_t)VDC_REG_BIT0;
    }
}   /* End of function VDC_GammaCorrection() */

/******************************************************************************
Local Functions
******************************************************************************/
/**************************************************************************//**
 * @fn          SetVideoExternalInput
 * @brief       Sets external input video signal
 * @param[in]   ext_sig_tmp             : External input signal parameter
 * @param[in]   input_ctrl              : Input controller registers
 * @retval      None
******************************************************************************/
static void SetVideoExternalInput ( 
    const vdc_ext_in_sig_t         * const ext_sig_tmp,
    const vdc_regaddr_input_ctrl_t * const input_ctrl)
{
    /* Cast to an appropriate type */
    (*(input_ctrl->inp_sel_cnt)) &= (uint32_t)~VDC_REG_MASK_0X00007111;

    /* External input format select */
    if (ext_sig_tmp->inp_format == VDC_EXTIN_FORMAT_YCBCR444)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)(VDC_EXTIN_FORMAT_RGB888 << VDC_REG_SHIFT_12);
    }
    else
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)(ext_sig_tmp->inp_format << VDC_REG_SHIFT_12);
    }

    /* Clock edge select for capturing external input video image signals */
    if (ext_sig_tmp->inp_pxd_edge != VDC_EDGE_RISING)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Clock edge select for capturing external input Vsync signals */
    if (ext_sig_tmp->inp_vs_edge != VDC_EDGE_RISING)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)VDC_REG_BIT4;
    }

    /* Clock edge select for capturing external input Hsync signals */
    if (ext_sig_tmp->inp_hs_edge != VDC_EDGE_RISING)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_sel_cnt)) |= (uint32_t)VDC_REG_BIT0;
    }

    /* Cast to an appropriate type */
    (*(input_ctrl->inp_ext_sync_cnt)) &= (uint32_t)~VDC_REG_MASK_0X11110113;
    
    /* External input bit endian change on/off control */
    if (ext_sig_tmp->inp_endian_on != VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT28;
    }

    /* External input B/R signal swap on/off control */
    if (ext_sig_tmp->inp_swap_on != VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT24;
    }

    /* External input Vsync signal DV_VSYNC inversion control */
    if (ext_sig_tmp->inp_vs_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT20;
    }

    /* External input Hsync signal DV_HSYNC inversion control */
    if (ext_sig_tmp->inp_hs_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT16;
    }

    /* Reference select for external input BT.656 Hsync signal */
    if (ext_sig_tmp->inp_h_edge_sel != VDC_EXTIN_REF_H_EAV)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Number of lines for BT.656 external input */
    if (ext_sig_tmp->inp_f525_625 != VDC_EXTIN_LINE_525)
    {
        /* Cast to an appropriate type */
        (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)VDC_REG_BIT4;
    }

    /* Y/Cb/Y/Cr data string start timing to Hsync reference */
    (*(input_ctrl->inp_ext_sync_cnt)) |= (uint32_t)ext_sig_tmp->inp_h_pos;
}   /* End of function SetVideoExternalInput() */

/**************************************************************************//**
 * @fn          SetInitialConnection
 * @brief       Initializes cascaded connection
 * @param[in]   void
 * @retval      None
******************************************************************************/
static void SetInitialConnection (void) 
{
    volatile uint32_t * vin_gr_update;
    volatile uint32_t * vin_gr_ab1;

    /* Cast to an appropriate type */
    vin_gr_update   = (volatile uint32_t *) &VDC6.GR_VIN_UPDATE;

    /* Cast to an appropriate type */
    vin_gr_ab1      = (volatile uint32_t *) &VDC6.GR_VIN_AB1;

    /* Selection of lower-layer plane in scaler: Selects graphics 0 as lower-layer graphics. */
    (*(vin_gr_ab1))  &= (uint32_t)~VDC_REG_BIT2;

    /* Graphics display mode in VIN */
    (*(vin_gr_ab1))  &= (uint32_t)~VDC_REG_MASK_0X00000003;

    /* Cast to an appropriate type */
    (*(vin_gr_ab1))  |= (uint32_t)VDC_DISPSEL_LOWER;

    /* Graphics register update control register (GR_VIN_UPDATE)
       b8       GR_VIN_UPDATE
       b4       GR_VIN_P_VEN */
    (*(vin_gr_update)) |= (uint32_t)(VDC_REG_BIT8|VDC_REG_BIT4);

}   /* End of function SetInitialConnection() */

/**************************************************************************//**
 * @fn          SetGrapicsBackgroundColor
 * @brief       Sets background color for graphics
 * @param[in]   void
 * @retval      None
******************************************************************************/
static void SetGrapicsBackgroundColor (void) 
{
    uint32_t                bg_color_tmp;
    uint32_t                bg_color_conv;
    volatile uint32_t     * bg_color_reg;
    volatile uint32_t     * update_reg;

    /* Background color in 24-bit RGB color format or CrYCb format */
    bg_color_tmp = VDC_ShrdPrmGetBgColor(VDC_COLOR_SPACE_GBR);

    /* Conversion from RGB into GBR / from CrYCb into YCbCr */
    bg_color_conv   = ColorConversionIntoAgbr(bg_color_tmp);

    /* Cast to an appropriate type */
    bg_color_conv  &= (uint32_t)VDC_REG_RGB_24BIT_MASK;

    /* GR2, GR3 */
    /* Background color in GBR format (GR2_BASE) */
    bg_color_reg    = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR2].gr_base;
    (*bg_color_reg)   = bg_color_conv;

    /* Graphics register update control register (GR2_UPDATE)
       b4       GR2_P_VEN */
    update_reg      = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR2].gr_update;

    /* Cast to an appropriate type */
    (*update_reg)    |= (uint32_t)VDC_REG_BIT4;

    /* Background color in GBR format (GR3_BASE) */
    bg_color_reg    = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_base;
    (*bg_color_reg)   = bg_color_conv;

    /* Graphics register update control register (GR3_UPDATE)
       b4       GR3_P_VEN */
    update_reg      = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_update;

    /* Cast to an appropriate type */
    (*update_reg)    |= (uint32_t)VDC_REG_BIT4;
}   /* End of function SetGrapicsBackgroundColor() */

/**************************************************************************//**
 * @fn          SetScalerBackgroundColor
 * @brief       Sets background color for scaler 0 (graphics 0)
 *
 *              Description:<br>
 *              This function should be called only when graphics 0 is selected.
 * @param[in]   color_space         : Color space (GBR or YCbCr)
 * @retval      None
******************************************************************************/
static void SetScalerBackgroundColor (const vdc_color_space_t color_space) 
{
    volatile uint32_t * scl0_update_reg;
    volatile uint32_t * scl0_ovr1_reg;
    volatile uint32_t * gr_base_reg;
    volatile uint32_t * gr_update_reg;
    uint32_t            bg_color_tmp;
    uint32_t            bg_color_conv;

    scl0_ovr1_reg   = vdc_regaddr_scaler.scl0_ovr1;
    scl0_update_reg = vdc_regaddr_scaler.scl0_update;
    gr_base_reg     = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR0].gr_base;
    gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR0].gr_update;

    /* Background color in 24-bit RGB color format or CrYCb format */
    bg_color_tmp    = VDC_ShrdPrmGetBgColor(color_space);

    /* Conversion from RGB into GBR / from CrYCb into YCbCr */
    bg_color_conv   = ColorConversionIntoAgbr(bg_color_tmp);

    /* Cast to an appropriate type */
    bg_color_conv  &= (uint32_t)VDC_REG_RGB_24BIT_MASK;

    /* Background color in RGB format or CrYCb format */
    (*scl0_ovr1_reg)      = bg_color_tmp;

    /* Register update control register
       b4       SCL0_VEN_B */
    (*scl0_update_reg)   |= (uint32_t)VDC_REG_BIT4;

    /* Background color in GBR format or YCbCr format */
    (*gr_base_reg)        = bg_color_conv;

    /* Graphics register update control register
       b4       GRx_P_VEN */
    (*gr_update_reg)     |= (uint32_t)VDC_REG_BIT4;
}   /* End of function SetScalerBackgroundColor() */

/**************************************************************************//**
 * @fn          SetLcdTcon_STVA_VS
 * @brief       Sets LCD TCON - STVA/VS
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_STVA_VS ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_stva1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_stva1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv == VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stva2)) &= (uint32_t)~VDC_REG_BIT4;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stva2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_STVA_VS() */

/**************************************************************************//**
 * @fn          SetLcdTcon_STVB_VE
 * @brief       Sets LCD TCON - STVB/VE
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_STVB_VE ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_stvb1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_stvb1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv == VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stvb2)) &= (uint32_t)~VDC_REG_BIT4;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stvb2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_STVB_VE() */

/**************************************************************************//**
 * @fn          SetLcdTcon_STH_HS
 * @brief       Sets LCD TCON - STH/HS
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_STH_HS ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_sth1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_sth1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Cast to an appropriate type */
    (*(output_ctrl->tcon_tim_sth2)) &= (uint32_t)~VDC_REG_MASK_0X00000110;

    /* Signal operating reference select */
    if (lcd_tcon->tcon_hs_sel != VDC_LCD_TCON_REFSEL_HSYNC)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_sth2)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_sth2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_STH_HS() */

/**************************************************************************//**
 * @fn          SetLcdTcon_STB_HE
 * @brief       Sets LCD TCON - STB/HE
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_STB_HE ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_stb1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_stb1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Cast to an appropriate type */
    (*(output_ctrl->tcon_tim_stb2)) &= (uint32_t)~VDC_REG_MASK_0X00000110;

    /* Signal operating reference select */
    if (lcd_tcon->tcon_hs_sel != VDC_LCD_TCON_REFSEL_HSYNC)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stb2)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_stb2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_STB_HE() */

/**************************************************************************//**
 * @fn          SetLcdTcon_CPV_GCK
 * @brief       Sets LCD TCON - CPV/GCK
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_CPV_GCK ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_cpv1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_cpv1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Cast to an appropriate type */
    (*(output_ctrl->tcon_tim_cpv2)) &= (uint32_t)~VDC_REG_MASK_0X00000110;

    /* Signal operating reference select */
    if (lcd_tcon->tcon_hs_sel != VDC_LCD_TCON_REFSEL_HSYNC)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_cpv2)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_cpv2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_CPV_GCK() */

/**************************************************************************//**
 * @fn          SetLcdTcon_POLA
 * @brief       Sets LCD TCON - POLA
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_POLA ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_pola1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_pola1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Cast to an appropriate type */
    (*(output_ctrl->tcon_tim_pola2)) &= (uint32_t)~VDC_REG_MASK_0X00003110;

    /* POLA/POLB signal generation mode select */
    (*(output_ctrl->tcon_tim_pola2)) |= (uint32_t)(lcd_tcon->tcon_md << VDC_REG_SHIFT_12);

    /* Signal operating reference select */
    if (lcd_tcon->tcon_hs_sel != VDC_LCD_TCON_REFSEL_HSYNC)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_pola2)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_pola2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_POLA() */

/**************************************************************************//**
 * @fn          SetLcdTcon_POLB
 * @brief       Sets LCD TCON - POLB
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_POLB ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Signal pulse start position */
    (*(output_ctrl->tcon_tim_polb1))  = (uint32_t)(lcd_tcon->tcon_hsvs << VDC_REG_SHIFT_16);

    /* Pulse width */
    (*(output_ctrl->tcon_tim_polb1)) |= (uint32_t)lcd_tcon->tcon_hwvw;

    /* Cast to an appropriate type */
    (*(output_ctrl->tcon_tim_polb2)) &= (uint32_t)~VDC_REG_MASK_0X00003110;

    /* POLA/POLB signal generation mode select */
    (*(output_ctrl->tcon_tim_polb2)) |= (uint32_t)(lcd_tcon->tcon_md << VDC_REG_SHIFT_12);

    /* Signal operating reference select */
    if (lcd_tcon->tcon_hs_sel != VDC_LCD_TCON_REFSEL_HSYNC)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_polb2)) |= (uint32_t)VDC_REG_BIT8;
    }

    /* Polarity inversion control of signal */
    if (lcd_tcon->tcon_inv != VDC_SIG_POL_NOT_INVERTED)
    {
        /* Cast to an appropriate type */
        (*(output_ctrl->tcon_tim_polb2)) |= (uint32_t)VDC_REG_BIT4;
    }
}   /* End of function SetLcdTcon_POLB() */

/**************************************************************************//**
 * @fn          SetLcdTcon_DE
 * @brief       Sets LCD TCON - DE
 * @param[in]   lcd_tcon                : LCD TCON timing signal parameter
 * @param[in]   output_ctrl             : Output controller registers
 * @retval      None
******************************************************************************/
static void SetLcdTcon_DE ( 
    const vdc_lcd_tcon_timing_t        * const lcd_tcon,
    const vdc_regaddr_output_ctrl_t    * const output_ctrl)
{
    /* Polarity inversion control of signal */
    (*(output_ctrl->tcon_tim_de)) = (lcd_tcon->tcon_inv == VDC_SIG_POL_NOT_INVERTED) ? (uint32_t)0u : (uint32_t)1u;

}   /* End of function SetLcdTcon_DE() */

/**************************************************************************//**
 * @fn          SetScalingDown
 * @brief       Sets scaling-down
 * @param[in]   scldw_rot               : Scaling-down and rotation parameter
 * @param[in]   res_inter               : Field operating mode select
 * @param[in]   scaler                  : Scaler registers
 * @retval      None
******************************************************************************/
static void SetScalingDown ( 
    const vdc_scalingdown_rot_t    * const scldw_rot,
    const vdc_res_inter_t                  res_inter,
    const vdc_regaddr_scaler_t     * const scaler)
{
    uint32_t    vw_before_scld;    /* Vertical width before scaling-down */
    uint32_t    hw_before_scld;    /* Horizontal width before scaling-down */
    uint32_t    vw_after_scld;     /* Vertical width after scaling-down */
    uint32_t    hw_after_scld;     /* Horizontal width after scaling-down */
    uint32_t    scaling_ratio;

    /* Cast to an appropriate type */
    vw_before_scld = (uint32_t)scldw_rot->res.vw;

    /* Cast to an appropriate type */
    hw_before_scld = (uint32_t)scldw_rot->res.hw;

    /* Cast to an appropriate type */
    vw_after_scld = (uint32_t)scldw_rot->res_out_vw;

    /* Cast to an appropriate type */
    hw_after_scld = (uint32_t)scldw_rot->res_out_hw;

    /* Vertical scaling-down: ON, vertical scaling-up: OFF */
    (*(scaler->scl0_ds1)) |= (uint32_t)VDC_REG_BIT4;

    /* Cast to an appropriate type */
    (*(scaler->scl0_us1)) &= (uint32_t)~VDC_REG_BIT4;
    if (vw_before_scld > vw_after_scld)
    {
        scaling_ratio = CalcScalingRatio(vw_before_scld, vw_after_scld, scldw_rot->adj_sel, VDC_OFF);

        (*(scaler->scl0_ds6)) = scaling_ratio;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds6)) = (uint32_t)SCALING_RATIO_SAME_SIZE;
    }

    /* Horizontal scaling-down */
    if (hw_before_scld > hw_after_scld)
    {
        /* ON */
        (*(scaler->scl0_ds1)) |= (uint32_t)VDC_REG_BIT0;

        scaling_ratio = CalcScalingRatio(hw_before_scld, hw_after_scld, VDC_ON, VDC_ON);

        /* Cast to an appropriate type */
        (*(scaler->scl0_ds4)) &= (uint32_t)~VDC_REG_MASK_0X0000FFFF;
        (*(scaler->scl0_ds4)) |= scaling_ratio;
    }
    else
    {
        /* OFF */
        (*(scaler->scl0_ds1)) &= (uint32_t)~VDC_REG_BIT0;
    }

    /* Initial scaling phase */
    (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

    /* Cast to an appropriate type */
    (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;
    if (res_inter != VDC_RES_INTER_PROGRESSIVE)
    {
        /* Top */
        (*(scaler->scl0_ds5)) |= (INITIAL_SCALING_PHASE << VDC_REG_SHIFT_16);
    }

}   /* End of function SetScalingDown() */

/**************************************************************************//**
 * @fn          SetScalingUp
 * @brief       Sets scaling-up
 * @param[in]   w_read_fb               : Size of the frame buffer to be read
 * @param[in]   gr_grc                  : Graphics display area
 * @param[in]   adj_sel                 : Folding handling (on/off)
 * @param[in]   ip_conversion           : IP conversion on/off
 * @param[in]   res_ds_wr_md            : Frame buffer writing mode for image processing
 * @param[in]   ln_off_dir              : Line offset address direction of the frame buffer
 * @param[in]   scaler                  : Scaler registers
 * @retval      None
******************************************************************************/
static void SetScalingUp ( 
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc,
    const vdc_onoff_t                  adj_sel,
    const vdc_onoff_t                  ip_conversion,
    const vdc_wr_md_t                  res_ds_wr_md,
    const vdc_gr_ln_off_dir_t          ln_off_dir,
    const vdc_regaddr_scaler_t * const scaler)
{
    uint32_t            w_before_scl;       /* Width before scaling-up */
    uint32_t            w_after_scl;        /* Width after scaling-up */
    uint32_t            scaling_ratio;
    vdc_onoff_t         vscl_up;            /* Vertical scaling-up */

    vscl_up = VDC_OFF;

    /* Vertical scaling-up */
    w_before_scl    = (uint32_t)w_read_fb->in_vw;

    /* Cast to an appropriate type */
    w_after_scl     = (uint32_t)gr_grc->vw;
    if (w_before_scl < w_after_scl)
    {
        /* Vertical scaling-down: OFF, vertical scaling-up: ON */
        (*(scaler->scl0_ds1)) &= (uint32_t)~VDC_REG_BIT4;

        /* Cast to an appropriate type */
        (*(scaler->scl0_us1)) |= (uint32_t)VDC_REG_BIT4;

        scaling_ratio = CalcScalingRatio(w_before_scl, w_after_scl, adj_sel, VDC_OFF);
        (*(scaler->scl0_ds6)) = scaling_ratio;

        vscl_up = VDC_ON;
    }
    else
    {
        /* Vertical scaling-up: OFF */
        (*(scaler->scl0_us1)) &= (uint32_t)~VDC_REG_BIT4;
    }

    /* Horizontal scaling-up */
    w_before_scl    = (uint32_t)w_read_fb->in_hw;

    /* Cast to an appropriate type */
    w_after_scl     = (uint32_t)gr_grc->hw;
    if (w_before_scl < w_after_scl)
    {
        /* Horizontal scaling-up: ON */
        (*(scaler->scl0_us1)) |= (uint32_t)VDC_REG_BIT0;

        scaling_ratio = CalcScalingRatio(w_before_scl, w_after_scl, adj_sel, VDC_OFF);

        /* Cast to an appropriate type */
        (*(scaler->scl0_us5)) &= (uint32_t)~VDC_REG_MASK_0X0000FFFF;

        /* Cast to an appropriate type */
        (*(scaler->scl0_us5)) |= scaling_ratio;
    }
    else
    {
        /* Horizontal scaling-up: OFF */
        (*(scaler->scl0_us1)) &= (uint32_t)~VDC_REG_BIT0;
    }

    /* Initial scaling phase */
    if (ip_conversion == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

        /* Cast to an appropriate type */
        (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;
    }
    else
    {
        if (vscl_up != VDC_OFF)
        {
            switch (res_ds_wr_md)
            {
                case VDC_WR_MD_ROT_90DEG:

                    /* Cast to an appropriate type */
                    (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                    /* Cast to an appropriate type */
                    (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                    /* US_HB */
                    (*(scaler->scl0_us6)) |= INITIAL_SCALING_PHASE;
                break;
                case VDC_WR_MD_ROT_180DEG:
                    if (ln_off_dir == VDC_GR_LN_OFF_DIR_INC)
                    {
                        /* Cast to an appropriate type */
                        (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                        /* Cast to an appropriate type */
                        (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                        /* BTM */
                        (*(scaler->scl0_ds5)) |= INITIAL_SCALING_PHASE;
                    }
                break;
                case VDC_WR_MD_ROT_270DEG:
                    /* Cast to an appropriate type */
                    (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                    /* Cast to an appropriate type */
                    (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                    /* US_HT */
                    (*(scaler->scl0_us6)) |= (INITIAL_SCALING_PHASE << VDC_REG_SHIFT_16);
                break;
                default:    /* Normal or horizontal mirroring */
                    if (ln_off_dir == VDC_GR_LN_OFF_DIR_DEC)
                    {
                        /* Cast to an appropriate type */
                        (*(scaler->scl0_ds5)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                        /* Cast to an appropriate type */
                        (*(scaler->scl0_us6)) &= (uint32_t)~VDC_REG_MASK_0X0FFF0FFF;

                        /* BTM */
                        (*(scaler->scl0_ds5)) |= INITIAL_SCALING_PHASE;
                    }
                break;
            }
        }
    }
}   /* End of function SetScalingUp() */

/**************************************************************************//**
 * @fn          CalcScalingRatio
 * @brief       Calculates scaling ratio
 *
 *              Description:<br>
 *              In this function, the overflow in calculation is not considered.
 *              before_scl and after_scl are 11bit width, so the overflow does not occur.
 * @param[in]   before_scl              : Size before scaling
 * @param[in]   after_scl               : Size after scaling
 * @param[in]   adj_sel                 : Handling for lack of last-input pixel/line, or folding handling
 * @param[in]   round_up                : Round-up on/off
 * @retval      Scaling ratio
******************************************************************************/
static uint32_t CalcScalingRatio ( 
    const uint32_t      before_scl,
    const uint32_t      after_scl,
    const vdc_onoff_t   adj_sel,
    const vdc_onoff_t   round_up)
{
    float32_t   ratio;
    float32_t   sigma;
    int32_t     ratio_int;
    uint32_t    scaling_ratio;

    /* When parameter checking is not performed,
       following checks are necessary to avoid a division by zero exception and a negative number. */
    if ((after_scl == 0u) || (after_scl == 1u))
    {
        /* Cast to an appropriate type */
        scaling_ratio = (uint32_t)SCALING_RATIO_SAME_SIZE;
    }
    else
    {
        /* ratio = before_scl * 4096.0 / after_scl */
        ratio  = (float32_t)before_scl * (float32_t)SCALING_RATIO_SAME_SIZE;

        /* Cast to an appropriate type */
        ratio /= (float32_t)after_scl;

        if (adj_sel != VDC_OFF)
        {
            /* Cast to an appropriate type */
            sigma  = ratio * ((float32_t)after_scl - (float32_t)VDC_REG_FLOAT_1_0);

            /* Cast to an appropriate type */
            sigma -= (((float32_t)before_scl - (float32_t)VDC_REG_FLOAT_1_0) * (float32_t)SCALING_RATIO_SAME_SIZE);

            /* Cast to an appropriate type */
            sigma /= ((float32_t)after_scl - (float32_t)VDC_REG_FLOAT_1_0);
            ratio -= sigma;
        }

        if (round_up == VDC_OFF)
        {
            /* Round off */
            ratio    += (float32_t)VDC_REG_FLOAT_0_5;

            /* Cast to an appropriate type */
            ratio_int = (int32_t)ratio;
        }
        else
        {
            /* Round up */
            ratio_int = (int32_t)ratio;

            /* Cast to an appropriate type */
            if ((float32_t)ratio_int < ratio)
            {
                ratio_int++;
            }
        }

        /* Cast to an appropriate type */
        scaling_ratio = (uint32_t)ratio_int;
    }
    return scaling_ratio;
}   /* End of function CalcScalingRatio() */

/**************************************************************************//**
 * @fn          SetInputCntrlColorMtx
 * @brief       Sets color matrix in the input controller
 * @param[in]   void
 * @retval      None
******************************************************************************/
static void SetInputCntrlColorMtx (void) 
{
    vdc_color_space_t      color_space_input;
    vdc_color_space_t      color_space_fb;
    vdc_colormtx_mode_t    mtx_mode_tmp;
    const uint16_t       * offset_tmp;
    const uint16_t       * gain_tmp;

    color_space_input   = VDC_ShrdPrmGetColorSpace();
    color_space_fb      = VDC_ShrdPrmGetColorSpaceFbWr();

    /* Color matrix operating mode */
    if (color_space_input == VDC_COLOR_SPACE_GBR)
    {
        if (color_space_fb == VDC_COLOR_SPACE_GBR)
        {
            /* GBR to GBR */
            mtx_mode_tmp = VDC_COLORMTX_GBR_GBR;
        }
        else
        {
            /* GBR to YCbCr */
            mtx_mode_tmp = VDC_COLORMTX_GBR_YCBCR;
        }
    }
    else
    {
        if (color_space_fb == VDC_COLOR_SPACE_GBR)
        {
            /* YCbCr to GBR */
            mtx_mode_tmp = VDC_COLORMTX_YCBCR_GBR;
        }
        else
        {
            /* YCbCr to YCbCr */
            mtx_mode_tmp = VDC_COLORMTX_YCBCR_YCBCR;
        }
    }

    /* Color matrix offset (DC) adjustment (YG, B, and R) */
    offset_tmp = colormtx_offset_adj;

    /* Color matrix signal gain adjustment (GG, GB, GR, BG, BB, BR, RG, RB, and RR) */
    gain_tmp = colormtx_gain_adj[mtx_mode_tmp];

    SetColorMatrix(mtx_mode_tmp, offset_tmp, gain_tmp, &vdc_regaddr_color_matrix[VDC_COLORMTX_IMGCNT]);

}   /* End of function SetInputCntrlColorMtx() */

/**************************************************************************//**
 * @fn          ConfirmGraphicsEnlargement
 * @brief       Confirms whether the graphics enlargement will occur or not
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   gr_flm_sel              : Frame buffer address setting signal
 * @param[in]   w_read_fb               : Size of the frame buffer to be read
 * @param[in]   gr_grc                  : Graphics display area
 * @retval      Graphics enlargement on/off
******************************************************************************/
static vdc_onoff_t ConfirmGraphicsEnlargement ( 
    const vdc_graphics_type_t          graphics_id,
    const vdc_gr_flm_sel_t             gr_flm_sel,
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc)
{
    vdc_onoff_t graphics_enlargement;

    graphics_enlargement = VDC_OFF;
    if (gr_flm_sel == VDC_GR_FLM_SEL_FLM_NUM)
    {
        if (graphics_id == VDC_GR_TYPE_GR0)
        {
            /* Cast to an appropriate type */
            if (((uint32_t)w_read_fb->in_vw < (uint32_t)gr_grc->vw) ||

                /* Cast to an appropriate type */
                ((uint32_t)w_read_fb->in_hw < (uint32_t)gr_grc->hw))
            {
                graphics_enlargement = VDC_ON;
            }
        }
    }
    else
    {
        /* Cast to an appropriate type */
        if (((uint32_t)w_read_fb->in_vw < (uint32_t)gr_grc->vw) || ((uint32_t)w_read_fb->in_hw < (uint32_t)gr_grc->hw))
        {
            graphics_enlargement = VDC_ON;
        }
    }
    return graphics_enlargement;
}   /* End of function ConfirmGraphicsEnlargement() */

/**************************************************************************//**
 * @fn          SetScalerGraphics
 * @brief       Sets scaler for graphics
 * @param[in]   graphics_id             : Graphics type ID
 * @param[in]   w_read_fb               : Size of the frame buffer to be read
 * @param[in]   gr_grc                  : Graphics display area
 * @param[in]   gr_flm_sel              : Frame buffer address setting signal
 * @param[in]   gr_enlarge              : Graphics enlargement on/off
 * @retval      None
******************************************************************************/
static void SetScalerGraphics ( 
    const vdc_graphics_type_t          graphics_id,
    const vdc_width_read_fb_t  * const w_read_fb,
    const vdc_period_rect_t    * const gr_grc,
    const vdc_gr_flm_sel_t             gr_flm_sel,
    const vdc_onoff_t                  gr_enlarge)
{
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;
    const vdc_regaddr_scaler_t           * scaler;
    vdc_res_inter_t                        res_inter_tmp;
    vdc_wr_md_t                            res_ds_wr_md_tmp;
    vdc_onoff_t                            adj_sel_tmp;
    vdc_onoff_t                            ip_conversion;
    uint32_t                                reg_data;
    vdc_gr_ln_off_dir_t                    ln_off_dir;

    scaler = &vdc_regaddr_scaler;

    if ((gr_flm_sel == VDC_GR_FLM_SEL_FLM_NUM) && (gr_enlarge == VDC_OFF))
    {
        /* Normal graphics display */
        /* Sync signals from the graphics processing block */
        (*(scaler->scl0_us8)) |= (uint32_t)VDC_REG_BIT4;

        /* Register update control register (SC0_SCL0_UPDATE, SC1_SCL0_UPDATE, OIR_SCL0_UPDATE)
           b13      SCL0_VEN_D
           b4       SCL0_VEN_B */
        (*(scaler->scl0_update)) |= (uint32_t)(VDC_REG_BIT13 | VDC_REG_BIT4);
    }
    else
    {
        /* Video image or graphics enlargement */
        /* Sync signals from the scaling-up control block */
        (*(scaler->scl0_us8)) &= (uint32_t)~VDC_REG_BIT4;

        /* Graphics display area */
        (*(scaler->scl0_us2))  = (uint32_t)(gr_grc->vs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(scaler->scl0_us2)) |= (uint32_t)gr_grc->vw;

        /* Cast to an appropriate type */
        (*(scaler->scl0_us3))  = (uint32_t)(gr_grc->hs << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(scaler->scl0_us3)) |= (uint32_t)gr_grc->hw;

        /* Cast to an appropriate type */
        (*(scaler->scl0_us4))  = (uint32_t)(w_read_fb->in_vw << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(scaler->scl0_us4)) |= (uint32_t)w_read_fb->in_hw;

        res_ds_wr_md_tmp    = VDC_ShrdPrmGetWritingMode();
        adj_sel_tmp         = VDC_ShrdPrmGetMeasureFolding(graphics_id);
        ip_conversion       = VDC_OFF;
        if ((gr_flm_sel == VDC_GR_FLM_SEL_SCALE_DOWN) || (gr_flm_sel == VDC_GR_FLM_SEL_POINTER_BUFF))
        {
            res_inter_tmp = VDC_ShrdPrmGetInterlace();
            if (res_inter_tmp != VDC_RES_INTER_PROGRESSIVE)
            {
                ip_conversion = VDC_ON;
            }
        }

        ln_off_dir = VDC_ShrdPrmGetLineOfsAddrDir(graphics_id);

        /* Scaling-up */
        SetScalingUp(w_read_fb, gr_grc, adj_sel_tmp, ip_conversion, res_ds_wr_md_tmp, ln_off_dir, scaler);

        /* Register update control register (SC0_SCL0_UPDATE, SC1_SCL0_UPDATE, OIR_SCL0_UPDATE)
           b13      SCL0_VEN_D
           b8       SCL0_UPDATE
           b4       SCL0_VEN_B
           b0       SCL0_VEN_A */
        (*(scaler->scl0_update)) |= (uint32_t)(VDC_REG_BIT13 | VDC_REG_BIT8 | VDC_REG_BIT4 | VDC_REG_BIT0);
    }

    if (gr_flm_sel != VDC_GR_FLM_SEL_FLM_NUM)
    {
        img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

        /* Frame buffer frame offset address */
        reg_data                    = *(scaler->scl1_wr4);
        (*(img_synthesizer->gr_flm4)) = reg_data;

        /* Graphics register update control register (GRx_UPDATE)
           b0       GRx_IBUS_VEN */
        (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT0;
    }
}   /* End of function SetScalerGraphics() */

/**************************************************************************//**
 * @fn          DisplayStartPixelSetting
 * @brief       Gets the amount of data to be skipped through
 * @param[in]   gr_base                 : Frame buffer base address
 * @param[in]   gr_format               : Graphics format
 * @retval      The amount of data to be skipped through
******************************************************************************/
static uint32_t DisplayStartPixelSetting 
                (const uint32_t gr_base, const vdc_gr_format_t gr_format)
{
    uint32_t                gr_sta_pos;
    static const uint32_t   bits_per_pixel[VDC_GR_FORMAT_NUM] =
    {
        VDC_REG_BIT_PER_PIXEL_RGB565,         /* RGB565 */
        VDC_REG_BIT_PER_PIXEL_RGB888,         /* RGB888 */
        VDC_REG_BIT_PER_PIXEL_ARGB1555,       /* ARGB1555 */
        VDC_REG_BIT_PER_PIXEL_ARGB4444,       /* ARGB4444 */
        VDC_REG_BIT_PER_PIXEL_ARGB8888,       /* ARGB8888 */
        VDC_REG_BIT_PER_PIXEL_CLUT8,          /* CLUT8 */
        VDC_REG_BIT_PER_PIXEL_CLUT4,          /* CLUT4 */
        VDC_REG_BIT_PER_PIXEL_CLUT1,          /* CLUT1 */
        VDC_REG_BIT_PER_PIXEL_YCBCR422,       /* YCbCr422: In the YCbCr422 format, 32 bits are used for two pixels. */
        VDC_REG_BIT_PER_PIXEL_YCBCR444,       /* YCbCr444 */
        VDC_REG_BIT_PER_PIXEL_RGBA5551,       /* RGBA5551 */
        VDC_REG_BIT_PER_PIXEL_RGBA8888        /* RGBA8888 */
    };

    /* Cast to an appropriate type */
    gr_sta_pos  = gr_base & (uint32_t)VDC_REG_MASK_0X00000007;

    /* Cast to an appropriate type */
    gr_sta_pos *= (uint32_t)VDC_REG_BIT_PER_PIXEL_VALUE_8;

    /* Cast to an appropriate type */
    gr_sta_pos /= bits_per_pixel[gr_format];

    /* Cast to an appropriate type */
    gr_sta_pos &= (uint32_t)VDC_REG_MASK_0X0000003F;

    return gr_sta_pos;
}   /* End of function DisplayStartPixelSetting() */

/**************************************************************************//**
 * @fn          SetImgQaImproverColorMtx
 * @brief       Sets color matrix in the image quality improver
 * @param[in]   color_space             : Color space (GBR or YCbCr)
 * @retval      None
******************************************************************************/
static void SetImgQaImproverColorMtx (const vdc_color_space_t color_space) 
{
    vdc_colormtx_mode_t    mtx_mode_tmp;
    const uint16_t        * offset_tmp;
    const uint16_t        * gain_tmp;

    /* Color matrix operating mode */
    if (color_space == VDC_COLOR_SPACE_GBR)
    {
        mtx_mode_tmp = VDC_COLORMTX_GBR_GBR;
    }
    else
    {
        mtx_mode_tmp = VDC_COLORMTX_YCBCR_GBR;
    }

    /* Color matrix offset (DC) adjustment (YG, B, and R) */
    offset_tmp = colormtx_offset_adj;

    /* Color matrix signal gain adjustment (GG, GB, GR, BG, BB, BR, RG, RB, and RR) */
    gain_tmp = colormtx_gain_adj[mtx_mode_tmp];

    SetColorMatrix(mtx_mode_tmp, offset_tmp, gain_tmp, &vdc_regaddr_color_matrix[VDC_COLORMTX_ADJ_0]);

}   /* End of function SetImgQaImproverColorMtx() */

/**************************************************************************//**
 * @fn          SetRwProcAbility_Write_0
 * @brief       Activates/deactivates frame buffer writing for scaler 0
 * @param[in]   ability         : Ability to enable read/write access to the memory
 * @retval      None
******************************************************************************/
static void SetRwProcAbility_Write_0 (const vdc_onoff_t ability) 
{
    const vdc_regaddr_scaler_t   * scaler;
    volatile uint32_t               dummy_read;
    uint32_t                        reg_data;

    /* SC0/GR0 writing */
    scaler = &vdc_regaddr_scaler;

    if (ability == VDC_OFF)

    {   /* SC0 frame buffer writing is disabled */
        (*(scaler->scl1_wr5)) &= (uint32_t)~VDC_REG_BIT0;

        VDC_ShrdPrmSetRwProcDisable(VDC_LAYER_ID_0_WR);
    }
    else
    {
        /* SC0 frame buffer writing is enabled */
        (*(scaler->scl1_wr5)) |= (uint32_t)VDC_REG_BIT0;

        /* Reset and reset cancellation of the pointer buffer with dummy read */
        (*(scaler->scl1_pbuf_cnt)) |= (uint32_t)VDC_REG_BIT16;
        reg_data    = (*(scaler->scl1_pbuf_cnt));
        dummy_read  = reg_data;

        /* Register update control register (SCx_SCL1_UPDATE)
           b20      SCL1_UPDATE_B */
        (*(scaler->scl1_update)) |= (uint32_t)VDC_REG_BIT20;

        /* Cast to an appropriate type */
        (*(scaler->scl1_pbuf_cnt)) &= (uint32_t)~VDC_REG_BIT16;
        reg_data    = *(scaler->scl1_pbuf_cnt);
        dummy_read  = reg_data;

        VDC_ShrdPrmSetRwProcEnable(VDC_LAYER_ID_0_WR);
    }
}   /* End of function SetRwProcAbility_Write_0() */

/**************************************************************************//**
 * @fn          SetRwProcAbility_Read_0
 * @brief       Activates/deactivates frame buffer reading for graphics 0
 * @param[in]   ability         : Ability to enable read/write access to the memory
 * @retval      None
******************************************************************************/
static void SetRwProcAbility_Read_0 (const vdc_onoff_t ability) 
{
    const vdc_regaddr_img_synthesizer_t * img_synthesizer;

    /* SC0/GR0 reading */
    img_synthesizer = &vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR0];

    if (ability == VDC_OFF)
    {
        /* GR0 frame buffer reading is disabled */
        (*(img_synthesizer->gr_flm_rd)) &= (uint32_t)~VDC_REG_BIT0;

        /* Graphics display mode */
        (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_DISPSEL_BACK;

        VDC_ShrdPrmSetRwProcDisable(VDC_LAYER_ID_0_RD);
    }
    else
    {
        /* GR0 frame buffer reading is enabled */
        (*(img_synthesizer->gr_flm_rd)) |= (uint32_t)VDC_REG_BIT0;

        VDC_ShrdPrmSetRwProcEnable(VDC_LAYER_ID_0_RD);
    }
}   /* End of function SetRwProcAbility_Read_0() */

/**************************************************************************//**
 * @fn          SetRwProcAbility_Read_2
 * @brief       Activates/deactivates frame buffer reading for graphics 2
 * @param[in]   ability         : Ability to enable read/write access to the memory
 * @retval      None
******************************************************************************/
static void SetRwProcAbility_Read_2 (const vdc_onoff_t ability) 
{
    const vdc_regaddr_img_synthesizer_t * img_synthesizer;

    /* GR2 reading */
    img_synthesizer = &vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR2];

    if (ability == VDC_OFF)
    {
        /* GR2 frame buffer reading is disabled */
        (*(img_synthesizer->gr_flm_rd)) &= (uint32_t)~VDC_REG_BIT0;

        /* Graphics display mode */
        (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_DISPSEL_LOWER;

        VDC_ShrdPrmSetRwProcDisable(VDC_LAYER_ID_2_RD);
    }
    else
    {
        /* GR2 frame buffer reading is enabled */
        (*(img_synthesizer->gr_flm_rd)) |= (uint32_t)VDC_REG_BIT0;

        VDC_ShrdPrmSetRwProcEnable(VDC_LAYER_ID_2_RD);
    }
}   /* End of function SetRwProcAbility_Read_2() */

/**************************************************************************//**
 * @fn          SetRwProcAbility_Read_3
 * @brief       Activates/deactivates frame buffer reading for graphics 3
 * @param[in]   ability         : Ability to enable read/write access to the memory
 * @retval      None
******************************************************************************/
static void SetRwProcAbility_Read_3 (const vdc_onoff_t ability) 
{
    const vdc_regaddr_img_synthesizer_t * img_synthesizer;

    /* GR3 reading */
    img_synthesizer = &vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3];

    if (ability == VDC_OFF)
    {
        /* GR3 frame buffer reading is disabled */
        (*(img_synthesizer->gr_flm_rd)) &= (uint32_t)~VDC_REG_BIT0;

        /* Graphics display mode */
        (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab1)) |= (uint32_t)VDC_DISPSEL_LOWER;

        VDC_ShrdPrmSetRwProcDisable(VDC_LAYER_ID_3_RD);
    }
    else
    {
        /* GR3 frame buffer reading is enabled */
        (*(img_synthesizer->gr_flm_rd)) |= (uint32_t)VDC_REG_BIT0;

        VDC_ShrdPrmSetRwProcEnable(VDC_LAYER_ID_3_RD);
    }
}   /* End of function SetRwProcAbility_Read_3() */

/**************************************************************************//**
 * @fn          SetRegUpdateRwEnable
 * @brief       Sets register update control register to update the frame buffer
 *              read and write enable settings.
 * @param[in]   layer_id                : Layer ID
 * @retval      None
******************************************************************************/
static void SetRegUpdateRwEnable (const vdc_layer_id_t layer_id) 
{
    volatile uint32_t * scl1_update_reg;
    volatile uint32_t * gr_update_reg;

    switch (layer_id)
    {
        case VDC_LAYER_ID_0_WR:        /* Layer 0, write process */
            scl1_update_reg   = vdc_regaddr_scaler.scl1_update;

            /* Cast to an appropriate type */
            (*scl1_update_reg) |= (uint32_t)(VDC_REG_BIT20 | VDC_REG_BIT4 | VDC_REG_BIT0);
        break;
        case VDC_LAYER_ID_0_RD:        /* Layer 0, read process */
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR0].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
        break;
        case VDC_LAYER_ID_2_RD:        /* Layer 2, read process */
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR2].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
        break;
        case VDC_LAYER_ID_3_RD:        /* Layer 3, read process */
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
        break;

        case VDC_LAYER_ID_ALL:         /* All */

            /* Register update control register (SCx_SCL1_UPDATE)*/
            /* b20      SCL1_UPDATE_B */
            /* b4       SCL1_VEN_B */
            /* b0       SCL1_VEN_A */
            scl1_update_reg   = vdc_regaddr_scaler.scl1_update;

            /* Cast to an appropriate type */
            (*scl1_update_reg) |= (uint32_t)(VDC_REG_BIT20 | VDC_REG_BIT4 | VDC_REG_BIT0);

            /* Graphics register update control register (GRx_UPDATE) */
            /*  b4       GRx_P_VEN */
            /*  b0       GRx_IBUS_VEN */
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR0].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR2].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
            gr_update_reg   = vdc_regaddr_img_synthesizer[VDC_GR_TYPE_GR3].gr_update;

            /* Cast to an appropriate type */
            (*gr_update_reg) |= (uint32_t)(VDC_REG_BIT4|VDC_REG_BIT0);
        break;

        default:

            /* DO NOTHING */
        break;
    }
}   /* End of function SetRegUpdateRwEnable() */

/**************************************************************************//**
 * @fn          SetGraphicsDisplayMode
 * @brief       Sets graphics display mode
 * @param[in]   layer_id                : Layer ID
 * @param[in]   gr_disp_sel             : Graphics display mode
 * @retval      None
******************************************************************************/
static void SetGraphicsDisplayMode  
            (const vdc_layer_id_t layer_id, const vdc_gr_disp_sel_t * const gr_disp_sel)
{
    uint32_t                               graphics_id;
    const vdc_regaddr_img_synthesizer_t  * img_synthesizer;

    if (layer_id == VDC_LAYER_ID_ALL)
    {
        for (graphics_id = 0; graphics_id < VDC_GR_TYPE_NUM; graphics_id++)
        {
            if (gr_disp_sel[graphics_id] != VDC_DISPSEL_IGNORED)
            {
                img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

                /* Cast to an appropriate type */
                (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

                /* Cast to an appropriate type */
                (*(img_synthesizer->gr_ab1)) |= (uint32_t)gr_disp_sel[graphics_id];

                /* Graphics register update control register (GRx_UPDATE)
                   b4       GRx_P_VEN */
                (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;
            }
        }
    }
    else if ((layer_id == VDC_LAYER_ID_0_RD) || (layer_id == VDC_LAYER_ID_2_RD) || (layer_id == VDC_LAYER_ID_3_RD))
    {
        if ((*gr_disp_sel) != VDC_DISPSEL_IGNORED)
        {
            /* Cast to an appropriate type */
            graphics_id = (uint32_t)(layer_id - VDC_SC_TYPE_NUM);

            img_synthesizer = &vdc_regaddr_img_synthesizer[graphics_id];

            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) &= (uint32_t)~VDC_REG_MASK_0X00000003;

            /* Cast to an appropriate type */
            (*(img_synthesizer->gr_ab1)) |= (uint32_t)*gr_disp_sel;

            /* Graphics register update control register (GRx_UPDATE)
               b4       GRx_P_VEN */
            (*(img_synthesizer->gr_update)) |= (uint32_t)VDC_REG_BIT4;
        }
    }
    else
    {
        /* Do nothing */
        ;
    }
}   /* End of function SetGraphicsDisplayMode() */

/**************************************************************************//**
 * @fn          SetColorMatrix
 * @brief       Sets color matrix
 * @param[in]   mtx_mode                : Operating mode
 * @param[in]   offset                  : Offset (DC) adjustment of Y/G, B, and R signal
 * @param[in]   gain                    : GG, GB, GR, BG, BB, BR, RG, RB, and RR signal gain adjustment
 * @param[in]   color_matrix            : Color matrix registers
 * @retval      None
******************************************************************************/
static void SetColorMatrix ( 
    const vdc_colormtx_mode_t                   mtx_mode,
    const uint16_t                      * const offset,
    const uint16_t                      * const gain,
    const vdc_regaddr_color_matrix_t    * const color_matrix)
{
    /* Operating mode */
    (*(color_matrix->mtx_mode)) = (uint32_t)mtx_mode;

    /* Offset (DC) adjustment of Y/G, B, and R signal and
       GG, GB, GR, BG, BB, BR, RG, RB, and RR signal gain adjustment */
    (*(color_matrix->mtx_yg_adj0))  = (uint32_t)(offset[VDC_COLORMTX_OFFST_YG] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_yg_adj0))   |= (uint32_t)gain[VDC_COLORMTX_GAIN_GG];

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_yg_adj1))    = (uint32_t)(gain[VDC_COLORMTX_GAIN_GB] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_yg_adj1))   |= (uint32_t)gain[VDC_COLORMTX_GAIN_GR];

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_cbb_adj0))   = (uint32_t)(offset[VDC_COLORMTX_OFFST_B] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_cbb_adj0))  |= (uint32_t)gain[VDC_COLORMTX_GAIN_BG];

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_cbb_adj1))   = (uint32_t)(gain[VDC_COLORMTX_GAIN_BB] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_cbb_adj1))  |= (uint32_t)gain[VDC_COLORMTX_GAIN_BR];

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_crr_adj0))   = (uint32_t)(offset[VDC_COLORMTX_OFFST_R] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_crr_adj0))  |= (uint32_t)gain[VDC_COLORMTX_GAIN_RG];

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_crr_adj1))   = (uint32_t)(gain[VDC_COLORMTX_GAIN_RB] << VDC_REG_SHIFT_16);

    /* Cast to an appropriate type */
    (*(color_matrix->mtx_crr_adj1))  |= (uint32_t)gain[VDC_COLORMTX_GAIN_RR];

    /* Register update control register (IMGCNT_UPDATE/ADJx_UPDATE)
       b0       IMGCNT_VEN/ADJx_VEN */
    (*(color_matrix->mtx_update)) |= (uint32_t)VDC_REG_BIT0;

}   /* End of function SetColorMatrix() */

/**************************************************************************//**
 * @fn          SetImageEnhancementSharpness
 * @brief       Sets sharpness
 * @param[in]   shp_h_on                : Sharpness ON/OFF setting
 * @param[in]   sharp_param             : Sharpness ON/OFF setting
 * @param[in]   img_qlty_imp            : Image quality improver registers
 * @retval      None
******************************************************************************/
static void SetImageEnhancementSharpness ( 
    const vdc_onoff_t                          shp_h_on,
    const vdc_enhance_sharp_t          * const sharp_param,
    const vdc_regaddr_img_qlty_imp_t   * const img_qlty_imp)
{
    const vdc_sharpness_ctrl_t * sharpness_ctrl;

    /* NULL check */
    if (sharp_param != NULL)
    {
        /* H1, adjacent pixel used as reference */
        sharpness_ctrl = &sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H1];

        /* Active sharpness range */
        (*(img_qlty_imp->adj_enh_shp1)) &= (uint32_t)~VDC_REG_MASK_0X0000003F;

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp1)) |= (uint32_t)sharpness_ctrl->shp_core;

        /* Sharpness correction value clipping and sharpness edge amplitude value gain */
        (*(img_qlty_imp->adj_enh_shp2))  = (uint32_t)(sharpness_ctrl->shp_clip_o << VDC_REG_SHIFT_24);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp2)) |= (uint32_t)(sharpness_ctrl->shp_clip_u << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp2)) |= (uint32_t)(sharpness_ctrl->shp_gain_o << VDC_REG_SHIFT_8);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp2)) |= (uint32_t)sharpness_ctrl->shp_gain_u;

        /* H2, second adjacent pixel used as reference */
        sharpness_ctrl = &sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H2];

        /* LPF selection for folding prevention before H2 edge detection */
        if (sharp_param->shp_h2_lpf_sel == VDC_OFF)
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_shp3)) = (uint32_t)0x00000000u;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_shp3)) = (uint32_t)VDC_REG_BIT16;
        }

        /* Active sharpness range */
        (*(img_qlty_imp->adj_enh_shp3)) |= (uint32_t)sharpness_ctrl->shp_core;

        /* Sharpness correction value clipping and sharpness edge amplitude value gain */
        (*(img_qlty_imp->adj_enh_shp4))  = (uint32_t)(sharpness_ctrl->shp_clip_o << VDC_REG_SHIFT_24);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp4)) |= (uint32_t)(sharpness_ctrl->shp_clip_u << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp4)) |= (uint32_t)(sharpness_ctrl->shp_gain_o << VDC_REG_SHIFT_8);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp4)) |= (uint32_t)sharpness_ctrl->shp_gain_u;

        /* H3, third adjacent pixel used as reference */
        sharpness_ctrl = &sharp_param->hrz_sharp[VDC_IMGENH_SHARP_H3];

        /* Active sharpness range */
        (*(img_qlty_imp->adj_enh_shp5)) = (uint32_t)sharpness_ctrl->shp_core;

        /* Sharpness correction value clipping and sharpness edge amplitude value gain */
        (*(img_qlty_imp->adj_enh_shp6))  = (uint32_t)(sharpness_ctrl->shp_clip_o << VDC_REG_SHIFT_24);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp6)) |= (uint32_t)(sharpness_ctrl->shp_clip_u << VDC_REG_SHIFT_16);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp6)) |= (uint32_t)(sharpness_ctrl->shp_gain_o << VDC_REG_SHIFT_8);

        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp6)) |= (uint32_t)sharpness_ctrl->shp_gain_u;
    }

    if (shp_h_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp1)) &= (uint32_t)~VDC_REG_BIT16;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_shp1)) |= (uint32_t)VDC_REG_BIT16;
    }
}   /* End of function SetImageEnhancementSharpness() */

/**************************************************************************//**
 * @fn          SetImageEnhancementLti
 * @brief       Sets LTI
 * @param[in]   lti_h_on                : LTI ON/OFF setting
 * @param[in]   lti_param               : LTI setup parameter
 * @param[in]   img_qlty_imp            : Image quality improver registers
 * @retval      None
******************************************************************************/
static void SetImageEnhancementLti ( 
    const vdc_onoff_t                          lti_h_on,
    const vdc_enhance_lti_t            * const lti_param,
    const vdc_regaddr_img_qlty_imp_t   * const img_qlty_imp)
{
    const vdc_lti_ctrl_t * lti_ctrl;

    /* NULL check */
    if (lti_param != NULL)
    {
        /* H2, second adjacent pixel used as reference */
        lti_ctrl = &lti_param->lti[VDC_IMGENH_LTI1];

        /* LPF selection for folding prevention before H2 edge detection */
        if (lti_param->lti_h2_lpf_sel == VDC_OFF)
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_lti1)) = (uint32_t)0x00000000u;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_lti1)) = (uint32_t)VDC_REG_BIT24;
        }

        /* Median filter LTI correction threshold */
        (*(img_qlty_imp->adj_enh_lti1)) |= (uint32_t)((uint32_t)lti_ctrl->lti_inc_zero << VDC_REG_SHIFT_16);

        /* LTI edge amplitude value gain */
        (*(img_qlty_imp->adj_enh_lti1)) |= (uint32_t)((uint32_t)lti_ctrl->lti_gain << VDC_REG_SHIFT_8);

        /* LTI coring (maximum core value of 255) */
        (*(img_qlty_imp->adj_enh_lti1)) |= (uint32_t)lti_ctrl->lti_core;

        /* H4, fourth adjacent pixel used as reference */
        lti_ctrl = &lti_param->lti[VDC_IMGENH_LTI2];

        /* Median filter reference pixel select */
        if (lti_param->lti_h4_median_tap_sel == VDC_LTI_MDFIL_SEL_ADJ2)
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_lti2)) = (uint32_t)0x00000000u;
        }
        else
        {
            /* Cast to an appropriate type */
            (*(img_qlty_imp->adj_enh_lti2)) = (uint32_t)VDC_REG_BIT24;
        }

        /* Median filter LTI correction threshold */
        (*(img_qlty_imp->adj_enh_lti2)) |= (uint32_t)((uint32_t)lti_ctrl->lti_inc_zero << VDC_REG_SHIFT_16);

        /* LTI edge amplitude value gain */
        (*(img_qlty_imp->adj_enh_lti2)) |= (uint32_t)((uint32_t)lti_ctrl->lti_gain << VDC_REG_SHIFT_8);

        /* LTI coring (maximum core value of 255) */
        (*(img_qlty_imp->adj_enh_lti2)) |= (uint32_t)lti_ctrl->lti_core;
    }

    if (lti_h_on == VDC_OFF)
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_lti1)) &= (uint32_t)~VDC_REG_BIT31;
    }
    else
    {
        /* Cast to an appropriate type */
        (*(img_qlty_imp->adj_enh_lti1)) |= (uint32_t)VDC_REG_BIT31;
    }
}   /* End of function SetImageEnhancementLti() */

/**************************************************************************//**
 * @fn          SetAlphaRectArea
 * @brief       Sets alpha blending area of a rectangle
 * @param[in]   pd_disp_rect        : Rectangular area for alpha blending
 * @param[in]   img_synthesizer     : Image synthesizer registers
 * @retval      None
******************************************************************************/
static void SetAlphaRectArea ( 
    const vdc_pd_disp_rect_t               * const pd_disp_rect,
    const vdc_regaddr_img_synthesizer_t    * const img_synthesizer)
{
    uint32_t v_start;
    uint32_t h_start;

    /* NULL check */
    if (pd_disp_rect != NULL)
    {
        v_start = (*(img_synthesizer->gr_ab2)) >> VDC_REG_SHIFT_16;
        h_start = (*(img_synthesizer->gr_ab3)) >> VDC_REG_SHIFT_16;

        /* Cast to an appropriate type */
        v_start += (uint32_t)pd_disp_rect->vs_rel;

        /* Cast to an appropriate type */
        h_start += (uint32_t)pd_disp_rect->hs_rel;

        /* If the start position exceeds the maximum bit width, the value should be saturated. */
        if ((v_start & (uint32_t)~VDC_REG_MASK_0X000007FF) != 0u)
        {
            /* Cast to an appropriate type */
            v_start = (uint32_t)VDC_REG_MASK_0X000007FF;
        }

        /* Cast to an appropriate type */
        if ((h_start & (uint32_t)~VDC_REG_MASK_0X000007FF) != 0u)
        {
            /* Cast to an appropriate type */
            h_start = (uint32_t)VDC_REG_MASK_0X000007FF;
        }

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab4)) = ((v_start << VDC_REG_SHIFT_16) | (uint32_t)pd_disp_rect->vw_rel);

        /* Cast to an appropriate type */
        (*(img_synthesizer->gr_ab5)) = ((h_start << VDC_REG_SHIFT_16) | (uint32_t)pd_disp_rect->hw_rel);
    }
}   /* End of function SetAlphaRectArea() */

/**************************************************************************//**
 * @fn          ColorConversion
 * @brief       Converts the color format from gr_format format into 24-bit RGB888 format
 * @param[in]   gr_format           : Color format
 * @param[in]   input_color         : Color data
 * @retval      RGB888 color data
******************************************************************************/
static uint32_t ColorConversion  
                (const vdc_gr_format_t gr_format, const uint32_t input_color)
{
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t conv_color;

    conv_color = 0;
    switch (gr_format)
    {
        case VDC_GR_FORMAT_RGB565:

            /* Cast to an appropriate type */
            red   = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_RGB565_R_MASK) >> VDC_REG_SHIFT_11));

            /* Cast to an appropriate type */
            green = ColorConversionFrom6to8((uint32_t)((input_color & VDC_REG_RGB565_G_MASK) >> VDC_REG_SHIFT_5));

            /* Cast to an appropriate type */
            blue  = ColorConversionFrom5to8((uint32_t)(input_color & VDC_REG_RGB565_B_MASK));
            conv_color = ((green << VDC_REG_SHIFT_16) | (blue << VDC_REG_SHIFT_8) | red);
        break;
        case VDC_GR_FORMAT_RGB888:
            conv_color = ColorConversionIntoAgbr(input_color);
        break;
        case VDC_GR_FORMAT_ARGB1555:

            /* Cast to an appropriate type */
            red   = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_ARGB1555_R_MASK) >> VDC_REG_SHIFT_10));

            /* Cast to an appropriate type */
            green = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_ARGB1555_G_MASK) >> VDC_REG_SHIFT_5));

            /* Cast to an appropriate type */
            blue  = ColorConversionFrom5to8((uint32_t)(input_color & VDC_REG_ARGB1555_B_MASK));
            conv_color = ((green << VDC_REG_SHIFT_16) | (blue << VDC_REG_SHIFT_8) | red);
        break;
        case VDC_GR_FORMAT_ARGB4444:

            /* Cast to an appropriate type */
            red   = ColorConversionFrom4to8((uint32_t)((input_color & VDC_REG_ARGB4444_R_MASK) >> VDC_REG_SHIFT_8));

            /* Cast to an appropriate type */
            green = ColorConversionFrom4to8((uint32_t)((input_color & VDC_REG_ARGB4444_G_MASK) >> VDC_REG_SHIFT_4));

            /* Cast to an appropriate type */
            blue  = ColorConversionFrom4to8((uint32_t)(input_color & VDC_REG_ARGB4444_B_MASK));
            conv_color = ((green << VDC_REG_SHIFT_16) | (blue << VDC_REG_SHIFT_8) | red);
        break;
        case VDC_GR_FORMAT_ARGB8888:

            /* Cast to an appropriate type */
            conv_color = ColorConversionIntoAgbr((uint32_t)(input_color & VDC_REG_RGB_24BIT_MASK));
        break;
        case VDC_GR_FORMAT_RGBA5551:

            /* Cast to an appropriate type */
            red   = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_RGBA5551_R_MASK) >> VDC_REG_SHIFT_11));

            /* Cast to an appropriate type */
            green = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_RGBA5551_G_MASK) >> VDC_REG_SHIFT_6));

            /* Cast to an appropriate type */
            blue  = ColorConversionFrom5to8((uint32_t)((input_color & VDC_REG_RGBA5551_B_MASK) >> VDC_REG_SHIFT_1));
            conv_color = ((green << VDC_REG_SHIFT_16) | (blue << VDC_REG_SHIFT_8) | red);
        break;
        case VDC_GR_FORMAT_RGBA8888:


            conv_color= ColorConversionIntoAgbr

            /* Cast to an appropriate type */
            ((uint32_t)((input_color >> VDC_REG_SHIFT_8) & VDC_REG_RGB_24BIT_MASK));
        break;
        default:

            /* DO NOTHING */
        break;
    }
    return conv_color;
}   /* End of function ColorConversion() */

/**************************************************************************//**
 * @fn          ColorConversionFrom4to8
 * @brief       Converts the color data from 4 bits value into 8 bits value
 * @param[in]   color_value         : 4-bit color data
 * @retval      8-bit color data
******************************************************************************/
static uint32_t ColorConversionFrom4to8 (const uint32_t color_value) 
{
    uint32_t color;

    /* Cast to an appropriate type */
    color = color_value & (uint32_t)VDC_REG_COLOR_4BIT_MASK;

    /* Cast to an appropriate type */
    color *= (uint32_t)VDC_REG_EXTENSION_VALUE_17;

    return color;
}   /* End of function ColorConversionFrom4to8() */

/**************************************************************************//**
 * @fn          ColorConversionFrom5to8
 * @brief       Converts the color data from 5 bits value into 8 bits value
 * @param[in]   color_value         : 5-bit color data
 * @retval      8-bit color data
******************************************************************************/
static uint32_t ColorConversionFrom5to8 (const uint32_t color_value) 
{
    uint32_t color;

    /* Cast to an appropriate type */
    color = color_value & (uint32_t)VDC_REG_COLOR_5BIT_MASK;

    /* Cast to an appropriate type */
    color *= (uint32_t)(VDC_REG_EXTENSION_VALUE_263 * VDC_REG_ROUND_OFF_VALUE_2);

    /* Cast to an appropriate type */
    color /= (uint32_t)VDC_REG_EXTENSION_VALUE_32;

    /* Round off */
    color += (uint32_t)VDC_REG_ROUND_OFF_VALUE_1;

    /* Cast to an appropriate type */
    color /= (uint32_t)VDC_REG_ROUND_OFF_VALUE_2;

    return color;
}   /* End of function ColorConversionFrom5to8() */

/**************************************************************************//**
 * @fn          ColorConversionFrom6to8
 * @brief       Converts the color data from 6 bits value into 8 bits value
 * @param[in]   color_value         : 6-bit color data
 * @retval      8-bit color data
******************************************************************************/
static uint32_t ColorConversionFrom6to8 (const uint32_t color_value) 
{
    uint32_t color;

    /* Cast to an appropriate type */
    color = color_value & (uint32_t)VDC_REG_COLOR_6BIT_MASK;

    /* Cast to an appropriate type */
    color *= (uint32_t)(VDC_REG_EXTENSION_VALUE_259 * VDC_REG_ROUND_OFF_VALUE_2);

    /* Cast to an appropriate type */
    color /= (uint32_t)VDC_REG_EXTENSION_VALUE_64;

    /* Round off */
    color += (uint32_t)VDC_REG_ROUND_OFF_VALUE_1;

    /* Cast to an appropriate type */
    color /= (uint32_t)VDC_REG_ROUND_OFF_VALUE_2;

    return color;
}   /* End of function ColorConversionFrom6to8() */

/**************************************************************************//**
 * @fn          ColorConversionIntoAgbr
 * @brief       Converts the color data from ARGB8888 format into AGBR8888 format
 * @param[in]   color_value         : ARGB8888 color data
 * @retval      AGBR8888 color data
******************************************************************************/
static uint32_t ColorConversionIntoAgbr (const uint32_t color_value) 
{
    uint32_t color_bgr;
    uint32_t color_dat;

    /* Green and blue */
    color_bgr   = (uint32_t)(color_value << VDC_REG_SHIFT_8);

    /* Cast to an appropriate type */
    color_bgr  &= (uint32_t)VDC_REG_ARGB8888_GB_MASK;

    /* Alpha */
    color_dat   = (uint32_t)(color_value & VDC_REG_ALPHA_8BIT);
    color_bgr  |= color_dat;

    /* Red */
    color_dat   = (uint32_t)((color_value >> VDC_REG_SHIFT_16) & VDC_REG_COLOR_8BIT_MASK);
    color_bgr  |= color_dat;

    return color_bgr;
}   /* End of function ColorConversionIntoAgbr() */

/**************************************************************************//**
 * @fn          Set_Clut
 * @brief       Sets color lookup table (CLUT)
 * @param[in]   param               : CLUT parameter
 * @param[out]  regaddr_clut        : CLUT register top address
 * @retval      None
******************************************************************************/
static void Set_Clut  
            (const vdc_clut_t * const param, volatile uint32_t * regaddr_clut)
{
    uint32_t            data_len;
    const uint32_t    * clut_tmp;

    clut_tmp = param->clut;
    for (data_len = 0; data_len < param->color_num; data_len++)
    {
        (*regaddr_clut) = (*clut_tmp);
        regaddr_clut++;
        clut_tmp++;
    }
}   /* End of function Set_Clut() */

/**************************************************************************//**
 * @fn          Set_StartThreshold_Gamma
 * @brief       Sets start threshold for gamma correction
 * @param[in]   gam_th          : Start threshold of area 1 to 31
 * @param[out]  gam_area        : Gamma correction registers for start threshold
 * @retval      None
******************************************************************************/
static void Set_StartThreshold_Gamma  
            (const uint8_t * gam_th, volatile uint32_t * const * const gam_area)
{
    uint32_t reg_index;

    /* NULL check */
    if (gam_th != NULL)
    {
        /* Cast to an appropriate type */
        (*(gam_area[0]))  = (uint32_t)((*gam_th) << VDC_REG_SHIFT_16);
        gam_th++;

        /* Cast to an appropriate type */
        (*(gam_area[0])) |= (uint32_t)((*gam_th) << VDC_REG_SHIFT_8);
        gam_th++;

        /* Cast to an appropriate type */
        (*(gam_area[0])) |= (uint32_t)*gam_th;
        gam_th++;

        for (reg_index = 1; reg_index < VDC_GAM_AREA_REG_NUM; reg_index++)
        {
            /* Cast to an appropriate type */
            (*(gam_area[reg_index]))  = (uint32_t)((*gam_th) << VDC_REG_SHIFT_24);
            gam_th++;

            /* Cast to an appropriate type */
            (*(gam_area[reg_index])) |= (uint32_t)((*gam_th) << VDC_REG_SHIFT_16);
            gam_th++;

            /* Cast to an appropriate type */
            (*(gam_area[reg_index])) |= (uint32_t)((*gam_th) << VDC_REG_SHIFT_8);
            gam_th++;

            /* Cast to an appropriate type */
            (*(gam_area[reg_index])) |= (uint32_t)*gam_th;
            gam_th++;
        }
    }
}   /* End of function Set_StartThreshold_Gamma() */

/**************************************************************************//**
 * @fn          Set_GainAdjustment_Gamma
 * @brief       Sets gain adjustment for gamma correction
 * @param[in]   gam_gain        : Gain adjustment of area 0 to 31
 * @param[out]  gam_lut         : Gamma correction registers for gain adjustment
 * @retval      None
******************************************************************************/
static void Set_GainAdjustment_Gamma  
            (const uint16_t * gam_gain, volatile uint32_t * const * const gam_lut)
{
    uint32_t reg_index;

    /* NULL check */
    if (gam_gain != NULL)
    {
        for (reg_index = 0; reg_index < VDC_GAM_LUT_REG_NUM; reg_index++)
        {
            /* Cast to an appropriate type */
            (*(gam_lut[reg_index]))  = (uint32_t)((*gam_gain) << VDC_REG_SHIFT_16);
            gam_gain++;

            /* Cast to an appropriate type */
            (*(gam_lut[reg_index])) |= (uint32_t)*gam_gain;
            gam_gain++;
        }
    }
}   /* End of function Set_GainAdjustment_Gamma() */

/**************************************************************************//**
 * @fn          Wait_200_usec
 * @brief       Waits for 200 usec
 * @param[in]   void
 * @retval      None
******************************************************************************/
static void Wait_200_usec (void) 
{
    volatile uint32_t   counter;

    /* Cast to an appropriate type */
    for (counter = 0; counter < (uint32_t)VDC_LVDS_PLL_WAIT_200USEC; counter++)
    {
        /* Wait for 200 usec. */
        ;
    }
}   /* End of function Wait_200_usec() */

