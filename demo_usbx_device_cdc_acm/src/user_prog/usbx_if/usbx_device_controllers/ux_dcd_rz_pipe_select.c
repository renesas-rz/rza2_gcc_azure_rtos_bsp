/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   RZ Controller Driver                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_rz.h"
#include "ux_device_stack.h"

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                 RELEASE      */
/*                                                                        */
/*    _ux_dcd_rz_pipe_select                                PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function selects a pipe                                        */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rz                              Pointer to device controller    */
/*    ed                                  Pointer to endpoint             */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rz_register_read            Read register                   */ 
/*    _ux_dcd_rz_register_write           Write register                  */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    RZ Controller Driver                                                */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  06-01-2014     TCRG                     Initial Version 5.7           */ 
/*  06-01-2017     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.8    */ 
/*                                                                        */ 
/**************************************************************************/
UINT  _ux_dcd_rz_pipe_select(UX_DCD_RZ *dcd_rz, ULONG ed_index)
{

ULONG volatile delay_loop;

    /* Set PIPEx FIFO in in status. */
    _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_PIPESEL, ed_index);

    /* Check if Pipe selected.  */
    for( delay_loop = 0; delay_loop < 4; delay_loop++ )
    {
    
        /* Check pie selection.  */
        if( (_ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_PIPESEL) & 0x0F) == ed_index)
            break;
    }

    /* Cautions !!!
     * Depending on the external bus speed of CPU, you may need to wait for 450ns here.
     * For details, please look at the data sheet.     */
    delay_loop = 100;
    while(delay_loop-- > 0);

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

