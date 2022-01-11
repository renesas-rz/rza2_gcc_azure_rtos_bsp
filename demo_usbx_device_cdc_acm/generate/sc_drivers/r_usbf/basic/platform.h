/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_bsp.h
* H/W Platform : GENERIC_RX64M 
* Description  : Has the header files that should be included for this platform.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.08.2013 1.00     First Release
*         : 01.11.2017 2.00     Added include path of "mcu_startup.h".
*         : 01.07.2018 2.01     Additional RTOS header file.
***********************************************************************************************************************/

//#ifndef BSP_BOARD_GENERIC_RX64M
//#define BSP_BOARD_GENERIC_RX64M

/* Make sure that no other platforms have already been defined. Do not touch this! */
//#ifdef  PLATFORM_DEFINED
//#error  "Error - Multiple platforms defined in platform.h!"
//#else
//#define PLATFORM_DEFINED
//#endif

/***********************************************************************************************************************
INCLUDE APPROPRIATE MCU AND BOARD FILES
***********************************************************************************************************************/
/************** 20181218 ADD This FILE sawada*******************/
/***************************************************************
#include    "mcu/all/r_bsp_common.h"
#include    "r_bsp_config.h"
#include    "r_bsp_interrupt_config.h"
#include    "mcu/rx64m/register_access/iodefine.h"
#include    "mcu/rx64m/mcu_info.h"
#include    "mcu/rx64m/mcu_locks.h"
#include    "mcu/rx64m/locking.h"
#include    "mcu/rx64m/cpu.h"
#include    "mcu/rx64m/mcu_init.h"
#include    "mcu/rx64m/mcu_interrupts.h"
#include    "mcu/rx64m/mcu_mapped_interrupts_private.h"
#include    "mcu/rx64m/mcu_mapped_interrupts.h"
#include    "mcu/rx64m/mcu_startup.h"
#include    "board/generic_rx64m/hwsetup.h"
#include    "board/generic_rx64m/lowsrc.h"
#include    "board/generic_rx64m/vecttbl.h"
#if BSP_CFG_RTOS_USED == 0      // Non-OS
#elif BSP_CFG_RTOS_USED == 1    // FreeRTOS
******************************************************************/
#include "iodefine.h"
#include "r_usbf_basic_drv_sc_cfg.h"
#if BSP_CFG_RTOS_USED == 1
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "croutine.h"
#include "timers.h"
#include "event_groups.h"
#endif

//#include "freertos_start.h"  //20181218 CHANGE sawada
/******************************************************************
#elif BSP_CFG_RTOS_USED == 2    // SEGGER embOS
#elif BSP_CFG_RTOS_USED == 3    // Micrium MicroC/OS
#elif BSP_CFG_RTOS_USED == 4    // Renesas RI600V4 & RI600PX
#else
#endif
******************************************************************/
//#endif /* BSP_BOARD_GENERIC_RX64M */ //20181218 CHANGE sawada


