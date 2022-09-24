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
 * File Name :     r_rvapi_ceu.h
 * @headerfile     r_rvapi_ceu.h
 * @brief          RVAPI for CEU function header
 * @version        1.12
 * H/W Platform    RZ/A2M
 *****************************************************************************/

/**************************************************************************//**
 * @ingroup R_SW_PKG_95_VIDEO_CEU_API
 * @defgroup R_SW_PKG_93_VIDEO_CEU Video CEU
 * @brief CEU Unit Module
 *
 * @anchor R_SW_PKG_93_VIDEO_CEU_API_SUMMARY
 * @par Summary
 *
 * This middleware module provides functionality for a user to utilise the
 * CEU unit, allowing configuration and capturing.
 *
 * @anchor R_SW_PKG_93_VIDEO_CEU_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M Software Package.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_ceu.h"

/* Multiple inclusion prevention macro */
#ifndef R_RVAPI_CEU_H
#define R_RVAPI_CEU_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

/**
 * @fn       R_RVAPI_InitializeCEU
 * @brief    CEU initialization processing
 *
 * @return None.
 */
void R_RVAPI_InitializeCEU(void);

/**
 * @fn       R_RVAPI_TerminateCEU
 * @brief    Closes CEU Operation
 *
 * @return None.
 */
void R_RVAPI_TerminateCEU(void);

/**
 * @fn       R_RVAPI_PortSettingCEU
 * @brief    Calls the function passed as argument
 *          'port_func'
 *
 * @param[in] port_func: Pointer to a function relating
 *                       to CEU port settings.
 *
 * @return None.
 */
void R_RVAPI_PortSettingCEU(void (* const port_func)(uint32_t));

/**
 * @fn          R_RVAPI_OpenCEU
 * @brief       CEU Setting main processing
 *
 * @param[in]   config: ceu config
 *
 * @retval      CEU_ER: driver error code
 */
ceu_error_t R_RVAPI_OpenCEU(const ceu_config_t * const config);

/**
 * @fn          R_RVAPI_CaptureStartCEU
 * @brief       1 frame capture start
 *
 * @param[in]   cayr:      Capture buffer (Y)
 * @param[in]   cacr:      Capture buffer (CbCr)
 * @param[in]   chdw:      stride
 * @param[in]   auto_capture:       auto_capture on/off
 *
 * @retval      CEU_ER: driver error code
 */
ceu_error_t R_RVAPI_CaptureStartCEU(const void * cayr, const void * cacr, uint32_t chdw, ceu_onoff_t auto_capture);

/**
 * @fn          R_RVAPI_CaptureStopCEU
 * @brief       capture end
 *
 * @return None.
 */
ceu_error_t R_RVAPI_CaptureStopCEU(void);

/**
 * @fn         R_RVAPI_InterruptEnableCEU
 * @brief      CEU Interrupt Enable
 *
 * @param[in]   int_type:      Capture Event Interrupt Enable Register (CEIER)
 * @param[in]   callback:      Callback func
 *
 * @retval      CEU_ER: driver error code
 */
ceu_error_t R_RVAPI_InterruptEnableCEU(const ceu_int_type_t int_type, void (* const callback)(ceu_int_type_t));

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_RVAPI_CEU_H */
