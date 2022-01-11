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
 * File Name :   r_rvapi_vdc.c
 * @file         r_rvapi_vdc.c
 * @version      1.12
 * @brief        RVAPI for VDC function
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include    "r_rvapi_vdc.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define WEITE_LAYER_NUM  (2u)

#define LAYER_ID_0_WR    (0u)
#define LAYER_ID_1_WR    (1u)

#define DOUBLE_BUFFER    (2u)
#define BUFFER_0         (0u)
#define BUFFER_1         (1u)

#define CLUT8_TABLE_NUM  (256u)
#define CLUT4_TABLE_NUM  (16u)
#define CLUT1_TABLE_NUM  (2u)

#define BLACK            (0xFF000000u)
#define WHITE            (0xFFFFFFFFu)

#define LVDSPLL_TST (0u)     /* LVDSPLL_TST (LVDS PLL internal parameters) */

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
volatile vdc_period_rect_t full_screen;

/* default Monochrome color 256 ARGB   */
static const uint32_t color_table256[CLUT8_TABLE_NUM] =
{
BLACK,
0xFF010101u, 0xFF020202u, 0xFF030303u, 0xFF040404u, 0xFF050505u, 0xFF060606u, 0xFF070707u, /*   0 -   7 */
0xFF080808u, 0xFF090909u, 0xFF0A0A0Au, 0xFF0B0B0Bu, 0xFF0C0C0Cu, 0xFF0D0D0Du, 0xFF0E0E0Eu, 0xFF0F0F0Fu, /*   8 -  15 */
0xFF101010u, 0xFF111111u, 0xFF121212u, 0xFF131313u, 0xFF141414u, 0xFF151515u, 0xFF161616u, 0xFF171717u, /*  16 -  23 */
0xFF181818u, 0xFF191919u, 0xFF1A1A1Au, 0xFF1B1B1Bu, 0xFF1C1C1Cu, 0xFF1E1E1Eu, 0xFF1E1E1Eu, 0xFF1F1F1Fu, /*  24 -  31 */
0xFF202020u, 0xFF212121u, 0xFF222222u, 0xFF232323u, 0xFF242424u, 0xFF252525u, 0xFF262626u, 0xFF272727u, /*  32 -  39 */
0xFF282828u, 0xFF292929u, 0xFF2A2A2Au, 0xFF2B2B2Bu, 0xFF2C2C2Cu, 0xFF2E2E2Eu, 0xFF2E2E2Eu, 0xFF2F2F2Fu, /*  40 -  47 */
0xFF303030u, 0xFF313131u, 0xFF323232u, 0xFF333333u, 0xFF343434u, 0xFF353535u, 0xFF363636u, 0xFF373737u, /*  48 -  55 */
0xFF383838u, 0xFF393939u, 0xFF3A3A3Au, 0xFF3B3B3Bu, 0xFF3C3C3Cu, 0xFF3E3E3Eu, 0xFF3E3E3Eu, 0xFF3F3F3Fu, /*  56 -  63 */
0xFF404040u, 0xFF414141u, 0xFF424242u, 0xFF434343u, 0xFF444444u, 0xFF454545u, 0xFF464646u, 0xFF474747u, /*  64 -  71 */
0xFF484848u, 0xFF494949u, 0xFF4A4A4Au, 0xFF4B4B4Bu, 0xFF4C4C4Cu, 0xFF4E4E4Eu, 0xFF4E4E4Eu, 0xFF4F4F4Fu, /*  72 -  79 */
0xFF505050u, 0xFF515151u, 0xFF525252u, 0xFF535353u, 0xFF545454u, 0xFF555555u, 0xFF565656u, 0xFF575757u, /*  80 -  87 */
0xFF585858u, 0xFF595959u, 0xFF5A5A5Au, 0xFF5B5B5Bu, 0xFF5C5C5Cu, 0xFF5E5E5Eu, 0xFF5E5E5Eu, 0xFF5F5F5Fu, /*  88 -  95 */
0xFF606060u, 0xFF616161u, 0xFF626262u, 0xFF636363u, 0xFF646464u, 0xFF656565u, 0xFF666666u, 0xFF676767u, /*  96 - 103 */
0xFF686868u, 0xFF696969u, 0xFF6A6A6Au, 0xFF6B6B6Bu, 0xFF6C6C6Cu, 0xFF6E6E6Eu, 0xFF6E6E6Eu, 0xFF6F6F6Fu, /* 104 - 111 */
0xFF707070u, 0xFF717171u, 0xFF727272u, 0xFF737373u, 0xFF747474u, 0xFF757575u, 0xFF767676u, 0xFF777777u, /* 112 - 119 */
0xFF787878u, 0xFF797979u, 0xFF7A7A7Au, 0xFF7B7B7Bu, 0xFF7C7C7Cu, 0xFF7E7E7Eu, 0xFF7E7E7Eu, 0xFF7F7F7Fu, /* 120 - 127 */
0xFF808080u, 0xFF818181u, 0xFF828282u, 0xFF838383u, 0xFF848484u, 0xFF858585u, 0xFF868686u, 0xFF878787u, /* 128 - 135 */
0xFF888888u, 0xFF898989u, 0xFF8A8A8Au, 0xFF8B8B8Bu, 0xFF8C8C8Cu, 0xFF8E8E8Eu, 0xFF8E8E8Eu, 0xFF8F8F8Fu, /* 136 - 143 */
0xFF909090u, 0xFF919191u, 0xFF929292u, 0xFF939393u, 0xFF949494u, 0xFF959595u, 0xFF969696u, 0xFF979797u, /* 144 - 151 */
0xFF989898u, 0xFF999999u, 0xFF9A9A9Au, 0xFF9B9B9Bu, 0xFF9C9C9Cu, 0xFF9E9E9Eu, 0xFF9E9E9Eu, 0xFF9F9F9Fu, /* 152 - 159 */
0xFFA0A0A0u, 0xFFA1A1A1u, 0xFFA2A2A2u, 0xFFA3A3A3u, 0xFFA4A4A4u, 0xFFA5A5A5u, 0xFFA6A6A6u, 0xFFA7A7A7u, /* 160 - 167 */
0xFFA8A8A8u, 0xFFA9A9A9u, 0xFFAAAAAAu, 0xFFABABABu, 0xFFACACACu, 0xFFAEAEAEu, 0xFFAEAEAEu, 0xFFAFAFAFu, /* 168 - 175 */
0xFFB0B0B0u, 0xFFB1B1B1u, 0xFFB2B2B2u, 0xFFB3B3B3u, 0xFFB4B4B4u, 0xFFB5B5B5u, 0xFFB6B6B6u, 0xFFB7B7B7u, /* 176 - 183 */
0xFFB8B8B8u, 0xFFB9B9B9u, 0xFFBABABAu, 0xFFBBBBBBu, 0xFFBCBCBCu, 0xFFBEBEBEu, 0xFFBEBEBEu, 0xFFBFBFBFu, /* 184 - 191 */
0xFFC0C0C0u, 0xFFC1C1C1u, 0xFFC2C2C2u, 0xFFC3C3C3u, 0xFFC4C4C4u, 0xFFC5C5C5u, 0xFFC6C6C6u, 0xFFC7C7C7u, /* 192 - 199 */
0xFFC8C8C8u, 0xFFC9C9C9u, 0xFFCACACAu, 0xFFCBCBCBu, 0xFFCCCCCCu, 0xFFCECECEu, 0xFFCECECEu, 0xFFCFCFCFu, /* 200 - 207 */
0xFFD0D0D0u, 0xFFD1D1D1u, 0xFFD2D2D2u, 0xFFD3D3D3u, 0xFFD4D4D4u, 0xFFD5D5D5u, 0xFFD6D6D6u, 0xFFD7D7D7u, /* 208 - 215 */
0xFFD8D8D8u, 0xFFD9D9D9u, 0xFFDADADAu, 0xFFDBDBDBu, 0xFFDCDCDCu, 0xFFDEDEDEu, 0xFFDEDEDEu, 0xFFDFDFDFu, /* 216 - 223 */
0xFFE0E0E0u, 0xFFE1E1E1u, 0xFFE2E2E2u, 0xFFE3E3E3u, 0xFFE4E4E4u, 0xFFE5E5E5u, 0xFFE6E6E6u, 0xFFE7E7E7u, /* 224 - 231 */
0xFFE8E8E8u, 0xFFE9E9E9u, 0xFFEAEAEAu, 0xFFEBEBEBu, 0xFFECECECu, 0xFFEEEEEEu, 0xFFEEEEEEu, 0xFFEFEFEFu, /* 232 - 239 */
0xFFF0F0F0u, 0xFFF1F1F1u, 0xFFF2F2F2u, 0xFFF3F3F3u, 0xFFF4F4F4u, 0xFFF5F5F5u, 0xFFF6F6F6u, 0xFFF7F7F7u, /* 240 - 247 */
0xFFF8F8F8u, 0xFFF9F9F9u, 0xFFFAFAFAu, 0xFFFBFBFBu, 0xFFFCFCFCu, 0xFFFEFEFEu, 0xFFFEFEFEu, WHITE /* 248 - 255 */
};

/* Base 16 color ARGB */
static const uint32_t color_table16[CLUT4_TABLE_NUM] =
{
BLACK,
0xFFFF0000u, 0xFF00FF00u, 0xFF00FFFFu, 0xFF0000FFu, 0xFFFF00FFu, 0xFF800000u, 0xFF808000u, 0xFF008080u, 0xFF000080u,
0xFF800080u, 0xFF808080u, 0xFFC0C0C0u, 0xFFFFA500u, WHITE, 0x00FFFFFFu };

/* Base 2 color ARGB */
static const uint32_t color_table2[CLUT1_TABLE_NUM] =
{
BLACK,
WHITE };
/******************************************************************************
 Exported global variables
 ******************************************************************************/
extern void R_VDC_OnInitialize
            ( const uint32_t user_num );
extern void R_VDC_OnFinalize
            ( const uint32_t user_num );


/**************************************************************************//**
 * Function Name : R_RVAPI_InitializeVDC
 * @brief       VDC initialization processing
 * @param[in]   ch              : Channel
 * @param[in]   c_cnf           : Clock setting config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_InitializeVDC
            (const vdc_channel_t ch, const clock_config_t * const c_cnf)
{
    vdc_error_t error;
    vdc_init_t init;
    vdc_lvds_t lvds;

    /* full screen init */
    full_screen.vs = 0u;
    full_screen.vw = 0u;
    full_screen.hs = 0u;
    full_screen.hw = 0u;

    /* pointer NULL check */
    if (NULL == c_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Initialization parameter */
        init.panel_icksel = c_cnf->panel_clk; /* Panel clock select */
        init.panel_dcdr = c_cnf->panel_clk_div; /* Panel clock frequency division ratio */

        /* lvds */
        if (NULL != c_cnf->lvds)
        {
            lvds.lvds_idiv_set = c_cnf->lvds->lvds_idiv_set;
            lvds.lvds_in_clk_sel = c_cnf->lvds->lvds_in_clk_sel;
            lvds.lvds_odiv_set = c_cnf->lvds->lvds_odiv_set;
            lvds.lvds_vdc_sel = c_cnf->lvds->lvds_vdc_sel;
            lvds.lvdspll_fd = c_cnf->lvds->lvdspll_fd;
            lvds.lvdspll_od = c_cnf->lvds->lvdspll_od;
            lvds.lvdspll_rd = c_cnf->lvds->lvdspll_rd;
            lvds.lvdspll_tst = LVDSPLL_TST;
            init.lvds = &lvds; /* LVDS parameter */
        }
        else
        {
            /* Set NULL */
            init.lvds = NULL;
        }

        /* Initialize */
        error = R_VDC_Initialize (ch, &init, &R_VDC_OnInitialize, (uint32_t) ch);
    }

    return error;
} /* End of function R_RVAPI_InitializeVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_TerminateVDC
 * @brief       VDC terminate processing
 * @param[in]   ch              : Channel
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_TerminateVDC(const vdc_channel_t ch)
{
    vdc_error_t error;

    /* Finalize */
    error = R_VDC_Terminate (ch, &R_VDC_OnFinalize, (uint32_t) ch);

    return error;
} /* End of function R_RVAPI_TerminateVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_DispControlVDC
 * @brief       VDC display out control processing
 * @param[in]   ch              : Channel
 * @param[in]   res_vs_sel      : Vsync signal output select
 *                                - VDC_OFF: External input Vsync signal
 *                                - VDC_ON : Internally generated free-running Vsync signal
 * @param[in]   q_cnf           : LCD setting config (QE for VDC header)
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_DispControlVDC
        (const vdc_channel_t ch, const vdc_onoff_t res_vs_sel,
        const qe_config_t * const q_cnf)
{
    vdc_error_t error;
    vdc_sync_ctrl_t sync_ctrl;

    /* Set NULL */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Sync signal control */
        sync_ctrl.res_vs_sel = res_vs_sel; /* Vsync signal output select (free-running Vsync on/off control) */

        /* Sync signal output and full-screen enable signal select */
        sync_ctrl.res_vs_in_sel = VDC_RES_VS_IN_SEL_SC0;
        sync_ctrl.res_fv = q_cnf->vtp; /* Free-running Vsync period setting */
        sync_ctrl.res_fh = q_cnf->htp; /* Hsync period setting */
        sync_ctrl.res_vsdly = (uint16_t) 0u; /* Vsync signal delay control */

        /* Full-screen enable control */
        sync_ctrl.res_f.vs = q_cnf->vs;
        sync_ctrl.res_f.vw = q_cnf->vdp;
        sync_ctrl.res_f.hs = q_cnf->hs;
        sync_ctrl.res_f.hw = q_cnf->hdp;

        /* Set NULL */
        sync_ctrl.vsync_cpmpe = NULL; /* Vsync signal compensation */

        /* Sync control */
        error = R_VDC_SyncControl (ch, &sync_ctrl);
    }

    /* pointer NULL check */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (VDC_OK == error)
        {
            /* full screen vale set */
            full_screen.vs = q_cnf->vs;
            full_screen.vw = q_cnf->vdp;
            full_screen.hs = q_cnf->hs;
            full_screen.hw = q_cnf->hdp;
        }
    }

    /* pointer NULL check */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (VDC_OK == error)
        {
            vdc_output_t output;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_vs;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_hs;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_ve;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_he;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_de;

            lcd_tcon_timing_vs.tcon_hsvs = q_cnf->vps; /* Signal pulse start position */
            lcd_tcon_timing_vs.tcon_hwvw = q_cnf->vpw; /* Pulse width */
            lcd_tcon_timing_vs.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_vs.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_vs.tcon_inv = q_cnf->tcon_vsync_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_vs.tcon_pin = q_cnf->tcon_vsync; /* LCD TCON output pin select */
            lcd_tcon_timing_vs.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_hs.tcon_hsvs = q_cnf->hps; /* Signal pulse start position */
            lcd_tcon_timing_hs.tcon_hwvw = q_cnf->hpw; /* Pulse width */
            lcd_tcon_timing_hs.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_hs.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_hs.tcon_inv = q_cnf->tcon_hsync_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_hs.tcon_pin = q_cnf->tcon_hsync; /* LCD TCON output pin select */
            lcd_tcon_timing_hs.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_de.tcon_hsvs = 0u; /* Signal pulse start position */
            lcd_tcon_timing_de.tcon_hwvw = 0u; /* Pulse width */
            lcd_tcon_timing_de.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_de.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_de.tcon_inv = q_cnf->tcon_de_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_de.tcon_pin = q_cnf->tcon_de; /* LCD TCON output pin select */
            lcd_tcon_timing_de.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_ve.tcon_hsvs = q_cnf->vs; /* Signal pulse start position */
            lcd_tcon_timing_ve.tcon_hwvw = q_cnf->vdp; /* Pulse width */
            lcd_tcon_timing_ve.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_ve.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_ve.tcon_inv = VDC_SIG_POL_NOT_INVERTED; /* Polarity inversion control of signal */
            lcd_tcon_timing_ve.tcon_pin = VDC_LCD_TCON_PIN_NON; /* LCD TCON output pin select */
            lcd_tcon_timing_ve.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_he.tcon_hsvs = q_cnf->hs; /* Signal pulse start position */
            lcd_tcon_timing_he.tcon_hwvw = q_cnf->hdp; /* Pulse width */
            lcd_tcon_timing_he.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_he.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_he.tcon_inv = VDC_SIG_POL_NOT_INVERTED; /* Polarity inversion control of signal */
            lcd_tcon_timing_he.tcon_pin = VDC_LCD_TCON_PIN_NON; /* LCD TCON output pin select */
            lcd_tcon_timing_he.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            output.outctrl[VDC_LCD_TCONSIG_STVA_VS] = (&lcd_tcon_timing_vs); /* STVA/VS: Vsync */
            output.outctrl[VDC_LCD_TCONSIG_STVB_VE] = (&lcd_tcon_timing_ve); /* STVB/VE: Not used */
            output.outctrl[VDC_LCD_TCONSIG_STH_SP_HS] = (&lcd_tcon_timing_hs); /* STH/SP/HS: Hsync */
            output.outctrl[VDC_LCD_TCONSIG_STB_LP_HE] = (&lcd_tcon_timing_he); /* STB/LP/HE: Not used */
            output.outctrl[VDC_LCD_TCONSIG_CPV_GCK] = NULL; /* CPV/GCK: Not used */
            output.outctrl[VDC_LCD_TCONSIG_POLA] = NULL; /* POLA: Not used */
            output.outctrl[VDC_LCD_TCONSIG_POLB] = NULL; /* POLB: Not used */
            output.outctrl[VDC_LCD_TCONSIG_DE] = (&lcd_tcon_timing_de); /* DE: Not used */

            /* Output parameter */
            output.tcon_half = q_cnf->tcon_half; /* TCON reference timing, 1/2fH timing */
            output.tcon_offset = q_cnf->tcon_ofset; /* TCON reference timing, offset Hsync signal timing */

            /* LCD TCON timing setting */
            output.outcnt_lcd_edge
                    = q_cnf->lcd_data_out_edge; /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
            output.out_endian_on = VDC_OFF; /* Bit endian change on/off control */
            output.out_swap_on = VDC_OFF; /* B/R signal swap on/off control */
            output.out_format = q_cnf->lcd_outformat; /* Output format select */
            output.out_frq_sel = VDC_LCD_PARALLEL_CLKFRQ_1; /* Clock frequency control */
            output.out_dir_sel = VDC_LCD_SERIAL_SCAN_FORWARD; /* Scan direction select */
            output.out_phase = VDC_LCD_SERIAL_CLKPHASE_0; /* Clock phase adjustment */
            output.bg_color = (uint32_t) 0x00000000u; /* Background color in 24-bit RGB color format */

            /* Display output */
            error = R_VDC_DisplayOutput (ch, &output); /* Green screen */
        }
    }

    return error;
} /* End of function R_RVAPI_DispControlVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_DispControlConfigVDC
 * @brief       VDC display out control processing
 *               Add the following parameters
 *                -Bit Endian Change On/Off Control
 *                -B/R Signal Swap On/Off Control
 *
 * @param[in]   ch              : Channel
 * @param[in]   res_vs_sel      : Vsync signal output select
 *                                - VDC_OFF: External input Vsync signal
 *                                - VDC_ON : Internally generated free-running Vsync signal
 * @param[in]   q_cnf           : LCD setting config (QE for VDC header)
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_DispControlConfigVDC
        (const vdc_channel_t ch, const vdc_onoff_t res_vs_sel,
        const qe_config_t * const q_cnf)
{
    vdc_error_t error;
    vdc_sync_ctrl_t sync_ctrl;

    /* Set NULL */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Sync signal control */
        sync_ctrl.res_vs_sel = res_vs_sel; /* Vsync signal output select (free-running Vsync on/off control) */

        /* Sync signal output and full-screen enable signal select */
        sync_ctrl.res_vs_in_sel = VDC_RES_VS_IN_SEL_SC0;
        sync_ctrl.res_fv = q_cnf->vtp; /* Free-running Vsync period setting */
        sync_ctrl.res_fh = q_cnf->htp; /* Hsync period setting */
        sync_ctrl.res_vsdly = (uint16_t) 0u; /* Vsync signal delay control */

        /* Full-screen enable control */
        sync_ctrl.res_f.vs = q_cnf->vs;
        sync_ctrl.res_f.vw = q_cnf->vdp;
        sync_ctrl.res_f.hs = q_cnf->hs;
        sync_ctrl.res_f.hw = q_cnf->hdp;

        /* Set NULL */
        sync_ctrl.vsync_cpmpe = NULL; /* Vsync signal compensation */

        /* Sync control */
        error = R_VDC_SyncControl (ch, &sync_ctrl);
    }

    /* pointer NULL check */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (VDC_OK == error)
        {
            /* full screen vale set */
            full_screen.vs = q_cnf->vs;
            full_screen.vw = q_cnf->vdp;
            full_screen.hs = q_cnf->hs;
            full_screen.hw = q_cnf->hdp;
        }
    }

    /* pointer NULL check */
    if (NULL == q_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (VDC_OK == error)
        {
            vdc_output_t output;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_vs;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_hs;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_ve;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_he;
            vdc_lcd_tcon_timing_t lcd_tcon_timing_de;

            lcd_tcon_timing_vs.tcon_hsvs = q_cnf->vps; /* Signal pulse start position */
            lcd_tcon_timing_vs.tcon_hwvw = q_cnf->vpw; /* Pulse width */
            lcd_tcon_timing_vs.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_vs.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_vs.tcon_inv = q_cnf->tcon_vsync_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_vs.tcon_pin = q_cnf->tcon_vsync; /* LCD TCON output pin select */
            lcd_tcon_timing_vs.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_hs.tcon_hsvs = q_cnf->hps; /* Signal pulse start position */
            lcd_tcon_timing_hs.tcon_hwvw = q_cnf->hpw; /* Pulse width */
            lcd_tcon_timing_hs.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_hs.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_hs.tcon_inv = q_cnf->tcon_hsync_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_hs.tcon_pin = q_cnf->tcon_hsync; /* LCD TCON output pin select */
            lcd_tcon_timing_hs.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_de.tcon_hsvs = 0u; /* Signal pulse start position */
            lcd_tcon_timing_de.tcon_hwvw = 0u; /* Pulse width */
            lcd_tcon_timing_de.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_de.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_de.tcon_inv = q_cnf->tcon_de_inv; /* Polarity inversion control of signal */
            lcd_tcon_timing_de.tcon_pin = q_cnf->tcon_de; /* LCD TCON output pin select */
            lcd_tcon_timing_de.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_ve.tcon_hsvs = q_cnf->vs; /* Signal pulse start position */
            lcd_tcon_timing_ve.tcon_hwvw = q_cnf->vdp; /* Pulse width */
            lcd_tcon_timing_ve.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_ve.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_ve.tcon_inv = VDC_SIG_POL_NOT_INVERTED; /* Polarity inversion control of signal */
            lcd_tcon_timing_ve.tcon_pin = VDC_LCD_TCON_PIN_NON; /* LCD TCON output pin select */
            lcd_tcon_timing_ve.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            lcd_tcon_timing_he.tcon_hsvs = q_cnf->hs; /* Signal pulse start position */
            lcd_tcon_timing_he.tcon_hwvw = q_cnf->hdp; /* Pulse width */
            lcd_tcon_timing_he.tcon_md = VDC_LCD_TCON_POLMD_NORMAL; /* POLA/POLB signal generation mode select */
            lcd_tcon_timing_he.tcon_hs_sel = VDC_LCD_TCON_REFSEL_HSYNC; /* Signal operating reference select */
            lcd_tcon_timing_he.tcon_inv = VDC_SIG_POL_NOT_INVERTED; /* Polarity inversion control of signal */
            lcd_tcon_timing_he.tcon_pin = VDC_LCD_TCON_PIN_NON; /* LCD TCON output pin select */
            lcd_tcon_timing_he.outcnt_edge = VDC_EDGE_RISING; /* Output phase control of signal */

            output.outctrl[VDC_LCD_TCONSIG_STVA_VS] = (&lcd_tcon_timing_vs); /* STVA/VS: Vsync */
            output.outctrl[VDC_LCD_TCONSIG_STVB_VE] = (&lcd_tcon_timing_ve); /* STVB/VE: Not used */
            output.outctrl[VDC_LCD_TCONSIG_STH_SP_HS] = (&lcd_tcon_timing_hs); /* STH/SP/HS: Hsync */
            output.outctrl[VDC_LCD_TCONSIG_STB_LP_HE] = (&lcd_tcon_timing_he); /* STB/LP/HE: Not used */
            output.outctrl[VDC_LCD_TCONSIG_CPV_GCK] = NULL; /* CPV/GCK: Not used */
            output.outctrl[VDC_LCD_TCONSIG_POLA] = NULL; /* POLA: Not used */
            output.outctrl[VDC_LCD_TCONSIG_POLB] = NULL; /* POLB: Not used */
            output.outctrl[VDC_LCD_TCONSIG_DE] = (&lcd_tcon_timing_de); /* DE: Not used */

            /* Output parameter */
            output.tcon_half = q_cnf->tcon_half; /* TCON reference timing, 1/2fH timing */
            output.tcon_offset = q_cnf->tcon_ofset; /* TCON reference timing, offset Hsync signal timing */

            /* LCD TCON timing setting */
            output.outcnt_lcd_edge
                    = q_cnf->lcd_data_out_edge; /* Output phase control of LCD_DATA23 to LCD_DATA0 pin */
            output.out_endian_on = q_cnf->out_endian_on; /* Bit endian change on/off control */
            output.out_swap_on = q_cnf->out_swap_on; /* B/R signal swap on/off control */
            output.out_format = q_cnf->lcd_outformat; /* Output format select */
            output.out_frq_sel = VDC_LCD_PARALLEL_CLKFRQ_1; /* Clock frequency control */
            output.out_dir_sel = VDC_LCD_SERIAL_SCAN_FORWARD; /* Scan direction select */
            output.out_phase = VDC_LCD_SERIAL_CLKPHASE_0; /* Clock phase adjustment */
            output.bg_color = (uint32_t) 0x00000000u; /* Background color in 24-bit RGB color format */

            /* Display output */
            error = R_VDC_DisplayOutput (ch, &output); /* Green screen */
        }
    }

    return error;
} /* End of function R_RVAPI_DispControlConfigVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphCreateSurfaceVDC
 * @brief       Graphics create surface processing
 * @param[in]   ch            : Channel
 * @param[in]   gr_disp_cnf   : Graphics surface read config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_GraphCreateSurfaceVDC
            (const vdc_channel_t ch, const gr_surface_disp_config_t * const gr_disp_cnf)
{
    vdc_error_t error;
    vdc_read_t read_cfg;

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        /* Read data parameter */
        read_cfg.gr_ln_off_dir = VDC_GR_LN_OFF_DIR_INC; /* Line offset address direction of the frame buffer */
        read_cfg.gr_flm_sel = VDC_GR_FLM_SEL_FLM_NUM; /* Selects a frame buffer address setting signal */
        read_cfg.gr_imr_flm_inv = VDC_OFF; /* Frame buffer number for distortion correction */
        read_cfg.gr_bst_md = VDC_BST_MD_32BYTE; /* Frame buffer burst transfer mode */
        read_cfg.gr_base = gr_disp_cnf->fb_buff; /* Frame buffer base address */
        read_cfg.gr_ln_off = gr_disp_cnf->fb_stride; /* Frame buffer line offset address */
        read_cfg.width_read_fb = NULL; /* Width of the image read from frame buffer */
        read_cfg.adj_sel = VDC_ON; /* Measures to decrease the influence by folding pixels/lines (on/off) */
        read_cfg.gr_format = gr_disp_cnf->read_format; /* Format of the frame buffer read signal */
        read_cfg.gr_ycc_swap = gr_disp_cnf->read_ycc_swap; /* Controls swapping of data read from buffer
         in the YCbCr422 format */
        read_cfg.gr_rdswa = gr_disp_cnf->read_swap; /* Frame buffer swap setting */

        /* Display area */
        /* vertical start position */
        read_cfg.gr_grc.vs = (uint16_t) (gr_disp_cnf->disp_area.vs_rel + full_screen.vs);

        /* vertical display size */
        read_cfg.gr_grc.vw = (uint16_t) gr_disp_cnf->disp_area.vw_rel;

        /* horizontal start position */
        read_cfg.gr_grc.hs = (uint16_t) (gr_disp_cnf->disp_area.hs_rel + full_screen.hs);

        /* horizontal display size */
        read_cfg.gr_grc.hw = (uint16_t) gr_disp_cnf->disp_area.hw_rel;

        /* Read data control */
        error = R_VDC_ReadDataControl (ch, gr_disp_cnf->layer_id, &read_cfg);
    }

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (((VDC_GR_FORMAT_CLUT8 == gr_disp_cnf->read_format) || (VDC_GR_FORMAT_CLUT4 == gr_disp_cnf->read_format))
                || (VDC_GR_FORMAT_CLUT1 == gr_disp_cnf->read_format))
        {
            vdc_clut_t clut_conf;

            if (VDC_GR_FORMAT_CLUT8 == gr_disp_cnf->read_format)
            {
                clut_conf.color_num = CLUT8_TABLE_NUM;

                /* pointer NULL check */
                if (NULL == gr_disp_cnf->clut_table)
                {
                    clut_conf.clut = &color_table256[0u];
                }
                else
                {
                    clut_conf.clut = gr_disp_cnf->clut_table;
                }
            }
            else if (VDC_GR_FORMAT_CLUT4 == gr_disp_cnf->read_format)
            {
                clut_conf.color_num = CLUT4_TABLE_NUM;

                /* pointer NULL check */
                if (NULL == gr_disp_cnf->clut_table)
                {
                    clut_conf.clut = &color_table16[0u];
                }
                else
                {
                    clut_conf.clut = gr_disp_cnf->clut_table;
                }
            }
            else
            {
                clut_conf.color_num = CLUT1_TABLE_NUM;

                /* pointer NULL check */
                if (NULL == gr_disp_cnf->clut_table)
                {
                    clut_conf.clut = &color_table2[0u];
                }
                else
                {
                    clut_conf.clut = gr_disp_cnf->clut_table;
                }
            }
            error = R_VDC_CLUT (ch, gr_disp_cnf->layer_id, &clut_conf);
        }
    }

    /* pointer NULL check */
    if (NULL == gr_disp_cnf)
    {
        error = VDC_ERR_PARAM_UNDEFINED;
    }
    else
    {
        if (VDC_OK == error)
        {
            vdc_start_t start;
            vdc_gr_disp_sel_t disp_mode;

            disp_mode = gr_disp_cnf->disp_mode;
            start.gr_disp_sel = &disp_mode;
            error = R_VDC_StartProcess (ch, gr_disp_cnf->layer_id, &start);
        }
    }
    return error;
} /* End of function R_RVAPI_GraphCreateSurfaceVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphChangeSurfaceVDC
 * @brief       Graphics change surface processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @param[in]   fb_buff       : Base address of the frame buffer
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_GraphChangeSurfaceVDC
        (const vdc_channel_t ch, const vdc_layer_id_t layer_id, void * const fb_buff)
{
    vdc_error_t error;
    vdc_read_chg_t read_chg;

    read_chg.gr_base = fb_buff;     /* Set buffer */
    read_chg.gr_disp_sel = NULL;    /* Set NULL */
    read_chg.gr_grc = NULL;         /* Set NULL */
    read_chg.width_read_fb = NULL;  /* Set NULL */
    error = R_VDC_ChangeReadProcess (ch, layer_id, &read_chg);

    return error;
} /* End of function R_RVAPI_GraphChangeSurfaceVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphChangeSurfaceConfigVDC
 * @brief       Graphics change surface processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @param[in]   fb_buff         : Base address of the frame buffer
 * @param[in]   gr_grc          : Disp area
 * @param[in]   width_read_fb   : VRAM Read size
 * @param[in]   gr_disp_sel     : disp sel
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t
        R_RVAPI_GraphChangeSurfaceConfigVDC
        (const vdc_channel_t ch, const vdc_layer_id_t layer_id,
        void * const fb_buff, vdc_period_rect_t * const gr_grc,
        vdc_width_read_fb_t * const width_read_fb, vdc_gr_disp_sel_t * const gr_disp_sel)
{
    vdc_error_t         error;
    vdc_period_rect_t   change_area;
    vdc_read_chg_t      read_chg;

    /* pointer NULL check */
    if( NULL != gr_grc )
    {
        /* Display area */
        change_area.hs = (uint16_t)(gr_grc->hs + full_screen.hs);  /* horizontal start position */
        change_area.hw = (uint16_t)gr_grc->hw;                     /* horizontal display size */
        change_area.vs = (uint16_t)(gr_grc->vs + full_screen.vs);  /* vertical start position */
        change_area.vw = gr_grc->vw;                               /* vertical display size */
        read_chg.gr_grc = &change_area;
    }
    else
    {
        /* Set NULL*/
        read_chg.gr_grc = NULL;
    }

    read_chg.gr_base = fb_buff;
    read_chg.gr_disp_sel = gr_disp_sel;
    read_chg.width_read_fb = width_read_fb;

    error = R_VDC_ChangeReadProcess (ch, layer_id, &read_chg);

    return error;
} /* End of function R_RVAPI_GraphChangeSurfaceConfigVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_GraphDestroySurfaceVDC
 * @brief       Graphics destroy surface processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t
        R_RVAPI_GraphDestroySurfaceVDC
            (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t error;

    /* Stop process */
    error = R_VDC_StopProcess (ch, layer_id);

    if (VDC_OK == error)
    {
        /* Release data control */
        error = R_VDC_ReleaseDataControl (ch, layer_id);
    }

    return error;
} /* End of function R_RVAPI_GraphDestroySurfaceVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_DispPortSettingVDC
 * @brief       Graphics display port setting
 * @param[in]   ch              : Channel
 * @param[in]   port_func       : Callback function for port setting
 * @retval      none
 ******************************************************************************/
void R_RVAPI_DispPortSettingVDC
        (const vdc_channel_t ch, void (* const port_func)(uint32_t))
{
    /* pointer NULL check */
    if( NULL != port_func )
    {
        /*Port Setting */
        port_func ((uint32_t) ch);
    }
} /* End of function R_RVAPI_DispPortSettingVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoControlVDC
 * @brief       Video input control
 * @param[in]   ch              : Channel
 * @param[in]   digital         : Digital Video input config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoControlVDC
            (const vdc_channel_t ch, const digital_in_t * const digital)
{
    vdc_error_t error;
    vdc_input_t input;
    vdc_ext_in_sig_t ext_sig_param; /* External input signal parameter */

    /* pointer NULL check */
    if (NULL != digital)
    {
        /* Input parameter */
        input.inp_sel = VDC_INPUT_SEL_EXT; /* Input select */
        input.inp_fh50 = (uint16_t) VSYNC_1_2_FH_TIMING; /* Vsync signal 1/2fH phase timing */
        input.inp_fh25 = (uint16_t) VSYNC_1_4_FH_TIMING; /* Vsync signal 1/4fH phase timing */
        input.dly = NULL; /* Sync signal delay adjustment */

        ext_sig_param.inp_format = digital->inp_format; /* External input format select */
        ext_sig_param.inp_pxd_edge  /* Clock edge select for capturing external input video image signals */
                        = digital->inp_pxd_edge;
        ext_sig_param.inp_vs_edge = VDC_EDGE_RISING; /* Clock edge select for capturing external input Vsync signals */
        ext_sig_param.inp_hs_edge = VDC_EDGE_RISING; /* Clock edge select for capturing external input Hsync signals */
        ext_sig_param.inp_endian_on = digital->inp_endian_on; /* External input bit endian change on/off control */
        ext_sig_param.inp_swap_on = digital->inp_swap_on; /* External input B/R signal swap on/off control */
        ext_sig_param.inp_vs_inv = digital->inp_vs_inv; /* External input Vsync signal DV_VSYNC inversion control */
        ext_sig_param.inp_hs_inv = digital->inp_hs_inv; /* External input Hsync signal DV_HSYNC inversion control */
        ext_sig_param.inp_h_edge_sel/* Reference select for external input BT.656 Hsync signal */
                        = digital->inp_h_edge_sel;
        ext_sig_param.inp_f525_625 = digital->inp_f525_625; /* Number of lines for BT.656 external input */
        ext_sig_param.inp_h_pos = digital->inp_h_pos; /* Y/Cb/Y/Cr data string start timing to Hsync reference */
        input.ext_sig = &ext_sig_param; /* External input signal */

        error = R_VDC_VideoInput (ch, &input);
    }
    else
    {
        error = VDC_ERR_RESOURCE_INPUT;
    }

    return error;
} /* End of function R_RVAPI_VideoControlVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoCreateSurfaceVDC
 * @brief       Video create surface processing
 * @param[in]   ch              : Channel
 * @param[in]   v_cnf           : Video surface write config
 * @param[in]   v_disp_cnf      : Video surface read config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoCreateSurfaceVDC
        (const vdc_channel_t ch, const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf)
{
    vdc_error_t error;
    vdc_write_t write_cfg;
    vdc_scalingdown_rot_t * pscldw_rot;
    vdc_read_t read_cfg;

    const vdc_gr_format_t
    gr_format_tmp[VDC_RES_MD_NUM] =
    { VDC_GR_FORMAT_YCBCR422, VDC_GR_FORMAT_RGB565, VDC_GR_FORMAT_RGB888, VDC_GR_FORMAT_YCBCR444 };

    /* pointer NULL check */
    if (NULL == v_cnf)
    {
        error = VDC_ERR_PARAM_NULL;
    }
    else
    {
        /* Scaling-down and rotation parameter */
        pscldw_rot = (&write_cfg.scalingdown_rot);

        /* Image area to be captured */
        /* Video capture is 1 delay */
        pscldw_rot->res.vs = (uint16_t) ((uint32_t) v_cnf->cap_area.vs - 1u);
        pscldw_rot->res.vw = v_cnf->cap_area.vw;
        pscldw_rot->res.hs = v_cnf->cap_area.hs;
        pscldw_rot->res.hw = v_cnf->cap_area.hw;

        /* Write data parameter */
        pscldw_rot->res_pfil_sel = VDC_ON; /* Prefilter mode select for brightness signals (on/off) */

        pscldw_rot->res_out_vw = v_cnf->write_fb_vw; /* scaling output vw */
        pscldw_rot->res_out_hw = v_cnf->write_fb_hw; /* scaling output hw */

        pscldw_rot->adj_sel = VDC_ON; /* lack of last-input line (on/off) */
        pscldw_rot->adj_sel = VDC_ON; /* lack of last-input line (on/off) */
        pscldw_rot->res_ds_wr_md = v_cnf->write_rot; /* Frame buffer writing mode */

        write_cfg.res_wrswa = v_cnf->write_swap; /* Frame buffer swap setting */
        write_cfg.res_md = v_cnf->write_format; /* Frame buffer video-signal writing format */
        write_cfg.res_bst_md = VDC_BST_MD_32BYTE; /* Transfer burst length for frame buffer */
        write_cfg.res_inter = v_cnf->res_inter; /* Field operating mode select */
        write_cfg.res_fs_rate = VDC_RES_FS_RATE_PER1; /* Writing rate */
        write_cfg.res_fld_sel = VDC_RES_FLD_SEL_TOP; /* Write field select */
        write_cfg.res_dth_on = VDC_ON; /* Dither correction on/off */
        write_cfg.base = v_cnf->fb_buff; /* Frame buffer base address */
        write_cfg.ln_off = v_cnf->fb_stride; /* Frame buffer line offset address [byte] */
        write_cfg.flm_num = (uint32_t) (v_cnf->fb_num - 1u); /* Number of frames of buffer (res_fb_num + 1) */
        write_cfg.flm_off = v_cnf->fb_offset; /* Write buffer fb offset[byte]   */
        write_cfg.btm_base = NULL; /* Frame buffer base address for bottom */

        /* Write data control */
        error = R_VDC_WriteDataControl (ch, v_cnf->layer_id, &write_cfg);

        if (VDC_OK == error)
        {
            vdc_start_t start;
            vdc_gr_disp_sel_t disp_mode;

            disp_mode = VDC_DISPSEL_LOWER;
            start.gr_disp_sel = &disp_mode;
            error = R_VDC_StartProcess (ch, v_cnf->layer_id, &start);
        }

        /* pointer NULL check */
        if (NULL != v_disp_cnf)
        {
            vdc_layer_id_t read_layer_id;

            if (VDC_LAYER_ID_0_WR == v_cnf->layer_id)
            {
                read_layer_id = VDC_LAYER_ID_0_RD;
            }
            else
            {
                read_layer_id = VDC_LAYER_ID_ALL;
                error = VDC_ERR_PARAM_LAYER_ID;
            }

            if (VDC_OK == error)
            {
                int32_t format;
                vdc_width_read_fb_t width_read_fb;

                /* Read data parameter */
                read_cfg.gr_ln_off_dir = VDC_GR_LN_OFF_DIR_INC; /* Line offset address direction of the frame buffer */
                read_cfg.gr_flm_sel = VDC_GR_FLM_SEL_POINTER_BUFF;
                read_cfg.gr_imr_flm_inv = VDC_OFF; /* Frame buffer number for distortion correction */
                read_cfg.gr_bst_md = VDC_BST_MD_32BYTE; /* Frame buffer burst transfer mode */
                read_cfg.gr_base = v_cnf->fb_buff; /* Frame buffer base address */
                read_cfg.gr_ln_off = v_cnf->fb_stride; /* Frame buffer line offset address */
                read_cfg.width_read_fb = NULL; /* Width of the image read from frame buffer */
                read_cfg.adj_sel = VDC_ON; /* Measures to decrease the influence by folding pixels/lines (ON/OFF) */

                if((v_cnf->write_rot == VDC_WR_MD_ROT_90DEG) || (v_cnf->write_rot == VDC_WR_MD_ROT_270DEG))
                {
                    width_read_fb.in_hw = v_cnf->write_fb_vw;
                    width_read_fb.in_vw = v_cnf->write_fb_hw;
                    read_cfg.width_read_fb = &width_read_fb;
                }

                /* Set write format */
                format = (int32_t) v_cnf->write_format;
                if (VDC_RES_MD_NUM > format)
                {
                    read_cfg.gr_format = gr_format_tmp[format]; /* Format of the frame buffer read signal */
                }
                else
                {
                    read_cfg.gr_format = VDC_GR_FORMAT_NUM;
                }
                read_cfg.gr_ycc_swap  /* Controls swapping of data read from buffer in the YCbCr422 format */
                            = v_disp_cnf->read_ycc_swap;
                read_cfg.gr_rdswa = v_disp_cnf->read_swap;

                /* Display area */
                read_cfg.gr_grc.vs /* vertical start position */
                    = (uint16_t)(v_disp_cnf->disp_area.vs_rel + full_screen.vs);
                read_cfg.gr_grc.vw /* vertical display size */
                    = v_disp_cnf->disp_area.vw_rel;
                read_cfg.gr_grc.hs /* horizontal start position */
                    = (uint16_t)(v_disp_cnf->disp_area.hs_rel + full_screen.hs);
                read_cfg.gr_grc.hw /* horizontal display size */
                    = v_disp_cnf->disp_area.hw_rel;

                /* Read data control */
                error = R_VDC_ReadDataControl (ch, read_layer_id, &read_cfg);

                if (VDC_OK == error)
                {
                    vdc_start_t start;
                    vdc_gr_disp_sel_t disp_mode;

                    disp_mode = VDC_DISPSEL_LOWER;
                    start.gr_disp_sel = &disp_mode;
                    error = R_VDC_StartProcess (ch, read_layer_id, &start);
                }
            }
        }
    }

    return error;
} /* End of function R_RVAPI_VideoCreateSurfaceVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoDestroySurfaceVDC
 * @brief       Video destroy surface processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoDestroySurfaceVDC
            (const vdc_channel_t ch, const vdc_layer_id_t layer_id)
{
    vdc_error_t error;

    if (VDC_LAYER_ID_0_WR == layer_id)
    {
        error = R_VDC_StopProcess (ch, layer_id);
        if (VDC_OK == error)
        {
            R_VDC_ReleaseDataControl (ch, layer_id);
            R_VDC_StopProcess (ch, VDC_LAYER_ID_0_RD);
            R_VDC_ReleaseDataControl (ch, VDC_LAYER_ID_0_RD);
        }
    }
    else
    {
        error = VDC_ERR_PARAM_LAYER_ID;
    }

    return error;
} /* End of function R_RVAPI_VideoDestroySurfaceVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoPortSettingVDC
 * @brief       Video port setting
 * @param[in]   ch              : Channel
 * @param[in]   port_func       : Callback function for port setting
 * @retval      none
 ******************************************************************************/
void R_RVAPI_VideoPortSettingVDC
        (const vdc_channel_t ch, void (* const port_func)(uint32_t))
{
    /* pointer NULL check */
    if( NULL != port_func )
    {
        /*Port Setting */
        port_func ((uint32_t) ch);
    }
} /* End of function R_RVAPI_VideoPortSettingVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_InterruptEnableVDC
 * @brief       Interrupt Enable
 * @param[in]   ch              : Channel
 * @param[in]   flag            : VDC interrupt type
 * @param[in]   line_num        : Line num
 * @param[in]   callback        : Callback function for VDC interruput
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_InterruptEnableVDC
        (const vdc_channel_t ch, const vdc_int_type_t flag, const uint16_t line_num,
        void (* const callback)(vdc_int_type_t int_type))
{
    vdc_error_t error;
    vdc_int_t param;

    param.type = flag;
    param.line_num = line_num;
    param.callback = callback;
    error = R_VDC_CallbackISR (ch, &param);

    return error;
} /* End of function R_RVAPI_InterruptEnableVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_InterruptDisableVDC
 * @brief       Interrupt disable
 * @param[in]   ch              : Channel
 * @param[in]   flag            : VDC interrupt type
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_InterruptDisableVDC
            (const vdc_channel_t ch, const vdc_int_type_t flag)
{
    vdc_error_t error;
    vdc_int_t param;

    param.type = flag;      /* Set VDC interrupt type */
    param.callback = NULL;  /* Set NULL */
    param.line_num = 0;     /* Set Line interrupt set */
    error = R_VDC_CallbackISR (ch, &param);

    return error;
} /* End of function R_RVAPI_InterruptDisableVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_AlphablendingRectVDC
 * @brief       Alpha Blending Rect processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @param[in]   alpha_onoff     : ON/OFF setting for alpha blending in a rectangular area
 * @param[in]   alpha_area      : Parameter for alpha blending in a rectangular area
 * @param[in]   alpha_value     : Alpha value0 (0 - 255)
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_AlphablendingRectVDC
        (const vdc_channel_t ch, const vdc_layer_id_t layer_id,
        const vdc_onoff_t alpha_onoff, const vdc_pd_disp_rect_t * const alpha_area, const uint8_t alpha_value)
{
    vdc_error_t error;
    vdc_alpha_blending_rect_t alpha_blending_rect;
    vdc_alpha_rect_t alpha_config;

    /* rect alpha config */
    alpha_config.gr_arc_coef = 0u;
    alpha_config.gr_arc_def = alpha_value;
    alpha_config.gr_arc_mul = VDC_OFF;
    alpha_config.gr_arc_rate = 0u;

    /* rect alpha area */
    alpha_blending_rect.scl_und_sel = NULL;
    alpha_blending_rect.gr_arc = alpha_area;
    alpha_blending_rect.alpha_rect = &alpha_config;

    error = R_VDC_AlphaBlendingRect (ch, layer_id, alpha_onoff, &alpha_blending_rect);
    return error;
} /* End of function R_RVAPI_AlphablendingRectVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_AlphablendingVDC
 * @brief       Alpha Blending Rect processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @param[in]   alpha_value     : Alpha value0 (0 - 255) ARGB1555 alpha value A=0
 * @param[in]   alpha_value     : Alpha value1 (0 - 255) ARGB1555 alpha value A=1
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_AlphablendingVDC
        (const vdc_channel_t ch, const vdc_layer_id_t layer_id,
        uint8_t alpha_value0, uint8_t alpha_value1 )
{
    vdc_error_t            error;
    vdc_alpha_argb1555_t   alpha_1bit;
    vdc_alpha_pixel_t      alpha_pixel;
    vdc_alpha_blending_t   param;

    alpha_1bit.gr_a0        = alpha_value0; /* Alpha signal when alpha is set to '0' */
    alpha_1bit.gr_a1        = alpha_value1; /* Alpha signal when alpha is set to '1' */
    param.alpha_1bit        = &alpha_1bit;
    alpha_pixel.gr_acalc_md = VDC_OFF;      /* Pre-multiplication processing at alpha blending */
    param.alpha_pixel       = &alpha_pixel;

    error = R_VDC_AlphaBlending (ch, layer_id, &param);
    return error;
} /* End of function R_RVAPI_AlphablendingVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_ChromakeyVDC
 * @brief       Chromakey processing
 * @param[in]   ch              : Channel
 * @param[in]   layer_id        : Layer ID
 * @param[in]   gr_ck_on        : Chroma-key ON/OFF setting
 * @param[in]   ck_color        : Replaced color
 * @param[in]   rep_alpha       : Replaced alpha (0 - 255)
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_ChromakeyVDC
        (const vdc_channel_t ch, const vdc_layer_id_t layer_id, const vdc_onoff_t gr_ck_on,
        const uint32_t ck_color, const uint8_t rep_alpha)
{
    vdc_error_t error;
    vdc_chromakey_t chromakey;

    /* Chromakey config */
    chromakey.ck_color = ck_color;
    chromakey.rep_color = ck_color;
    chromakey.rep_alpha = rep_alpha;

    error = R_VDC_Chromakey (ch, layer_id, gr_ck_on, &chromakey);

    return error;
} /* End of function R_RVAPI_ChromakeyVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_DispCalibrationVDC
 * @brief       Display out calibration initialization processing
 * @param[in]   ch              : Channel
 * @param[in]   route           : Correction circuit sequence control
 * @param[in]   bright          : Brightness (DC) adjustment parameter
 * @param[in]   contrast        : Contrast (gain) adjustment parameter
 * @param[in]   panel_dither    : Panel dithering parameter
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_DispCalibrationVDC
        (const vdc_channel_t ch, const vdc_calibr_route_t route,
        const vdc_calibr_bright_t * const bright, const vdc_calibr_contrast_t * const contrast,
        const vdc_calibr_dither_t * const panel_dither)
{
    vdc_error_t error;
    vdc_disp_calibration_t disp_calibration;

    disp_calibration.route = route;
    disp_calibration.panel_dither = panel_dither;
    disp_calibration.bright = bright;
    disp_calibration.contrast = contrast;

    error = R_VDC_DisplayCalibration (ch, &disp_calibration);

    return error;
} /* End of function R_RVAPI_DispCalibrationVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_DispGammaVDC
 * @brief       Display Gamma initialization processing
 * @param[in]   ch              : Channel
 * @param[in]   gam_on          : Gamma correction ON/OFF setting
 * @param[in]   gam_r_gain      : Gain adjustment of area 0 to 31 of R signal
 * @param[in]   gam_r_th        : Start threshold of area 1 to 31 of R signal
 * @param[in]   gam_g_gain      : Gain adjustment of area 0 to 31 of G signal
 * @param[in]   gam_g_th        : Start threshold of area 1 to 31 of G signal
 * @param[in]   gam_b_gain      : Gain adjustment of area 0 to 31 of B signal
 * @param[in]   gam_b_th        : Start threshold of area 1 to 31 of B signal
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_DispGammaVDC
        (const vdc_channel_t ch, const vdc_onoff_t gam_on,
        const uint16_t * const gam_r_gain, const uint8_t * const gam_r_th, const uint16_t * const gam_g_gain,
        const uint8_t * const gam_g_th, const uint16_t * const gam_b_gain, const uint8_t * const gam_b_th)
{
    vdc_error_t error;
    vdc_gamma_correction_t param;

    param.gam_b_gain = gam_b_gain;
    param.gam_b_th = gam_b_th;
    param.gam_g_gain = gam_g_gain;
    param.gam_g_th = gam_g_th;
    param.gam_r_gain = gam_r_gain;
    param.gam_r_th = gam_r_th;

    error = R_VDC_GammaCorrection (ch, gam_on, &param);

    return error;
} /* End of function R_RVAPI_DispGammaVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoCalibrationVDC
 * @brief       Video Contrast and Brightness initialization processing
 * @param[in]   ch                   : Channel
 * @param[in]   color_matrix         : Color matrix setup parameter
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoCalibrationVDC
            (const vdc_channel_t ch, const vdc_color_matrix_t * const color_matrix)
{
    vdc_error_t error;

    error = R_VDC_ImageColorMatrix (ch, color_matrix);

    return error;
} /* End of function R_RVAPI_VideoCalibrationVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoSharpnessLtiVDC
 * @brief       Video Sharpness and LTI initialization processing
 * @param[in]   ch              : Channel
 * @param[in]   imgimprv_id     : Image quality improver ID
 * @param[in]   shp_h_on        : Sharpness ON/OFF setting
 * @param[in]   sharp_param     : Sharpness setup parameter
 * @param[in]   lti_h_on        : LTI ON/OFF setting
 * @param[in]   lti_param       : LTI setup parameter
 * @param[in]   enh_area        : Enhancer-enabled area setup parameter
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoSharpnessLtiVDC
        (const vdc_channel_t ch, const vdc_imgimprv_id_t imgimprv_id,
        const vdc_onoff_t shp_h_on, const vdc_enhance_sharp_t * const sharp_param, const vdc_onoff_t lti_h_on,
        const vdc_enhance_lti_t * const lti_param, const vdc_period_rect_t * const enh_area)
{
    vdc_error_t error;

    error = R_VDC_ImageEnhancement (ch, imgimprv_id, shp_h_on, sharp_param, lti_h_on, lti_param, enh_area);

    return error;
} /* End of function R_RVAPI_VideoSharpnessLtiVDC() */

/**************************************************************************//**
 * Function Name : R_RVAPI_VideoCreateSurfaceIMRL2
 * @brief       Video create surface processing
 * @param[in]   ch              : Channel
 * @param[in]   v_cnf           : Video surface write config
 * @param[in]   v_disp_cnf      : Video surface read config
 * @retval      VDC driver error code
 ******************************************************************************/
vdc_error_t R_RVAPI_VideoCreateSurfaceIMRL2
        (const vdc_channel_t ch, const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf)
{
    vdc_error_t error;
    vdc_write_t write_cfg;
    vdc_scalingdown_rot_t * pscldw_rot;
    vdc_read_t read_cfg;
    vdc_width_read_fb_t width_read_fb;

    /* pointer NULL check */
    if (NULL == v_cnf)
    {
        error = VDC_ERR_PARAM_NULL;
    }
    else
    {
        /* Scaling-down and rotation parameter */
        pscldw_rot = (&write_cfg.scalingdown_rot);

        /* Image area to be captured */
        /* Video capture is 1 delay */
        pscldw_rot->res.vs = (uint16_t) ((uint32_t) v_cnf->cap_area.vs - 1u);
        pscldw_rot->res.vw = v_cnf->cap_area.vw + 4u;
        pscldw_rot->res.hs = v_cnf->cap_area.hs;
        pscldw_rot->res.hw = v_cnf->cap_area.hw;

        /* Write data parameter */
        pscldw_rot->res_pfil_sel = VDC_ON; /* Prefilter mode select for brightness signals (on/off) */

        pscldw_rot->res_out_vw = v_cnf->write_fb_vw + 4u; /* scaling output vw */
        pscldw_rot->res_out_hw = v_cnf->write_fb_hw; /* scaling output hw */

        pscldw_rot->adj_sel = VDC_ON; /* lack of last-input line (on/off) */
        pscldw_rot->res_ds_wr_md = VDC_WR_MD_NORMAL; /* Frame buffer writing mode */

        write_cfg.res_wrswa = VDC_WR_RD_WRSWA_NON; /* Frame buffer swap setting */
        write_cfg.res_md = VDC_RES_MD_YCBCR422; /* Frame buffer video-signal writing format */
        write_cfg.res_bst_md = VDC_BST_MD_32BYTE; /* Transfer burst length for frame buffer */
        write_cfg.res_inter = v_cnf->res_inter; /* Field operating mode select */
        write_cfg.res_fs_rate = VDC_RES_FS_RATE_PER1; /* Writing rate */
        write_cfg.res_fld_sel = VDC_RES_FLD_SEL_TOP; /* Write field select */
        write_cfg.res_dth_on = VDC_ON; /* Dither correction on/off */
        write_cfg.base = v_cnf->fb_buff; /* Frame buffer base address */
        write_cfg.ln_off = v_cnf->fb_stride; /* Frame buffer line offset address [byte] */
        write_cfg.flm_num = (uint32_t) (v_cnf->fb_num - 1u);
        write_cfg.flm_off = v_cnf->fb_offset; /* Write buffer fb offset[byte]   */
        write_cfg.btm_base = NULL; /* Frame buffer base address for bottom */

        /* Write data control */
        error = R_VDC_WriteDataControl (ch, v_cnf->layer_id, &write_cfg);

        /* pointer NULL check */
        if (NULL != v_disp_cnf)
        {
            vdc_layer_id_t read_layer_id;

            if (VDC_LAYER_ID_0_WR == v_cnf->layer_id)
            {
                read_layer_id = VDC_LAYER_ID_0_RD;
            }
            else
            {
                read_layer_id = VDC_LAYER_ID_ALL;
                error = VDC_ERR_PARAM_LAYER_ID;
            }

            if (VDC_OK == error)
            {
                /* Read data parameter */
                read_cfg.gr_ln_off_dir = VDC_GR_LN_OFF_DIR_INC; /* Line offset address direction of the frame buffer */
                read_cfg.gr_flm_sel = VDC_GR_FLM_SEL_DISTORTION;
                read_cfg.gr_imr_flm_inv = VDC_ON; /* Frame buffer number for distortion correction */
                read_cfg.gr_bst_md = VDC_BST_MD_32BYTE; /* Frame buffer burst transfer mode */
                read_cfg.gr_base = v_cnf->fb_buff; /* Frame buffer base address */
                read_cfg.gr_ln_off = v_cnf->fb_stride; /* Frame buffer line offset address */

                width_read_fb.in_hw = v_disp_cnf->disp_area.hw_rel;
                width_read_fb.in_vw = v_disp_cnf->disp_area.vw_rel;
                read_cfg.width_read_fb = &width_read_fb; /* Width of the image read from frame buffer */
                read_cfg.adj_sel = VDC_ON; /* Measures to decrease the influence by folding pixels/lines (ON/OFF) */

                read_cfg.gr_format = VDC_GR_FORMAT_YCBCR422; /* Format of the frame buffer read signal */

                read_cfg.gr_ycc_swap /* Controls swapping of data read from buffer in the YCbCr422 format */
                    = v_disp_cnf->read_ycc_swap;
                read_cfg.gr_rdswa = v_disp_cnf->read_swap;

                /* Display area */
                read_cfg.gr_grc.vs = v_disp_cnf->disp_area.vs_rel + full_screen.vs;
                read_cfg.gr_grc.vw = v_disp_cnf->disp_area.vw_rel;
                read_cfg.gr_grc.hs = v_disp_cnf->disp_area.hs_rel + full_screen.hs;
                read_cfg.gr_grc.hw = v_disp_cnf->disp_area.hw_rel;

                /* Read data control */
                error = R_VDC_ReadDataControl (ch, read_layer_id, &read_cfg);

                if (VDC_OK == error)
                {
                    vdc_start_t start;
                    vdc_gr_disp_sel_t disp_mode;

                    disp_mode = VDC_DISPSEL_LOWER;
                    start.gr_disp_sel = &disp_mode;
                    error = R_VDC_StartProcess (ch, read_layer_id, &start);
                }
            }
        }
    }

    return error;
} /* End of function R_RVAPI_VideoCreateSurfaceIMRL2() */
