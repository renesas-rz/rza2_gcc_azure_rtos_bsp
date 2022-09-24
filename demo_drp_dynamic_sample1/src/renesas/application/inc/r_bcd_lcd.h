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
/**************************************************************************//**
* @file         r_bcd_lcd.h
* @version      0.01
* @brief        graphics lcd header
 ******************************************************************************/
#ifndef R_BCD_LCD_H
#define R_BCD_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "draw.h"
#include "r_bcd_main.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define R_BCD_LCD_WIDTH     (640u)
#define R_BCD_LCD_HEIGHT    (480u)
#define R_BCD_LCD_STRIDE    (32u)

/******************************************************************************
Functions Prototypes
******************************************************************************/
/******************************************************************************
 * Function Name: R_BCD_LcdVSync
* Description   : Interrupt process for VDC6
*               : Check the swap request of the video buffer and the overlap buffer,
*               : and swap the buffer if there is a request.
* Arguments     : -
* Return Value  : -
 *****************************************************************************/
void R_BCD_LcdVSync(void);

/******************************************************************************
 * Function Name: R_BCD_LcdInit
* Description   : LCD initialization processing
* Arguments     : -
* Return Value  : Always returns 0
 *****************************************************************************/
int32_t R_BCD_LcdInit(void);


/******************************************************************************
 * Function Name: R_BCD_LcdGetVramAddress
* Description   : Acquire the address of the video buffer which is not currently being displayed
* Arguments     : -
* Return Value  : Video buffer address
 *****************************************************************************/
uint8_t *R_BCD_LcdGetVramAddress(void);

/******************************************************************************
 * Function Name: R_BCD_LcdSwapVideoBuffer
* Description   : Record video buffer swap request
* Arguments     : -
* Return Value  : -
 *****************************************************************************/
void R_BCD_LcdSwapVideoBuffer(void);

/******************************************************************************
 * Function Name: R_BCD_LcdSwapGraphicsBuffer
* Description   : Record graphics buffer swap request
* Arguments     : -
* Return Value  : -
 *****************************************************************************/
void R_BCD_LcdSwapGraphicsBuffer(void);

/******************************************************************************
 * Function Name: R_BCD_LcdWriteChar
* Description   : Display character in overlap buffer
* Arguments     : code
*               :   Code to be displayed
*               : x
*               :   X coordinate to display character
*               : y
*               :   Y coordinate to display character
*               : color
*               :   character color
* Return Value  : -
 *****************************************************************************/
void R_BCD_LcdWriteChar(uint8_t code, uint32_t x, uint32_t y, uint8_t color);

/******************************************************************************
 * Function Name: R_BCD_LcdWriteString
 * Description   : Display character string in overlap buffer
 * Arguments     : pcode
 *               :   A pointer to the character string to be displayed
 *               : x
 *               :   X coordinate to display character string
 *               : y
 *               :   Y coordinate to display character string
 *               : color
 *               :   character string color
 * Return Value  : -
 *****************************************************************************/
void R_BCD_LcdWriteString(uint8_t *pcode, uint32_t x, uint32_t y, uint8_t color);

/******************************************************************************
 * Function Name: R_BCD_LcdWriteLine
 * Description  : Draw a line in the overlap buffer
 * Arguments    : x1
 *              :   X coordinate of the start point of the line
 *              : y1
 *              :   Y coordinate of the start point of the line
 *              : x2
 *              :   X coordinate of the end point of the line
 *              : y2
 *              :   Y coordinate of the end point of the line
 *              : color
 *              :   Line color
 * Return Value : -
 *****************************************************************************/
void R_BCD_LcdWriteLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color);

/******************************************************************************
 * Function Name: R_BCD_LcdGetOLVramAddress
 * Description  : Acquire the address of the video buffer which is not currently being displayed
 * Arguments    : -
 * Return Value : Video buffer address
 *****************************************************************************/
uint8_t *R_BCD_LcdGetVramAddress(void);

/******************************************************************************
 * Function Name: R_BCD_LcdGetOLVramAddress
 * Description  : Acquire the address of the Overlap buffer which is not currently being displayed
 * Arguments    : -
 * Return Value : Overlap buffer address
 *****************************************************************************/
uint8_t *R_BCD_LcdGetOLVramAddress(void);

/******************************************************************************
 * Function Name :R_BCD_LcdClearGraphicsBuffer
 * Description   : Clear overlap buffer
 * Arguments     : -
 * Return Value  : -
 *****************************************************************************/
void R_BCD_LcdClearGraphicsBuffer(void);

#ifdef __cplusplus
}
#endif

#endif  /* R_BCD_LCD_H */

/* End of File */
