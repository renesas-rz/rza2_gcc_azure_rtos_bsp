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
* @file         camera_imx219_b.h
* @version      0.01
* @brief        MIPI camera IMX219B definition header
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_mipi_api.h"

#ifndef IMX219_B_H
#define IMX219_B_H
/******************************************************************************
Macro definitions
******************************************************************************/
/* MIPI camera setting */
#define     CAM_MIPI_LANE_NUM        ( 2u)          /* Number of data lane */
#define     CAM_MIPI_LANE_SWAP       ( 0u)          /* Lane swap enable */
#define     CAM_MIPI_SEL_VC          ( 0u)          /* Vertual channel number */
#define     CAM_MIPI_TRANSFER_RATE   (80u)          /* Data transfer rate (Mbit per sec) */

/* PHY timing */
#define     CAM_MIPI_THS_PREPARE     (0x00000009u)  /* Setting of the duration of the LP-00 state (immediately before entry to the HS-0 state) */ 
#define     CAM_MIPI_THS_SETTLE      (0x00000014u)  /* Setting of the period in which a transition to the HS state is ignored after the TTHS_PREPARE period begins */ 
#define     CAM_MIPI_TCLK_PREPARE    (0x0000000Au)  /* Setting of the duration of the LP-00 state (immediately before entry to the HS-0) */ 
#define     CAM_MIPI_TCLK_SETTLE     (0x00000014u)  /* Setting of the period in which a transition to the HS state is ignored after the TCLK_PREPARE period begins */ 
#define     CAM_MIPI_TCLK_MISS       (0x00000003u)  /* Setting of the period in which the absence of the clock is detected, and the HS-RX is disabled */ 
#define     CAM_MIPI_T_INIT_SLAVE    (0x000033F3u)  /* Minimum duration of the INIT state */

/* VIN capture setting */
#define     CAM_VIN_PRECLIP_START_Y  ( 24u)                 /* Start line  of pre clip area */
#define     CAM_VIN_PRECLIP_WIDTH_Y  (480u)                 /* Line width of pre clip area */
#define     CAM_VIN_PRECLIP_START_X  (100u)                 /* Start pixel of pre clip area */
#define     CAM_VIN_PRECLIP_WIDTH_X  (640u)                 /* Pixel width of pre clip area */
#define     CAM_VIN_INPUT_YCAL       (VIN_Y_UPPER)          /* YCbCr422 data alignment */
#define     CAM_VIN_INPUT_FORMAT     (VIN_INPUT_RAW8)       /* Input image format */
#define     CAM_VIN_OUTPUT_FORMAT    (VIN_OUTPUT_RAW8)      /* Output image format */
#define     CAM_VIN_OUTPUT_ENDIAN    (VIN_OUTPUT_EN_LITTLE) /* Output data endian */
#define     CAM_VIN_OUTPUT_BPSM      (VIN_SWAP_OFF)         /* Output data byte swap mode */
#define     CAM_VIN_OUTPUT_IS        (640u)                 /* Image Stride size */

#endif  /*  IMX219_B_H */
