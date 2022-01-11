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
/*****************************************************************************
 * File Name    : r_lpm_lld_rza2m.h
 * Description  : Low Power Mode driver API header
 *****************************************************************************/

/*****************************************************************************
 Includes   <System Includes> , "Project Includes"
 *****************************************************************************/
#include "iodefine.h"
#include "r_typedefs.h"
#include "driver.h"

/*****************************************************************************
 Macro definitions
 *****************************************************************************/
#ifndef R_LPM_LLD_RZA2M_H_
#define R_LPM_LLD_RZA2M_H_

/* Version Number of API */

#define STDIO_LPM_RZ_LLD_DRV_NAME ("LLD EBK_RZA2M LPM")

/** Major Version Number of API */
#define STDIO_LPM_RZ_LLD_VERSION_MAJOR      (1)
/** Minor Version Number of API */
#define STDIO_LPM_RZ_LLD_VERSION_MINOR      (11)
/** Minor Version Number of API */
#define STDIO_LPM_RZ_LLD_BUILD_NUM          (0)
/** Unique ID */
#define STDIO_LPM_RZ_LLD_UID                (0)


/*****************************************************************************
 Typedef definitions
 *****************************************************************************/
typedef enum {
    LPM_MODE_SLEEP = 0,                           /*!< Sleep mode */
    LPM_MODE_SOFTWARE_STANDBY,                    /*!< Software Standby mode */
    LPM_MODE_DEEP_STANDBY,                        /*!< Deep Standby mode */
}e_r_drv_lpm_mode_t;

typedef enum {
    LPM_PIN_STATUS_RETAINED = 0,                  /*!< Pin state during Software Standby or Deep Standby : Retained */
    LPM_PIN_STATUS_HIZ,                           /*!< Pin state during Software Standby or Deep Standby : Hiz */
}e_r_drv_lpm_pin_status_t;

typedef enum {
    LPM_SSTANDBY_DISABLE_IRQ = 0,                 /*!< Not use the IRQ7~IRQ0 pin as factor cancel
                                                        for software standby mode */
    LPM_SSTANDBY_ENABLE_IRQ,                      /*!< Use the IRQ7~IRQ0 pin as factor cancel
                                                        for software standby mode */
}e_r_drv_lpm_cancel_factor_irq_t;

typedef enum {
    LPM_PIN_CANCEL_FACTOR_DISABLED = 0,           /*!< Not use the pin as factor cancel for deep standby mode */
    LPM_PIN_CANCEL_FACTOR_FALLING_EDGE   = 2,     /*!< Use the pin falling as factor cancel for deep standby mode */
    LPM_PIN_CANCEL_FACTOR_RISING_EDGE    = 3,     /*!< Use the pin rising as factor cancel for deep standby mode  */
}e_r_drv_lpm_cancel_factor_t;

typedef enum {
    LPM_SS_CANCEL_FACTOR_DISABLED = 0,            /*!< Not use the Module(USB) as factor cancel
                                                        for software standby mode */
    LPM_SS_CANCEL_FACTOR_ENABLED,                 /*!< Use the Module(USB) as factor cancel
                                                        for software standby mode */
}e_r_drv_lpm_ss_cancel_factor_t;

typedef enum {
    LPM_DS_CANCEL_FACTOR_DISABLED = 0,            /*!< Not use the Module(USB,RTC) as factor cancel
                                                        for deep standby mode */
    LPM_DS_CANCEL_FACTOR_ENABLED,                 /*!< Use the Module(USB,RTC) as factor cancel
                                                        for deep standby mode */
}e_r_drv_lpm_ds_cancel_factor_t;

typedef enum {
    LPM_RETENTION_RAM_NOT_RETAINED = 0,           /*!< Retention RAM contents are not retained,
                                                        during the deep standby mode. */
    LPM_RETENTION_RAM_RETAINED,                   /*!< Retention RAM contents are retained,
                                                        during the deep standby mode. */
}e_r_drv_lpm_retention_ram_t;

typedef enum {
    LPM_REBOOT_TYPE_EXTERNAL_MEMORY_BOOT = 0,     /*!< Boot from external memory */
    LPM_REBOOT_TYPE_RAMBOOT_EBUSKEEPE_0,          /*!< Boot from Retention RAM : Memory pin is not retained. */
    LPM_REBOOT_TYPE_RAMBOOT_EBUSKEEPE_1,          /*!< Boot from Retention RAM : Memory pin is retained. */
}e_r_drv_lpm_reboot_type_t;


/**
 * @typedef st_r_drv_lpm_sc_config_t Smart Configurator Interface
 */
typedef struct
{
    /* ==== Common ==== */
    e_r_drv_lpm_mode_t                  mode;                       /* Mode (Sleep or Software Standby
                                                                     or Deep Standby) */

    /* ==== for Software Standby, Deep Standby  ==== */
    e_r_drv_lpm_pin_status_t            pin_status;                 /* Pin state in Software Standby or Deep Standby */

    /* ==== for Software Standby  ==== */
    e_r_drv_lpm_cancel_factor_irq_t     ss_cancel_by_irq;           /* IRQ Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_cc2_rd;   /* USB0_CC2_RD Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_cc1_rd;   /* USB0_CC1_RD Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_ovrclr;   /* USB0_OVRCLR Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_vbusin;   /* USB0_VBUSIN Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_dm;       /* USB0_DM Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb0_dp;       /* USB0_DP Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_cc2_rd;   /* USB1_CC2_RD Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_cc1_rd;   /* USB1_CC1_RD Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_ovrclr;   /* USB1_OVRCLR Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_vbusin;   /* USB1_VBUSIN Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_dm;       /* USB1_DM Cancel Factor */
    e_r_drv_lpm_ss_cancel_factor_t      ss_cancel_by_usb1_dp;       /* USB1_DP Cancel Factor */
    float64_t                           ss_osc_stab_time;           /* Clock oscillation stabilization time (ms)*/

    /* ==== for Deep Stanby  ==== */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pk_4;          /* PK_4 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pk_2;          /* PK_2 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pj_5;          /* PJ_5 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pj_1;          /* PJ_1 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_ph_0;          /* PH_0 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pg_6;          /* PG_6 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_nmi;           /* NMI Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_rtc1;          /* RTC1 Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_rtc0;          /* RTC0 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pg_2;          /* PG_2 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_ph_1;          /* PH_1 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_pe_1;          /* PE_1 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_p6_2;          /* P6_2 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_p3_3;          /* P3_3 Cancel Factor */
    e_r_drv_lpm_cancel_factor_t         ds_cancel_by_p3_1;          /* P3_1 Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_usb1_dm;       /* USB DM1 Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_usb1_dp;       /* USB DP1 Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_usb0_dm;       /* USB DM0 Cancel Factor */
    e_r_drv_lpm_ds_cancel_factor_t      ds_cancel_by_usb0_dp;       /* USB DP0 Cancel Factor */
    e_r_drv_lpm_retention_ram_t         ds_retention_ram_page3;     /* Setting of Retention RAM page 3
                                                                        (Retained or Not Retained) */
    e_r_drv_lpm_retention_ram_t         ds_retention_ram_page2;     /* Setting of Retention RAM page 2
                                                                        (Retained or Not Retained) */
    e_r_drv_lpm_retention_ram_t         ds_retention_ram_page1;     /* Setting of Retention RAM page 1
                                                                        (Retained or Not Retained) */
    e_r_drv_lpm_retention_ram_t         ds_retention_ram_page0;     /* Setting of Retention RAM page 0
                                                                        (Retained or Not Retained) */
    float64_t                           ds_osc_stab_time;           /* Clock oscillation stabilization time (ms) */
    e_r_drv_lpm_reboot_type_t           ds_reboot_type;             /* reboot type */
} st_r_drv_lpm_sc_config_t;





/******************************************************************************
 Interface
 ******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/**
 * @fn         R_LPM_SleepTransition
 *
 * @brief      Transition to Sleep Mode
 *
 * @param[in]  none
 *
 * @retval     DRV_SUCCESS   Successful operation.
 */
extern int_t R_LPM_SleepTransition(void);

/**
 * @fn         R_LPM_SStandbyTransition
 *
 * @brief      Transition to Software Standby Mode
 *
 * @param[in]  p_config_tbl:   configuration table pointer
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_LPM_SStandbyTransition(st_r_drv_lpm_sc_config_t *p_config_tbl);

/**
 * @fn         R_LPM_DStandbyTransition
 *
 * @brief      Transition to Deep Standby Mode
 *
 * @param[in]  p_config_tbl:   configuration table pointer
 *
 * @retval     DRV_SUCCESS   Successful operation.
 * @retval     DRV_ERROR     On failure
 */
extern int_t R_LPM_DStandbyTransition(st_r_drv_lpm_sc_config_t *p_config_tbl);

/**
 * @fn         R_LPM_Transition
 *
 * @brief      Transition to Sleep/Standby Mode
 *
 * @param[in]  mode          :   LPM mode
 * @param[in]  ds_reboot_type:   RAMBOOT option of Deep standby mode
 *
 * @retval     DRV_SUCCESS   Successful operation.
 */
extern int_t R_LPM_Transition(e_r_drv_lpm_mode_t mode, e_r_drv_lpm_reboot_type_t ds_reboot_type);

/**
 * @fn         R_LPM_GetVersion
 *
 * @brief      Get version information
 *
 * @param[out] pVerInfo:  driver version information is populated into this
 *                        structure.
 *                        @note the structure resources are created
 *                              by the application not this function
 *
 * @retval     DRV_SUCCESS   Successful operation.
 */
extern int_t R_LPM_GetVersion(st_ver_info_t *p_ver_info);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* R_LPM_LLD_RZA2M_H_ */
