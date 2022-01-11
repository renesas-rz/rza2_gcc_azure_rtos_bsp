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
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_rz_endpoint_create                          PORTABLE C      */
/*                                                           5.8          */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will create a physical endpoint.                      */
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
/*    _ux_dcd_rz_endpoint_reset           Reset endpoint                  */ 
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
UINT  _ux_dcd_rz_endpoint_create(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RZ_ED        *ed;
ULONG               rz_endpoint_index;
ULONG               rz_endpoint_mask;
ULONG               pipe_index;
ULONG               buffer_index;
ULONG               current_pipe_index;
ULONG               endpoint_direction;
ULONG               endpoint_type;
ULONG               pipe_type;

TX_INTERRUPT_SAVE_AREA

    /* The endpoint index in the array of the RZ must match the endpoint number. 
       The RZ has 15 endpoints maximum.  */
    rz_endpoint_index =  endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & ~UX_ENDPOINT_DIRECTION;
    
    /* Fetch the address of the physical endpoint.  */
    ed =  &dcd_rz -> ux_dcd_rz_ed[rz_endpoint_index];

    /* Check the endpoint status, if it is free, reserve it. If not reject this endpoint.  */
    if ((ed -> ux_dcd_rz_ed_status & UX_DCD_RZ_ED_STATUS_USED) == 0)
    {
        
        /* We can use this endpoint.  */
        ed -> ux_dcd_rz_ed_status |=  UX_DCD_RZ_ED_STATUS_USED;

        /* Keep the physical endpoint address in the endpoint container.  */
        endpoint -> ux_slave_endpoint_ed =  (VOID *) ed;

        /* Save the endpoint pointer.  */
        ed -> ux_dcd_rz_ed_endpoint =  endpoint;
        
        /* Build the endpoint mask from the endpoint descriptor.  */
        rz_endpoint_mask =  endpoint -> ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE;
        
        /* Set the endpoint type and direction.  */
        switch (rz_endpoint_mask)
        {

        case UX_CONTROL_ENDPOINT:

            /* Control pipe index is always 0.  */
            pipe_index = 0;
            ed -> ux_dcd_rz_ed_buffer_number =  0;
            ed -> ux_dcd_rz_ed_buffer_size =  (( endpoint ->  ux_slave_endpoint_descriptor.wMaxPacketSize) / 64);

            /* Control endpoint is idle.  */
            ed -> ux_dcd_rz_ed_state =  0;

            /* For Control pipes, use FIFO_C.  */
            ed -> ux_dcd_rz_ed_fifo_index =  UX_RZ_DCD_FIFO_C; 

            break;                

        case UX_BULK_ENDPOINT:

            /* Obtain the first available pipe. */
            current_pipe_index = 1;
            pipe_index = 0;
            
            while( ( current_pipe_index <= 15 ) && ( pipe_index ==  0 ) )
            {
            
                /* Select pipe 1 - 15.  */
                _ux_dcd_rz_pipe_select(dcd_rz, current_pipe_index);
               
                /* Check whether pipe is in use.  */
                pipe_type =  dcd_rz -> ux_dcd_rz_pipe[current_pipe_index];
            
                /* Not in use, choose this one.  */
                if (pipe_type == 0)
                {
                
                    /* This is the pipe index to use.  */
                    pipe_index = current_pipe_index;  
                    
                    /* Memorise the pipe associated with the endpoint.  */
                    dcd_rz -> ux_dcd_rz_pipe[current_pipe_index] = rz_endpoint_index;
                   
                }                    
                    
                /* Increment the current index.  */
                current_pipe_index++;
            }

            /* Set the endpoint type to BULK.  */
            endpoint_type =  UX_RZ_DCD_PIPECFG_TYPE_BULK;

            /* Configure the  endpoint  hardware, pick up buffer size & number. */
            ed -> ux_dcd_rz_ed_buffer_number =  pipe_index * (UX_DCD_RZ_MAX_BULK_PAYLOAD / UX_DCD_RZ_MAX_BUF_SIZE);
            ed -> ux_dcd_rz_ed_buffer_size =  UX_DCD_RZ_MAX_BULK_PAYLOAD / UX_DCD_RZ_MAX_BUF_SIZE - 1;

#ifdef UX_DCD_RZA2
/* RZ/A2x */
            /* For Bulk pipes, use FIFO_C.  */
            ed -> ux_dcd_rz_ed_fifo_index =  UX_RZ_DCD_FIFO_C; 
#else
            /* For Bulk pipes, use FIFO_D0.  */
            ed -> ux_dcd_rz_ed_fifo_index =  UX_RZ_DCD_FIFO_D0;
#endif

            break;

        case UX_INTERRUPT_ENDPOINT:

            /* Set first buffer number. */
            buffer_index = 4;
            pipe_index = 0;
            
            /* Browse Pipes from 6 to 9 which can be interrupt.  */
            for (current_pipe_index = 6; current_pipe_index < 11; current_pipe_index++)
            {
            
                /* Select pipe 6 - 9.  */
                _ux_dcd_rz_pipe_select(dcd_rz, current_pipe_index);
               
                /* Check whether pipe is in use.  */
                pipe_type =  dcd_rz -> ux_dcd_rz_pipe[current_pipe_index];
            
                /* Not in use, choose this one.  */
                if (pipe_type == 0)
                {
        
                    /* Set the pipe index.  */
                    pipe_index = current_pipe_index;
        
                    /* Memorise the pipe associated with the endpoint.  */
                    dcd_rz -> ux_dcd_rz_pipe[current_pipe_index] = rz_endpoint_index;
                   
                    /* We are done with pipe selection.  */
                    break;
                }            
        
                /* Next buffer.  */
                buffer_index++;
        
            }

            /* Set the endpoint type to Interrupt.  */
            endpoint_type =  UX_RZ_DCD_PIPECFG_TYPE_INTERRUPT;

            /* Configure the  endpoint  hardware, pick up buffer size. Number is fixed. */
            ed -> ux_dcd_rz_ed_buffer_number =  buffer_index;
            ed -> ux_dcd_rz_ed_buffer_size =  0;

#ifdef UX_DCD_RZA2
/* RZ/A2x */
            /* For Bulk pipes, use FIFO_C.  */
            ed -> ux_dcd_rz_ed_fifo_index =  UX_RZ_DCD_FIFO_C; 
#else
            /* For Bulk pipes, use FIFO_D0.  */
            ed -> ux_dcd_rz_ed_fifo_index =  UX_RZ_DCD_FIFO_D0;
#endif

            break;                

        default:

            return(UX_ERROR);
        }

        /* Memorise the endpoint index.  */
        ed -> ux_dcd_rz_ed_index =  pipe_index;

        /* Continue the configuration for Bulk and Interrupt only.  */
        if (rz_endpoint_mask !=  UX_CONTROL_ENDPOINT)
        {

            /* Check if IN or OUT.  */
            if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & UX_ENDPOINT_DIRECTION) == UX_ENDPOINT_IN)
            
                /* Set the direction to OUT.  */
                endpoint_direction = UX_RZ_DCD_PIPECFG_DIR;
             
            else
                    
                /* Reset the endpoint direction as bit 4 of PIPECFG is 0 for IN.  */
                endpoint_direction = 0;
                
            /* Set PID to NAK. */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + (ed -> ux_dcd_rz_ed_index * 2), UX_RZ_DCD_DCPCTR_PIDNAK);
             
            /* Critical section, even ISR cannot be disrupted.  */
            TX_DISABLE
        
            /* Select the endpoint register to map all registers.  */
            _ux_dcd_rz_pipe_select(dcd_rz, ed -> ux_dcd_rz_ed_index);
        
            /* Set Pipe configuration registers.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_PIPECFG, (endpoint_type | UX_RZ_DCD_PIPECFG_SHTNAK | endpoint_direction | endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & 0xF));
        
            /* Set the endpoint buffer info.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_PIPEBUF, (((ed -> ux_dcd_rz_ed_buffer_size) << UX_RZ_DCD_PIPEBUF_SHIFT) | (ed -> ux_dcd_rz_ed_buffer_number)));
    
            /* Inititalize the PIPEMAX register.  */
            _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_PIPEMAXP,  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize);
        
            /* Free the INTs now.  */
            TX_RESTORE
        
        }
        
        /* Reset the endpoint.  */
        _ux_dcd_rz_endpoint_reset(dcd_rz, endpoint);
        
        /* Return successful completion.  */
        return(UX_SUCCESS);         
    }


    /* The endpoint is already used.  */
    return(UX_NO_ED_AVAILABLE);
    
}

