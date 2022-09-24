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

/**********************************************************************************************************************
 * File Name    : r_bcd_main.c
 * Description  : This is a sample source code of DRP.
 *********************************************************************************************************************/

/*******************************************************************************
  Includes   <System Includes> , "Project Includes"
 *******************************************************************************/
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_cache_lld_rza2m.h"

#include "r_bcd_ae.h"
#include "r_bcd_camera.h"
#include "r_bcd_lcd.h"
#include "draw.h"
#include "perform.h"
#include "r_mmu_lld_rza2m.h"

#include "r_dk2_if.h"
#include "r_drp_simple_isp.h"
#include "r_drp_resize_bilinear_fixed.h"
#include "r_drp_resize_bilinear_fixed_rgb.h"

#include "r_drp_gaussian_blur.h"
#include "r_drp_canny_calculate.h"
#include "r_drp_canny_hysterisis.h"
#include "r_drp_dilate.h"
#include "r_drp_find_contours.h"
#include "r_drp_bayer2rgb_color_correction.h"

#include "r_drp_cropping_rgb.h"

/*******************************************************************************
  Macro definitions
 *******************************************************************************/
#define TILE_0          (0)
#define TILE_1          (1)
#define TILE_2          (2)
#define TILE_3          (3)
#define TILE_4          (4)
#define TILE_5          (5)

#define DRP_NOT_FINISH  (0)
#define DRP_FINISH      (1)

#define DRP_DRV_ASSERT(var_x) if ((var_x) != 0) goto sample_finish
#define WCHAR(str_buf, str, time_sec) (sprintf((uint8_t *)&(str_buf)[0], "%s %d.%d[ms]", \
            (str), (int32_t)((time_sec) / 1000), \
            (int32_t)(((time_sec) - (((time_sec) / 1000) * 1000)) / 100)))

#define WEAK_LEVEL      (0x48)
#define STRONG_LEVEL    (0xA8)

/*After processing Application Find Contours*/
#define R_BCD_DRP_PRESTIC_LENGTH_MIN    (1)
#define R_BCD_DRP_CROP_LENGTH_MIN       (16)
#define R_BCD_DRP_CROP_LENGTH_MAX       (400)
#define R_BCD_DRP_CROP_LENGTH_DIFF      (10)
#define R_BCD_DRP_IMAGE_RESIZE          (2)
#define HYSTERISIS_COUNT                (2)
#define FIND_CONTOURS_NUM               (400)
#define R_BCD_DRP_CROP_ONE_SIDE_OVER

/* DRP Parameter of Resize bilinear fixed */
#define DRP_RESIZE_HALF         (0x20)

/* Offset lines for Layer0 display */
#define LAYER0_DISP_OFFSET      (240)

/* Display result string length */
#define RESULT_DISP_BUF_SIZE    (80)

/* ISP parameter */
#define ISP_GAIN_R              (0x1500)
#define ISP_GAIN_G              (0x1000)
#define ISP_GAIN_B              (0x18E3)
#define ISP_GAMMA               (0x01)
#define ISP_COMPONENT           (0x01)
#define ISP_BIAS_R              (0x0000)
#define ISP_BIAS_G              (0x0000)
#define ISP_BIAS_B              (0x0000)
#define ISP_MEDIAN_A            (0x0100)
#define ISP_STRENGTH            (28)
#define ISP_CORING              (60)

#if 0 
#define GAMMA_VALUE             ((double)1.2)
#else
#define GAMMA_VALUE             (1.2)
#endif

#define AE_MARGIN               (BRIGHTNESS_TH * 3)

/* Status of key_status */
#define KEY_STAUS_INIT  (0xFFFFFFFFU)
#define KEY_CHECK_BIT   (0x00000003U)
#define KEY_JUST_ON     (0x00000002U)

/* Status of isp_ae_mode */
#define ISP_AE_OFF  (0)
#define ISP_AE_ON   (1)

/* Work RAM Size */
#define DRP_WORK_RAM_SIZE_WIDTH         (320)
#define DRP_WORK_RAM_SIZE_HEIGHT        (240)

#define IMAGE_OUTPUT_WIDTH  (64)
#define IMAGE_OUTPUT_HEIGHT (64)

#define R_BCD_CROP_INPUT_X  (64)
#define R_BCD_CROP_INPUT_Y  (64)

#define R_BCD_DRP_CROP_NUM    (3)

#define R_BCD_DRP_CONTOUR_COMPARE_OCCUPIED_AREA
/*#define R_BCD_DRP_CONTOUR_COMPARE_AREA_SIZE*/

#define R_BCD_DRP_COUNTOURS_TO_LCD
#define R_BCD_DRP_CROP_SQUARE

/**********************************************************************************************************************
  Local Typedef definitions
 *********************************************************************************************************************/

typedef struct contours_rect_t
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint32_t num;
    uint32_t addr;
} contours_rect_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
} contours_region_t;

/*******************************************************************************
  Exported global variables
 *******************************************************************************/
extern uint8_t g_drp_lib_isp_bayer2grayscale_6[];
extern uint8_t g_drp_lib_resize_bilinear_fixed[];

extern uint8_t g_drp_lib_gaussian_blur[];
extern uint8_t g_drp_lib_canny_calculate[];
extern uint8_t g_drp_lib_canny_hysterisis[];

extern uint8_t g_drp_lib_find_contours[];
extern uint8_t g_drp_lib_bayer2rgb_color_correction[];

extern uint8_t g_drp_lib_resize_bilinear_fixed_rgb[];
extern uint8_t g_drp_lib_cropping_rgb[];

/*******************************************************************************
  Private (static) variables and functions
 *******************************************************************************/
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = { 0 }; 
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = { DRP_NOT_FINISH };

static r_drp_simple_isp_t param_isp         __attribute__ ((section("Uncache_IRAM")));
static uint8_t look_up_table[256]           __attribute__ ((section("Uncache_IRAM")));
static uint32_t ave_result[9]               __attribute__ ((section("Uncache_IRAM")));

static r_drp_resize_bilinear_fixed_t        param_resize\
                                                __attribute__ ((section("Uncache_IRAM")));

contours_rect_t                             g_contours_rect_adr[FIND_CONTOURS_NUM + 1]\
                                                __attribute__ ((section("Uncache_IRAM")));

contours_region_t                           g_contours_region_adr[FIND_CONTOURS_NUM + 1]\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_gaussian_blur_t                param_gaussian [R_DK2_TILE_NUM]\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_canny_calculate_t              param_canny_cal[3]\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_canny_hysterisis_t             param_canny_hyst\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_find_contours_t                param_find_contours\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_bayer2rgb_color_correction_t   param_bayer2rgb_color_correction\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_cropping_rgb_t                 param_cropping_rgb\
                                                __attribute__ ((section("Uncache_IRAM")));

static r_drp_resize_bilinear_fixed_rgb_t    param_resize_rgb\
                                                __attribute__ ((section("Uncache_IRAM")));

static uint8_t g_frame_RAM[DRP_WORK_RAM_SIZE_HEIGHT * DRP_WORK_RAM_SIZE_WIDTH * 5]\
                            __attribute__ ((section("ImageWork_RAM")));
static uint16_t app_data_cropping[4]        __attribute__ ((section("Uncache_IRAM"))) = {0x00, 0x00, 0x00, 0x00};

static void cb_drp_finish(uint8_t id);
static void write_canny_data(uint8_t *psrc, uint8_t *pdst);

static void bcd_drp_resize_contours(void);
static void bcd_drp_compress_contours(uint32_t length_min);
static void bcd_drp_stic_countours(uint32_t contours_num);
static uint32_t bcd_drp_sort_contours(void);
static uint32_t bcd_drp_check_empty_contours(uint32_t i);

/* Comparison */
static uint16_t bcd_drp_point_higer(uint16_t x, uint16_t y);
static uint16_t rcd_drp_point_lower(uint16_t x, uint16_t y);

static int8_t bcd_drp_check_length_contours(uint16_t x_length, uint16_t y_length, uint32_t length_min);
static uint8_t bcd_drp_check_crop_param(void);
static uint16_t bcd_drp_check_occupied_area_contours(uint32_t i);
static uint32_t bcd_drp_set_crop_param(uint32_t i);

static uint16_t bcd_drp_get_crop_length_longer(uint16_t x, uint16_t y);
static uint16_t bcd_drp_get_crop_length(uint16_t length);

/*******************************************************************************
 * Function Name: cb_drp_finish
 * Description  : This function is a callback function called from the
 *              : DRP driver at the finish of the DRP library processing.
 * Arguments    : id
 *              :   The ID of the DRP library that finished processing.
 * Return Value : -
 *******************************************************************************/
static void cb_drp_finish(uint8_t id)
{
    uint32_t tile_no;

    /* Change the operation state of the DRP library notified by the argument to finish */
    for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
    {
        if (drp_lib_id[tile_no] == id)
        {
            drp_lib_status[tile_no] = DRP_FINISH;
            break;
        }
        else
        {
            /* DO NOTHING */
        }
    }

    return;
}
/*******************************************************************************
 * End of function cb_drp_finish
 *******************************************************************************/

/******************************************************************************
 * Function Name: write_canny_data
 * Description  : Write Canny(Hysteresis) output result to the overlapping buffer
 * Arguments    : psrc
 *              :   Pointer of input image.
 *              : pdst
 *              :   Pointer of Output buffer.
 * Return Value : -
 ******************************************************************************/
static void write_canny_data(uint8_t *psrc, uint8_t *pdst)
{
    /* Cast to an appropriate type */
    uint32_t *p_src_w = (uint32_t *)psrc;

    /* Cast to an appropriate type */
    uint32_t *p_dst_h = (uint32_t *)pdst;
    uint32_t i;
    uint32_t j;

    /* Loop for one screen */
    for (i = 0; i < (R_BCD_CAMERA_HEIGHT / 2); i++)
    {
        for (j = 0; j < ((R_BCD_CAMERA_WIDTH / 2) / 4); j++)
        {
            /* Write patterns corresponding to the data output by the DRP application in the overlapping buffer */
            switch (*p_src_w)
            {
                case 0x000000FF:
                    p_dst_h[0]  = 0xFFFFFF55;
                    p_dst_h[80] = 0xFFFFFF55;
                    break;
                case 0x0000FF00:
                    p_dst_h[0]  = 0xFFFF55FF;
                    p_dst_h[80] = 0xFFFF55FF;
                    break;
                case 0x0000FFFF:
                    p_dst_h[0]  = 0xFFFF5555;
                    p_dst_h[80] = 0xFFFF5555;
                    break;
                case 0x00FF0000:
                    p_dst_h[0]  = 0xFF55FFFF;
                    p_dst_h[80] = 0xFF55FFFF;
                    break;
                case 0x00FF00FF:
                    p_dst_h[0]  = 0xFF55FF55;
                    p_dst_h[80] = 0xFF55FF55;
                    break;
                case 0x00FFFF00:
                    p_dst_h[0]  = 0xFF5555FF;
                    p_dst_h[80] = 0xFF5555FF;
                    break;
                case 0x00FFFFFF:
                    p_dst_h[0]  = 0xFF555555;
                    p_dst_h[80] = 0xFF555555;
                    break;
                case 0xFF000000:
                    p_dst_h[0]  = 0x55FFFFFF;
                    p_dst_h[80] = 0x55FFFFFF;
                    break;
                case 0xFF0000FF:
                    p_dst_h[0]  = 0x55FFFF55;
                    p_dst_h[80] = 0x55FFFF55;
                    break;
                case 0xFF00FF00:
                    p_dst_h[0]  = 0x55FF55FF;
                    p_dst_h[80] = 0x55FF55FF;
                    break;
                case 0xFF00FFFF:
                    p_dst_h[0]  = 0x55FF5555;
                    p_dst_h[80] = 0x55FF5555;
                    break;
                case 0xFFFF0000:
                    p_dst_h[0]  = 0x5555FFFF;
                    p_dst_h[80] = 0x5555FFFF;
                    break;
                case 0xFFFF00FF:
                    p_dst_h[0]  = 0x5555FF55;
                    p_dst_h[80] = 0x5555FF55;
                    break;
                case 0xFFFFFF00:
                    p_dst_h[0]  = 0x555555FF;
                    p_dst_h[80] = 0x555555FF;
                    break;
                case 0xFFFFFFFF:
                    p_dst_h[0]  = 0x55555555;
                    p_dst_h[80] = 0x55555555;
                    break;
                default : break;
            }

            p_src_w++;
            p_dst_h++;
        }
        p_dst_h = p_dst_h + 80;
    }
    return;
}
/*******************************************************************************
 * End of function write_canny_data
 *******************************************************************************/

/*******************************************************************************
 * Function Name: sample_main
 * Description  : First function called after initialization is completed
 * Arguments    : -
 * Return Value : -
 *******************************************************************************/
void sample_main(void) 
{
    int32_t frame_buf_id;
    uint8_t buf[RESULT_DISP_BUF_SIZE];
    int32_t ret_val;
    uint32_t us;
    int32_t i;

    uint8_t *p_input_bufadr;
    uint8_t *p_output_bufadr;

    uint32_t resize_width;
    uint32_t resize_height;

    uint8_t color;
    uint32_t x1 = 60;
    uint32_t y1 = 60;
    uint32_t length = 0;
    uint32_t height = 0;

    uint16_t brightness;
    r_bcd_ae_setting_t ae_setting;

    uint32_t key_status;
    uint32_t isp_ae_mode;

    uint32_t tile_no;
    uint32_t bufsize;

    uint8_t *p_frame_ram1;
    uint8_t *p_frame_ram2;
    uint8_t *p_frame_ram3;
    uint8_t *p_frame_ram4;
    uint8_t *p_frame_ram5;

    p_frame_ram1 = &g_frame_RAM[0];
    p_frame_ram2 = &g_frame_RAM[DRP_WORK_RAM_SIZE_HEIGHT * DRP_WORK_RAM_SIZE_WIDTH * 1];
    p_frame_ram3 = &g_frame_RAM[DRP_WORK_RAM_SIZE_HEIGHT * DRP_WORK_RAM_SIZE_WIDTH * 2];
    p_frame_ram4 = &g_frame_RAM[DRP_WORK_RAM_SIZE_HEIGHT * DRP_WORK_RAM_SIZE_WIDTH * 3];
    p_frame_ram5 = &g_frame_RAM[DRP_WORK_RAM_SIZE_HEIGHT * DRP_WORK_RAM_SIZE_WIDTH * 4];

    uint32_t scale_check_x = 0x10;
    uint32_t scale_check_y = 0x10;
    uint32_t x_axis_resize = 0x10;
    uint32_t y_axis_resize = 0x10;

    /**************/
    /* Initialize */
    /**************/

    /* VDC6 L2 Cache ON */
    /* Cast to an appropriate type */
    PRR.AXIBUSCTL6.BIT.VDC601ARCACHE = 0xF;
    /* Cast to an appropriate type */
    PRR.AXIBUSCTL6.BIT.VDC602ARCACHE = 0xF;
    /* Cast to an appropriate type */
    PRR.AXIBUSCTL7.BIT.VDC604ARCACHE = 0xF;

    /* DRP L2 Cache ON */
    /* Cast to an appropriate type */
    PRR.AXIBUSCTL4.BIT.DRPARCACHE = 0xF;
    /* Cast to an appropriate type */
    PRR.AXIBUSCTL4.BIT.DRPAWCACHE = 0xF;

    /* Initialization of VIN and MIPI driver */
    R_BCD_CameraInit();

    /* Initialization of LCD driver */
    R_BCD_LcdInit();

    /* SW3 Readout setting */
    /* Cast to an appropriate type */
    PORTJ.PMR.BIT.PMR1 = 1;
    /* Cast to an appropriate type */
    PORTJ.PMR.BIT.PMR1 = 0;
    /* Cast to an appropriate type */
    PORTJ.PDR.BIT.PDR1 = 2;
    /* Cast to an appropriate type */
    PORTJ.PIDR.BIT.PIDR1;   /* Dummy read */

    /* Key status initialize */
    key_status = KEY_STAUS_INIT;

    /* Initialize AE mode (=ON) */
    isp_ae_mode = ISP_AE_ON;

    /* Capture Start */
    R_BCD_CameraClearCaptureStatus();
    R_BCD_CameraCaptureStart();

    /* Initialize of Performance counter */
    PerformInit();

    /* Initialization of DRP driver */
    R_DK2_Initialize();

    /* Initialization of Simple ISP */
    R_BCD_AeInit(&ae_setting);
    R_BCD_AeMakeGammaTable(&ae_setting, GAMMA_VALUE, look_up_table);

    /*************/
    /* Main loop */
    /*************/
    while (1)
    {

        /* Get User Switch0 (SW2) key */
        key_status = (key_status << 1) | PORTD.PIDR.BIT.PIDR6;
        if ( ( key_status & KEY_CHECK_BIT) == KEY_JUST_ON )
        {
            /* Change AE mode On/Off */
            if ( isp_ae_mode == ISP_AE_OFF )
            {
                R_BCD_AeStart(&ae_setting);
                isp_ae_mode = ISP_AE_ON;
            }
            else
            {
                R_BCD_AeStop(&ae_setting);
                isp_ae_mode = ISP_AE_OFF;
            }
        }

        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 |IspBayer2Gray    | */
        /*        |                 | */
        /* tile 1 |                 | */
        /*        |                 | */
        /* tile 2 |                 | */
        /*        |                 | */
        /* tile 3 |                 | */
        /*        |                 | */
        /* tile 4 |                 | */
        /*        |                 | */
        /* tile 5 |                 | */
        /*        +-----------------+ */
        /******************************/
        /* Cast to an appropriate type */
        ret_val = R_DK2_Load(&g_drp_lib_isp_bayer2grayscale_6[0],
                R_DK2_TILE_0,
                /* Cast to an appropriate type */
                R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set start time of process*/
        PerformSetStartTime(0);

        /* Set ISP parameters */
        p_input_bufadr = R_BCD_CameraGetFrameAddress(frame_buf_id);
        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)p_input_bufadr, &(param_isp.src));
        /* Cast to an appropriate type */
        param_isp.dst = (uint32_t) p_frame_ram2;
        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)param_isp.dst, &(param_isp.dst));

        param_isp.width             = R_BCD_CAMERA_WIDTH;
        param_isp.height            = R_BCD_CAMERA_HEIGHT;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)look_up_table, &(param_isp.table));
        param_isp.gain_r            = ISP_GAIN_R;
        param_isp.gain_g            = ISP_GAIN_G;
        param_isp.gain_b            = ISP_GAIN_B;
        param_isp.gamma             = ISP_GAMMA;
        param_isp.component         = ISP_COMPONENT;
        param_isp.bias_r            = ISP_BIAS_R;
        param_isp.bias_g            = ISP_BIAS_G;
        param_isp.bias_b            = ISP_BIAS_B;
        param_isp.blend             = ISP_MEDIAN_A;
        param_isp.strength          = ISP_STRENGTH;
        param_isp.coring            = ISP_CORING;

        param_isp.area1_offset_x    = 0;
        param_isp.area1_offset_y    = 0;
        param_isp.area1_width       = R_BCD_CAMERA_WIDTH;
        param_isp.area1_height      = R_BCD_CAMERA_HEIGHT;
        param_isp.area2_offset_x    = 0;
        param_isp.area2_offset_y    = 0;
        param_isp.area2_width       = 0;
        param_isp.area2_height      = 0;
        param_isp.area3_offset_x    = 0;
        param_isp.area3_offset_y    = 0;
        param_isp.area3_width       = 0;
        param_isp.area3_height      = 0;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)ave_result, &(param_isp.accumulate));

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_isp, sizeof(r_drp_simple_isp_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set AE */
        if (isp_ae_mode == ISP_AE_ON)
        {
            /* Cast to an appropriate type */
            brightness = (uint16_t)(((ave_result[0] * 0.299 * 4) + (ave_result[1] * 0.587 * 2) + (ave_result[2] * 0.114 * 4))
                            / (R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT));
            R_BCD_AeRunAutoExpousure( &ae_setting, brightness);
        }

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /**************************/
        /* Load DRP Library       */
        /*        +-------------+ */
        /* tile 0 | Resize      | */
        /*        | Bilinear    | */
        /* tile 1 | Fixed       | */
        /*        |             | */
        /* tile 2 |             | */
        /*        |             | */
        /* tile 3 |             | */
        /*        +-------------+ */
        /* tile 4 | (not use)   | */
        /*        +-------------+ */
        /* tile 5 | (not use)   | */
        /*        +-------------+ */
        /**************************/
        ret_val = R_DK2_Load(&g_drp_lib_resize_bilinear_fixed[0],
                R_DK2_TILE_0 ,R_DK2_TILE_PATTERN_4_1_1,
                /* Cast to an appropriate type */
                NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);
        /************************
         * Activate DRP Library *
         ************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(1);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set the address of buffer to be read/write by DRP */
        /* Cast to an appropriate type */
        param_resize.src = (uint32_t)p_frame_ram2;

        /* Cast to an appropriate type */
        param_resize.dst = (uint32_t)p_frame_ram1;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)param_resize.src, &(param_resize.src));

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t)param_resize.dst, &(param_resize.dst));

        /* Set Image size */
        param_resize.src_width = R_BCD_CAMERA_WIDTH;
        param_resize.src_height = R_BCD_CAMERA_HEIGHT;

        /* Set magnification */
        param_resize.fx = DRP_RESIZE_HALF;
        param_resize.fy = DRP_RESIZE_HALF;

        /* Set Image size */
        resize_width = R_BCD_CAMERA_WIDTH / 2;
        resize_height = R_BCD_CAMERA_HEIGHT / 2;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_resize, sizeof(r_drp_resize_bilinear_fixed_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(1);

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /*****************************
         * Load DRP Library          *
         *        +----------------+ *
         * tile 0 | Gaussian Blur  | *
         *        +----------------+ *
         * tile 1 | Gaussian Blur  | *
         *        +----------------+ *
         * tile 2 | Gaussian Blur  | *
         *        +----------------+ *
         * tile 3 | Gaussian Blur  | *
         *        +----------------+ *
         * tile 4 | Gaussian Blur  | *
         *        +----------------+ *
         * tile 5 | Gaussian Blur  | *
         *        +----------------+ *
         *****************************/

        ret_val = R_DK2_Load( &g_drp_lib_gaussian_blur[0],
                R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5,
                /* Cast to an appropriate type */
                R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************
         * Activate DRP Library *
         ************************/
        ret_val = R_DK2_Activate(
                drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3] | drp_lib_id[TILE_4]
                | drp_lib_id[TILE_5], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);

        /*********************************************************
         * Set R_DK2_Start function parameters for Gaussian Blur *
         *********************************************************/

        /* Set the address of buffer to be read/write by DRP */
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
        {
            /* Set Image size */
            param_gaussian[tile_no].width = resize_width;
            param_gaussian[tile_no].height = resize_height / R_DK2_TILE_NUM;

            /* Set the address of buffer to be read/write by DRP */
            /* Cast to an appropriate type */
            param_gaussian[tile_no].src = (uint32_t) &p_frame_ram1[(param_gaussian[tile_no].width
                    * param_gaussian[tile_no].height * tile_no)];

            /* Cast to an appropriate type */
            param_gaussian[tile_no].dst = (uint32_t) &p_frame_ram5[(param_gaussian[tile_no].width
                    * param_gaussian[tile_no].height * tile_no)];

            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_gaussian[tile_no].src, &(param_gaussian[tile_no].src));

            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_gaussian[tile_no].dst, &(param_gaussian[tile_no].dst));

            /* Set whether to perform top or bottom edge border processing. */
            param_gaussian[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param_gaussian[tile_no].bottom = (tile_no == TILE_5) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP application */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************
             * Start DRP Library *
             *********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *) &param_gaussian[tile_no],
                    sizeof(r_drp_gaussian_blur_t));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************
         * Wait until DRP processing is finish *
         ***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_5; tile_no += 1)
        {
            /***************************************
             * Wait until DRP processing is finish *
             ***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH)
            {
                /* Spin here forever..*/
                ;
            }
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(2);

        /**********************
         * Unload DRP library *
         **********************/
        ret_val = R_DK2_Unload(
                drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3] | drp_lib_id[TILE_4]
                | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        resize_height -= 4; //

        /******************************
         * Load DRP Library           *
         *        +-----------------+ *
         * tile 0 |                 | *
         *        + Canny Calculate + *
         * tile 1 |                 | *
         *        +-----------------+ *
         * tile 2 |                 | *
         *        + Canny Calculate + *
         * tile 3 |                 | *
         *        +-----------------+ *
         * tile 4 |                 | *
         *        + Canny Calculate + *
         * tile 5 |                 | *
         *        +-----------------+ *
         ******************************/
        ret_val = R_DK2_Load( &g_drp_lib_canny_calculate[0],
                R_DK2_TILE_0 | R_DK2_TILE_2 | R_DK2_TILE_4,
                R_DK2_TILE_PATTERN_2_2_2, NULL, &cb_drp_finish, &drp_lib_id[0]); 
        DRP_DRV_ASSERT(ret_val);

        /********************
         * Activate DRP Lib *
         ********************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(3);

        /***************************************
         * Set R_DK2_Start function parameters *
         ***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /* Set Image size */
            param_canny_cal[tile_no / 2].width = resize_width;
            param_canny_cal[tile_no / 2].height = (resize_height / 3);

            /* Set Buffer size */
            bufsize = (uint32_t) (param_canny_cal[tile_no / 2].width * param_canny_cal[tile_no / 2].height);

            /* Set the address of buffer to be read/write by DRP */
            /* Cast to an appropriate type */
            param_canny_cal[tile_no / 2].src = (uint32_t) &p_frame_ram5[((tile_no / 2) * bufsize)];

            /* Cast to an appropriate type */
            param_canny_cal[tile_no / 2].dst = (uint32_t) &p_frame_ram4[((tile_no / 2) * bufsize)];

            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_canny_cal[tile_no / 2].src, &(param_canny_cal[tile_no / 2].src));

            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_canny_cal[tile_no / 2].dst, &(param_canny_cal[tile_no / 2].dst));

            /* Set additional information (currently no additional information) */
            /* Cast to an appropriate type */
            param_canny_cal[tile_no / 2].work =
                /* Cast to an appropriate type */
                (uint32_t) &p_frame_ram1[((tile_no / 2) * (bufsize + (resize_width * 2))) * 2];

            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_canny_cal[tile_no / 2].work, &(param_canny_cal[tile_no / 2].work));

            param_canny_cal[tile_no / 2].threshold_high = STRONG_LEVEL;
            param_canny_cal[tile_no / 2].threshold_low = WEAK_LEVEL;

            /* Set whether to perform top or bottom edge border processing. */
            param_canny_cal[tile_no / 2].top = (tile_no == TILE_0) ? 1 : 0;
            param_canny_cal[tile_no / 2].bottom = (tile_no == TILE_4) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************
             * Start DRP Library *
             *********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *) &param_canny_cal[tile_no / 2],
                    sizeof(r_drp_canny_calculate_t));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************
         * Wait until DRP processing is finish *
         ***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /***************************************
             * Wait until DRP processing is finish *
             ***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH)
            {
                /* Spin here forever..*/
                ;
            }
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(3);

        /**********************
         * Unload DRP library *
         **********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /*******************************
         * Load DRP Library            *
         *        +------------------+ *
         * tile 0 |                  | *
         *        +                  + *
         * tile 1 |                  | *
         *        +                  + *
         * tile 2 |                  | *
         *        + Canny Hysterisis + *
         * tile 3 |                  | *
         *        +                  + *
         * tile 4 |                  | *
         *        +                  + *
         * tile 5 |                  | *
         *        +------------------+ *
         *******************************/
        ret_val = R_DK2_Load( &g_drp_lib_canny_hysterisis[0],
                    R_DK2_TILE_0,
                    /* Cast to an appropriate type */
                    R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************
         * Activate DRP Library *
         ************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of CannyHyst process */
        PerformSetStartTime(4);

        /***************************************
         * Set R_DK2_Start function parameters *
         ***************************************/
        /* Set Image size */
        param_canny_hyst.width = resize_width;
        param_canny_hyst.height = resize_height;

        /* Set the address of buffer to be read/write by DRP */
        /* Cast to an appropriate type */
        param_canny_hyst.src = (uint32_t) p_frame_ram4;

        /* Cast to an appropriate type */
        param_canny_hyst.dst = (uint32_t) p_frame_ram5;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_canny_hyst.src, &(param_canny_hyst.src));

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_canny_hyst.dst, &(param_canny_hyst.dst));

        /* Set additional information (currently no additional information) */
        /* Cast to an appropriate type */
        param_canny_hyst.work = (uint32_t) p_frame_ram1;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_canny_hyst.work, &(param_canny_hyst.work));
        param_canny_hyst.iterations = HYSTERISIS_COUNT;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************
         * Start DRP Library *
         *********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *) &param_canny_hyst, sizeof(r_drp_canny_hysterisis_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************
         * Wait until DRP processing is finish *
         ***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(4);

        /**********************
         * Unload DRP Library *
         **********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /******************************
         * Load DRP Library           *
         *        +-----------------+ *
         * tile 0 |                 | *
         *        +  FindContours   + *
         * tile 1 |                 | *
         *        +-----------------+ *
         * tile 2 |                 | *
         *        +-----------------+ *
         * tile 3 |                 | *
         *        +-----------------+ *
         * tile 4 |                 | *
         *        +-----------------+ *
         * tile 5 |                 | *
         *        +-----------------+ *
         ******************************/
        ret_val = R_DK2_Load( &g_drp_lib_find_contours[0],
                    R_DK2_TILE_0,
                    /* Cast to an appropriate type */
                    R_DK2_TILE_PATTERN_2_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /********************
         * Activate DRP Lib *
         ********************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(5);

        /***************************************
         * Set R_DK2_Start function parameters *
         ***************************************/
        /* Cast to an appropriate type */
        param_find_contours.src = (uint32_t) p_frame_ram5;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_find_contours.src, &(param_find_contours.src));

        /* Cast to an appropriate type */
        param_find_contours.dst_region = (uint32_t) g_contours_region_adr;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_find_contours.dst_region, &(param_find_contours.dst_region));

        /* Set Image size */
        param_find_contours.width = resize_width;
        param_find_contours.height = resize_height;

        /* Set whether to perform top or bottom edge border processing. */
        /* Cast to an appropriate type */
        param_find_contours.work = (uint32_t) p_frame_ram1;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_find_contours.work, &(param_find_contours.work));

        /* Cast to an appropriate type */
        param_find_contours.dst_rect = (uint32_t) g_contours_rect_adr;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_find_contours.dst_rect, &(param_find_contours.dst_rect));

        /* Cast to an appropriate type */
        param_find_contours.dst_region = (uint32_t) g_contours_region_adr;

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) param_find_contours.dst_region, &(param_find_contours.dst_region));

        param_find_contours.dst_region_size = FIND_CONTOURS_NUM;
        param_find_contours.dst_rect_size = FIND_CONTOURS_NUM;
        param_find_contours.threshold_width = 10;
        param_find_contours.threshold_height = 10;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[0] = DRP_NOT_FINISH;

        /*********************
         * Start DRP Library *
         *********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *) &param_find_contours, sizeof(r_drp_find_contours_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************
         * Wait until DRP processing is finish *
         ***************************************/
        while (drp_lib_status[0] == DRP_NOT_FINISH)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(0);

        /* Set end time of Findcontours processing */
        PerformSetEndTime(5);

        /**********************
         * Unload DRP library *
         **********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

#if(1)
        /*After processing Application Find Contours*/
        bcd_drp_resize_contours();
        bcd_drp_compress_contours(R_BCD_DRP_PRESTIC_LENGTH_MIN);
        bcd_drp_stic_countours(bcd_drp_sort_contours());
        bcd_drp_compress_contours(R_BCD_DRP_CROP_LENGTH_MIN);
#endif

        /************************************/
        /* Load DRP Library                 */
        /*        +-----------------------+ */
        /* tile 0 |                       | */
        /*        |                       | */
        /* tile 1 |                       | */
        /*        |                       | */
        /* tile 2 |                       | */
        /*        |Bayer2 RgbWhiteBalance | */
        /* tile 3 |                       | */
        /*        |                       | */
        /* tile 4 |                       | */
        /*        |                       | */
        /* tile 5 |                       | */
        /*        +-----------------------+ */
        /************************************/
        ret_val = R_DK2_Load( &g_drp_lib_bayer2rgb_color_correction[0],
                    R_DK2_TILE_0,R_DK2_TILE_PATTERN_6,
                    /* Cast to an appropriate type */ 
                    NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************
         * Activate DRP Library *
         ************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(6);

        /* Start processing */
        p_output_bufadr = R_BCD_LcdGetVramAddress();

        /***************************************
         * Set R_DK2_Start function parameters *
         ***************************************/

        /* Set the address of buffer to be read/write by DRP */
        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) p_input_bufadr, &(param_bayer2rgb_color_correction.src));

        /* Cast to an appropriate type */
        R_MMU_VAtoPA((uint32_t) p_output_bufadr, &(param_bayer2rgb_color_correction.dst));

        /* Set Image size */
        param_bayer2rgb_color_correction.width = R_BCD_CAMERA_WIDTH;
        param_bayer2rgb_color_correction.height = R_BCD_CAMERA_HEIGHT;

        /* Set magnification */
        param_bayer2rgb_color_correction.gain_r = 0x148F;
        param_bayer2rgb_color_correction.gain_g = 0x1000;
        param_bayer2rgb_color_correction.gain_b = 0x1A72;
        param_bayer2rgb_color_correction.pattern = 0x0000;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************
         * Start DRP Library *
         *********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *) &param_bayer2rgb_color_correction,
                sizeof(r_drp_bayer2rgb_color_correction_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************
         * Wait until DRP processing is finish *
         ***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* Spin here forever..*/
            ;
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(6);

        /**********************
         * Unload DRP library *
         **********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        uint8_t  volatile loop_count = 0;
        uint32_t src_offset = 0;
        uint32_t dst_offset = 0;

        while((bcd_drp_set_crop_param(loop_count) != 0) && (loop_count < R_BCD_DRP_CROP_NUM))
        {
            /* Run Application Cropping */
            /*****************************/
            /* Load DRP Library          */
            /*        +----------------+ */
            /* tile 0 | cropping RGB   | */
            /*        +----------------+ */
            /* tile 1 |                | */
            /*        +----------------+ */
            /* tile 2 |                | */
            /*        +----------------+ */
            /* tile 3 |                | */
            /*        +----------------+ */
            /* tile 4 |                | */
            /*        +----------------+ */
            /* tile 5 |                | */
            /*        +----------------+ */
            /*****************************/

            ret_val = R_DK2_Load(&g_drp_lib_cropping_rgb[0],
                        R_DK2_TILE_0, R_DK2_TILE_PATTERN_1_1_1_1_1_1,
                        /* Cast to an appropriate type */
                        NULL,&cb_drp_finish, &drp_lib_id[0]);
            DRP_DRV_ASSERT(ret_val);

            /************************
             * Activate DRP Library *
             ************************/
            ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
            DRP_DRV_ASSERT(ret_val);

            /* Set start time of process*/
            PerformSetStartTime(7);

            /*********************************************************
             * Set R_DK2_Start function parameters for Cropping RGB *
             *********************************************************/
            /* Start processing */
            p_output_bufadr = R_BCD_LcdGetVramAddress();

            /* Set the address of buffer to be read/write by DRP */
            R_MMU_VAtoPA((uint32_t)p_output_bufadr, &(param_cropping_rgb.src));
            /* Cast to an appropriate type */
            param_cropping_rgb.dst = (uint32_t)p_frame_ram1;
            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t)param_cropping_rgb.dst, &(param_cropping_rgb.dst));

            /* Set Image size */
            param_cropping_rgb.src_width  = R_BCD_CAMERA_WIDTH;
            param_cropping_rgb.src_height = R_BCD_CAMERA_HEIGHT;

            param_cropping_rgb.offset_x  = g_contours_rect_adr[loop_count].x;
            param_cropping_rgb.offset_y  = g_contours_rect_adr[loop_count].y;

            /* Set Image size */
            param_cropping_rgb.dst_width  = g_contours_rect_adr[loop_count].width;
            param_cropping_rgb.dst_height = g_contours_rect_adr[loop_count].height;

            /* Initialize variables to be used in termination judgment of the DRP application */
            drp_lib_status[TILE_0] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *) &param_cropping_rgb, sizeof(r_drp_cropping_rgb_t));
            DRP_DRV_ASSERT(ret_val);

            /**************************************
              Wait until DRP processing is finish
             **************************************/
            while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
            {
                /* Spin here forever..*/
                ;
            }

            /* Set end time of DRP processing */
            PerformSetEndTime(7);

            /*********************
              Unload DRP library
             *********************/
            ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
            DRP_DRV_ASSERT(ret_val);
            R_CACHE_L1DataCleanInvalidLine(p_frame_ram1,
            g_contours_rect_adr[loop_count].width * g_contours_rect_adr[loop_count].height * 3);

            scale_check_x = g_contours_rect_adr[loop_count].width;
            switch(scale_check_x)
            {
                case(32):
                    x_axis_resize = 0x08;
                    goto x_axis;
                case(64):
                    x_axis_resize = 0x10;
                    goto x_axis;
                case(128):
                    x_axis_resize = 0x20;
                    goto x_axis;
                case(256):
                    x_axis_resize = 0x40;
                    goto x_axis;
                default:
                    x_axis_resize = 0x10;
            }
x_axis:

            scale_check_y = g_contours_rect_adr[loop_count].height; 
            switch(scale_check_y)
            {
                case(32):
                    y_axis_resize = 0x08;
                    goto y_axis;
                case(64):
                    y_axis_resize = 0x10;
                    goto y_axis;
                case(128):
                    y_axis_resize = 0x20;
                    goto y_axis;
                case(256):
                    y_axis_resize = 0x40;
                    goto y_axis;
                default:
                    y_axis_resize = 0x10;
            }
y_axis:

            /************************************/
            /* Load DRP Library                 */
            /*        +-----------------------+ */
            /* tile 0 |                       | */
            /*        |                       | */
            /* tile 1 |                       | */
            /*        |                       | */
            /* tile 2 |                       | */
            /*        |ResizeBilinearFixedRgb | */
            /* tile 3 |                       | */
            /*        |                       | */
            /* tile 4 |                       | */
            /*        |                       | */
            /* tile 5 |                       | */
            /*        +-----------------------+ */
            /************************************/
            ret_val = R_DK2_Load( &g_drp_lib_resize_bilinear_fixed_rgb[0],
                        R_DK2_TILE_0, R_DK2_TILE_PATTERN_6,
                        /* Cast to an appropriate type */
                        NULL, &cb_drp_finish, &drp_lib_id[0]);
            DRP_DRV_ASSERT(ret_val);
            /***********************
              Activate DRP Library
             ***********************/
            ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
            DRP_DRV_ASSERT(ret_val);

            /**************************************
              Set R_DK2_Start function parameters
             **************************************/

            /* Set the address of buffer to be read/write by DRP */
            param_resize_rgb.src = (uint32_t)p_frame_ram1;
            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_resize_rgb.src, &(param_resize_rgb.src));
            /* Cast to an appropriate type */
            param_resize_rgb.dst = (uint32_t)&p_frame_ram4[dst_offset];
            /* Cast to an appropriate type */
            R_MMU_VAtoPA((uint32_t) param_resize_rgb.dst, &(param_resize_rgb.dst));

            /* Set Image size */
            param_resize_rgb.src_width  = g_contours_rect_adr[loop_count].width;
            param_resize_rgb.src_height = g_contours_rect_adr[loop_count].height;

            param_resize_rgb.fx = x_axis_resize;
            param_resize_rgb.fy = y_axis_resize;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[TILE_0] = DRP_NOT_FINISH;

            /********************
              Start DRP Library
             ********************/
            ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *) &param_resize_rgb,
                    sizeof(r_drp_resize_bilinear_fixed_rgb_t));
            DRP_DRV_ASSERT(ret_val);

            /**************************************
              Wait until DRP processing is finish
             **************************************/
            while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
            {
                /* Spin here forever..*/
                ;
            }

            /*********************
              Unload DRP library
             *********************/
            ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
            DRP_DRV_ASSERT(ret_val);

            R_CACHE_L1DataCleanInvalidLine(&p_frame_ram4[dst_offset],
                g_contours_rect_adr[loop_count].width *g_contours_rect_adr[loop_count].height * 3);

            loop_count++;
            dst_offset += (64*64*3);
        }

        for(i = (R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT) -1; i > 0; i-- )
        {
            uint8_t temp_r = p_output_bufadr[(i * 3) + 0];
            uint8_t temp_g = p_output_bufadr[(i * 3) + 1];
            uint8_t temp_b = p_output_bufadr[(i * 3) + 2];

            p_output_bufadr[(i * 4) + 0] = 0;
            p_output_bufadr[(i * 4) + 1] = temp_r;
            p_output_bufadr[(i * 4) + 2] = temp_g;
            p_output_bufadr[(i * 4) + 3] = temp_b;
        }

        uint32_t frame_size = R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT;
        dst_offset = 0;

        uint8_t x;
        uint8_t y;

        for(i = 0; i < loop_count; i++)
        {
            src_offset = (64*64) * i;
            dst_offset = frame_size + (64 * i);

            for(y = 0; y < 64; y++)
            {
                for(x = 0; x <64; x++)
                {
                    uint8_t temp_r = p_frame_ram4[((src_offset + (y * 64) + x) * 3)  + 0];
                    uint8_t temp_g = p_frame_ram4[((src_offset + (y * 64) + x) * 3)  + 1];
                    uint8_t temp_b = p_frame_ram4[((src_offset + (y * 64) + x) * 3)  + 2];

                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 0] = 0;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 1] = temp_r;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 2] = temp_g;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 3] = temp_b;
                }
            }
        }

        for(i = loop_count; i < 10; i++)
        {
            dst_offset = frame_size + (64 * i);

            for(y=0; y<64; y++)
            {
                for(x=0; x<64; x++)
                {

                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 0] = 0;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 1] = 0;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 2] = 0;
                    p_output_bufadr[((dst_offset +(y * R_BCD_CAMERA_WIDTH) + x) * 4) + 3] = 0;
                }
            }
        }

proccesing_end:

        /* Write the data(buf) on the cache to physical memory */
        R_CACHE_L1DataCleanLine(R_BCD_LcdGetVramAddress(), 
            ((R_BCD_LCD_WIDTH * (R_BCD_LCD_HEIGHT + 64)) *4 ));
        R_BCD_LcdSwapVideoBuffer();

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Clear Graphics buffer*/
        R_BCD_LcdClearGraphicsBuffer();

        color = WHITE;
        for (tile_no = 0; tile_no <= loop_count; tile_no++)
        {
            x1 = g_contours_rect_adr[tile_no].x;
            y1 = g_contours_rect_adr[tile_no].y;
            length = g_contours_rect_adr[tile_no].width;
            height = g_contours_rect_adr[tile_no].height;

            R_BCD_LcdWriteLine(x1           , y1           , (x1 + length), y1           , color);
            R_BCD_LcdWriteLine((x1 + length), y1           , (x1 + length), (y1 + height), color);
            R_BCD_LcdWriteLine(x1           , (y1 + height), (x1 + length), (y1 + height), color);
            R_BCD_LcdWriteLine(x1           , y1           , x1           , (y1 + height), color);
        }

        if (isp_ae_mode == ISP_AE_ON)
        {
            /* Cast to an appropriate type */
            sprintf((char *) &buf[0], "AE ON");
        }
        else
        {
            /* Cast to an appropriate type */
            sprintf((char *) &buf[0], "AE OFF");
        }
        R_BCD_LcdWriteString(&buf[0], 2, 38, WHITE);

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Clear the cache to read the frame buffer in physical memory */
        /* Cast to an appropriate type */
        R_CACHE_L1DataCleanInvalidLine((void *) &buf[0], (uint32_t) (RESULT_DISP_BUF_SIZE));
                /* Cast to an appropriate type */
        R_CACHE_L1DataCleanInvalidLine((void *) &p_frame_ram5[0],
                /* Cast to an appropriate type */
                (uint32_t) (DRP_WORK_RAM_SIZE_WIDTH * DRP_WORK_RAM_SIZE_HEIGHT));

        /* Write the canny's result into the graphics buffer */
        /* Cast to an appropriate type */
        write_canny_data((uint8_t *) p_frame_ram5, R_BCD_LcdGetOLVramAddress());

        us = PerformGetElapsedTime_us(0);

        /* Cast to an appropriate type */
        WCHAR(buf, "Detection :", (int32_t )us);
        R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);

        /* Write the data(buf) on the cache to physical memory */
        R_CACHE_L1DataCleanLine(R_BCD_LcdGetOLVramAddress(), ((R_BCD_LY2_WIDTH * R_BCD_LY2_HEIGHT) / 2));
    }

sample_finish:

return; 
}

/*******************************************************************************
 * End of function sample_main
 *******************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_resize_contours
 * Description  : Resize output contours fit Camera size
 * Arguments    : -
 * Return Value : -
 ******************************************************************************/
static void bcd_drp_resize_contours (void)
{
    uint32_t i = 0;

    while (bcd_drp_check_empty_contours(i))
    {
        g_contours_rect_adr[i].x = (g_contours_rect_adr[i].x * R_BCD_DRP_IMAGE_RESIZE);
        g_contours_rect_adr[i].y = (g_contours_rect_adr[i].y * R_BCD_DRP_IMAGE_RESIZE);
        g_contours_rect_adr[i].width = (g_contours_rect_adr[i].width * R_BCD_DRP_IMAGE_RESIZE)
            + (R_BCD_DRP_IMAGE_RESIZE - 1);
        g_contours_rect_adr[i].height = (g_contours_rect_adr[i].height * R_BCD_DRP_IMAGE_RESIZE)
            + (R_BCD_DRP_IMAGE_RESIZE - 1);
        g_contours_rect_adr[i].num = (g_contours_rect_adr[i].num * R_BCD_DRP_IMAGE_RESIZE * R_BCD_DRP_IMAGE_RESIZE);
        i++;
    }
}
/******************************************************************************
 * Function Name: bcd_drp_compress_contours
 * Description  : Compress output contours
 * Arguments    : length_min
 *              :
 * Return Value : -
 ******************************************************************************/
static void bcd_drp_compress_contours(uint32_t length_min)
{

    uint32_t i;
    uint32_t data_num;

    /* Check length */
    {
        i = 0;
        data_num = 0;

        while (1)
        {
            if (bcd_drp_check_length_contours(g_contours_rect_adr[i].width, g_contours_rect_adr[i].height, length_min))
            {
                if ((data_num != i) && (g_contours_rect_adr[i].num != 0))
                {
                    memcpy( &g_contours_rect_adr[data_num], &g_contours_rect_adr[i], sizeof(contours_rect_t));
                    memset( &g_contours_rect_adr[i], 0, sizeof(contours_rect_t));
                }
                else
                {
                    /* DO NOTHING */
                }
                data_num++;

            }
            else if (bcd_drp_check_empty_contours(i))
            {
                memset( &g_contours_rect_adr[i], 0, sizeof(contours_rect_t));
            }
            else
            {
                memset( &g_contours_rect_adr[i], 0, sizeof(contours_rect_t));
                break;
            }
            i++;
        }
    }

    uint32_t write_point;
    uint32_t j;

    /* check dual */
    {
        write_point = 1;
        j = 0;
        i = 1;

        while (i < data_num)
        {
            while (i > j)
            {
                int32_t x1_diff = g_contours_rect_adr[i].x - g_contours_rect_adr[j].x;
                int32_t y1_diff = g_contours_rect_adr[i].y - g_contours_rect_adr[j].y;
                int32_t x2_diff = (g_contours_rect_adr[i].x + g_contours_rect_adr[i].width)
                    - (g_contours_rect_adr[j].x + g_contours_rect_adr[j].width);
                int32_t y2_diff = (g_contours_rect_adr[i].y + g_contours_rect_adr[i].height)
                    - (g_contours_rect_adr[j].y + g_contours_rect_adr[j].height);
                int32_t max_diff = R_BCD_DRP_CROP_LENGTH_DIFF;

                if (((( -max_diff) < x1_diff) && (x1_diff < max_diff))
                        && ((( -max_diff) < y1_diff) && (y1_diff < max_diff))
                        && ((( -max_diff) < x2_diff) && (x2_diff < max_diff))
                        && ((( -max_diff) < y2_diff) && (y2_diff < max_diff)))
                {
                    break;
                }
                else
                {
                    j++;
                    continue;
                }

            }

            if (i == j)
            {
                if (write_point != i)
                {
                    memcpy( &g_contours_rect_adr[write_point], &g_contours_rect_adr[i], sizeof(contours_rect_t));
                }
                else
                {
                    /* DO NOTHING */
                }
                write_point++;
            }
            else
            {
                /* DO NOTHING */
            }
            i++;
            j = 0;
        }

        while (write_point < data_num)
        {
            memset( &g_contours_rect_adr[write_point], 0, sizeof(contours_rect_t));
            write_point++;
        }
    }

    return;
}
/******************************************************************************
 * Function Name: bcd_drp_sort_contours
 * Description  : sort output contours
 * Arguments    : -
 * Return Value : Number of contours
 ******************************************************************************/
static uint32_t bcd_drp_sort_contours(void)
{
    uint32_t i;
    uint32_t j;
    contours_rect_t buff;

    i = 0;
    j = 0;

    while(bcd_drp_check_empty_contours(i))
    {
        j = i + 1;
        while(bcd_drp_check_empty_contours(j))
        {
            if(i == j)
            {
                j++;
                continue;
            }

#if defined(R_BCD_DRP_CONTOUR_COMPARE_OCCUPIED_AREA)
            if(bcd_drp_check_occupied_area_contours(i) < bcd_drp_check_occupied_area_contours(j))
#elif defined(R_BCD_DRP_CONTOUR_COMPARE_AREA_SIZE)
                if(R_BCD_DrpCheckAreaSizeContours(i) < R_BCD_DrpCheckAreaSizeContours(j))
#else
                    break;
#endif
            {
                buff = g_contours_rect_adr[i];
                g_contours_rect_adr[i] = g_contours_rect_adr[j];
                g_contours_rect_adr[j] = buff;
            }
            else
            {
                /* DO NOTHING */
            }
            j++;
        }
        i++;
    }
    return i;
}

/******************************************************************************
 * Function Name: bcd_drp_stic_countours
 * Description  : Union output contours
 * Arguments    : contours_num
 *              :  number of contours in output contours buffer
 * Return Value : -
 ******************************************************************************/
static void bcd_drp_stic_countours(uint32_t contours_num)
{
    uint32_t i;
    uint32_t j;
    uint32_t flag = 1;
    contours_rect_t buff;

    /*data sort*/
    i = 0;

    while(flag)
    {
        flag = 0;
        while(bcd_drp_check_empty_contours(i))
        {
            j = i + 1;

            while(bcd_drp_check_empty_contours(j))
            {

                if(contours_num > 400)
                {
                    return;
                }
                else
                {
                    /* DO NOTHING */
                }

                if(j <= i)
                {
                    j = i + 1;
                    continue;
                }
                else
                {
                    /* DO NOTHING */
                }

                /*create combined data of contours*/
                buff.x = rcd_drp_point_lower(g_contours_rect_adr[i].x, g_contours_rect_adr[j].x);
                buff.y = rcd_drp_point_lower(g_contours_rect_adr[i].y, g_contours_rect_adr[j].y);
                buff.width = bcd_drp_point_higer
                    (g_contours_rect_adr[i].x + g_contours_rect_adr[i].width,
                    g_contours_rect_adr[j].x + g_contours_rect_adr[j].width)
                    - buff.x;
                buff.height = bcd_drp_point_higer
                    (g_contours_rect_adr[i].y + g_contours_rect_adr[i].height,
                    g_contours_rect_adr[j].y + g_contours_rect_adr[j].height)
                    - buff.y;
                buff.num = g_contours_rect_adr[i].num + g_contours_rect_adr[j].num;

                if(bcd_drp_check_length_contours(buff.width, buff.height, R_BCD_DRP_CROP_LENGTH_MIN))
                {
                    g_contours_rect_adr[i].width = buff.width;
                    g_contours_rect_adr[i].x  = buff.x;
                    g_contours_rect_adr[i].height = buff.height;
                    g_contours_rect_adr[i].y  = buff.y;
                    g_contours_rect_adr[i].num = buff.num;
                    g_contours_rect_adr[i].addr = 0xdeadbeaf;

                    contours_num--;
                    g_contours_rect_adr[j].width = g_contours_rect_adr[contours_num].width;
                    g_contours_rect_adr[j].x  = g_contours_rect_adr[contours_num].x;
                    g_contours_rect_adr[j].height = g_contours_rect_adr[contours_num].height;
                    g_contours_rect_adr[j].y  = g_contours_rect_adr[contours_num].y;
                    g_contours_rect_adr[j].num = g_contours_rect_adr[contours_num].num;
                    g_contours_rect_adr[j].addr = g_contours_rect_adr[contours_num].addr;

                    g_contours_rect_adr[contours_num].width = 0;
                    g_contours_rect_adr[contours_num].x  = 0;
                    g_contours_rect_adr[contours_num].height = 0;
                    g_contours_rect_adr[contours_num].y  = 0;
                    g_contours_rect_adr[contours_num].num = 0;
                    g_contours_rect_adr[contours_num].addr = 0;
                    flag = 1;
                    continue;
                }
                else
                {
                    /* DO NOTHING */
                }
                j++;

            }

            i++;

        }
    }

    return;
}
/**********************************************************************************************************************
 End of function bcd_drp_stic_countours
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_check_occupied_area_contours
 * Description  : Check the priority of contour
 * Arguments    : i
 *              :  number of output contour
 * Return Value : the point of priority
 ******************************************************************************/
static uint16_t bcd_drp_check_occupied_area_contours(uint32_t i)
{
    uint16_t occupied_area;
    occupied_area = ((g_contours_rect_adr[i].x * g_contours_rect_adr[i].y)
            / g_contours_rect_adr[i].num);

    return occupied_area;
}
/**********************************************************************************************************************
 End of function bcd_drp_check_occupied_area_contours
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_set_crop_param
 * Description  : Set parameter for Crop process
 * Arguments    : i
 *              :  number of output contour
 * Return Value : -
 ******************************************************************************/
static uint32_t bcd_drp_set_crop_param(uint32_t i)
{
    if((g_contours_rect_adr[i].width == 0) || (g_contours_rect_adr[i].height == 0))
    {
        return 0;
    }
    else
    {
        /* DO NOTHING */
    }

#if defined(R_BCD_DRP_CROP_SQUARE)
    app_data_cropping[2] = bcd_drp_get_crop_length_longer(g_contours_rect_adr[i].width, g_contours_rect_adr[i].height);
    app_data_cropping[3] = bcd_drp_get_crop_length_longer(g_contours_rect_adr[i].width, g_contours_rect_adr[i].height);
#else
    app_data_cropping[2] = bcd_drp_get_crop_length(g_contours_rect_adr[i].width);
    app_data_cropping[3] = bcd_drp_get_crop_length(g_contours_rect_adr[i].height);
#endif
    int32_t x_point = g_contours_rect_adr[i].x - ((app_data_cropping[2] - g_contours_rect_adr[i].width) / 2);
    int32_t y_point = g_contours_rect_adr[i].y - ((app_data_cropping[3] - g_contours_rect_adr[i].height) / 2);

    if(x_point <= 0)
    {
        app_data_cropping[0] = 0;
    }
    else if((x_point + app_data_cropping[2]) >= R_BCD_CAMERA_WIDTH)
    {
        app_data_cropping[0] = (R_BCD_CAMERA_WIDTH - 1) - app_data_cropping[2];
    }
    else
    {
        app_data_cropping[0] = x_point;
    }

    if((y_point <= 0))
    {
        app_data_cropping[1] = 0;
    }
    else if(((y_point + app_data_cropping[3]) >= R_BCD_CAMERA_HEIGHT))
    {
        app_data_cropping[1] = (R_BCD_CAMERA_HEIGHT - 1) - app_data_cropping[3];
    }
    else
    {
        app_data_cropping[1] = y_point;
    }

#if defined(R_BCD_DRP_COUNTOURS_TO_LCD)
    g_contours_rect_adr[i].x = app_data_cropping[0];
    g_contours_rect_adr[i].y = app_data_cropping[1];
    g_contours_rect_adr[i].width = app_data_cropping[2];
    g_contours_rect_adr[i].height = app_data_cropping[3];
#endif

    return 1;
}
/**********************************************************************************************************************
 End of function bcd_drp_set_crop_param
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_check_empty_contours
 * Description  :
 * Arguments    : -
 * Return Value : -
 ******************************************************************************/
static uint32_t bcd_drp_check_empty_contours(uint32_t i)
{

    if(g_contours_rect_adr[i].x)
    {
        return 1;
    }
    else if(g_contours_rect_adr[i].y)
    {
        return 1;
    }
    else if(g_contours_rect_adr[i].width)
    {
        return 1;
    }
    else if(g_contours_rect_adr[i].height)
    {
        return 1;
    }
    else if(g_contours_rect_adr[i].num)
    {
        return 1;
    }
    else if(g_contours_rect_adr[i].addr)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/**********************************************************************************************************************
 End of function bcd_drp_check_empty_contours
 *********************************************************************************************************************/


/******************************************************************************
 * Function Name: bcd_drp_check_length_contours
 * Description  : Check if the contour is necessary
 * Arguments    : x_length
 *              :  contour width
 *              : y_length
 *              :  contour height
 *              : length_min
 *              :  the minimum length of contour
 * Return Value : 0 : Not need
 *              : 1 : Need
 ******************************************************************************/
static int8_t bcd_drp_check_length_contours(uint16_t x_length,uint16_t y_length,uint32_t length_min)
{ 
    if((x_length > R_BCD_DRP_CROP_LENGTH_MAX) || (y_length > R_BCD_DRP_CROP_LENGTH_MAX))
    {
        return 0;
    }
    else
    {
        /* DO NOTHING */
    }


    if((x_length < length_min) && (y_length < length_min))
    {
        return 0;
    }
    else
    {
        /* DO NOTHING */
    }
    return 1;
}
/**********************************************************************************************************************
 End of function bcd_drp_check_length_contours
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_point_higer
 * Description  : return grater number x or y
 * Arguments    : x
 *              :  number
 *              : y
 *              :  number
 * Return Value : grater number
 ******************************************************************************/
static uint16_t bcd_drp_point_higer(uint16_t x, uint16_t y)
{
    if(x > y)
    {
        return x;
    }
    else
    {
        return y;
    }

}
/**********************************************************************************************************************
 End of function bcd_drp_point_higer
 *********************************************************************************************************************/


/******************************************************************************
 * Function Name: rcd_drp_point_lower
 * Description  : return lower number x or y
 * Arguments    : x
 *              :  number
 *              : y
 *              :  number
 * Return Value : lower number
 ******************************************************************************/
static uint16_t rcd_drp_point_lower(uint16_t x, uint16_t y)
{
    if(x < y)
    {
        return x;
    }
    else
    {
        return y;
    }
}
/**********************************************************************************************************************
 End of function rcd_drp_point_lower
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_check_crop_param
 * Description  : Check if Resize Application is necessary
 * Arguments    : -
 * Return Value : 1 : Skip Resize
 *              : 0 : Need Resize
 ******************************************************************************/
static uint8_t bcd_drp_check_crop_param(void)
{
    if((app_data_cropping[2] == R_BCD_CROP_INPUT_X) && (app_data_cropping[3] == R_BCD_CROP_INPUT_Y))
    {
        return 1;
    }
    else
    {
        return 0;
    }

    return 2;
}
/**********************************************************************************************************************
 End of function bcd_drp_check_crop_param
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_get_crop_length_longer
 * Description  : Return longer crop length between x and y
 * Arguments    : x
 *              :  contour width
 *              : y
 *              :  contour height
 * Return Value : -
 ******************************************************************************/
static uint16_t bcd_drp_get_crop_length_longer(uint16_t x, uint16_t y)
{

    if(x > y)
    {
        return bcd_drp_get_crop_length(x);
    }
    else
    {
        return bcd_drp_get_crop_length(y);
    }
}
/**********************************************************************************************************************
 End of function bcd_drp_get_crop_length_longer
 *********************************************************************************************************************/

/******************************************************************************
 * Function Name: bcd_drp_get_crop_length
 * Description  : Return crop length
 * Arguments    : length
 *              :  contour length
 * Return Value : length fitting
 ******************************************************************************/
static uint16_t bcd_drp_get_crop_length(uint16_t length)
{
    if(length <= 32)
    {
        return 32;
    }
    else if(length <= 64)
    {
        return 64;
    }
    else if(length <= 128)
    {
        return 128;
    }
    else if(length <= 256)
    {
        return 256;
    }
    else
    {
        return 0;
    }
}
/**********************************************************************************************************************
 * End of function bcd_drp_get_crop_length
 *********************************************************************************************************************/

/* End of File */
