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
* File         : jcu_reg.c
* Description  : JPEG Codec Unit (JCU) driver. Setting registers.
* Version      : 1.12
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "iodefine.h"
#include  "iobitmask.h"
#include  "r_stb_lld_rza2m.h"
#include  "../src/r_jcu_local.h"
#include  "r_register_bit.h"
#include  "rz_co.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define NUMBER_OF_QUANTIZATION_TABLE_DATA   (64)
#define NUMBER_OF_HUFFMAN_TABLE_DATA_DC     (28)
#define NUMBER_OF_HUFFMAN_TABLE_DATA_AC     (178)

#define CBCR_OFFSET_NUM     (2u)

#define SWAP_NORMAL         (0u)
#define SWAP_16BIT_OUTPUT   (1u)
#define SWAP_32BIT_OUTPUT   (2u)

#define KIND_COLOR_FORMAT   (3u)
#define KIND_LINEOFFSET     (2u)
#define KIND_TABLE          (4u)


/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
STATIC_INLINE uint8_t  GET_UPPER_BYTE( uint32_t const  value );
STATIC_INLINE uint8_t  GET_LOWER_BYTE( uint32_t const  value );


/***********************************************************************
* Function Name: GET_UPPER_BYTE
*    GET_UPPER_BYTE
*
* Return Value:
*    Byte
************************************************************************/
STATIC_INLINE uint8_t  GET_UPPER_BYTE( uint32_t const  value ) 
{
    enum { num_8 = 8 }; /* SEC M1.10.1, QAC-3132 */ 
    return  (uint8_t)( value >> num_8 );
} /* End of function GET_UPPER_BYTE */

/***********************************************************************
* Function Name: GET_LOWER_BYTE
*    GET_LOWER_BYTE
*
* Return Value:
*    Byte
************************************************************************/
STATIC_INLINE uint8_t  GET_LOWER_BYTE( uint32_t const  value ) 
{
    /* Cast to an appropriate type */
    return  (uint8_t)( value );
} /* End of function GET_LOWER_BYTE */

/***********************************************************************
* Function Name: JCU_SetRegisterForInitialize
*    SetRegisterForInitialize
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_SetRegisterForInitialize( 
    void)
{
    return JCU_ERROR_OK;
} /* End of function JCU_SetRegisterForInitialize */

/***********************************************************************
* Function Name: JCU_SetRegisterForCodec
*    SetRegisterForCodec
*
* Arguments:
*    codec
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForCodec( const jcu_codec_t  codec ) 
{
    volatile uint8_t  dummy_read;


    /* Reset this IP */
    while ( R_STB_RequestModuleStop( MODULE_JCU ) == STB_AGAIN )
    {
        /* Spin here forever.. */
        ;
    }

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( CPG,SWRSTCR1,SRST11,  1 );

    /* Cast to an appropriate type */
    dummy_read = R_BIT_GET_IN_REGISTER8( CPG,SWRSTCR1,SRST11 );

        /* Cast to an appropriate type */
        R_UNREFERENCED_VARIABLE( dummy_read );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( CPG,SWRSTCR1,SRST11,  0 );

    /* Cast to an appropriate type */
    dummy_read = R_BIT_GET_IN_REGISTER8( CPG,SWRSTCR1,SRST11 );

        /* Cast to an appropriate type */
        R_UNREFERENCED_VARIABLE( dummy_read );
    while ( R_STB_RequestModuleStart( MODULE_JCU ) == STB_AGAIN )
        {
            /* Spin here forever */
            ;
        }

    /* Issue bus reset of JCU. */
    R_BIT_SET_IN_REGISTER8( JCU,JCCMD,BRST,  1 );

    /* Enable interrupts in registers (Interrupt lines in INTC are still disabled) */
    /* This registers are set to 0 by reset this IP */
    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JINTE1,  JCU_JINTS1_ALL );

    /* <-QAC 0306 */

    /* Clear the all of count mode settings */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT,DINLC,  0 );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT,DINRINI,  0 );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFESLC,  0u );

    /* <-QAC 0306 */

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT,JOUTC,  0 );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT,JOUTRINI,  0 );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFEDDC,  0u );

    /* <-QAC 0306 */

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT,JINC,  0 );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT,JINRINI,  0 );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFDSDC,  0u );

    /* <-QAC 0306 */

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT,DOUTLC,  0 );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT,DOUTRINI,  0 );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFDDLC,  0u );

    /* <-QAC 0306 */

    /* Set the which process is select */
    R_BIT_SET_IN_REGISTER8( JCU,JCMOD,DSP,  codec );
    return;
}  /* End of function JCU_SetRegisterForCodec */

/***********************************************************************
* Function Name: JCU_ClearInterruptFlag
*    ClearInterruptFlag
*
* Return Value:
*    None
************************************************************************/
void JCU_ClearInterruptFlag(void) 
{
    uint32_t  value;

    /* Interrupt request clear */
    R_BIT_SET_IN_REGISTER8( JCU,JCCMD,JEND,  1 );

    /* Clear the JEDI interrupt source flag */
    /* ->QAC 0306 */
    R_REGISTER_WRITE8( &JCU.JINTS0,  0u );  /* Write 0 only */

    /* <-QAC 0306 */

    /* Clear the JDTI interrupt source flag */
    /* ->QAC 0306 */
    value = R_REGISTER_READ32( &JCU.JINTS1 );


    value &= (~JCU_JINTS1_ALL);

    /* Cast to an appropriate type */
    R_REGISTER_WRITE32( &JCU.JINTS1,  value );

    /* <-QAC 0306 */
    return;
}  /* End of function JCU_ClearInterruptFlag */

/***********************************************************************
* Function Name: JCU_SetRegisterForStart
*    SetRegisterForStart
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForStart(const jcu_int_detail_errors_t error_filter) 
{
    uint32_t  reg_jinte0;

    /* Write error_filter */
    reg_jinte0 = (uint32_t) R_REGISTER_READ8( &JCU.JINTE0 )  &  (uint32_t) ~JCU_INT_ERROR_ALL;

    /* Cast to an appropriate type */
    reg_jinte0 |= (uint32_t) error_filter;

    /* Cast to an appropriate type */
    R_REGISTER_WRITE8( &JCU.JINTE0,  (uint8_t) reg_jinte0 );

    /* Start */
    R_BIT_SET_IN_REGISTER8( JCU,JCCMD,JSRT,  1 );
    return;
}  /* End of function JCU_SetRegisterForStart */

/***********************************************************************
* Function Name: JCU_SetRegisterForContinue
*    SetRegisterForContinue
*
* Arguments:
*     type - [in]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForContinue(const jcu_continue_type_t  type) 
{
    errnum_t  e;
    jcu_internal_information_t*
        const  self = R_JCU_GetInternalInformation(); 

    #if ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( self == NULL )
        {
            e=JCU_ERROR_PARAM_2A;
            goto fin;
        } /* Error: <Parameter/> */
    #endif

    /* Restart register setting */
    switch (type)
    {
        case JCU_INPUT_BUFFER:
            if (self->Codec == JCU_STATUS_DECODE)
            {
                /* Cast to an appropriate type */
                R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, JINRCMD,  1 );
            }
            else
            {
                /* Cast to an appropriate type */
                R_BIT_SET_IN_REGISTER32( JCU,JIFECNT, DINRCMD,  1 );
            } /* end if */
                break;

        case JCU_OUTPUT_BUFFER:
            if (self->Codec == JCU_STATUS_DECODE)
            {
                /* Cast to an appropriate type */
                R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, DOUTRCMD,  1 );
            }
            else
            {
                /* Cast to an appropriate type */
                R_BIT_SET_IN_REGISTER32( JCU,JIFECNT, JOUTRCMD,  1 );
            } /* end if */
                break;

        case JCU_IMAGE_INFO:

            /* Cast to an appropriate type */
            R_BIT_SET_IN_REGISTER8( JCU,JCCMD, JRST,  1 );
                break;

        /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:

            /* <-QAC 2018 */
            /* NOT REACHED */
                break;
    } /* end switch */

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;
} /* End of function JCU_SetRegisterForContinue */

/***********************************************************************
* Function Name: JCU_SetRegisterForSetDecodePrm
*    SetRegisterForSetDecod
*
* Arguments:
*     decode - [in]
*     buffer - [in]
*     interruptKind - [in]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForSetDecodePrm( 
    const jcu_decode_param_t   *const decode,
    const jcu_buffer_param_t   *const buffer)
{
    const uint32_t byteSize[KIND_COLOR_FORMAT] =   

        /* ->SEC M1.10.1 Table data. */
        {  2u, /* JCU_OUTPUT_YCbCr422 */
            4u, /* JCU_OUTPUT_ARGB8888 */
            2u  /* JCU_OUTPUT_RGB565   */
        };

        /* <-SEC M1.10.1 */
    errnum_t  e;

    #if ! defined( JCU_RELEASE )
        /* Cast to an appropriate type */
        if ( decode == NULL )
        {
            e=JCU_ERROR_PARAM_2B; goto fin;
        } /* Error: <Parameter/> */
        if ( buffer == NULL )
        {
            e=JCU_ERROR_PARAM_2C; goto fin;
        } /* Error: <Parameter/> */
    #endif

    /* Settings for the cull */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, VINTER,  decode->verticalSubSampling );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, HINTER,  decode->horizontalSubSampling );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, OPF,     decode->decodeFormat );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, JINSWAP,   buffer->source.swapSetting );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDCNT, DOUTSWAP,  buffer->destination.swapSetting );

    /* Settings for input jpeg file information */
    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFDSA,  (uintptr_t) buffer->source.address );

    /* <-QAC 0306 */
    /* Settings for output image data information */
    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFDDA,  (uintptr_t) buffer->destination.address );

    /* <-QAC 0306 */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDDOFST, DDMW,
        ((uint32_t)buffer->lineOffset * (uint32_t)(byteSize[decode->decodeFormat])) );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFDADT, ALPHA,
        (uint32_t)(decode->alpha) );


    /* Setting for Cb/Cr offset */
    R_BIT_SET_IN_REGISTER32( JCU,JIFESHSZ, DOUTYCHG,  decode->outputCbCrOffset );

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;

} /* End of function JCU_SetRegisterForSetDecodePrm */

/***********************************************************************
* Function Name: JCU_SetRegisterForSetQtTable
*    SetRegisterForSetQtTable
*
* Arguments:
*     tableNo - [in]
*     table   - [in]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForSetQtTable( 
    const jcu_table_no_t   tableNo, 
    const uint8_t* const   table)
{
    volatile uint8_t*
        tableAtRegister;  
    int_fast32_t       counter;
    errnum_t           e;

    /* Select the destination of the Quantization table */
    switch (tableNo)
    {
        /* ->QAC 0306 */
        case JCU_TABLE_NO_0:

            /* Cast to an appropriate type */
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL0;
        break;
        case JCU_TABLE_NO_1:

            /* Cast to an appropriate type */
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL1;
        break;
        case JCU_TABLE_NO_2:

            /* Cast to an appropriate type */
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL2;
        break;
        case JCU_TABLE_NO_3:

            /* Cast to an appropriate type */
            tableAtRegister = (volatile uint8_t *)&JCU.JCQTBL3;
        break;

        /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:

        /* <-QAC 2018 */
            /* NOT REACHED */
            return;

        /* <-QAC 0306 */
    } /* end switch */

    #if ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( tableAtRegister == NULL )
        {
            e=JCU_ERROR_PARAM_2D; goto fin;
        } /* Error: <Parameter/> */

        /* Cast to an appropriate type */
        if ( table == NULL )
        {
            e=JCU_ERROR_PARAM_2E; goto fin;
        } /* Error: <Parameter/> */
    #endif
    
    /* Update the quantization table data */
    for (counter = 0; counter < NUMBER_OF_QUANTIZATION_TABLE_DATA; counter++)
    {
        RZA_IO_RegWrite_8( &tableAtRegister[ counter ],  table[ counter ],
                IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS );
    } /* end for */

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;
} /* End of function JCU_SetRegisterForSetQtTable */

/***********************************************************************
* Function Name: JCU_SetRegisterForSetHuffTbl
*    SetRegisterForSetHuffTbl
*
* Arguments:
*     tableNo - [in]
*     type    - [in]
*     table   - [in]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForSetHuffTbl( 
    const jcu_table_no_t   tableNo, 
    const jcu_huff_t       type,
    const uint8_t *const   table)
{
    volatile uint8_t*
        tableAtRegister;  
    int_fast32_t       copyCount; 
    int_fast32_t       counter;
    errnum_t           e;

    /* Select the destination Huffman table and the size of copy data */
    if (tableNo == JCU_TABLE_NO_0)
    {
        /* ->QAC 0306 */
        if (type == JCU_HUFFMAN_DC)
        {
            /* Cast to an appropriate type */
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBD0;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_DC;
        }
        else
        {
            /* Cast to an appropriate type */
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBA0;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_AC;
        } /* end if */

        /* <-QAC 0306 */
    }
    else
    {
        /* ->QAC 0306 */
        if (type == JCU_HUFFMAN_DC)
        {
            /* Cast to an appropriate type */
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBD1;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_DC;
        }
        else
        {
            /* Cast to an appropriate type */
            tableAtRegister  = (volatile uint8_t *)&JCU.JCHTBA1;
            copyCount        = NUMBER_OF_HUFFMAN_TABLE_DATA_AC;
        } /* end if */
        /* <-QAC 0306 */
    }  /* end if */

    #if ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( tableAtRegister == NULL )
        {
            e=JCU_ERROR_PARAM_2F; goto fin;
        } /* Error: <Parameter/> */

        /* Cast to an appropriate type */
        if ( table == NULL )
        {
            e=JCU_ERROR_PARAM_30; goto fin;
        } /* Error: <Parameter/> */
    #endif

    /* Update the Huffman table */
    for (counter = 0; counter < copyCount; counter++)
    {
        RZA_IO_RegWrite_8( &tableAtRegister[ counter ],  table[ counter ],
            IOREG_NONSHIFT_ACCESS,  IOREG_NONMASK_ACCESS );
    } /* end for */

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;
} /* End of function JCU_SetRegisterForSetHuffTbl */

/***********************************************************************
* Function Name: JCU_SetRegisterForSetEncodePrm
*    SetRegisterForSetEncode
*
* Arguments:
*     encode - [in]
*     buffer - [in]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForSetEncodePrm(  
    const jcu_encode_param_t   *const encode, 
    const jcu_buffer_param_t   *const buffer)
{
    errnum_t  e;
    jcu_jpeg_format_t encodeFormatTemp; 
    const uint32_t byteSize[KIND_LINEOFFSET] =   

        /* ->SEC M1.10.1 Table data. */
        {  0u, /* Reserved          */
            2u  /* JCU_JPEG_YCbCr422 */ };

        /* <-SEC M1.10.1 */

    #if ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( encode == NULL )
        {
            e=JCU_ERROR_PARAM_31; goto fin;
        } /* Error: <Parameter/> */

        /* Cast to an appropriate type */
        if ( buffer == NULL )
        {
            e=JCU_ERROR_PARAM_32; goto fin;
        } /* Error: <Parameter/> */
    #endif

    /* Settings for the input image format */
    R_BIT_SET_IN_REGISTER8( JCU,JCMOD, REDU,  encode->encodeFormat );

    /* Settings for the quantization table */
    R_BIT_SET_IN_REGISTER8( JCU,JCQTN, QT1,  encode->QuantizationTable[JCU_ELEMENT_Y] );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCQTN, QT2,  encode->QuantizationTable[JCU_ELEMENT_Cb] );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCQTN, QT3,  encode->QuantizationTable[JCU_ELEMENT_Cr] );

    /* Settings for the Huffman table */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTA1,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Y])  );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTD1,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Y])  );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTA2,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cb]) );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTD2,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cb]) );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTA3,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cr]) );

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER8( JCU,JCHTN, HTD3,  (uint32_t)(encode->HuffmanTable[JCU_ELEMENT_Cr]) );

    /* ->QAC 0306 */
    R_REGISTER_WRITE8( &JCU.JCDRIU,  GET_UPPER_BYTE( encode->DRI_value ) );

    /* Cast to an appropriate type */
    R_REGISTER_WRITE8( &JCU.JCDRID,  GET_LOWER_BYTE( encode->DRI_value ) );
    /* Settings for the image size */
    /* width setting */
    R_REGISTER_WRITE8( &JCU.JCHSZU,  GET_UPPER_BYTE( encode->width ) );

    /* Cast to an appropriate type */
    R_REGISTER_WRITE8( &JCU.JCHSZD,  GET_LOWER_BYTE( encode->width ) );
    /* height setting */
    R_REGISTER_WRITE8( &JCU.JCVSZU,  GET_UPPER_BYTE( encode->height ) );

    /* Cast to an appropriate type */
    R_REGISTER_WRITE8( &JCU.JCVSZD,  GET_LOWER_BYTE( encode->height ) );
    /* <-QAC 0306 */


    /* Settings for input jpeg file information */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT, DINSWAP,  buffer->source.swapSetting );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFESA,  (uintptr_t) buffer->source.address );
    /* <-QAC 0306 */

    /* Settings for output image data information */
    R_BIT_SET_IN_REGISTER32( JCU,JIFECNT, JOUTSWAP,  buffer->destination.swapSetting );

    /* ->QAC 0306 */
    R_REGISTER_WRITE32( &JCU.JIFEDA,  (uintptr_t) buffer->destination.address );
    /* <-QAC 0306 */

    encodeFormatTemp = encode->encodeFormat;
    if((encodeFormatTemp != JCU_JPEG_YCbCr444) && (encodeFormatTemp != JCU_JPEG_YCbCr422))
    {
        encodeFormatTemp = JCU_JPEG_YCbCr444;
    } /* end if */

    /* Cast to an appropriate type */
    R_BIT_SET_IN_REGISTER32( JCU,JIFESOFST, ESMW,
        ((uint32_t)buffer->lineOffset * (uint32_t)(byteSize[encodeFormatTemp])) );

    /* Setting for Cb/Cr offset */
    R_BIT_SET_IN_REGISTER32( JCU,JIFESVSZ, DINYCHG,  encode->inputCbCrOffset );

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;

} /* End of function JCU_SetRegisterForSetEncodePrm */

/***********************************************************************
* Function Name: JCU_SetRegisterForSetPause
*
* Arguments:
*     is_stop - [out]
*
* Return Value:
*    None
************************************************************************/
void JCU_SetRegisterForSetPause( const bool_t is_pause, 
        const jcu_int_detail_errors_t filter )
{
    /* ->QAC 0306 */
    if ( is_pause )
    {
        /* Cast to an appropriate type */
        uint8_t  filter_or_int3 = (uint8_t)( filter | JCU_JINTE0_INT3 );

        /* Cast to an appropriate type */
        R_REGISTER_WRITE8( &JCU.JINTE0,  filter_or_int3 );
    }
    else
    {
        /* Cast to an appropriate type */
        R_REGISTER_WRITE8( &JCU.JINTE0,  (uint8_t) filter );
    }
    /* <-QAC 0306 */
} /* End of function JCU_SetRegisterForSetPause */

/***********************************************************************
* Function Name: JCU_GetRegisterForGetImageInfo
*    Get "jcu_image_info_t" type value
*
* Arguments:
*     buffer - [out]<jcu_image_info_t>.
*
* Return Value:
*    None
************************************************************************/
void JCU_GetRegisterForGetImageInfo( 
    jcu_image_info_t *const buffer)
{
    enum { num_8_bit = 8 }; 
    errnum_t  e;

    #if ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( buffer == NULL )
        {
            e=JCU_ERROR_PARAM_33; goto fin;
        } /* Error: <Parameter/> */
    #endif

    /* Set the register value to the variables */
    /* ->QAC 0306 */
    buffer->width           = ( R_REGISTER_READ8( &JCU.JCHSZU ) << num_8_bit);

    /* Cast to an appropriate type */
    buffer->width           |=  R_REGISTER_READ8( &JCU.JCHSZD );

    /* Cast to an appropriate type */
    buffer->height          = ( R_REGISTER_READ8( &JCU.JCVSZU ) << num_8_bit);

    /* Cast to an appropriate type */
    buffer->height          |=  R_REGISTER_READ8( &JCU.JCVSZD );

    /* Cast to an appropriate type */
    buffer->encodedFormat   = (jcu_jpeg_format_t)R_BIT_GET_IN_REGISTER8( JCU,JCMOD, REDU );

    /* <-QAC 0306 */

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return;
} /* End of function JCU_GetRegisterForGetImageInfo */

/***********************************************************************
* Function Name: JCU_GetRegisterForGetErrorInfo
*    Get "jcu_detail_error_t" type value
*
* Arguments:
*     errorCode - [out].
*
* Return Value:
*    None
************************************************************************/
void JCU_GetRegisterForGetErrorInfo( 
    jcu_detail_error_t *const errorCode)
{
    /* Cast to an appropriate type */
    if (errorCode != NULL)
    {
        /* ->QAC 0306 */
        uint8_t const  code = R_BIT_GET_IN_REGISTER8( JCU,JCDERR, ERR );  

        /* <-QAC 0306 */

        /* Cast to an appropriate type */
        if ( (uint_fast8_t) code == 0u )
        {
            *errorCode = JCU_JCDERR_OK;
        }
        else
        {
            /* Cast to an appropriate type */
            *errorCode = (jcu_detail_error_t)( E_JCU_JCDERR + ( (int_t) code << 8 ) );
        }
    } /* end if */
    return;
} /* End of function JCU_GetRegisterForGetErrorInfo */

/***********************************************************************
* Function Name: JCU_GetEncodedSize
*    GetEncodedSize
*
* Arguments:
*     out_Size - [out].
*
* Return Value:
*    None
************************************************************************/
void JCU_GetEncodedSize( 
    size_t *const out_Size)
{
    /* Cast to an appropriate type */
    if (out_Size != NULL)
    {
        enum { num_16_bits = 16 }; 
        enum { num_8_bits = 8 }; 

        /* ->QAC 0306 */
        *out_Size  = (( R_REGISTER_READ8( &JCU.JCDTCU )) << num_16_bits);

        /* Cast to an appropriate type */
        (*out_Size) |= (( R_REGISTER_READ8( &JCU.JCDTCM )) << num_8_bits);

        /* Cast to an appropriate type */
        (*out_Size) |=  ( R_REGISTER_READ8( &JCU.JCDTCD ));

        /* <-QAC 0306 */
    } /* end if */
    return;
} /* End of function JCU_GetEncodedSize */

/***********************************************************************
* Function Name: JCU_OnInterrupting
*    Operations on interrupting
*
* Return Value:
*    Error code, 0=No error
************************************************************************/
void  JCU_OnInterrupting( bit_flags_fast32_t*  in_out_InterruptFlags ) 
{
    enum { num_8_bit = 8 }; 

    uint32_t  flags0;
    uint32_t  flags1;

    /* Get interrupt status */
    /* ->QAC 0306 */
    flags0 = R_REGISTER_READ8(  &JCU.JINTS0 );

    /* Cast to an appropriate type */
    flags1 = R_REGISTER_READ32( &JCU.JINTS1 );

    /* Cast to an appropriate type */
    (*in_out_InterruptFlags) |= flags0;

    /* Cast to an appropriate type */
    (*in_out_InterruptFlags) |= (flags1 << ((sizeof( JCU.JINTS0 )) * num_8_bit));

    /* Clear interrupt status "JINTS0" */
    if ( flags0 != 0u )
    {
        /* Cast to an appropriate type */
        R_REGISTER_WRITE8( &JCU.JINTS0,  0u );  /* Write 0 only */
        R_BIT_SET_IN_REGISTER8( JCU,JCCMD,  JEND,  1u );
    }

    /* Clear interrupt status "JINTS1" */
    R_REGISTER_WRITE32( &JCU.JINTS1,  ~flags1 & JCU_JINTS1_ALL );

    /* <-QAC 0306 */
} /* End of function JCU_OnInterrupting */

