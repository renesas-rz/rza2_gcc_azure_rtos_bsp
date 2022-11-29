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
* File        : jcu_user.c
* Description : JPEG Codec Unit (JCU)  driver. User defined funcions.
* Version     : 1.12
******************************************************************************/


/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdio.h>
#include  "r_typedefs.h"
#include  "iodefine.h"
#include  "iobitmask.h"
#include  "r_register_bit.h"
#include  "rz_co.h"
#include  "r_jcu.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Defines: Macros in this file scope. Global static defines (GS)
************************************************************************/
#define  GS_N  "\r\n"


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


/**************************************************************************//**
* Function Name: R_JCU_PrintRegisters
* @brief
* @retval        None
******************************************************************************/
void  R_JCU_PrintRegisters() 
{
    volatile uint8_t*  array; 
    const uint8_t   no_shift = IOREG_NONSHIFT_ACCESS; 
    const uint32_t  no_mask  = IOREG_NONMASK_ACCESS; 

    #ifdef __GNUC__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wformat="
    #endif

    printf( ""GS_N );
    printf( "JCU Registers"GS_N );/* Cast to an appropriate type */
    printf( "JCMOD  = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCMOD  ) );/* Cast to an appropriate type */
    printf( "JCCMD  = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCCMD  ) );/* Cast to an appropriate type */
    printf( "JCQTN  = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCQTN  ) );/* Cast to an appropriate type */
    printf( "JCHTN  = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCHTN  ) );/* Cast to an appropriate type */
    printf( "JCDRIU = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDRIU ) );/* Cast to an appropriate type */
    printf( "JCDRID = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDRID ) );/* Cast to an appropriate type */
    printf( "JCVSZU = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCVSZU ) );/* Cast to an appropriate type */
    printf( "JCVSZD = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCVSZD ) );/* Cast to an appropriate type */
    printf( "JCHSZU = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCHSZU ) );/* Cast to an appropriate type */
    printf( "JCHSZD = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCHSZD ) );/* Cast to an appropriate type */
    printf( "JCDTCU = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDTCU ) );/* Cast to an appropriate type */
    printf( "JCDTCM = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDTCM ) );/* Cast to an appropriate type */
    printf( "JCDTCD = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDTCD ) );/* Cast to an appropriate type */
    printf( "JINTE0 = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JINTE0 ) );/* Cast to an appropriate type */
    printf( "JINTS0 = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JINTS0 ) );/* Cast to an appropriate type */
    printf( "JCDERR = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCDERR ) );/* Cast to an appropriate type */
    printf( "JCRST  = 0x%02X"GS_N,  R_REGISTER_READ8( &JCU.JCRST  ) );/* Cast to an appropriate type */
    printf( "JIFECNT   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFECNT   ) );/* Cast to an appropriate type */
    printf( "JIFESA    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFESA    ) );/* Cast to an appropriate type */
    printf( "JIFESOFST = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFESOFST ) );/* Cast to an appropriate type */
    printf( "JIFEDA    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFEDA    ) );/* Cast to an appropriate type */
    printf( "JIFESLC   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFESLC   ) );/* Cast to an appropriate type */
    printf( "JIFEDDC   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFEDDC   ) );/* Cast to an appropriate type */
    printf( "JIFDCNT   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDCNT   ) );/* Cast to an appropriate type */
    printf( "JIFDSA    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDSA    ) );/* Cast to an appropriate type */
    printf( "JIFDDOFST = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDDOFST ) );/* Cast to an appropriate type */
    printf( "JIFDDA    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDDA    ) );/* Cast to an appropriate type */
    printf( "JIFDSDC   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDSDC   ) );/* Cast to an appropriate type */
    printf( "JIFDDLC   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDDLC   ) );/* Cast to an appropriate type */
    printf( "JIFDADT   = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFDADT   ) );/* Cast to an appropriate type */
    printf( "JINTE1    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JINTE1    ) );/* Cast to an appropriate type */
    printf( "JINTS1    = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JINTS1    ) );/* Cast to an appropriate type */
    printf( "JIFESVSZ  = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFESVSZ  ) );/* Cast to an appropriate type */
    printf( "JIFESHSZ  = 0x%08X"GS_N,  R_REGISTER_READ32( &JCU.JIFESHSZ  ) );/* Cast to an appropriate type */

    /* Cast to an appropriate type */
    array = &JCU.JCQTBL0;
    printf( "JCQTBL0[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "JCQTBL0[1] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[1],  no_shift,  no_mask ) );
    printf( "JCQTBL0[2] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[2],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCQTBL1;
    printf( "JCQTBL1[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "JCQTBL1[1] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[1],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCQTBL2;
    printf( "JCQTBL2[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCQTBL3;
    printf( "JCQTBL3[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCHTBD0;
    printf( "JCHTBD0[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCHTBA0;
    printf( "JCHTBA0[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCHTBD1;
    printf( "JCHTBD1[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    /* Cast to an appropriate type */
    array = &JCU.JCHTBA1;
    printf( "JCHTBA1[0] = 0x%02X"GS_N,  RZA_IO_RegRead_8( &array[0],  no_shift,  no_mask ) );
    printf( "        :"GS_N );

    #ifdef __GNUC__
    #pragma GCC diagnostic pop
    #endif
}/* End of function R_JCU_PrintRegisters */

