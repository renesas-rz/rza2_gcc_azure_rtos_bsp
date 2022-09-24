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
* @file         camera_mipi_imx219_b_regdata.h
* @version      0.01
* @brief        MIPI Camera Raspberry Pi Camera Module V2 reg data table header
******************************************************************************/

#ifndef MIPI_REGDATA_H
#define MIPI_REGDATA_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "mipi_camera.h"
#if (CAM_MIPI == CAM_IMX219_B)
#include    <stdlib.h>
#include    "r_typedefs.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define INIT_REG_NUM_MIPI         (37u)

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef struct
{
    uint8_t  reg_add_upper; /* address of [15:8] */
    uint8_t  reg_add_lower; /* address of [7:0]  */
    uint8_t  value;         /* value of register */
} st_mipi_reg_t;

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
extern const  st_mipi_reg_t g_mipi_reg_init_table[INIT_REG_NUM_MIPI]; 

#endif /* CAM_MIPI == CAM_IMX219_B */
#endif  /* MIPI_REGDATA_H */
