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
* File: decode_encode_sample.c
*     Decode and encode a JPEG image.
* - Device(s)    : RZ/A2M
* - Tool-Chain   : -
* - OS           : -
* - H/W Platform : RZ/A2M CPU board
* - Description  : -
* - Operation    : None.
* - Limitations  : None.
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdio.h>
#include  <string.h>
#include  "jcu_swap.h"
#include  "r_cache_lld_rza2m.h"
#include  "r_jcu.h"
#include  "r_memory_map.h"
#include  "rz_co.h"
#include  "r_os_abstraction_api.h"
#include  "VRAM.h"
#include  "JCU_ExampleImage.h"
#include  "application.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Defines: Macros in this file scope. Global static defines (GS)
************************************************************************/
#define  GS_N  "\r\n"


/***********************************************************************
* Constant: GS_OUTPUT_PIXEL_FORMAT
*    JCU_OUTPUT_YCbCr422. JCU can encode only YCbCr422 format.
************************************************************************/
#define  GS_OUTPUT_PIXEL_FORMAT  (JCU_OUTPUT_YCbCr422)


/***********************************************************************
* Defines:
************************************************************************/
#define  GS_FRAME_BYTE_PER_PIXEL  (2u)  /* ARGB8888 is 4,  RGB565 is 2 */
#define  GS_FRAME_WIDTH           (800u)
#define  GS_FRAME_HEIGHT          (480u)
#define  GS_ALPHA_VAL_MAX         (0xFF)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint8_t*  gs_memory_address_of_RAW; 
static size_t    gs_memory_size_of_RAW;
static uint8_t*  gs_memory_address_of_JPEG; 
static size_t    gs_memory_size_of_JPEG;

static errnum_t  gs_AllocateMemory(void);
static errnum_t  gs_EncodeToJPEG(void);
static errnum_t  gs_DecodeJPEG(void);


/***********************************************************************
* Function Name: R_JCU_SampleDecodeEncode
*    Decodes and encode JPEG data.
*
* Arguments:
*    None.
*
* Returns:
*    Error Code. 0=No Error.
************************************************************************/
errnum_t  R_JCU_SampleDecodeEncode(void)
{
    errnum_t  e;

    e= gs_AllocateMemory();
    if(e)
    {
        goto fin;
    }
    e= gs_DecodeJPEG();
    if(e)
    {
        goto fin;
    }
    e= gs_EncodeToJPEG();
    if(e)
    {
        goto fin;
    }

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return  e;
}/* End of function R_JCU_SampleDecodeEncode */



/***********************************************************************
* Function Name: gs_AllocateMemory
*    AllocateMemory
*
* Arguments:
*    None.
*
* Returns:
*    Error Code. 0=No Error.
************************************************************************/
static errnum_t  gs_AllocateMemory(void) 
{
    errnum_t  e;
    size_t    frame_size = GS_FRAME_WIDTH * GS_FRAME_BYTE_PER_PIXEL * GS_FRAME_HEIGHT;
    size_t    size_of_VRAM; 
    uint8_t*
        memory_address_of_VRAM;

    e= R_SAMPLE_GetVRAM( &memory_address_of_VRAM,  &size_of_VRAM );
    if(e)
    {
        goto fin;
    }

    /* (variables) = ... */
    gs_memory_address_of_RAW  = memory_address_of_VRAM;
    gs_memory_size_of_RAW     = frame_size;
    gs_memory_address_of_JPEG = memory_address_of_VRAM + frame_size;
    gs_memory_size_of_JPEG    = size_of_VRAM - frame_size;

    /* Guard */
    if (!( ((uintptr_t) gs_memory_address_of_RAW  % 8) == 0 ))
    {
        e=__LINE__;
        goto fin;
    }

    /* Cast to an appropriate type */
    if (!( ((uintptr_t) gs_memory_address_of_JPEG % 8) == 0 ))
    {
        e=__LINE__;
        goto fin;
    }

    /* Cast to an appropriate type */
    if (!( ((uintptr_t) gs_memory_size_of_JPEG) > 0 ))
    {
        e=__LINE__;
        goto fin;
    }

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }
    return  e;
}/* End of function gs_AllocateMemory */


/***********************************************************************
* Function Name: gs_DecodeJPEG
*    Decodes JPEG data to a memory
*
* Arguments:
*    None.
*
* Returns:
*    Error Code. 0=No Error.
************************************************************************/
static errnum_t  gs_DecodeJPEG(void)  
{
    const uint8_t*      address_of_JPEG_image = g_jcu_example_image; 
    size_t const        size_of_JPEG_image = sizeof( g_jcu_example_image );

    errnum_t            e;
    bool_t              s;
    jcu_decode_param_t  decode;
    jcu_buffer_param_t  buffer;
    bool_t              is_JCU_initialized = false;
    uintptr_t           physical_address_of_RAW;
    uintptr_t           physical_address_of_JPEG;
    uint32_t            decoded_event = 0;


    /* Check JPEG data exists */
    {
        static const  uint8_t  num_0xFF_JPEG_header_letter_1 = 0xFFu;
        static const  uint8_t  num_0xD8_JPEG_header_letter_2 = 0xD8u;

        if (!(

            /* Cast to an appropriate type */
            ( (uint32_t) address_of_JPEG_image[0] == num_0xFF_JPEG_header_letter_1 ) &&

            /* Cast to an appropriate type */
            ( (uint32_t) address_of_JPEG_image[1] == num_0xD8_JPEG_header_letter_2 ) ))
        {
            e = E_OTHER;
            goto fin;
        } /* Error: JPEG data is not in ROM */
    }


    /* (variables) = ... */
    {
        /* physical_address_of_RAW = ... */
        e= R_MEMORY_ToPhysicalAddress( gs_memory_address_of_RAW,   &physical_address_of_RAW );
        if(e)
        {
            goto fin;
        }

        /* physical_address_of_JPEG = ... */
        e= R_MEMORY_ToPhysicalAddress( gs_memory_address_of_JPEG,  &physical_address_of_JPEG );
        if(e)
        {
            goto fin;
        }
        if ( !( gs_memory_size_of_JPEG > size_of_JPEG_image ))
        {
            e=__LINE__;
            goto fin;
        }

        /* decoded_event = ... */
        s= R_OS_SemaphoreCreate( &decoded_event,  1 );
        if(!s)
        {
            e=__LINE__;
            goto fin;
        }
    }


    /* Write JPEG data in physical memory */
    memcpy( gs_memory_address_of_JPEG,  address_of_JPEG_image,  size_of_JPEG_image );
    memset( gs_memory_address_of_RAW,  0,  gs_memory_size_of_RAW );

    R_CACHE_L1DataCleanInvalidAll();


    /* R_JCU_Initialize */
    e= R_JCU_Initialize( NULL );
    if(e)
    {
        goto fin;
    }
    is_JCU_initialized = true;

    e= R_JCU_SelectCodec( JCU_DECODE );
    if(e)
    {
        goto fin;
    }


    /* R_JCU_SetDecodeParam */
    {
        buffer.source.swapSetting       = JCU_SWAP_JPEG_NONE;

        /* Cast to an appropriate type */
        buffer.source.address           = (uint32_t*) physical_address_of_JPEG;

        /* Cast to an appropriate type */
        buffer.destination.address      = (uint32_t*) physical_address_of_RAW;

        /* Cast to an appropriate type */
        buffer.lineOffset               = (int16_t) GS_FRAME_WIDTH;
        decode.decodeFormat             = GS_OUTPUT_PIXEL_FORMAT;

        if ( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_YCbCr422 )
        {
            buffer.destination.swapSetting  = JCU_SWAP_CB_Y0_CR_Y1_PIXEL0123;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_128;
        }
        else if ( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_RGB565 )
        {
            buffer.destination.swapSetting  = JCU_SWAP_RGB565_PIXEL0123;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_0;
        }
        else
        {
            if (!( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_ARGB8888 ))
            {
                e=E_OTHER; goto fin;
            }
            buffer.destination.swapSetting  = JCU_SWAP_ARGB8888_PIXEL01;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_0;
        }

        decode.alpha                 = GS_ALPHA_VAL_MAX;
        decode.horizontalSubSampling = JCU_SUB_SAMPLING_1_1;
        decode.verticalSubSampling   = JCU_SUB_SAMPLING_1_1;

        e= R_JCU_SetDecodeParam( &decode,  &buffer );
        if(e)
        {
            goto fin;
        }
    }


    /* R_JCU_StartAsync() : Decode JPEG image */
    s= R_OS_SemaphoreWait( &decoded_event,  0 ); /* Clear. Time out must be ignored. */

    /* Cast to an appropriate type */
    e= R_JCU_StartAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &decoded_event );
    if(e)
    {
        goto fin;
    } /* Set */
    s= R_OS_SemaphoreWait( &decoded_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
    if(!s)
    {
        e=__LINE__; goto fin;
    }
    e= R_JCU_GetAsyncError();
    if(e)
    {
        goto fin;
    }

    e=0;
fin:
    /* R_JCU_TerminateAsync() */
    if ( is_JCU_initialized )
    {
        errnum_t  ee;
        uint32_t  terminate_event = 0;
        s= R_OS_SemaphoreCreate( &terminate_event,  1 );
        if((!s) && (e==0))
        {
            e=__LINE__;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  0 ); /* Clear. Time out must be ignored. */

        /* Cast to an appropriate type */
        ee = R_JCU_TerminateAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &terminate_event ); /* Set */
        if(e==0)
        {
            e=ee;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
        if((!s) && (e==0))
        {
            e=__LINE__;
        }
        ee= R_JCU_GetAsyncError();
        if(e==0)
        {
            e=ee;
        }
        R_OS_SemaphoreDelete( &terminate_event );
    }

    /* ... */
    R_OS_SemaphoreDelete( &decoded_event );
    R_ERROR_Set( e );
    return  e;
}/* End of function gs_DecodeJPEG */


/***********************************************************************
* Function Name: gs_EncodeToJPEG
*    Encode to JPEG data.
*
* Arguments:
*    None.
*
* Returns:
*    Error Code. 0=No Error.
************************************************************************/
static errnum_t  gs_EncodeToJPEG(void) 
{
    errnum_t            e;
    bool_t              s;
    jcu_buffer_param_t  buffer;
    bool_t              is_JCU_initialized = false;
    uintptr_t           physical_address_of_RAW;
    uintptr_t           physical_address_of_JPEG;
    uint32_t            encoded_event = 0;
    jcu_encode_param_t  encode;
    size_t              encoded_size;

    /* Variable: a_QuantizationTable_Y
    *    Quality is IJG75 calculated by "QuantizationTable_Generator.xls" */
    static const uint8_t  a_QuantizationTable_Y[] =
    {
        8, 6, 5, 8, 12, 20, 26, 31,
        6, 6, 7, 10, 13, 29, 30, 28,
        7, 7, 8, 12, 20, 29, 35, 28,
        7, 9, 11, 15, 26, 44, 40, 31,
        9, 11, 19, 28, 34, 55, 52, 39,
        12, 18, 28, 32, 41, 52, 57, 46,
        25, 32, 39, 44, 52, 61, 60, 51,
        36, 46, 48, 49, 56, 50, 52, 50
    };

    /* Variable: a_QuantizationTable_C
    *    Quality is IJG75 calculated by "QuantizationTable_Generator.xls" */
    static const uint8_t  a_QuantizationTable_C[] =
    {
        9, 9, 12, 24, 50, 50, 50, 50,
        9, 11, 13, 33, 50, 50, 50, 50,
        12, 13, 28, 50, 50, 50, 50, 50,
        24, 33, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50,
        50, 50, 50, 50, 50, 50, 50, 50
    };

    /* Variable: a_HuffmanTable_Y_DC
    *    Example written in ITU-T T81 specification */
    static const uint8_t  a_HuffmanTable_Y_DC[] =
    {
        0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
    };


    /* Variable: a_HuffmanTable_C_DC
    *    Example written in ITU-T T81 specification */
    static const uint8_t  a_HuffmanTable_C_DC[] =
    {
        0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B
    };


    /* Variable: a_HuffmanTable_Y_AC
    *    Example written in ITU-T T81 specification */
    static const uint8_t  a_HuffmanTable_Y_AC[] =
    {
        0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
        0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
        0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
        0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    };


    /* Variable: a_HuffmanTable_C_AC
    *    Example written in ITU-T T81 specification */
    static const uint8_t  a_HuffmanTable_C_AC[] =
    {
        0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
        0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
        0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
        0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
        0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
        0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
        0xF9, 0xFA
    };


    /* (variables) = ... */
    {
        /* physical_address_of_RAW = ... */
        e= R_MEMORY_ToPhysicalAddress( gs_memory_address_of_RAW,   &physical_address_of_RAW );
        if(e)
        {
            goto fin;
        }

        /* physical_address_of_JPEG = ... */
        e= R_MEMORY_ToPhysicalAddress( gs_memory_address_of_JPEG,  &physical_address_of_JPEG );
        if(e)
        {
            goto fin;
        }

        /* encoded_event = ... */
        s= R_OS_SemaphoreCreate( &encoded_event,  1 );
        if(!s)
        {
            e=__LINE__;
            goto fin;
        }
    }


    /* Clear JPEG output memory in physical memory */
    memset( gs_memory_address_of_JPEG,  0,  gs_memory_size_of_JPEG );

    R_CACHE_L1DataCleanInvalidAll();


    /* R_JCU_Initialize() */
    e= R_JCU_Initialize( NULL );
    if(e)
    {
        goto fin;
    }
    is_JCU_initialized = true;

    e= R_JCU_SelectCodec( JCU_ENCODE );
    if(e)
    {
        goto fin;
    }


    /* R_JCU_SetQuantizationTable(),  R_JCU_SetHuffmanTable() */
    {
        e= R_JCU_SetQuantizationTable( JCU_TABLE_NO_0, a_QuantizationTable_Y );
        if(e)
        {
            goto fin;
        }
        e= R_JCU_SetQuantizationTable( JCU_TABLE_NO_1, a_QuantizationTable_C );
        if(e)
        {
            goto fin;
        }
        e= R_JCU_SetHuffmanTable( JCU_TABLE_NO_0, JCU_HUFFMAN_DC, a_HuffmanTable_Y_DC );
        if(e)
        {
            goto fin;
        }
        e= R_JCU_SetHuffmanTable( JCU_TABLE_NO_1, JCU_HUFFMAN_DC, a_HuffmanTable_C_DC );
        if(e)
        {
            goto fin;
        }
        e= R_JCU_SetHuffmanTable( JCU_TABLE_NO_0, JCU_HUFFMAN_AC, a_HuffmanTable_Y_AC );
        if(e)
        {
            goto fin;
        }
        e= R_JCU_SetHuffmanTable( JCU_TABLE_NO_1, JCU_HUFFMAN_AC, a_HuffmanTable_C_AC );
        if(e)
        {
            goto fin;
        }
    }


    /* R_JCU_SetEncodeParam() */
    {
        encode.width  = 800u;
        encode.height = 480u;
        buffer.source.swapSetting       = JCU_SWAP_CB_Y0_CR_Y1_PIXEL0123;

        /* Cast to an appropriate type */
        buffer.source.address           = (uint32_t*) physical_address_of_RAW;

        /* Cast to an appropriate type */
        buffer.destination.address      = (uint32_t*) physical_address_of_JPEG;

        /* Cast to an appropriate type */
        buffer.destination.swapSetting  = JCU_SWAP_JPEG_NONE;

        /* Cast to an appropriate type */
        buffer.lineOffset               = (int16_t) GS_FRAME_WIDTH;

        /* Cast to an appropriate type */
        encode.encodeFormat = (jcu_jpeg_format_t) JCU_JPEG_YCbCr422;
        encode.QuantizationTable[ JCU_ELEMENT_Y  ] = JCU_TABLE_NO_0;
        encode.QuantizationTable[ JCU_ELEMENT_Cb ] = JCU_TABLE_NO_1;
        encode.QuantizationTable[ JCU_ELEMENT_Cr ] = JCU_TABLE_NO_1;
        encode.HuffmanTable[ JCU_ELEMENT_Y  ] = JCU_TABLE_NO_0;
        encode.HuffmanTable[ JCU_ELEMENT_Cb ] = JCU_TABLE_NO_1;
        encode.HuffmanTable[ JCU_ELEMENT_Cr ] = JCU_TABLE_NO_1;
        encode.DRI_value = 0;
        encode.inputCbCrOffset = JCU_CBCR_OFFSET_128;

        e= R_JCU_SetEncodeParam( &encode, &buffer );
        if(e)
        {
            goto fin;
        }
    }

    printf( "Encoding:"GS_N );

    /* Cast to an appropriate type */
    printf( "    gs_memory_address_of_RAW = 0x%08X"GS_N,  (uintptr_t) gs_memory_address_of_RAW );
    printf( "    physical_address_of_RAW  = 0x%08X"GS_N,  physical_address_of_RAW );

    /* Cast to an appropriate type */
    printf( "    gs_memory_address_of_JPEG = 0x%08X"GS_N,  (uintptr_t) gs_memory_address_of_JPEG );
    printf( "    physical_address_of_JPEG  = 0x%08X"GS_N,  physical_address_of_JPEG );


    /* R_JCU_StartAsync() : Encode RAW image to JPEG image */
    s= R_OS_SemaphoreWait( &encoded_event,  0 ); /* Clear. Time out must be ignored. */

    /* Cast to an appropriate type */
    e= R_JCU_StartAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &encoded_event );
    if(e)
    {
        goto fin;
    } /* Set */
    s= R_OS_SemaphoreWait( &encoded_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
    if(!s)
    {
        e=__LINE__;
        goto fin;
    }
    e= R_JCU_GetAsyncError();
    if(e)
    {
        goto fin;
    }


    /* Show encoded JPEG size */
    e= R_JCU_GetEncodedSize( &encoded_size );
    if(e)
    {
        goto fin;
    }
    printf( "    encoded_size = %d = 0x%X"GS_N,  encoded_size,  encoded_size );

    e=0;
fin:
    /* R_JCU_TerminateAsync() */
    if ( is_JCU_initialized )
    {
        errnum_t  ee;
        uint32_t  terminate_event = 0;
        s= R_OS_SemaphoreCreate( &terminate_event,  1 );
        if((!s) && (e==0))
        {
            e=__LINE__;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  0 ); /* Clear. Time out must be ignored. */

        /* Cast to an appropriate type */
        ee = R_JCU_TerminateAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &terminate_event ); /* Set */
        if(e==0)
        {
            e=ee;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
        if(!s)
        {
            e=__LINE__;
        }
        ee= R_JCU_GetAsyncError();
        if(e==0)
        {
            e=ee;
        }
        R_OS_SemaphoreDelete( &terminate_event );
    }

    /* ... */
    R_OS_SemaphoreDelete( &encoded_event );
    R_ERROR_Set( e );
    return  e;
} /* End of function gs_EncodeToJPEG */


