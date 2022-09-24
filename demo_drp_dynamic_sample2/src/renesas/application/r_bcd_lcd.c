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
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <string.h>
#include "r_typedefs.h"

#include "r_rvapi_vdc.h"
#include "lcd_panel.h"
#include "r_image_config.h"

#include "r_bcd_lcd.h"
#include "draw.h"
#include "fontdata.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define     LAYER0_DISP_AREA_HS         (0u)
#define     LAYER0_DISP_AREA_VS         (0u)
#define     LAYER0_DISP_AREA_HW         (R_BCD_LCD_WIDTH)
#define     LAYER0_DISP_AREA_VW         (R_BCD_LCD_HEIGHT)

#define     VIDEO_BUFFER_STRIDE         ((LAYER0_DISP_AREA_HW + (R_BCD_LCD_STRIDE - 1)) & ~(R_BCD_LCD_STRIDE - 1))
#define     VIDEO_BUFFER_HEIGHT         (LAYER0_DISP_AREA_VW)
#define     VIDEO_BUFFER_NUM            (2u)

#define     LAYER2_DISP_AREA_HS         (0u)
#define     LAYER2_DISP_AREA_VS         (0u)
#define     LAYER2_DISP_AREA_HW         (R_BCD_LCD_WIDTH)
#define     LAYER2_DISP_AREA_VW         (R_BCD_LCD_HEIGHT)

#define     GRAPHICS2_BUFFER_STRIDE     (((LAYER2_DISP_AREA_HW / 2) + (R_BCD_LCD_STRIDE - 1)) & ~(R_BCD_LCD_STRIDE - 1)) 
#define     GRAPHICS2_BUFFER_HEIGHT     (LAYER2_DISP_AREA_VW)
#define     GRAPHICS2_BUFFER_NUM        (2u)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void VDC_LcdPortSetting(uint32_t param);
extern void DrawLine( vdc_layer_id_t layer_id, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color );

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t video_buffer[VIDEO_BUFFER_NUM][(VIDEO_BUFFER_STRIDE * VIDEO_BUFFER_HEIGHT)] __attribute__ ((section("Video_RAM")));
static uint8_t graphics2_buffer[GRAPHICS2_BUFFER_NUM][(GRAPHICS2_BUFFER_STRIDE * GRAPHICS2_BUFFER_HEIGHT)] __attribute__ ((section("Graphics_RAM")));
static volatile int32_t vsync_count = 0;

static uint32_t Req_Layer1_Swap = 0;
static uint32_t Req_Layer2_Swap = 0;
static uint32_t Layer1_buffer_id = 0;
static uint32_t layer2_buffer_id = 0;

static void IntCallbackFunc (vdc_int_type_t int_type);
vdc_error_t r_display_init(const vdc_channel_t vdc_ch);
vdc_error_t r_image_quality_adjustment(vdc_channel_t vdc_ch);
vdc_error_t r_video_quality_adjustment(vdc_channel_t vdc_ch);

/******************************************************************************
* Function Name: R_BCD_LcdGetVramAddress
* Description  : Acquire the address of the video buffer which is not currently being displayed
* Arguments    : -
* Return Value : Video buffer address
******************************************************************************/
uint8_t *R_BCD_LcdGetVramAddress(void)
{
    uint8_t *paddr; 

    if (Req_Layer1_Swap != 0)
    {
        paddr = &video_buffer[1-Layer1_buffer_id][0];
    }
    else
    {
        paddr = &video_buffer[Layer1_buffer_id][0];
    }

    return paddr;
}   /* End of function R_BCD_LcdGetVramAddress() */

/******************************************************************************
* Function Name: R_BCD_LcdGetOLVramAddress
* Description  : Acquire the address of the Overlap buffer which is not currently being displayed
* Arguments    : -
* Return Value : Overlap buffer address
******************************************************************************/
uint8_t *R_BCD_LcdGetOLVramAddress(void) 
{
    uint8_t *paddr; 

    if (Req_Layer2_Swap != 0)
    {
        paddr = &graphics2_buffer[1-layer2_buffer_id][0];
    }
    else
    {
        paddr =  &graphics2_buffer[layer2_buffer_id][0];
    }

    return paddr;
}   /* End of function R_BCD_LcdGetOLVramAddress() */

/******************************************************************************
* Function Name: IntCallbackFunc
* Description  : VDC6 Interrupt callback function
* Arguments    : int_type
*              :   VDC6 interrupt type
* Return Value : -
******************************************************************************/
static void IntCallbackFunc(vdc_int_type_t int_type)
{
    if( int_type == VDC_INT_TYPE_VLINE )
    {
        /* Update the LCD output.*/
        R_BCD_LcdVSync();
    }

    if( int_type == VDC_INT_TYPE_S0_LO_VSYNC )
    {
        if (vsync_count > 0)
        {
            vsync_count--;
        }
    }

    return;
}   /* End of function IntCallbackFunc() */

/******************************************************************************
* Function Name: R_BCD_LcdInit
* Description  : LCD initialization processing
* Arguments    : -
* Return Value : Always returns 0
******************************************************************************/
int32_t R_BCD_LcdInit(void)
{
    vdc_error_t error;
    vdc_channel_t vdc_ch = VDC_CHANNEL_0;
    uint32_t table_no;
    uint32_t clut[256];

    /***********************************************************************/
    /* display init (VDC6 output setting) */
    /***********************************************************************/
    error = r_display_init (vdc_ch);

    /***********************************************************************/
    /* Interrupt init */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        uint16_t line_num;

        line_num = LCD_CH0_DISP_VS + R_BCD_LCD_HEIGHT;
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_VLINE, line_num, IntCallbackFunc);
    }

    if (error == VDC_OK)
    {
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_S0_LO_VSYNC, 0, IntCallbackFunc);
    }

    /***********************************************************************/
    /* Graphic Layer 0 VDC6_GR_FORMAT_CLUT8 */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        gr_surface_disp_config_t gr_disp_cnf;

        for(table_no = 0; table_no < 256 ; table_no ++ )
        {
            clut[table_no] = 0xFF000000 | (table_no << 16 ) | (table_no << 8) | table_no;
        }

        gr_disp_cnf.layer_id  = VDC_LAYER_ID_0_RD;         /* Layer ID                        */
        gr_disp_cnf.fb_buff   = (void *)&video_buffer[0];   /* Frame buffer address            */
        gr_disp_cnf.fb_stride = VIDEO_BUFFER_STRIDE;        /* Frame buffer stride             */

        /* Display Area               */
        gr_disp_cnf.disp_area.hs_rel = LAYER0_DISP_AREA_HS;
        gr_disp_cnf.disp_area.hw_rel = LAYER0_DISP_AREA_HW;
        gr_disp_cnf.disp_area.vs_rel = LAYER0_DISP_AREA_VS;
        gr_disp_cnf.disp_area.vw_rel = LAYER0_DISP_AREA_VW;

        gr_disp_cnf.read_format   = VDC_GR_FORMAT_CLUT8;           /* Read Format                */
        gr_disp_cnf.read_ycc_swap = VDC_GR_YCCSWAP_CBY0CRY1;       /* Read Swap for YCbCr422     */
        gr_disp_cnf.read_swap     = VDC_WR_RD_WRSWA_32_16_8BIT;    /* Read Swap 8bit/16bit/32bit */

        gr_disp_cnf.clut_table = clut;  /* Setting if Read Format is CLUT. */

        gr_disp_cnf.disp_mode = VDC_DISPSEL_CURRENT;   /* Display mode select        */

        error = R_RVAPI_GraphCreateSurfaceVDC(vdc_ch, &gr_disp_cnf);
    }

    /***********************************************************************/
    /* Graphic Layer 2 VDC6_GR_FORMAT_CLUT4 */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        gr_surface_disp_config_t gr_disp_cnf;

        clut[BLACK]       = BLACK_DATA;
        clut[BLUE]        = BLUE_DATA;
        clut[GREEN]       = GREEN_DATA;
        clut[RED]         = RED_DATA;
        clut[WHITE]       = WHITE_DATA;
        clut[CYAN]        = CYAN_DATA;
        clut[YELLOW]      = YELLOW_DATA;
        clut[MAGENTA]     = MAGENTA_DATA;
        clut[NAVY]        = NAVY_DATA;
        clut[DARKGREEN]   = DARKGREEN_DATA;
        clut[DEEPSKYBLUE] = DEEPSKYBLUE_DATA;
        clut[PURPLE]      = PURPLE_DATA;
        clut[GRAY]        = GRAY_DATA;
        clut[BROWN]       = BROWN_DATA;
        clut[GOLD]        = GOLD_DATA;
        clut[TRANSPARENT] = TRANSPARENT_DATA;

        gr_disp_cnf.layer_id  = VDC_LAYER_ID_2_RD;             /* Layer ID                        */
        gr_disp_cnf.fb_buff   = (void *)&graphics2_buffer[0];   /* Frame buffer address            */
        gr_disp_cnf.fb_stride = GRAPHICS2_BUFFER_STRIDE;        /* Frame buffer stride             */

        /* Display Area               */
        gr_disp_cnf.disp_area.hs_rel = LAYER2_DISP_AREA_HS;
        gr_disp_cnf.disp_area.hw_rel = LAYER2_DISP_AREA_HW;
        gr_disp_cnf.disp_area.vs_rel = LAYER2_DISP_AREA_VS;
        gr_disp_cnf.disp_area.vw_rel = LAYER2_DISP_AREA_VW;

        gr_disp_cnf.read_format   = VDC_GR_FORMAT_CLUT4;           /* Read Format                */
        gr_disp_cnf.read_ycc_swap = VDC_GR_YCCSWAP_CBY0CRY1;       /* Read Swap for YCbCr422     */
        gr_disp_cnf.read_swap     = VDC_WR_RD_WRSWA_32_16_8BIT;    /* Read Swap 8bit/16bit/32bit */

        gr_disp_cnf.clut_table = clut;  /* Setting if Read Format is CLUT. */

        gr_disp_cnf.disp_mode = VDC_DISPSEL_BLEND;   /* Display mode select */

        error = R_RVAPI_GraphCreateSurfaceVDC(vdc_ch, &gr_disp_cnf);
    }

    {
        draw_buffer_info            info;

        /* gra draw init */
        info.buff_addr1        = graphics2_buffer[0];
        info.buff_addr2        = graphics2_buffer[1];
        info.buff_stride       = GRAPHICS2_BUFFER_STRIDE;
        info.window_width      = LAYER2_DISP_AREA_HW;
        info.window_height     = LAYER2_DISP_AREA_VW;
        DrawInit( VDC_LAYER_ID_2_RD, &info );

        /* buffer claer */
        DrawRectFill( VDC_LAYER_ID_2_RD, 0 ,0, R_BCD_LCD_WIDTH, R_BCD_LCD_HEIGHT, TRANSPARENT );
    }

    /* image and video quality adjustment */
    if (VDC_OK == error)
    {
        r_image_quality_adjustment(vdc_ch);
#if 0 
        r_video_quality_adjustment(vdc_ch);  /* It is possible to use it when inputting video with VDC.
                               In this sample, it can not be used because it is video input with MIPI. */
#endif
    }

    /* Display On */
    if (VDC_OK == error)
    {
        R_RVAPI_DispPortSettingVDC(vdc_ch, &VDC_LcdPortSetting);
    }

    return 0;
}   /* End of function R_BCD_LcdInit() */

/******************************************************************************
* Function Name: r_display_init
* Description  : VDC6 output setting
* Arguments    : vdc_ch
*              :   Channel
* Return Value : 0
*              :   success
*              : other
*              :   fail (vdc_error_t)
******************************************************************************/
vdc_error_t r_display_init(const vdc_channel_t vdc_ch)
{
    vdc_error_t error;

    /* Display clock setting */
    {
        clock_config_t clock_config;
        vdc_lvds_t lvds;

        clock_config.panel_clk = LCD_CH0_PANEL_CLK;
        clock_config.panel_clk_div = LCD_CH0_PANEL_CLK_DIV;

        lvds.lvds_vdc_sel = vdc_ch;                 /* A channel in VDC6 whose data is to be output through the LVDS */
        lvds.lvds_in_clk_sel = LVDS_PLL_INPUT_CLK;  /* The clock input to frequency divider 1 */
        lvds.lvds_idiv_set = LVDS_PLL_NIDV;         /* Not use */
        lvds.lvds_odiv_set = LVDS_PLL_NODIV;        /* The frequency dividing value (NODIV) for frequency divider 2 */
        lvds.lvdspll_fd = LVDS_PLL_NFD;             /* The frequency dividing value (NFD) for the feedback frequency */
        lvds.lvdspll_rd = LVDS_PLL_NRD;             /* The frequency dividing value (NRD) for the input frequency */
        lvds.lvdspll_od = LVDS_PLL_NOD;             /* Not use */
        clock_config.lvds = &lvds;

        error = R_RVAPI_InitializeVDC (vdc_ch, &clock_config);
    }

    /* Display setting */
    if (VDC_OK == error)
    {
        qe_config_t q_cnf;

        q_cnf.vps = LCD_CH0_S_VSYNC;                    /* Vertical Pulse start position */
        q_cnf.vpw = LCD_CH0_W_VSYNC;                    /* Vertical Pulse Width(VPW) */
        q_cnf.vs = LCD_CH0_DISP_VS;                     /* LCD display area vertical start position */
        q_cnf.vdp = LCD_CH0_DISP_VW;                    /* Vertical Display Period(VDP) */
        q_cnf.hps = LCD_CH0_S_HSYNC;                    /* Horizontal Pulse start position */
        q_cnf.hpw = LCD_CH0_W_HSYNC;                    /* Horizontal Pulse Width(HPW) */
        q_cnf.hs = LCD_CH0_DISP_HS;                     /* LCD display area horizontal start position */
        q_cnf.hdp = LCD_CH0_DISP_HW;                    /* Horizontal Display Period(HDP */
        q_cnf.vtp = LCD_CH0_SIG_FV;                     /* Vertical Total Period(VTP) */
        q_cnf.htp = LCD_CH0_SIG_FH;                     /* Horizontal Total Period(HTP) */
        q_cnf.tcon_vsync = LCD_CH0_TCON_PIN_VSYNC;      /* Vsync_TCON_select */
        q_cnf.tcon_hsync = LCD_CH0_TCON_PIN_HSYNC;      /* Hsync_TCON_select */
        q_cnf.tcon_de = LCD_CH0_TCON_PIN_DE;            /* DE_TCON_select */
        q_cnf.tcon_vsync_inv = LCD_CH0_TOCN_POL_VSYNC;  /* Vsync_POL */
        q_cnf.tcon_hsync_inv = LCD_CH0_TOCN_POL_HSYNC;  /* Hsync_POL */
        q_cnf.tcon_de_inv = LCD_CH0_TOCN_POL_DE;        /* DE_POL */
        q_cnf.tcon_half = LCD_CH0_TCON_HALF;            /* TCON reference timing, 1/2fH timing(HTP) */
        q_cnf.tcon_ofset = LCD_CH0_TCON_OFFSET;         /* TCON reference timing, offset Hsync signal timing */
        q_cnf.lcd_data_out_edge = LCD_CH0_OUT_EDGE;     /* LCD_DATA_OUT_EDGE */
        q_cnf.lcd_outformat = LCD_CH0_OUT_FORMAT;       /* LCD output format select */
        q_cnf.out_endian_on = VDC_ON;
        q_cnf.out_swap_on = VDC_ON;
        error = R_RVAPI_DispControlConfigVDC (vdc_ch, VDC_ON, &q_cnf);
     }

    return error;
}   /* End of function r_display_init() */

/******************************************************************************
* Function Name: r_image_quality_adjustment
* Description  : Image Quality Adjustment
* Arguments    : vdc_ch
*              :   Channel
* Return Value : 0
*              :   success
*              : other
*              :   fail (vdc_error_t)
******************************************************************************/
vdc_error_t r_image_quality_adjustment(vdc_channel_t vdc_ch)
{
    vdc_error_t error;

    /***********************************************************************/
    /* Display out calibration */
    /***********************************************************************/
    {
        vdc_calibr_bright_t bright;
        vdc_calibr_contrast_t contrast;
        vdc_calibr_dither_t dither;

        /* Bright */
        bright.pbrt_g = IMGC_BRIGHT_OUTCTL_OFFSET_G;
        bright.pbrt_b = IMGC_BRIGHT_OUTCTL_OFFSET_B;
        bright.pbrt_r = IMGC_BRIGHT_OUTCTL_OFFSET_R;

        /* Contrast */
        contrast.cont_g = IMGC_CONTRAST_OUTCTL_GAIN_G;
        contrast.cont_b = IMGC_CONTRAST_OUTCTL_GAIN_B;
        contrast.cont_r = IMGC_CONTRAST_OUTCTL_GAIN_R;

        /* Dither settings */
        dither.pdth_sel = IMGC_DITHER_MODE;
        dither.pdth_pa = IMGC_DITHER_2X2_PA;
        dither.pdth_pb = IMGC_DITHER_2X2_PB;
        dither.pdth_pc = IMGC_DITHER_2X2_PC;
        dither.pdth_pd = IMGC_DITHER_2X2_PD;
        error = R_RVAPI_DispCalibrationVDC (vdc_ch, IMGC_OUTCTL_CALIB_ROUTE, &bright, &contrast, &dither);
    }

    /***********************************************************************/
    /*  Display Gamma */
    /***********************************************************************/
    if (VDC_OK == error)
    {
        static const uint16_t gam_g_gain[VDC_GAM_GAIN_ADJ_NUM] =
        { IMGC_GAMMA_G_GAIN_00, IMGC_GAMMA_G_GAIN_01, IMGC_GAMMA_G_GAIN_02, IMGC_GAMMA_G_GAIN_03,
            IMGC_GAMMA_G_GAIN_04, IMGC_GAMMA_G_GAIN_05, IMGC_GAMMA_G_GAIN_06, IMGC_GAMMA_G_GAIN_07,
            IMGC_GAMMA_G_GAIN_08, IMGC_GAMMA_G_GAIN_09, IMGC_GAMMA_G_GAIN_10, IMGC_GAMMA_G_GAIN_11,
            IMGC_GAMMA_G_GAIN_12, IMGC_GAMMA_G_GAIN_13, IMGC_GAMMA_G_GAIN_14, IMGC_GAMMA_G_GAIN_15,
            IMGC_GAMMA_G_GAIN_16, IMGC_GAMMA_G_GAIN_17, IMGC_GAMMA_G_GAIN_18, IMGC_GAMMA_G_GAIN_19,
            IMGC_GAMMA_G_GAIN_20, IMGC_GAMMA_G_GAIN_21, IMGC_GAMMA_G_GAIN_22, IMGC_GAMMA_G_GAIN_23,
            IMGC_GAMMA_G_GAIN_24, IMGC_GAMMA_G_GAIN_25, IMGC_GAMMA_G_GAIN_26, IMGC_GAMMA_G_GAIN_27,
            IMGC_GAMMA_G_GAIN_28, IMGC_GAMMA_G_GAIN_29, IMGC_GAMMA_G_GAIN_30, IMGC_GAMMA_G_GAIN_31 };
        static const uint8_t gam_g_th[VDC_GAM_START_TH_NUM] =
        { IMGC_GAMMA_G_TH_01, IMGC_GAMMA_G_TH_02, IMGC_GAMMA_G_TH_03, IMGC_GAMMA_G_TH_04, IMGC_GAMMA_G_TH_05,
            IMGC_GAMMA_G_TH_06, IMGC_GAMMA_G_TH_07, IMGC_GAMMA_G_TH_08, IMGC_GAMMA_G_TH_09,
            IMGC_GAMMA_G_TH_10, IMGC_GAMMA_G_TH_11, IMGC_GAMMA_G_TH_12, IMGC_GAMMA_G_TH_13,
            IMGC_GAMMA_G_TH_14, IMGC_GAMMA_G_TH_15, IMGC_GAMMA_G_TH_16, IMGC_GAMMA_G_TH_17,
            IMGC_GAMMA_G_TH_18, IMGC_GAMMA_G_TH_19, IMGC_GAMMA_G_TH_20, IMGC_GAMMA_G_TH_21,
            IMGC_GAMMA_G_TH_22, IMGC_GAMMA_G_TH_23, IMGC_GAMMA_G_TH_24, IMGC_GAMMA_G_TH_25,
            IMGC_GAMMA_G_TH_26, IMGC_GAMMA_G_TH_27, IMGC_GAMMA_G_TH_28, IMGC_GAMMA_G_TH_29,
            IMGC_GAMMA_G_TH_30, IMGC_GAMMA_G_TH_31 };

        static const uint16_t gam_b_gain[VDC_GAM_GAIN_ADJ_NUM] =
        { IMGC_GAMMA_B_GAIN_00, IMGC_GAMMA_B_GAIN_01, IMGC_GAMMA_B_GAIN_02, IMGC_GAMMA_B_GAIN_03, IMGC_GAMMA_B_GAIN_04,
            IMGC_GAMMA_B_GAIN_05, IMGC_GAMMA_B_GAIN_06, IMGC_GAMMA_B_GAIN_07, IMGC_GAMMA_B_GAIN_08,
            IMGC_GAMMA_B_GAIN_09, IMGC_GAMMA_B_GAIN_10, IMGC_GAMMA_B_GAIN_11, IMGC_GAMMA_B_GAIN_12,
            IMGC_GAMMA_B_GAIN_13, IMGC_GAMMA_B_GAIN_14, IMGC_GAMMA_B_GAIN_15, IMGC_GAMMA_B_GAIN_16,
            IMGC_GAMMA_B_GAIN_17, IMGC_GAMMA_B_GAIN_18, IMGC_GAMMA_B_GAIN_19, IMGC_GAMMA_B_GAIN_20,
            IMGC_GAMMA_B_GAIN_21, IMGC_GAMMA_B_GAIN_22, IMGC_GAMMA_B_GAIN_23, IMGC_GAMMA_B_GAIN_24,
            IMGC_GAMMA_B_GAIN_25, IMGC_GAMMA_B_GAIN_26, IMGC_GAMMA_B_GAIN_27, IMGC_GAMMA_B_GAIN_28,
            IMGC_GAMMA_B_GAIN_29, IMGC_GAMMA_B_GAIN_30, IMGC_GAMMA_B_GAIN_31 };
        static const uint8_t gam_b_th[VDC_GAM_START_TH_NUM] =
        { IMGC_GAMMA_B_TH_01, IMGC_GAMMA_B_TH_02, IMGC_GAMMA_B_TH_03, IMGC_GAMMA_B_TH_04, IMGC_GAMMA_B_TH_05,
            IMGC_GAMMA_B_TH_06, IMGC_GAMMA_B_TH_07, IMGC_GAMMA_B_TH_08, IMGC_GAMMA_B_TH_09, IMGC_GAMMA_B_TH_10,
            IMGC_GAMMA_B_TH_11, IMGC_GAMMA_B_TH_12, IMGC_GAMMA_B_TH_13, IMGC_GAMMA_B_TH_14, IMGC_GAMMA_B_TH_15,
            IMGC_GAMMA_B_TH_16, IMGC_GAMMA_B_TH_17, IMGC_GAMMA_B_TH_18, IMGC_GAMMA_B_TH_19, IMGC_GAMMA_B_TH_20,
            IMGC_GAMMA_B_TH_21, IMGC_GAMMA_B_TH_22, IMGC_GAMMA_B_TH_23, IMGC_GAMMA_B_TH_24, IMGC_GAMMA_B_TH_25,
            IMGC_GAMMA_B_TH_26, IMGC_GAMMA_B_TH_27, IMGC_GAMMA_B_TH_28, IMGC_GAMMA_B_TH_29, IMGC_GAMMA_B_TH_30,
            IMGC_GAMMA_B_TH_31 };

        static const uint16_t gam_r_gain[VDC_GAM_GAIN_ADJ_NUM] =
        { IMGC_GAMMA_R_GAIN_00, IMGC_GAMMA_R_GAIN_01, IMGC_GAMMA_R_GAIN_02, IMGC_GAMMA_R_GAIN_03, IMGC_GAMMA_R_GAIN_04,
            IMGC_GAMMA_R_GAIN_05, IMGC_GAMMA_R_GAIN_06, IMGC_GAMMA_R_GAIN_07, IMGC_GAMMA_R_GAIN_08,
            IMGC_GAMMA_R_GAIN_09, IMGC_GAMMA_R_GAIN_10, IMGC_GAMMA_R_GAIN_11, IMGC_GAMMA_R_GAIN_12,
            IMGC_GAMMA_R_GAIN_13, IMGC_GAMMA_R_GAIN_14, IMGC_GAMMA_R_GAIN_15, IMGC_GAMMA_R_GAIN_16,
            IMGC_GAMMA_R_GAIN_17, IMGC_GAMMA_R_GAIN_18, IMGC_GAMMA_R_GAIN_19, IMGC_GAMMA_R_GAIN_20,
            IMGC_GAMMA_R_GAIN_21, IMGC_GAMMA_R_GAIN_22, IMGC_GAMMA_R_GAIN_23, IMGC_GAMMA_R_GAIN_24,
            IMGC_GAMMA_R_GAIN_25, IMGC_GAMMA_R_GAIN_26, IMGC_GAMMA_R_GAIN_27, IMGC_GAMMA_R_GAIN_28,
            IMGC_GAMMA_R_GAIN_29, IMGC_GAMMA_R_GAIN_30, IMGC_GAMMA_R_GAIN_31 };

        static const uint8_t gam_r_th[VDC_GAM_START_TH_NUM] =
        { IMGC_GAMMA_R_TH_01, IMGC_GAMMA_R_TH_02, IMGC_GAMMA_R_TH_03, IMGC_GAMMA_R_TH_04, IMGC_GAMMA_R_TH_05,
            IMGC_GAMMA_R_TH_06, IMGC_GAMMA_R_TH_07, IMGC_GAMMA_R_TH_08, IMGC_GAMMA_R_TH_09, IMGC_GAMMA_R_TH_10,
            IMGC_GAMMA_R_TH_11, IMGC_GAMMA_R_TH_12, IMGC_GAMMA_R_TH_13, IMGC_GAMMA_R_TH_14, IMGC_GAMMA_R_TH_15,
            IMGC_GAMMA_R_TH_16, IMGC_GAMMA_R_TH_17, IMGC_GAMMA_R_TH_18, IMGC_GAMMA_R_TH_19, IMGC_GAMMA_R_TH_20,
            IMGC_GAMMA_R_TH_21, IMGC_GAMMA_R_TH_22, IMGC_GAMMA_R_TH_23, IMGC_GAMMA_R_TH_24, IMGC_GAMMA_R_TH_25,
            IMGC_GAMMA_R_TH_26, IMGC_GAMMA_R_TH_27, IMGC_GAMMA_R_TH_28, IMGC_GAMMA_R_TH_29, IMGC_GAMMA_R_TH_30,
            IMGC_GAMMA_R_TH_31 };

        error = R_RVAPI_DispGammaVDC (vdc_ch, IMGC_GAMMA_ACTIVE, &gam_r_gain[0], &gam_r_th[0], &gam_g_gain[0],
                                        &gam_g_th[0], &gam_b_gain[0], &gam_b_th[0]);
    }

    return error;
} /* End of function r_image_quality_adjustment() */

/******************************************************************************
* Function Name: r_video_quality_adjustment
* Description  : Video Quality Adjustment
* Arguments    : vdc_ch
*              :   Channel
* Return Value : 0
*              :   success
*              : other
*              :   fail (vdc_error_t)
******************************************************************************/
vdc_error_t r_video_quality_adjustment(vdc_channel_t vdc_ch)
{
    vdc_error_t error;

    {
        vdc_color_matrix_t color_matrix;

        /* Operating mode */
        color_matrix.module = IMGC_BRIGHT_INPUTCTL_MODULE;

        /* Mode Select */
        color_matrix.mtx_mode = IMGC_BRIGHT_INPUTCTL_MODE;

        /* Bright */
        color_matrix.offset[VDC_COLORMTX_OFFST_YG] = IMGC_BRIGHT_INPUTCTL_OFFSET_YG;
        color_matrix.offset[VDC_COLORMTX_OFFST_B]  = IMGC_BRIGHT_INPUTCTL_OFFSET_B;
        color_matrix.offset[VDC_COLORMTX_OFFST_R]  = IMGC_BRIGHT_INPUTCTL_OFFSET_R;

        /* Contrast */
        color_matrix.gain[VDC_COLORMTX_GAIN_GG] = IMGC_CONTRAST_INPUTCTL_GAIN_GG;
        color_matrix.gain[VDC_COLORMTX_GAIN_GB] = IMGC_CONTRAST_INPUTCTL_GAIN_GB;
        color_matrix.gain[VDC_COLORMTX_GAIN_GR] = IMGC_CONTRAST_INPUTCTL_GAIN_GR;
        color_matrix.gain[VDC_COLORMTX_GAIN_BG] = IMGC_CONTRAST_INPUTCTL_GAIN_BG;
        color_matrix.gain[VDC_COLORMTX_GAIN_BB] = IMGC_CONTRAST_INPUTCTL_GAIN_BB;
        color_matrix.gain[VDC_COLORMTX_GAIN_BR] = IMGC_CONTRAST_INPUTCTL_GAIN_BR;
        color_matrix.gain[VDC_COLORMTX_GAIN_RG] = IMGC_CONTRAST_INPUTCTL_GAIN_RG;
        color_matrix.gain[VDC_COLORMTX_GAIN_RB] = IMGC_CONTRAST_INPUTCTL_GAIN_RB;
        color_matrix.gain[VDC_COLORMTX_GAIN_RR] = IMGC_CONTRAST_INPUTCTL_GAIN_RR;
        error = R_RVAPI_VideoCalibrationVDC (vdc_ch, &color_matrix);
    }

    if (VDC_OK == error)
    {
        vdc_enhance_sharp_t sharp_param;
        vdc_enhance_lti_t lti_param;
        vdc_period_rect_t enh_area;

        /* Sharpness of H1 */
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H1].shp_clip_o = IMGC_SHARPNESS_H1_CLIP_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H1].shp_clip_u = IMGC_SHARPNESS_H1_CLIP_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H1].shp_gain_o = IMGC_SHARPNESS_H1_GAIN_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H1].shp_gain_u = IMGC_SHARPNESS_H1_GAIN_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H1].shp_core   = IMGC_SHARPNESS_H1_CORE;

        /* Sharpness of H2 */
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H2].shp_clip_o = IMGC_SHARPNESS_H2_CLIP_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H2].shp_clip_u = IMGC_SHARPNESS_H2_CLIP_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H2].shp_gain_o = IMGC_SHARPNESS_H2_GAIN_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H2].shp_gain_u = IMGC_SHARPNESS_H2_GAIN_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H2].shp_core   = IMGC_SHARPNESS_H2_CORE;

        if (IMGC_SHARPNESS_H2_LPF == 0)
        {
            sharp_param.shp_h2_lpf_sel = VDC_OFF;
        }
        else
        {
            sharp_param.shp_h2_lpf_sel = VDC_ON;
        }

        /* Sharpness of H3 */
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H3].shp_clip_o = IMGC_SHARPNESS_H3_CLIP_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H3].shp_clip_u = IMGC_SHARPNESS_H3_CLIP_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H3].shp_gain_o = IMGC_SHARPNESS_H3_GAIN_O;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H3].shp_gain_u = IMGC_SHARPNESS_H3_GAIN_U;
        sharp_param.hrz_sharp[VDC_IMGENH_SHARP_H3].shp_core   = IMGC_SHARPNESS_H3_CORE;

        /* LTI H2 */
        if (IMGC_SHARPNESS_LTI_H2_LPF == 0)
        {
            lti_param.lti_h2_lpf_sel = VDC_OFF;
        }
        else
        {
            lti_param.lti_h2_lpf_sel = VDC_ON;
        }
        lti_param.lti[VDC_IMGENH_LTI1].lti_inc_zero = IMGC_SHARPNESS_LTI_H2_ZERO;
        lti_param.lti[VDC_IMGENH_LTI1].lti_gain     = IMGC_SHARPNESS_LTI_H2_GAIN;
        lti_param.lti[VDC_IMGENH_LTI1].lti_core     = IMGC_SHARPNESS_LTI_H2_CORE;

        /* LTI H4 */
        if (IMGC_SHARPNESS_LTI_H4_MEDTAP_SEL == VDC_LTI_MDFIL_SEL_ADJ2)
        {
            lti_param.lti_h4_median_tap_sel = VDC_LTI_MDFIL_SEL_ADJ2;
        }
        else
        {
            lti_param.lti_h4_median_tap_sel = VDC_LTI_MDFIL_SEL_ADJ1;
        }

        lti_param.lti[VDC_IMGENH_LTI2].lti_inc_zero = IMGC_SHARPNESS_LTI_H4_ZERO;
        lti_param.lti[VDC_IMGENH_LTI2].lti_gain     = IMGC_SHARPNESS_LTI_H4_GAIN;
        lti_param.lti[VDC_IMGENH_LTI2].lti_core     = IMGC_SHARPNESS_LTI_H4_CORE;

        /* Sharpness Effective area */
        enh_area.hs = IMGC_SHARPNESS_HS;
        enh_area.hw = IMGC_SHARPNESS_HW;
        enh_area.vs = IMGC_SHARPNESS_VS;
        enh_area.vw = IMGC_SHARPNESS_VW;
        error = R_RVAPI_VideoSharpnessLtiVDC (vdc_ch, IMGC_IMG_IMPRV_ID, IMGC_SHARPNESS_ACTIVE, &sharp_param,
            IMGC_SHARPNESS_LTI_ACTIVE, &lti_param, &enh_area);

    }

    return error;
} /* End of function r_video_quality_adjustment() */

/******************************************************************************
* Function Name: R_BCD_LcdSwapVideoBuffer
* Description  : Record video buffer swap request
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_LcdSwapVideoBuffer( void )
{
    Req_Layer1_Swap = 1;

    return;
} /* End of function R_BCD_LcdSwapVideoBuffer() */

/******************************************************************************
* Function Name: R_BCD_LcdSwapGraphicsBuffer
* Description  : Record graphics buffer swap request
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_LcdSwapGraphicsBuffer(void)
{
    Req_Layer2_Swap = 1;

    return;
} /* End of function R_BCD_LcdSwapGraphicsBuffer() */

/******************************************************************************
* Function Name: R_BCD_LcdVSync
* Description  : Interrupt process for VDC6
*              : Check the swap request of the video buffer and the overlap buffer,
*              : and swap the buffer if there is a request.
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_LcdVSync(void) 
{
    /* Check whether the video buffer swap request is pending */
    if (Req_Layer1_Swap != 0)
    {
        if( Layer1_buffer_id == 0 )
        {
            R_RVAPI_GraphChangeSurfaceVDC(VDC_CHANNEL_0, VDC_LAYER_ID_0_RD, video_buffer[0]);
            Layer1_buffer_id = 1;
        }
        else
        {
            R_RVAPI_GraphChangeSurfaceVDC(VDC_CHANNEL_0, VDC_LAYER_ID_0_RD, video_buffer[1]);
            Layer1_buffer_id = 0;
        }
    }

    /* Check whether the overlap buffer swap request is pending */
    if (Req_Layer2_Swap != 0)
    {
        if (layer2_buffer_id == 0)
        {
            R_RVAPI_GraphChangeSurfaceVDC(VDC_CHANNEL_0, VDC_LAYER_ID_2_RD, graphics2_buffer[0]);
            layer2_buffer_id = 1;
        }
        else
        {
            R_RVAPI_GraphChangeSurfaceVDC(VDC_CHANNEL_0, VDC_LAYER_ID_2_RD, graphics2_buffer[1]);
            layer2_buffer_id = 0;
        }
        DrawSwapBuffer(VDC_LAYER_ID_2_RD);
    }
    Req_Layer1_Swap = 0;
    Req_Layer2_Swap = 0;
} /* End of function R_BCD_LcdVSync() */

/******************************************************************************
* Function Name: R_BCD_LcdWriteLine
* Description  : Draw a line in the overlap buffer
* Arguments    : x1
*              :   X coordinate of the start point of the line
*              : y1
*              :   Y coordinate of the start point of the line
*              : x2
*              :   X coordinate of the end point of the line
*              : y2
*              :   Y coordinate of the end point of the line
*              : color
*              :   Line color
* Return Value : -
******************************************************************************/
void R_BCD_LcdWriteLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color)
{
    DrawLine( VDC_LAYER_ID_2_RD, x1, y1, x2, y2, color );

    return;
} /* End of function R_BCD_LcdWriteLine() */

/******************************************************************************
* Function Name: R_BCD_LcdWriteChar
* Description  : Display character in overlap buffer
* Arguments    : code
*              :   Code to be displayed
*              : x
*              :   X coordinate to display character
*              : y
*              :   Y coordinate to display character
*              : color
*              :   character color
* Return Value : -
******************************************************************************/
void R_BCD_LcdWriteChar(uint8_t code, uint32_t x, uint32_t y, uint8_t color) 
{
    int32_t height;
    int32_t width;
    uint8_t font;

    int32_t width_offset;
    int32_t check_bit;
    for (height = 0; height < R_BCD_FONTDATA_HEIGHT; height++)
    {
        check_bit = 7;
        width_offset = 0;
        font = font_array[code-0x20][height][width_offset++];
        for (width = 0; width < R_BCD_FONTDATA_WIDTH; width++)
        {
            if ((font & (0x1 << (check_bit--))) != 0)
            {
                DrawPoint( VDC_LAYER_ID_2_RD, (width + x), (height + y) , color );
            }
            if (check_bit < 0)
            {
                check_bit = 7;
                font = font_array[code-0x20][height][width_offset++];
            }
        }
    }

    return;
} /* End of function R_BCD_LcdWriteChar() */

/******************************************************************************
* Function Name: R_BCD_LcdWriteString
* Description  : Display character string in overlap buffer
* Arguments    : pcode
*              :   A pointer to the character string to be displayed
*              : x
*              :   X coordinate to display character string
*              : y
*              :   Y coordinate to display character string
*              : color
*              :   character string color
* Return Value : -
******************************************************************************/
void R_BCD_LcdWriteString(uint8_t *pcode, uint32_t x, uint32_t y, uint8_t color)
{
    int i; 
    int len; 

    len = strlen(pcode);

    for (i = 0; i < len; i++)
    {
        R_BCD_LcdWriteChar(*pcode++, (x + (i * R_BCD_FONTDATA_WIDTH)), y, color);
    }

    return;
} /* End of function R_BCD_LcdWriteString() */

/******************************************************************************
* Function Name: R_BCD_LcdClearGraphicsBuffer
* Description  : Clear overlap buffer
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_LcdClearGraphicsBuffer(void)
{
    /* Cast to an appropriate type */
    memset(R_BCD_LcdGetOLVramAddress(), (int)0xffffffff, (GRAPHICS2_BUFFER_STRIDE * LAYER2_DISP_AREA_VW));

    return;
} /* End of function R_BCD_LcdClearGraphicsBuffer() */

/* End of file */
