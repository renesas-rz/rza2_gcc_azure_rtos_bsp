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

/******************************************************************************
Typedef definitions
******************************************************************************/
/* Structure for buffer management between CPU and DRP */


/******************************************************************************
Macro definitions
******************************************************************************/


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/


/******************************************************************************
* Function Name: sample_main
* Description  : First function called after initialization is completed
* Arguments    : -
* Return Value : -
******************************************************************************/
void sample_main(void) 
{
    int32_t   frame_buf_id;
    uint8_t   buf[80];
    uint8_t * input_bufadr; 
    uint8_t * output_bufadr; 

    /* Initialization of VIN and MIPI driver */
    R_BCD_CameraInit();

    /* Initialization of LCD driver */
    R_BCD_LcdInit();

    /* Capture Start */
    R_BCD_CameraCaptureStart();

    /* Initialize of Performance counter */
    PerformInit();

    /* main loop */
    while (1)
    {
        uint32_t us;
        uint32_t width;
        uint32_t height;

        /* Wait until camera capture is complete */
        while ((frame_buf_id = R_BCD_CameraGetCaptureStatus()) == R_BCD_CAMERA_NOT_CAPTURED)
        {
            /* DO NOTHING */
            ;
        }
        input_bufadr  = R_BCD_CameraGetFrameAddress(frame_buf_id);
        output_bufadr = R_BCD_LcdGetVramAddress();

        /* Set start time of process*/
        PerformSetStartTime(0);

        for( height = 0 ; height < R_BCD_LCD_HEIGHT; height ++ )
        {
            for( width = 0 ; width < R_BCD_LCD_WIDTH ; width ++ )
            {
                output_bufadr[(R_BCD_LCD_WIDTH * height) + width]
                                    = input_bufadr[(R_BCD_CAMERA_WIDTH * height) + width];
            }
        }

        /* Set end time of process */
        PerformSetEndTime(0);
        us = PerformGetElapsedTime_us(0);

        /* Clear the current capture state and enable the detection of the next capture completion */
        R_BCD_CameraClearCaptureStatus();

        /* Display image processing result */
        R_BCD_LcdSwapVideoBuffer();

        /* Display time taken for image processing */
        R_BCD_LcdClearGraphicsBuffer();
        {
            sprintf(&buf[0],"Processing time : %d.%d[ms]", us / 1000, (us - ((us / 1000) * 1000)) / 100);
            R_BCD_LcdWriteString(&buf[0], 2, 2, GREEN);
        }
        R_BCD_LcdSwapGraphicsBuffer();

    }
    return;
}   /* End of function sample_main() */

/* End of File */
