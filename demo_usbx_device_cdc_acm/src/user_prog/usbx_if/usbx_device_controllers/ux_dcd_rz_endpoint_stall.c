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
/*    _ux_dcd_rz_endpoint_stall                             PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will stall a physical endpoint.                       */
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
UINT  _ux_dcd_rz_endpoint_stall(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint)
{

UX_DCD_RZ_ED      *ed;
#ifdef UX_RZ_DCD_USE_DMA
ULONG             pipectr_reg; 
#endif


    /* Get the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_RZ_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Force STALL on this endpoint.  */
    switch (ed -> ux_dcd_rz_ed_index)
    {
    
        case 0 :
            
            /* Set STALL in DCPCTR register.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_DCPCTR, UX_RZ_DCD_DCPCTR_PIDSTALL);
            break;        
        
        default : 
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
            pipectr_reg = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2));
            while(pipectr_reg & UX_RZ_DCD_DCPCTR_SUREQ )
                pipectr_reg = _ux_dcd_rz_register_read(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index -1) * 2));
#endif

            /* Set STALL in PIPExCTR register.  */
            _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_PIPE1CTR + ((ed -> ux_dcd_rz_ed_index - 1) * 2), UX_RZ_DCD_DCPCTR_PIDSTALL);
            break;        
        
    }

    /* Set the endpoint to stall.  */
    ed -> ux_dcd_rz_ed_status |=  UX_DCD_RZ_ED_STATUS_STALLED;

    /* This function never fails.  */
    return(UX_SUCCESS);         
}

