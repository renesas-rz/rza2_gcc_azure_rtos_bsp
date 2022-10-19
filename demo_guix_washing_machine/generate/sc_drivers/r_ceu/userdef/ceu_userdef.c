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
 * File Name :   ceu_userdef.c
 * @file         ceu_userdef.c
 * @version      1.12
 * @brief        RZ/A2M CEU driver user function
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include     "r_typedefs.h"
#include     "iodefine.h"
#include     "r_intc_drv_api.h"
#include     "r_mmu_drv_api.h"
#include     "r_ceu.h"
#include     "r_ceu_user.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define STP56_BIT               (0x40u)
#define STBRQ10_BIT             (0x01u)
#define STBAK10_BIT             (0x01u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void setinterrupt (const uint32_t user_num);

/**************************************************************************//**
 * Function Name: R_CEU_CPUVAddrToSysPAddr
 * @brief       Cet the System Physical address of the given virtual memory address.
 * @param[in]   vaddr          : virtual address
 * @retval      none
 *****************************************************************************/
uint32_t R_CEU_CPUVAddrToSysPAddr(uint32_t vaddr)
{
    e_mmu_err_t ret;
    uint32_t  paddress = 0;

    ret = R_MMU_VAtoPA(vaddr, &paddress);
    if( MMU_SUCCESS != ret)
    {
        paddress = 0;
    }
    return paddress;
}   /* End of function R_CEU_CPUVAddrToSysPAddr() */

/**************************************************************************//**
 * Function Name: R_CEU_OnInitialize
 * @brief       CEU User difinistion part Init
 * @param[in]   user_num          :
 * @retval      none
 *****************************************************************************/
void R_CEU_OnInitialize(const uint32_t user_num)
{
    uint32_t reg_data;
    volatile uint8_t dummy_read;

    /* Standby control register 5 (STBCR5)
        b6      -0------;  MSTP56 : 0 : CEU enable */
    reg_data    = (uint32_t)CPG.STBCR5.BYTE & (uint32_t)~STP56_BIT;

    /* This implicit casting is valid. */
    CPG.STBCR5.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR5.BYTE;

    /* Standby Request Register 1 (STBREQ1)
        b0      -------0;  STBRQ10 : The standby request to CEU is invalid. */
    reg_data    = (uint32_t)CPG.STBREQ1.BYTE & (uint32_t)~STBRQ10_BIT;

    /* This implicit casting is valid. */
    CPG.STBREQ1.BYTE = (uint8_t)reg_data;

    /* Standby Acknowledge Register 1 (STBACK1)
        b0      -------*;  STBAK10 : Standby acknowledgement from CEU. */
    while (((uint32_t)CPG.STBACK1.BYTE & (uint32_t)STBAK10_BIT) != 0u)
    {
        /* Wait for the STBAK10 to be cleared to 0. */
        ;
    }

    setinterrupt(user_num);

} /* End of function R_CEU_OnInitialize() */

/**************************************************************************//**
 * Function Name: R_CEU_OnFinalize
 * @brief       CEU User difinistion part final
 * @param[in]   user_num          : user param
 * @retval      none
 *****************************************************************************/
void R_CEU_OnFinalize(const uint32_t user_num)
{
    uint32_t reg_data;
    volatile uint8_t dummy_read;

    /* Standby Request Register 1 (STBREQ1)
       b0      -------1;  STBRQ10 : The standby request to CEU is valid. */
    reg_data    = (uint32_t)CPG.STBREQ1.BYTE | (uint32_t)STBRQ10_BIT;

    /* This implicit casting is valid. */
    CPG.STBREQ1.BYTE = (uint8_t)reg_data;

    /* Standby Acknowledge Register 1 (STBACK1)
        b0      -------*;  STBAK10 : Standby acknowledgement from CEU. */
    while (((uint32_t)CPG.STBACK1.BYTE & (uint32_t)STBAK10_BIT) == 0u)
    {
        /* Wait for the STBAK10 to be set to 1. */
        ;
    }

    /* Standby control register 5 (STBCR5)
       b6      -1------;  MSTP56 : 1 : CEU disable */
    reg_data    = (uint32_t)CPG.STBCR5.BYTE | (uint32_t)STP56_BIT;

    /* This implicit casting is valid. */
    CPG.STBCR5.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR5.BYTE;

} /* End of function R_CEU_OnFinalize() */

/**************************************************************************//**
 * Function Name: setinterrupt
 * @brief       Setup CEU interrupt
 * @param[in]   user_num          : user param
 * @retval      None
******************************************************************************/
static void setinterrupt (const uint32_t user_num)
{
    uint16_t        int_id;
    int32_t         ret_code;

    /* Set Interrupt ID */
    int_id = (uint16_t)INTC_ID_CEU_CEUI;
    ret_code = R_INTC_RegistIntFunc(int_id, R_CEU_Isr);
    if (ret_code != INTC_SUCCESS)
    {
        /* Do Nothing */
        ;
    }

    if (ret_code == INTC_SUCCESS)
    {
        /* Set Interrupt priority */
        ret_code = R_INTC_SetPriority(int_id, (uint8_t)CEU_INTERRUPT_PRIORITY);
        if (ret_code != INTC_SUCCESS)
        {
            /* Do Nothing */
            ;
        }
    }

    if (ret_code == INTC_SUCCESS)
    {
        ret_code = R_INTC_Enable(int_id);
        if (ret_code != INTC_SUCCESS)
        {
            /* Do Nothing */
            ;
        }
    }
}   /* End of function setinterrupt() */

/* End of File */
