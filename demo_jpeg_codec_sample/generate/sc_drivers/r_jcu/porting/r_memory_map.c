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
* File         : r_memory_map.c
* Description:
*    "gs_shared_memory_areas" variable must be changed by the setting of MMU.
* Version      : 1.12
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdint.h>
#include  "r_mmu_lld_rza2m.h"
#include  "rz_co.h"
#include  "r_memory_map.h"
#include  "r_mmu_drv_sc_cfg.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Defines: Macros in this file scope. Global static defines (GS)
************************************************************************/

/* Constant: GS_NOT_SHARE */
#define  GS_NOT_SHARE  ((uintptr_t)  1u  )

/* Constant: GS_TTB_ATTR_TEX_C_B
*    TEX, C and B bit in translation table entry (MMU) */
#define  GS_TTB_ATTR_TEX_C_B  (  0x0000700Cu  )

/* Constant: GS_PAGE_SIZE */
#define  GS_PAGE_SIZE  (  0x00100000u  )

/* Constant: GS_INDEX_ERROR */
#define  GS_INDEX_ERROR  (  -1  )


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
/* Section: Global */

/******************************************************************************
* Function Name: gs_ChangeToOtherVirtualAddress
*    Get the other virtual address sharing the physical address
*
* Arguments:
*   in_VirtualAddress - A virtual address
*
* Return Value:
*   The other virtual address  or  GS_NOT_SHARE
******************************************************************************/
static uintptr_t  gs_ChangeToOtherVirtualAddress 
        ( const volatile void* const  in_VirtualAddress )
{
    static const int_fast32_t  count_of_area = R_COUNT_OF( MMU_SC_TABLE );
    static const uint32_t      size_of_32bit_address_area = 0u;  /* 0x100000000 */

    /* Cast to an appropriate type */
    uintptr_t     input_virtual_address = (uintptr_t) in_VirtualAddress;

    /* Cast to an appropriate type */
    uintptr_t     other_virtual_address = GS_NOT_SHARE;
    uint32_t      physical_address;
    int_fast32_t  area_index;
    int_fast32_t  input_area_index = GS_INDEX_ERROR;


    /* physical_address = ... */
    for ( area_index = 0;  area_index < count_of_area;  area_index += 1 )
    {
        uintptr_t  start_virtual_address = MMU_SC_TABLE[ area_index ].virtual_address;

        if ( input_virtual_address >= start_virtual_address )
        {
            uint32_t   area_size = MMU_SC_TABLE[ area_index ].page_count * GS_PAGE_SIZE;
            uintptr_t  last_virtual_address = (start_virtual_address + area_size) - 1;

            if ( (input_virtual_address <= last_virtual_address)  &&  (area_size != size_of_32bit_address_area) )
            {
                uint32_t  offset = input_virtual_address - start_virtual_address;

                physical_address = MMU_SC_TABLE[ area_index ].physical_address + offset;
                input_area_index = area_index;
                break;
            }
        }
    }


    /* other_virtual_address = ... */
    if ( input_area_index != GS_INDEX_ERROR )
    {
        for ( area_index = 0;  area_index < count_of_area;  area_index += 1 )
        {
            uint32_t  start_physical_address = MMU_SC_TABLE[ area_index ].physical_address;

            if ( physical_address >= start_physical_address )
            {
                uint32_t   area_size = MMU_SC_TABLE[ area_index ].page_count * GS_PAGE_SIZE;
                uintptr_t  last_physical_address = (start_physical_address + area_size) - 1;

                if ( (physical_address <= last_physical_address)  &&  (area_size != size_of_32bit_address_area) )
                {

                    if ( area_index != input_area_index )
                    {
                        uint32_t  offset = physical_address - start_physical_address;

                        other_virtual_address = MMU_SC_TABLE[ area_index ].virtual_address + offset;
                        break;
                    }
                }
            }
        }
    }

    return  other_virtual_address;
} /* End of function gs_ChangeToOtherVirtualAddress */


/******************************************************************************
* Function Name: gs_IsCached
*    Returns whether there is the specified virtual address on the cache
*
* Arguments:
*   in_VirtualAddress - Cheking virtual address
*
* Return Value:
*   Whether the specified virtual address is on the cache
******************************************************************************/
static bool_t  gs_IsCached( const volatile void*  in_VirtualAddress ) 
{
    /* Cast to an appropriate type */
    uint32_t  table_entry_value = R_MMU_ReadTbl( (uint32_t) in_VirtualAddress );

    bool_t  is_cached =
        ( ( table_entry_value & GS_TTB_ATTR_TEX_C_B ) ==

            /* Cast to an appropriate type */
            ( MMU_ATTR_NORMAL_L1CACHE & GS_TTB_ATTR_TEX_C_B ) ) ||
        ( ( table_entry_value & ( GS_TTB_ATTR_TEX_C_B | MMU_ATTR_NORMAL_L1L2CACHE ) ) ==
            MMU_ATTR_NORMAL_L1L2CACHE );

    return  is_cached;
}/* End of function gs_IsCached */


/******************************************************************************
* Function Name: R_MEMORY_ToPhysicalAddress
******************************************************************************/
errnum_t  R_MEMORY_ToPhysicalAddress( const volatile void*  in_VirtualAddress, 
        uintptr_t*  out_PhysicalAddress )
{
    errnum_t     e;
    e_mmu_err_t  er;
    uint32_t     physical_address;

    /* Cast to an appropriate type */
    er= R_MMU_VAtoPA( (uint32_t)(uintptr_t) in_VirtualAddress,  &physical_address );  /* physical_address = . */
    *out_PhysicalAddress = (uintptr_t) physical_address;
    if ( er == MMU_SUCCESS )
    {
        e = 0;
    }
    else
    {
        e = E_MEMORY_ACCESS_DENIED_7;

            /* Error: Failed to get physical address from MMU. */
    }
    return  e;
} /* End of function R_MEMORY_ToPhysicalAddress */


/******************************************************************************
* Function Name: R_MEMORY_ToCachedAddress
******************************************************************************/
errnum_t  R_MEMORY_ToCachedAddress 
        ( const volatile void* const  in_VirtualAddress,  void* const  out_CachedAddress )
{
    bool_t    is_cached = gs_IsCached( in_VirtualAddress );
    errnum_t  e;

    if ( is_cached )
    {
        /* out_CachedAddress = ... */
        *(uintptr_t*) out_CachedAddress = (uintptr_t) in_VirtualAddress;

    }
    else
    {  /* if not "in_VirtualAddress" "is_cached" */
        uintptr_t  other_address = gs_ChangeToOtherVirtualAddress( in_VirtualAddress );

        /* Guard */
        {
            bool_t     is_other_address_cached_address;
            uintptr_t  physical_address_from_uncached;
            uintptr_t  physical_address_from_cached;

            /* (variables) = ... */
            {
                e= R_MEMORY_ToPhysicalAddress( in_VirtualAddress,  &physical_address_from_uncached );
                    if(e)
                    {
                        goto fin;
                    }  /* physical_address_from_uncached = . */
                e= R_MEMORY_ToPhysicalAddress( (void*) other_address,  &physical_address_from_cached );
                    if(e)
                    {
                        goto fin;
                    }  /* physical_address_from_cached = . */
                is_other_address_cached_address = gs_IsCached( (void*) other_address );
            }

            /* Guard */
            if ( other_address == GS_NOT_SHARE )
            {
                e = E_MEMORY_ACCESS_DENIED_1;  goto fin;
                    /* Error: The specified address is not in cached area or cannot change to cached area. */
            }
            if ( ! is_other_address_cached_address )
            {
                e = E_MEMORY_ACCESS_DENIED_3;  goto fin;
                    /* Error: Changed address is not in cached area. */
            }
            if ( physical_address_from_uncached != physical_address_from_cached )
            {
                e = E_MEMORY_ACCESS_DENIED_5;  goto fin;
                    /* Error: The specified physical address is not same between before and after. */
            }
        }

        /* out_CachedAddress = ... */
        *(uintptr_t*) out_CachedAddress = other_address;
    }

    e=0;
fin:
    {
        return e;
    }
} /* End of function R_MEMORY_ToCachedAddress */


/******************************************************************************
* Function Name: R_MEMORY_ToUncachedAddress
******************************************************************************/
errnum_t  R_MEMORY_ToUncachedAddress
        ( const volatile void* const  in_VirtualAddress,void* const  out_UncachedAddress )
{
    bool_t    is_cached = gs_IsCached( in_VirtualAddress );
    errnum_t  e;

    if ( ! is_cached )
    {
        /* out_UncachedAddress = ... */
        *(uintptr_t*) out_UncachedAddress = (uintptr_t) in_VirtualAddress;

    }
    else
    {  /* if "in_VirtualAddress" "is_cached" */
        uintptr_t  other_address = gs_ChangeToOtherVirtualAddress( in_VirtualAddress );

        /* Guard */
        {
            bool_t     is_other_address_cached_address;
            uintptr_t  physical_address_from_uncached;
            uintptr_t  physical_address_from_cached;

            /* (variables) = ... */
            {
                /* Cast to an appropriate type */
                e= R_MEMORY_ToPhysicalAddress( (void*) other_address,  &physical_address_from_uncached );
                    if(e)
                    {
                        goto fin;
                    }  /* physical_address_from_uncached = . */
                e= R_MEMORY_ToPhysicalAddress( in_VirtualAddress,  &physical_address_from_cached );
                    if(e)
                    {
                        goto fin;
                    }  /* physical_address_from_cached = . */
                is_other_address_cached_address = gs_IsCached( (void*) other_address );
            }

            /* Guard */
            if ( other_address == GS_NOT_SHARE )
            {
                e = E_MEMORY_ACCESS_DENIED_2;
                goto fin;

                /* Error: The specified address is not in uncached area or cannot change to uncached area. */
            }
            if ( is_other_address_cached_address )
            {
                e = E_MEMORY_ACCESS_DENIED_4;  goto fin;

                /* Error: Changed address is not in uncached area. */
            }
            if ( physical_address_from_uncached != physical_address_from_cached )
            {
                e = E_MEMORY_ACCESS_DENIED_6;  goto fin;

                /* Error: The specified physical address is not same between before and after. */
            }
        }

        /* out_UncachedAddress = ... */
        *(uintptr_t*) out_UncachedAddress = other_address;
    }

    e=0;
fin:
    {
        return e;
    }
}/* End of function R_MEMORY_ToUncachedAddress */
