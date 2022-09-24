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
 * File Name :    ceu_portsetting.c
 * @file          ceu_portsetting.c
 * @brief        CEU port setting functions
 ******************************************************************************/

/*******************************************************************************
 Includes <System Includes>, "Project Includes"
 *******************************************************************************/
#include "r_typedefs.h"
#include "iodefine.h"
#include "r_gpio_drv_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Enumerated Types
 ******************************************************************************/

/******************************************************************************
 Structures
 ******************************************************************************/

/******************************************************************************
 Global Vaiables
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/

/******************************************************************************
 Function prototypes
 ******************************************************************************/
void CEU_CameraPortSetting(uint32_t param); 


/**************************************************************************//**
 * Function Name : CEU_CameraPortSetting
 * @brief       Camera port setup
 * @param[in]   param     :
 * @retval      None
 ******************************************************************************/
void CEU_CameraPortSetting(uint32_t param)
{
    int_t err;
    int_t gpio_handle;

    /*  Camera module for RZ/A2M Eva board
            VIO_HD   :     ... P6_3
            VIO_VD   :     ... P6_2
            VIO_CLK  :     ... P6_1
            VIO_D0   :     ... PH_1
            VIO_D1   :     ... PH_0
            VIO_D2   :     ... PE_6
            VIO_D3   :     ... PE_5
            VIO_D4   :     ... PE_4
            VIO_D5   :     ... PE_3
            VIO_D6   :     ... PE_2
            VIO_D7   :     ... PE_1
    */

    st_r_drv_gpio_pin_list_t pin_ceu;

    static const r_gpio_port_pin_t pin_ceu_table[] =
    {
        /* Cast to an appropriate type */
        GPIO_PORT_6_PIN_3,
        /* Cast to an appropriate type */
        GPIO_PORT_6_PIN_2,
        /* Cast to an appropriate type */
        GPIO_PORT_6_PIN_1,
        /* Cast to an appropriate type */
        GPIO_PORT_H_PIN_1,
        /* Cast to an appropriate type */
        GPIO_PORT_H_PIN_0,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_6,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_5,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_4,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_3,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_2,
        /* Cast to an appropriate type */
        GPIO_PORT_E_PIN_1,
    };

    gpio_handle = direct_open("gpio", 0);
    if ( gpio_handle < 0 )
    {
        /* stop execute */
        while (1)
        {
            /* Spin here forever..*/
            ;
        }
    }

    /* === CEU === */
    /* ---- Port initialize ---- */
    pin_ceu.p_pin_list = pin_ceu_table;
    pin_ceu.count = (sizeof(pin_ceu_table)) / (sizeof(pin_ceu_table[0]));
    err = direct_control(gpio_handle, CTL_GPIO_INIT_BY_PIN_LIST, &pin_ceu);
    if ( err < 0 )
    {
        /* stop execute */
        while (1)
        {
            /* Spin here forever..*/
            ;
        }
    }

    direct_close(gpio_handle);

} /* End of function CEU_CameraPortSetting() */
