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
* File         : jcu_para.c
* Description  : JPEG Codec Unit (JCU) driver. checking parameter.
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
#include  "r_register_bit.h"
#include  "../src/r_jcu_local.h"
#include  "rz_co.h"

#ifndef  JCU_RELEASE

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define MASK_LOW_3BIT   (0x7u)
#define MOD_8           (0x8u)
#define MOD_16          (0x10u)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/

/***********************************************************************
* Function Name: JCU_ParaCheckSelectCodec
*    SelectCodec api's parameter checking
*
* Arguments:
*    [in] codec  codec type
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckSelectCodec( 
    const jcu_codec_t   codec)
{
    jcu_errorcode_t  returnValue; 

    if ((codec != JCU_ENCODE)
        && (codec != JCU_DECODE))
    {
        returnValue = JCU_ERROR_PARAM_F;  /* Error: <Parameter/> */
    }
    else
    {
        returnValue = JCU_ERROR_OK;
    } /* end if */

    R_ERROR_Set( returnValue );
    return (returnValue);
} /* End of function JCU_ParaCheckSelectCodec */

/***********************************************************************
* Function Name: JCU_ParaCheckStart
*    Start api's parameter checking
*
* Arguments:
*    [in] codec  codec type
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckStart( 
    void)
{
    jcu_errorcode_t   returnValue; 

    /* If jcu already start, it cannot update this bit (this is limitation of hardware) */
    if ( (uint32_t) R_BIT_GET_IN_REGISTER8( JCU,JCCMD,JSRT ) == 1u )
    {
        returnValue = JCU_ERROR_STATUS_14;  /* Error: */
    }
    else
    {
        returnValue = JCU_ERROR_OK;
    } /* end if */

    R_ERROR_Set( returnValue );
    return (returnValue);
} /* End of function JCU_ParaCheckStart */

/***********************************************************************
* Function Name: JCU_ParaCheckSetDecodeParam
*    SetDecodeParam api's parameter checking
*
* Arguments:
*    [in] buffer        - input and output buffer settings
*    [in] interruptKind - tye type of interrupt that use in this system
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetDecodeParam( 
    const jcu_decode_param_t   *const decode, 
    const jcu_buffer_param_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK; 

    /* Cast to an appropriate type */
    if ((decode == NULL) || (buffer == NULL))
    {
        returnValue = JCU_ERROR_PARAM_19;  /* Error: <Parameter/> */
        goto fin;
    } /* end if */

    /* NULL check for the address data */
    if ((buffer->source.address == NULL)|| (buffer->destination.address == NULL))
    {
        returnValue = JCU_ERROR_PARAM_1A;  /* Error: <Parameter/> */
        goto fin;
    }  /* end if */

    /* Check for the address alignment */
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    if ( ((uint32_t) buffer->source.address % MOD_8) != 0u )
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
    {
        returnValue = JCU_ERROR_PARAM_35;  /* Error: The source address is not multiple of 8. */
        goto fin;
    }  /* end if */

    /* Check for the address alignment */
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    if ( ((uint32_t) buffer->destination.address % MOD_8) != 0u )
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
    {
        returnValue = JCU_ERROR_PARAM_1B;  /* Error: The destination address is not multiple of 8. */
        goto fin;
    }  /* end if */

    /*Check for Cb/Cr offset */
    if(decode->decodeFormat == JCU_OUTPUT_YCbCr422)
    {
        if (((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_0) && ((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_128)) 
        {
            returnValue = JCU_ERROR_PARAM_1C;  /* Error: Bad value in "outputCbCrOffset" */
        }  /* end if */
    }
    else
    {
        if ((decode->outputCbCrOffset) != JCU_CBCR_OFFSET_0) 
        {
            returnValue = JCU_ERROR_PARAM_34;  /* Error: Bad value in "outputCbCrOffset" */
        }  /* end if */
    } /* end if */
fin:
    {
        R_ERROR_Set(returnValue);
    }
    return (returnValue);
} /* End of function JCU_ParaCheckSetDecodeParam */

/***********************************************************************
* Function Name: JCU_ParaCheckGetImageInfo
*    GetImageInfo api's parameter checking
*
* Arguments:
*    [in] buffer         buffer address that set the image information
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckGetImageInfo( 
    const jcu_image_info_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK; 

    /* Cast to an appropriate type */
    if (buffer == NULL)
    {
        returnValue = JCU_ERROR_PARAM_1D;  /* Error: <Parameter/> */
    } /* end if */

    R_ERROR_Set( returnValue );
    return (returnValue);
} /* End of function JCU_ParaCheckGetImageInfo */

/***********************************************************************
* Function Name: JCU_ParaCheckSetQuantizationTbl
*    SetQuantizationTable api's parameter checking
*
* Arguments:
*    [in] tableNo        the table number that set the parameter
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetQuantizationTbl( 
    const jcu_table_no_t   tableNo)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK; 

    /* ->MISRA 13.7 : For fail safe. */
    if (tableNo > JCU_TABLE_NO_3)
    /* <-MISRA 13.7 */
    /* ->MISRA 14.1 ->QAC 3201 : For fail safe. */
    {
        returnValue = JCU_ERROR_PARAM_1E;  /* Error: <Parameter/> */
    } /* end if */
    /* <-MISRA 14.1 <-QAC 3201 */

    R_ERROR_Set( returnValue );
    return (returnValue);
} /* End of function JCU_ParaCheckSetQuantizationTbl */

/***********************************************************************
* Function Name: JCU_ParaCheckSetHuffmanTable
*    SetHuffmanTable api's parameter checking
*
* Arguments:
*    [in] tableNo   - the table number that set the parameter
*    [in] type      - the type which table is set(AC or DC)
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckSetHuffmanTable( 
    const jcu_table_no_t   tableNo,
    const jcu_huff_t       type)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK; 

    /* ->MISRA 13.7 : For fail safe. */
    if (tableNo > JCU_TABLE_NO_1)

    /* <-MISRA 13.7 */
    {
        returnValue = JCU_ERROR_PARAM_1F;  /* Error: <Parameter/> */
        goto fin;
    } /* end if */

    /* ->MISRA 13.7 : For fail safe. */
    if ((type != JCU_HUFFMAN_AC)
        && (type != JCU_HUFFMAN_DC))

    /* <-MISRA 13.7 */
    /* ->MISRA 14.1 ->QAC 3201 : For fail safe. */
    {
        returnValue = JCU_ERROR_PARAM_20;  /* Error: <Parameter/> */
    } /* end if */

    /* <-MISRA 14.1 <-QAC 3201 */
fin:
    {
        R_ERROR_Set(returnValue);
    }
    return (returnValue);
} /* End of function JCU_ParaCheckSetHuffmanTable */

/***********************************************************************
* Function Name: JCU_ParaCheckEncodeParam
*     api's parameter checking
*
* Arguments:
*     encode - [in] <jcu_encode_param_t>.
*     buffer - [in] <jcu_buffer_param_t>.
*
* Return Value:
*    jcu_errorcode_t
************************************************************************/
jcu_errorcode_t JCU_ParaCheckEncodeParam( 
    const jcu_encode_param_t   *const encode, 
    const jcu_buffer_param_t   *const buffer)
{
    jcu_errorcode_t  returnValue = JCU_ERROR_OK; 

    /* Cast to an appropriate type */
    if ((encode == NULL) || (buffer == NULL))
    {
        returnValue = JCU_ERROR_PARAM_21;  /* Error: <Parameter/> */
        goto fin;
    } /* end if */

    /* NULL check for the address data */
    if ((buffer->source.address == NULL) || (buffer->destination.address == NULL))
    {
        returnValue = JCU_ERROR_PARAM_22;  /* Error: <Parameter/> */
        goto fin;
    }  /* end if */


    if (encode->encodeFormat == JCU_JPEG_YCbCr422)
    {
        /* Check the width of the image data */
        if ((encode->width % MOD_16) != 0u)
        {
            returnValue = JCU_ERROR_PARAM_23;  /* Error: <Parameter/> */
            goto fin;
        } /* end if */

        /* Check the height of the image data */
        if ((encode->height % MOD_8) != 0u)
        {
            returnValue = JCU_ERROR_PARAM_24;  /* Error: <Parameter/> */
            goto fin;
        } /* end if */
    }
    else
    {
        returnValue = JCU_ERROR_PARAM;
        goto fin;
    } /* end if */

    /*Check for Cb/Cr offset */
    if (((encode->inputCbCrOffset) != JCU_CBCR_OFFSET_0) && ((encode->inputCbCrOffset) != JCU_CBCR_OFFSET_128))
    {
        returnValue = JCU_ERROR_PARAM_25;  /* Error: <Parameter/> */
    }  /* end if */

fin:
    {
        R_ERROR_Set(returnValue);
    }
    return (returnValue);
} /* End of function JCU_ParaCheckEncodeParam */

#endif /* JCU_RELEASE */
