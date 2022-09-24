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
* @file         camera_mipi_imx219_b_regdata.c
* @version      0.01
* @brief        MIPI Camera Register Setting
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "mipi_camera.h"
#if (CAM_MIPI == CAM_IMX219_B)
#include    "r_typedefs.h"
#include    "camera_mipi_imx219_b_regdata.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
const st_mipi_reg_t g_mipi_reg_init_table[INIT_REG_NUM_MIPI] =
{
    /* Initialize */
        /* 2-Lane */
        {0x01, 0x14, 0x01},

        /* timing setting:manual mode */
        {0x01, 0x28, 0x01},

        /* INCK = 12MHz */
        {0x01, 0x2A, 0x0C},
        {0x01, 0x2B, 0x00},

        /* frame length(height) = 2600(=A28H) */
        /* line  length(width)  = 3448(=D78H)(default) */
        {0x01, 0x60, 0x0A},
        {0x01, 0x61, 0x28},

        /* X range */
        {0x01, 0x64, 0x02},
        {0x01, 0x65, 0xA8},
        {0x01, 0x66, 0x0A},
        {0x01, 0x67, 0x27},

        /* Y range */
        {0x01, 0x68, 0x02},
        {0x01, 0x69, 0xB4},
        {0x01, 0x6A, 0x06},
        {0x01, 0x6B, 0xEB},

        /* signal range */
        {0x01, 0x6C, 0x05},
        {0x01, 0x6D, 0x00},
        {0x01, 0x6E, 0x04},
        {0x01, 0x6F, 0x38},

        /* output format RAW8 */
        {0x01, 0x8C, 0x08},
        {0x01, 0x8D, 0x08},

        /* binning (digital) mode */
        {0x01, 0x74, 0x01},
        {0x01, 0x75, 0x01},
        {0x01, 0x76, 0x01},
        {0x01, 0x77, 0x01},

        /* analog gain(LONG) control */
        {0x01, 0x57, 0xC0},

        /* Timing settings */
        {0x01, 0x1C, 0x00}, /* THS_ZERO_MIN */
        {0x01, 0x1D, 0x57}, /* THS_ZERO_MIN */
        {0x01, 0x24, 0x00}, /* TCLK_ZERO */
        {0x01, 0x25, 0xBF}, /* TCLK_ZERO */

        /* clock setting */
        {0x03, 0x01, 0x05},
        {0x03, 0x04, 0x02},
        {0x03, 0x05, 0x02},
        {0x03, 0x06, 0x00},
        {0x03, 0x07, 0x3A},
        {0x03, 0x09, 0x08},
        {0x03, 0x0C, 0x00},
        {0x03, 0x0D, 0x2A},
};

#endif /* CAM_MIPI == CAM_IMX219_B */
