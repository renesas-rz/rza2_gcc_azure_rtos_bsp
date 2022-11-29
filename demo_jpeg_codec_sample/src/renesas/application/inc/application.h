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
* File: application.h
*    Application.
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  "r_typedefs.h"
#include  "rz_co.h"

#ifndef  APPLICATION_H
#define  APPLICATION_H

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
 * Function Name: R_S_WindowsSurfaces
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_WindowsSurfaces(void); 

/******************************************************************************
 * Function Name: R_S_UserFrameBuffer
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_UserFrameBuffer(void); 

/******************************************************************************
 * Function Name: R_S_Image
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Image(void); 

/******************************************************************************
 * Function Name: R_S_Image_Compatible
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Image_Compatible(void);

/******************************************************************************
 * Function Name: R_S_Blend
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Blend(void); 

/******************************************************************************
 * Function Name: R_S_RenderToTexture_ByRGA
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_RenderToTexture_ByRGA(void);

/******************************************************************************
 * Function Name: R_S_RenderToTexture_ByApplication
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_RenderToTexture_ByApplication(void);

/******************************************************************************
 * Function Name: R_S_FadeInOutDissolve
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_FadeInOutDissolve(void); 

/******************************************************************************
 * Function Name: R_S_PanScroll
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_PanScroll(void); 

/******************************************************************************
 * Function Name: R_S_Zoom
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Zoom(void); 

/******************************************************************************
 * Function Name: R_S_Rotation
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Rotation(void); 

/******************************************************************************
 * Function Name: R_S_Rotation_Compatible
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Rotation_Compatible(void);

/******************************************************************************
 * Function Name: R_S_Rotation_Clip
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Rotation_Clip(void);

/******************************************************************************
 * Function Name: R_S_GlassReflection
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_GlassReflection(void); 

/******************************************************************************
 * Function Name: R_S_AcceleratedMotion
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_AcceleratedMotion(void); 

/******************************************************************************
 * Function Name: R_S_Text_ByImagePackager
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Text_ByImagePackager(void);

/******************************************************************************
 * Function Name: R_S_Text_ByToolOutputROM
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Text_ByToolOutputROM(void);

/******************************************************************************
 * Function Name: R_S_Text_ByApplication
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
errnum_t  R_S_Text_ByApplication(void);




#endif /* #define  APPLICATION_H */
