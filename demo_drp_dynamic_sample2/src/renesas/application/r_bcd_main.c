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
#include "r_cache_lld_rza2m.h"

#include "r_bcd_camera.h"
#include "r_bcd_lcd.h"
#include "draw.h"
#include "perform.h"
#include "r_mmu_lld_rza2m.h"

#include "r_dk2_if.h"
#include "r_drp_bayer2grayscale.h"
#include "r_drp_median_blur.h"
#include "r_drp_corner_harris.h"

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

#define HARRIS_SHIFT_NUM    (3)

#define DRP_DRV_ASSERT(x) if ((x) != 0) goto sample_finish

/**********************************************************************************************************************
Imported global variables and functions (from other files)
**********************************************************************************************************************/
extern uint8_t g_drp_lib_bayer2grayscale[];
extern uint8_t g_drp_lib_median_blur[];
extern uint8_t g_drp_lib_corner_harris[];

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = {DRP_NOT_FINISH};
static uint8_t work_buf[R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT] __attribute__ ((aligned(32)));

static r_drp_bayer2grayscale_t param_b2g[R_DK2_TILE_NUM] __attribute__ ((section("UNCACHED_BSS")));
static r_drp_median_blur_t param_median[R_DK2_TILE_NUM] __attribute__ ((section("UNCACHED_BSS")));
static r_drp_corner_harris_t param_harris __attribute__ ((section("UNCACHED_BSS")));

static void cb_drp_finish(uint8_t id);
static void write_harris_data(uint8_t *psrc, uint32_t *pdst);

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
* Function Name: write_harris_data
* Description  : Write Harris output result to the overlapping buffer
* Arguments    : psrc
*              :   Pointer of input image.
*              : pdst
*              :   Pointer of Output buffer.
* Return Value : -
**********************************************************************************************************************/
static void write_harris_data(uint8_t *psrc, uint32_t *pdst)
{
    /* auto variables */
    uint32_t i;
    uint32_t j;
    uint32_t k;
    uint8_t cmp_data = 3;

    /* Loop for one screen */
    for (i = 0; i < R_BCD_LCD_HEIGHT; i++)
    {
        for (j = 0; j < (R_BCD_LCD_WIDTH / 8); j++)
        {
            /* Write patterns corresponding to the data output by the DRP/CPU application in the overlapping buffer */
            k = 0xFFFFFFFF;
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA0;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA000;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA00;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA00000;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA0000;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA0000000;
            }
            if ((*psrc++) >= cmp_data)
            {
                k -= 0xA000000;
            }

            *pdst = k;
            pdst++;
        }
    }

    return;
}
/**********************************************************************************************************************
* End of function write_harris_data
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
    uint8_t buf[80];
    int32_t ret_val;
    uint32_t us;
    uint32_t input_bufadr;
    uint32_t output_bufadr;
    uint32_t work_bufadr;
    uint32_t tile_no;
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
        p_work_bufadr   = (uint8_t *)&work_buf[0];

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
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0] | drp_lib_id[TILE_1] | drp_lib_id[TILE_2] | drp_lib_id[TILE_3]
                                | drp_lib_id[TILE_4] | drp_lib_id[TILE_5], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /* Display image processing result */
        R_BCD_LcdSwapVideoBuffer();

        /****************************/
        /* Load DRP Library         */
        /*        +---------------+ */
        /* tile 0 |               | */
        /*        +               + */
        /* tile 1 |               | */
        /*        +               + */
        /* tile 2 |               | */
        /*        + Corner Harris + */
        /* tile 3 |   (6 Tile)    | */
        /*        +               + */
        /* tile 4 |               | */
        /*        +               + */
        /* tile 5 |               | */
        /*        +---------------+ */
        /****************************/
        ret_val = R_DK2_Load(&g_drp_lib_corner_harris[0],
                            R_DK2_TILE_0,
                            R_DK2_TILE_PATTERN_6, NULL, &cb_drp_finish, &drp_lib_id[0]);
        DRP_DRV_ASSERT(ret_val);

        /************************/
        /* Activate DRP Library */
        /************************/
        ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
        DRP_DRV_ASSERT(ret_val);

        /* Set start time of process*/
        PerformSetStartTime(2);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set the address of buffer to be read/write by DRP */
        param_harris.src = output_bufadr;
        param_harris.dst = work_bufadr;

        /* Set Image size */
        param_harris.width = R_BCD_CAMERA_WIDTH;
        param_harris.height = R_BCD_CAMERA_HEIGHT;

        /* Set additional information (currently no additional information) */
        param_harris.shift = HARRIS_SHIFT_NUM;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param_harris, sizeof(r_drp_corner_harris_t));
        DRP_DRV_ASSERT(ret_val);

        /***************************************/
        /* Wait until DRP processing is finish */
        /***************************************/
        while (drp_lib_status[TILE_0] == DRP_NOT_FINISH)
        {
            /* DO NOTHING */
        }

        /* Set end time of process */
        PerformSetEndTime(2);

        /**********************/
        /* Unload DRP Library */
        /**********************/
        ret_val = R_DK2_Unload(drp_lib_id[TILE_0], &drp_lib_id[TILE_0]);
        DRP_DRV_ASSERT(ret_val);

        /* Clear Graphics buffer*/
        R_BCD_LcdClearGraphicsBuffer();

        /* Clear the cache to read the frame buffer in physical memory */
        R_CACHE_L1DataCleanInvalidLine((void *)&work_buf[0], (R_BCD_CAMERA_WIDTH * R_BCD_CAMERA_HEIGHT));

        /* Write the corner harris detection result into the graphics buffer */
        write_harris_data((uint8_t *)work_buf, (uint32_t *)R_BCD_LcdGetOLVramAddress());

        /* Display time taken for image processing */
        us = PerformGetElapsedTime_us(0);
        sprintf((void *)&buf[0],"Bayer2Grayscale : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000))
                                                                                                            / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);
        us = PerformGetElapsedTime_us(1);
        sprintf((void *)&buf[0],"Median blur : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000))
                                                                                                            / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 32, GREEN);
        us = PerformGetElapsedTime_us(2);
        sprintf((void *)&buf[0],"Corner Harris : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000))
                                                                                                            / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 62, GREEN);

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
