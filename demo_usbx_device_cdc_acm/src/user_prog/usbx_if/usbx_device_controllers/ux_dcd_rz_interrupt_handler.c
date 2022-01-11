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
#include "ux_system.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                 RELEASE      */
/*                                                                        */
/*    _ux_dcd_rz_interrupt_handler                          PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function is the interrupt handler for the RZ controller.       */
/*    The controller will trigger an interrupt when something happens on  */
/*    an endpoint whose mask has been set in the interrupt enable         */
/*    register.                                                           */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rz_initialize_complete      Complete initialization         */ 
/*    _ux_dcd_rz_register_read            Read register                   */ 
/*    _ux_dcd_rz_register_write           Write register                  */ 
/*    _ux_dcd_rz_transfer_callback        Process callback                */ 
/*    _ux_device_stack_disconnect           Disconnect device             */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    USBX Device Stack                                                   */ 
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

VOID  _ux_dcd_rz_interrupt_handler(UINT int_sense)
{

ULONG                   rz_interrupt;
UX_SLAVE_TRANSFER       *transfer_request;
UX_DCD_RZ_ED            *ed;
UX_SLAVE_ENDPOINT       *endpoint;
UX_SLAVE_DCD            *dcd;
UX_DCD_RZ               *dcd_rz;
UX_SLAVE_DEVICE         *device;
ULONG                   rz_register1, rz_register2, rz_register3;
ULONG                   rz_dvstctr;
ULONG                   bempsts, nrdysts, brdysts;
ULONG                   bempenb, nrdyenb, brdyenb;
ULONG                   bemp_bit, nrdy_bit, brdy_bit;
ULONG                   pipe_index;
ULONG                   endpoint_index;
volatile ULONG          reset_check_register;
               

    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* Get the pointer to the RX DCD.  */
    dcd_rz = (UX_DCD_RZ *) dcd -> ux_slave_dcd_controller_hardware;

    /* Get the pointer to the device.  */
    device =  &_ux_system_slave -> ux_system_slave_device;

    /* Read the interrupt status register from the controller.  */
    rz_interrupt =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_INTSTS0);

    /* Check if we have an RESUME.  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_RESM)
    {        

        /* Check the status change callback.  */
        if(_ux_system_slave -> ux_system_slave_change_function != UX_NULL)
        {

            /* Inform the application if a callback function was programmed.  */
            _ux_system_slave -> ux_system_slave_change_function(UX_DEVICE_RESUMED);
        }

        /* Clear RESUME.  */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_INTSTS0, ~UX_RZ_DCD_INTSTS0_RESM);

    }

    /* Check the source of the interrupt. Is it VBUS transition ?  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_VBINT)
    {

        /* Clear VBUSINT.  */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_INTSTS0, ~UX_RZ_DCD_INTSTS0_VBINT);

        /* Check the state of VBUS. Attached or Detached.  */
        do {

            /* Read 3 times the current status and wait for all read to be equal to avoid jitter.  */
            rz_register1 = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_INTSTS0) & UX_RZ_DCD_INTSTS0_VBSTS;
            rz_register2 = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_INTSTS0) & UX_RZ_DCD_INTSTS0_VBSTS;
            rz_register3 = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_INTSTS0) & UX_RZ_DCD_INTSTS0_VBSTS;

        } while( ( rz_register1 != rz_register2 ) || ( rz_register1 != rz_register3 ) );

        /* VBUS is stable. Check if on/off.  */        
        if (rz_register1 & UX_RZ_DCD_INTSTS0_VBSTS)
        {
             
            /* Pull up D+ and open D- This will generate a RESET from the host.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_DPRPU);
        }

        else
        {
        
            /* Open D+ and D-  */
            _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_DPRPU);

            /* Device is disconnected.  */
            _ux_device_stack_disconnect();

        }        
    }

    /* Check the source of the interrupt. Is it Device State transition (DVST) ?  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_DVST)
    {

        /* Clear DVST.  */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_INTSTS0, ~UX_RZ_DCD_INTSTS0_DVST);

        /* Check the DVSQ state. */
        switch(rz_interrupt & UX_RZ_DCD_INTSTS0_DVSQ_MASK) 
        {

            case    UX_RZ_DCD_INTSTS0_DVSQ_POWERED  :
                break;
                
            case    UX_RZ_DCD_INTSTS0_DVSQ_DEFAULT  :

                /* We enter this state when there is a Bus Reset.  */
                /* Decide what speed is used by the host, read DVSTCTR and isolate speed.  */
                rz_dvstctr =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_DVSTCTR0) & UX_RZ_DCD_DVSTCTR_RHST;
                
                /* What speed ?  Either Full or High speed.  */
                switch(rz_dvstctr)
                {
                    
                    case    UX_RZ_DCD_DVSTCTR_SPEED_FULL    :

                        /* We are connected at full speed.  */
                        _ux_system_slave -> ux_system_slave_speed =  UX_FULL_SPEED_DEVICE;
                        break;

                    case    UX_RZ_DCD_DVSTCTR_SPEED_HIGH    :

                        /* We are connected at high speed.  */
                        _ux_system_slave -> ux_system_slave_speed =  UX_HIGH_SPEED_DEVICE;
                        break;                            
              
                    default                                 :
                        break;
                }

                /* Check for bouncing signal.  */
                if (device -> ux_slave_device_state !=  UX_DEVICE_ATTACHED)
                {

                    /* Complete the device initialization.  */
                    _ux_dcd_rz_initialize_complete();

                    /* Mark the device as attached now.  */
                    device -> ux_slave_device_state =  UX_DEVICE_ATTACHED;

                }
                break;
                                
            
            case    UX_RZ_DCD_INTSTS0_DVSQ_ADDRESS  :
            
                /* Internally decoded command : SET_ADDRESS, memorise the state.  */
                dcd -> ux_slave_dcd_device_address =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_USBADDR);

                break;
                
            case    UX_RZ_DCD_INTSTS0_DVSQ_CONFIGURED   :

                /* Internally decoded command : SET_CONFIGURATION, memorise the state.  */
                dcd_rz -> ux_dcd_rz_debug =  UX_DEVICE_CONFIGURED;

                break;
                
        default:   
                /* We enter this state when we have a SUSPEND signal.  */
                /* Check the status change callback.  */
                if(_ux_system_slave -> ux_system_slave_change_function != UX_NULL)
                {
        
                    /* Inform the application if a callback function was programmed.  */
                    _ux_system_slave -> ux_system_slave_change_function(UX_DEVICE_SUSPENDED);
                }
        
                /* If the device is attached or configured, we have a disconnection signal.  */
                if (device -> ux_slave_device_state !=  UX_DEVICE_RESET)
                {
                    /* Device is reset, the behavior is the same as disconnection.  */
                    _ux_device_stack_disconnect();
                    
                } 

                break;        
        }
    }

    /* Check if we have a BEMP interrupt.  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_BEMP)
    {

        /* Check BEMP irq flags. */
        bempsts =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_BEMPSTS);
        bempenb =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_BEMPENB);
        bemp_bit = 1;

        /* 15 pipes to check.  */
        for (pipe_index = 0; pipe_index <= 15; pipe_index++)
        {
            /* if flag set, we have something done. */
            if( (bempsts & bemp_bit) && (bempenb & bemp_bit) )
            { 

                /* Clear BEMP.  */
                _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_BEMPSTS, ~bemp_bit);

                /* Get the physical endpoint associated with this pipe.  */
                endpoint_index =  dcd_rz -> ux_dcd_rz_pipe[pipe_index];

                /* Get the physical endpoint associated with this endpoint_index.  */
                ed =  &dcd_rz -> ux_dcd_rz_ed[endpoint_index];

                /* Get the endpoint to the endpoint.  */
                endpoint =  ed -> ux_dcd_rz_ed_endpoint;
    
                /* Get the pointer to the transfer request.  */
                transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;
    
                /* Process the call back.  */
                _ux_dcd_rz_transfer_callback(dcd_rz, transfer_request, UX_RZ_DCD_INTSTS0_BEMP, 0);

            }

            /* Shift the BEMP bit.  */
            bemp_bit = bemp_bit << 1;
        }
    }                   


    /* Check if we have a BRDY interrupt.  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_BRDY)
    {

        /* Check BRDY irq flags. */
        brdysts =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_BRDYSTS);
        brdyenb =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_BRDYENB);
        brdy_bit = 1;

        /* 15 pipes to check.  */
        for (pipe_index = 0; pipe_index <= 15; pipe_index++)
        {
            /* if flag set, we have something done. */
            if( (brdysts & brdy_bit) && (brdyenb & brdy_bit) )
            { 

                /* Clear BRDY.  */
                _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_BRDYSTS, ~brdy_bit);

                /* Get the physical endpoint associated with this pipe.  */
                endpoint_index =  dcd_rz -> ux_dcd_rz_pipe[pipe_index];

                /* Get the physical endpoint associated with this endpoint_index.  */
                ed =  &dcd_rz -> ux_dcd_rz_ed[endpoint_index];

                /* Get the endpoint to the endpoint.  */
                endpoint =  ed -> ux_dcd_rz_ed_endpoint;
    
                /* Get the pointer to the transfer request.  */
                transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;
    
                /* Process the call back.  */
                _ux_dcd_rz_transfer_callback(dcd_rz, transfer_request, UX_RZ_DCD_INTSTS0_BRDY, 0);

            }

            /* Shift the BRDY bit.  */
            brdy_bit = brdy_bit << 1;
        }
    }                   

    /* Check if we have a NRDY interrupt.  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_NRDY)
    {

        /* Check NRDY irq flags. */
        nrdysts =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_NRDYSTS);
        nrdyenb =  _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_NRDYENB);
        nrdy_bit = 1;

        /* 15 pipes to check.  */
        for (pipe_index = 0; pipe_index <= 15; pipe_index++)
        {
            /* If flag set, we have something done. */
            if( (nrdysts & nrdy_bit) && (nrdyenb & nrdy_bit))
            { 

                /* Clear NRDY.  */
                _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_NRDYSTS, ~nrdy_bit);

                /* Get the physical endpoint associated with this pipe.  */
                endpoint_index =  dcd_rz -> ux_dcd_rz_pipe[pipe_index];

                /* Get the physical endpoint associated with this endpoint_index.  */
                ed =  &dcd_rz -> ux_dcd_rz_ed[endpoint_index];

                /* Get the endpoint to the endpoint.  */
                endpoint =  ed -> ux_dcd_rz_ed_endpoint;
    
                /* Get the pointer to the transfer request.  */
                transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;
    
                /* Process the call back.  */
                _ux_dcd_rz_transfer_callback(dcd_rz, transfer_request, UX_RZ_DCD_INTSTS0_NRDY, 0);

            }

            /* Shift the NRDY bit.  */
            nrdy_bit = nrdy_bit << 1;
        }
    }                   

    /* Check if we have a SETUP transaction phase.  */
    if (rz_interrupt & UX_RZ_DCD_INTSTS0_CTRT)
    {
    
        /* Clear CTRT.  */
        _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_INTSTS0, ~UX_RZ_DCD_INTSTS0_CTRT);

        /* Get the physical endpoint associated with this endpoint.  SETUP is on endpoint 0.  */
        ed =  &dcd_rz -> ux_dcd_rz_ed[0];

        /* Get the endpoint to the endpoint.  */
        endpoint =  ed -> ux_dcd_rz_ed_endpoint;
    
        /* Get the pointer to the transfer request.  */
        transfer_request =  &endpoint -> ux_slave_endpoint_transfer_request;
    
        /* Process the call back.  */
        _ux_dcd_rz_transfer_callback(dcd_rz, transfer_request, UX_RZ_DCD_INTSTS0_CTRT, rz_interrupt & UX_RZ_DCD_INTSTS0_CTSQ_MASK);

    }    
}

