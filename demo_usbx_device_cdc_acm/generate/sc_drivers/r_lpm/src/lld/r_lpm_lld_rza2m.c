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
/*******************************************************************************
* File Name    : r_lpm_lld_rza2m.c
* Device(s)    : RZ/A2M
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : RZ/A2M Sample Program - LowLevel function of LPM Module
* Operation    :
* Limitations  :
*******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <string.h>
#include <ctype.h>
#include "r_typedefs.h"
#include "r_lpm_drv_api.h"
#include "r_lpm_drv_sc_cfg.h"
#include "r_intc_drv_api.h"
#include "iodefine.h"
#include "iobitmask.h"
#include "rza_io_regrw.h"
#include "r_cpg_lld_rza2m.h"
#include "compiler_settings.h"

/******************************************************************************
Typedef definitions
******************************************************************************/


/******************************************************************************
Macro definitions
******************************************************************************/
/* STBCR1 Reg. Setting Value */
#define LPM_PRV_SLEEP_REG_VALUE            (0x00u)
#define LPM_PRV_SOFTWARE_STANDBY_REG_VALUE (0x80u)
#define LPM_PRV_DEEP_STANDBY_REG_VALUE     (0xC0u)

/* DSCTR Reg. Setting Value */
#define LPM_PRV_EXTERNAL_MEMORY_BOOT_REG_VALUE (0x00u)
#define LPM_PRV_RAMBOOT_EBUSKEEPE_0_REG_VALUE  (0x40u)
#define LPM_PRV_RAMBOOT_EBUSKEEPE_1_REG_VALUE  (0xC0u)

/* Retention RAM Page0 - Page3 Top Address */
#define LPM_PRV_RETENTION_RAM_PAGE0_ADDRESS (0x80000000uL)
#define LPM_PRV_RETENTION_RAM_PAGE1_ADDRESS (0x80004000uL)
#define LPM_PRV_RETENTION_RAM_PAGE2_ADDRESS (0x80008000uL)
#define LPM_PRV_RETENTION_RAM_PAGE3_ADDRESS (0x80010000uL)

#define LPM_PRV_WTCNT_WRITE_ACCESS (0x5A00uL)
#define LPM_PRV_WTCSR_WRITE_ACCESS (0xA500uL)


/**
 * @typedef st_r_drv_lpm_osc_stab_count_t
 */
typedef struct
{
    uint32_t count_min;     /* minimun count value(divide_ratio * 1, same as divide_ratio) */
    uint32_t count_max;     /* maximun count value(divide_ratio * 255) */
} st_r_drv_lpm_osc_stab_count_t;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/


/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
static int_t get_counter_value(const st_r_drv_lpm_osc_stab_count_t *table,
                                uint32_t request_count, uint16_t *setting_count);

static st_r_drv_lpm_sc_config_t * get_sc_table_ptr (e_r_drv_lpm_mode_t mode, e_r_drv_lpm_reboot_type_t ds_reboot_type);

/******************************************************************************
Private global variables and functions
******************************************************************************/

/*! Version Information */
static const st_drv_info_t gs_lld_info =
{
    {
        ((STDIO_LPM_RZ_LLD_VERSION_MAJOR << 16) + STDIO_LPM_RZ_LLD_VERSION_MINOR)
    },
    STDIO_LPM_RZ_LLD_BUILD_NUM,
    STDIO_LPM_RZ_LLD_DRV_NAME
};


/**
 * 
 This table is used for oscillation stabilization time of software standby mode.

  This table data is used to set the count value to 
  the WTCNT register and the division ratio to WTCSR.CKS[3:0].
*/
static const st_r_drv_lpm_osc_stab_count_t gs_lpm_ss_cnt_table[] =
{
    /* min                max        index  : WTCSR.CKS[3:0]  */
    {       1*1,       1*255 },    /*   0   :   b'0000        */
    {      64*1,      64*255 },    /*   1   :   b'0001        */
    {     128*1,     128*255 },    /*   2   :   b'0010        */
    {     256*1,     256*255 },    /*   3   :   b'0011        */
    {     512*1,     512*255 },    /*   4   :   b'0100        */
    {    1024*1,    1024*255 },    /*   5   :   b'0101        */
    {    4096*1,    4096*255 },    /*   6   :   b'0110        */
    {   16384*1,   16384*255 },    /*   7   :   b'0111        */
    {   32768*1,   32768*255 },    /*   8   :   b'1000        */
    {   65536*1,   65536*255 },    /*   9   :   b'1001        */
    {  131072*1,  131072*255 },    /*  10   :   b'1010        */
    {  262144*1,  262144*255 },    /*  11   :   b'1011        */
    {  524288*1,  524288*255 },    /*  12   :   b'1100        */
    { 1048576*1, 1048576*255 },    /*  13   :   b'1101        */
    { 2097152*1, 2097152*255 },    /*  14   :   b'1110        */
    { 4194304*1, 4194304*255 },    /*  15   :   b'1111        */
};

/**
 * table data for making oscillation settling time on deep standby mode
*/
static const st_r_drv_lpm_osc_stab_count_t gs_lpm_ds_cnt_table[] =
{
    {    2048,     522240 },
};


/******************************************************************************
 * End of function check_inrange
 ******************************************************************************/

/******************************************************************************
* Function Name: sstandby_set_cancel_factor
*              :
* Description  : set software standby cancel factor
*              :
* Arguments    : st_r_drv_lpm_sc_config_t p_config_tbl : IN : configuration table pointer
*              :
* Return Value : none
******************************************************************************/
static void sstandby_set_cancel_factor (st_r_drv_lpm_sc_config_t *p_config_tbl)
{
    uint16_t tmp16;


    /* ==== Calculate the setting value for SSTBCCR0 register ==== */
    tmp16 = (uint16_t) ((p_config_tbl->ss_cancel_by_usb0_cc2_rd << INTC_SSTBCCR0_CC2_RD_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb0_cc1_rd << INTC_SSTBCCR0_CC1_RD_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb0_ovrclr << INTC_SSTBCCR0_OVRCLR_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb0_vbusin << INTC_SSTBCCR0_VBUSIN_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb0_dm     << INTC_SSTBCCR0_DM_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb0_dp     << INTC_SSTBCCR0_DP_SHIFT));

    /* Set Cancel Factor by USB0 */
    RZA_IO_RegWrite_16( &INTC.SSTBCCR0.WORD, tmp16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Calculate the setting value for SSTBCCR1 register ==== */
    tmp16 = (uint16_t) ((p_config_tbl->ss_cancel_by_usb1_cc2_rd << INTC_SSTBCCR1_CC2_RD_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb1_cc1_rd << INTC_SSTBCCR1_CC1_RD_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb1_ovrclr << INTC_SSTBCCR1_OVRCLR_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb1_vbusin << INTC_SSTBCCR1_VBUSIN_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb1_dm     << INTC_SSTBCCR1_DM_SHIFT)
                    |   (p_config_tbl->ss_cancel_by_usb1_dp     << INTC_SSTBCCR1_DP_SHIFT));

    /* Set Cancel Factor by USB1 */
    RZA_IO_RegWrite_16( &INTC.SSTBCCR1.WORD, tmp16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Read SSTBCRR0 to satisfy the bit clear condition. */
    RZA_IO_RegRead_16( &INTC.SSTBCRR0.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Clear SSTBCRR0 Flag */
    RZA_IO_RegWrite_16( &INTC.SSTBCRR0.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Read SSTBCRR1 register to satisfy the bit clear condition. */
    RZA_IO_RegRead_16( &INTC.SSTBCRR1.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Clear SSTBCRR1 Flag */
    RZA_IO_RegWrite_16( &INTC.SSTBCRR1.WORD, 0, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Dummy read */
    RZA_IO_RegRead_16( &INTC.SSTBCRR1.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
}
/******************************************************************************
 * End of function sstandby_set_cancel_factor
 ******************************************************************************/

/******************************************************************************
* Function Name: dstandby_set_cancel_factor
*              :
* Description  : set deep standby cancel factor
*              :
* Arguments    : st_r_drv_lpm_sc_config_t p_config_tbl  : IN : configuration table pointer
*              :
* Return Value : none
******************************************************************************/
static void dstandby_set_cancel_factor (st_r_drv_lpm_sc_config_t *p_config_tbl)
{
    uint8_t  tmp8;
    uint16_t tmp16;

    /* ==== Calculate the setting value for DSSSR register  ==== */
    tmp16 = (uint16_t) ((((p_config_tbl->ds_cancel_by_pk_4 & 2) >> 1) << PMG_DSSSR_PK_4_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pk_2 & 2) >> 1) << PMG_DSSSR_PK_2_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pj_5 & 2) >> 1) << PMG_DSSSR_PJ_5_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pj_1 & 2) >> 1) << PMG_DSSSR_PJ_1_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_ph_0 & 2) >> 1) << PMG_DSSSR_PH_0_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pg_6 & 2) >> 1) << PMG_DSSSR_PG_6_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_nmi  & 2) >> 1) << PMG_DSSSR_NMI_SHIFT)
                    |     (p_config_tbl->ds_cancel_by_rtc1            << PMG_DSSSR_RTCAR1_SHIFT)
                    |     (p_config_tbl->ds_cancel_by_rtc0            << PMG_DSSSR_RTCAR0_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pg_2 & 2) >> 1) << PMG_DSSSR_PG_2_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_ph_1 & 2) >> 1) << PMG_DSSSR_PH_1_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_pe_1 & 2) >> 1) << PMG_DSSSR_PE_1_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_p6_2 & 2) >> 1) << PMG_DSSSR_P6_2_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_p3_3 & 2) >> 1) << PMG_DSSSR_P3_3_SHIFT)
                    |   (((p_config_tbl->ds_cancel_by_p3_1 & 2) >> 1) << PMG_DSSSR_P3_1_SHIFT));

    /* Set Cancel Factor by Pin or RTC */
    RZA_IO_RegWrite_16( &PMG.DSSSR.WORD, tmp16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Calculate the setting value for USBDSSSR register ==== */
    tmp8 = (uint8_t)  ((p_config_tbl->ds_cancel_by_usb1_dm << PMG_USBDSSSR_USBDSCE3_SHIFT )
                    |  (p_config_tbl->ds_cancel_by_usb1_dp << PMG_USBDSSSR_USBDSCE2_SHIFT)
                    |  (p_config_tbl->ds_cancel_by_usb0_dm << PMG_USBDSSSR_USBDSCE1_SHIFT)
                    |  (p_config_tbl->ds_cancel_by_usb0_dp << PMG_USBDSSSR_USBDSCE0_SHIFT) );

    /* Set USB0,1 Cancel Factor */
    RZA_IO_RegWrite_8( &PMG.USBDSSSR.BYTE, tmp8, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Calculate the setting value for DSESR register ==== */
    tmp16 = (uint16_t) (((p_config_tbl->ds_cancel_by_pk_4 & 1) << PMG_DSESR_PK_4E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pk_2 & 1) << PMG_DSESR_PK_2E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pj_5 & 1) << PMG_DSESR_PJ_5E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pj_1 & 1) << PMG_DSESR_PJ_1E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_ph_0 & 1) << PMG_DSESR_PH_0E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pg_6 & 1) << PMG_DSESR_PG_6E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_nmi  & 1) << PMG_DSESR_NMIE_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pg_2 & 1) << PMG_DSESR_PG_2E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_ph_1 & 1) << PMG_DSESR_PH_1E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_pe_1 & 1) << PMG_DSESR_PE_1E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_p6_2 & 1) << PMG_DSESR_P6_2E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_p3_3 & 1) << PMG_DSESR_P3_3E_SHIFT)
                    |   ((p_config_tbl->ds_cancel_by_p3_1 & 1) << PMG_DSESR_P3_1E_SHIFT));

    /* Setting of Deep Standby Cancel Edge Select Register (DSESR). */
    RZA_IO_RegWrite_16( &PMG.DSESR.WORD, tmp16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);
}
/******************************************************************************
 * End of function dstandby_set_cancel_factor
 ******************************************************************************/

/******************************************************************************
* Function Name: dstandby_read_retention_ram
*              : 
* Description  : 
*              : 
* Arguments    : st_r_drv_lpm_sc_config_t *p_config_tbl : IN :configuration table pointer
*              : 
* Return Value : none
******************************************************************************/
static void dstandby_read_retention_ram (st_r_drv_lpm_sc_config_t *p_config_tbl)
{
    volatile uint32_t dummy_read32;

    /* Is setting of page 0 to retention ? */
    if (LPM_RETENTION_RAM_RETAINED == p_config_tbl->ds_retention_ram_page0)
    {
        /* Dummy Read for Retention RAM Page0 */
        dummy_read32 = *(volatile uint32_t *) (LPM_PRV_RETENTION_RAM_PAGE0_ADDRESS);
    }

    /* Is setting of page 1 to retention ? */
    if (LPM_RETENTION_RAM_RETAINED == p_config_tbl->ds_retention_ram_page1)
    {
        /* Dummy Read for Retention RAM Page1 */
        dummy_read32 = *(volatile uint32_t *) (LPM_PRV_RETENTION_RAM_PAGE1_ADDRESS);
    }

    /* Is setting of page 2 to retention ? */
    if (LPM_RETENTION_RAM_RETAINED == p_config_tbl->ds_retention_ram_page2)
    {
        /* Dummy Read for Retention RAM Page2 */
        dummy_read32 = *(volatile uint32_t *) (LPM_PRV_RETENTION_RAM_PAGE2_ADDRESS);
    }

    /* Is setting of page 3 to retention ? */
    if (LPM_RETENTION_RAM_RETAINED == p_config_tbl->ds_retention_ram_page3)
    {
        /* Dummy Read for Retention RAM Page3 */
        dummy_read32 = *(volatile uint32_t *) (LPM_PRV_RETENTION_RAM_PAGE3_ADDRESS);
    }

}
/******************************************************************************
 * End of function dstandby_read_retention_ram
 ******************************************************************************/

/******************************************************************************
* Function Name: dstandby_set_retention_ram
*              :
* Description  : set retention_ram
*              :
* Arguments    : st_r_drv_lpm_sc_config_t p_config_tbl  : IN : configuration table pointer
*              :
* Return Value : none
******************************************************************************/
static void dstandby_set_retention_ram(st_r_drv_lpm_sc_config_t *p_config_tbl)
{
    uint8_t tmp8;

    /* ==== Calculate the setting value for RRAMKP register  ==== */
    tmp8 = (uint8_t) (  (p_config_tbl->ds_retention_ram_page3 << PMG_RRAMKP_RRAMKP3_SHIFT)
                    |   (p_config_tbl->ds_retention_ram_page2 << PMG_RRAMKP_RRAMKP2_SHIFT)
                    |   (p_config_tbl->ds_retention_ram_page1 << PMG_RRAMKP_RRAMKP1_SHIFT)
                    |   (p_config_tbl->ds_retention_ram_page0 << PMG_RRAMKP_RRAMKP0_SHIFT));

    /* Setting of On-Chip Data-Retention RAM Area Setting Register (RRAMKP) */
    /* RRAMKPn bit = 0 : The contents of the on-chip data-retention RAM are not retained in deep standby mode. */
    /* RRAMKPn bit = 1 : The contents of the on-chip data-retention RAM are retained in deep standby mode. */
    RZA_IO_RegWrite_8( &PMG.RRAMKP.BYTE, tmp8, IOREG_NONSHIFT_ACCESS, IOREG_NONSHIFT_ACCESS);


    /* Dummy Read for Retention ram */
    dstandby_read_retention_ram(p_config_tbl);
}
/******************************************************************************
 * End of function dstandby_set_retention_ram
 ******************************************************************************/

/******************************************************************************
* Function Name: dstandby_set_reboot_type
*              :
* Description  : set reboot type
*              :
* Arguments    : st_r_drv_lpm_sc_config_t p_config_tbl  : IN : configuration table pointer
*              :
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Failure
******************************************************************************/
static int_t dstandby_set_reboot_type(st_r_drv_lpm_sc_config_t *p_config_tbl)
{
    int_t err = DRV_SUCCESS;
    uint8_t reg8;

    /* Select DSCTR Setting Value */
    switch(p_config_tbl->ds_reboot_type)
    {
        case LPM_REBOOT_TYPE_EXTERNAL_MEMORY_BOOT:
        {
            reg8 = LPM_PRV_EXTERNAL_MEMORY_BOOT_REG_VALUE;
        }
        break;
        case LPM_REBOOT_TYPE_RAMBOOT_EBUSKEEPE_0:
        {
            reg8 = LPM_PRV_RAMBOOT_EBUSKEEPE_0_REG_VALUE;
        }
        break;
        case LPM_REBOOT_TYPE_RAMBOOT_EBUSKEEPE_1:
        {
            reg8 = LPM_PRV_RAMBOOT_EBUSKEEPE_1_REG_VALUE;
        }
        break;
        default:
        {
            err = DRV_ERROR;
            return err;
        }
        break;
    }

    /* Set DSCTR.EBUSKEEPE, DSCTR.RAMBOOT */
    RZA_IO_RegWrite_8( &PMG.DSCTR.BYTE, reg8, IOREG_NONSHIFT_ACCESS, IOREG_NONSHIFT_ACCESS);


    return err;

}
/******************************************************************************
 * End of function dstandby_set_reboot_type
 ******************************************************************************/

/******************************************************************************
* Function Name: get_counter_value
*              :
* Description  : Get the count value used for oscillation stabilization time setting.
*              :
* Arguments    : const st_r_drv_lpm_osc_stab_count_t *table : IN : table pointer
*              : uint32_t request_count  : IN  : p0 counter value
*              : uint16_t *setting_count : OUT : counter value
*              : 
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Failure
******************************************************************************/
static int_t get_counter_value(const st_r_drv_lpm_osc_stab_count_t *table,
                                uint32_t request_count, uint16_t *setting_count)
{
    int_t err = DRV_ERROR;

    if (table->count_min >= request_count)
    {
        *setting_count = 254;
        err = DRV_SUCCESS;
    }
    else if (table->count_max > request_count)
    {
        /* Divisible. */
        if ((request_count % table->count_min) == 0)
        {
            /* Cast : access size of the WTCNT register is 16 bits. */
            *setting_count = (uint16_t) (255 -  (request_count / table->count_min));
        }
        /* Indivisible. */
        else
        {
            /* Cast : access size of the WTCNT register is 16 bits. */
            *setting_count = (uint16_t) (255 - ((request_count / table->count_min) + 1));
        }
        err = DRV_SUCCESS;
    }
    else
    {
        /* The value of request_count is too large. */
        err = DRV_ERROR;
    }

    return err;
}
/******************************************************************************
 * End of function get_counter_value
 ******************************************************************************/

/******************************************************************************
* Function Name: sstandby_get_wdt_reg_setting
* Description  :
* Arguments    : st_r_drv_lpm_sc_config_t p_config_tbl  : IN : configuration table pointer
*              : uint16_t *wdt_clock_ratio : OUT : clock ratio
*              : uint16_t *wdt_count : OUT : count
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Error
******************************************************************************/
static int_t sstandby_get_wdt_reg_setting (st_r_drv_lpm_sc_config_t *p_config_tbl,
                                uint16_t *wdt_clock_ratio, uint16_t *wdt_count )
{
    int_t err = DRV_ERROR;
    float64_t p0_clk;
    float64_t request_period;
    uint32_t request_count;
    uint16_t setting_count;
    uint16_t index;

    /* Get oscillation stabilization time from configuration table */
    request_period = p_config_tbl->ss_osc_stab_time;

    /* Get P0 clock frequency (KHz) */
    R_CPG_GetClock(CPG_FREQ_P0CLK, &p0_clk);

    /* Calculate request_count */
    request_count = (uint32_t) (p0_clk * request_period);

    /* search gs_lpm_ss_cnt_table[] */
    index = 0;
    while(index < 16)
    {
        if (get_counter_value(&gs_lpm_ss_cnt_table[index], request_count, &setting_count) == DRV_SUCCESS)
        {
            err = DRV_SUCCESS;
            break;
        }
        index++;
    }

    if ( DRV_ERROR == err)
    {
        return err;
    }
    
    *wdt_clock_ratio = index;
    *wdt_count = setting_count;

    return err;
}
/******************************************************************************
 * End of function sstandby_get_wdt_reg_setting
 ******************************************************************************/

/******************************************************************************
* Function Name: dstandby_get_pmg_reg_setting
*              :
* Description  :
*              :
* Arguments    : p_config_tbl  : configuration table pointer
*              : 
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Error
******************************************************************************/
static int_t dstandby_get_pmg_reg_setting (st_r_drv_lpm_sc_config_t *p_config_tbl, uint16_t *pmg_count)
{
    float64_t p0_clk;
    float64_t request_period;
    uint32_t request_count;
    uint16_t setting_count;

    /* Get oscillation stabilization time from configuration table */
    request_period = p_config_tbl->ds_osc_stab_time;

    /* Get P0 clock frequency (KHz) */
    R_CPG_GetClock(CPG_FREQ_P0CLK, &p0_clk);

    /* Calculate request_count */
    request_count = (uint32_t) (p0_clk * request_period);

    /* search table */
    if (get_counter_value(&gs_lpm_ds_cnt_table[0], request_count, &setting_count) == DRV_ERROR)
    {
        return DRV_ERROR;
    }


    *pmg_count = setting_count;

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of function dstandby_get_pmg_reg_setting
 ******************************************************************************/


/******************************************************************************
* Function Name: get_sc_table_ptr
*              :
* Description  : Search pointer of SC table matching argument
*              :
* Arguments    : mode           : IN : LPM mode
*              : ds_reboot_type : IN : RAMBOOT option of Deep standby mode
*              : 
* Return Value : selected sc table pointer
*              : 
******************************************************************************/
static st_r_drv_lpm_sc_config_t * get_sc_table_ptr (e_r_drv_lpm_mode_t mode, e_r_drv_lpm_reboot_type_t ds_reboot_type)
{
    st_r_drv_lpm_sc_config_t * p_use_sc_table;
    int_t sc_table_count;
    int_t i;

    /* Initialize p_use_sc_table to default value */
    p_use_sc_table = NULL;

    /* Set SC table max count */
    sc_table_count = ((sizeof(LPM_SC_TABLE)) / sizeof(st_r_drv_lpm_sc_config_t));

    /* Searching SC table */
    for (i = 0; i < sc_table_count; i++)
    {
        if (LPM_SC_TABLE[i].mode == mode)
        {
            if ((LPM_MODE_SLEEP  == mode) || (LPM_MODE_SOFTWARE_STANDBY == mode) ||
                ((LPM_MODE_DEEP_STANDBY == mode) && (LPM_SC_TABLE[i].ds_reboot_type == ds_reboot_type)))
            {
                /* Make p_use_sc_table pointer value & exit from loop */
                p_use_sc_table = &LPM_SC_TABLE[i];
                break;
            }
        }
    }

    return p_use_sc_table;

}
/******************************************************************************
 * End of function get_sc_table_ptr
 ******************************************************************************/


/******************************************************************************
* Function Name: R_LPM_GetVersion
*              :
* Description  : Get version information
*              :
* Arguments    : pVerInfo      driver version information is populated into this
*              :               structure.
*              :               NOTE: the structure resources are created
*              :                     by the application not this function
*              :
* Return Value : DRV_SUCCESS   Success
******************************************************************************/
int_t R_LPM_GetVersion(st_ver_info_t *pVerInfo)
{
    pVerInfo->lld.p_szdriver_name = gs_lld_info.p_szdriver_name;
    pVerInfo->lld.version.sub.major = gs_lld_info.version.sub.major;
    pVerInfo->lld.version.sub.minor = gs_lld_info.version.sub.minor;
    pVerInfo->lld.build = gs_lld_info.build;

    return DRV_SUCCESS;
}
/******************************************************************************
 * End of function R_LPM_GetVersion
 ******************************************************************************/

/******************************************************************************
* Function Name: R_LPM_SleepTransition
*              :
* Description  : Transition to Sleep Mode
*              :
* Arguments    : none
*              :
* Return Value : DRV_SUCCESS   Success
******************************************************************************/
int_t R_LPM_SleepTransition(void)
{
    int_t err = DRV_SUCCESS;
    uint8_t dummy_buf_8b;

    /* Sleep mode setting (STBY bit =b'0, DEEP bit =b'0 ) */
    RZA_IO_RegWrite_8(&CPG.STBCR1.BYTE, LPM_PRV_SLEEP_REG_VALUE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Dummy read */
    dummy_buf_8b = RZA_IO_RegRead_8(&CPG.STBCR1.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Transition to Sleep mode */
    __wfi();

    return err;
}
/******************************************************************************
 * End of function R_LPM_SleepTransition
 ******************************************************************************/

/******************************************************************************
* Function Name: R_LPM_SStandbyTransition
*              :
* Description  : Transition to Software Standby Mode
*              :
* Arguments    : st_r_drv_lpm_sc_config_t * p_config_tbl : IN : configuration table pointer
*              :
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Error
******************************************************************************/
int_t R_LPM_SStandbyTransition(st_r_drv_lpm_sc_config_t * p_config_tbl)
{
    int_t err = DRV_ERROR;
    uint16_t tmp16;
    uint16_t wdt_clock_ratio;
    uint16_t setting_count;
    uint8_t  dummy_buf_8b;
    uint32_t dummy_buf_32b;

    /* Check mode  */
    if(LPM_MODE_SOFTWARE_STANDBY != p_config_tbl->mode)
    {
        return err;
    }

    /* ==== standby_mode_en bit of Power Control Register setting  ==== */
    RZA_IO_RegWrite_32(&pl310.REG15_POWER_CTRL.LONG, 1, PL310_REG15_POWER_CTRL_standby_mode_en_SHIFT,
                        PL310_REG15_POWER_CTRL_standby_mode_en);

    /* Dummy read */
    dummy_buf_32b =RZA_IO_RegRead_32(&pl310.REG15_POWER_CTRL.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Get WDT's register value to set oscillation stabilization time ==== */
    err = sstandby_get_wdt_reg_setting(p_config_tbl, &wdt_clock_ratio, &setting_count);
    if ( DRV_ERROR == err)
    {
        return err;
    }

    /* Stop WDT */
    RZA_IO_RegWrite_16(&WDT.WTCSR.WORD, LPM_PRV_WTCSR_WRITE_ACCESS | 0x0000,
                        IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ====  Oscillation stabilization count setting (software standby) ==== */
    RZA_IO_RegWrite_16(&WDT.WTCNT.WORD, (LPM_PRV_WTCNT_WRITE_ACCESS | setting_count),
                                        IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Clock Ratio setting (WTCSR.CKS[3:0]) ====*/
    RZA_IO_RegWrite_16(&WDT.WTCSR.WORD, (LPM_PRV_WTCSR_WRITE_ACCESS | wdt_clock_ratio),
                        IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Read IRQRR register to satisfy the bit clear condition.   */
    tmp16 = RZA_IO_RegRead_16( &INTC.IRQRR.WORD, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Calculate Write Data for IRQRR register */
    tmp16 = tmp16 & INTC_IRQRR_IRQMSK;

    /* Clear IRQRR.IRQ7F - IRQRR.IRQ0F */
    RZA_IO_RegWrite_16( &INTC.IRQRR.WORD, tmp16, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Sets whether to use the IRQ pins(IRQ7-IRQ0) to cancel software standby mode. */
    R_INTC_SetIrqMask(p_config_tbl->ss_cancel_by_irq);

    /* ==== Pin state setting in standby ==== */
    /*   HIZ bit = 0 : The pin state is retained in software standby mode or deep standby mode. */
    /*   HIZ bit = 1 : The pin is set to high-impedance in software standby mode or deep standby mode. */
    RZA_IO_RegWrite_8( &CPG.STBCR2.BYTE, p_config_tbl->pin_status, CPG_STBCR2_HIZ_SHIFT, CPG_STBCR2_HIZ);

    /* Software standby cancel factor setting */
    sstandby_set_cancel_factor(p_config_tbl);

    /* Software standby mode setting (STBY bit =b'1,  DEEP bit =b'0) */
    RZA_IO_RegWrite_8(&CPG.STBCR1.BYTE, LPM_PRV_SOFTWARE_STANDBY_REG_VALUE,
                    IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Dummy read */
    dummy_buf_8b = RZA_IO_RegRead_8(&CPG.STBCR1.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ====  Transition to Software standby mode ==== */
    __wfi();

    return err;
}
/******************************************************************************
 * End of function R_LPM_SStandbyTransition
 ******************************************************************************/

/******************************************************************************
* Function Name: R_LPM_DStandbyTransition
*              : 
* Description  : Transition to Deep Standby Mode
*              : 
* Arguments    : st_r_drv_lpm_sc_config_t * p_config_tbl : IN : configuration table pointer
*              : 
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Error
******************************************************************************/
int_t R_LPM_DStandbyTransition(st_r_drv_lpm_sc_config_t * p_config_tbl)
{
    int_t err = DRV_ERROR;
    uint16_t setting_count;
    uint8_t  dummy_buf_8b;
    uint16_t dummy_buf_16b;
    uint32_t dummy_buf_32b;

    /* Check mode  */
    if(LPM_MODE_DEEP_STANDBY != p_config_tbl->mode)
    {
        return err;
    }

    /* ==== standby_mode_en bit of Power Control Register setting  ==== */
    RZA_IO_RegWrite_32(&pl310.REG15_POWER_CTRL.LONG, 1, PL310_REG15_POWER_CTRL_standby_mode_en_SHIFT,
                        PL310_REG15_POWER_CTRL_standby_mode_en);

    /* Dummy read */
    dummy_buf_32b = RZA_IO_RegRead_32(&pl310.REG15_POWER_CTRL.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* ==== Retention RAM setting ==== */
    dstandby_set_retention_ram(p_config_tbl);

    /* ==== Pin state setting in standby ==== */
    /*   HIZ bit = 0 : The pin state is retained in software standby mode or deep standby mode. */
    /*   HIZ bit = 1 : The pin is set to high-impedance in software standby mode or deep standby mode. */
    RZA_IO_RegWrite_8( &CPG.STBCR2.BYTE, p_config_tbl->pin_status, CPG_STBCR2_HIZ_SHIFT, CPG_STBCR2_HIZ);

    /* ==== reboot type setting ====  */
    if( dstandby_set_reboot_type(p_config_tbl) == DRV_ERROR )
    {
        return err;
    }

    /* ==== Deep standby cancel factor setting ==== */
    dstandby_set_cancel_factor(p_config_tbl);

    /* ==== Get oscillation stabilization time for deep standby ==== */
    err = dstandby_get_pmg_reg_setting (p_config_tbl, &setting_count);
    if ( DRV_ERROR == err)
    {
        return err;
    }

    /* ==== oscillation stabilization count setting (deep standby) ==== */
    RZA_IO_RegWrite_16(&PMG.DSCNT.WORD, setting_count, IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS);

    /* ==== Deep standby mode setting (STBY bit =b'1, DEEP bit =b'1) ==== */
    RZA_IO_RegWrite_8(&CPG.STBCR1.BYTE, LPM_PRV_DEEP_STANDBY_REG_VALUE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Dummy read */
    dummy_buf_8b = RZA_IO_RegRead_8(&CPG.STBCR1.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Read DSFR */
    dummy_buf_16b = RZA_IO_RegRead_16(&PMG.DSFR.WORD, IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS);

    /* Clear All DSFR Flag */
    RZA_IO_RegWrite_16(&PMG.DSFR.WORD, 0, IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS);

    /* Read USBDSFR */
    dummy_buf_8b = RZA_IO_RegRead_8(&PMG.USBDSFR.BYTE, IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS);

    /* Clear All USBDSFR Flag */
    RZA_IO_RegWrite_8(&PMG.USBDSFR.BYTE, 0, IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS);

    /* Dummy read */
    dummy_buf_8b = RZA_IO_RegRead_8(&PMG.USBDSFR.BYTE, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* CPU is not notified of interrupts other than NMIs. */
    RZA_IO_RegWrite_32(&INTC.GICC_CTLR.LONG, 0x00000000uL, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    /* Dummy read */
    dummy_buf_32b = RZA_IO_RegRead_32(&INTC.GICC_CTLR.LONG, IOREG_NONSHIFT_ACCESS, IOREG_NONMASK_ACCESS);

    while(1)
    {
        /* ==== Transition to Deep standby mode ==== */
        __wfi();
    }

    return err;
}
/******************************************************************************
 * End of function R_LPM_DStandbyTransition
 ******************************************************************************/

/******************************************************************************
* Function Name: R_LPM_Transition
*              : 
* Description  : Transition to Sleep/Standby mode
*              : 
* Arguments    : mode           : IN : LPM mode
*              : ds_reboot_type : IN : RAMBOOT option of Deep standby mode
*              : 
* Return Value : DRV_SUCCESS   Success
*              : DRV_ERROR     Error
******************************************************************************/
int_t R_LPM_Transition (e_r_drv_lpm_mode_t mode, e_r_drv_lpm_reboot_type_t ds_reboot_type)
{
    st_r_drv_lpm_sc_config_t * p_use_sc_table;
    int_t err;

    err = DRV_SUCCESS;

    /* Search pointer of SC table matching argumant */
    p_use_sc_table = get_sc_table_ptr(mode, ds_reboot_type);

    if (NULL != p_use_sc_table)
    {
        if (LPM_MODE_SLEEP == mode)
        {
            /* Call sleep mode transition function */
            err = R_LPM_SleepTransition();
        }
        if (LPM_MODE_SOFTWARE_STANDBY == mode)
        {
            /* Call software standby mode transition function */
            err = R_LPM_SStandbyTransition(p_use_sc_table);
        }
        else
        {
            /* Call deep standby mode transition function */
            err = R_LPM_DStandbyTransition(p_use_sc_table);
        }
    }
    else
    {
        /* error return */
        err = DRV_ERROR;
    }

    return err;
}
/******************************************************************************
 * End of function R_LPM_Transition
 ******************************************************************************/

/* End of File */

