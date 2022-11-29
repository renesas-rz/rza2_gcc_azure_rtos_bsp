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
* File         : jcu_api.c
* Description  : JPEG Codec Unit (JCU) driver API. Main Code for single thread.
* Version      : 1.12
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <string.h>
#include  "r_typedefs.h"
#include  "iodefine.h"
#include  "iobitmask.h"
#include  "r_jcu.h"
#include  "../src/r_jcu_local.h"
#include  "../src/r_jcu_pl.h"
#include  "rz_co.h"


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/***********************************************************************
* Variable: gs_jcu_internal_information
************************************************************************/
static jcu_internal_information_t  gs_jcu_internal_information;


/******************************************************************************
Private global variables and functions
******************************************************************************/
static jcu_errorcode_t  R_JCU_Initialize_ST( jcu_config_t*  in_out_Config );
static jcu_errorcode_t  R_JCU_TerminateAsync_ST( r_co_function_t  in_OnFinalized,
        volatile void*  in_OnFinalizedArgument );
static jcu_errorcode_t  R_JCU_SelectCodec_ST( const jcu_codec_t  in_Codec );
static jcu_errorcode_t  R_JCU_StartAsync_ST( r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument );
static jcu_errorcode_t  R_JCU_GetAsyncError_ST(void);
static jcu_errorcode_t  R_JCU_ContinueAsync_ST( const jcu_continue_type_t  in_Type,
        r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument );
static jcu_errorcode_t  R_JCU_SetDecodeParam_ST( const jcu_decode_param_t*const  in_Decode,
        const jcu_buffer_param_t*const  in_Buffer );
static jcu_errorcode_t  R_JCU_SetPauseForImageInfo_ST( const bool_t  in_IsPause );
static jcu_errorcode_t  R_JCU_GetImageInfo_ST( jcu_image_info_t* const  out_Buffer );
static jcu_errorcode_t  R_JCU_SetErrorFilter_ST( jcu_int_detail_errors_t  in_Filter );
static jcu_errorcode_t  R_JCU_SetQuantizationTable_ST( const jcu_table_no_t  in_TableNum,
        const uint8_t* const  in_Table );
static jcu_errorcode_t  R_JCU_SetHuffmanTable_ST( const jcu_table_no_t  in_TableNum,
        const jcu_huff_t  in_Type,  const uint8_t* const  in_Table );
static jcu_errorcode_t  R_JCU_SetEncodeParam_ST( const jcu_encode_param_t* const  in_Encode,
        const jcu_buffer_param_t* const  in_Buffer );
static jcu_errorcode_t  R_JCU_GetEncodedSize_ST( size_t* const  out_Size );
static errnum_t  R_JCU_OnInterrupted(void); 


/* Section: JCU_methods */
/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_Initialize( jcu_config_t*  in_out_Config ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_Initialize_ST( in_out_Config );
    #else
        return  R_JCU_Initialize_MT( in_out_Config );
    #endif
} /* End of function R_JCU_Initialize */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t R_JCU_TerminateAsync( r_co_function_t  in_OnFinalized,   
        volatile void*  in_OnFinalizedArgument )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_TerminateAsync_ST( in_OnFinalized,  in_OnFinalizedArgument );
    #else
        return  R_JCU_TerminateAsync_MT( in_OnFinalized,  in_OnFinalizedArgument );
    #endif
} /* End of function R_JCU_TerminateAsync */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_SelectCodec( const jcu_codec_t  in_Codec ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SelectCodec_ST( in_Codec );
    #else
        return  R_JCU_SelectCodec_MT( in_Codec );
    #endif
} /* End of function R_JCU_SelectCodec */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_StartAsync( r_co_function_t  in_OnFinished, 
        volatile void*  in_OnFinishedArgument )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_StartAsync_ST( in_OnFinished,  in_OnFinishedArgument );
    #else
        return  R_JCU_StartAsync_MT( in_OnFinished,  in_OnFinishedArgument );
    #endif
} /* End of function R_JCU_StartAsync */


/***********************************************************************
* Function Name: R_JCU_GetAsyncError
*    Returns error raised in asynchronized process
*
* Arguments:
*    None
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
************************************************************************/
jcu_errorcode_t  R_JCU_GetAsyncError() 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_GetAsyncError_ST();
    #else
        return  R_JCU_GetAsyncError_MT();
    #endif
} /* End of function R_JCU_GetAsyncError */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_ContinueAsync( const jcu_continue_type_t  in_Type, 
    r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_ContinueAsync_ST( in_Type,  in_OnFinished,  in_OnFinishedArgument );
    #else
        return  R_JCU_ContinueAsync_MT( in_Type,  in_OnFinished,  in_OnFinishedArgument );
    #endif
} /* End of function R_JCU_ContinueAsync */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t R_JCU_SetDecodeParam( 
    const jcu_decode_param_t*const  in_Decode,
    const jcu_buffer_param_t*const  in_Buffer )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetDecodeParam_ST( in_Decode,  in_Buffer );
    #else
        return  R_JCU_SetDecodeParam_MT( in_Decode,  in_Buffer );
    #endif
} /* End of function R_JCU_SetDecodeParam */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t R_JCU_SetPauseForImageInfo( const bool_t  in_IsPause ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetPauseForImageInfo_ST( in_IsPause );
    #else
        return  R_JCU_SetPauseForImageInfo_MT( in_IsPause );
    #endif
} /* End of function R_JCU_SetPauseForImageInfo */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t R_JCU_GetImageInfo( jcu_image_info_t* const  out_Buffer ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_GetImageInfo_ST( out_Buffer );
    #else
        return  R_JCU_GetImageInfo_MT( out_Buffer );
    #endif
} /* End of function R_JCU_GetImageInfo */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_SetErrorFilter( jcu_int_detail_errors_t  in_Filter ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetErrorFilter_ST( in_Filter );
    #else
        return  R_JCU_SetErrorFilter_MT( in_Filter );
    #endif
} /* End of function R_JCU_SetErrorFilter */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_SetQuantizationTable( 
    const jcu_table_no_t  in_TableNum,  const uint8_t* const  in_Table )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetQuantizationTable_ST( in_TableNum,  in_Table );
    #else
        return  R_JCU_SetQuantizationTable_MT( in_TableNum,  in_Table );
    #endif
} /* End of function R_JCU_SetQuantizationTable */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_SetHuffmanTable( const jcu_table_no_t  in_TableNum,   
        const jcu_huff_t  in_Type, const uint8_t* const  in_Table )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetHuffmanTable_ST( in_TableNum,  in_Type,  in_Table );
    #else
        return  R_JCU_SetHuffmanTable_MT( in_TableNum,  in_Type,  in_Table );
    #endif
} /* End of function R_JCU_SetHuffmanTable */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_SetEncodeParam 
        ( const jcu_encode_param_t* const  in_Encode, const jcu_buffer_param_t* const  in_Buffer )
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_SetEncodeParam_ST( in_Encode,  in_Buffer );
    #else
        return  R_JCU_SetEncodeParam_MT( in_Encode,  in_Buffer );
    #endif
} /* End of function R_JCU_SetEncodeParam */


/***********************************************************************
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
************************************************************************/
jcu_errorcode_t  R_JCU_GetEncodedSize( size_t* const  out_Size ) 
{
    #if  ! JCU_MULTI_THREAD
        return  R_JCU_GetEncodedSize_ST( out_Size );
    #else
        return  R_JCU_GetEncodedSize_MT( out_Size );
    #endif
} /* End of function R_JCU_GetEncodedSize */


/***********************************************************************
* Implement: R_JCU_Initialize_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */

/******************************************************************************
 * Function Name: R_JCU_Initialize_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_Initialize_ST( jcu_config_t*  in_out_Config ) 

/* <-SEC M1.1.1 */
{
    errnum_t          e;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 

    /* Cast to an appropriate type */
    R_UNREFERENCED_VARIABLE( in_out_Config );

    if (!( self->AsyncStatus.Status == JCU_STATUS_UNDEF ))
    {
        e = JCU_ERROR_STATUS_1;
        goto fin;  /* Error: Already initialized. */
    }

    /* Initialize the variable */
    self->AsyncStatus.OnInterrupted = (r_co_function_t) R_JCU_OnInterrupted;

    /* Cast to an appropriate type */
    self->AsyncStatus.OnInterruptedArgument = NULL;

    /* Cast to an appropriate type */
    self->AsyncStatus.OnFinished = NULL;

    /* Cast to an appropriate type */
    self->AsyncStatus.OnFinishedArgument = NULL;

    /* Cast to an appropriate type */
    self->AsyncStatus.OnFinalized = NULL;

    /* Cast to an appropriate type */
    self->AsyncStatus.OnFinalizedArgument = NULL;
    self->AsyncStatus.InterruptFlags = 0u;
    self->AsyncStatus.CancelFlags = 0u;
    self->AsyncStatus.IsPaused = false;
    self->AsyncStatus.SubStatusFlags = 0u;
    self->IsCountMode = false;
    self->ErrorFilter = JCU_INT_ERROR_ALL;

    /* User defined initialize */
    e= R_JCU_OnInitialize();
    if(e)
    {
        goto fin;
    }

    /* Set the register */
    e= JCU_SetRegisterForInitialize();
    if(e)
    {
        goto fin;
    }

    /* Set the updated status */
    self->AsyncStatus.Status = JCU_STATUS_INIT;

    /* Initialize the status for the encode */
    self->Codec = JCU_CODEC_NOT_SELECTED;

    e = 0;
fin:
    if ( e )
    {
        /* Rollback */
        bool_t  is_event = false;

        /* Cast to an appropriate type */
        R_JCU_TerminateAsync( (r_co_function_t) R_CO_SetTrue,  &is_event );
        while ( ! is_event )
            { /* Pooling */ ;}
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_Initialize_ST */


/***********************************************************************
* Implement: R_JCU_TerminateAsync_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_TerminateAsync_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_TerminateAsync_ST( 
    r_co_function_t  in_OnFinalized,  volatile void*  in_OnFinalizedArgument )

/* <-SEC M1.1.1 */
{
    errnum_t  e;
    bool_t    was_enabled = false;
    bool_t    is_callback = false;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 
    int_fast32_t  oblivious = 0;

    #if  ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( in_OnFinalized == NULL )
        {
            e = JCU_ERROR_PARAM_1;  goto fin;
        }  /* Error: <Parameter/> */
    #endif

    self->AsyncStatus.ReturnValue = 0;

    /* Guard */
    if ( !(
        (self->AsyncStatus.OnFinalizedArgument != self->AsyncStatus.OnFinishedArgument)  ||
        (self->AsyncStatus.OnFinalized != self->AsyncStatus.OnFinished)  ||

        /* Cast to an appropriate type */
        (self->AsyncStatus.OnFinalizedArgument == NULL) ))
    {
        e=JCU_ERROR_PARAM_36; goto fin;

        /* Error: Arguments of "R_JCU_TerminateAsync" and "R_JCU_StartAsync" are same synchronized objects. */
    }

    /* R_JCU_DisableInterrupt */
    was_enabled = R_JCU_DisableInterrupt();

    /* Check the status */
    if ( self->AsyncStatus.Status == JCU_STATUS_UNDEF )
    {
        in_OnFinalized( in_OnFinalizedArgument );
    }
    else if ( self->AsyncStatus.Status == JCU_STATUS_RUN )
    {
        self->AsyncStatus.OnFinalized = in_OnFinalized;
        self->AsyncStatus.OnFinalizedArgument = in_OnFinalizedArgument;

        self->AsyncStatus.CancelFlags |= JCU_FINALIZE_REQUEST;

        /* "R_JCU_OnInterrupted" will be called */
    }
    else
    {

        /* R_JCU_FinalizeStep2_Sub */
        e= R_JCU_FinalizeStep2_Sub();
        is_callback = true;
    }

    if ( oblivious != 0 )
    {
        e=E_JCU_OBLIVIOUS_3; goto fin;
    }  /* Error: */
    e=0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );
    if ( is_callback )
        { in_OnFinalized( in_OnFinalizedArgument ); }

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_TerminateAsync_ST */


/***********************************************************************
* Function Name: R_JCU_FinalizeStep2_Sub
*    Terminate the driver function (for internal)
*
* Arguments:
*    None
*
* Return Value:
*    <jcu_errorcode_t> type.
************************************************************************/
jcu_errorcode_t  R_JCU_FinalizeStep2_Sub(void) 
{
    errnum_t  e;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    /* Set the updated status */
    self->AsyncStatus.CancelFlags = 0;
    self->AsyncStatus.Status = JCU_STATUS_UNDEF;


    /* Run the additional function selected by parameter */
    e= R_JCU_OnFinalize();
    if(e)
    {
        goto fin;
    }

    e=0;
fin:
    {
        R_ERROR_Set(e);
    }

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_FinalizeStep2_Sub */


/***********************************************************************
* Implement: R_JCU_SelectCodec_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_SelectCodec_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
jcu_errorcode_t R_JCU_SelectCodec_ST( const jcu_codec_t codec )   

/* <-SEC M1.1.1 */
{
    errnum_t          e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_INIT)  ||
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_2;
            goto fin;
        }  /* Error: <State/> */

        e= JCU_ParaCheckSelectCodec(codec);
        if(e)
        {
            goto fin;
        }
    #endif

    /* Clear the count mode flag */
    self->IsCountMode = false;

    /* Register set */
    JCU_SetRegisterForCodec(codec);
    if ( codec == JCU_DECODE )
    {
        JCU_SetRegisterForSetPause( false,  self->ErrorFilter );  /* Clear previous decoding setting */
    }

    /* Set the updated status */
    self->AsyncStatus.Status = JCU_STATUS_SELECTED;
    self->AsyncStatus.IsPaused = false;

    /* Set codec type to internal information variable */
    switch (codec)
    {
        case JCU_ENCODE:
            self->Codec = JCU_STATUS_ENCODE;
        break;
        case JCU_DECODE:
            self->Codec = JCU_STATUS_DECODE;
        break;

    /* ->QAC 2018 : For MISRA 15.3, SEC R3.5.2 */
        default:

    /* <-QAC 2018 */
    /* NOT REACHED */
        break;
    } /* end switch */

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SelectCodec_ST */


/***********************************************************************
* Implement: R_JCU_StartAsync_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_StartAsync_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_StartAsync_ST( r_co_function_t  in_OnFinished, 
        volatile void*  in_OnFinishedArgument )

/* <-SEC M1.1.1 */
{
    errnum_t          e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!( self->AsyncStatus.Status == JCU_STATUS_READY ))
            { e = JCU_ERROR_STATUS_3;  goto fin; }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_OnFinished != NULL ))
            { e = JCU_ERROR_PARAM_2;  goto fin; }  /* Error: <Parameter/> */

        e= JCU_ParaCheckStart();
        if(e)
        {
            goto fin;
        }
    #endif

    self->AsyncStatus.OnFinished = in_OnFinished;
    self->AsyncStatus.OnFinishedArgument = in_OnFinishedArgument;
    self->AsyncStatus.ReturnValue = 0;

    self->AsyncStatus.Status = JCU_STATUS_RUN;
    JCU_SetRegisterForStart( self->ErrorFilter );

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_StartAsync_ST */


/***********************************************************************
* Implement: R_JCU_GetAsyncError_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_GetAsyncError_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_GetAsyncError_ST() 

/* <-SEC M1.1.1 */
{
    errnum_t  e;

    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 

    e= self->AsyncStatus.ReturnValue;
    if(e)
    {
        goto fin;
    }

    e = 0;
fin:
    {
        R_ERROR_Set(e);
    }

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_GetAsyncError_ST */


/***********************************************************************
* Implement: R_JCU_ContinueAsync_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_ContinueAsync_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_ContinueAsync_ST 
    ( const jcu_continue_type_t  in_Type, r_co_function_t  in_OnFinished,  volatile void*  in_OnFinishedArgument )

/* <-SEC M1.1.1 */
{
    errnum_t          e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    enum {            num_8_bit = 8 }; 
    bit_flags32_t     mask;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!( self->AsyncStatus.Status == JCU_STATUS_READY ))
            { e = JCU_ERROR_STATUS_4; goto fin; }  /* Error: <State/> */

        if (!( self->AsyncStatus.IsPaused == true ))
            { e = JCU_ERROR_STATUS_5; goto fin; }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_OnFinished != NULL ))
            { e = JCU_ERROR_PARAM_3; goto fin; }  /* Error: <Parameter/> */
    #endif

    /* Clear the sub status flag */
    switch ( in_Type )
    {
        case JCU_INPUT_BUFFER:

            /* Cast to an appropriate type */
            mask = ( ( JCU_JINTS1_JINF | JCU_JINTS1_DINLF ) << ( (sizeof( JCU.JINTS0 )) * num_8_bit ) );
            break;
        case JCU_OUTPUT_BUFFER:

            /* Cast to an appropriate type */
            mask = ( ( JCU_JINTS1_DOUTLF | JCU_JINTS1_JOUTF ) << ( (sizeof( JCU.JINTS0 )) * num_8_bit ) );
            break;
        default:
            if (!( in_Type == JCU_IMAGE_INFO ))
            {
                e=JCU_ERROR_PARAM_4; goto fin;
            }  /* Error: <Parameter/> */

            /* Cast to an appropriate type */
            mask = JCU_JINTS0_INS3;
            break;
    }
    if (!( IS_ANY_BITS_SET( self->AsyncStatus.SubStatusFlags, mask ) ))
    {
        e=JCU_ERROR_PARAM_5;
        goto fin;
    }  /* Error: <Parameter/> */
    self->AsyncStatus.SubStatusFlags &= (~mask);
    self->AsyncStatus.ReturnValue = 0;

    /* JCU will restart */
    if ( self->AsyncStatus.SubStatusFlags == 0 )
    {
        /* Change the status */
        self->AsyncStatus.Status = JCU_STATUS_RUN;
        self->AsyncStatus.IsPaused = false;

        /* Register set */
        JCU_SetRegisterForContinue( in_Type );
    }

    /* JCU will not restart */
    else
    {
        /* Register set */
        JCU_SetRegisterForContinue( in_Type );

        /* Change the status */
        self->AsyncStatus.Status = JCU_STATUS_READY;

        /* Set event flags */
        in_OnFinished( in_OnFinishedArgument );
    }

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_ContinueAsync_ST */


/***********************************************************************
* Implement: R_JCU_SetDecodeParam_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_SetDecodeParam_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_SetDecodeParam_ST( 
    const jcu_decode_param_t*const  in_Decode,
    const jcu_buffer_param_t*const  in_Buffer )

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_6;
            goto fin;
        }  /* Error: <State/> */
        if (!(
            self->Codec == JCU_STATUS_DECODE ))
        {
            e = JCU_ERROR_STATUS_7;
            goto fin;
        }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_Decode != NULL ))
        {
            e = JCU_ERROR_PARAM_6;
            goto fin;
        }  /* Error: <Parameter/> */

        /* Cast to an appropriate type */
        if (! (in_Buffer != NULL ))
        {
            e = JCU_ERROR_PARAM_7;
            goto fin;
        }  /* Error: <Parameter/> */

        e = JCU_ParaCheckSetDecodeParam(in_Decode, in_Buffer);
        if(e)
        {
            goto fin;
        }
    #endif /* ! defined( JCU_RELEASE ) */

    /* Register set */
    JCU_SetRegisterForSetDecodePrm(in_Decode, in_Buffer);

    /* Set the updated status */
    self->AsyncStatus.Status = JCU_STATUS_READY;

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetDecodeParam_ST */


/***********************************************************************
* Implement: R_JCU_SetPauseForImageInfo_ST
************************************************************************/
/******************************************************************************
 * Function Name: R_JCU_SetPauseForImageInfo_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t R_JCU_SetPauseForImageInfo_ST 
    ( const bool_t  in_IsPause )
{
    jcu_errorcode_t   e;
    bool_t            was_enabled = false;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_16;
            goto fin;
        }  /* Error: Not called "R_JCU_SelectCodec" yet */

        /* Writing to the register is not effective before reset by "R_JCU_SelectCodec" */
    #endif


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_8;
            goto fin;
        }  /* Error: <State/> */
        if (!(
            self->Codec == JCU_STATUS_DECODE ))
        {
            e = JCU_ERROR_STATUS_9;
            goto fin;
        }  /* Error: <State/> */
    #endif

    /* Register set */
    JCU_SetRegisterForSetPause( in_IsPause, self->ErrorFilter );

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetPauseForImageInfo_ST */


/***********************************************************************
* Implement: R_JCU_GetImageInfo_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_GetImageInfo_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
jcu_errorcode_t R_JCU_GetImageInfo_ST   
    ( jcu_image_info_t* const  out_Buffer )

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    jcu_internal_information_t*
            const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!( self->AsyncStatus.Status == JCU_STATUS_READY ))
        {
            e = JCU_ERROR_STATUS_A;
            goto fin;
        }  /* Error: <State/> */

        if (!( self->Codec == JCU_STATUS_DECODE ))
        {
            e = JCU_ERROR_STATUS_B;
            goto fin;
        }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( out_Buffer != NULL ))
        {
            e = JCU_ERROR_PARAM_8;
            goto fin;
        }  /* Error: <Parameter/> */

        e = JCU_ParaCheckGetImageInfo( out_Buffer );
        if(e)
        {
            goto fin;
        }
    #endif

    /* Register set */
    JCU_GetRegisterForGetImageInfo( out_Buffer );

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_GetImageInfo_ST */


/***********************************************************************
* Implement: R_JCU_SetErrorFilter_ST
************************************************************************/
/******************************************************************************
 * Function Name: R_JCU_SetErrorFilter_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_SetErrorFilter_ST 
    ( jcu_int_detail_errors_t  in_Filter )
{
    jcu_errorcode_t  e;

    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_INIT)  ||
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_C;
            goto fin;
        }  /* Error: <State/> */
    #endif

    /* Cast to an appropriate type */
    if ( IS_ANY_BITS_SET( in_Filter, (bit_flags_fast32_t) ~JCU_INT_ERROR_ALL ) )
    {
        e = JCU_ERROR_PARAM_9;
        goto fin;
    }  /* Error: <Parameter/> */

    self->ErrorFilter = in_Filter;

    e = 0;
fin:
    {
        R_ERROR_Set(e);
    }

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetErrorFilter_ST */


/***********************************************************************
* Implement: R_JCU_SetQuantizationTable_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_SetQuantizationTable_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_SetQuantizationTable_ST( 
    const jcu_table_no_t  in_TableNum,  const uint8_t* const  in_Table )

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;
    bool_t            was_enabled = false;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 

    #if  ! defined( JCU_RELEASE )
        e = JCU_ParaCheckSetQuantizationTbl( in_TableNum );
            if (e != JCU_ERROR_OK)
            {
                goto fin;
            }
    #endif

    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_D;
            goto fin;
        }  /* Error: <State/> */
        if (!(
            self->Codec == JCU_STATUS_ENCODE ))
        {
            e = JCU_ERROR_STATUS_E;
            goto fin;
        }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_Table != NULL ))
        {
            e = JCU_ERROR_PARAM_A;
            goto fin;
        }  /* Error: <Parameter/> */
    #endif

    /* Register set */
    JCU_SetRegisterForSetQtTable( in_TableNum,  in_Table );

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetQuantizationTable_ST */


/***********************************************************************
* Implement: R_JCU_SetHuffmanTable_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_SetHuffmanTable_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_SetHuffmanTable_ST 
    ( const jcu_table_no_t  in_TableNum,  const jcu_huff_t  in_Type, const uint8_t* const  in_Table )

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;
    bool_t            was_enabled = false;
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 

    #if  ! defined( JCU_RELEASE )
        e= JCU_ParaCheckSetHuffmanTable( in_TableNum,  in_Type );
        if(e)
        {
            goto fin;
        }
    #endif


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_F;
            goto fin;
        }  /* Error: <State/> */
        if (!(
            self->Codec == JCU_STATUS_ENCODE ))
        {
            e = JCU_ERROR_STATUS_10;
            goto fin;
        }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_Table != NULL ))
        {
            e = JCU_ERROR_PARAM_B;
            goto fin;
        }  /* Error: <Parameter/> */
    #endif


    /* Register set */
    JCU_SetRegisterForSetHuffTbl( in_TableNum,  in_Type,  in_Table );

    e = JCU_ERROR_OK;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetHuffmanTable_ST */


/***********************************************************************
* Implement: R_JCU_SetEncodeParam_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_SetEncodeParam_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_SetEncodeParam_ST 
    ( const jcu_encode_param_t* const  in_Encode, const jcu_buffer_param_t* const  in_Buffer )

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;
    bool_t            was_enabled; /* = false; */ /* QAC 3197 */
    jcu_internal_information_t*
        const  self = &gs_jcu_internal_information; 


    was_enabled = R_JCU_DisableInterrupt();

    #if  ! defined( JCU_RELEASE )
        if (!(
            (self->AsyncStatus.Status == JCU_STATUS_SELECTED)  ||
            (self->AsyncStatus.Status == JCU_STATUS_READY) ))
        {
            e = JCU_ERROR_STATUS_11;
            goto fin;
        }  /* Error: <State/> */
        if (!(
            self->Codec == JCU_STATUS_ENCODE ))
        {
            e = JCU_ERROR_STATUS_12;
            goto fin;
        }  /* Error: <State/> */

        /* Cast to an appropriate type */
        if (!( in_Encode != NULL ))
        {
            e = JCU_ERROR_PARAM_C;
            goto fin;
        }  /* Error: <Parameter/> */

        /* Cast to an appropriate type */
        if (!( in_Buffer != NULL ))
        {
            e = JCU_ERROR_PARAM_D;
            goto fin;
        }  /* Error: <Parameter/> */

        e= JCU_ParaCheckEncodeParam( in_Encode,  in_Buffer );
        if(e)
        {
            goto fin;
        }
    #endif /* ! defined( JCU_RELEASE ) */

    /* Register set */
    JCU_SetRegisterForSetEncodePrm( in_Encode,  in_Buffer );

    /* Set the updated status */
    self->AsyncStatus.Status = JCU_STATUS_READY;

    e = 0;
fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }
    R_ERROR_Set( e );

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_SetEncodeParam_ST */


/***********************************************************************
* Implement: R_JCU_GetEncodedSize_ST
************************************************************************/
/* ->SEC M1.1.1 API Function */
/******************************************************************************
 * Function Name: R_JCU_GetEncodedSize_ST
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
static jcu_errorcode_t  R_JCU_GetEncodedSize_ST( size_t* const  out_Size ) 

/* <-SEC M1.1.1 */
{
    jcu_errorcode_t   e;

    #if  ! defined( JCU_RELEASE )
    {
        jcu_internal_information_t*
            const  self = &gs_jcu_internal_information; 

        if (!( self->AsyncStatus.Status == JCU_STATUS_READY ))
        {
            e = JCU_ERROR_STATUS_13;
            goto fin;
        }  /* Error: <State/> */
    }
    #endif


    JCU_GetEncodedSize(out_Size);


    e = 0;
#if  ! defined( JCU_RELEASE )
fin:
#endif
    {
        R_ERROR_Set(e);
    }

    /* Cast to an appropriate type */
    return  (jcu_errorcode_t) e;
} /* End of function R_JCU_GetEncodedSize_ST */


/***********************************************************************
* Function Name: R_JCU_GetInternalInformation
*    R_JCU_GetInternalInformation (for internal)
*
* Arguments:
*    None
*
* Return Value:
*    <jcu_internal_information_t> type.
************************************************************************/
jcu_internal_information_t*  R_JCU_GetInternalInformation(void) 
{
    return  &gs_jcu_internal_information;
} /* End of function R_JCU_GetInternalInformation */


/***********************************************************************
* Function Name: R_JCU_OnInterrupting
*    Inerrupt service routine (ISR)
*
* Return Value:
*    Error code. 0=No error. <jcu_errorcode_t> type.
*
* Description:
*    All JCU interrupt callback functions registered by R_JCU_OnInitialize
*    function must call this ISR.
************************************************************************/
errnum_t  R_JCU_OnInterrupting() 
{
    errnum_t  e;

    jcu_internal_information_t*
        const  self = R_JCU_GetInternalInformation(); 

    #if  ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( self == NULL )
        {
            e=JCU_ERROR_PARAM_26; goto fin;
        } /* Error: <Parameter/> */
    #endif

    e=0;

    self->AsyncStatus.Status = JCU_STATUS_INTERRUPTING;

    JCU_OnInterrupting( &self->AsyncStatus.InterruptFlags );


    /* Set state */
    self->AsyncStatus.Status = JCU_STATUS_INTERRUPTED;

    /* Call callback function */
    self->AsyncStatus.OnInterrupted( self->AsyncStatus.OnInterruptedArgument );

fin:
    {
        return e;
    }
} /* End of function R_JCU_OnInterrupting */


/***********************************************************************
* Function Name: R_JCU_OnInterrupted
*    Bottom half operations on interrupting (for internal)
*
* Return Value:
*    Error code, 0=No error
************************************************************************/
static errnum_t  R_JCU_OnInterrupted(void) 
{
    enum { num_8_bit = 8 }; 
    bit_flags32_t const  sub_status_bits = 
        ( JCU_JINTS0_INS3 ) |                                           /* Cast to an appropriate type */
        ( JCU_JINTS1_JINF   << ((sizeof( JCU.JINTS0 )) * num_8_bit) ) | /* Cast to an appropriate type */
        ( JCU_JINTS1_DOUTLF << ((sizeof( JCU.JINTS0 )) * num_8_bit) ) | /* Cast to an appropriate type */
        ( JCU_JINTS1_DINLF  << ((sizeof( JCU.JINTS0 )) * num_8_bit) ) | /* Cast to an appropriate type */
        ( JCU_JINTS1_JOUTF  << ((sizeof( JCU.JINTS0 )) * num_8_bit) );  /* Cast to an appropriate type */

    errnum_t  e;
    errnum_t  ee;
    uint32_t  interrupt_flags;
    bool_t    was_enabled = false;
    bool_t    is_end;
    bool_t    or; 
    bool_t    is_callback_1 = false;
    bool_t    is_callback_2 = false;

    jcu_internal_information_t*
        const  self = R_JCU_GetInternalInformation(); 

    #if  ! defined( JCU_RELEASE )

        /* Cast to an appropriate type */
        if ( self == NULL )
        {
            e=JCU_ERROR_PARAM_27; goto fin;
        } /* Error: <Parameter/> */
    #endif

    e=0;

    was_enabled = R_JCU_DisableInterrupt();

    if (!( self->AsyncStatus.Status != JCU_STATUS_UNDEF ))
    {
        e=E_STATE; goto fin;
    }  /* Error: <State/> */

    {
        interrupt_flags = self->AsyncStatus.InterruptFlags;
        self->AsyncStatus.InterruptFlags = 0u;
    } /* This code must do in I-Lock area. */

    /* Cast to an appropriate type */
    if ( IS_ANY_BITS_SET( interrupt_flags,
            ( JCU_JINTS1_DBTF | JCU_JINTS1_CBTF ) << ((sizeof( JCU.JINTS0 )) * num_8_bit) ) )
    {
        self->AsyncStatus.IsPaused = false;
        self->AsyncStatus.SubStatusFlags = 0u;
        is_end = true;
    }
    else if ( IS_ANY_BITS_SET( interrupt_flags,  sub_status_bits ) )
    {
        self->AsyncStatus.IsPaused = true;
        self->AsyncStatus.SubStatusFlags = interrupt_flags & sub_status_bits;
        is_end = false;
    }
    else
    {
        is_end = false;
    }


    or = ( (is_end) || (self->AsyncStatus.IsPaused) );
    if (!or)
    {
        or = IS_BIT_SET( interrupt_flags,  JCU_JINTS0_INS5 );
    }  /* or = for MISRA 12.4 */
    if ( or )
    {
        /* Set "AsyncStatus" */
        self->AsyncStatus.Status = JCU_STATUS_READY;
        is_callback_1 = true;;
    }
    else
    {
        self->AsyncStatus.Status = JCU_STATUS_RUN;
    }


    if ( IS_BIT_SET( interrupt_flags,  JCU_JINTS0_INS5 ) )  /* Check input data and try to flush the cache. */
    {
        jcu_detail_error_t  ed;

        JCU_GetRegisterForGetErrorInfo( &ed );

        /* Cast to an appropriate type */
        e = (errnum_t) ed;
        goto fin;
    }


    if ( is_callback_1 )
    {
        if ( IS_BIT_SET( self->AsyncStatus.CancelFlags,  JCU_FINALIZE_REQUEST ) )
        {
            is_callback_2 = true;
            ee= R_JCU_FinalizeStep2_Sub();
            if(e==0)
            {
                e=ee;
            }
        }
    }

fin:
    if ( was_enabled )
    {
        R_JCU_EnableInterrupt();
    }

    self->AsyncStatus.ReturnValue = e;
    R_ERROR_Set( e );

    if ( is_callback_2 )
    {
        self->AsyncStatus.OnFinalized( self->AsyncStatus.OnFinalizedArgument );
    }
    if ( is_callback_1 )
    {
        self->AsyncStatus.OnFinished( self->AsyncStatus.OnFinishedArgument );
    }

    return  e;
} /* End of function R_JCU_OnInterrupted */

