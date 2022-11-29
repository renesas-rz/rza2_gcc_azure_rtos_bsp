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
* File Name   : r_jcu_pl.h
* Description : JPEG Codec Unit (JCU) Sample Driver. OS Porting Layer.
* Version     : 1.12
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "rz_co_typedef.h"

#ifndef JCU_PL_H
#define JCU_PL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
* Function Name: R_JCU_OnInitialize
*    OnInitialize
*
* Arguments:None.
* Return Value:Error code, 0=No error.
 *****************************************************************************/
errnum_t  R_JCU_OnInitialize(void);

/******************************************************************************
* Function Name: R_JCU_OnFinalize
*    OnFinalize
*
* Arguments:None.
* Return Value:Error code or e, 0 = successful and input e=0.
 *****************************************************************************/
errnum_t  R_JCU_OnFinalize(void);

/******************************************************************************
* Function Name: R_JCU_DisableInterrupt
*    Start I-Lock
*
* Arguments:None.
* Return Value:Whether interrupts of this function was enabled.
 *****************************************************************************/
bool_t    R_JCU_DisableInterrupt(void);

/******************************************************************************
* Function Name: R_JCU_EnableInterrupt
*    End I-Lock
*
* Arguments:None.
* Return Value:None.
 *****************************************************************************/
void      R_JCU_EnableInterrupt(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* JCU_PL_H */

