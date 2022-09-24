/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name   : r_drp_image_merging.h
* Description : This source code is the header file of Image merging processing.
* Version     : 1.00
*******************************************************************************/
#ifndef R_DRP_IMAGE_MERGING_H
#define R_DRP_IMAGE_MERGING_H

/*******************************************************************************
Global Typedef definitions
*******************************************************************************/
#ifdef BDL
typedef unsigned long long uint64_t;
typedef unsigned long uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef signed long long int64_t;
typedef signed long int32_t;
typedef signed short int16_t;
typedef signed char int8_t;
#else /* BDL */
#include <stdint.h>
#endif /* BDL */

/* Structure of interface parameters between CPU and DRP library. */
typedef struct
{
    uint32_t src_adr1;          /* Address of input image1. */
    uint32_t src_adr2;          /* Address of input image2. */

    uint32_t dst_adr;           /* Address of output image. */
    uint32_t angledata_adr;     /* Address of angle data. */

    uint32_t result_adr;        /* Address of result data. */
    uint16_t width;             /* The horizontal size (pixels) of input image. */
    uint16_t height1;           /* The vertical size (pixels) of input image1. */

    uint16_t height2;           /* The vertical size (pixels) of input image2. */
    uint16_t src_diff;          /* Difference pixels in image position. */
    uint8_t  max_gap_x;         /* Maximum X-gap (pixels) allowed. */
    uint8_t  max_gap_y;         /* Maximum Y-gap (pixels) allowed. */
    uint8_t  search_window_w;   /* The horizontal size (pixels) of search window */
    uint8_t  search_window_h;   /* The vertical size (pixels) of search window */

    uint16_t search_window1_x;  /* Upper left x-coordinate of search window 1 */
    uint16_t search_window1_y;  /* Upper left y-coordinate of search window 1 */
    uint16_t search_window2_x;  /* Upper left x-coordinate of search window 2 */
    uint16_t search_window2_y;  /* Upper left y-coordinate of search window 2 */

    uint16_t search_window3_x;  /* Upper left x-coordinate of search window 3 */
    uint16_t search_window3_y;  /* Upper left y-coordinate of search window 3 */
    uint16_t search_window4_x;  /* Upper left x-coordinate of search window 4 */
    uint16_t search_window4_y;  /* Upper left y-coordinate of search window 4 */

    uint8_t  padding_value;     /* Value to set out of range */
    uint8_t  angle_times;       /* Steps of tilt */
} r_drp_image_merging_t;

#endif /* R_DRP_IMAGE_MERGING_H */

/* end of file */
