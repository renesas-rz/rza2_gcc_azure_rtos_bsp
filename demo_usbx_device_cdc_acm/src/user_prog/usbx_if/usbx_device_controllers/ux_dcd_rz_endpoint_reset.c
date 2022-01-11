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
/*    _ux_dcd_rz_endpoint_reset                             PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will reset a physical endpoint.                       */
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
UINT  _ux_dcd_rz_endpoint_reset(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RZ_ED      *ed;
                            
    /* Get the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_RZ_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Reset PID on this endpoint.  */
    switch (ed -> ux_dcd_rz_ed_index)
    {
    
        case 0 :
            
            /* Set NAK in DCPCTR register. This will reset the STALL condition.  */
            _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_DCPCTR_PID_MASK);

            /* Set SQCLR in PIPExCTR register. This will reset data toggle.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_DCPCTR_SQCLR);

            break;        
        
        default : 
            
            /* Set NAK in PIPExCTR register. This will reset the STALL condition.  */
            _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index - 1) * 2), UX_RZ_DCD_DCPCTR_PID_MASK);
            
            /* Set SQCLR in PIPExCTR register. This will reset data toggle.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index - 1) * 2), UX_RZ_DCD_DCPCTR_SQCLR);

               /* Clear the FIFO buffer memory. */
               _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);
             /* Clear the FIFO buffer memory. */
               _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);

            break;        
        
    }

    /* Set the state of the endpoint to not stalled.  */
    ed -> ux_dcd_rz_ed_status &=  ~UX_DCD_RZ_ED_STATUS_STALLED;

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

