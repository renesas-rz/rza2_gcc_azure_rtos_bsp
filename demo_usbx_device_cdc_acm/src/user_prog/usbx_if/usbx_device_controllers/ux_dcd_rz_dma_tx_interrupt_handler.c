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
/*    _ux_dcd_rz_dma_tx_interrupt_handler                   PORTABLE C    */
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
/*    None                                                                */
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

VOID  _ux_dcd_rz_dma_tx_interrupt_handler(UINT int_sense)
{

UX_SLAVE_DCD            *dcd;
UX_DCD_RZ               *dcd_rz;


    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* Get the pointer to the RX DCD.  */
    dcd_rz = (UX_DCD_RZ *) dcd -> ux_slave_dcd_controller_hardware;

    /* Clear DMA flags.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCTRL(UX_RZ_DMA_TX_CH),
                                  UX_RZ_DMA_CHCTRL_CLREN | UX_RZ_DMA_CHCTRL_SWRST);

    /* Check if we are transmitting a short packet.  */
    if (dcd_rz -> ux_dcd_rz_dma_status == UX_RZ_DMA_STATUS_WRITING_SHORT)
    {

        /* Yes, set the Buffer Memory Valid flag.  
           This bit is set to 1 when data has been written to the FIFO and
           this is a short packet or zero packet or a full packet but not the
           end of the transmission.  */
        _ux_dcd_rz_register_set(dcd_rz, dcd_rz -> ux_dcd_rz_current_fifo_ctrl, UX_RZ_DCD_FIFOCTR_BVAL);

    }

    /* Set status to IDLE.  */
    dcd_rz -> ux_dcd_rz_dma_status = UX_RZ_DMA_STATUS_IDLE;
}

#endif