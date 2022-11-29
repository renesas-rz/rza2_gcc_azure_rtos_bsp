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
* File Name    : r_jcu_local.h
* Description  : JPEG Codec Unit (JCU) driver. Internal functions.
* Version      : 1.12
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_jcu.h"
#include  "rz_co_typedef.h"

#ifndef JCU_LOCAL_H
#define JCU_LOCAL_H

/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Constant: JCU_JINTS1_ALL
************************************************************************/
#define JCU_JINTS1_ALL  ( \
    JCU_JINTS1_CBTF   | \
    JCU_JINTS1_DINLF  | \
    JCU_JINTS1_JOUTF  | \
    JCU_JINTS1_DBTF   | \
    JCU_JINTS1_JINF   | \
    JCU_JINTS1_DOUTLF )


/******************************************************************************
Typedef definitions
******************************************************************************/

/***********************************************************************
* Structure: jcu_internal_information_t
*    Internal Information for this driver.
************************************************************************/
typedef struct {
    jcu_codec_status_t       Codec; 
    bool_t                   IsCountMode; 
    jcu_int_detail_errors_t  ErrorFilter;  /* 1=Enable */ 

    jcu_async_status_t       AsyncStatus; 
} jcu_internal_information_t;


/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
* Function Name: R_JCU_GetInternalInformation
*    R_JCU_GetInternalInformation (for internal)
*
* Arguments:None
* Return Value:<jcu_internal_information_t> type.
 *****************************************************************************/
jcu_internal_information_t*  R_JCU_GetInternalInformation(void);

/******************************************************************************
* Function Name: R_JCU_FinalizeStep2_Sub
*    Terminate the driver function (for internal)
*
* Arguments:None
* Return Value:<jcu_errorcode_t> type.
 *****************************************************************************/
jcu_errorcode_t  R_JCU_FinalizeStep2_Sub(void);

#ifndef  JCU_RELEASE
/******************************************************************************
* Function Name: JCU_ParaCheckStart
*    Start api's parameter checking
*
* Arguments:[in] codec  codec type
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckStart(void);

/******************************************************************************
* Function Name: JCU_ParaCheckSelectCodec
*    SelectCodec api's parameter checking
*
* Arguments:[in] codec  codec type
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSelectCodec(const jcu_codec_t codec);

/******************************************************************************
* Function Name: JCU_ParaCheckSetDecodeParam
*    SetDecodeParam api's parameter checking
*
* Arguments:
*    [in] buffer        - input and output buffer settings
*    [in] interruptKind - tye type of interrupt that use in this system
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetDecodeParam
    (const jcu_decode_param_t *const decode, const jcu_buffer_param_t *const buffer);

/******************************************************************************
* Function Name: JCU_ParaCheckGetImageInfo
*    GetImageInfo api's parameter checking
*
* Arguments:[in] buffer         buffer address that set the image information
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckGetImageInfo(const jcu_image_info_t *const buffer);

/******************************************************************************
* Function Name: JCU_ParaCheckSetQuantizationTbl
*    SetQuantizationTable api's parameter checking
*
* Arguments:[in] tableNo        the table number that set the parameter
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetQuantizationTbl(const jcu_table_no_t tableNo);

/******************************************************************************
* Function Name: JCU_ParaCheckSetHuffmanTable
*    SetHuffmanTable api's parameter checking
*
* Arguments:
*    [in] tableNo   - the table number that set the parameter
*    [in] type      - the type which table is set(AC or DC)
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetHuffmanTable(const jcu_table_no_t tableNo, const jcu_huff_t type);

/******************************************************************************
* Function Name: JCU_ParaCheckEncodeParam
*     api's parameter checking
*
* Arguments:
*     encode - [in] <jcu_encode_param_t>.
*     buffer - [in] <jcu_buffer_param_t>.
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_ParaCheckEncodeParam
    (const jcu_encode_param_t *const encode, const jcu_buffer_param_t *const buffer);

#endif /* JCU_RELEASE */

/******************************************************************************
* Function Name: JCU_SetRegisterForInitialize
*    SetRegisterForInitialize
*
* Return Value:jcu_errorcode_t
 *****************************************************************************/
jcu_errorcode_t JCU_SetRegisterForInitialize(void);

/******************************************************************************
* Function Name: JCU_SetRegisterForCodec
*    SetRegisterForCodec
*
* Arguments:codec
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForCodec(const jcu_codec_t codec);

/******************************************************************************
* Function Name: JCU_SetRegisterForStart
*    SetRegisterForStart
*
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForStart(const jcu_int_detail_errors_t error_filter);

/******************************************************************************
* Function Name: JCU_SetRegisterForContinue
*    SetRegisterForContinue
*
* Arguments:type - [in]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForContinue(const jcu_continue_type_t type);

/******************************************************************************
* Function Name: JCU_SetRegisterForSetDecodePrm
*    SetRegisterForSetDecod
*
* Arguments:
*     decode - [in]
*     buffer - [in]
*     interruptKind - [in]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForSetDecodePrm(const jcu_decode_param_t *const decode, const jcu_buffer_param_t *const buffer);

/******************************************************************************
* Function Name: JCU_SetRegisterForSetQtTable
*    SetRegisterForSetQtTable
*
* Arguments:
*     tableNo - [in]
*     table   - [in]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForSetQtTable(const jcu_table_no_t tableNo, const uint8_t* const  table);

/******************************************************************************
* Function Name: JCU_SetRegisterForSetHuffTbl
*    SetRegisterForSetHuffTbl
*
* Arguments:
*     tableNo - [in]
*     type    - [in]
*     table   - [in]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForSetHuffTbl(const jcu_table_no_t tableNo, const jcu_huff_t type, const uint8_t *const table);

/******************************************************************************
* Function Name: JCU_SetRegisterForSetEncodePrm
*    SetRegisterForSetEncode
*
* Arguments:
*     encode - [in]
*     buffer - [in]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForSetEncodePrm(const jcu_encode_param_t *const encode, const jcu_buffer_param_t *const buffer);

/******************************************************************************
* Function Name: JCU_SetRegisterForSetPause
*
* Arguments:is_stop - [out]
* Return Value:None
 *****************************************************************************/
void JCU_SetRegisterForSetPause(const bool_t is_pause, const jcu_int_detail_errors_t filter);

/******************************************************************************
* Function Name: JCU_GetRegisterForGetImageInfo
*    Get "jcu_image_info_t" type value
*
* Arguments:buffer - [out]<jcu_image_info_t>.
* Return Value:None
 *****************************************************************************/
void JCU_GetRegisterForGetImageInfo(jcu_image_info_t *const buffer);

/******************************************************************************
* Function Name: JCU_GetRegisterForGetErrorInfo
*    Get "jcu_detail_error_t" type value
*
* Arguments:errorCode - [out].
* Return Value:None
 *****************************************************************************/
void JCU_GetRegisterForGetErrorInfo(jcu_detail_error_t *const errorCode);

/******************************************************************************
* Function Name: JCU_GetEncodedSize
*    GetEncodedSize
*
* Arguments:out_Size - [out].
* Return Value:None
 *****************************************************************************/
void JCU_GetEncodedSize(size_t *const out_Size);

/******************************************************************************
* Function Name: JCU_OnInterrupting
*    Operations on interrupting
*
* Return Value:Error code, 0=No error
 *****************************************************************************/
void JCU_OnInterrupting( bit_flags_fast32_t*  in_out_InterruptFlags );

/******************************************************************************
* Function Name: JCU_ClearInterruptFlag
*    ClearInterruptFlag
*
* Return Value:None
 *****************************************************************************/
void JCU_ClearInterruptFlag(void);

#endif /* JCU_LOCAL_H */
