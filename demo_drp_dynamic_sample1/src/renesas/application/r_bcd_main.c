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
#include <stdio.h>
#include <fcntl.h>

#include "r_typedefs.h"
#include "iodefine.h"
#include "r_cache_lld_rza2m.h"

#include "r_bcd_camera.h"
#include "r_bcd_lcd.h"
#include "draw.h"
#include "perform.h"
#include "r_mmu_lld_rza2m.h"

#include "r_dk2_if.h"
#include "r_drp_bayer2grayscale.h"
#include "r_drp_median_blur.h"
#include "r_drp_canny_calculate.h"
#include "r_drp_canny_hysterisis.h"

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Structure for buffer management between CPU and DRP */


/******************************************************************************
Macro definitions
******************************************************************************/
#define TILE_0          (0)
#define TILE_1          (1)
#define TILE_2          (2)
#define TILE_3          (3)
#define TILE_4          (4)
#define TILE_5          (5)

#define DRP_NOT_FINISH  (0)
#define DRP_FINISH      (1)

#define WEAK_LEVEL      (0x18)
#define STRONG_LEVEL    (0x28)

#define HYSTERISIS_COUNT (2)
#define DRP_DRV_ASSERT(x) if ((x) != 0) goto sample_finish

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint8_t g_drp_lib_bayer2grayscale[];
extern uint8_t g_drp_lib_median_blur[];
extern uint8_t g_drp_lib_canny_calculate[];
extern uint8_t g_drp_lib_canny_hysterisis[];
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = {DRP_NOT_FINISH};
static uint8_t work_buf[R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT] __attribute__ ((aligned(32)));
static uint8_t canny_work_buf[R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT+(2*3)) * 2] __attribute__ ((aligned(32)));

static r_drp_bayer2grayscale_t param_b2g[R_DK2_TILE_NUM] __attribute__ ((section("UNCACHED_BSS")));
static r_drp_median_blur_t param_median[R_DK2_TILE_NUM] __attribute__ ((section("UNCACHED_BSS")));
static r_drp_canny_calculate_t param_canny_cal[R_DK2_TILE_NUM] __attribute__ ((section("UNCACHED_BSS")));
static r_drp_canny_hysterisis_t param_canny_hyst __attribute__ ((section("UNCACHED_BSS")));

static void cb_drp_finish(uint8_t id);
static void write_canny_data(uint8_t *psrc, uint8_t *pdst);

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
    uint32_t *p_src_w = (uint32_t *)psrc;
    uint16_t *p_dst_h = (uint16_t *)pdst;
    uint32_t i;
    uint32_t j;

    /* Loop for one screen */
    for (i = 0; i < R_BCD_LCD_HEIGHT; i++)
    {
        for (j = 0; j < (R_BCD_LCD_WIDTH/4); j++)
        {
            /* Write patterns corresponding to the data output by the DRP application in the overlapping buffer */
            switch (*p_src_w)
            {
                case 0x000000FF:
                    *p_dst_h = 0xFF5F;
                    break;
                case 0x0000FF00:
                    *p_dst_h = 0xFFF5;
                    break;
                case 0x0000FFFF:
                    *p_dst_h = 0xFF55;
                    break;
                case 0x00FF0000:
                    *p_dst_h = 0x5FFF;
                    break;
                case 0x00FF00FF:
                    *p_dst_h = 0x5F5F;
                    break;
                case 0x00FFFF00:
                    *p_dst_h = 0x5FF5;
                    break;
                case 0x00FFFFFF:
                    *p_dst_h = 0x5F55;
                    break;
                case 0xFF000000:
                    *p_dst_h = 0xF5FF;
                    break;
                case 0xFF0000FF:
                    *p_dst_h = 0xF55F;
                    break;
                case 0xFF00FF00:
                    *p_dst_h = 0xF5F5;
                    break;
                case 0xFF00FFFF:
                    *p_dst_h = 0xF555;
                    break;
                case 0xFFFF0000:
                    *p_dst_h = 0x55FF;
                    break;
                case 0xFFFF00FF:
                    *p_dst_h = 0x555F;
                    break;
                case 0xFFFFFF00:
                    *p_dst_h = 0x55F5;
                    break;
                case 0xFFFFFFFF:
                    *p_dst_h = 0x5555;
                    break;
                default :
                    break;
            }

            p_src_w++;
            p_dst_h++;
        }
    }
    return;
}
/**********************************************************************************************************************
* End of function write_canny_data
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: sample_main
* Description  : First function called after initialization is completed
* Arguments    : -
* Return Value : -
******************************************************************************/
void sample_main(void)
{
    int32_t frame_buf_id;
    uint8_t buf[80];
    int32_t ret_val;
    uint32_t us;
    uint32_t input_bufadr;
    uint32_t output_bufadr;
    uint32_t work_bufadr;
    uint32_t tile_no;
    uint32_t bufsize;
    uint8_t * p_input_bufadr;
    uint8_t * p_output_bufadr;
    uint8_t * p_work_bufadr;

    /* Initialization of VIN and MIPI driver */
    R_BCD_CameraInit();

    /* Initialization of LCD driver */
    R_BCD_LcdInit();

    /* Capture Start */
    R_BCD_CameraCaptureStart();

    /* Initialize of Performance counter */
    PerformInit();

    /********************************/
    /* Initialization of DRP driver */
    /********************************/
    R_DK2_Initialize();
    /* main loop */
    while (1)
    {
        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* DO NOTHING */
        }
        p_input_bufadr  = R_BCD_CameraGetFrameAddress(frame_buf_id);
        p_output_bufadr = R_BCD_LcdGetVramAddress();
        p_work_bufadr = (uint8_t *)&work_buf[0];

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_input_bufadr, &input_bufadr);

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_output_bufadr, &output_bufadr);

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_work_bufadr, &work_bufadr);

        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /* tile 1 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /* tile 2 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /* tile 3 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /* tile 4 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /* tile 5 | Bayer2Grayscale | */
        /*        +-----------------+ */
        /******************************/
        ret_val = R_DK2_Load(&g_drp_lib_bayer2grayscale[0],
                            R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5,
                            R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3]
                                                                        | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(0);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
        {
            /* Set the address of buffer to be read/write by DRP */
            param_b2g[tile_no].src = input_bufadr + ((R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM))
                                                                                                            * tile_no);
            param_b2g[tile_no].dst = work_bufadr + ((R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM))
                                                                                                            * tile_no);

            /* Set Image size */
            param_b2g[tile_no].width = R_BCD_CAMERA_WIDTH;
            param_b2g[tile_no].height = R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM;

            /* Set whether to perform top or bottom edge border processing. */
            param_b2g[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param_b2g[tile_no].bottom = (tile_no == TILE_5) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_b2g[tile_no], sizeof(r_drp_bayer2grayscale_t));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
        {
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH)
            {
                /* DO NOTHING */
            }
        }

        /* Set end time of process */
        PerformSetEndTime(0);

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3]
                                                            | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Capture Start */
        R_BCD_CameraCaptureStart();

        /**************************/
        /* Load DRP Library       */
        /*        +-------------+ */
        /* tile 0 | Median blur | */
        /*        +-------------+ */
        /* tile 1 | Median blur | */
        /*        +-------------+ */
        /* tile 2 | Median blur | */
        /*        +-------------+ */
        /* tile 3 | Median blur | */
        /*        +-------------+ */
        /* tile 4 | Median blur | */
        /*        +-------------+ */
        /* tile 5 | Median blur | */
        /*        +-------------+ */
        /**************************/
        ret_val = R_DK2_Load(&g_drp_lib_median_blur[0],
                            R_DK2_TILE_0 | R_DK2_TILE_1 | R_DK2_TILE_2 | R_DK2_TILE_3 | R_DK2_TILE_4 | R_DK2_TILE_5,
                            R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3]
                                                                        | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(1);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
        {
            /* Set the address of buffer to be read/write by DRP */
            param_median[tile_no].src = work_bufadr + ((R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM))
                                                                                                            * tile_no);
            param_median[tile_no].dst = output_bufadr + ((R_BCD_CAMERA_WIDTH * (R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM))
                                                                                                            * tile_no);

            /* Set Image size */
            param_median[tile_no].width = R_BCD_CAMERA_WIDTH;
            param_median[tile_no].height = R_BCD_CAMERA_HEIGHT / R_DK2_TILE_NUM;

            /* Set whether to perform top or bottom edge border processing. */
            param_median[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param_median[tile_no].bottom = (tile_no == TILE_5) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_median[tile_no], sizeof(r_drp_median_blur_t));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        for (tile_no = 0; tile_no < R_DK2_TILE_NUM; tile_no++)
        {
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH)
            {
                /* DO NOTHING */
            }
        }

        /* Set end time of process */
        PerformSetEndTime(1);

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3] |
                                                        drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /* Display image processing result */
        R_BCD_LcdSwapVideoBuffer();

        /******************************/
        /* Load DRP Library           */
        /*        +-----------------+ */
        /* tile 0 |                 | */
        /*        + Canny Calculate + */
        /* tile 1 |                 | */
        /*        +-----------------+ */
        /* tile 2 |                 | */
        /*        + Canny Calculate + */
        /* tile 3 |                 | */
        /*        +-----------------+ */
        /* tile 4 |                 | */
        /*        + Canny Calculate + */
        /* tile 5 |                 | */
        /*        +-----------------+ */
        /******************************/
        ret_val = R_DK2_Load(&g_drp_lib_canny_calculate[0], R_DK2_TILE_0 | R_DK2_TILE_2 | R_DK2_TILE_4,
                            R_DK2_TILE_PATTERN_2_2_2, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /********************/
        /* Activate DRP Lib */
        /********************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /* Set Image size */
            param_canny_cal[tile_no].width = R_BCD_CAMERA_WIDTH;
            param_canny_cal[tile_no].height = (R_BCD_CAMERA_HEIGHT / 3);

            /* Set Buffer size */
            bufsize = (uint32_t)(param_canny_cal[tile_no].width * param_canny_cal[tile_no].height);

            /* Set the address of buffer to be read/write by DRP */
            param_canny_cal[tile_no].src = (uint32_t)output_bufadr + ((tile_no / 2) * bufsize);
            param_canny_cal[tile_no].dst = (uint32_t)&work_buf[0]  + ((tile_no / 2) * bufsize);

            /* Set additional information (currently no additional information) */
            param_canny_cal[tile_no].work =
                                (uint32_t)&canny_work_buf[((tile_no / 2) * (bufsize + (R_BCD_CAMERA_WIDTH*2) ))*2];
            param_canny_cal[tile_no].threshold_high = STRONG_LEVEL;
            param_canny_cal[tile_no].threshold_low = WEAK_LEVEL;

            /* Set whether to perform top or bottom edge border processing. */
            param_canny_cal[tile_no].top = (tile_no == TILE_0) ? 1 : 0;
            param_canny_cal[tile_no].bottom = (tile_no == TILE_4) ? 1 : 0;

            /* Initialize variables to be used in termination judgment of the DRP library */
            drp_lib_status[tile_no] = DRP_NOT_FINISH;

            /*********************/
            /* Start DRP Library */
            /*********************/
            ret_val = R_DK2_Start(drp_lib_id[tile_no], (void *)&param_canny_cal[tile_no],
                                                                                sizeof(param_canny_cal[tile_no]));
            DRP_DRV_ASSERT(ret_val);
        }

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        for (tile_no = TILE_0; tile_no <= TILE_4; tile_no += 2)
        {
            /***************************************/
            /* Wait until DRP processing is finish */
            /***************************************/
            while (drp_lib_status[tile_no] == DRP_NOT_FINISH)
            {
                /* DO NOTHING */
            }
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(2);

        /**********************/
        /* Unload DRP library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_2] | drp_lib_id[TILE_4], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /*******************************/
        /* Load DRP Library            */
        /*        +------------------+ */
        /* tile 0 |                  | */
        /*        +                  + */
        /* tile 1 |                  | */
        /*        +                  + */
        /* tile 2 |                  | */
        /*        + Canny Hysterisis + */
        /* tile 3 |                  | */
        /*        +                  + */
        /* tile 4 |                  | */
        /*        +                  + */
        /* tile 5 |                  | */
        /*        +------------------+ */
        /*******************************/
        ret_val = R_DK2_Load(&g_drp_lib_canny_hysterisis[0],
                            R_DK2_TILE_0,
                            R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of Canny Hyst process */
        PerformSetStartTime(3);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set Image size */
        param_canny_hyst.width = R_BCD_CAMERA_WIDTH;
        param_canny_hyst.height = R_BCD_CAMERA_HEIGHT;

        /* Set the address of buffer to be read/write by DRP */
        param_canny_hyst.src = (uint32_t)&work_buf[0];
        param_canny_hyst.dst = (uint32_t)&canny_work_buf[0];

        /* Set additional information (currently no additional information) */
        param_canny_hyst.work = (uint32_t)&canny_work_buf[R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT];
        param_canny_hyst.iterations = HYSTERISIS_COUNT;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_canny_hyst, sizeof(param_canny_hyst));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* DO NOTHING */
        }

        /* Set end time of DRP processing */
        PerformSetEndTime(3);

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /* Clear Graphics buffer*/
        R_BCD_LcdClearGraphicsBuffer();

        /* Clear the cache to read the frame buffer in physical memory */
        R_CACHE_L1DataCleanInvalidLine((void *)&canny_work_buf[0],
                                        (uint32_t)(param_canny_hyst.width * param_canny_hyst.height));

        /* Write the canny's result into the graphics buffer */
        write_canny_data((uint8_t *)canny_work_buf, R_BCD_LcdGetOLVramAddress());

        /* Display time taken for image processing */
        us = PerformGetElapsedTime_us(0) + PerformGetElapsedTime_us(1) + PerformGetElapsedTime_us(2)
            + PerformGetElapsedTime_us(3);
        sprintf((void *)&buf[0],"Total : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000)) / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);
        us = PerformGetElapsedTime_us(0);
        sprintf((void *)&buf[0],"Bayer2Grayscale : %d.%d[ms]", (int)(us / 1000),
                (int)((us - ((us / 1000) * 1000)) / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 32, GREEN);
        us = PerformGetElapsedTime_us(1);
        sprintf((void *)&buf[0],"Median blur : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000)) / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 62, GREEN);
        us = PerformGetElapsedTime_us(2) + PerformGetElapsedTime_us(3);
        sprintf((void *)&buf[0],"Canny Edge : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000)) / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 92, GREEN);

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

/* End of File */
