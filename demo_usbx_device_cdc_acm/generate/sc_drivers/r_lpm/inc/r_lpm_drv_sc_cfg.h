/******************************************************************************
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
 * Renesas reserves the right, without notice, to make changes to this
 * software and to discontinue the availability of this software. By using this
 * software, you agree to the additional terms and conditions found by
 * accessing the following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 *****************************************************************************/
/******************************************************************************
* File Name    : r_lpm_drv_sc_cfg.h
* Date         : 2019-03-01
* version      : 1.11.0
* Description  : LPM configuration table updated by Smart Configurator
******************************************************************************/

#include "r_typedefs.h"
#include "r_lpm_drv_api.h"

#ifndef R_LPM_DRV_SC_CFG_H_
#define R_LPM_DRV_SC_CFG_H_

#if defined(__cplusplus)
extern "C" {
#endif
#if defined(__cplusplus)
}
#endif


/**
 * @section LPM_SC_TABLE Smart configurator settings table.
 */
static const st_r_drv_lpm_sc_config_t LPM_SC_TABLE[] =
{
    /* This code is auto-generated. Do not edit manually */
    {
        LPM_MODE_SOFTWARE_STANDBY, 
        LPM_PIN_STATUS_RETAINED, 
        LPM_SSTANDBY_ENABLE_IRQ, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        LPM_SS_CANCEL_FACTOR_DISABLED, 
        7, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_FALLING_EDGE, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_FALLING_EDGE, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_PIN_CANCEL_FACTOR_DISABLED, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_DS_CANCEL_FACTOR_DISABLED, 
        LPM_RETENTION_RAM_NOT_RETAINED, 
        LPM_RETENTION_RAM_NOT_RETAINED, 
        LPM_RETENTION_RAM_NOT_RETAINED, 
        LPM_RETENTION_RAM_RETAINED, 
        7, 
        LPM_REBOOT_TYPE_EXTERNAL_MEMORY_BOOT, 
    },
    /* End of modification */
};


#endif /* R_LPM_DRV_SC_CFG_H_ */
