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
#include "ux_utility.h"
#include "ux_device_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                RELEASE       */
/*                                                                        */
/*    _ux_dcd_rz_transfer_request                          PORTABLE C     */
/*                                                            5.8         */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will initiate a transfer to a specific endpoint.      */
/*    If the endpoint is IN, the endpoint register will be set to accept  */
/*    the request.                                                        */
/*                                                                        */
/*    If the endpoint is IN, the endpoint FIFO will be filled with the    */
/*    buffer and the endpoint register set.                               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_rz                              Pointer to device controller    */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rz_register_read            Read register                   */ 
/*    _ux_dcd_rz_register_set             Set register                    */ 
/*    _ux_dcd_rz_register_write           Write register                  */ 
/*    _ux_utility_semaphore_get           Get semaphore                   */ 
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
/*                                                                        */ 
/**************************************************************************/
UINT  _ux_dcd_rz_transfer_request(UX_DCD_RZ *dcd_rz, UX_SLAVE_TRANSFER *transfer_request)
{

UX_DCD_RZ_ED            *ed;
UCHAR                   *fifo_buffer;
ULONG                   fifo_length;
UX_SLAVE_ENDPOINT       *endpoint;
UINT                    status; 
ULONG                   pipectr_reg;                           


    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;

    /* Get the physical endpoint from the logical endpoint.  */
    ed =  (UX_DCD_RZ_ED *) endpoint -> ux_slave_endpoint_ed;
    
    /* Set PIPEx FIFO in in status. */
    _ux_dcd_rz_pipe_select(dcd_rz, ed -> ux_dcd_rz_ed_index);

#ifdef UX_RZ_DCD_USE_DMA
    /* Check if DMA is not idle.  */
    if (dcd_rz -> ux_dcd_rz_dma_status != UX_RZ_DMA_STATUS_IDLE)
    {
        
        /* Check if DMA is still enabled.  */
        if ((_ux_dcd_rz_dma_register_read(dcd_rz, UX_RZ_DMA_CHSTAT(UX_RZ_DMA_TX_CH)) & UX_RZ_DMA_CHSTAT_EN) != 0)
        {
            
            /* Wait until transfer is completed.  */
            while((_ux_dcd_rz_dma_register_read(dcd_rz, UX_RZ_DMA_CHSTAT(UX_RZ_DMA_TX_CH)) & UX_RZ_DMA_CHSTAT_EN) != 0);
        }
    }
#endif
    
    /* Set PID to NAK. Isolate Control endpoint and others.  */
    if (ed -> ux_dcd_rz_ed_index == 0)
    
        /* Set PID to NAK for Control Endpoint.  */
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_PIPECTR_PID_MASK);

    else
    {
        pipectr_reg = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2));
        while(pipectr_reg & UX_RZ_DCD_DCPCTR_SUREQ )
                pipectr_reg = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2));

        /* Set PID to NAK for non Control Endpoints.  */
        _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2), UX_RZ_DCD_PIPECTR_PID_MASK);
        
    }

    /* Clear the BRDY and BEMP status for this pipe.  */
    _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_BEMPSTS, (1 << ed -> ux_dcd_rz_ed_index));
    _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_BRDYSTS, (1 << ed -> ux_dcd_rz_ed_index));

    /* Check for transfer direction.  */
    if (transfer_request -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    {

        /* Data Out. So we expect a IN transaction from the host.  */
        /* Get the size of the transfer, used for a IN transaction only.  */
        fifo_length =  transfer_request -> ux_slave_transfer_request_requested_length;

        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rz_ed_payload_length =  fifo_length;

        /* Check if the endpoint size is bigger that data requested. */
        if(fifo_length > endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize)

            /* Adjust the transfer size.  */
            fifo_length =  endpoint -> ux_slave_endpoint_descriptor.wMaxPacketSize;

        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rz_ed_payload_address =  fifo_buffer;

        /* Clear the FIFO buffer memory. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);

        /* Point the FIFO buffer to the current transfer request buffer address.  */
        fifo_buffer =  transfer_request -> ux_slave_transfer_request_data_pointer;
        
        /* Keep the FIFO length in the endpoint.  */
        ed -> ux_dcd_rz_ed_payload_address =  fifo_buffer;
        
        /* Write the buffer to the pipe.  */
        status =  _ux_dcd_rz_buffer_write(dcd_rz, ed); 
        
        /* Check status.  */
        if (status != UX_ERROR) 
        {        

            /* Adjust the FIFO length in the endpoint.  */
            ed -> ux_dcd_rz_ed_payload_length -=  fifo_length;

            /* Adjust the data pointer.  */
            transfer_request -> ux_slave_transfer_request_current_data_pointer += fifo_length;
            
            /* Update the length of the data transmitted.  */
            transfer_request -> ux_slave_transfer_request_actual_length += fifo_length;
              
            /* Adjust the transfer length remaining.  */
            transfer_request -> ux_slave_transfer_request_in_transfer_length -= fifo_length;
        
            /* Set PID to BUF. Isolate Control endpoint and others.  */
            if (ed -> ux_dcd_rz_ed_index == 0)
        
                /* Set PID to BUF for Control Endpoint.  */
                _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_PIPECTR_PIDBUF);
    
            else
        
                /* Set PID to BUF for non Control Endpoints.  */
                _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2), UX_RZ_DCD_PIPECTR_PIDBUF);
        }
                
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_rz_ed_index != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
    
            /* Check the completion code. */
            if (status != UX_SUCCESS)
            {
                return(status);
            }
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return(transfer_request -> ux_slave_transfer_request_completion_code);
    
            /* Return to caller with success.  */
            return(UX_SUCCESS);

        }
    }
    else
    {

        /* We have a request for a OUT transaction from the host.  */
        /* Clear the FIFO buffer memory. */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_CFIFOCTR, UX_RZ_DCD_FIFOCTR_BCLR);

        /* Set the current endpoint fifo.  */
        _ux_dcd_rz_current_endpoint_change(dcd_rz, ed, 0);
    
        /* Enable the BRDY interrupt.  */
        _ux_dcd_rz_buffer_ready_interrupt(dcd_rz, ed, UX_DCD_RZ_ENABLE);
           
        /* Set PID to BUF. Isolate Control endpoint and others.  */
        if (ed -> ux_dcd_rz_ed_index == 0)
    
            /* Set PID to BUF for Control Endpoint.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_PIPECTR_PIDBUF);

        else
    
            /* Set PID to BUF for non Control Endpoints.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2), UX_RZ_DCD_PIPECTR_PIDBUF);
    
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if (ed -> ux_dcd_rz_ed_index != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore, UX_WAIT_FOREVER);
                
            /* Check the completion code. */
            if (status != UX_SUCCESS)
            {

                return(status);
            }
            
            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
                return(transfer_request -> ux_slave_transfer_request_completion_code);
        
            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }

    /* Return to caller with success.  */
    return(UX_SUCCESS);
}

