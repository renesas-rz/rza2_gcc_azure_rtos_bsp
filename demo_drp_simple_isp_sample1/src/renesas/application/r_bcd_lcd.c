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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
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
#define     LAYER0_DISP_AREA_HS      (0u)
#define     LAYER0_DISP_AREA_VS      (0u)
#define     LAYER0_DISP_AREA_HW      (R_BCD_LCD_WIDTH)
#define     LAYER0_DISP_AREA_VW      (R_BCD_LCD_HEIGHT)

#define     VIDEO_BUFFER_STRIDE      ((LAYER0_DISP_AREA_HW + (R_BCD_LCD_STRIDE - 1)) & ~(R_BCD_LCD_STRIDE - 1))
#define     VIDEO_BUFFER_HEIGHT      (LAYER0_DISP_AREA_VW)
#define     VIDEO_BUFFER_NUM         (2u)
#define     VIDEO_BUFFER_SIZE        ((VIDEO_BUFFER_STRIDE * VIDEO_BUFFER_HEIGHT) * 2)

#define     LAYER2_DISP_AREA_HS      (0u)
#define     LAYER2_DISP_AREA_VS      (0u)
#define     LAYER2_DISP_AREA_HW      (R_BCD_LCD_WIDTH)
#define     LAYER2_DISP_AREA_VW      (R_BCD_LCD_HEIGHT)

#define     GRAPHICS2_BUFFER_STRIDE  (((LAYER2_DISP_AREA_HW / 2) + (R_BCD_LCD_STRIDE - 1)) & ~(R_BCD_LCD_STRIDE - 1)) 
#define     GRAPHICS2_BUFFER_HEIGHT  (LAYER2_DISP_AREA_VW)
#define     GRAPHICS2_BUFFER_NUM     (2u)
#define     GRAPHICS2_BUFFER_SIZE    (GRAPHICS2_BUFFER_STRIDE * GRAPHICS2_BUFFER_HEIGHT)

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t video_buffer[VIDEO_BUFFER_NUM][VIDEO_BUFFER_SIZE] __attribute__ ((section("Video_RAM"))); 
static uint8_t graphics2_buffer[GRAPHICS2_BUFFER_NUM][GRAPHICS2_BUFFER_SIZE] __attribute__ ((section("Graphics_RAM")));
static volatile int32_t vsync_count = 0;

static uint32_t Req_Layer1_Swap = 0;
static uint32_t Req_Layer2_Swap = 0;
static uint32_t Layer1_buffer_id = 0;
static uint32_t layer2_buffer_id = 0;

static void lcd_int_callback(vdc_int_type_t int_type);

static vdc_error_t display_init(const vdc_channel_t vdc_ch);
static vdc_error_t image_quality_adjustment(vdc_channel_t vdc_ch);

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern void VDC_LcdPortSetting(uint32_t param);

/******************************************************************************
* Function Name: R_BCD_LcdGetVramAddress
* Description  : Acquire the address of the video buffer which is not currently being displayed
* Arguments    : -
* Return Value : Video buffer address
******************************************************************************/
uint8_t *R_BCD_LcdGetVramAddress(void)
{
    uint8_t *p_addr; 

    if (Req_Layer1_Swap != 0)
    {
        p_addr = &video_buffer[1-Layer1_buffer_id][0];
    }
    else
    {
        p_addr = &video_buffer[Layer1_buffer_id][0];
    }

    return p_addr;
}   /* End of function R_BCD_LcdGetVramAddress() */

/******************************************************************************
* Function Name: R_BCD_LcdGetOLVramAddress
* Description  : Acquire the address of the Overlap buffer which is not currently being displayed
* Arguments    : -
* Return Value : Overlap buffer address
******************************************************************************/
uint8_t *R_BCD_LcdGetOLVramAddress(void) 
{
    uint8_t *p_addr; 

    if (Req_Layer2_Swap != 0)
    {
        p_addr = &graphics2_buffer[1-layer2_buffer_id][0];
    }
    else
    {
        p_addr =  &graphics2_buffer[layer2_buffer_id][0];
    }

    return p_addr;
}   /* End of function R_BCD_LcdGetOLVramAddress() */

/******************************************************************************
* Function Name: lcd_int_callback
* Description  : VDC6 Interrupt callback function
* Arguments    : int_type
*              :   VDC6 interrupt type
* Return Value : -
******************************************************************************/
static void lcd_int_callback(vdc_int_type_t int_type)
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
}   /* End of function lcd_int_callback() */

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
    error = display_init (vdc_ch);

    /***********************************************************************/
    /* Interrupt init */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        uint16_t line_num;

        line_num = LCD_CH0_DISP_VS + R_BCD_LCD_HEIGHT;
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_VLINE, line_num, lcd_int_callback);
    }

    if (error == VDC_OK)
    {
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_S0_LO_VSYNC, 0, lcd_int_callback);
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
        gr_disp_cnf.fb_stride = VIDEO_BUFFER_STRIDE*2;        /* Frame buffer stride             */

        /* Display Area               */
        gr_disp_cnf.disp_area.hs_rel = LAYER0_DISP_AREA_HS;
        gr_disp_cnf.disp_area.hw_rel = LAYER0_DISP_AREA_HW;
        gr_disp_cnf.disp_area.vs_rel = LAYER0_DISP_AREA_VS;
        gr_disp_cnf.disp_area.vw_rel = LAYER0_DISP_AREA_VW;

        gr_disp_cnf.read_format   = VDC_GR_FORMAT_YCBCR422;        /* Read Format                */
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
        image_quality_adjustment(vdc_ch);
    }

    /* Display On */
    if (VDC_OK == error)
    {
        R_RVAPI_DispPortSettingVDC(vdc_ch, &VDC_LcdPortSetting);
    }

    return 0;
}   /* End of function R_BCD_LcdInit() */

/******************************************************************************
* Function Name: display_init
* Description  : VDC6 output setting
* Arguments    : vdc_ch
*              :   Channel
* Return Value : 0
*              :   success
*              : other
*              :   fail (vdc_error_t)
******************************************************************************/
static vdc_error_t display_init(const vdc_channel_t vdc_ch)
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
}   /* End of function display_init() */

/******************************************************************************
* Function Name: image_quality_adjustment
* Description  : Image Quality Adjustment
* Arguments    : vdc_ch
*              :   Channel
* Return Value : 0
*              :   success
*              : other
*              :   fail (vdc_error_t)
******************************************************************************/
static vdc_error_t image_quality_adjustment(vdc_channel_t vdc_ch)
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
} /* End of function image_quality_adjustment() */

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
                DrawPoint( VDC_LAYER_ID_2_RD, (width + (int32_t)x), (height + (int32_t)y) , /* less than 1920 */
                        color );
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
    uint32_t i; 
    size_t len; 

    len = (uint32_t)strlen((char_t *)pcode);    /* Converted a pointer that stores a character string */

    for (i = 0; i < (uint32_t)len; i++)         /* less than 80 */
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
