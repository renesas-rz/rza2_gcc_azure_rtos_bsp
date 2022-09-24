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
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"

#include "r_intc_drv_api.h"
#include "r_stb_drv_api.h"

#include "iodefine.h"

#include "perform.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define TIMER_MAX   (256)

#define START_TIME  (0)
#define END_TIME    (1)
#define ELAPSE_TIME (2)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint32_t overhead;
static uint32_t app_time[TIMER_MAX][ELAPSE_TIME + 1];    /* 0:START_TIME 1:END_TIME 2:ELAPSE_TIME */

static uint32_t GetElapsedClockCount(uint32_t timer_no);

/******************************************************************************
* Function Name: PerformInit
* Description  : Initialization of performance measurement processing
*              : (initial count clock of OSTM 1: P1: 66.666666 [MHz], 1 count: 15 [ns])
* Arguments    : -
* Return Value : 0  success
*                -1 fail
******************************************************************************/
int32_t PerformInit(void)
{
    uint32_t time1;
    uint32_t time2;

    /* OSTM0 is used as the run time stats counter. */
    R_STB_StartModule(MODULE_OSTM0);

    /* Stop the counter. */
    OSTM0.OSTMnTT.BIT.OSTMnTT = 1;

    /* ---- OSTM compare register (CMP) setting ---- */
    OSTM0.OSTMnCMP.LONG =  (uint32_t)(- 1) ;

    /* Work in compare mode mode. */
    OSTM0.OSTMnCTL.BIT.OSTMnMD1 = 1;

    /* Don't use interrupts. */
    OSTM0.OSTMnCTL.BIT.OSTMnMD0 = 0;

    /* Start counting */
    OSTM0.OSTMnTS.BIT.OSTMnTS = 1;

    /* Cast to an appropriate type */
    time1 = OSTM0.OSTMnCNT.LONG;

    /* Cast to an appropriate type */
    time2 = OSTM0.OSTMnCNT.LONG;
    overhead = time2 - time1;

    return 0;
}   /* End of function PerformInit() */

/******************************************************************************
* Function Name: PerformSetStartTime
* Description  : Save start time of performance measurement
* Arguments    : timer_no
*              :   Timer number used for performance measurement
* Return Value : -
******************************************************************************/
void PerformSetStartTime(uint32_t timer_no)
{
    if (timer_no < TIMER_MAX)
    {
        /* Cast to an appropriate type */
        app_time[timer_no][START_TIME] = OSTM0.OSTMnCNT.LONG;
    }

    return;
}   /* End of function PerformSetStartTime() */

/******************************************************************************
* Function Name: PerformSetEndTime
* Description  : Save end time of performance measurement
* Arguments    : timer_no
*              :   Timer number used for performance measurement
* Return Value : -
******************************************************************************/
void PerformSetEndTime(uint32_t timer_no)
{
    if (timer_no < TIMER_MAX)
    {
        /* Cast to an appropriate type */
        app_time[timer_no][END_TIME] = OSTM0.OSTMnCNT.LONG;
    }

    return;
}   /* End of function PerformSetEndTime() */

/******************************************************************************
* Function Name: PerformGetElapsedTime_ns
* Description  : Return elapsed time[ns]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
******************************************************************************/
uint32_t PerformGetElapsedTime_ns(uint32_t timer_no)
{
    uint32_t elapsed_time_ns = 0;

    if (timer_no < TIMER_MAX)
    {
        /* Calculate the number of clocks from the start time to the end time */
        elapsed_time_ns = GetElapsedClockCount(timer_no);

        /* Multiply by the period of 1 clock to calculate elapsed time (ns) */
        elapsed_time_ns *= 15;
    }

    return elapsed_time_ns;
}   /* End of function PerformGetElapsedTime_ns() */

/******************************************************************************
* Function Name: PerformGetElapsedTime_us
* Description  : Return elapsed time[us]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
******************************************************************************/
uint32_t PerformGetElapsedTime_us(uint32_t timer_no)
{
    uint32_t elapsed_time_us = 0;

    if (timer_no < TIMER_MAX)
    {
        /* Calculate the number of clocks from the start time to the end time */
        elapsed_time_us = GetElapsedClockCount(timer_no);

        /* Multiply by the period of 1 clock to calculate elapsed time (us) */
        elapsed_time_us /= 66;
    }

    return elapsed_time_us;
}   /* End of function PerformGetElapsedTime_us() */

/******************************************************************************
* Function Name: PerformGetElapsedTime_ms
* Description  : Return elapsed time[ms]
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed time
* Return Value : Elapsed time
******************************************************************************/
uint32_t PerformGetElapsedTime_ms(uint32_t timer_no)
{
    uint32_t elapsed_time_ms = 0;

    if (timer_no < TIMER_MAX)
    {
        /* Calculate the number of clocks from the start time to the end time */
        elapsed_time_ms = GetElapsedClockCount(timer_no);

        /* Multiply by the period of 1 clock to calculate elapsed time (us) */
        elapsed_time_ms /= 66000;
    }

    return elapsed_time_ms;
}   /* End of function PerformGetElapsedTime_ms() */

/******************************************************************************
* Function Name: GetElapsedClockCount
* Description  : Return elapsed clock count (66.666[MHz])
* Arguments    : timer_no
*                  Number of the timer that acquires the elapsed clock count
* Return Value : Elapsed clock count
******************************************************************************/
static uint32_t GetElapsedClockCount(uint32_t timer_no)
{
    uint32_t clock_count = 0;

    if (timer_no < TIMER_MAX)
    {
        if (app_time[timer_no][END_TIME] > app_time[timer_no][START_TIME])
        {
            /* Calculate the number of clocks from the start time to the end time */
            clock_count = (app_time[timer_no][END_TIME] - app_time[timer_no][START_TIME]) - overhead;
        }
        else
        {
            /* Calculate the number of clocks from the start time to the end time */
            clock_count = (((0xFFFFFFFF - app_time[timer_no][START_TIME]) + 1) + app_time[timer_no][END_TIME])
                            - overhead;
        }
    }

    return clock_count;
}   /* End of function GetElapsedClockCount() */

/* End of file */
