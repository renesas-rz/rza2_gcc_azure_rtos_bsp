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
* Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/

/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include <stdio.h>
#include <fcntl.h>

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_mmu_lld_rza2m.h"
#include "r_cache_lld_rza2m.h"

#include "r_bcd_camera.h"
#include "r_bcd_lcd.h"
#include "r_bcd_ae.h"
#include "draw.h"
#include "perform.h"

#include "r_dk2_if.h"
#include "r_drp_simple_isp_colcal_3dnr.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#define TILE_0          (0)
#define TILE_1          (1)
#define TILE_2          (2)
#define TILE_3          (3)
#define TILE_4          (4)
#define TILE_5          (5)

#define DRP_NOT_FINISH  (0)
#define DRP_FINISH      (1)

#define DRP_DRV_ASSERT(x) if ((x) != 0) goto sample_finish      /* Any type can be used as it compares with 0 */

/* color mode */
#define COLOR_MODE_OFF          (0)
#define COLOR_MODE_BIASGAIN     (1)
#define COLOR_MODE_ON_STD       (2)
#define COLOR_MODE_ON_SEPIA     (3)
#define COLOR_MODE_MAX          (3)

/* color parameter */
#define COLOR_PARAM_BIAS_R      (0)
#define COLOR_PARAM_BIAS_G      (1)
#define COLOR_PARAM_BIAS_B      (2)
#define COLOR_PARAM_GAIN_R      (3)
#define COLOR_PARAM_GAIN_G      (4)
#define COLOR_PARAM_GAIN_B      (5)
#define COLOR_PARAM_GAIN_C11    (6)
#define COLOR_PARAM_GAIN_C12    (7)
#define COLOR_PARAM_GAIN_C13    (8)
#define COLOR_PARAM_GAIN_C21    (9)
#define COLOR_PARAM_GAIN_C22    (10)
#define COLOR_PARAM_GAIN_C23    (11)
#define COLOR_PARAM_GAIN_C31    (12)
#define COLOR_PARAM_GAIN_C32    (13)
#define COLOR_PARAM_GAIN_C33    (14)

/* 3DNR mode */
#define NR_MODE_OFF             (0)
#define NR_MODE_ON              (1)

#define ISP_3DNR_Y_COEF         (64)
#define ISP_3DNR_C_COEF         (32)
#define ISP_3DNR_Y_ALPHA_MAX    (128)
#define ISP_3DNR_Y_THRESH_A     (8)
#define ISP_3DNR_Y_THRESH_B     (16)
#define ISP_3DNR_Y_TILT         (512)
#define ISP_3DNR_C_ALPHA_MAX    (128)
#define ISP_3DNR_C_THRESH_A     (8)
#define ISP_3DNR_C_THRESH_B     (16)
#define ISP_3DNR_C_TILT         (512)

/* default camera gain */
#define CAMERA_GAIN_DEFAULT     (14)    /* 7dB */

/* Key status */
#define KEY_STAUS_INIT  (0x00000000U)
#define KEY_CHECK_BIT   (0x00000003U)
#define KEY_JUST_ON     (0x00000002U)
#define KEY_ON_KEEP     (0x00000000U)

/**********************************************************************************************************************
Imported global variables and functions (from other files)
**********************************************************************************************************************/
extern uint8_t g_drp_simple_isp_colcal_3dnr_6[];

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = {DRP_NOT_FINISH};
static r_drp_simple_isp_colcal_3dnr_t param __attribute__ ((section("UNCACHED_BSS")));

static uint8_t  look_up_table[256] __attribute__ ((section("UNCACHED_BSS")));
static uint32_t ave_result[9] __attribute__ ((section("UNCACHED_BSS")));

/* color mode */
static uint8_t color_mode;

/* 3DNR mode */
static uint8_t nr_mode;

/* Simple ISP parameter table */
static uint16_t const correct_data[4][15] = 
{
    /* Through Color */
    {
        /* R/G/B bias table */
        0x0000, 0x0000, 0x0000,
        /* R/G/B gain table */
        0x1000, 0x1000, 0x1000,
        /* Color matrix(=Through) table */
        0x2000, 0x0000, 0x0000,
        0x0000, 0x2000, 0x0000,
        0x0000, 0x0000, 0x2000
    },
    /* Standard color */
    {
        /* R/G/B bias table */
        0x00F0, 0x00F0, 0x00F0,
        /* R/G/B gain table */
        0x2480, 0x1800, 0x275b,
        /* Color matrix(=Through) table */
        0x2000, 0x0000, 0x0000,
        0x0000, 0x2000, 0x0000,
        0x0000, 0x0000, 0x2000
    },
    /* Natural color */
    {
        /* R/G/B bias table */
        0x00F0, 0x00F0, 0x00F0,
        /* R/G/B gain table */
        0x2480, 0x1800, 0x275b,
        /* Color matrix table */
        0x25dd, 0xfef0, 0xf79a,
        0xfd6a, 0x1ef4, 0xfdd5,
        0x029e, 0xecf0, 0x2c0d
    },
    /* Sepia color */
    {
        /* R/G/B bias table */
        0x00F0, 0x00F0, 0x00F0,
        /* R/G/B gain table */
        0x255d, 0x1800, 0x27bc,
        /* Color matrix table */
        0x0946, 0x15a7, 0x04ce,
        0x0512, 0x1af2, 0xff8f,
        0xff7d, 0x1bfa, 0xfc78
    }
};

/* key status */
static uint32_t key3_status;
static uint32_t key4_status;

static r_bcd_ae_setting_t ae_setting;

static void cb_drp_finish(uint8_t id);
static void display_data( void );

/**********************************************************************************************************************
* Function Name: cb_drp_finish
* Description  : This function is a callback function called from the
*              : DRP driver at the finish of the DRP library processing.
* Arguments    : id
*              :   The ID of the DRP library that finished processing.
* Return Value : -
**********************************************************************************************************************/
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
    }

    return;
}
/**********************************************************************************************************************
* End of function cb_drp_finish
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: sample_main
* Description  : First function called after initialization is completed
* Arguments    : -
* Return Value : -
**********************************************************************************************************************/
void sample_main(void)
{
    int32_t frame_buf_id;
    int32_t ret_val;
    uint8_t * p_input_bufadr;
    uint8_t * p_output_bufadr;

    /* VDC6 L2 Cache ON */
    PRR.AXIBUSCTL6.BIT.VDC601ARCACHE = 0xF;
    PRR.AXIBUSCTL6.BIT.VDC602ARCACHE = 0xF;
    PRR.AXIBUSCTL7.BIT.VDC604ARCACHE = 0xF;

    /* DRP L2 Cache ON */
    PRR.AXIBUSCTL4.BIT.DRPARCACHE = 0xF;
    PRR.AXIBUSCTL4.BIT.DRPAWCACHE = 0xF;

    /* Initialization of VIN and MIPI driver */
    R_BCD_CameraInit();

    /* Initialization of LCD driver */
    R_BCD_LcdInit();

    /* Set SW4 readable */
    PORT5.PMR.BIT.PMR6 = 0;
    PORT5.PDR.BIT.PDR6 = 2;

    /* Capture Start */
    R_BCD_CameraCaptureStart();

    /* Initialize of Performance counter */
    PerformInit();

    /********************************/
    /* Initialization of DRP driver */
    /********************************/
    R_DK2_Initialize();

    R_BCD_AeInit(&ae_setting);

    /* Gamma Table */
    R_BCD_AeMakeGammaTable(&ae_setting, (double)1.2, look_up_table);

    /******************************/
    /* Load DRP Library           */
    /*        +-----------------+ */
    /* tile 0 |IspLibrary       | */
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
    ret_val = R_DK2_Load(&g_drp_simple_isp_colcal_3dnr_6[0], R_DK2_TILE_0, R_DK2_TILE_PATTERN_6, NULL,
                        &cb_drp_finish, &drp_lib_id[0]);
    DRP_DRV_ASSERT(ret_val);

    /************************/
    /* Activate DRP Library */
    /************************/
    ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
    DRP_DRV_ASSERT(ret_val);
    
    R_MMU_VAtoPA((uint32_t)R_BCD_LcdGetVramAddress(), &(param.dst));         /* Convert pointer to uint32_t */
    R_MMU_VAtoPA((uint32_t)ave_result, &(param.accumulate));                 /* Convert pointer to uint32_t */
    R_MMU_VAtoPA((uint32_t)look_up_table, &(param.table));                   /* Convert pointer to uint32_t */

    /* Image size */
    param.width = 640;
    param.height = 480;

    /* Accumlate ( Not use ) */
    param.area1_offset_x = 0;
    param.area1_offset_y = 0;
    param.area1_width    = R_BCD_CAMERA_WIDTH;
    param.area1_height   = R_BCD_CAMERA_HEIGHT;
    param.area2_offset_x = 0;
    param.area2_offset_y = 0;
    param.area2_width    = 0;
    param.area2_height   = 0;
    param.area3_offset_x = 0;
    param.area3_offset_y = 0;
    param.area3_width    = 0;
    param.area3_height   = 0;    
    param.component      = 1;     /* Accumlate ON */

    /* Gain and Bias  */
    color_mode = COLOR_MODE_OFF;
    param.bias_r = (int8_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_BIAS_R]; /* actual type is int8_t */
    param.bias_g = (int8_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_BIAS_G]; /* actual type is int8_t */
    param.bias_b = (int8_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_BIAS_B]; /* actual type is int8_t */
    param.gain_r = correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_R];
    param.gain_g = correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_G];
    param.gain_b = correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_B];

    /* Median ON */
    param.blend  = 0x100;

    /* Gamma ON */
    param.gamma = 0x01; 

    /* Unsharp Mask ON */
    param.strength = 28;
    param.coring = 60;

    /* Color matrix */
    param.matrix_c11 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C11]; /* actual type is int16_t */
    param.matrix_c12 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C12]; /* actual type is int16_t */
    param.matrix_c13 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C13]; /* actual type is int16_t */
    param.matrix_c21 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C21]; /* actual type is int16_t */
    param.matrix_c22 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C22]; /* actual type is int16_t */
    param.matrix_c23 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C23]; /* actual type is int16_t */
    param.matrix_c31 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C31]; /* actual type is int16_t */
    param.matrix_c32 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C32]; /* actual type is int16_t */
    param.matrix_c33 = (int16_t)correct_data[COLOR_MODE_OFF][COLOR_PARAM_GAIN_C33]; /* actual type is int16_t */

    /* 3DNR */
    nr_mode= NR_MODE_OFF;
    param.prev = 0; //3DNR OFF
    param.y_coef = ISP_3DNR_Y_COEF;
    param.c_coef = ISP_3DNR_C_COEF;
    param.y_alpha_max = ISP_3DNR_Y_ALPHA_MAX;
    param.y_thresh_a = ISP_3DNR_Y_THRESH_A;
    param.y_thresh_b = ISP_3DNR_Y_THRESH_B;
    param.y_tilt = ISP_3DNR_Y_TILT;
    param.c_alpha_max = ISP_3DNR_C_ALPHA_MAX;
    param.c_thresh_a = ISP_3DNR_C_THRESH_A;
    param.c_thresh_b = ISP_3DNR_C_THRESH_B;
    param.c_tilt = ISP_3DNR_C_TILT;

    R_BCD_SetCameraGain(CAMERA_GAIN_DEFAULT);

    /* Key initialize */
    key3_status = KEY_STAUS_INIT;
    key4_status = KEY_STAUS_INIT;

    /* main loop */
    while (1)
    {
        /* Get User Switch0 (SW2) key */
        key3_status = (key3_status << 1) | PORTD.PIDR.BIT.PIDR6;
        if ( ( key3_status & KEY_CHECK_BIT) == KEY_JUST_ON )
        {
            /* SW3 JUST ON */
            /* Change color mode */
            color_mode++;
            if( color_mode > COLOR_MODE_MAX )
            {
                color_mode = COLOR_MODE_OFF;
            }
            /* Renew gain and matrix data */
            param.bias_r = (int8_t)correct_data[color_mode][COLOR_PARAM_BIAS_R]; /* actual type is int8_t */
            param.bias_g = (int8_t)correct_data[color_mode][COLOR_PARAM_BIAS_G]; /* actual type is int8_t */
            param.bias_b = (int8_t)correct_data[color_mode][COLOR_PARAM_BIAS_B]; /* actual type is int8_t */
            param.gain_r = correct_data[color_mode][COLOR_PARAM_GAIN_R];
            param.gain_g = correct_data[color_mode][COLOR_PARAM_GAIN_G];
            param.gain_b = correct_data[color_mode][COLOR_PARAM_GAIN_B];
            param.matrix_c11 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C11]; /* actual type is int16_t */
            param.matrix_c12 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C12]; /* actual type is int16_t */
            param.matrix_c13 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C13]; /* actual type is int16_t */
            param.matrix_c21 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C21]; /* actual type is int16_t */
            param.matrix_c22 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C22]; /* actual type is int16_t */
            param.matrix_c23 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C23]; /* actual type is int16_t */
            param.matrix_c31 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C31]; /* actual type is int16_t */
            param.matrix_c32 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C32]; /* actual type is int16_t */
            param.matrix_c33 = (int16_t)correct_data[color_mode][COLOR_PARAM_GAIN_C33]; /* actual type is int16_t */
        }
        /* Get User Switch1 (SW3) key */
        key4_status = (key4_status << 1) | PORTD.PIDR.BIT.PIDR7;
        if ( ( key4_status & KEY_CHECK_BIT) == KEY_JUST_ON )
        {
            /* SW3 JUST ON */
            /* Change 3DNR mode */
            if( nr_mode == NR_MODE_OFF)
            {
                nr_mode = NR_MODE_ON;
                param.prev = param.dst;
            }
            else
            {
                nr_mode = NR_MODE_OFF;
                param.prev = 0;
            }
        }

        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* DO NOTHING */
        }

        /* Set the address of buffer to be read/write by DRP */
        p_input_bufadr  = R_BCD_CameraGetFrameAddress(frame_buf_id);
        p_output_bufadr = R_BCD_LcdGetVramAddress();

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_input_bufadr, &(param.src));    /* Convert pointer to uint32_t */
        R_MMU_VAtoPA((uint32_t)p_output_bufadr, &(param.dst));   /* Convert pointer to uint32_t */

        /* Set start time of process*/
        PerformSetStartTime(0);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set the address of buffer to be read/write by DRP */

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param, sizeof(r_drp_simple_isp_colcal_3dnr_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            ;
        }

        /* Set end time of process */
        PerformSetEndTime(0);

        /* AE */
        /****************************************************************/
        /* Auto Exposure Correction                                     */
        /* ilumi = (R * 0.299 * 4) + (G * 0.587 * 2) + (B * 0.114  * 4) */
        /****************************************************************/
        /* Cast to an appropriate type */
        R_BCD_AeRunAutoExpousure(&ae_setting, 
            (uint16_t)( ( (ave_result[0] * 0.299 * 4) + (ave_result[1] * 0.587 * 2) + (ave_result[2] * 0.114 * 4) )
            / (R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT) ) );

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Capture Start */
        R_BCD_CameraCaptureStart();

        /* Display image processing result */
        R_BCD_LcdSwapVideoBuffer();

        /* Diplay process time and color mode */
        R_BCD_LcdClearGraphicsBuffer();
        display_data();

        /* Write the data(buf) on the cache to physical memory */
        R_CACHE_L1DataCleanLine(R_BCD_LcdGetOLVramAddress(), ((R_BCD_LCD_WIDTH * R_BCD_LCD_HEIGHT) / 2));

        /* Display overlay buffer written processing time */
        R_BCD_LcdSwapGraphicsBuffer();
    }

sample_finish:

    return;
}
/**********************************************************************************************************************
* End of function sample_main
**********************************************************************************************************************/


/**********************************************************************************************************************
* Function Name: display_data
* Description  : This function is create overlay display.
* Arguments    : none
* Return Value : -
**********************************************************************************************************************/
static void display_data( void )
{
    uint8_t buf[80];
    uint32_t us;

    us = PerformGetElapsedTime_us(0);
    sprintf((void *)&buf[0], /* Convert buffer addres to void* */
            "Simple ISP with color calibration and 3DNR : %d.%d[ms]", 
            (int_t)(us / 1000), (int_t)((us - ((us / 1000) * 1000)) / 100));    /* less than 0x80000000 */
    R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);
    if( color_mode == COLOR_MODE_OFF )
    {
        sprintf((void *)&buf[0],"Bias/Gain correction : OFF"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 32, BLUE);
        sprintf((void *)&buf[0],"Color Matrix correction : OFF"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 62, BLUE);
    }
    if( color_mode == COLOR_MODE_BIASGAIN )
    {
        sprintf((void *)&buf[0],"Bias/Gain correction : ON"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 32, GREEN);
        sprintf((void *)&buf[0],"Color Matrix correction : OFF"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 62, BLUE);
    }
    if( color_mode == COLOR_MODE_ON_STD )
    {
        sprintf((void *)&buf[0],"Bias/Gain correction : ON"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 32, GREEN);
        sprintf((void *)&buf[0],"Color Matrix correction : Standard"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 62, GREEN);
    }
    if( color_mode == COLOR_MODE_ON_SEPIA )
    {
        sprintf((void *)&buf[0],"Bias/Gain correction : ON"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 32, GREEN);
        sprintf((void *)&buf[0],"Color Matrix correction : Sepia"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 62, YELLOW);
    }
    if( nr_mode == NR_MODE_ON )
    {
        sprintf((void *)&buf[0],"3D Noise reduction : ON"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 92, GREEN);
    }
    else
    {
        sprintf((void *)&buf[0],"3D Noise reduction : OFF"); /* Convert buffer addres to void* */
        R_BCD_LcdWriteString(&buf[0], 2, 92, BLUE);
    }
}

/* End of File */
