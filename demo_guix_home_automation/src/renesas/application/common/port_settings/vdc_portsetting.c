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
/****************************************************************************
 * File Name    : vdc_portsetting.c
 * Device(s)    : RZ/A2M
 * Tool-Chain   : e2Studio Ver 7.8.0
 *              : GNU ARM Embedded 6.3.1.20170620
 * OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
 * H/W Platform : GR-MANGO(mbed-RZ/A2M) X71A-M01-B
 * @brief       : VDC port setting functions
 ******************************************************************************/

/*******************************************************************************
 Includes <System Includes>, "Project Includes"
 *******************************************************************************/
#include <unistd.h>
#include "r_typedefs.h"
#include "iodefine.h"
#include "lcd_panel.h"
#include "r_rvapi_vdc.h"
#include "r_gpio_drv_api.h"
#include "r_riic_drv_api.h"
#include "r_devlink_wrapper.h"
#include "r_riic_drv_sc_cfg.h"
#include "r_gpio_drv_sc_cfg.h"

#include "tx_api.h"

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#if ( LCD_PANEL==3 )
#define EP952_DEV_ADDR  (0x52u)
#endif

/******************************************************************************
 Enumerated Types
 ******************************************************************************/

/******************************************************************************
 Structures
 ******************************************************************************/
typedef struct
{
    uint8_t  reg_addr;  /* address */
    uint8_t  value;     /* value of register */
} st_dvi_reg_t;

#if ( LCD_PANEL==3 )
static st_r_drv_gpio_pin_rw_t s_pk5_hi =
{
    GPIO_PORT_K_PIN_5,
    GPIO_LEVEL_HIGH,
    GPIO_SUCCESS
};
static st_r_drv_gpio_pin_rw_t s_pk5_lo =
{
    GPIO_PORT_K_PIN_5,
    GPIO_LEVEL_LOW,
    GPIO_SUCCESS
};
static const r_gpio_port_pin_t s_ep952_pin_list[] =
{
    GPIO_PORT_K_PIN_5,
};
#endif

/******************************************************************************
 Global Vaiables
 ******************************************************************************/


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
const st_dvi_reg_t g_dvi_reg_init_table[] =
{
    /* dvi short */
    {0x08u, 0x96u},
    {0x00u, 0x80u},
    {0x05u, 0x14u},
    {0x0Au, 0x8Fu},
    {0x0Cu, 0x30u},
    {0x0Du, 0x80u},
    {0x0Eu, 0x00u},
    {0x33u, 0x0Bu},
    {0x3Fu, 0xF8u},
    {0x40u, 0x08u},
    {0x08u, 0x97u},
};


static void RIIC_DviWrite( int_t handle, uint8_t ic_adr, uint8_t reg_adr, uint8_t reg_data ); 

/******************************************************************************
 Function prototypes
 ******************************************************************************/
void VDC_LcdPortSetting(uint32_t param);


/**************************************************************************//**
 * Function Name : VDC_LcdPortSetting
 * @brief       LCD panel port setup
 * @param[in]   param     :
 * @retval      None
 ******************************************************************************/
void VDC_LcdPortSetting(uint32_t param) 
{
    int_t err;
    int_t gpio_handle;

    gpio_handle = direct_open("gpio", 0);
    if ( gpio_handle < 0 )
    {
        /* stop execute */
        while(1)
        {
            /* Spin here forever */
            ;
        }
    }

#if     (LCD_PANEL==1)
    /*  LVDS LCD for RZ/A2M Eva board
            TXCLKOUTM   :                       ... P4_7
            TXCLKOUTP   :                       ... P4_6
            TXOUT2M     :                       ... P4_5
            TXOUT2P     :                       ... P4_4
            TXOUT1M     :                       ... P4_3
            TXOUT1P     :                       ... P4_2
            TXOUT0M     :                       ... P4_1
            TXOUT0P     :                       ... P4_0
    */

    st_r_drv_gpio_pin_list_t pin_lcd_lvds;

    static const r_gpio_port_pin_t pin_lcd_lvds_table[] =
    {
        GPIO_PORT_4_PIN_7,  GPIO_PORT_4_PIN_6,
        GPIO_PORT_4_PIN_5,  GPIO_PORT_4_PIN_4,
        GPIO_PORT_4_PIN_3,  GPIO_PORT_4_PIN_2,
        GPIO_PORT_4_PIN_1,  GPIO_PORT_4_PIN_0,
    };

    /* === LCD === */
    /* ---- Port initialize ---- */
    pin_lcd_lvds.p_pin_list = pin_lcd_lvds_table;
    pin_lcd_lvds.count = (sizeof(pin_lcd_lvds_table)) / (sizeof(pin_lcd_lvds_table[0]));
    err = direct_control(gpio_handle, CTL_GPIO_INIT_BY_PIN_LIST, &pin_lcd_lvds);
    if ( err < 0 )
    {
        /* stop execute */
        while(1);
    }


#elif (LCD_PANEL==2 || LCD_PANEL == 3)

    /*  EBK LCD for RZ/A2M Eva board
            1    PF_7  LCD0_DATA0           2    PH_2  LCD0_DATA1
            3    PF_6  LCD0_DATA2           4    PF_5  LCD0_DATA3
            5    PF_4  LCD0_DATA4           6    PF_3  LCD0_DATA5
            7    PF_2  LCD0_DATA6           8    PF_1  LCD0_DATA7
            9    PF_0  LCD0_DATA8           10   P8_0  LCD0_DATA9
            11   PA_0  LCD0_DATA10          12   PA_1  LCD0_DATA11
            13   PA_2  LCD0_DATA12          14   PA_3  LCD0_DATA13
            15   PA_4  LCD0_DATA14          16   PA_5  LCD0_DATA15
            17   PA_6  LCD0_DATA16          18   PA_7  LCD0_DATA17
            19   PB_0  LCD0_DATA18          20   PB_1  LCD0_DATA19
            21   PB_2  LCD0_DATA20          22   PB_3  LCD0_DATA21
            23   PB_4  LCD0_DATA22          24   PB_5  LCD0_DATA23
            26   PJ_6  LCD0_CLK             27   PC_3  LCD0_TCON4(DE)
            28   PC_4  LCD0_TCON3(Hsync)    29   P7_7  LCD0_TCON0(Vsync)

                 PJ_7  LCD0_EXTCLK
            */


    st_r_drv_gpio_config_t pin_back_light;

    /* Cast to an appropriate type */
    static const st_r_drv_gpio_pin_rw_t    p76_hi = { GPIO_PORT_7_PIN_6, GPIO_LEVEL_HIGH };

    /* Cast to an appropriate type */
    static const st_r_drv_gpio_pin_rw_t    p76_lo = { GPIO_PORT_7_PIN_6, GPIO_LEVEL_LOW  };

    st_r_drv_gpio_pin_init_table_t vdc_pin =
    {
        &GPIO_SC_TABLE_vdc[0],
        (sizeof(GPIO_SC_TABLE_vdc))/sizeof(st_r_drv_gpio_sc_config_t),
    };

    /* === LCD === */
    /* ---- Port initialize ---- */
    err = direct_control(gpio_handle, CTL_GPIO_INIT_BY_TABLE, &vdc_pin);
    if ( err < 0 )
    {
        /* stop execute */
        while(1)
        {
            /* Spin here forever */
            ;
        }
    }

#if ( LCD_PANEL==3 )
    {
        /**************************************************
         * Initialise P6_0 pin parameterised in GPIO_SC_TABLE_MANUAL
         **************************************************/
        st_r_drv_gpio_pin_list_t pin_ep952;
        
        pin_ep952.p_pin_list = s_ep952_pin_list;
        pin_ep952.count = (sizeof(s_ep952_pin_list)) / (sizeof(s_ep952_pin_list[0]));
        err = direct_control(gpio_handle, CTL_GPIO_INIT_BY_PIN_LIST, &pin_ep952);
        if ( err < 0 )
        {
            /* stop execute */
            while(1)
            {
                /* Spin here forever */
                ;
            }
        }

        err = direct_control(gpio_handle, CTL_GPIO_PIN_WRITE, &s_pk5_lo);
        if ( err < 0 )
        {
            /* stop execute */
            while(1)
            {
                /* Spin here forever */
                ;
            }
        }

        tx_thread_sleep(10); /* R_OS_TaskSleep(10); change tx api */

        err = direct_control(gpio_handle, CTL_GPIO_PIN_WRITE, &s_pk5_hi);
        if ( err < 0 )
        {
            /* stop execute */
            while(1)
            {
                /* Spin here forever */
                ;
            }
        }
    }

    {
        int_t handle;
        int_t count;

        /* open RIIC */
        handle = open(DEVICE_INDENTIFIER "riic1", O_RDWR);

        if( handle >= 0 )
        {


            for (count = 0; count < (sizeof(g_dvi_reg_init_table) / sizeof(st_dvi_reg_t)); count++)
            {
                RIIC_DviWrite(
                    handle,
                    EP952_DEV_ADDR,
                    g_dvi_reg_init_table[count].reg_addr,
                    g_dvi_reg_init_table[count].value);
            }

            close(handle);
        }
    }
#endif

#else /* Not (LCD_PANEL==2 || LCD_PANEL == 3) */
    #error  Undefined LCD panel!
#endif

    direct_close(gpio_handle);
} /* End of function VDC_LcdPortSetting() */
#if ( LCD_PANEL==3 )
/******************************************************************************
* Function Name: RIIC_DviWrite
* Description  :
* @param[in]  hld            : io handle.
* @param[in]  ic_adr         : device address.
* @param[in]  reg_adr        : register address to write .
* @param[in]  reg_data       : write data .
* @retval  : none
******************************************************************************/
static void RIIC_DviWrite( int_t handle, uint8_t ic_adr, uint8_t reg_adr, uint8_t reg_data )
{
    int_t ret;
    st_r_drv_riic_transfer_t i2c_write;

    i2c_write.device_address = ic_adr;
    i2c_write.sub_address_type = RIIC_SUB_ADDR_WIDTH_8_BITS;
    i2c_write.sub_address = reg_adr;
    i2c_write.number_of_bytes = 1;
    i2c_write.p_data_buffer = &reg_data;

    ret = control(handle, CTL_RIIC_WRITE, &i2c_write);

}   /* End of function RIIC_DviWrite() */
#endif
