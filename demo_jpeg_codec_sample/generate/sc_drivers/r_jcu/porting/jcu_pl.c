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
* File         : jcu_pl.c
* Description  : JPEG Codec Unit (JCU) Sample Driver. OS Porting Layer. For RZ/A2M.
* Version      : 1.12
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "iodefine.h"
#include  "iobitmask.h"
#include  "r_intc_drv_api.h"
#include  "r_stb_lld_rza2m.h"
#include  "r_jcu.h"
#include  "r_register_bit.h"
#include  "../src/r_jcu_pl.h"
#include  "rz_co.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Constant: JCU_INT_PRIORITY
************************************************************************/
enum { JCU_INT_PRIORITY = 30 };


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
void  JCU_IRQ_JEDI_Handler(void); 
void  JCU_IRQ_JDTI_Handler(void); 
static bool_t  gs_IsEnabledAllInterrupt;


/***********************************************************************
* Function Name: R_JCU_OnInitialize
*    OnInitialize
*
* Arguments:
*    None.
*
* Return Value:
*    Error code, 0=No error.
************************************************************************/
errnum_t  R_JCU_OnInitialize(void) 
{
    int32_t   er;
    errnum_t  e;


    gs_IsEnabledAllInterrupt = true;


    /* Register "JEDI" */
    er= R_INTC_RegistIntFunc( INTC_ID_JPGC_JEDI,  (r_co_intc_function_t) &( JCU_IRQ_JEDI_Handler ) );  /* MISRA 16.9 */
        if ( er != INTC_SUCCESS )
        {
            e=JCU_ERROR_PARAM_61; goto fin;
        }  /* Error: RegistIntFunc */

    /* Cast to an appropriate type */
    er= R_INTC_SetPriority( INTC_ID_JPGC_JEDI,  JCU_INT_PRIORITY );
        if ( er != INTC_SUCCESS )
        {
            e=JCU_ERROR_PARAM_62; goto fin;
        }  /* Error: IntPriority */


    /* Register "JDTI" */
    er= R_INTC_RegistIntFunc( INTC_ID_JPGC_JDTI,  (r_co_intc_function_t) &( JCU_IRQ_JDTI_Handler ) );  /* MISRA 16.9 */
        if ( er != INTC_SUCCESS )
        {
            e=JCU_ERROR_PARAM_63; goto fin;
        }  /* Error: RegistIntFunc */

    /* Cast to an appropriate type */
    er= R_INTC_SetPriority( INTC_ID_JPGC_JDTI,  JCU_INT_PRIORITY );
        if ( er != INTC_SUCCESS )
        {
            e=JCU_ERROR_PARAM_64; goto fin;
        }  /* Error: IntPriority */


    /* start to suuply the clock for JCU */
    { /* ->QAC 0306 */

        /* TARGET_RZA2 */
        R_BIT_SET_IN_REGISTER8( CPG,STBCR5, MSTP51,  0 );  /* 0 = Supply the clock of JCU */
        R_REGISTER_READ8( &CPG.STBCR5 );  /* Dummy read for operation completely */
        while ( R_STB_RequestModuleStart( MODULE_JCU ) == STB_AGAIN )
        {
            /* Spin here forever.. */
            ;
        }
    } /* <-QAC 0306 */

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return  e;
} /* End of function R_JCU_OnInitialize */


/***********************************************************************
* Function Name: R_JCU_OnFinalize
*    OnFinalize
*
* Arguments:
*    None.
*
* Return Value:
*    Error code or e, 0 = successful and input e=0.
************************************************************************/
/* ->QAC 3227 : "e" is usually changed in finalize function. */
errnum_t  R_JCU_OnFinalize(void) 

/* <-QAC 3227 */
{
    /* stop to suuply the clock for JCU */
    { /* ->QAC 0306 */

        /* TARGET_RZA2 */
        while ( R_STB_RequestModuleStop( MODULE_DRW ) == STB_AGAIN )
        {
            /* Spin here forever */
            ;
        }

        /* Cast to an appropriate type */
        R_BIT_SET_IN_REGISTER8( CPG,STBCR5, MSTP51,  1u );  /* 1u = Stop the clock of JCU */
        R_REGISTER_READ8( &CPG.STBCR5 );  /* Dummy read for operation completely */
    } /* <-QAC 0306 */

    return  0;
} /* End of function R_JCU_OnFinalize */


/***********************************************************************
* Function Name: R_JCU_DisableInterrupt
*    Start I-Lock
*
* Arguments:
*    None.
*
* Return Value:
*    Whether interrupts of this function was enabled.
*
* Example:
*    > void  Func()
*    > {
*    >     bool_t  was_all_enabled = false;
*    >
*    >     was_all_enabled = R_..._DisableAllInterrupt();
*    >
*    >     // All interrupt disabled
*    >
*    >     if ( was_all_enabled )
*    >         { R_..._EnableAllInterrupt(); }
*    > }
************************************************************************/
bool_t  R_JCU_DisableInterrupt(void) 
{
    int32_t   er;
    errnum_t  e;
    int32_t   oblivious = 0;
    bool_t    is_enabled_all_interrupt = gs_IsEnabledAllInterrupt;

    gs_IsEnabledAllInterrupt = false;

    /* Cast to an appropriate type */
    er= R_INTC_Disable( (uint16_t) INTC_ID_JPGC_JEDI );
        if ( er != INTC_SUCCESS )
        {
            oblivious = __LINE__;
        }

    /* Cast to an appropriate type */
    er= R_INTC_Disable( (uint16_t) INTC_ID_JPGC_JDTI );
        if ( er != INTC_SUCCESS )
        {
            oblivious = __LINE__;
        }

    if ( oblivious != 0 )
    {
        e=E_JCU_OBLIVIOUS_1;
        goto fin;
    }  /* Error: */
    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return  is_enabled_all_interrupt;
} /* End of function R_JCU_DisableInterrupt */


/***********************************************************************
* Function Name: R_JCU_EnableInterrupt
*    End I-Lock
*
* Arguments:
*    None.
*
* Return Value:
*    None.
************************************************************************/
void  R_JCU_EnableInterrupt(void) 
{
    int32_t   er;
    errnum_t  e;
    int32_t   oblivious = 0;

    gs_IsEnabledAllInterrupt = true;

    /* Cast to an appropriate type */
    er= R_INTC_Enable( (uint16_t) INTC_ID_JPGC_JEDI );
        if ( er != INTC_SUCCESS )
        {
            oblivious = __LINE__;
        }

    /* Cast to an appropriate type */
    er= R_INTC_Enable( (uint16_t) INTC_ID_JPGC_JDTI );
        if ( er != INTC_SUCCESS )
        {
            oblivious = __LINE__;
        }

    if ( oblivious != 0 )
    {
        e=E_JCU_OBLIVIOUS_2;
        goto fin;
    }  /* Error: */
    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
} /* End of function R_JCU_EnableInterrupt */


/***********************************************************************
* Function Name: JCU_IRQ_JEDI_Handler
*    JEDI (JCU Encode Decode Interrupt) interrupt handler.
*
* Arguments:
*    None.
*
* Return Value:
*    None.
************************************************************************/
void  JCU_IRQ_JEDI_Handler() 
{
    R_JCU_OnInterrupting();
} /* End of function JCU_IRQ_JEDI_Handler */


/***********************************************************************
* Function Name: JCU_IRQ_JDTI_Handler
*    JDTI (JCU Data Transfer Interrupt) interrupt handler.
*
* Arguments:
*    None.
*
* Return Value:
*    None.
************************************************************************/
void JCU_IRQ_JDTI_Handler()  
{
    R_JCU_OnInterrupting();
} /* End of function JCU_IRQ_JDTI_Handler */


