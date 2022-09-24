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
 * File Name :   perform.h
 * @file         perform.h
 * @version      0.01
 * @brief        graphics perform header
 ******************************************************************************/
#ifndef _PERFORM_H
#define _PERFORM_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
* Function Name: PerformInit
* Description  : Initialization of performance measurement processing
*              : (initial count clock of OSTM 1: P1: 66.666666 [MHz], 1 count: 15 [ns])
* Arguments    : -
* Return Value : 0  success
*                -1 fail
 *****************************************************************************/
int32_t PerformInit(void);

/******************************************************************************
* Function Name: PerformSetStartTime
* Description  : Save start time of performance measurement
* Arguments    : timer_no
*              :   Timer number used for performance measurement
* Return Value : -
 *****************************************************************************/
void PerformSetStartTime(uint32_t timer_no);

/******************************************************************************
* Function Name: PerformSetEndTime
* Description  : Save end time of performance measurement
* Arguments    : timer_no
*              :   Timer number used for performance measurement
* Return Value : -
 *****************************************************************************/
void PerformSetEndTime(uint32_t timer_no);

/******************************************************************************
* Function Name: PerformGetElapsedTime_ns
* Description  : Return elapsed time[ns]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
 *****************************************************************************/
uint32_t PerformGetElapsedTime_ns(uint32_t timer_no);

/******************************************************************************
* Function Name: PerformGetElapsedTime_us
* Description  : Return elapsed time[us]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
 *****************************************************************************/
uint32_t PerformGetElapsedTime_us(uint32_t timer_no);

/******************************************************************************
* Function Name: PerformGetElapsedTime_ms
* Description  : Return elapsed time[ms]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
 *****************************************************************************/
uint32_t PerformGetElapsedTime_ms(uint32_t timer_no);

#ifdef __cplusplus
}
#endif

#endif  /* _PERFORM_H */

/* End of File */
