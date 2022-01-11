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
/*    _ux_dcd_rz_endpoint_destroy                           PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will destroy a physical endpoint.                     */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rz                              Pointer to device controller    */
/*    endpoint                              Pointer to endpoint container */
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
UINT  _ux_dcd_rz_endpoint_destroy(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RZ_ED        *ed;

    /* Keep the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_RZ_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Set PIPEx FIFO in in status. */
    _ux_dcd_rz_pipe_select(dcd_rz, ed -> ux_dcd_rz_ed_index);

    /* Check the type of endpoint.  */
    if (ed -> ux_dcd_rz_ed_index != 0)
    {

        /* Reset the pipe used here.  */
        dcd_rz -> ux_dcd_rz_pipe[ed -> ux_dcd_rz_ed_index] = 0;
        
        /* Set PID to NAK for non Control Endpoints.  */
        _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2), UX_RZ_DCD_PIPECTR_PIDNAK);

        /* Clear Pipe Ctr.  */
        _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1TRE + ((ed -> ux_dcd_rz_ed_index -1) * 4), UX_RZ_DCD_PIPE1TRE_TRCLR);

        /* Clear the FIFO buffer memory. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);

        /* Clear the enabled interrupts.  */
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_BEMPSTS, (1 << ed -> ux_dcd_rz_ed_index));
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_BRDYSTS, (1 << ed -> ux_dcd_rz_ed_index));
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_NRDYSTS, (1 << ed -> ux_dcd_rz_ed_index));

        /* Disable the Ready interrupt.  */
        _ux_dcd_rz_buffer_ready_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
        
        /* Disable the Buffer empty interrupt.  */
        _ux_dcd_rz_buffer_empty_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
            
        /* Disable the Buffer Not Ready interrupt.  */
        _ux_dcd_rz_buffer_notready_interrupt(dcd_rz, ed, UX_DCD_RZ_DISABLE);
            
        /* Select Pipe 0. */
        _ux_dcd_rz_pipe_select(dcd_rz, 0);

        /* Reset the toggle bits. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 4), (UX_RZ_DCD_PIPECTR_CSCLR | UX_RZ_DCD_PIPECTR_SQCLR));

        /* Set ACRLM.  */
        _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 4), UX_RZ_DCD_PIPECTR_ACLRM);

        /* Reset ACRLM.  */
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 4), UX_RZ_DCD_PIPECTR_ACLRM);

    }
    else
    {

        /* Set PID to NAK for Control Endpoints.  */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_DCPCTR_PIDNAK);
        
        /* Reset the toggle bits. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_DCPCTR, (UX_RZ_DCD_DCPCTR_CSCLR | UX_RZ_DCD_DCPCTR_SUREQCLR | UX_RZ_DCD_DCPCTR_SQCLR));
        
        /* Clear the FIFO buffer memory. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);
        

    }

    /* We can free this endpoint.  */
    ed -> ux_dcd_rz_ed_status =  UX_DCD_RZ_ED_STATUS_UNUSED;

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

