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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/*****************************************************************************
 * @file        : camera_mipi_imx219_b.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : e2Studio Ver 7.8.0
 *              : GNU ARM Embedded 6.3.1.20170620
 * OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
 * H/W Platform : GR-MANGO(mbed-RZ/A2M) X71A-M01-B
 * @brief       : MIPI Camera Register Setting
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "mipi_camera.h"
#if (CAM_MIPI == CAM_IMX219_B)
#include    <stdio.h>
#include    <iodefine.h>
#include    "r_mipi_api.h"
#include    "r_mmu_drv_api.h"
#include    "r_riic_drv_api.h"
#include    "camera_mipi_imx219_b_regdata.h"
#include    "r_devlink_wrapper.h"
#include    "r_riic_drv_sc_cfg.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define MIPI_CAMERA_SUBADD    (0x20)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
int_t hi2c2 = ( -1);

/**************************************************************************//**
 * @fn          Init_Camera
 * @brief       Init_Camera
 * @param[in]   None
 * @retval      None
******************************************************************************/
void Init_Camera(void) 
{
    volatile uint32_t count;
    uint8_t  reg_sw_standby_in[3] = {0x01,0x00,0x00};    /* Software standby in setting */
    uint8_t  reg_sw_reset[3]      = {0x01,0x03,0x01};    /* Software reset setting */
    int_t    ret;

    /* open RIIC */
    ret = open(DEVICE_INDENTIFIER "riic1", O_RDWR);

    if (ret >= 0)
    {
        hi2c2 = ret;
    }

    /* SW standby */
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD,
                    reg_sw_standby_in[0],
                    reg_sw_standby_in[1],
                    reg_sw_standby_in[2]);

    /* SW Reset */
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD,
                    reg_sw_reset[0],
                    reg_sw_reset[1],
                    reg_sw_reset[2]);

    /* Have to wait Camera ready */
    for (count = (528 * 2000); count > 0; count--) /* CPU Clock = 528MHz, 528 clock is needed to wait 1us */
    {
        /* Do nothing 2ms(2000us) wait*/
        ;
    }

    /* MIPI Camera Init */
    for(count = 0; count < INIT_REG_NUM_MIPI; count++)
    {
        RIIC_MipiWrite(MIPI_CAMERA_SUBADD,
                        g_mipi_reg_init_table[count].reg_add_upper,
                        g_mipi_reg_init_table[count].reg_add_lower,
                        g_mipi_reg_init_table[count].value);
    }
}   /* End of function Init_Camera() */

/**************************************************************************//**
 * @fn          Start_Camera
 * @brief       Start_Camera
 * @param[in]   None
 * @retval      None
******************************************************************************/
void Start_Camera( void )
{
    /* MIPI camera clock start setting */
    uint8_t reg_sw_standby_out[3] = {0x01,0x00,0x01};

    RIIC_MipiWrite(MIPI_CAMERA_SUBADD,
                    reg_sw_standby_out[0],
                    reg_sw_standby_out[1],
                    reg_sw_standby_out[2]);
}   /* End of function Start_Camera() */


/******************************************************************************
 * @fn        RIIC_MipiWrite
 * @brief     RIIC_MipiWrite
* @param[in]  ic_adr         : device address.
* @param[in]  reg_adr_upper  : upper register address to write .
* @param[in]  reg_adr_lower  : lower register address to write .
* @param[in]  reg_data       : write data .
* @retval  : none
******************************************************************************/
void RIIC_MipiWrite 
    ( uint8_t ic_adr, uint8_t reg_adr_upper, uint8_t reg_adr_lower, uint8_t reg_data )
{
    int_t ret;
    st_r_drv_riic_transfer_t i2c_write;

    i2c_write.device_address = ic_adr;
    i2c_write.sub_address_type = RIIC_SUB_ADDR_WIDTH_16_BITS;
    i2c_write.sub_address = (reg_adr_upper << 8) | reg_adr_lower;
    i2c_write.number_of_bytes = 1;
    i2c_write.p_data_buffer = &reg_data;

    ret = control(hi2c2, CTL_RIIC_WRITE, &i2c_write);

}   /* End of function RIIC_MipiWrite() */

#endif /* CAM_MIPI == CAM_IMX219_B */
