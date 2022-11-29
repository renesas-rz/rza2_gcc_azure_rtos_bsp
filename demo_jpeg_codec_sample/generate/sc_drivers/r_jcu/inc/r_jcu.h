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
* File         : r_jcu.h
* Description  : JPEG Codec Unit (JCU) driver's API. Main Header.
* Version      : 1.12
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stddef.h>
#include  "r_typedefs.h"
#include  "r_jcu_typedef.h"
#include  "rz_co_typedef.h"

#ifndef  R_JCU_API_H
#define  R_JCU_API_H

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
Struct & Enum definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

/******************************************************************************
* Function Name: R_JCU_Initialize
*    Initializes the JCU driver
*
* Arguments:
*    in_out_Config - NULL
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    The state will be in the initialized status.
*    Initializes the internal status(gs_jcu_internal_information).
*    The user defined function(R_JCU_OnInitialize) is called.
*    Perform the following processing in the user defined function.
*
*    - Clock supply to JCU.
*    - Sets the priority of interrupt.
*    - Sets the environment-depend process.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_Initialize( jcu_config_t*  in_out_Config ); 

#ifdef  JCU_CALLBACK_ARGUMENT
/******************************************************************************
* Function Name: R_JCU_TerminateAsync
*    Performs termination processing for the JCU driver
*
* Arguments:
*    in_OnFinalized - Callback function called when interrupt was signaled.
*                     This function will be called from interrupt or thread.
*                     If any error was raised, this function will be not called.
*    in_OnFinalizedArgument - Argument of callback function called when interrupt was signaled
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    The processing which finishes a JCU driver.
*    This function is asynchronous function that will return before processing ends.
*    The state will be changed.
*    Perform the following processing in the user defined function.
*
*    - Clock stopped to JCU.
*    - Clear the priority of interrupt.
*    - Sets the environment-depend process.
*
*    <R_JCU_GetAsyncError> must be called after finishing this asynchronous operation.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_TerminateAsync 
                    ( r_co_function_t  in_OnFinalized,  volatile void*  in_OnFinalizedArgument );
#endif

/******************************************************************************
* Function Name: R_JCU_SelectCodec
*    Sets the JCU mode
*
* Arguments:
*    in_Codec - JCU mode(Compression or De-compression)
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    This function selects the JCU mode(Compression or De-compression).
*    All parameters of decode, encode and count mode must be set again.
*    Because when this function was called, these parameters were initialized.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SelectCodec( const jcu_codec_t  in_Codec ); 

#ifdef  JCU_CALLBACK_ARGUMENT
/******************************************************************************
* Function Name: R_JCU_StartAsync
*    Starts JCU process
*
* Arguments:
*    in_OnFinished         - Callback function called after interrupt handling.
*                            This function will be called from interrupt or thread.
*                            If any error was raised, this function will be not called.
*    in_OnFinishedArgument - Argument of callback function called after interrupt handling
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Starts JCU process. The function is asynchronous function that will return before
*    decoding or encoding ends or pauses.
*    Using the <R_JCU_SetDecodeParam> API function or the <R_JCU_SetEncodeParam> API function,
*    set the parameters before the JCU process starts
*    You cannot stop the JCU process, after the JCU process starts.
*    <R_JCU_GetAsyncError> must be called after finishing this asynchronous operation.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_StartAsync
                    ( r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument );

/******************************************************************************
* Function Name: R_JCU_GetAsyncError
*    Returns error raised in asynchronized process
*
* Arguments:
*    None
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_GetAsyncError(void); 

/******************************************************************************
* Function Name: R_JCU_ContinueAsync
*    Resumes the JCU process (asynchronous process)
*
* Arguments:
*    in_Type               - Mode of restarting JCU
*    in_OnFinished         - Callback function called after interrupt handling.
*                            This function will be called from interrupt or thread.
*                            If any error was raised, this function will be not called.
*    in_OnFinishedArgument - Argument of callback function called after interrupt handling
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Processing of JCU which paused is resumed.
*    The function is asynchronous function that will return before decoding or encoding ends or pauses.
*    <R_JCU_GetAsyncError> must be called after finishing this asynchronous operation.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_ContinueAsync
    ( const jcu_continue_type_t  in_Type,  r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument );
#endif /* JCU_CALLBACK_ARGUMENT */

/******************************************************************************
* Function Name: R_JCU_SetDecodeParam
*    Sets decoding parameter
*
* Arguments:
*    in_Decode - Decode parameter
*    in_Buffer - Output buffer
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Sets decoding parameter.
*    If the pixel format isn't ARGB8888, the alpha value must be zero.
*    If the pixel format isn't YCbCr, the Cb/Cr value must be JCU_CBCR_OFFSET_0.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetDecodeParam 
    ( const jcu_decode_param_t*const  in_Decode,  const jcu_buffer_param_t*const  in_Buffer );

/******************************************************************************
* Function Name: R_JCU_SetPauseForImageInfo
*    When the image information can be acquired, it's made the setting which is paused
*
* Arguments:
*    in_IsPause - true: It's made the setting which is paused, false: It's made the setting which isn't paused.
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    When the image information can be acquired,
*    it's made the setting which is paused by the <R_JCU_GetImageInfo> function.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetPauseForImageInfo( const bool_t  in_IsPause );

/******************************************************************************
* Function Name: R_JCU_GetImageInfo
*    Gets information on the JPEG data
*
* Arguments:
*    out_Buffer - Output: image information
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Gets the image information (width, height, pixel format) of the decoded JPEG data.
*    If data is read before the request which reads the image information, the data is not guaranteed.
*    If the pixel format of the decoded JPEG data is outside of the jcu_jpeg_format_t,
*    it's the error, so JCU can't decode.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_GetImageInfo( jcu_image_info_t* const  out_Buffer ); 

/******************************************************************************
* Function Name: R_JCU_SetErrorFilter
*    The particular error code was set to valid
*
* Arguments:
*    in_Filter - The valid decoding error code <jcu_int_detail_error_t> as the bit flag value.
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    The particular error code was set to valid.
*    When the valid decoding error occurred, interrupt occurs.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetErrorFilter( jcu_int_detail_errors_t  in_Filter ); 

/******************************************************************************
* Function Name: R_JCU_SetQuantizationTable
*    Sets the Quantization table
*
* Arguments:
*    tableNo - Quantization table number
*    table   - Quantization table
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Quantization table data.
*    For the setting value of the quantization table data,
*    see "RZ/A2M Group User's Manual: Hardware"
*    section 45.3.1 (4), (a) Quantization Table Specification.
*    Attached "QuantizationTable_Generator.html" file can calculate
*    an example of quantization table.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetQuantizationTable 
    ( const jcu_table_no_t  in_TableNum,  const uint8_t* const  in_Table );

/******************************************************************************
* Function Name: R_JCU_SetHuffmanTable
*    Sets the Huffman table
*
* Arguments:
*    in_TableNum - Huffman table number
*    in_Type     - Type of Huffman table (AC or DC)
*    in_Table    - Huffman table
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Huffman table data.
*    For the setting value of the Huffman table data,
*    see "RZ/A2M Group User's Manual: Hardware"
*    section 45.3.1 (4), (b) Huffman Table Specification.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetHuffmanTable 
    ( const jcu_table_no_t  in_TableNum,  const jcu_huff_t  in_Type,  const uint8_t* const  in_Table );

/******************************************************************************
 * Function Name: R_JCU_SetEncodeParam
*    Sets encoding parameter
*
* Arguments:
*    in_Encode - Encode parameter
*    in_Buffer - Input buffer
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Sets Encoding parameter.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_SetEncodeParam 
    ( const jcu_encode_param_t* const  in_Encode,  const jcu_buffer_param_t* const  in_Buffer );

/******************************************************************************
* Function Name: R_JCU_GetEncodedSize
*    Gets the size of data to be compressed
*
* Arguments:
*    out_Size - Output: Pointer to variable of the data size
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    Gets the size of data to be compressed.
*    If data is read before interrupt of encoding complete, the data is not guaranteed.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_GetEncodedSize( size_t* const  out_Size ); 

/******************************************************************************
* Function Name: R_JCU_OnInterrupting
*    Inerrupt service routine (ISR)
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    All JCU interrupt callback functions registered by R_JCU_OnInitialize
*    function must call this ISR.
 *****************************************************************************/
errnum_t  R_JCU_OnInterrupting(void); 

/******************************************************************************
 * Function Name: R_JCU_PrintRegisters
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
void  R_JCU_PrintRegisters(void); 

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* #define  R_JCU_API_H */
