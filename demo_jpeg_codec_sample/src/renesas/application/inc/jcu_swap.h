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
* File Name: jcu_swap.h
*    JCU swap
******************************************************************************/
#ifndef JCU_SWAP_H
#define JCU_SWAP_H

/******************************************************************************
Macro definitions
******************************************************************************/

#ifdef _SH

/* JpegData Input/Output SwapSetting */
#define JCU_SWAP_JPEG_NONE                          (JCU_SWAP_NONE)
#define JCU_SWAP_JPEG_BYTE                          (JCU_SWAP_BYTE)
#define JCU_SWAP_JPEG_WORD                          (JCU_SWAP_WORD)
#define JCU_SWAP_JPEG_WORD_AND_BYTE                 (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_JPEG_LONG_WORD                     (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_JPEG_LONG_WORD_AND_BYTE            (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_JPEG_LONG_WORD_AND_WORD            (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_JPEG_LONG_WORD_AND_WORD_AND_BYTE   (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)

/* ARGB8888 Output SwapSetting */
#define JCU_SWAP_ARGB8888_PIXEL01                   (JCU_SWAP_NONE)
#define JCU_SWAP_RABG8888_PIXEL01                   (JCU_SWAP_BYTE)
#define JCU_SWAP_GBAR8888_PIXEL01                   (JCU_SWAP_WORD)
#define JCU_SWAP_BGRA8888_PIXEL01                   (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_ARGB8888_PIXEL10                   (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_RABG8888_PIXEL10                   (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_GBAR8888_PIXEL10                   (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_BGRA8888_PIXEL10                   (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)

/* RGB565 Output SwapSetting */
#define JCU_SWAP_RGB565_PIXEL0123                   (JCU_SWAP_NONE)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL0123          (JCU_SWAP_BYTE)
#define JCU_SWAP_RGB565_PIXEL1032                   (JCU_SWAP_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL1032          (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_RGB565_PIXEL2301                   (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL2301          (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_RGB565_PIXEL3210                   (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL3210          (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)

/* YUV422 Input/Output SwapSetting */
#define JCU_SWAP_CB_Y0_CR_Y1_PIXEL0123              (JCU_SWAP_NONE)
#define JCU_SWAP_Y0_CB_Y1_CR_PIXEL0123              (JCU_SWAP_BYTE)
#define JCU_SWAP_CR_Y1_CB_Y0_PIXEL0123              (JCU_SWAP_WORD)
#define JCU_SWAP_Y1_CR_Y0_CB_PIXEL0123              (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_CB_Y0_CR_Y1_PIXEL2301              (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_Y0_CB_Y1_CR_PIXEL2301              (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_CR_Y1_CB_Y0_PIXEL2301              (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_Y1_CR_Y0_CB_PIXEL2301              (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)

#else /* Not ifdef _SH */

/* JpegData Input/Output SwapSetting */
#define JCU_SWAP_JPEG_NONE                          (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)
#define JCU_SWAP_JPEG_BYTE                          (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_JPEG_WORD                          (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_JPEG_WORD_AND_BYTE                 (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_JPEG_LONG_WORD                     (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_JPEG_LONG_WORD_AND_BYTE            (JCU_SWAP_WORD)
#define JCU_SWAP_JPEG_LONG_WORD_AND_WORD            (JCU_SWAP_BYTE)
#define JCU_SWAP_JPEG_LONG_WORD_AND_WORD_AND_BYTE   (JCU_SWAP_NONE)

/* ARGB8888 Output SwapSetting */
#define JCU_SWAP_ARGB8888_PIXEL01                   (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_RABG8888_PIXEL01                   (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_GBAR8888_PIXEL01                   (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_BGRA8888_PIXEL01                   (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)
#define JCU_SWAP_ARGB8888_PIXEL10                   (JCU_SWAP_NONE)
#define JCU_SWAP_RABG8888_PIXEL10                   (JCU_SWAP_BYTE)
#define JCU_SWAP_GBAR8888_PIXEL10                   (JCU_SWAP_WORD)
#define JCU_SWAP_BGRA8888_PIXEL10                   (JCU_SWAP_WORD_AND_BYTE)

/* RGB565 Output SwapSetting */
#define JCU_SWAP_RGB565_PIXEL0123                   (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL0123          (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)
#define JCU_SWAP_RGB565_PIXEL1032                   (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL1032          (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_RGB565_PIXEL2301                   (JCU_SWAP_WORD)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL2301          (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_RGB565_PIXEL3210                   (JCU_SWAP_NONE)
#define JCU_SWAP_RGB565_BYTESWAP_PIXEL3210          (JCU_SWAP_BYTE)

/* YUV422 Input/Output SwapSetting */
#define JCU_SWAP_CB_Y0_CR_Y1_PIXEL0123              (JCU_SWAP_LONG_WORD_AND_WORD)
#define JCU_SWAP_Y0_CB_Y1_CR_PIXEL0123              (JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE)
#define JCU_SWAP_CR_Y1_CB_Y0_PIXEL0123              (JCU_SWAP_LONG_WORD)
#define JCU_SWAP_Y1_CR_Y0_CB_PIXEL0123              (JCU_SWAP_LONG_WORD_AND_BYTE)
#define JCU_SWAP_CB_Y0_CR_Y1_PIXEL2301              (JCU_SWAP_WORD)
#define JCU_SWAP_Y0_CB_Y1_CR_PIXEL2301              (JCU_SWAP_WORD_AND_BYTE)
#define JCU_SWAP_CR_Y1_CB_Y0_PIXEL2301              (JCU_SWAP_NONE)
#define JCU_SWAP_Y1_CR_Y0_CB_PIXEL2301              (JCU_SWAP_BYTE)

#endif /* _SH */

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* JCU_SWAP_H */

