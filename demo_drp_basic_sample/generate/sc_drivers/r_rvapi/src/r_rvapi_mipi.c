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
 * File Name :   r_rvapi_mipi.c
 * @file         r_rvapi_mipi.c
 * @version      1.12
 * @brief        RVAPI for MIPI function
 ******************************************************************************/

/******************************************************************************
 Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include    "r_rvapi_mipi.h"
#include    "r_mipi_user.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Typedef definitions
 ******************************************************************************/

/******************************************************************************
 Imported global variables and functions (from other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables and functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/**************************************************************************//**
 * Function Name : R_RVAPI_InitializeMIPI
 * @brief       MIPI Initialize
 * @retval      None
 *****************************************************************************/
void R_RVAPI_InitializeMIPI(void) 
{
    R_MIPI_Initialize (&R_MIPI_OnInitialize, 0);

} /* End of function R_RVAPI_InitializeMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_TerminateMIPI
 * @brief       MIPI  Terminate
 * @retval      None
 *****************************************************************************/
void R_RVAPI_TerminateMIPI(void) 
{
    R_MIPI_InterruptDisable ();
    R_MIPI_Close (&R_MIPI_OnFinalize, 0);
} /* End of function R_RVAPI_TerminateMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_OpenMIPI
 * @brief       MIPI Open
 * @param[in]   config              : MIPI config
 * @retval      MIPI driver error code
 *****************************************************************************/
e_mipi_error_t R_RVAPI_OpenMIPI(const st_mipi_param_t * const config) 
{
    e_mipi_error_t mipi_error = MIPI_OK;

    /* This implicit casting is valid because either values are pointer */
    if( NULL != config )
    {
        R_MIPI_Open(config);
    }
    else
    {
        mipi_error = MIPI_PARAM_ERR;
    }
    return mipi_error;
} /* End of function R_RVAPI_OpenMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_InterruptEnableMIPI
 * @brief       MIPI Interrupt Enable
 * @param[in]   param       : interrupt parameter of MIPI
 * @retval      MIPI driver error code
 *****************************************************************************/
e_mipi_error_t R_RVAPI_InterruptEnableMIPI(const st_mipi_int_t * const param) 
{
    e_mipi_error_t mipi_error = MIPI_OK;

    /* This implicit casting is valid because either values are pointer */
    if( NULL != param )
    {
        R_MIPI_InterruptEnable (param);
    }
    else
    {
        mipi_error = MIPI_PARAM_ERR;
    }
    return mipi_error;

} /* End of function R_RVAPI_InterruptEnableMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_SetupMIPI
 * @brief       Setup MIPI driver
 * @param[in]   setup         : setup parameter of MIPI
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_SetupMIPI(const st_vin_setup_t * const setup) 
{
    e_mipi_error_t mipi_error = MIPI_OK;
    
    /* This implicit casting is valid because either values are pointer */
    if( NULL != setup )
    {
        mipi_error = R_MIPI_Setup(setup);
    }
    else
    {
        mipi_error = MIPI_PARAM_ERR;
    }
    return mipi_error;
} /* End of function R_RVAPI_SetupMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_SetBufferMIPI
 * @brief       set buffer address
 * @param[in]   buffer_no     : Select buffer base (MB1, MB2, MB3)
                buffer        : capture mode
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_SetBufferMIPI 
                (const uint8_t buffer_no, const uint8_t * const buffer)
{
    e_mipi_error_t mipi_error = MIPI_OK;
    
    /* This implicit casting is valid because either values are pointer */
    if( NULL != buffer )
    {
        mipi_error = R_MIPI_SetBufferAdr(buffer_no, buffer);
    }
    else
    {
        mipi_error = MIPI_PARAM_ERR;
    }
    return mipi_error;
} /* End of function R_RVAPI_SetBufferMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_CaptureStartMIPI
 * @brief       capture start
 * @param[in]   none
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_CaptureStartMIPI(void) 
{
    e_mipi_error_t mipi_error;

    mipi_error = R_MIPI_CaptureStart(MIPI_CONTINUOUS_MODE);

    return mipi_error;
} /* End of function R_RVAPI_CaptureStartMIPI() */

/**************************************************************************//**
 * Function Name : R_RVAPI_CaptureStopMIPI
 * @brief       Stop capture
 * @param[in]   None
 * @retval      MIPI driver error code
 ******************************************************************************/
e_mipi_error_t R_RVAPI_CaptureStopMIPI(void) 
{
    return R_MIPI_CaptureStop();
} /* End of function R_RVAPI_CaptureStopMIPI() */
