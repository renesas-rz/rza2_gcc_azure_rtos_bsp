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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* System Name  : DRP Driver
* File Name    : r_dk2_core.h
* Device       : RZ/A2M
* Abstract     : Control software of DRP.
* Tool-Chain   : Renesas e2 studio
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Core part of DRP Driver.
* Limitation   : None
**********************************************************************************************************************/
/**********************************************************************************************************************
* History      : History is managed by Revision Control System.
**********************************************************************************************************************/
/**********************************************************************************************************************
Includes <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "r_dk2_if.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#ifndef R_DK2_CORE_H
#define R_DK2_CORE_H

/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/

/**********************************************************************************************************************
Public Functions
**********************************************************************************************************************/
/* Function Name: R_DK2_CORE_Initialize */
void R_DK2_CORE_Initialize(void);
/* Function Name: R_DK2_CORE_PreLoad */
int32_t R_DK2_CORE_PreLoad(const uint8_t tile_num, const uint8_t top_tiles, const uint32_t config, const uint32_t size,
    const bool del_zero, const uint16_t context_num, const uint16_t state_num, void *const pwork, uint8_t *const pid);
/* Function Name: R_DK2_CORE_Load */
int32_t R_DK2_CORE_Load(const uint8_t id, const uint8_t top_tiles, const uint32_t tile_pattern,
    const uint16_t state_num, const uint32_t work, uint8_t *const paid);
/* Function Name: R_DK2_CORE_Activate */
int32_t R_DK2_CORE_Activate(const uint8_t id, const uint8_t div);
/* Function Name: R_DK2_CORE_PreStart */
int32_t R_DK2_CORE_PreStart(const uint8_t id, void *const pwork, const uint32_t param, const uint32_t size);
/* Function Name: R_DK2_CORE_Start */
int32_t R_DK2_CORE_Start(const uint8_t id, const uint32_t work);
/* Function Name: R_DK2_CORE_Unload */
int32_t R_DK2_CORE_Unload(const uint8_t id);
/* Function Name: R_DK2_CORE_GetInt */
uint32_t R_DK2_CORE_GetInt(void);
/* Function Name: R_DK2_CORE_GetVersion */
uint32_t R_DK2_CORE_GetVersion(void);

#endif /* R_DK2_CORE_H */
