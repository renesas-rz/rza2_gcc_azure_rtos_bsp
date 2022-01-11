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
/*    _ux_dcd_rz_initialize                                 PORTABLE C    */
/*                                                             5.8        */
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function initializes the USB slave controller of the Renesas   */
/*    RZ chipset.                                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd                                   Address of DCD                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */ 
/*                                                                        */
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _ux_dcd_rz_register_write           Read register                   */ 
/*    _ux_utility_memory_allocate           Allocate memory               */ 
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
UINT  _ux_dcd_rz_initialize(ULONG dcd_io)
{

UX_SLAVE_DCD        *dcd;
UX_DCD_RZ           *dcd_rz;

    /* Call the bsp low level init.  */
    _ux_dcd_rz_low_level_setup();

    /* Get the pointer to the DCD.  */
    dcd =  &_ux_system_slave -> ux_system_slave_dcd;

    /* The controller initialized here is of RX type.  */
    dcd -> ux_slave_dcd_controller_type =  UX_DCD_RZ_SLAVE_CONTROLLER;
    
    /* Allocate memory for this RX DCD instance.  */
    dcd_rz =  _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY, sizeof(UX_DCD_RZ));

    /* Check if memory was properly allocated.  */
    if(dcd_rz == UX_NULL)
        return(UX_MEMORY_INSUFFICIENT);

    /* Set the pointer to the RX DCD.  */
    dcd -> ux_slave_dcd_controller_hardware =  (VOID *) dcd_rz;

    /* Save the base address of the controller.  */
    dcd -> ux_slave_dcd_io =        dcd_io;
    dcd_rz -> ux_dcd_rz_base =  dcd_io;

    /* Set the generic DCD owner for the RX DCD.  */
    dcd_rz -> ux_dcd_rz_dcd_owner =  dcd;

    /* Initialize the function collector for this DCD.  */
    dcd -> ux_slave_dcd_function =  _ux_dcd_rz_function;

    /*  Buffer number starts at 8 , till 127 */
    dcd_rz -> ux_dcd_rz_next_available_bufnum =  8;

    /* Supply the clock to the USB module. */
    _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_SUSPMODE, UX_RZ_DCD_SUSPMODE_SUSPM); //USB0.SUSPMODE.BIT.SUSPM = 0;
    _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_SYSCFG, 0); //USB0.SYSCFG.WORD = 0;
    /* UPLLE bit only available in channel 0.  */
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SYSCFG_0, UX_RZ_DCD_SYSCFG_UPLLE );// USB0.SYSCFG.WORD = (USB_FUNCTION_BITUPLLE | clockmode);
    tx_thread_sleep(15);
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SUSPMODE, UX_RZ_DCD_SUSPMODE_SUSPM);//USB0.SUSPMODE.BIT.SUSPM = 1;
   
    _ux_dcd_rz_register_write(dcd_rz, UX_RZ_DCD_BUSWAIT, 0x1); //USB1.BUSWAIT.WORD = (USB_U16)(USB_FUNCTION_BUSWAIT_03 & USB_FUNCTION_BITBWAIT);

    /* Reset USB Module.  */

    /* Make sure we are in Device mode.  */
    _ux_dcd_rz_register_clear(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_DCFM);

    /* USB Module Operation Enable.  */
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_USBE);
    
    /*Enable D+ pull up*/
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_DPRPU);
    
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_SYSCFG, UX_RZ_DCD_SYSCFG_HSE);
    
    /* Enable the required interrupts.  */
    /* Enable only the interrupts we will use: SOF, ATTCH/BCHG. */
    _ux_dcd_rz_register_set(dcd_rz, UX_RZ_DCD_INTENB0, 
                                (UX_RZ_DCD_INTENB0_VBSE |
                                UX_RZ_DCD_INTENB0_DVSE  |
                                UX_RZ_DCD_INTENB0_CTRE  |
                                UX_RZ_DCD_INTENB0_BEMPE |
                                UX_RZ_DCD_INTENB0_NRDYE |
                                UX_RZ_DCD_INTENB0_BRDYE));

#ifdef UX_RZ_DCD_USE_DMA
    
    /* Reset DMA TX channel.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCTRL(UX_RZ_DMA_TX_CH),
                                  UX_RZ_DMA_CHCTRL_CLREN | UX_RZ_DMA_CHCTRL_SWRST);

    /* Select peripheral trigger for DMA TX channel.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_TX_RS, UX_RZ_DMA_TX_RS_VALUE);

    /* Configure DMA TX channel.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCFG(UX_RZ_DMA_TX_CH),
                              (UX_RZ_DMA_TX_CH & 0x7) | 
                              (0x2<<8) | 
                              UX_RZ_DMA_CHCFG_LVL | 
                              UX_RZ_DMA_CHCFG_HIEN |
                              UX_RZ_DMA_CHCFG_REQD | 
                              UX_RZ_DMA_CHCFG_DAD);

    /* Reset DMA RX channel.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCTRL(UX_RZ_DMA_RX_CH),
                                  UX_RZ_DMA_CHCTRL_CLREN | UX_RZ_DMA_CHCTRL_SWRST);

    /* Select peripheral trigger for DMA RX channel.  */
    _ux_dcd_rz_dma_register_set(dcd_rz, UX_RZ_DMA_RX_RS, UX_RZ_DMA_RX_RS_VALUE);

    /* Configure DMA RX channel.  */
    _ux_dcd_rz_dma_register_write(dcd_rz, UX_RZ_DMA_CHCFG(UX_RZ_DMA_RX_CH),
                              (UX_RZ_DMA_RX_CH & 0x7) | 
                              (0x2<<8) | 
                              UX_RZ_DMA_CHCFG_LVL | 
                              UX_RZ_DMA_CHCFG_HIEN |
                              UX_RZ_DMA_CHCFG_SAD);
#endif

    /* Set the state of the controller to OPERATIONAL now.  */
    dcd -> ux_slave_dcd_status =  UX_DCD_STATUS_OPERATIONAL;


    /* Return successful completion.  */
    return(UX_SUCCESS);
}

