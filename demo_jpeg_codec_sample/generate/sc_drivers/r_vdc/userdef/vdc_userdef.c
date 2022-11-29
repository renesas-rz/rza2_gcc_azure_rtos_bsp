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
 * File Name :   vdc_userdef.c
 * @file         vdc_userdef.c
 * @version      1.12
 * @brief        RZ/A2M VDC driver user function
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include     "r_typedefs.h"
#include     "iodefine.h"
#include     "r_intc_drv_api.h"
#include     "r_mmu_drv_api.h"
#include     "r_rvapi_vdc.h"
#include     "r_vdc.h"
#include     "r_vdc_user.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define STP81_BIT               (0x02u)
#define STBRQ25_BIT             (0x20u)
#define STBAK25_BIT             (0x20u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static const uint16_t interrupt_id_table[VDC_INT_TYPE_NUM] =
{
    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_S0_VI_VSYNC0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_S0_LO_VSYNC0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_S0_VSYNCERR0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_GR3_VLINE0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_S0_VFIELD0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_IV1_VBUFERR0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_IV3_VBUFERR0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_IV5_VBUFERR0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_IV6_VBUFERR0,

    /* Cast to an appropriate type */
    (uint16_t)INTC_ID_VDC6_S0_WLINE0,
};

static void SetInterrupt (const uint32_t user_num);

/**************************************************************************//**
 * @fn          R_VDC_CPUVAddrToSysPAddr
 * @brief       Physical address translation
 *
 *              Description:<br>
 *              This function performs the following processing:
 *              - Get the System Physical address of the given virtual memory address.
 * @param[in]   vaddr               : virtual address
 * @retval      physical address
 *****************************************************************************/
uint32_t R_VDC_CPUVAddrToSysPAddr(uint32_t vaddr) 
{
    e_mmu_err_t ret;
    uint32_t  paddress = 0;

    ret = R_MMU_VAtoPA(vaddr, &paddress);
    if( MMU_SUCCESS != ret)
    {
        paddress = 0;
    }
    return paddress;
}   /* End of function R_VDC_CPUVAddrToSysPAddr() */

/**************************************************************************//**
 * @fn          R_VDC_OnInitialize
 * @brief       VDC User difinistion part Init
 * @param[in]   user_num          :User defined number
 * @retval      none
 *****************************************************************************/
void R_VDC_OnInitialize (const uint32_t user_num)   
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    /* Cast to an appropriate type */
    if ((vdc_channel_t)user_num == VDC_CHANNEL_0)
    {
        /* Standby control register 8 (STBCR8)
            b1      ------0-;  MSTP81 : 0 : Video display controller channel 0 & LVDS enable */
        reg_data    = (uint32_t)CPG.STBCR8.BYTE & (uint32_t)~STP81_BIT;

        /* Cast to an appropriate type */
        CPG.STBCR8.BYTE  = (uint8_t)reg_data;

        /* In order to reflect the change, a dummy read should be done. */
        dummy_read = CPG.STBCR8.BYTE;

        /* Standby Request Register 2 (STBREQ2)
            b5      --0-----;  STBRQ25 : The standby request to VDC channel 0 is invalid. */
        reg_data    = (uint32_t)CPG.STBREQ2.BYTE & (uint32_t)~STBRQ25_BIT;

        /* Cast to an appropriate type */
        CPG.STBREQ2.BYTE = (uint8_t)reg_data;

        /* Standby Acknowledge Register 2 (STBACK2)
            b5      --*-----;  STBAK25 : Standby acknowledgment from VDC channel 0. */
        while (((uint32_t)CPG.STBACK2.BYTE & (uint32_t)STBAK25_BIT) != 0u)
        {
            /* Wait for the STBAK25 to be cleared to 0. */
            ;
        }
    }
    SetInterrupt(user_num);

}   /* End of function R_VDC_OnInitialize() */

/**************************************************************************//**
 * Function Name: R_VDC_OnFinalize
 * @brief       VDC User difinistion part final
 * @param[in]   user_num          :User defined number
 * @retval      none
******************************************************************************/
void R_VDC_OnFinalize( const uint32_t user_num )   
{
    uint32_t            reg_data;
    volatile uint8_t    dummy_read;

    /* Cast to an appropriate type */
    if ((vdc_channel_t)user_num == VDC_CHANNEL_0)
    {
        /* Standby Request Register 2 (STBREQ2)
            b5      --1-----;  STBRQ25 : The standby request to VDC channel 0 is valid. */
        reg_data    = (uint32_t)CPG.STBREQ2.BYTE | (uint32_t)STBRQ25_BIT;

        /* Cast to an appropriate type */
        CPG.STBREQ2.BYTE = (uint8_t)reg_data;

        /* Standby Acknowledge Register 2 (STBACK2)
            b5      --*-----;  STBAK25 : Standby acknowledgment from VDC channel 0. */
        while (((uint32_t)CPG.STBACK2.BYTE & (uint32_t)STBAK25_BIT) == 0u)
        {
            /* Wait for the STBAK25 to be set to 1. */
            ;
        }

        /* Standby control register 8 (STBCR8)
            b1      ------1-;  MSTP81 : 1 : Video display controller channel 0 & LVDS disable */
        reg_data    = (uint32_t)CPG.STBCR8.BYTE | (uint32_t)STP81_BIT;

        /* Cast to an appropriate type */
        CPG.STBCR8.BYTE  = (uint8_t)reg_data;
    }

    /* In order to reflect the change, a dummy read should be done. */
    dummy_read = CPG.STBCR8.BYTE;

}   /* End of function R_VDC_OnFinalize() */

/**************************************************************************//**
 * @fn          SetInterrupt
 * @brief       Setup VDC interrupt
 * @param[in]   user_num          :User defined number
 * @retval      None
******************************************************************************/
static void SetInterrupt (const uint32_t user_num) 
{
    void         (* func)(uint32_t int_sense);
    int32_t         ret_code;
    uint16_t        int_id;
    vdc_int_type_t  int_type;

    /* Cast to an appropriate type */
    for (int_type = (vdc_int_type_t)0; int_type < VDC_INT_TYPE_NUM; int_type++)
    {
        int_id  = interrupt_id_table[int_type];
        func    = R_VDC_GetISR(VDC_CHANNEL_0, int_type);

        ret_code = R_INTC_RegistIntFunc(int_id, func);
        if (ret_code != INTC_SUCCESS)
        {
            break;
        }
        ret_code = R_INTC_SetPriority(int_id, VDC_INTERRUPT_PRIORITY);
        if (ret_code != INTC_SUCCESS)
        {
            break;
        }
        ret_code = R_INTC_Enable(int_id);
        if (ret_code != INTC_SUCCESS)
        {
            break;
        }
    }
}   /* End of function SetInterrupt() */

/* End of File */

