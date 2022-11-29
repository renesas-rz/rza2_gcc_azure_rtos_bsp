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
/***********************************************************************
* File         : r_memory_map.h
* Description  : Mempry Map.
* Version      : 1.12
************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdint.h>
#include  "r_typedefs.h"
#include  "rz_co_typedef.h"

#ifndef R_MEMORY_MAP_H
#define R_MEMORY_MAP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************************
Macro definitions
******************************************************************************/
/* In "rz_co_app_typedef.h" */

/******************************************************************************
Typedef definitions
******************************************************************************/
/* In "rz_co_app_typedef.h" */

/******************************************************************************
Variable Externs
******************************************************************************/
/* In "rz_co_app_typedef.h" */

/******************************************************************************
Functions Prototypes
******************************************************************************/

/***********************************************************************
* Function Name: R_MEMORY_ToPhysicalAddress
*    Changes to physical address
*
* Arguments:
*    in_Address          - Virtual address
*    out_PhysicalAddress - Output: Physical address
*
* Return Value:
*    Error code.  If there is no error, the return value is 0.
*
* Example:
*    // Case that physical address is integer type:
*    uintptr_t  physical_address;
*    e= R_DRW_ToPhysicalAddress( address, &physical_address ); if(e){goto fin;}
*
*    // Case that physical address is pointer type:
*    uintptr_t  physical_address;
*    void*  pointer;
*    e= R_DRW_ToPhysicalAddress( address, &physical_address ); if(e){goto fin;}
*    pointer = (void*) physical_address;
*
* Description:
*    The define of this function must be modified to same setting with MMU setting.
************************************************************************/
errnum_t  R_MEMORY_ToPhysicalAddress 
    ( const volatile void* const  in_Address,  uintptr_t* const  out_PhysicalAddress );


/***********************************************************************
* Function Name: R_MEMORY_ToCachedAddress
*    Checks if the address is in the L1 cache area or changes to L1 cache area
*
* Arguments:
*    in_Address        - Virtual address
*    out_CachedAddress - Output: Virtual address for cached area
*
* Return Value:
*    Error code.  If there is no error, the return value is 0.
*
* Description:
*    The define of this function must be modified to same setting with MMU setting.
*    "out_CachedAddress" argument is passed the address of pointer.
*    If the target environment did not have mirror area and "in_Address"
*    argument was passed uncached address, an error is raised.
*    If an error was raised, you may know the variable by
*    looking at value of "in_Address" argument and map file.
*    If the environment did not have any mirror area, it is not necessary
*    to call the function to change the address. But, the function can be
*    used by checking whether passed address is in cached area or uncached area.
************************************************************************/
errnum_t  R_MEMORY_ToCachedAddress( const volatile void* const  in_Address,  void* const  out_CachedAddress );


/***********************************************************************
* Function Name: R_MEMORY_ToUncachedAddress
*    Checks if the address is in L1 uncached area or changes to L1 uncached area
*
* Arguments:
*    in_Address          - Virtual address
*    out_UncachedAddress - Output: Virtual address for uncached area
*
* Return Value:
*    Error code.  If there is no error, the return value is 0.
*
* Description:
*    The define of this function must be modified to same setting with MMU setting.
*    "out_UncachedAddress" argument is passed the address of pointer.
*    If the target environment did not have mirror area and "in_Address"
*    argument was passed cached address, an error is raised.
*    If an error was raised, you may know the variable by looking at value
*    of "in_Address" argument and map file.
*    If the environment did not have any mirror area, it is not necessary
*    to call the function to change the address. But, the function can be
*    used by checking whether passed address is in cached area or uncached area.
************************************************************************/
errnum_t  R_MEMORY_ToUncachedAddress( const volatile void* const  in_Address,  void* const  out_UncachedAddress );


/***********************************************************************
* Constants: E_MEMORY
*    Error code group of the MemoryMap.
************************************************************************/
/* Constant: E_MEMORY_MASK
*    Mask for checking E_MEMORY. */
#define  E_MEMORY            ( 0x00000000 )
#define  E_MEMORY_MASK       ( 0x00FF0000 )

/* Sub class of E_ACCESS_DENIED */
#define  E_MEMORY_ACCESS_DENIED_1    ( 0x07000001 )
#define  E_MEMORY_ACCESS_DENIED_2    ( 0x07000002 )
#define  E_MEMORY_ACCESS_DENIED_3    ( 0x07000003 )
#define  E_MEMORY_ACCESS_DENIED_4    ( 0x07000004 )
#define  E_MEMORY_ACCESS_DENIED_5    ( 0x07000005 )
#define  E_MEMORY_ACCESS_DENIED_6    ( 0x07000006 )
#define  E_MEMORY_ACCESS_DENIED_7    ( 0x07000007 )


/***********************************************************************
* End of File:
************************************************************************/
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

#endif /* R_MEMORY_MAP_H */
