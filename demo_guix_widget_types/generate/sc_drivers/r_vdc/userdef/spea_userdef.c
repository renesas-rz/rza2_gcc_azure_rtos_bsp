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
 * File Name :   spea_userdef.c
 * @file         spea_userdef.c
 * @version      1.12
 * @brief        RZ/A2M SPEA driver user function
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_typedefs.h"
#include    "r_mmu_drv_api.h"
#include    "r_spea.h"
#include    "r_spea_user.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * @fn          R_SPEA_CPUVAddrToSysPAddr
 * @brief       Set the System Physical address of the given virtual memory address.
 * @param[in]   vaddr          : virtual address
 * @retval      none
 *****************************************************************************/
uint32_t R_SPEA_CPUVAddrToSysPAddr(uint32_t vaddr)
{
    e_mmu_err_t ret;
    uint32_t  paddress = 0;

    ret = R_MMU_VAtoPA(vaddr, &paddress);
    if( MMU_SUCCESS != ret)
    {
        paddress = 0;
    }
    return paddress;
}   /* End of function R_SPEA_CPUVAddrToSysPAddr() */

/* End of File */

