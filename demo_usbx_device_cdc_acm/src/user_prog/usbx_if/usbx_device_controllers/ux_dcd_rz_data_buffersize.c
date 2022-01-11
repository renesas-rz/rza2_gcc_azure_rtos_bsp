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


/* Include necessary system files.  */

#define UX_SOURCE_CODE

#include "ux_api.h"
#include "ux_dcd_rz.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rz_data_buffer_size                         PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function returns the size of the buffer data                   */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rz                           Pointer to RX controller           */ 
/*    ed                                 Register to the ed               */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    buffer size                                                         */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
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
ULONG  _ux_dcd_rz_data_buffer_size(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed)
{

ULONG       buffer_size;

    /* First get the pipe umber. If this is the control endpoint, we
       treat it differently. */
    if (ed -> ux_dcd_rz_ed_index == 0)
    {
        
        /* Use the endpoint max packet size.  */
        buffer_size = ed -> ux_dcd_rz_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

    }
    else
    {

        /* Select the pipe.  */
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_PIPESEL, UX_RZ_DCD_PIPESEL_NO_PIPE); 
        _ux_dcd_rz_pipe_select(dcd_rz, ed -> ux_dcd_rz_ed_index); 

        /* Use the endpoint max packet size.  */
        buffer_size = ed -> ux_dcd_rz_ed_endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;
        
    }

    /* Return the buffer size.  */
    return(buffer_size);
}

