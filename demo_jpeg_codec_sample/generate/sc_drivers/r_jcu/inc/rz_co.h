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
* File Name    : rz_co.h
* Description  : Common Functions.
* Version      : 1.12
************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "rz_co_typedef.h"

#ifndef RZ_CO_H
#define RZ_CO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/
/* In "rz_co_typedef.h" */

/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "rz_co_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "rz_co_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* Function Name: R_ERROR_Set
*    Sets error code to current thread.
*
* Arguments:
*    e - Error code
*
* Return Value:
*    None.
*
* Description:
*    If "e" argument is 0, this function do nothing.
*
*    If you set break point at this function, you can see call tree
*    at the place of raising an error.
************************************************************************/
void  R_ERROR_Set( errnum_t e );


/***********************************************************************
* Function: IS_BIT_SET
*    Evaluate whether any passed bits are 1 or not (bit_flags_fast32_t)
*
* Arguments:
*    in_Variable   - The value of target bit flags
*    in_ConstValue - The value that investigating bits are 1
*
* Return Value:
*    Whether the any passed bit are 1
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_BIT_SET( in_Variable, in_ConstValue ) \
    ( ( (in_Variable) & (in_ConstValue) ) != 0u )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: IS_ANY_BITS_SET
*    Evaluate whether any passed bits are 1 or not
*
* Arguments:
*    in_Variable     - The value of target bit flags
*    in_OrConstValue - The value that investigating bits are 1
*
* Return Value:
*    Whether the any passed bit are 1
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_ANY_BITS_SET( in_Variable, in_OrConstValue ) \
    ( ( (in_Variable) & (in_OrConstValue) ) != 0u )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: IS_ALL_BITS_SET
*    Evaluate whether all passed bits are 1 or not
*
* Arguments:
*    in_Variable     - The value of target bit flags
*    in_OrConstValue - The value that investigating bits are 1
*
* Return Value:
*    Whether the all passed bit are 1
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_ALL_BITS_SET( in_Variable, in_OrConstValue ) \
    ( ( (in_Variable) & (in_OrConstValue) ) == (in_OrConstValue) )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: IS_BIT_NOT_SET
*    Evaluate whether the passed bit is 0 or not
*
* Arguments:
*    in_Variable   - The value of target bit flags
*    in_ConstValue - The value that investigating bit is 1
*
* Return Value:
*    Whether the passed bit is 0
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_BIT_NOT_SET( in_Variable, in_ConstValue ) \
    ( ( (in_Variable) & (in_ConstValue) ) == 0u )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: IS_ANY_BITS_NOT_SET
*    Evaluate whether any passed bits are 0 or not
*
* Arguments:
*    in_Variable     - The value of target bit flags
*    in_OrConstValue - The value that investigating bits are 1
*
* Return Value:
*    Whether the any passed bit are 0
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_ANY_BITS_NOT_SET( in_Variable, in_OrConstValue ) \
    ( ( (in_Variable) & (in_OrConstValue) ) != (in_OrConstValue) )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: IS_ALL_BITS_NOT_SET
*    Evaluate whether all passed bits are 0 or not
*
* Arguments:
*    in_Variable     - The value of target bit flags
*    in_OrConstValue - The value that investigating bits are 1
*
* Return Value:
*    Whether the all passed bit are 0
************************************************************************/
/* ->MISRA 19.7 : For return _Bool type */ /* ->SEC M5.1.3 */

#define  IS_ALL_BITS_NOT_SET( in_Variable, in_OrConstValue ) \
    ( ( (in_Variable) & (in_OrConstValue) ) == 0u )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Function: R_CO_CountLeadingZeros
*    Counts bit 0 count from MSB.
*
* Arguments:
*    in_Variable - The value of target bit flags
*
* Return Value:
*    bit 0 count from MSB.
************************************************************************/
#if defined( __CC_ARM )
#define  R_CO_CountLeadingZeros( in_Variable ) \
    ( (int_fast32_t) __clz( in_Variable ) )

#elif defined( __GNUC__ )
#define  R_CO_CountLeadingZeros( in_Variable ) \
    ( (int_fast32_t)(int) __builtin_clz( (unsigned int)( in_Variable ) ) )
    /* gcc warns conversion to "uint_fast32_t" (?) */

#elif defined( __ICCARM__ )
#define  R_CO_CountLeadingZeros( in_Variable ) \
    ( (int_fast32_t) __CLZ( in_Variable ) )
#else
    #error
#endif


/***********************************************************************
* Function Name: R_CO_SetTrue
*    Sets the event. OS-less boolean version.
*
* Arguments:
*    ref_Event - Event to set
*
* Return Value:
*    None
*
* Example:
*    >    volatile bool_t  is_event;
*    >        :
*    >    is_event = false;
*    >    R_DRIVER_TransferStart( (r_co_function_t) R_CO_SetTrue,  &is_event );
*    >    while ( ! is_event )
*    >        { }  // Pooling
************************************************************************/
void  R_CO_SetTrue( volatile bool_t*  ref_Event ); 


/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* Function: R_COUNT_OF
*    Returns element count of the array
*
* Arguments:
*    Array - An array
*
* Return Value:
*    Count of specified array's element
*
* Description:
*    Compatible with _countof (MS C++) and ARRAY_SIZE (Linux).
*    But our naming rule is not match.
*
* Example:
*    > uint32_t  array[10];
*    > R_COUNT_OF( array )  // = 10
*
* Example:
*    Array argument must not be specified the pointer using like array.
*    > uint32_t   array[10];
*    > func( array );
*    >
*    > void  func( uint32_t array[] )  // "array" is a pointer
*    > {
*    >     R_COUNT_OF( array )  // NG
*    > }
************************************************************************/
/* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */

#define  R_COUNT_OF( Array )  ( sizeof( Array ) / sizeof( *(Array) ) )

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Macro: R_UNREFERENCED_VARIABLE
*    Assertion that specified variable is not referenced
*
* Description:
*    This avoids warning of "set but never used".
************************************************************************/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */

#define  R_UNREFERENCED_VARIABLE( a1 ) \
    ((void)(a1))

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */


/***********************************************************************
* Macro: R_UNREFERENCED_VARIABLE_2
*    Assertion that specified variable is not referenced
*
* Description:
*    This avoids warning of "set but never used".
************************************************************************/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */

#define  R_UNREFERENCED_VARIABLE_2( a1, a2 ) \
    ((void)(a1),  (void)(a2))

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */


/***********************************************************************
* Macro: R_UNREFERENCED_VARIABLE_3
*    Assertion that specified variable is not referenced
*
* Description:
*    This avoids warning of "set but never used".
************************************************************************/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */

#define  R_UNREFERENCED_VARIABLE_3( a1, a2, a3 ) \
    ((void)(a1),  (void)(a2),  (void)(a3))

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */


/***********************************************************************
* Macro: R_UNREFERENCED_VARIABLE_4
*    Assertion that specified variable is not referenced
*
* Description:
*    This avoids warning of "set but never used".
************************************************************************/
/* ->MISRA 19.7 : Argument must be any type */ /* ->SEC M5.1.3 */

#define  R_UNREFERENCED_VARIABLE_4( a1, a2, a3, a4 ) \
    ((void)(a1),  (void)(a2),  (void)(a3),  (void)(a4))

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* ->QAC 3206 : Not used argument */


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* RZ_CO_H */
