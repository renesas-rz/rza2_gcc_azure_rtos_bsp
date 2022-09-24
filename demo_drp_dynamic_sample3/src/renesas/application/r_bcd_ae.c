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

#include "r_bcd_ae.h"
#include "r_bcd_camera.h"
#include "draw.h"
#include "r_mmu_drv_api.h"
#include "r_riic_drv_api.h"
#include "r_devlink_wrapper.h"
#include "r_riic_drv_sc_cfg.h"
#include "mipi_camera.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#define MIPI_CAMERA_SUBADD  (0x20)
#define MASK_8BIT           (0xFFu)

/**********************************************************************************************************************
Imported global variables and functions (from other files)
**********************************************************************************************************************/

/**********************************************************************************************************************
Private global variables and functions
**********************************************************************************************************************/
static void set_camera_gain(uint8_t gain);

/**********************************************************************************************************************
* Function Name: R_BCD_AeInit
* Description  : Initialize AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
**********************************************************************************************************************/
uint8_t R_BCD_AeInit(r_bcd_ae_setting_t* pAeSet)
{
    pAeSet->mode           = R_BCD_AE_MODE_INIT;
    pAeSet->brightness     = 0;
    pAeSet->global_gain    = GAIN_DEFAULT;
    /* Cast to an appropriate type */
    set_camera_gain((uint8_t)pAeSet->global_gain);
    return R_BCD_AE_MODE_INIT;
}
/**********************************************************************************************************************
* End of function R_BCD_AeInit
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_BCD_AeStop
* Description  : Stop AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
**********************************************************************************************************************/
uint8_t R_BCD_AeStop(r_bcd_ae_setting_t* pAeSet)
{
    pAeSet->mode           = R_BCD_AE_MODE_STOP;
    pAeSet->global_gain    = GAIN_DEFAULT;
    /* Cast to an appropriate type */
    set_camera_gain((uint8_t)pAeSet->global_gain);
    return R_BCD_AE_MODE_STOP;
}
/**********************************************************************************************************************
* End of function R_BCD_AeStop
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_BCD_AeStart
* Description  : Start AE
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
**********************************************************************************************************************/
uint8_t R_BCD_AeStart(r_bcd_ae_setting_t* pAeSet)
{
    pAeSet->mode           = R_BCD_AE_MODE_INIT;

    return R_BCD_AE_MODE_INIT;
}
/**********************************************************************************************************************
* End of function R_BCD_AeStart
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_BCD_AeGetMode
* Description  : Get present AE mode 
* Arguments    : pAeSet
*              :   AE setting struct address
* Return Value : AE mode
**********************************************************************************************************************/
uint8_t R_BCD_AeGetMode(r_bcd_ae_setting_t* pAeSet)
{
    return pAeSet->mode;
}
/**********************************************************************************************************************
* End of function R_BCD_AeGetMode
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_BCD_AeMakeGammaTable
* Description  : Make gamma table
* Arguments    : pAeSet
*              :   AE setting struct address
*              : gamma
*              :   Gamma value
*              : pTable
*              :   Gamma table address
* Return Value : -
**********************************************************************************************************************/
void    R_BCD_AeMakeGammaTable(r_bcd_ae_setting_t* pAeSet, double gamma, uint8_t* pTable)
{
    int32_t i;
    
    pAeSet->brightness = 0;
    if (pAeSet->mode != R_BCD_AE_MODE_STOP)
    {
        pAeSet->mode = R_BCD_AE_MODE_INIT;
    }
    for( i = 0; i < 256; i++ )
    {
        /* Making Gamma teble */
        pTable[i] = (uint8_t)(pow((double)i / 255.0, 1.0 / gamma) * 255.0);
        if ((pTable[i] >= BRIGHTNESS_SET) && (pAeSet->brightness == 0))
        {
            /* set brightness */
            pAeSet->brightness = (uint16_t)i;
        }
    }
}
/**********************************************************************************************************************
* End of function R_BCD_AeMakeGammaTable
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_BCD_AeRunAutoExpousure
* Description  : Set average luminance for AE 
* Arguments    : pAeSet
*              :   AE setting struct address
*              : ave_lum
*              :   Average luminance
* Return Value : AE mode
**********************************************************************************************************************/
uint8_t R_BCD_AeRunAutoExpousure(r_bcd_ae_setting_t* pAeSet, uint16_t ave_lum)
{
    int8_t gain_chg = 0;

    /* Cast to an appropriate type */
    int16_t nowgain = (int16_t)pAeSet->global_gain;

    if (pAeSet->mode != R_BCD_AE_MODE_STOP)
    {
        /* Adjust gain */
        if (ave_lum < (pAeSet->brightness - (BRIGHTNESS_TH * 4)))
        {
            gain_chg = 4;
        }
        else if (ave_lum < (pAeSet->brightness - (BRIGHTNESS_TH * 2)))
        {
            gain_chg = 2;
        }
        else if (ave_lum < (pAeSet->brightness - BRIGHTNESS_TH))
        {
            /* When fine tuning, do it every other times. */
            if (pAeSet->mode == R_BCD_AE_MODE_INIT)
            {
                gain_chg = 1;
            }
        }
        else if (ave_lum > (pAeSet->brightness + (BRIGHTNESS_TH * 4)))
        {
            gain_chg = -4;
        }
        else if (ave_lum > (pAeSet->brightness + (BRIGHTNESS_TH * 2)))
        {
            gain_chg = -2;
        }
        else if (ave_lum > (pAeSet->brightness + BRIGHTNESS_TH))
        {
            /* When fine tuning, do it every other times. */
            if (pAeSet->mode == R_BCD_AE_MODE_INIT)
            {
                gain_chg = -1;
            }
        }
        else
        {
            /* DO NOTHING */
            ;
        }
        /* Init gain mode */
        pAeSet->mode = R_BCD_AE_MODE_INIT;

        nowgain += gain_chg;
        if( nowgain > GAIN_MAX )
        {
            nowgain = GAIN_MAX;
        }
        if( nowgain < GAIN_MIN )
        {
            nowgain = GAIN_MIN;
        }
        /* Cast to an appropriate type */
        if( nowgain != (int16_t)pAeSet->global_gain )
        {
            /* Cast to an appropriate type */
            pAeSet->global_gain = (uint16_t)nowgain;
            /* Cast to an appropriate type */
            set_camera_gain((uint8_t)pAeSet->global_gain);
            pAeSet->mode = R_BCD_AE_MODE_SET_IN;
        }
    }
    return pAeSet->mode;
}
/**********************************************************************************************************************
* End of function R_BCD_AeRunAutoExpousure
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: set_camera_gain
* Description  : Set Camera Gain
* Arguments    : gain        : gain 0:-12dB --- 12:0dB --- 36:+24dB
* Return Value : none
**********************************************************************************************************************/
static void set_camera_gain(uint8_t gain) 
{
    if( gain <= GAIN_MAX )
    {
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5A, ae_setting_tbl[gain][0]);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5B, ae_setting_tbl[gain][1]);
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x57, ae_setting_tbl[gain][2]);
    }
}
/**********************************************************************************************************************
* End of function set_camera_gain
**********************************************************************************************************************/

/* End of File */
