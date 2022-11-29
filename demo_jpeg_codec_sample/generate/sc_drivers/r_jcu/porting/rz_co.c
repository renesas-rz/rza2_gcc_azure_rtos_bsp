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
/***********************************************************************
* File         : rz_co.c
* Description  : Common Functions.
* Version      : 1.12
************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "use_config_debug.h"
#include  <stdio.h>
#include  "rz_co.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define  GS_N  "\r\n"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
#ifdef  USE_LIST_DEBUG_TOOL
void  R_USER_DebugPrint(void);
#endif

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/***********************************************************************
* Variable: g_CallingCountOfNotClearedError
************************************************************************/
#ifdef  USE_LIST_DEBUG_TOOL
int_fast32_t  g_CallingCountOfNotClearedError = 0;
#endif


/***********************************************************************
* Function Name: R_ERROR_Set
*
* Description:
*    To debug, set break point at "if ( calling_count >= 9999 )" or change 9999.
************************************************************************/
void  R_ERROR_Set( errnum_t e ) 
{
    if ( e )
    {

        /* printf <ERROR/>  and  R_DebugBreak */
        #ifdef  USE_LIST_DEBUG_TOOL
            bool_t  is_error_being_cleared = ( ( e & E_STATE_CLEAR_MASK ) == E_STATE_CLEAR );
            bool_t  is_failure_oblivious   = ( ( e & E_OBLIVIOUS_MASK   ) == E_OBLIVIOUS );
            bool_t  is_new_error = ( g_CallingCountOfNotClearedError == 0 );

            static int_fast32_t  calling_count = 0;

            calling_count += 1;

            /* g_CallingCountOfNotClearedError = ... */
            if ( is_error_being_cleared ) {
                g_CallingCountOfNotClearedError = 0;
            } else if ( is_new_error  &&  ! is_failure_oblivious ) {
                g_CallingCountOfNotClearedError = calling_count;
            }

            /* printf <ERROR/>  and  R_DebugBreak */
            if ( calling_count >= 9999 ) {  /* If 1 then, set break point here without changing source */

                /* R_USER_DebugPrint(); */

                if ( ! is_error_being_cleared ) {
                    printf( "<Error num=\"0x%08X\"  calling_count=\"%d\"/>"GS_N,  e,  calling_count );
                    if ( ! is_failure_oblivious ) {

                        R_DebugBreak();
                    }
                } else { /* is_error_being_cleared */
                    printf( "<Error clear=\"0x%08X\"  calling_count=\"%d\"/>"GS_N,  e,  calling_count );
                }
            }
        #endif /* USE_LIST_DEBUG_TOOL */
    }
} /* End of function R_ERROR_Set */


/***********************************************************************
* Function Name: R_CO_SetTrue
************************************************************************/
void  R_CO_SetTrue( volatile bool_t*  ref_Event ) 
{
    *ref_Event = true;
} /* End of function R_CO_SetTrue */


