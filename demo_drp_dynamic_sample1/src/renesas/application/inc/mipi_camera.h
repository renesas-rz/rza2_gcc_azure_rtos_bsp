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
 * @file         mipi_camera.h
 * @version      0.01
 * @brief        Camera for mipi definition header
 ******************************************************************************/
#ifndef MIPI_CAMERA_H
#define MIPI_CAMERA_H

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define CAM_IMX219_B    (0)   /* Raspberri Pi Camera V2, WVGA */

#define CAM_MIPI        (CAM_IMX219_B)

#if (CAM_MIPI == CAM_IMX219_B)
#include "camera/camera_imx219_b.h"
#else
    #error  Undefined Camera Module!
#endif
/******************************************************************************
 * Function Name: Init_Camera
 *****************************************************************************/
void Init_Camera(void);

/******************************************************************************
 * Function Name: Start_Camera
 *****************************************************************************/
void Start_Camera( void );

/******************************************************************************
 * Function Name: RIIC_MipiWrite
 *****************************************************************************/
void RIIC_MipiWrite( uint8_t ic_adr, uint8_t reg_adr_upper, uint8_t reg_adr_lower, uint8_t reg_data );

#endif  /*  MIPI_CAMERA_H */
