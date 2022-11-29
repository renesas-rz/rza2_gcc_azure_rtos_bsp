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
 * @headerfile     r_rvapi_vdc.h
 * @brief          RVAPI for VDC function header
 * @version        1.12
 * H/W Platform    RZ/A2M
 *****************************************************************************/



/**************************************************************************//**
 * @ingroup R_SW_PKG_93_VIDEO_API
 * @defgroup R_SW_PKG_93_VIDEO_VDC Video VDC
 * @brief VDC Unit Module
 *
 * @anchor R_SW_PKG_93_VIDEO_VDC_API_SUMMARY
 * @par Summary
 *
 * This middleware module provides functionality for a user to utilise the
 * Video Display Controller (VDC) for graphical/video output onto a display.
 * Configuration settings range from enabling interrupts to alpha blending.
 *
 * @anchor R_SW_PKG_93_VIDEO_VDC_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_vdc.h"

/* Multiple inclusion prevention macro */
#ifndef R_RVAPI_VDC_H
#define R_RVAPI_VDC_H
#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
 Macro definitions
 ******************************************************************************/
/* Version Number of API */
#define RVAPI_RZA2_VERSION_MAJOR (1)
#define RVAPI_RZA2_VERSION_MINOR (11)

#define VSYNC_1_2_FH_TIMING  (858u) /*!< Vsync signal 1/2fH phase timing */
#define VSYNC_1_4_FH_TIMING  (429u) /*!< Vsync signal 1/4fH phase timing */

/******************************************************************************
 Typedef definitions
 ******************************************************************************/
/*! @struct clock_config_t
 *  @brief Clock setting config
 */
typedef struct {
    vdc_panel_clksel_t panel_clk;       /*!< Panel Clock Source Select               */
    vdc_panel_clk_dcdr_t panel_clk_div; /*!< The clock input to frequency divider 1  */
    vdc_lvds_t * lvds;                  /*!< LVDS parameter  */
} clock_config_t;

/*! @struct qe_config_t
 *  @brief LCD setting config (QE for VDC header)
 */
typedef struct {
    uint16_t vps;                      /*!< Vertical Pulse start position                   */
    uint16_t vpw;                      /*!< Vertical Pulse Width(VPW)                       */
    uint16_t vs;                       /*!< LCD display area vertical start position        */
    uint16_t vdp;                      /*!< Vertical Display Period(VDP)                    */
    uint16_t hps;                      /*!< Horizontal Pulse start position                 */
    uint16_t hpw;                      /*!< Horizontal Pulse Width(HPW)                     */
    uint16_t hs;                       /*!< LCD display area horizontal start position      */
    uint16_t hdp;                      /*!< Horizontal Display Period(HDP)                  */
    uint16_t vtp;                      /*!< Vertical Total Period(VTP)                      */
    uint16_t htp;                      /*!< Horizontal Total Period(HTP)                    */
    vdc_lcd_tcon_pin_t tcon_vsync;     /*!< Vsync_TCON_select                               */
    vdc_lcd_tcon_pin_t tcon_hsync;     /*!< Hsync_TCON_select                               */
    vdc_lcd_tcon_pin_t tcon_de;        /*!< DE_TCON_select                                  */
    vdc_sig_pol_t tcon_vsync_inv;      /*!< Vsync_POL                                       */
    vdc_sig_pol_t tcon_hsync_inv;      /*!< Hsync_POL                                       */
    vdc_sig_pol_t tcon_de_inv;         /*!< DE_POL                                          */
    uint16_t tcon_half;                /*!< TCON reference timing, 1/2fH timing(HTP)        */
    uint16_t tcon_ofset;               /*!< TCON reference timing, offset Hsync signal timing */
    vdc_edge_t lcd_data_out_edge;      /*!< LCD_DATA_OUT_EDGE                               */
    vdc_edge_t out_endian_on;          /*!< Bit endian change on/off control                */
    vdc_edge_t out_swap_on;            /*!< B/R signal swap on/off control                  */
    vdc_lcd_outformat_t lcd_outformat; /*!<  LCD output format select                       */
} qe_config_t;

/*! @struct g_surface_config_t
 *  @brief Graphics surface read config
 */
typedef struct {
    vdc_layer_id_t layer_id;           /*!< Layer ID                      */
    vdc_pd_disp_rect_t disp_area;      /*!< Display Area                  */

    void * fb_buff;                    /*!< Frame buffer address          */
    uint32_t fb_stride;                /*!< Frame buffer stride           */

    vdc_gr_format_t read_format;       /*!< Read Format                   */
    uint32_t * clut_table;             /*!< CLUT table  */

    vdc_gr_ycc_swap_t read_ycc_swap;   /*!< Read Swap for YCbCr422        */
    vdc_wr_rd_swa_t read_swap;         /*!< Read Swap 8bit/16bit/32bit    */

    vdc_gr_disp_sel_t disp_mode;       /*!< Display mode select           */
} gr_surface_disp_config_t;

/*! @struct digital_in_t
 *  @brief Digital Video input config
 */
typedef struct {
    vdc_extin_format_t inp_format;          /*! External input format select */
    vdc_edge_t inp_pxd_edge;                /*!< Clock edge select for capturing external input video image signals */
    vdc_onoff_t inp_endian_on;              /*!< External input bit endian change on/off control */
    vdc_onoff_t inp_swap_on;                /*!< External input B/R signal swap on/off control */
    vdc_sig_pol_t inp_vs_inv;               /*!< External input Vsync signal DV_VSYNC inversion control */
    vdc_sig_pol_t inp_hs_inv;               /*!< External input Hsync signal DV_HSYNC inversion control */
    vdc_extin_ref_hsync_t inp_h_edge_sel;   /*!< BT656 EAV or SAV     */
    vdc_extin_input_line_t inp_f525_625;    /*!< BT656 input line num */
    vdc_extin_h_pos_t inp_h_pos;            /*!< Y/Cb/Y/Cr data string start timing to Hsync reference */
} digital_in_t;

/*! @struct v_surface_config_t
 *  @brief Video surface write config
 */
typedef struct {
    vdc_layer_id_t layer_id;            /*!< Layer ID                    */
    vdc_period_rect_t cap_area;         /*!< Capure timming              */

    void * fb_buff;                     /*!< Video frame buffer address  */
    uint32_t fb_stride;                 /*!< Write buffer stride[byte]   */
    uint32_t fb_offset;                 /*!< Write buffer offset[byte]   */
    uint32_t fb_num;                    /*!< Frame buffer num            */

    vdc_res_md_t write_format;          /*!< Write format                */
    uint16_t write_fb_vw;               /*!< Scaling out height          */
    uint16_t write_fb_hw;               /*!< Scaling out width           */
    vdc_wr_rd_swa_t write_swap;         /*!< Write swap 8bit/16bit/32bit */

    vdc_res_inter_t res_inter;          /*!< Field operating mode select */
    vdc_wr_md_t write_rot;              /*!< Frame buffer writing mode for image processing */
} v_surface_config_t;

/*! @struct v_surface_disp_config_t
 *  @brief Video surface read config
 */
typedef struct {
    vdc_pd_disp_rect_t disp_area;       /*!< Display Area                */
    vdc_gr_ycc_swap_t read_ycc_swap;    /*!< Read Swap for YCbCr422      */
    vdc_wr_rd_swa_t read_swap;          /*!< Read Swap 8bit/16bit/32bit  */
} v_surface_disp_config_t;

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
/* VDC initialization */

/**
 * @fn          R_RVAPI_InitializeVDC
 * @brief       VDC initialization processing
 *
 * @param[in]   ch:             Channel
 * @param[in]   c_cnf:          Clock setting config
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_InitializeVDC(const vdc_channel_t ch,
        const clock_config_t * const c_cnf);

/**
 * @fn          R_RVAPI_TerminateVDC
 * @brief       VDC terminate processing
 *
 * @param[in]   ch:             Channel
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_TerminateVDC(const vdc_channel_t ch);

/* Create Graphics surface */

/**
 * @fn          R_RVAPI_DispControlVDC
 * @brief       VDC display out control processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   res_vs_sel:       Vsync signal output select:<BR>
 *                                VDC_OFF: External input Vsync signal<BR>
 *                                VDC_ON : Internally generated free-running Vsync signal
 * @param[in]   q_cnf:            LCD setting config (QE for VDC header)
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_DispControlVDC(const vdc_channel_t ch,
        const vdc_onoff_t res_vs_sel, const qe_config_t * const q_cnf);

/**
 * @fn          R_RVAPI_DispControlConfigVDC
 * @brief       VDC display out control processing
 *               Add the following parameters
 *                -Bit Endian Change On/Off Control
 *                -B/R Signal Swap On/Off Control
  *
 * @param[in]   ch:               Channel
 * @param[in]   res_vs_sel:       Vsync signal output select:<BR>
 *                                VDC_OFF: External input Vsync signal<BR>
 *                                VDC_ON : Internally generated free-running Vsync signal
 * @param[in]   q_cnf:            LCD setting config (QE for VDC header)
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_DispControlConfigVDC(const vdc_channel_t ch,
        const vdc_onoff_t res_vs_sel, const qe_config_t * const q_cnf);

/**
 * @fn          R_RVAPI_GraphCreateSurfaceVDC
 * @brief       Graphics create surface processing
 *
 * @param[in]   ch:             Channel
 * @param[in]   gr_disp_cnf:    Graphics surface read config
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_GraphCreateSurfaceVDC(const vdc_channel_t ch,
        const gr_surface_disp_config_t * const gr_disp_cnf);

/**
 * @fn          R_RVAPI_GraphChangeSurfaceVDC
 * @brief       Graphics change surface processing
 *
 * @param[in]   ch:             Channel
 * @param[in]   layer_id:       Layer ID
 * @param[in]   fb_buff:        Base address of the frame buffer
 *
 * @retval      VDC_ER:         VDC driver error code
 */
vdc_error_t R_RVAPI_GraphChangeSurfaceVDC(const vdc_channel_t ch,
        const vdc_layer_id_t layer_id, void * const framebuff);

/**
 * @fn          R_RVAPI_GraphChangeSurfaceConfigVDC
 * @brief       Graphics change surface processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 * @param[in]   fb_buff:          Base address of the frame buffer
 * @param[in]   gr_grc:           Disp area
 * @param[in]   width_read_fb:    VRAM Read size
 * @param[in]   gr_disp_sel:      disp sel
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_GraphChangeSurfaceConfigVDC(const vdc_channel_t ch, const vdc_layer_id_t layer_id,
        void * const fb_buff, vdc_period_rect_t * const gr_grc,
        vdc_width_read_fb_t * const width_read_fb, vdc_gr_disp_sel_t * const gr_disp_sel);

/**
 * @fn          R_RVAPI_GraphDestroySurfaceVDC
 * @brief       Graphics destroy surface processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_GraphDestroySurfaceVDC(const vdc_channel_t ch,
        const vdc_layer_id_t layer_id);

/**
 * @fn          R_RVAPI_DispPortSettingVDC
 * @brief       Graphics display port setting
 *
 * @param[in]   ch              : Channel
 * @param[in]   port_func       : Callback function for port setting
 *
 * @return      None.
 */
void R_RVAPI_DispPortSettingVDC(const vdc_channel_t ch,
        void (* const port_func)(uint32_t));

/* Create Video surface */

/**
 * @fn          R_RVAPI_VideoControlVDC
 * @brief       Graphics display port setting
 *
 * @param[in]   ch:               Channel
 * @param[in]   port_func:        Callback function for port setting
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_VideoControlVDC(const vdc_channel_t ch,
        const digital_in_t * const digital);

/**
 * @fn          R_RVAPI_VideoCreateSurfaceVDC
 * @brief       Video create surface processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   v_cnf:            Video surface write config
 * @param[in]   v_disp_cnf:       Video surface read config
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_VideoCreateSurfaceVDC(const vdc_channel_t ch,
        const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf);

/**
 * @fn      R_RVAPI_Imrls2CreateSurfaceVDC
 * @warning This function has not been implemented.
 */
vdc_error_t R_RVAPI_Imrls2CreateSurfaceVDC(const vdc_channel_t ch,
        const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf);

/**
 * @fn      R_RVAPI_ImrlsdCreateSurfaceVDC
 * @warning This function has not been implemented.
 */
vdc_error_t R_RVAPI_ImrlsdCreateSurfaceVDC(const vdc_channel_t ch,
        const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf);

/**
 * @fn          R_RVAPI_VideoDestroySurfaceVDC
 * @brief       Video destroy surface processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_VideoDestroySurfaceVDC(const vdc_channel_t ch,
        const vdc_layer_id_t layer_id);

/**
 * @fn          R_RVAPI_VideoPortSettingVDC
 * @brief       Video port setting
 *
 * @param[in]   ch:               Channel
 * @param[in]   port_func:        Callback function for port setting
 *
 * @return None.
 */
void R_RVAPI_VideoPortSettingVDC(const vdc_channel_t ch,
        void (* const port_func)(uint32_t));

/* Interrupt Setting */

/**
 * @fn          R_RVAPI_InterruptEnableVDC
 * @brief       Interrupt Enable
 *
 * @param[in]   ch:               Channel
 * @param[in]   flag:             VDC interrupt type
 * @param[in]   line_num:         Line num
 * @param[in]   callback:         Callback function for VDC interrupt
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_InterruptEnableVDC(const vdc_channel_t ch,
        const vdc_int_type_t flag, const uint16_t line_num,
        void (* const callback)(vdc_int_type_t int_type));

/**
 * @fn          R_RVAPI_InterruptDisableVDC
 * @brief       Interrupt disable
 *
 * @param[in]   ch:               Channel
 * @param[in]   flag:             VDC interrupt type
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_InterruptDisableVDC(const vdc_channel_t ch,
        const vdc_int_type_t flag);

/* Alphablending  */

/**
 * @fn          R_RVAPI_AlphablendingRectVDC
 * @brief       Alpha Blending Rect processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 * @param[in]   alpha_onoff:      ON/OFF setting for alpha blending in a rectangular area
 * @param[in]   alpha_area:       Parameter for alpha blending in a rectangular area
 * @param[in]   alpha_value:      Alpha value0 (0 - 255)
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_AlphablendingRectVDC(const vdc_channel_t ch,
        const vdc_layer_id_t layer_id, const vdc_onoff_t alpha_onoff,
        const vdc_pd_disp_rect_t * const alpha_area,
        const uint8_t alpha_value);

/**
 * @fn          R_RVAPI_AlphablendingVDC
 * @brief       Alpha Blending Rect processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 * @param[in]   alpha_value:      Alpha value0 (0 - 255) ARGB1555 alpha value A=0
 * @param[in]   alpha_value:      Alpha value1 (0 - 255) ARGB1555 alpha value A=1
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_AlphablendingVDC(const vdc_channel_t ch, const vdc_layer_id_t layer_id,
        uint8_t alpha_value0, uint8_t alpha_value1 );

/**
 * @fn          R_RVAPI_ChromakeyVDC
 * @brief       Chromakey processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   layer_id:         Layer ID
 * @param[in]   gr_ck_on:         Chroma-key ON/OFF setting
 * @param[in]   ck_color:         Replaced color
 * @param[in]   rep_alpha:        Replaced alpha (0 - 255)
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_ChromakeyVDC(const vdc_channel_t ch,
        const vdc_layer_id_t layer_id, const vdc_onoff_t gr_ck_on,
        const uint32_t ck_color, const uint8_t rep_alpha);

/* Display Adjustment */

/**
 * @fn          R_RVAPI_DispCalibrationVDC
 * @brief       Display out calibration initialization processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   route:            Correction circuit sequence control
 * @param[in]   bright:           Brightness (DC) adjustment parameter
 * @param[in]   contrast:         Contrast (gain) adjustment parameter
 * @param[in]   panel_dither:     Panel dithering parameter
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_DispCalibrationVDC(const vdc_channel_t ch,
        const vdc_calibr_route_t route,
        const vdc_calibr_bright_t * const bright,
        const vdc_calibr_contrast_t * const contrast,
        const vdc_calibr_dither_t * const panel_dither);

/**
 * @fn          R_RVAPI_DispGammaVDC
 * @brief       Display Gamma initialization processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   gam_on:           Gamma correction ON/OFF setting
 * @param[in]   gam_r_gain:       Gain adjustment of area 0 to 31 of R signal
 * @param[in]   gam_r_th:         Start threshold of area 1 to 31 of R signal
 * @param[in]   gam_g_gain:       Gain adjustment of area 0 to 31 of G signal
 * @param[in]   gam_g_th:         Start threshold of area 1 to 31 of G signal
 * @param[in]   gam_b_gain:       Gain adjustment of area 0 to 31 of B signal
 * @param[in]   gam_b_th:         Start threshold of area 1 to 31 of B signal
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_DispGammaVDC(const vdc_channel_t ch,
        const vdc_onoff_t gam_on, const uint16_t * const gam_r_gain,
        const uint8_t * const gam_r_th, const uint16_t * const gam_g_gain,
        const uint8_t * const gam_g_th, const uint16_t * const gam_b_gain,
        const uint8_t * const gam_b_th);

/* Video Adjustment */

/**
 * @fn          R_RVAPI_VideoCalibrationVDC
 * @brief       Video Contrast and Brightness initialization processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   color_matrix:     Color matrix setup parameter
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_VideoCalibrationVDC(const vdc_channel_t ch,
        const vdc_color_matrix_t * const color_matrix);

/**
 * @fn          R_RVAPI_VideoSharpnessLtiVDC
 * @brief       Video Sharpness and LTI initialization processing
 *
 * @param[in]   ch:               Channel
 * @param[in]   imgimprv_id:      Image quality improver ID
 * @param[in]   shp_h_on:         Sharpness ON/OFF setting
 * @param[in]   sharp_param:      Sharpness setup parameter
 * @param[in]   lti_h_on:         LTI ON/OFF setting
 * @param[in]   lti_param:        LTI setup parameter
 * @param[in]   enh_area:         Enhancer-enabled area setup parameter
 *
 * @retval      VDC_ER:           VDC driver error code
 */
vdc_error_t R_RVAPI_VideoSharpnessLtiVDC(const vdc_channel_t ch,
        const vdc_imgimprv_id_t imgimprv_id, const vdc_onoff_t shp_h_on,
        const vdc_enhance_sharp_t * const sharp_param,
        const vdc_onoff_t lti_h_on, const vdc_enhance_lti_t * const lti_param,
        const vdc_period_rect_t * const enh_area);


/**
 * @fn          R_RVAPI_VideoCreateSurfaceIMRL2
 * @brief       Video create surface processing
 *
 * @param[in]   ch:              Channel
 * @param[in]   v_cnf:           Video surface write config
 * @param[in]   v_disp_cnf:      Video surface read config
 *
 * @retval      VDC driver error code
 */
vdc_error_t R_RVAPI_VideoCreateSurfaceIMRL2(const vdc_channel_t ch,
        const v_surface_config_t * const v_cnf,
        const v_surface_disp_config_t * const v_disp_cnf);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_RVAPI_VDC_H */
/**************************************************************************//**
 * @} (end addtogroup)
 *****************************************************************************/
