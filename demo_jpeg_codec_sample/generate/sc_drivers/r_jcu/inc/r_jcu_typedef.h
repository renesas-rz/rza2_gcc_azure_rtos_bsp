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
* File         : r_jcu_typedef.h
* Description  : JPEG Codec Unit (JCU) driver's API. Main Header.
* Version      : 1.12
******************************************************************************/

/***********************************************************************
* Class: JCU_driver
*    The driver of JPEG Codec Unit
*
*    - Methods: <JCU_methods>
************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "rz_co_typedef.h"

#ifndef  R_JCU_API_TYPEDEF_H
#define  R_JCU_API_TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Macro: JCU_RZA2_VERSION
*    JCU version number for RZ/A2M
************************************************************************/
/* Version Number of API */
#define  JCU_RZA2_VERSION_MAJOR (1)
#define  JCU_RZA2_VERSION_MINOR (11)

#define  JCU_RZA2_VERSION  (111)


/***********************************************************************
* Macro: JCU_VERSION
*    JCU version number
************************************************************************/
#define  JCU_VERSION  (311)


/***********************************************************************
* Macro: JCU_VERSION_STRING
*    Character string of the JCU version number. String of <JCU_RZA2_VERSION> .
************************************************************************/
#define  JCU_VERSION_STRING  "1.11"


/***********************************************************************
* Macro: JCU_RELEASE
*    1 or not defined
*
* Description:
*    not defined = enabling checking for debug
************************************************************************/
/* #define  JCU_RELEASE  1 */


/***********************************************************************
* Constant: JCU_CALLBACK_ARGUMENT
*    1. Not define is not supported in this version
************************************************************************/
#define  JCU_CALLBACK_ARGUMENT  (1)


/***********************************************************************
* Constant: JCU_MULTI_THREAD
*    Multi-threading is not supported
************************************************************************/
#define  JCU_MULTI_THREAD  (0)


/***********************************************************************
* Constant: JCU_BUFFER_ALIGNMENT
*    Alignment of start address. It is same as between before and after decoding and encoding
************************************************************************/
#define  JCU_BUFFER_ALIGNMENT  (8)


/***********************************************************************
* Constant: JCU_NUMBER_OF_QUANTIZATION_TABLE_DATA
*    Size of quantization table
************************************************************************/
#define  JCU_NUMBER_OF_QUANTIZATION_TABLE_DATA  (64)


/***********************************************************************
* Constant: JCU_NUMBER_OF_HUFFMAN_TABLE_DATA_DC
*    Size of huffman table DC component
************************************************************************/
#define  JCU_NUMBER_OF_HUFFMAN_TABLE_DATA_DC  (28)


/***********************************************************************
* Constant: JCU_NUMBER_OF_HUFFMAN_TABLE_DATA_AC
*    Size of huffman table AC component
************************************************************************/
#define  JCU_NUMBER_OF_HUFFMAN_TABLE_DATA_AC  (178)


/* Section: JCU_ERROR */
/***********************************************************************
* Constants: JCU_ERROR
*    The kind of error relating to JCU. <jcu_errorcode_t> type
*
* Description:
*    These error codes mean that the error was detected by software.
************************************************************************/
/* Constant: JCU_ERROR_MASK
*    Mask for checking JCU_ERROR. */
#define  JCU_ERROR_OK    (0)  /* Constant: JCU_ERROR_OK */
#define  JCU_ERROR       (0x00450000)
#define  JCU_ERROR_MASK  (0x00FF0000)

/* Sub class of E_OBLIVIOUS */
#define  E_JCU_OBLIVIOUS_1    (0x0B450001)
#define  E_JCU_OBLIVIOUS_2    (0x0B450002)
#define  E_JCU_OBLIVIOUS_3    (0x0B450003)

/* Sub class of E_UNRECOVERABLE */
#define  E_JCU_UNRECOVERABLE_1  (0x0D450001)


/***********************************************************************
* Constant: JCU_ERROR_PARAM
*    Value of type of JCU parameters. <jcu_errorcode_t> type
************************************************************************/
/* Constant: JCU_ERROR_PARAM_MASK
*    Mask for checking JCU_ERROR_PARAM. */
#define  JCU_ERROR_PARAM       (0x01450100)
#define  JCU_ERROR_PARAM_MASK  (0x7FFFFF00)
#define  JCU_ERROR_PARAM_1     (0x01450101)
#define  JCU_ERROR_PARAM_2     (0x01450102)
#define  JCU_ERROR_PARAM_3     (0x01450103)
#define  JCU_ERROR_PARAM_4     (0x01450104)
#define  JCU_ERROR_PARAM_5     (0x01450105)
#define  JCU_ERROR_PARAM_6     (0x01450106)
#define  JCU_ERROR_PARAM_7     (0x01450107)
#define  JCU_ERROR_PARAM_8     (0x01450108)
#define  JCU_ERROR_PARAM_9     (0x01450109)
#define  JCU_ERROR_PARAM_A     (0x0145010A)
#define  JCU_ERROR_PARAM_B     (0x0145010B)
#define  JCU_ERROR_PARAM_C     (0x0145010C)
#define  JCU_ERROR_PARAM_D     (0x0145010D)
#define  JCU_ERROR_PARAM_E     (0x0145010E)
#define  JCU_ERROR_PARAM_F     (0x0145010F)
#define  JCU_ERROR_PARAM_10    (0x01450110)
#define  JCU_ERROR_PARAM_11    (0x01450111)
#define  JCU_ERROR_PARAM_12    (0x01450112)
#define  JCU_ERROR_PARAM_13    (0x01450113)
#define  JCU_ERROR_PARAM_14    (0x01450114)
#define  JCU_ERROR_PARAM_15    (0x01450115)
#define  JCU_ERROR_PARAM_16    (0x01450116)
#define  JCU_ERROR_PARAM_17    (0x01450117)
#define  JCU_ERROR_PARAM_18    (0x01450118)
#define  JCU_ERROR_PARAM_19    (0x01450119)
#define  JCU_ERROR_PARAM_1A    (0x0145011A)
#define  JCU_ERROR_PARAM_1B    (0x0145011B)
#define  JCU_ERROR_PARAM_1C    (0x0145011C)
#define  JCU_ERROR_PARAM_1D    (0x0145011D)
#define  JCU_ERROR_PARAM_1E    (0x0145011E)
#define  JCU_ERROR_PARAM_1F    (0x0145011F)
#define  JCU_ERROR_PARAM_20    (0x01450120)
#define  JCU_ERROR_PARAM_21    (0x01450121)
#define  JCU_ERROR_PARAM_22    (0x01450122)
#define  JCU_ERROR_PARAM_23    (0x01450123)
#define  JCU_ERROR_PARAM_24    (0x01450124)
#define  JCU_ERROR_PARAM_25    (0x01450125)
#define  JCU_ERROR_PARAM_26    (0x01450126)
#define  JCU_ERROR_PARAM_27    (0x01450127)
#define  JCU_ERROR_PARAM_28    (0x01450128)
#define  JCU_ERROR_PARAM_29    (0x01450129)
#define  JCU_ERROR_PARAM_2A    (0x0145012A)
#define  JCU_ERROR_PARAM_2B    (0x0145012B)
#define  JCU_ERROR_PARAM_2C    (0x0145012C)
#define  JCU_ERROR_PARAM_2D    (0x0145012D)
#define  JCU_ERROR_PARAM_2E    (0x0145012E)
#define  JCU_ERROR_PARAM_2F    (0x0145012F)
#define  JCU_ERROR_PARAM_30    (0x01450130)
#define  JCU_ERROR_PARAM_31    (0x01450131)
#define  JCU_ERROR_PARAM_32    (0x01450132)
#define  JCU_ERROR_PARAM_33    (0x01450133)
#define  JCU_ERROR_PARAM_34    (0x01450134)
#define  JCU_ERROR_PARAM_35    (0x01450135)
#define  JCU_ERROR_PARAM_36    (0x01450136)

/* For porting layer */
#define  JCU_ERROR_PARAM_61    (0x01450161)
#define  JCU_ERROR_PARAM_62    (0x01450162)
#define  JCU_ERROR_PARAM_63    (0x01450163)
#define  JCU_ERROR_PARAM_64    (0x01450164)


/***********************************************************************
* Constant: JCU_ERROR_STATUS
*    Value of type of status error. <jcu_errorcode_t> type
************************************************************************/
/* Constant: JCU_ERROR_STATUS_MASK
*    Mask for checking JCU_ERROR_STATUS. */
/* Sub class of E_STATE */
#define  JCU_ERROR_STATUS       (0x10450200)
#define  JCU_ERROR_STATUS_MASK  (0x7FFFFF00)
#define  JCU_ERROR_STATUS_1     (0x10450201)
#define  JCU_ERROR_STATUS_2     (0x10450202)
#define  JCU_ERROR_STATUS_3     (0x10450203)
#define  JCU_ERROR_STATUS_4     (0x10450204)
#define  JCU_ERROR_STATUS_5     (0x10450205)
#define  JCU_ERROR_STATUS_6     (0x10450206)
#define  JCU_ERROR_STATUS_7     (0x10450207)
#define  JCU_ERROR_STATUS_8     (0x10450208)
#define  JCU_ERROR_STATUS_9     (0x10450209)
#define  JCU_ERROR_STATUS_A     (0x1045020A)
#define  JCU_ERROR_STATUS_B     (0x1045020B)
#define  JCU_ERROR_STATUS_C     (0x1045020C)
#define  JCU_ERROR_STATUS_D     (0x1045020D)
#define  JCU_ERROR_STATUS_E     (0x1045020E)
#define  JCU_ERROR_STATUS_F     (0x1045020F)
#define  JCU_ERROR_STATUS_10    (0x10450210)
#define  JCU_ERROR_STATUS_11    (0x10450211)
#define  JCU_ERROR_STATUS_12    (0x10450212)
#define  JCU_ERROR_STATUS_13    (0x10450213)
#define  JCU_ERROR_STATUS_14    (0x10450214)
#define  JCU_ERROR_STATUS_15    (0x10450215)
#define  JCU_ERROR_STATUS_16    (0x10450216)


/* Section: E_JCU_JCDERR */
/***********************************************************************
* Constants: E_JCU_JCDERR
*    Value of type of <jcu_detail_error_t> .
************************************************************************/
/* Constant: E_JCU_JCDERR_MASK
*    Mask for checking E_JCU_JCDERR. */
#define  E_JCU_JCDERR_MASK  (0x7FFFE000)
#define  E_JCU_JCDERR       (0x01452000)

/* Constant: JCU_JCDERR_OK
*    Normal */
#define  JCU_JCDERR_OK                (0)

/* Constant: JCU_JCDERR_SOI_NOT_FOUND
*    SOI not detected: SOI not detected until EOI detected */
#define  JCU_JCDERR_SOI_NOT_FOUND     (0x01452100)

/* Constant: JCU_JCDERR_INVALID_SOF
*    SOF1 to SOFF detected */
#define  JCU_JCDERR_INVALID_SOF       (0x01452200)

/* Constant: JCU_JCDERR_UNPROVIDED_SOF
*    Unprovided pixel format detected */
#define  JCU_JCDERR_UNPROVIDED_SOF    (0x01452300)

/* Constant: JCU_JCDERR_SOF_ACCURACY
*    SOF accuracy error: Other than 8 detected */
#define  JCU_JCDERR_SOF_ACCURACY      (0x01452400)

/* Constant: JCU_JCDERR_DQT_ACCURACY
*    DQT accuracy error: Other than 0 detected */
#define  JCU_JCDERR_DQT_ACCURACY      (0x01452500)

/* Constant: JCU_JCDERR_COMPONENT_1
*    Component error 1: The number of SOF0 header components detected is other than 1, 3, or 4. */
#define  JCU_JCDERR_COMPONENT_1       (0x01452600)

/* Constant: JCU_JCDERR_COMPONENT_2
*    Component error 2: The number of components differs between SOF0 header and SOS */
#define  JCU_JCDERR_COMPONENT_2       (0x01452700)

/* Constant: JCU_JCDERR_NO_SOF0_DQT_DHT
*    SOF0, DQT, and DHT not detected when SOS detected */
#define  JCU_JCDERR_NO_SOF0_DQT_DHT   (0x01452800)

/* Constant: JCU_JCDERR_SOS_NOT_FOUND
*    SOS not detected: SOS not detected until EOI detected */
#define  JCU_JCDERR_SOS_NOT_FOUND     (0x01452900)

/* Constant: JCU_JCDERR_EOI_NOT_FOUND
*    EOI not detected (default) */
#define  JCU_JCDERR_EOI_NOT_FOUND     (0x01452A00)

/* Constant: JCU_JCDERR_RESTART_INTERVAL
*    Restart interval data number error detected */
#define  JCU_JCDERR_RESTART_INTERVAL  (0x01452B00)

/* Constant: JCU_JCDERR_IMAGE_SIZE
*    Image size error detected
*
* Description:
*    If there is not EOI marker after compressed data part (huffman encoding segment)
*    in the JPEG data, JCU_JCDERR_IMAGE_SIZE error will be raised.
*    Not to specify JCU_INT_ERROR_SEGMENT_TOTAL_DATA and JCU_INT_ERROR_MCU_BLOCK_DATA bit
*    to <R_JCU_SetErrorFilter> function, JCU_JCDERR_IMAGE_SIZE error will not be raised. */
#define  JCU_JCDERR_IMAGE_SIZE        (0x01452C00)

/* Constant: JCU_JCDERR_LAST_MCU_DATA
*    Last MCU data number error detected */
#define  JCU_JCDERR_LAST_MCU_DATA     (0x01452D00)

/* Constant: JCU_JCDERR_BLOCK_DATA
*    Block data number error detected */
#define  JCU_JCDERR_BLOCK_DATA        (0x01452E00)


/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Enumeration: jcu_errorcode_t
*    Type of JCU error code. It is typedef of int
*
* Description:
*    The code raising error is found by searching the symbol in source files
*    or set conditional break point of the argument of <errnum_t> type in <R_ERROR_Set> function.
*    Example of error code: <JCU_ERROR_PARAM>
************************************************************************/
typedef errnum_t  jcu_errorcode_t; 


/***********************************************************************
* Enumeration: jcu_detail_error_t
*    Type of JCU hardware output error code. It is typedef of int.
*
* Description:
*    Example of error code: <JCU_JCDERR_SOI_NOT_FOUND>
************************************************************************/
typedef errnum_t  jcu_detail_error_t;


/******************************************************************************
Struct & Enum definitions
******************************************************************************/

/* Section: jcu_status_information_t */
/***********************************************************************
* Enumeration: jcu_status_information_t
*    Internal state of the JCU driver
************************************************************************/
typedef enum {

    /* Constant: JCU_STATUS_UNDEF
    *    0x00, The JCU is uninitialized status */
    JCU_STATUS_UNDEF = 0x00,


    /* Constant: JCU_STATUS_INIT
    *    0x01, The JCU is initialized status */
    JCU_STATUS_INIT = 0x01,


    /* Constant: JCU_STATUS_SELECTED
    *    0x02, Encoding or Decoding is selected */
    JCU_STATUS_SELECTED = 0x02,


    /* Constant: JCU_STATUS_READY
    *    0x08, The JCU decode/encode is ready, or the JCU decode/encode has been completed */
    JCU_STATUS_READY = 0x08,


    /* Constant: JCU_STATUS_RUN
    *    0x10, The JCU decode/encode being executed */
    JCU_STATUS_RUN = 0x10,


    /* Constant: JCU_STATUS_INTERRUPTING
    *    0x40, The state that interrupt occurred */
    JCU_STATUS_INTERRUPTING = 0x40,


    /* Constant: JCU_STATUS_INTERRUPTED
    *    0x80, The state after interrupt function executed */
    JCU_STATUS_INTERRUPTED = 0x80

} jcu_status_information_t;


/***********************************************************************
* Structure: jcu_async_status_t
*    Status of asynchronous operation of the JCU driver
************************************************************************/
typedef struct st_jcu_async_status_t  jcu_async_status_t;
struct st_jcu_async_status_t {  


    /* Variable: Status
    *    Main state */
    jcu_status_information_t  Status; 


    /* Variable: SubStatusFlags
    *    Causes of pausing. "jcu_sub_status_t" type */
    bit_flags_fast32_t  SubStatusFlags; 


    /* Variable: IsPaused
    *    Whether JCU is paused */
    bool_t  IsPaused; 


    /* Variable: InterruptFlags
    *    Copy of interrupt status register
    *
    * Description:
    *    Symbols related with each bit are defined in the driver.
    *    These symbols of some drivers are not public. */
    bit_flags_fast32_t  InterruptFlags; 


    /* Variable: OnInterrupted
    *    Callback function called when interrupt was signaled */
    r_co_function_t  OnInterrupted; 


    /* Variable: OnInterruptedArgument
    *    Argument of callback function called when interrupt was signaled */
    volatile void*  OnInterruptedArgument; 


    /* Variable: OnFinished
    *    Callback function called after interrupt handling */
    r_co_function_t  OnFinished; 


    /* Variable: OnFinishedArgument
    *    Argument of callback function called after interrupt handling */
    volatile void*  OnFinishedArgument; 


    /* Variable: OnFinalized
    *    Callback function called after termination processing */
    r_co_function_t  OnFinalized; 


    /* Variable: OnFinalizedArgument
    *    Argument of callback function called after termination processing */
    volatile void*  OnFinalizedArgument; 


    /* Variable: ReturnValue
    *    Return value of asynchronized operation */
    errnum_t  ReturnValue; 


    /* Variable: CancelFlags
    *    Internal flags of status regarding finalize operation from begin to end. <r_jcu_cancel_flags_t> type */
    bit_flags_fast32_t  CancelFlags; 
};


/* Section: jcu_sub_state_t */
/***********************************************************************
* Enumeration: jcu_sub_state_t
*    Sub status of the JCU driver
************************************************************************/
typedef enum {

    /* Constant: JCU_SUB_INFOMATION_READY
    *    0x00000008, This bit is set to 1, when image size or pixel format can be gotten */
    JCU_SUB_INFOMATION_READY    = 0x00000008,


    /* Constant: JCU_SUB_DECODE_OUTPUT_PAUSE
    *    0x00000100, Reserved value for division process */
    JCU_SUB_DECODE_OUTPUT_PAUSE = 0x00000100,


    /* Constant: JCU_SUB_DECODE_INPUT_PAUSE
    *    0x00000200, Reserved value for division process */
    JCU_SUB_DECODE_INPUT_PAUSE  = 0x00000200,


    /* Constant: JCU_SUB_ENCODE_OUTPUT_PAUSE
    *    0x00001000, Reserved value for division process */
    JCU_SUB_ENCODE_OUTPUT_PAUSE = 0x00001000,


    /* Constant: JCU_SUB_ENCODE_INPUT_PAUSE
    *    0x00002000, Reserved value for division process */
    JCU_SUB_ENCODE_INPUT_PAUSE  = 0x00002000,


    /* Constant: JCU_SUB_PAUSE_ALL
    *    All cause regarding pausing. It is a <jcu_sub_status_t> type */
    JCU_SUB_PAUSE_ALL = JCU_SUB_INFOMATION_READY | JCU_SUB_DECODE_OUTPUT_PAUSE |
        JCU_SUB_DECODE_INPUT_PAUSE | JCU_SUB_ENCODE_OUTPUT_PAUSE | JCU_SUB_ENCODE_INPUT_PAUSE
} jcu_sub_state_t;


/***********************************************************************
* Type: jcu_sub_status_t
*    Bitwise OR of the bit flag value <jcu_sub_state_t>
************************************************************************/
typedef bit_flags_fast32_t  jcu_sub_status_t;


/* Section: r_jcu_cancel_flag_t */
/***********************************************************************
* Enumeration: r_jcu_cancel_flag_t
*    Canceling state
************************************************************************/
typedef enum {

    /* Constant: JCU_CANNEL_REQUEST
    *    0x00000001, There is a request of canceling */
    JCU_CANNEL_REQUEST = 0x00000001,


    /* Constant: JCU_CANNELING
    *    0x00000002, Under canceling */
    JCU_CANNELING = 0x00000002,


    /* Constant: JCU_CANNELED
    *    0x00000004, Finished to cancel */
    JCU_CANNELED = 0x00000004,


    /* Constant: JCU_FINALIZE_REQUEST
    *    0x00000010, There is a request of finalizing */
    JCU_FINALIZE_REQUEST = 0x00000010,


    /* Constant: JCU_FINALIZING
    *    0x00000020, Under finalizing */
    JCU_FINALIZING = 0x00000020,


    /* Constant: JCU_FINALIZED
    *    0x00000040, Finished to finalize */
    JCU_FINALIZED = 0x00000040

} r_jcu_cancel_flag_t;


/***********************************************************************
* Type: r_jcu_cancel_flags_t
*    Bitwise OR of the bit flag value <r_jcu_cancel_flag_t>
************************************************************************/
typedef bit_flags_fast32_t  r_jcu_cancel_flags_t;


/* Section: jcu_codec_t */
/***********************************************************************
* Enumeration: jcu_codec_t
*    Mode selection (Compression or De-compression)
************************************************************************/
typedef enum {

    /* Constant: JCU_ENCODE
    *    0,  Compression process */
    JCU_ENCODE       = 0,

    /* Constant: JCU_DECODE
    *    1,  De-compression process */
    JCU_DECODE       = 1,

    /* Constant: JCU_NOT_SELECTED
    *    2,  Not selected state */
    JCU_NOT_SELECTED = 2

} jcu_codec_t;


/* Section: jcu_continue_type_t */
/***********************************************************************
* Enumeration: jcu_continue_type_t
*    Paused factor (continue mode)
************************************************************************/
typedef enum {

    /* Constant: JCU_INPUT_BUFFER
    *    0,  Reserved value for division process */
    JCU_INPUT_BUFFER,


    /* Constant: JCU_OUTPUT_BUFFER
    *    1,  Reserved value for division process */
    JCU_OUTPUT_BUFFER,


    /* Constant: JCU_IMAGE_INFO
    *    2,  Clears the process-stopped state caused by requests to read the image information */
    JCU_IMAGE_INFO

} jcu_continue_type_t;


/* Section: jcu_int_detail_error_t */
/***********************************************************************
* Enumeration: jcu_int_detail_error_t
*    Type of detected decode error
************************************************************************/
typedef enum {

    /* Constant: JCU_INT_ERROR_RESTART_INTERVAL_DATA
    *    INT7_MASK,  The number of data in the restart interval of
    *    the Huffman-coding segment is not correct in de-compression */
    JCU_INT_ERROR_RESTART_INTERVAL_DATA = 0x80u,


    /* Constant: JCU_INT_ERROR_SEGMENT_TOTAL_DATA
    *    INT6_MASK,  The total number of data in the Huffman-coding segment
    *    is not correct in de-compression */
    JCU_INT_ERROR_SEGMENT_TOTAL_DATA = 0x40u,


    /* Constant: JCU_INT_ERROR_MCU_BLOCK_DATA
    *    INT5_MASK,  The final number of MCU data in the Huffman-coding segment
    *    is not correct in de-compression */
    JCU_INT_ERROR_MCU_BLOCK_DATA = 0x20u,


    /* Constant: JCU_INT_ERROR_ALL
    *    All type */
    JCU_INT_ERROR_ALL                   = ( JCU_INT_ERROR_RESTART_INTERVAL_DATA |
        JCU_INT_ERROR_SEGMENT_TOTAL_DATA | JCU_INT_ERROR_MCU_BLOCK_DATA )

} jcu_int_detail_error_t;


/***********************************************************************
* Type: jcu_int_detail_errors_t
*    Bitwise OR of the bit flag value <jcu_int_detail_error_t>
************************************************************************/
typedef bit_flags_fast32_t  jcu_int_detail_errors_t;


/* Section: jcu_swap_t */
/***********************************************************************
* Enumeration: jcu_swap_t
*    Swap setting
************************************************************************/
typedef enum {

    /* Constant: JCU_SWAP_NONE
    *    0x00,  01234567,  No swap */
    JCU_SWAP_NONE = 0x00,


    /* Constant: JCU_SWAP_BYTE
    *    0x01,  10325476,  Byte swap */
    JCU_SWAP_BYTE = 0x01,


    /* Constant: JCU_SWAP_WORD
    *    0x02,  23016745,  Word swap */
    JCU_SWAP_WORD = 0x02,


    /* Constant: JCU_SWAP_WORD_AND_BYTE
    *    0x03,  32107654,  Word-byte swap */
    JCU_SWAP_WORD_AND_BYTE = 0x03,


    /* Constant: JCU_SWAP_LONG_WORD
    *    0x04,  45670123,  Longword swap */
    JCU_SWAP_LONG_WORD = 0x04,


    /* Constant: JCU_SWAP_LONG_WORD_AND_BYTE
    *    0x05,  54761032,  Longword-byte swap */
    JCU_SWAP_LONG_WORD_AND_BYTE = 0x05,


    /* Constant: JCU_SWAP_LONG_WORD_AND_WORD
    *    0x06,  67452301,  Longword-word swap */
    JCU_SWAP_LONG_WORD_AND_WORD = 0x06,


    /* Constant: JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE
    *    0x07,  76543210,  Longword-word-byte swap */
    JCU_SWAP_LONG_WORD_AND_WORD_AND_BYTE = 0x07

} jcu_swap_t;


/***********************************************************************
* Structure: jcu_buffer_t
*    Structure for the input/output buffer
************************************************************************/
typedef struct {

    /* Variable: swapSetting
    *    Byte/Word/Longword Swap */
    jcu_swap_t  swapSetting; 


    /* Variable: address
    *    Buffer address */
    uint32_t*  address; 

} jcu_buffer_t;


/***********************************************************************
* Structure: jcu_buffer_param_t
*    Parameters for the input/output buffer setting in de-compression
************************************************************************/
typedef struct {

    /* Variable: source
    *    Attributes of input buffer */
    jcu_buffer_t  source;


    /* Variable: destination
    *    Attributes of output buffer */
    jcu_buffer_t  destination;


    /* Variable: lineOffset
    *    Count of pixels in one line */
    int16_t       lineOffset; 

} jcu_buffer_param_t;


/* Section: jcu_sub_sampling_t */
/***********************************************************************
* Enumeration: jcu_sub_sampling_t
*    Sub sample of the decoded image data
************************************************************************/
typedef enum {

    /* Constant: JCU_SUB_SAMPLING_1_1
    *    0x00,  No subsampling */
    JCU_SUB_SAMPLING_1_1 = 0x00,


    /* Constant: JCU_SUB_SAMPLING_1_2
    *    0x01,  Subsamples output data into 1/2 */
    JCU_SUB_SAMPLING_1_2 = 0x01,


    /* Constant: JCU_SUB_SAMPLING_1_4
    *    0x02,  Subsamples output data into 1/4 */
    JCU_SUB_SAMPLING_1_4 = 0x02,


    /* Constant: JCU_SUB_SAMPLING_1_8
    *    0x03,  Subsamples output data into 1/8 */
    JCU_SUB_SAMPLING_1_8 = 0x03

} jcu_sub_sampling_t;


/* Section: jcu_decode_format_t */
/***********************************************************************
* Enumeration: jcu_decode_format_t
*    Output pixel format of RAW image data
************************************************************************/
typedef enum {

    /* Constant: JCU_OUTPUT_YCbCr422
    *    0x00,  YCbCr422 */
    JCU_OUTPUT_YCbCr422 = 0x00,


    /* Constant: JCU_OUTPUT_ARGB8888
    *    0x01,  ARGB8888 */
    JCU_OUTPUT_ARGB8888 = 0x01,


    /* Constant: JCU_OUTPUT_RGB565
    *    0x02,  RGB565 */
    JCU_OUTPUT_RGB565   = 0x02

} jcu_decode_format_t;


/* Section: jcu_color_element_t */
/***********************************************************************
* Enumeration: jcu_color_element_t
*    Color elements. Y, Cb, Cr
************************************************************************/
typedef enum {

    /* Constant: JCU_ELEMENT_Y
    *    0,  Y of YCbCr format */
    JCU_ELEMENT_Y,


    /* Constant: JCU_ELEMENT_Cb
    *    1,  Cb of YCbCr format */
    JCU_ELEMENT_Cb,


    /* Constant: JCU_ELEMENT_Cr
    *    2,  Cr of YCbCr format */
    JCU_ELEMENT_Cr

} jcu_color_element_t;


/***********************************************************************
* Constant: JCU_COLOR_ELEMENT_NUM
*    Number of color elements. Y, Cb, Cr
************************************************************************/
#define  JCU_COLOR_ELEMENT_NUM  (3)


/* Section: jcu_cbcr_offset_t */
/***********************************************************************
* Enumeration: jcu_cbcr_offset_t
*    The range of Cb/Cr
************************************************************************/
typedef enum {

    /* Constant: JCU_CBCR_OFFSET_0
    *    0,  The range of Cb/Cr is from -128 to 127 */
    JCU_CBCR_OFFSET_0 = 0,


    /* Constant: JCU_CBCR_OFFSET_128
    *    1,  The range of Cb/Cr is from 0 to 255 */
    JCU_CBCR_OFFSET_128 = 1

} jcu_cbcr_offset_t;


/* Section: jcu_jpeg_format_t */
/***********************************************************************
* Enumeration: jcu_jpeg_format_t
*    Pixel format of JPEG image data
************************************************************************/
typedef enum {

    /* Constant: JCU_JPEG_YCbCr444
    *    0,  YCbCr444 */
    JCU_JPEG_YCbCr444   = 0x00,


    /* Constant: JCU_JPEG_YCbCr422
    *    1,  YCbCr422 */
    JCU_JPEG_YCbCr422   = 0x01,


    /* Constant: JCU_JPEG_YCbCr420
    *    2,  YCbCr420 */
    JCU_JPEG_YCbCr420   = 0x02,


    /* Constant: JCU_JPEG_YCbCr411
    *    6,  YCbCr411 */
    JCU_JPEG_YCbCr411   = 0x06

} jcu_jpeg_format_t;


/***********************************************************************
* Structure: jcu_image_info_t
*    Structure for the image information of the decoded JPEG data
************************************************************************/
typedef struct {

    /* Variable: width
    *    The width of the image data */
    uint32_t  width;


    /* Variable: height
    *    The height of the image data */
    uint32_t  height;


    /* Variable: encodedFormat
    *    The pixel format of original JPEG data */
    jcu_jpeg_format_t  encodedFormat; 

} jcu_image_info_t;


/* Section: jcu_table_no_t */
/***********************************************************************
* Enumeration: jcu_table_no_t
*    Quantization table number or huffman table number
************************************************************************/
typedef enum {

    /* Constant: JCU_TABLE_NO_0
    *    0 */
    JCU_TABLE_NO_0 = 0,


    /* Constant: JCU_TABLE_NO_1
    *    1 */
    JCU_TABLE_NO_1 = 1,


    /* Constant: JCU_TABLE_NO_2
    *    2. This must not be used for huffman table */
    JCU_TABLE_NO_2 = 2,


    /* Constant: JCU_TABLE_NO_3
    *    3. This must not be used for huffman table */
    JCU_TABLE_NO_3 = 3

} jcu_table_no_t;


/* Section: jcu_huff_t */
/***********************************************************************
* Enumeration: jcu_huff_t
*    Type of Huffman table (AC or DC)
************************************************************************/
typedef enum {

    /* Constant: JCU_HUFFMAN_AC
    *    0,  AC */
    JCU_HUFFMAN_AC,


    /* Constant: JCU_HUFFMAN_DC
    *    1,  DC */
    JCU_HUFFMAN_DC

} jcu_huff_t;


/***********************************************************************
* Structure: jcu_decode_param_t
*    Parameters for the option setting in de-compression
************************************************************************/
typedef struct {

    /* Variable: verticalSubSampling
    *     Vertical subsampling */
    jcu_sub_sampling_t  verticalSubSampling; 


    /* Variable: horizontalSubSampling
    *     Horizontal subsampling */
    jcu_sub_sampling_t  horizontalSubSampling; 


    /* Variable: decodeFormat
    *     The output pixel format of RAW image data */
    jcu_decode_format_t  decodeFormat; 


    /* Variable: outputCbCrOffset
    *     The range of Cb/Cr
    *
    * Description:
    *    If the pixel format isn't YCbCr, the Cb/Cr value must be <JCU_CBCR_OFFSET_0>. */
    jcu_cbcr_offset_t  outputCbCrOffset; 


    /* Variable: alpha
    *     The alpha value, when pixel format ARGB888 is selected
    *
    * Description:
    *    If the pixel format isn't ARGB8888, the alpha value must be zero. */
    uint8_t  alpha;

} jcu_decode_param_t;


/***********************************************************************
* Structure: jcu_encode_param_t
*    Parameters for the option setting in compression
************************************************************************/
typedef struct {

    /* Variable: encodeFormat
    *    The pixel format of compressed JPEG data
    *
    * Description:
    *    This value must be JCU_JPEG_YCbCr422 */
    jcu_jpeg_format_t   encodeFormat; 


    /* Variable: QuantizationTable
    *    Quantization table used when compressing */
    uint8_t  QuantizationTable[ JCU_COLOR_ELEMENT_NUM ]; 


    /* Variable: HuffmanTable
    *    Huffman table used when compressing */
    uint8_t  HuffmanTable[ JCU_COLOR_ELEMENT_NUM ]; 


    /* Variable: DRI_value
    *    DRI (Define Restart Interval) value in encoded JPEG */
    uint32_t  DRI_value; 


    /* Variable: width
    *    The width of the input image data */
    uint32_t  width;


    /* Variable: height
    *    The height of the input image data */
    uint32_t  height;


    /* Variable: inputCbCrOffset
    *    The range of Cb/Cr of input image data */
    jcu_cbcr_offset_t   inputCbCrOffset; 

} jcu_encode_param_t;


/* Section: jcu_codec_status_t */
/***********************************************************************
* Enumeration: jcu_codec_status_t
*    Codec type
************************************************************************/
typedef enum {

    /* Constant: JCU_CODEC_NOT_SELECTED
    *    -1,  The state of the JCU mode is not selected */
    JCU_CODEC_NOT_SELECTED  = -1,


    /* Constant: JCU_STATUS_ENCODE
    *    0,  The state of the JCU mode is <JCU_ENCODE> */
    JCU_STATUS_ENCODE       = 0,


    /* Constant: JCU_STATUS_DECODE
    *    1,  The state of the JCU mode is <JCU_DECODE> */
    JCU_STATUS_DECODE       = 1

} jcu_codec_status_t;


/***********************************************************************
* Structure: jcu_config_t
*    Configuration of JCU
*
* Example:
*    > jcu_config_t  config = {0};  // Default setting
*
* Description:
*    There is not any setting in this version.
************************************************************************/
typedef struct st_jcu_config_t {
    int32_t  reserved;
} jcu_config_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* #define  R_JCU_API_TYPEDEF_H */
