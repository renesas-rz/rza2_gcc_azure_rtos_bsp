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
 * @headerfile     r_rvapi_mipi.h
 * @brief          RVAPI for MIPI function header
 * @version        1.12
 * H/W Platform    RZ/A2M
 *****************************************************************************/
/**************************************************************************//**
 * @ingroup R_SW_PKG_94_VIDEO_MIPI_API
 * @defgroup R_SW_PKG_93_VIDEO_MIPI Video MIPI
 * @brief MIPI Unit Module
 *
 * @anchor R_SW_PKG_94_VIDEO_MIPI_API_SUMMARY
 * @par Summary
 *
 * This middleware module provides functionality for a user to utilise the
 * MIPI.
 *
 * @anchor R_SW_PKG_93_VIDEO_MIPI_API_INSTANCES
 * @par Known Implementations:
 * This driver is used in the RZA2M.
 * @see RENESAS_APPLICATION_SOFTWARE_PACKAGE
 *
 * @see RENESAS_OS_ABSTRACTION  Renesas OS Abstraction interface
 * @{
 *****************************************************************************/
/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "r_typedefs.h"
#include "r_mipi_api.h"

/* Multiple inclusion prevention macro */
#ifndef R_RVAPI_MIPI_H
#define R_RVAPI_MIPI_H

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

/**************************************************************************//**
 * Function Name : R_RVAPI_InitializeMIPI
 * @brief       MIPI Initialize
 * @retval      None
 *****************************************************************************/
void R_RVAPI_InitializeMIPI(void);

/**************************************************************************//**
 * Function Name : R_RVAPI_TerminateMIPI
 * @brief       MIPI  Terminate
 * @retval      None
 *****************************************************************************/
void R_RVAPI_TerminateMIPI(void);

/**************************************************************************//**
 * Function Name : R_RVAPI_OpenMIPI
 * @brief       MIPI Open
 * @param[in]   config              : MIPI config
 * @retval      MIPI driver error code
 *****************************************************************************/
e_mipi_error_t R_RVAPI_OpenMIPI(const st_mipi_param_t * const config);

/**************************************************************************//**
 * Function Name : R_RVAPI_InterruptEnableMIPI
 * @brief       MIPI Interrupt Enable
 * @param[in]   param       : interrupt parameter of MIPI
 * @retval      MIPI driver error code
 *****************************************************************************/
e_mipi_error_t R_RVAPI_InterruptEnableMIPI(const st_mipi_int_t * const param);

/**************************************************************************//**
 * Function Name : R_RVAPI_SetupMIPI
 * @brief       Setup MIPI driver
 * @param[in]   setup         : setup parameter of MIPI
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_SetupMIPI(const st_vin_setup_t * const setup);

/**************************************************************************//**
 * Function Name : R_RVAPI_SetBufferMIPI
 * @brief       set buffer address
 * @param[in]   buffer_no     : Select buffer base (MB1, MB2, MB3)
                buffer        : capture mode
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_SetBufferMIPI(const uint8_t buffer_no, const uint8_t * const buffer);

/**************************************************************************//**
 * Function Name : R_RVAPI_CaptureStartMIPI
 * @brief       capture start
 * @param[in]   none
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_CaptureStartMIPI(void);

/**************************************************************************//**
 * Function Name : R_RVAPI_CaptureStopMIPI
 * @brief       Stop capture
 * @param[in]   None
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_CaptureStopMIPI(void);

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_RVAPI_MIPI_H */
