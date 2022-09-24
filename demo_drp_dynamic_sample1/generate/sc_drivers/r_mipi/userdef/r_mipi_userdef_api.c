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
/**************************************************************************
* File Name : r_mipi_userdef_api.c
* Version : 1.12
* Description : RZ/A2M MIPI driver user function
**************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include     "r_typedefs.h"
#include     "iodefine.h"
#include     "r_intc_drv_api.h"
#include     "r_mmu_drv_api.h"
#include     "r_mipi_api.h"
#include     "r_mipi_user.h"

/******************************************************************************
Macro definitions
******************************************************************************/
/* For MIPI */
#define MSTP75_BIT              (0x20u)
/* For VIN */
#define MSTP66_BIT              (0x40u)
#define STBRQ27_BIT             (0x80u)
#define STBAK27_BIT             (0x80u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static void set_interrupt_settings (const uint32_t user_num);

/**********************************************************************
*
* Function Name: R_MIPI_CPUVAddrToSysPAddr
* Description :  This function performs the following processing:
*                - Get the System Physical address of the given virtual memory address.
* Arguments :    vaddr : virtual address
* Return Value : physical address
**********************************************************************/
uint32_t R_MIPI_CPUVAddrToSysPAddr(uint32_t vaddr) 
{
    e_mmu_err_t ret;
    uint32_t  paddress = 0;

    ret = R_MMU_VAtoPA(vaddr, &paddress);
    if( MMU_SUCCESS != ret)
    {
        paddress = 0;
    }
    return paddress;
}   /* End of function R_MIPI_CPUVAddrToSysPAddr() */

/**********************************************************************
*
* Function Name: R_MIPI_OnInitialize
* Description :  MIPI and VIN User definition part of initialize
* Arguments :    user_num : user specify argument
* Return Value : none
**********************************************************************/
void R_MIPI_OnInitialize (const uint32_t user_num) 
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    /* MIPI */
    /* Standby control register 7 (STBCR7)
        b5      --0-----;  MSTP75 : 0 : MIPI enable */
    reg_data    = (uint32_t)CPG.STBCR7.BYTE & (uint32_t)~MSTP75_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBCR7.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR7.BYTE;

    /* VIN */
    /* Standby control register 6 (STBCR6)
        b6      -0------;  MSTP66 : 0 : VIN enable */
    reg_data    = (uint32_t)CPG.STBCR6.BYTE & (uint32_t)~MSTP66_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBCR6.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR6.BYTE;

    /* Standby Request Register 2 (STBREQ2)
        b7      0--------;  STBRQ27 : The standby request to VIN is invalid. */
    reg_data    = (uint32_t)CPG.STBREQ2.BYTE & (uint32_t)~STBRQ27_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBREQ2.BYTE = (uint8_t)reg_data;

    /* Standby Acknowledge Register 2 (STBACK2)
        b5      --*-----;  STBAK27 : Standby acknowledgment from VIN. */
    while (((uint32_t)CPG.STBACK2.BYTE & (uint32_t)STBAK27_BIT) != 0u)
    {
        /* Wait for the STBAK27 to be cleared to 0. */
        ;
    }

    set_interrupt_settings(user_num);

}   /* End of function R_MIPI_OnInitialize() */

/**********************************************************************
*
* Function Name: R_MIPI_OnFinalize
* Description :  MIPI User definition part of finalize
* Arguments :    user_num : user specify argument
* Return Value : none
**********************************************************************/
void R_MIPI_OnFinalize( const uint32_t user_num ) 
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    /* VIN */
    /* Standby Request Register 2 (STBREQ2)
        b7      1-------;  STBRQ27 : The standby request to VIN is valid. */
    reg_data    = (uint32_t)CPG.STBREQ2.BYTE | (uint32_t)STBRQ27_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBREQ2.BYTE = (uint8_t)reg_data;

    /* Standby Acknowledge Register 2 (STBACK2)
        b5      *-------;  STBAK27 : Standby acknowledgment from VIN. */
    while (((uint32_t)CPG.STBACK2.BYTE & (uint32_t)STBAK27_BIT) == 0u)
    {
        /* Wait for the STBAK27 to be set to 1. */
        ;
    }

    /* Standby control register 6 (STBCR6)
        b5      -1------;  MSTP66 : 0 : VIN disable */
    reg_data    = (uint32_t)CPG.STBCR6.BYTE | (uint32_t)MSTP66_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBCR6.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR6.BYTE;

    /* MIPI */
    /* Standby control register 7 (STBCR7)
        b5      --1-----;  MSTP75 : 0 : MIPI disable */
    reg_data    = (uint32_t)CPG.STBCR7.BYTE | (uint32_t)MSTP75_BIT;

    /* This casting is valid because uint8_t and unsigned char is same 1byte length */
    CPG.STBCR7.BYTE  = (uint8_t)reg_data;

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR7.BYTE;

}   /* End of function R_MIPI_OnFinalize() */

/**********************************************************************
*
* Function Name: set_interrupt_settings
* Description :  Setup MIPI and VIN interrupt
* Arguments :    user_num : user specify argument
* Return Value : none
**********************************************************************/
static void set_interrupt_settings (const uint32_t user_num)
{
    int32_t         ret_code;

    /* MIPI */
    ret_code = R_INTC_RegistIntFunc(INTC_ID_MIPI_CSII, R_MIPI_InterruptHandler);
    if (ret_code != INTC_SUCCESS)
    {
        /* DO NOTHING */
        ;
    }

    if (ret_code == INTC_SUCCESS)
    {
        /* This casting is valid because the value never out of range of 0xFF */
        ret_code = R_INTC_SetPriority (INTC_ID_MIPI_CSII, (uint8_t) MIPI_INTERRUPT_PRIORITY);
        if (ret_code != INTC_SUCCESS)
        {
            /* DO NOTHING */
            ;
        }
    }

    if (ret_code == INTC_SUCCESS)
    {
        /* This casting is valid because the value never out of range of 0xFF */
        ret_code = R_INTC_Enable(INTC_ID_MIPI_CSII);
        if (ret_code != INTC_SUCCESS)
        {
            /* DO NOTHING */
            ;
        }
    }

    /* MIPI */
    ret_code = R_INTC_RegistIntFunc(INTC_ID_VIN_VINI, R_VIN_InterruptHandler);
    if (ret_code != INTC_SUCCESS)
    {
        /* DO NOTHING */
        ;
    }

    if (ret_code == INTC_SUCCESS)
    {
        /* This casting is valid because the value never out of range of 0xFF */
        ret_code = R_INTC_SetPriority(INTC_ID_VIN_VINI, (uint8_t)VIN_INTERRUPT_PRIORITY);
        if (ret_code != INTC_SUCCESS)
        {
            /* DO NOTHING */
            ;
        }
    }

    if (ret_code == INTC_SUCCESS)
    {
        /* This casting is valid because the value never out of range of 0xFF */
        ret_code = R_INTC_Enable(INTC_ID_VIN_VINI);
        if (ret_code != INTC_SUCCESS)
        {
            /* DO NOTHING */
            ;
        }
    }

}   /* End of function set_interrupt_settings() */

/* End of File */

