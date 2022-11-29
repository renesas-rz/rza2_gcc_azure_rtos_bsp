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
* File         : rz_co_typedef.h
* Description  : Common Types.
* Version      : 1.12
************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>

#ifndef RZ_CO_TYPEDEF_H
#define RZ_CO_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Constants: Error code of global errnum_t type
************************************************************************/

/***********************************************************************
* Constant: E_OTHER
*    Error code that means parameter error or other error.
************************************************************************/
/* Constant: E_OTHER_MASK
*    Mask for checking E_OTHER. */
#define  E_OTHER_MASK              (0x7F000000)
#define  E_OTHER                   (0x01000000)


/***********************************************************************
* Constant: E_FEW_ARRAY
*    Error code that means error of few fixed length array.
************************************************************************/
/* Constant: E_FEW_ARRAY_MASK
*    Mask for checking E_FEW_ARRAY. */
#define  E_FEW_ARRAY_MASK          (0x7F000000)
#define  E_FEW_ARRAY               (0x02000000)


/***********************************************************************
* Constant: E_FEW_MEMORY
*    Error code that means few heap memory area.
************************************************************************/
/* Constant: E_FEW_MEMORY_MASK
*    Mask for checking E_FEW_MEMORY. */
#define  E_FEW_MEMORY_MASK         (0x7F000000)
#define  E_FEW_MEMORY              (0x03000000)


/***********************************************************************
* Constant: E_FIFO_OVER
*    Error code that means failed to enqueue.
************************************************************************/
/* Constant: E_FIFO_OVER_MASK
*    Mask for checking E_FIFO_OVER. */
#define  E_FIFO_OVER_MASK          (0x7F000000)
#define  E_FIFO_OVER               (0x04000000)


/***********************************************************************
* Constant: E_NOT_FOUND_SYMBOL
*    Error code that means not defined the symbol.
************************************************************************/
/* Constant: E_NOT_FOUND_SYMBOL_MASK
*    Mask for checking E_NOT_FOUND_SYMBOL. */
#define  E_NOT_FOUND_SYMBOL_MASK   (0x7F000000)
#define  E_NOT_FOUND_SYMBOL        (0x05000000)


/***********************************************************************
* Constant: E_NO_NEXT
*    Error code that means there is not next element of list.
************************************************************************/
/* Constant: E_NO_NEXT_MASK
*    Mask for checking E_NO_NEXT. */
#define  E_NO_NEXT_MASK            (0x7F000000)
#define  E_NO_NEXT                 (0x06000000)


/***********************************************************************
* Constant: E_ACCESS_DENIED
*    Error code that means error of denied read or write.
************************************************************************/
/* Constant: E_ACCESS_DENIED_MASK
*    Mask for checking E_ACCESS_DENIED. */
#define  E_ACCESS_DENIED_MASK      (0x7F000000)
#define  E_ACCESS_DENIED           (0x07000000)


/***********************************************************************
* Constant: E_NOT_IMPLEMENT_YET
*    Error code that means not implemented yet.
************************************************************************/
/* Constant: E_NOT_IMPLEMENT_YET_MASK
*    Mask for checking E_NOT_IMPLEMENT_YET. */
#define  E_NOT_IMPLEMENT_YET_MASK  (0x7F000000)
#define  E_NOT_IMPLEMENT_YET       (0x09000000)


/***********************************************************************
* Constant: E_OBLIVIOUS
*    Error code that means failure oblivious logged.
************************************************************************/
/* Constant: E_OBLIVIOUS_MASK
*    Mask for checking E_OBLIVIOUS. */
#define  E_OBLIVIOUS_MASK          (0x7F000000)
#define  E_OBLIVIOUS               (0x0B000000)


/***********************************************************************
* Constant: E_STATE_CLEAR
*    Error code that means clearing error state. This is not an error.
************************************************************************/
/* Constant: E_STATE_CLEAR_MASK
*    Mask for checking E_STATE_CLEAR. */
#define  E_STATE_CLEAR_MASK        (0x7F000000)
#define  E_STATE_CLEAR             (0x0C000000)


/***********************************************************************
* Constant: E_UNRECOVERABLE
*    Error code that means unrecoverable by self.
************************************************************************/
/* Constant: E_UNRECOVERABLE_MASK
*    Mask for checking E_UNRECOVERABLE. */
#define  E_UNRECOVERABLE_MASK      (0x7F000000)
#define  E_UNRECOVERABLE           (0x0D000000)


/***********************************************************************
* Constant: E_ERRNO
*    Error code that means Refer to "errno" of standard I/O.
************************************************************************/
/* Constant: E_ERRNO_MASK
*    Mask for checking E_ERRNO. */
#define  E_ERRNO_MASK              (0x7F000000)
#define  E_ERRNO                   (0x0E000000)


/***********************************************************************
* Constant: E_LIMITATION
*    Error code that means a limitation.
************************************************************************/
/* Constant: E_LIMITATION_MASK
*    Mask for checking E_LIMITATION. */
#define  E_LIMITATION_MASK         (0x7F000000)
#define  E_LIMITATION              (0x0F000000)


/***********************************************************************
* Constant: E_STATE
*    Error code that means it cannot do at this state.
************************************************************************/
/* Constant: E_STATE_MASK
*    Mask for checking E_STATE. */
#define  E_STATE_MASK              (0x7F000000)
#define  E_STATE                   (0x10000000)


/***********************************************************************
* Constant: E_NOT_THREAD
*    Error code that means not a thread, it cannot be called from interrupt context.
************************************************************************/
/* Constant: E_NOT_THREAD_MASK
*    Mask for checking E_NOT_THREAD. */
#define  E_NOT_THREAD_MASK         (0x7F000000)
#define  E_NOT_THREAD              (0x11000000)


/***********************************************************************
* Constant: E_THREAD_DENIED
*    Error code that means the object is locked by other thread.
************************************************************************/
/* Constant: E_THREAD_DENIED_MASK
*    Mask for checking E_THREAD_DENIED. */
#define  E_THREAD_DENIED_MASK      (0x7F000000)
#define  E_THREAD_DENIED           (0x12000000)


/***********************************************************************
* Constant: E_PATH_NOT_FOUND
*    Error code that means not found file or folder.
************************************************************************/
/* Constant: E_PATH_NOT_FOUND_MASK
*    Mask for checking E_PATH_NOT_FOUND. */
#define  E_PATH_NOT_FOUND_MASK     (0x7F000000)
#define  E_PATH_NOT_FOUND          (0x13000000)


/***********************************************************************
* Constant: E_BAD_COMMAND_ID
*    Error code that means out of number of command ID.
************************************************************************/
/* Constant: E_BAD_COMMAND_ID_MASK
*    Mask for checking E_BAD_COMMAND_ID. */
#define  E_BAD_COMMAND_ID_MASK     (0x7F000000)
#define  E_BAD_COMMAND_ID          (0x16000000)


/***********************************************************************
* Constant: E_TIME_OUT
*    Error code that means time out.
************************************************************************/
/* Constant: E_TIME_OUT_MASK
*    Mask for checking E_TIME_OUT. */
#define  E_TIME_OUT_MASK           (0x7F000000)
#define  E_TIME_OUT                (0x17000000)


/***********************************************************************
* Constant: E_STACK_OVERFLOW
*    Error code that means stack overflow.
************************************************************************/
/* Constant: E_STACK_OVERFLOW_MASK
*    Mask for checking E_STACK_OVERFLOW. */
#define  E_STACK_OVERFLOW_MASK     (0x7F000000)
#define  E_STACK_OVERFLOW          (0x1C000000)


/***********************************************************************
* Constant: E_EXIT_TEST
*    Error code that means a request of exit from the test.
************************************************************************/
/* Constant: E_EXIT_TEST_MASK
*    Mask for checking E_EXIT_TEST. */
#define  E_EXIT_TEST_MASK          (0x7F000000)
#define  E_EXIT_TEST               (0x1E000000)


/***********************************************************************
* Macro: STATIC_INLINE
*    Static inline in C source file
************************************************************************/
#ifndef  __cplusplus

#ifdef __CC_ARM
#define  STATIC_INLINE  static __inline 
#endif

#ifdef __ICCARM__
#define  STATIC_INLINE  static inline 
#endif

#if  defined( __GNUC__ )  &&  ! defined( __CC_ARM )
#define  STATIC_INLINE  static inline 
#endif

#ifdef _SH
#define  STATIC_INLINE  static
#endif

#else

#define  STATIC_INLINE  static inline

#endif  /* __cplusplus */


/***********************************************************************
* Macro: R_VARIABLE_SECTION
*    Names section name to function or varaible
************************************************************************/
/***********************************************************************
* Macro: R_VARIABLE_SECTION_FOR_ZERO_INIT
*    Names section name to zero initialized varaible
************************************************************************/
/* ->MISRA 19.10 : Cannot ( ) */ /* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#if defined( __CC_ARM )
    #define  R_VARIABLE_SECTION( SectionName, Declaration ) \
        __attribute__ ((section (SectionName)))  Declaration 

    #define  R_VARIABLE_SECTION_FOR_ZERO_INIT( SectionName, Declaration ) \
        __attribute__ ((section (SectionName), zero_init))  Declaration 

#elif defined( __GNUC__ )  &&  ! defined( __CC_ARM )
    #define  R_VARIABLE_SECTION( SectionName, Declaration ) \
        __attribute__ ((section (SectionName)))  Declaration 

    #define  R_VARIABLE_SECTION_FOR_ZERO_INIT( SectionName, Declaration ) \
        __attribute__ ((section (SectionName)))  Declaration 

#elif defined( __ICCARM__ )
    #define  R_VARIABLE_SECTION( SectionName, Declaration ) \
        Declaration @ SectionName 

    #define  R_VARIABLE_SECTION_FOR_ZERO_INIT( SectionName, Declaration ) \
        Declaration @ SectionName 

#elif defined( _SH )  /* For SH compiler */
    #define  R_VARIABLE_SECTION( SectionName, Declaration ) \
        Declaration 
        /* No section qualifier */
        /* #ifdef _SH */
        /* #pragma section <section_name> */
        /* #endif */
        /*   :   */
        /* <Not extern code> */
        /*   :   */
        /* #ifdef _SH */
        /* #pragma section */
        /* #endif */

    #define  R_VARIABLE_SECTION_FOR_ZERO_INIT( SectionName, Declaration ) \
        Declaration

#else
    #error
#endif
/* <-MISRA 19.10 */ /* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/***********************************************************************
* Macro: R_VARIABLE_ALIGNMENT
*    Alignments first addres of global variable
*
* Arguments:
*    ByteCount                     - Value of alignment
*    Declaration_without_Semicolon - Declaration of the variable
*
* Example:
*    > R_VARIABLE_ALIGNMENT( 0x100,
*    > int  g_Buffer[0x1000] );
************************************************************************/
/* ->MISRA 19.10 : Cannot ( ) */ /* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#if defined( __CC_ARM )
    #define  R_VARIABLE_ALIGNMENT( ByteCount, Declaration_without_Semicolon ) \
        __align( ByteCount )  Declaration_without_Semicolon 

#elif defined( __GNUC__ )  &&  ! defined( __CC_ARM )
    #define  R_VARIABLE_ALIGNMENT( ByteCount, Declaration_without_Semicolon ) \
        __attribute__((aligned( ByteCount )))  Declaration_without_Semicolon 

#elif defined( __ICCARM__ )
    #define  R_VARIABLE_ALIGNMENT( ByteCount, Declaration_without_Semicolon ) \
        R_VARIABLE_ALIGNMENT_SUB( ByteCount, Declaration_without_Semicolon ) 
    #define  R_VARIABLE_ALIGNMENT_SUB( ByteCount, Declaration_without_Semicolon ) \
        _Pragma( "diag_suppress=Pe606" ) \
        _Pragma( "diag_suppress=Pa060" ) \
        _Pragma( "diag_suppress=Pe609" ) \
        R_VARIABLE_ALIGNMENT_##ByteCount() \
        Declaration_without_Semicolon \
        R_VARIABLE_ALIGNMENT_##0x4()

    #define  R_VARIABLE_ALIGNMENT_0x4()      _Pragma( "data_alignment=0x4" ) 
    #define  R_VARIABLE_ALIGNMENT_0x8()      _Pragma( "data_alignment=0x8" ) 
    #define  R_VARIABLE_ALIGNMENT_0x10()     _Pragma( "data_alignment=0x10" ) 
    #define  R_VARIABLE_ALIGNMENT_0x20()     _Pragma( "data_alignment=0x20" ) 
    #define  R_VARIABLE_ALIGNMENT_0x40()     _Pragma( "data_alignment=0x40" ) 
    #define  R_VARIABLE_ALIGNMENT_0x80()     _Pragma( "data_alignment=0x80" ) 
    #define  R_VARIABLE_ALIGNMENT_0x100()    _Pragma( "data_alignment=0x100" ) 
    #define  R_VARIABLE_ALIGNMENT_0x200()    _Pragma( "data_alignment=0x200" ) 
    #define  R_VARIABLE_ALIGNMENT_0x400()    _Pragma( "data_alignment=0x400" ) 
    #define  R_VARIABLE_ALIGNMENT_0x800()    _Pragma( "data_alignment=0x800" ) 
    #define  R_VARIABLE_ALIGNMENT_0x1000()   _Pragma( "data_alignment=0x1000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x2000()   _Pragma( "data_alignment=0x2000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x4000()   _Pragma( "data_alignment=0x4000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x8000()   _Pragma( "data_alignment=0x8000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x10000()  _Pragma( "data_alignment=0x10000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x20000()  _Pragma( "data_alignment=0x20000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x40000()  _Pragma( "data_alignment=0x40000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x80000()  _Pragma( "data_alignment=0x80000" ) 
    #define  R_VARIABLE_ALIGNMENT_0x100000() _Pragma( "data_alignment=0x100000" ) 

    #define  R_VARIABLE_ALIGNMENT_4()        _Pragma( "data_alignment=4" ) 
    #define  R_VARIABLE_ALIGNMENT_8()        _Pragma( "data_alignment=8" ) 
    #define  R_VARIABLE_ALIGNMENT_16()       _Pragma( "data_alignment=16" ) 
    #define  R_VARIABLE_ALIGNMENT_32()       _Pragma( "data_alignment=32" ) 
    #define  R_VARIABLE_ALIGNMENT_64()       _Pragma( "data_alignment=64" ) 
    #define  R_VARIABLE_ALIGNMENT_128()      _Pragma( "data_alignment=128" ) 
    #define  R_VARIABLE_ALIGNMENT_256()      _Pragma( "data_alignment=256" ) 
    #define  R_VARIABLE_ALIGNMENT_512()      _Pragma( "data_alignment=512" ) 
    #define  R_VARIABLE_ALIGNMENT_1024()     _Pragma( "data_alignment=1024" ) 
    #define  R_VARIABLE_ALIGNMENT_2048()     _Pragma( "data_alignment=2048" ) 
    #define  R_VARIABLE_ALIGNMENT_4096()     _Pragma( "data_alignment=4096" ) 
    #define  R_VARIABLE_ALIGNMENT_8192()     _Pragma( "data_alignment=8192" ) 
    #define  R_VARIABLE_ALIGNMENT_16384()    _Pragma( "data_alignment=16384" ) 
    #define  R_VARIABLE_ALIGNMENT_32768()    _Pragma( "data_alignment=32768" ) 
    #define  R_VARIABLE_ALIGNMENT_65536()    _Pragma( "data_alignment=65536" ) 

#elif defined( _SH )
    #define  R_VARIABLE_ALIGNMENT( ByteCount, Declaration_without_Semicolon ) \
        Declaration_without_Semicolon 
        /* No alignment qualifier */
        /* Set aligned address by "Map section information" */

#else
    #error
#endif
/* <-MISRA 19.10 */ /* <-MISRA 19.7 */ /* <-SEC M5.1.3 */


/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Type: bit_flags_fast32_t
*    Bit flags as "uint_fast32_t"
************************************************************************/
typedef uint_fast32_t  bit_flags_fast32_t;


/***********************************************************************
* Type: bit_flags32_t
*    Bit flags as "uint32_t"
************************************************************************/
typedef uint32_t  bit_flags32_t;


/***********************************************************************
* Type: bit_flags16_t
*    Bit flags as "uint16_t"
************************************************************************/
typedef uint16_t  bit_flags16_t;


/***********************************************************************
* Type: bit_flags8_t
*    Bit flags as "uint8_t"
************************************************************************/
typedef uint8_t  bit_flags8_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Function Name: r_co_function_t
*    Type of general callback function
*
* Arguments:
*    in_Argument - A value specified when callback function was registered
*
* Return Value:
*    None
*
* Example:
*    > errnum_t  R_DRIVER_TransferStart( r_co_function_t  in_CallbackFunction,
*    >     volatile void*  in_Argument )
*    > {
*    >         :
*    >     in_CallbackFunction( in_Argument );  // R_CO_SetTrue( &event_A );
*    > }
*    >
*    > e= R_DRIVER_TransferStart( (r_co_function_t) R_CO_SetTrue,  &event_A );
************************************************************************/
typedef void  (* r_co_function_t )( volatile void* in_Argument );


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Function Name: r_co_intc_function_t
*    Type of interrupt callback function. This is for casting
*
* Arguments:
*    Arguments are depended on RTOS.
*
* Return Value:
*    Return value is depended on RTOS.
*
* Description:
*    Even if caller of this interrupt callback function calls with an argument,
*    define the interrupt callback function without any argument.
*
* Example:
*    > void  R_DRIVER_OnInterrupt(void)
*    > {
*    >         :
*    > }
*    >
*    > e= R_INTC_RegistIntFunc( (r_co_intc_function_t) &( R_DRIVER_OnInterrupt ) );  // MISRA 16.9
************************************************************************/
typedef void  (* r_co_intc_function_t )( uint32_t  in_Ignored );


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Constant: BSP_CFG_RTOS_USED
*    Whether there is RTOS that supports preemption
*
* Description:
*    This value is 0 in OSPL for OS less.
*    This value is 1 in OSPL for RTOS.
*    Even if it is not round robin, it is 1 when preemption is made by interrupt or API call.
************************************************************************/
#define  BSP_CFG_RTOS_USED  (1)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Type: errnum_t
*    Error code. 0=NoError.
*
* Description:
*    If an error was raised, the function that returns errnum_t type returns
*    non zeto value. Also, <R_ERROR_Set> is callbacked with 0 value at
*    errnum_t type argument.
*
*    You can find the error message by searching the symbol or numeric value
*    (8 digit hexadecimal number. e.g. 0x01234567).
*    You can find the source code raising the error by conditional break point.
*    The condition is the value of errnum_t type argument.
*
*    Bit field in the error code is:
*
*    > 3       2         1
*    > 1       4         6         8         0
*    > ---------------------------------------
*    > 0GGG GGGG MMMM MMMM SSSS SSSS IIII IIII
*    >
*    > 0 - 0 only
*    > G - Defined in global error code
*    > M - Defined in each module. ID of the module
*    > S - Defined in each module. Error code of the module
*    > I - Defined in each module. Internal ID
*
*    The length of S and I depends on the kind of S.
*
*    Error code must be compared with mask.
*    Because internal ID must be ignored.
*    > if ( ( e & E_FEW_MEMORY_MASK ) == E_FEW_MEMORY ) { }
************************************************************************/
typedef int  errnum_t; 


/***********************************************************************
* Type: bool8_t
*    Boolean type in 8 bit variable. This is not C99.
************************************************************************/
typedef uint8_t  bool8_t;


/***********************************************************************
* Type: bool16_t
*    Boolean type in 16 bit variable. This is not C99.
************************************************************************/
typedef uint16_t  bool16_t;


/***********************************************************************
* Type: bool32_t
*    Boolean type in 32 bit variable. This is not C99.
************************************************************************/
typedef uint32_t  bool32_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* In "rz_co.h" */


/***********************************************************************
* Macro: R_JOIN_SYMBOL_FOR_ASSERT
*    Sub routine of <R_STATIC_ASSERT>
*
* Description:
*    - This macro extends "x", "y"
*    - This code is referenced by CERT secure coding standard PRE05-C
************************************************************************/
    /* ->MISRA 19.7 : Extend macro arguments */ /* ->SEC M5.1.3 */
#define R_JOIN_SYMBOL_FOR_ASSERT(x, y)  R_JOIN_SYMBOL_FOR_ASSERT_SUB(x, y) 
    /* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
    /* ->MISRA 19.13 : This is used only assertion in compiling */ /* ->SEC M5.1.2 (1) */
#define R_JOIN_SYMBOL_FOR_ASSERT_SUB(x, y) x##y 
    /* <-MISRA 19.13 */ /* <-SEC M5.1.2 (1) */


/***********************************************************************
* Macro: R_PRAGMA_STRINGIFY
*    Changes a symbol to a string
************************************************************************/
#ifdef __GNUC__
#define  R_PRAGMA_STRINGIFY(a) #a 
#endif


/***********************************************************************
* Macro: R_STATIC_ASSERT
*    "static_assert" for in function
*
* Description:
*    Compatible with static_assert (C++0x). But our naming rule is not match.
*    This code is referenced by CERT secure coding standard DCL03-C.
*    This macro raised QAC warning 3205. This is no problem.
************************************************************************/
/* ->SEC M1.1.1 */
#ifndef __GNUC__
#define  R_STATIC_ASSERT( ConstantExpression, StringLiteral ) R_STATIC_ASSERT_SUB( (ConstantExpression), (StringLiteral) ) 
#else
#define  R_STATIC_ASSERT( ConstantExpression, StringLiteral ) \
    _Pragma(R_PRAGMA_STRINGIFY(GCC diagnostic ignored "-Wunused-local-typedefs")) R_STATIC_ASSERT_SUB( (ConstantExpression), (StringLiteral) ) 
#endif

#define  R_STATIC_ASSERT_SUB( ConstantExpression, StringLiteral ) \
    do { typedef char_t R_JOIN_SYMBOL_FOR_ASSERT( assertion_failed_t_, __LINE__ ) [(ConstantExpression) ? 1 : -1]; } while(0) 
    /* If "ConstantExpression" is false, illegal array size error will be raised. */

/* <-SEC M1.1.1 */


/***********************************************************************
* Macro: R_STATIC_ASSERT_GLOBAL
*    "static_assert" for in global scope
************************************************************************/
/* ->SEC M1.1.1 */
/* ->MISRA 19.4 : There is not if statement in global */ /* ->SEC M1.8.2 */
/* ->MISRA 19.7 : Cannot function */ /* ->SEC M5.1.3 */
#ifndef __GNUC__
#define  R_STATIC_ASSERT_GLOBAL( ConstantExpression, StringLiteral ) R_STATIC_ASSERT_GLOBAL_SUB( (ConstantExpression), (StringLiteral) ) 
#else
#define  R_STATIC_ASSERT_GLOBAL( ConstantExpression, StringLiteral ) _Pragma(R_PRAGMA_STRINGIFY(GCC diagnostic ignored "-Wunused-local-typedefs")) R_STATIC_ASSERT_GLOBAL_SUB( (ConstantExpression), (StringLiteral) ) 
#endif

#define  R_STATIC_ASSERT_GLOBAL_SUB( ConstantExpression, StringLiteral ) \
    typedef char_t R_JOIN_SYMBOL_FOR_ASSERT( assertion_failed_t_, __LINE__ ) [(ConstantExpression) ? 1 : -1]
    /* If "ConstantExpression" is false, illegal array size error will be raised. */

/* <-MISRA 19.7 */ /* <-SEC M5.1.3 */
/* <-MISRA 19.4 */ /* <-SEC M1.8.2 */
/* <-SEC M1.1.1 */


/* Section: Document */
/******************************************************************************
* Reference: Section_ValueMeaningEmptyAndFlaggedParameter
*    Value meaning empty and Flagged Structure
*
* Description:
*    RGA for RZ/A2M is supported to both value meaning empty and flagged structure.
*    User writes code by either coding pattern.
*
*    If the value of member variable is the value meaning empty, it is treated that
*    the default value is set or not change for the member variable. "*_SetEmpty"
*    function initializes all member variables to a value meaning empty.
*
*    > FuncA_ConfigClass config;
*    >
*    > FuncA_CONFIG_SetEmpty( &config );
*    > config.Param1 = 10;
*    > config.Param2 = 2;
*    >
*    > FuncA( &config );
*
*    "config.Param3" defined in "FuncA_ConfigClass" has a value meaning empty
*    by calling "FuncA_CONFIG_SetEmpty" function, then the default value is
*    set or not change for the member variable.
*
*    Flags member variables in the structure are used as a bit field.
*    When the bit is 1, the corresponding member variable is enabled in this coding pattern.
*    When the bit is 0, it is treated that the default value is set or not change for
*    the member variable as omitted. Updating the version allows binary compatibility
*    even when structure members increase.
*
*    > FuncA_ConfigClass config;
*    >
*    > config.Flags = F_FuncA_Param1 | F_FuncA_Param2;
*    > config.Param1 = 10;
*    > config.Param2 = 2;
*    >
*    > FuncA( &config );
*
*    The default value is config.Param3 because "Flags |= F_FuncA_Param3" does not exist.
******************************************************************************/

/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* RZ_CO_TYPEDEF_H */
