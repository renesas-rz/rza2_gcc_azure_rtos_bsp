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
 * File Name :   r_ceu_user.h
 * @file         r_ceu_user.h
 * @version      1.12
 * @brief        RZ/A2M CEU driver user-defined header
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_typedefs.h"

#ifndef R_CEU_USER_H
#define R_CEU_USER_H

#ifdef  __cplusplus
extern  "C"
{
#endif  /* __cplusplus */

/******************************************************************************
Macro definitions
******************************************************************************/
/* Version Number of API */
#define CEU_RZA2_VERSION_MAJOR (1)
#define CEU_RZA2_VERSION_MINOR (11)


#define CEU_INTERRUPT_PRIORITY  (5u)

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/

/**************************************************************************//**
 * Function Name: R_CEU_CPUVAddrToSysPAddr
 * @brief       Cet the System Physical address of the given virtual memory address.
 * @param[in]   vaddr          : virtual address
 * @retval      none
 *****************************************************************************/
uint32_t R_CEU_CPUVAddrToSysPAddr(uint32_t vaddr);

/**************************************************************************//**
 * Function Name: R_CEU_OnInitialize
 * @brief       CEU User difinistion part Init
 * @param[in]   user_num          : user param
 * @retval      none
 *****************************************************************************/
void R_CEU_OnInitialize(const uint32_t user_num);

/**************************************************************************//**
 * Function Name: R_CEU_OnFinalize
 * @brief       CEU User difinistion part final
 * @param[in]   user_num          : user param
 * @retval      none
 *****************************************************************************/
void R_CEU_OnFinalize(const uint32_t user_num);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* R_CEU_USER_H */

