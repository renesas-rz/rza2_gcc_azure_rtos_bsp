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
* File Name    : r_register_bit.h
* Descriotion  : Common Functions.
* Version      : 1.12
************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"

/* INTC related headers */
#include  "compiler_settings.h"

/* Register access headers */
#include  "rza_io_regrw.h"

#ifndef R_REGISTER_H
#define R_REGISTER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* Macro: R_REGISTER_WRITE32
*
* Declaration:
*    void  R_REGISTER_WRITE32( volatile uint32_t*  in_RegisterAddress,  uint32_t  in_Value );
*
* Example:
*    R_REGISTER_WRITE32( &UNIT.REG_A,  2 );
************************************************************************/
#define  R_REGISTER_WRITE32( in_RegisterAddress,  in_Value ) \
\
    (RZA_IO_RegWrite_32( \
        &(in_RegisterAddress)->LONG, \
        (in_Value), \
        IOREG_NONSHIFT_ACCESS, \
        IOREG_NONMASK_ACCESS ))


/***********************************************************************
* Macro: R_REGISTER_WRITE16
*
* Declaration:
*    void  R_REGISTER_WRITE16( volatile uint16_t*  in_RegisterAddress,  uint_fast32_t  in_Value );
*
* Example:
*    R_REGISTER_WRITE16( &UNIT.REG_A,  2 );
************************************************************************/
#define  R_REGISTER_WRITE16( in_RegisterAddress,  in_Value ) \
\
    (RZA_IO_RegWrite_16( \
        &(in_RegisterAddress)->WORD, \
        (in_Value), \
        IOREG_NONSHIFT_ACCESS, \
        IOREG_NONMASK_ACCESS ))


/***********************************************************************
* Macro: R_REGISTER_WRITE8
*
* Declaration:
*    void  R_REGISTER_WRITE8( volatile uint8_t*  in_RegisterAddress,  uint_fast32_t  in_Value );
*
* Example:
*    R_REGISTER_WRITE8( &UNIT.REG_A,  2 );
************************************************************************/
#define  R_REGISTER_WRITE8( in_RegisterAddress,  in_Value ) \
\
    (RZA_IO_RegWrite_8( \
        &(in_RegisterAddress)->BYTE, \
        (in_Value), \
        IOREG_NONSHIFT_ACCESS, \
        IOREG_NONMASK_ACCESS ))


/***********************************************************************
* Macro: R_REGISTER_READ32
*
* Declaration:
*    uint32_t  R_REGISTER_READ32( volatile uint32_t*  in_RegisterAddress );
*
* Example:
*    value = R_REGISTER_READ32( &UNIT.REG_A );
************************************************************************/
#define  R_REGISTER_READ32( in_RegisterAddress ) \
\
    (RZA_IO_RegRead_32( \
        &(in_RegisterAddress)->LONG, \
        IOREG_NONSHIFT_ACCESS, \
        IOREG_NONMASK_ACCESS ))


/***********************************************************************
* Macro: R_REGISTER_READ16
*
* Declaration:
*    uint_fast32_t  R_REGISTER_READ16( volatile uint16_t*  in_RegisterAddress );
*
* Example:
*    value = R_REGISTER_READ16( &UNIT.REG_A );
************************************************************************/
#define  R_REGISTER_READ16( in_RegisterAddress ) \
\
    ( (uint_fast32_t) \
        RZA_IO_RegRead_16( \
            &(in_RegisterAddress)->WORD, \
            IOREG_NONSHIFT_ACCESS, \
            IOREG_NONMASK_ACCESS ) \
    )


/***********************************************************************
* Macro: R_REGISTER_READ8
*
* Declaration:
*    uint_fast32_t  R_REGISTER_READ8( volatile uint8_t*  in_RegisterAddress );
*
* Example:
*    value = R_REGISTER_READ8( &UNIT.REG_A );
************************************************************************/
#define  R_REGISTER_READ8( in_RegisterAddress ) \
\
    ( (uint_fast32_t) \
        RZA_IO_RegRead_8( \
            &(in_RegisterAddress)->BYTE, \
            IOREG_NONSHIFT_ACCESS, \
            IOREG_NONMASK_ACCESS ) \
    )


/***********************************************************************
* Macro: R_BIT_SET_IN_REGISTER32
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Example:
*    R_BIT_SET_IN_REGISTER32( CPG,STBCR3,MSTP36, 0 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_SET_IN_REGISTER32( \
        in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    RZA_IO_RegWrite_32( \
        &in_UnitName.in_RegisterName.LONG, \
        in_Value, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_SET_IN_REGISTER16
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Example:
*    R_BIT_SET_IN_REGISTER16( CPG,STBCR3,MSTP36, 0 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_SET_IN_REGISTER16( \
        in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    RZA_IO_RegWrite_16( \
        &in_UnitName.in_RegisterName.WORD, \
        in_Value, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_SET_IN_REGISTER8
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Example:
*    R_BIT_SET_IN_REGISTER8( CPG,STBCR3,MSTP36, 0 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_SET_IN_REGISTER8( \
        in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    RZA_IO_RegWrite_8( \
        &in_UnitName.in_RegisterName.BYTE, \
        in_Value, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_GET_IN_REGISTER32
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Return Value:
*    A read value after shift. uint32_t type.
*
* Example:
*    value = R_BIT_GET_IN_REGISTER32( CPG,STBCR3,MSTP36 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_GET_IN_REGISTER32( \
        in_UnitName,  in_RegisterName,  in_BitName ) \
\
    RZA_IO_RegRead_32( \
        &in_UnitName.in_RegisterName.LONG, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_GET_IN_REGISTER16
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Return Value:
*    A read value after shift. uint16_t type.
*
* Example:
*    value = R_BIT_GET_IN_REGISTER16( CPG,STBCR3,MSTP36 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_GET_IN_REGISTER16( \
        in_UnitName,  in_RegisterName,  in_BitName ) \
\
    RZA_IO_RegRead_16( \
        &in_UnitName.in_RegisterName.WORD, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_GET_IN_REGISTER8
*
* Arguments:
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - uint_fast32_t type. A writing value
*
* Return Value:
*    A read value after shift. uint8_t type.
*
* Example:
*    value = R_BIT_GET_IN_REGISTER8( CPG,STBCR3,MSTP36 );
*        // This macro refers the following macros:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
************************************************************************/
#define  R_BIT_GET_IN_REGISTER8( \
        in_UnitName,  in_RegisterName,  in_BitName ) \
\
    RZA_IO_RegRead_8( \
        &in_UnitName.in_RegisterName.BYTE, \
        in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT, \
        in_UnitName##_##in_RegisterName##_##in_BitName )


/***********************************************************************
* Macro: R_BIT_SET_IN_UINT32
*
* Arguments:
*    in_AddressOfRegister - uint32_t*
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - A writing value
*
* Example:
*    R_BIT_SET_IN_UINT32( &value,
*        CPG,STBCR3,MSTP36,  0 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > *in_out_Value =
*    >     ( *in_out_Value & ~ Mask ) | ( in_Value << Shift & Mask );
************************************************************************/

#define  R_BIT_SET_IN_UINT32( in_out_Value, \
            in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    ( *(in_out_Value) = (uint32_t)( \
        ( \
            ( *(in_out_Value) ) & ~( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) | ( \
            ( \
                ( in_Value ) << ( \
                    in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
                ) \
            ) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) \
    ) )


/***********************************************************************
* Macro: R_BIT_SET_IN_UINT16
*
* Arguments:
*    in_AddressOfRegister - uint16_t*
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - A writing value
*
* Example:
*    R_BIT_SET_IN_UINT16( &value,
*        CPG,STBCR3,MSTP36,  0 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > *in_out_Value =
*    >     ( *in_out_Value & ~ Mask ) | ( in_Value << Shift & Mask );
************************************************************************/

#define  R_BIT_SET_IN_UINT16( in_out_Value, \
            in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    ( *(in_out_Value) = (uint16_t)( \
        ( \
            ( *(in_out_Value) ) & ~( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) | ( \
            ( \
                ( in_Value ) << ( \
                    in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
                ) \
            ) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) \
    ) )


/***********************************************************************
* Macro: R_BIT_SET_IN_UINT8
*
* Arguments:
*    in_AddressOfRegister - uint8_t*
*    in_UnitName          - e.g.) CPG
*    in_RegisterName      - e.g.) STBCR3
*    in_BitName           - e.g.) MSTP36
*    in_Value             - A writing value
*
* Example:
*    R_BIT_SET_IN_UINT8( &value,
*        CPG,STBCR3,MSTP36,  0 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > *in_out_Value =
*    >     ( *in_out_Value & ~ Mask ) | ( in_Value << Shift & Mask );
************************************************************************/

#define  R_BIT_SET_IN_UINT8( in_out_Value, \
            in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    ( *(in_out_Value) = (uint8_t)( \
        ( \
            ( *(in_out_Value) ) & ~( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) | ( \
            ( \
                ( in_Value ) << ( \
                    in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
                ) \
            ) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) \
    ) )


/***********************************************************************
* Macro: R_BIT_GET_IN_UINT32
*
* Arguments:
*    in_Value        - uint32_t
*    in_UnitName     - e.g.) CPG
*    in_RegisterName - e.g.) STBCR3
*    in_BitName      - e.g.) MSTP36
*
* Return Value:
*    A read value after shift. uint_fast32_t type.
*
* Example:
*    value = R_REGISTER_READ32( &CPG.STBCR3 );
*    bit = R_BIT_GET_IN_UINT32( value,
*        CPG,STBCR3,MSTP36 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > return  ( in_Value & Mask ) >> Shift;
************************************************************************/

#define  R_BIT_GET_IN_UINT32( in_Value, \
            in_UnitName,  in_RegisterName,  in_BitName ) \
\
    ( (uint_fast32_t)( \
        ( \
            (in_Value) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) >> ( \
            in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
        ) \
    ) )


/***********************************************************************
* Macro: R_BIT_GET_IN_UINT16
*
* Arguments:
*    in_Value         - uint16_t
*    in_UnitName      - e.g.) CPG
*    in_RegisterName  - e.g.) STBCR3
*    in_BitName       - e.g.) MSTP36
*
* Return Value:
*    A read value after shift. uint_fast16_t type.
*
* Example:
*    value = R_REGISTER_READ16( &CPG.STBCR3 );
*    bit = R_BIT_GET_IN_UINT16( value,
*        CPG,STBCR3,MSTP36 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > return  ( in_Value & Mask ) >> Shift;
************************************************************************/

#define  R_BIT_GET_IN_UINT16( in_Value, \
            in_UnitName,  in_RegisterName,  in_BitName ) \
\
    ( (uint_fast16_t)( \
        ( \
            (in_Value) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) >> ( \
            in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
        ) \
    ) )


/***********************************************************************
* Macro: R_BIT_GET_IN_UINT8
*
* Arguments:
*    in_Value         - uint8_t
*    in_UnitName      - e.g.) CPG
*    in_RegisterName  - e.g.) STBCR3
*    in_BitName       - e.g.) MSTP36
*
* Return Value:
*    A read value after shift. uint_fast8_t type.
*
* Example:
*    value = R_REGISTER_READ8( &CPG.STBCR3 );
*    bit = R_BIT_GET_IN_UINT8( value,
*        CPG,STBCR3,MSTP36 );
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > return  ( in_Value & Mask ) >> Shift;
************************************************************************/

#define  R_BIT_GET_IN_UINT8( in_Value, \
            in_UnitName,  in_RegisterName,  in_BitName ) \
\
    ( (uint_fast8_t)( \
        ( \
            (in_Value) & ( \
                in_UnitName##_##in_RegisterName##_##in_BitName \
            ) \
        ) >> ( \
            in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
        ) \
    ) )


/***********************************************************************
* Macro: R_CO_IS_U_BIT_OVERFLOW
*
* Arguments:
*    in_UnitName      - e.g.) CPG
*    in_RegisterName  - e.g.) STBCR3
*    in_BitName       - e.g.) MSTP36
*    in_Value         - A writing value
*
* Return Value:
*    Whether "in_Value" is overflow from the bits of "in_BitName".
*
* Example:
*    if ( R_CO_IS_U_BIT_OVERFLOW( CPG,STBCR3,MSTP36,  in_Argument ) ) { error(); }
*        // This macro refers:
*        //     CPG_STBCR3_MSTP36
*        //     CPG_STBCR3_MSTP36_SHIFT
*
* Implement:
*    > ( in_Value & ~ ( Mask >> Shift ) ) != 0
************************************************************************/

#define  R_CO_IS_U_BIT_OVERFLOW( \
            in_UnitName,  in_RegisterName,  in_BitName,  in_Value ) \
\
    ( ( ( in_Value ) & ~( \
        ( \
            in_UnitName##_##in_RegisterName##_##in_BitName \
        ) >> ( \
            in_UnitName##_##in_RegisterName##_##in_BitName##_SHIFT \
        ) \
    ) ) != 0 )


/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_REGISTER_H */
