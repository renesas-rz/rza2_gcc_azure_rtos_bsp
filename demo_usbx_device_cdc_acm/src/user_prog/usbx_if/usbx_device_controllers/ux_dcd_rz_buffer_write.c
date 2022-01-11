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
#include "ux_system.h"
#include "ux_utility.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _ux_dcd_rz_buffer_write                             PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function writes a buffer to the specified PIPE                 */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    dcd_rz                              Pointer to RX controller        */ 
/*    ed                                    Register to the ed            */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                                              */ 
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
UINT  _ux_dcd_rz_buffer_write(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed)
{

UINT    status;

    /* Select the fifo.  */
    switch (ed -> ux_dcd_rz_ed_fifo_index)
    {

        case UX_RZ_DCD_FIFO_D0        :
        case UX_RZ_DCD_FIFO_D1        :
        
            /* Write to the FIFO D0 or D1.  */
            status = _ux_dcd_rz_fifod_write(dcd_rz, ed);
            break;

        case UX_RZ_DCD_FIFO_C         :
        
            /* Write to the FIFO C1.  */
            status = _ux_dcd_rz_fifoc_write(dcd_rz, ed);
            break;

        default                        :
            break;

    }

    /* Analyze return status.  */
    switch (status)
    {
    
        case UX_RZ_DCD_FIFO_WRITING        :
        
            /* Enable the Ready interrupt.  */
            _ux_dcd_rz_buffer_ready_interrupt(dcd_rz, ed, UX_DCD_RZ_ENABLE);
            break;        

        case UX_RZ_DCD_FIFO_WRITE_END        :
        case UX_RZ_DCD_FIFO_WRITE_SHORT    :

            /* Disable the Ready interrupt.  */
            _ux_dcd_rz_buffer_ready_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
            
            /* Enable the Buffer empty interrupt.  */
           _ux_dcd_rz_buffer_empty_interrupt(dcd_rz, ed, UX_DCD_RZ_ENABLE);
            
            break;        
        
        case UX_RZ_DCD_FIFO_WRITE_DMA        :
            break;
            
        case UX_RZ_DCD_FIFO_WRITE_ERROR    :
        default                             :
        
            /* Disable the Ready interrupt.  */
            _ux_dcd_rz_buffer_ready_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
            
            /* Disable the Buffer empty interrupt.  */
            _ux_dcd_rz_buffer_empty_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
            
            break;        

    }

    /* Return to caller. */
    return(status);
}

