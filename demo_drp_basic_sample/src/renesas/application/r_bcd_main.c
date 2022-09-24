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

#include "r_dk2_if.h"
#include "r_drp_bayer2grayscale.h"

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

#define DRP_DRV_ASSERT(x) if ((x) != 0) goto sample_finish

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern uint8_t g_drp_lib_bayer2grayscale[];
/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t drp_lib_id[R_DK2_TILE_NUM] = {0};
static volatile uint8_t drp_lib_status[R_DK2_TILE_NUM] = {DRP_NOT_FINISH};
static r_drp_bayer2grayscale_t param __attribute__ ((section("UNCACHED_BSS")));

static void cb_drp_finish(uint8_t id);

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
    uint8_t * p_input_bufadr;
    uint8_t * p_output_bufadr;

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

    /******************************/
    /* Load DRP Library           */
    /*        +-----------------+ */
    /* tile 0 | Bayer2Grayscale | */
    /*        +-----------------+ */
    /* tile 1 |                 | */
    /*        +-----------------+ */
    /* tile 2 |                 | */
    /*        +-----------------+ */
    /* tile 3 |                 | */
    /*        +-----------------+ */
    /* tile 4 |                 | */
    /*        +-----------------+ */
    /* tile 5 |                 | */
    /*        +-----------------+ */
    /******************************/
    ret_val = R_DK2_Load(&g_drp_lib_bayer2grayscale[0], R_DK2_TILE_0, R_DK2_TILE_PATTERN_1_1_1_1_1_1, NULL,
                        &cb_drp_finish, &drp_lib_id[0]);
    DRP_DRV_ASSERT(ret_val);

    /************************/
    /* Activate DRP Library */
    /************************/
    ret_val = R_DK2_Activate(drp_lib_id[TILE_0], 0);
    DRP_DRV_ASSERT(ret_val);

    /* main loop */
    while (1)
    {
        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* DO NOTHING */
        }

        /* Set start time of process*/
        PerformSetStartTime(0);

        /***************************************/
        /* Set R_DK2_Start function parameters */
        /***************************************/
        /* Set the address of buffer to be read/write by DRP */
        p_input_bufadr  = R_BCD_CameraGetFrameAddress(frame_buf_id);
        p_output_bufadr = R_BCD_LcdGetVramAddress();

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_input_bufadr, &(param.src));

        /* convert to phisical address */
        R_MMU_VAtoPA((uint32_t)p_output_bufadr, &(param.dst));

        /* Set Image size */
        param.width = R_BCD_CAMERA_WIDTH;
        param.height = R_BCD_CAMERA_HEIGHT;

        /* Set whether to perform top or bottom edge border processing. */
        param.top = 1;
        param.bottom = 1;

        /* Initialize variables to be used in termination judgment of the DRP library */
        drp_lib_status[TILE_0] = DRP_NOT_FINISH;

        /*********************/
        /* Start DRP Library */
        /*********************/
        ret_val = R_DK2_Start(drp_lib_id[TILE_0], (void *)&param, sizeof(r_drp_bayer2grayscale_t));
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

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Capture Start */
        R_BCD_CameraCaptureStart();

        /* Display image processing result */
        R_BCD_LcdSwapVideoBuffer();

        /* Display time taken for image processing */
        R_BCD_LcdClearGraphicsBuffer();
        us = PerformGetElapsedTime_us(0);
        sprintf((void *)&buf[0],"Bayer2Grayscale : %d.%d[ms]", (int)(us / 1000), (int)((us - ((us / 1000) * 1000))
                / 100));
        R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);

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
