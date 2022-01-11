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

#ifdef UX_RZ_DCD_USE_DMA
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                                 RELEASE      */
/*                                                                        */
/*    _ux_dcd_rz_dma_rx_interrupt_handler                   PORTABLE C    */
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

VOID  _ux_dcd_rz_dma_rx_interrupt_handler(UINT int_sense)
{

UX_SLAVE_DCD            *dcd;
UX_DCD_RZ               *dcd_rz;


    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* Get the pointer to the RX DCD.  */
    dcd_rz = (UX_DCD_RZ *) dcd -> ux_slave_dcd_controller_hardware;

    /* Clear DMA flags.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCTRL(UX_RZ_DMA_RX_CH),
                                  UX_RZ_DMA_CHCTRL_CLREN | UX_RZ_DMA_CHCTRL_SWRST);

    /* Set status to idle.  */
    dcd_rz -> ux_dcd_rz_dma_status = UX_RZ_DMA_STATUS_IDLE;

    /* Check if we need to put a semaphore.  */
    if (dcd_rz -> ux_dcd_rz_transfer_semaphore != UX_NULL)
    {

        /* Put the semaphore and clear the pointer.  */
        _ux_utility_semaphore_put(dcd_rz -> ux_dcd_rz_transfer_semaphore);
        dcd_rz -> ux_dcd_rz_transfer_semaphore = UX_NULL;
    }

}

#endif
