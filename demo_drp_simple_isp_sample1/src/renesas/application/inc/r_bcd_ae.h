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
* File Name    : r_bcd_ae.h
**********************************************************************************************************************/

/**********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include "r_typedefs.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#ifndef R_BCD_AE_H
#define R_BCD_AE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CAM_IMX219_B    (0)   /* Raspberry Pi Camera V2, WVGA */

#define ISP_CAM_MIPI        (CAM_IMX219_B)

#if   (ISP_CAM_MIPI == CAM_IMX219_B)
#else
    #error  Undefined mipi camera!
#endif

#define R_BCD_AE_MODE_STOP          (0xFFu)
#define R_BCD_AE_MODE_INIT          (0xFEu)
#define R_BCD_AE_MODE_SET_IN        (0x01u)

#define BRIGHTNESS_SET              (120)
#define BRIGHTNESS_TH               (15)

#define GAIN_MIN      (0)   /* -12dB */
#define GAIN_DEFAULT  (18)  /*   6dB */
#define GAIN_MAX      (36)  /* +24dB */

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

typedef struct
{
    uint16_t    brightness;
    uint16_t    global_gain;
    uint8_t     mode;
}
r_bcd_ae_setting_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
static const uint8_t ae_setting_tbl[GAIN_MAX+1][3] = 
{
/* GAIN_MIN */      0x00,0xFB,0x00,     // -12dB
                    0x01,0x1A,0x00,     // -11dB
                    0x01,0x3C,0x00,     // -10dB
                    0x01,0x63,0x00,     //  -9dB
                    0x01,0x86,0x00,     //  -8dB
                    0x01,0xBF,0x00,     //  -7dB
                    0x01,0xF5,0x00,     //  -6dB
                    0x02,0x32,0x00,     //  -5dB
                    0x02,0x77,0x00,     //  -4dB
                    0x02,0xC4,0x00,     //  -3dB
                    0x03,0x1A,0x00,     //  -2dB
                    0x03,0x7B,0x00,     //  -1dB
                    0x03,0xE8,0x00,     //   0dB
                    0x04,0x62,0x00,     //   1dB
                    0x04,0xEB,0x00,     //   2dB
                    0x05,0x85,0x00,     //   3dB
                    0x06,0x31,0x00,     //   4dB
                    0x06,0xF2,0x00,     //   5dB
/* GAIN_DEFAULT */  0x07,0xCB,0x00,     //   6dB
                    0x08,0xBF,0x00,     //   7dB
                    0x09,0xD0,0x00,     //   8dB
                    0x0B,0x02,0x00,     //   9dB
                    0x0C,0x5A,0x00,     //  10dB
                    0x0C,0x5A,0x1C,     //  11dB(Including analog gain 1dB)
                    0x0C,0x5A,0x35,     //  12dB(Including analog gain 2dB)
                    0x0C,0x5A,0x4B,     //  13dB(Including analog gain 3dB)
                    0x0C,0x5A,0x5F,     //  14dB(Including analog gain 4dB)
                    0x0C,0x5A,0x70,     //  15dB(Including analog gain 5dB)
                    0x0C,0x5A,0x80,     //  16dB(Including analog gain 6dB)
                    0x0C,0x5A,0x8E,     //  17dB(Including analog gain 7dB)
                    0x0C,0x5A,0x9A,     //  18dB(Including analog gain 8dB)
                    0x0C,0x5A,0xA5,     //  19dB(Including analog gain 9dB)
                    0x0C,0x5A,0xAF,     //  20dB(Including analog gain 10dB)
                    0x0C,0x5A,0xB8,     //  21dB(Including analog gain 11dB)
                    0x0C,0x5A,0xC0,     //  22dB(Including analog gain 12dB)
                    0x0C,0x5A,0xC6,     //  23dB(Including analog gain 13dB)
/* GAIN_MAX */      0x0C,0x5A,0xCD      //  24dB(Including analog gain 14dB)
};

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/* Function Name: R_BCD_AeInit */
uint8_t R_BCD_AeInit(r_bcd_ae_setting_t* pAeSet);
/* Function Name: R_BCD_AeStart */
uint8_t R_BCD_AeStart(r_bcd_ae_setting_t* pAeSet);
/* Function Name: R_BCD_AeStop */
uint8_t R_BCD_AeStop(r_bcd_ae_setting_t* pAeSet);
/* Function Name: R_BCD_AeGetMode */
uint8_t R_BCD_AeGetMode(r_bcd_ae_setting_t* pAeSet);
/* Function Name: R_BCD_AeMakeGammaTable */
void    R_BCD_AeMakeGammaTable(r_bcd_ae_setting_t* pAeSet, double gamma, uint8_t* pTable);
/* Function Name: R_BCD_AeRunAutoExpousure */
uint8_t R_BCD_AeRunAutoExpousure(r_bcd_ae_setting_t* pAeSet, uint16_t ave_lum);

#ifdef __cplusplus
}
#endif

#endif  /* R_BCD_AE_H */

/* End of File */
