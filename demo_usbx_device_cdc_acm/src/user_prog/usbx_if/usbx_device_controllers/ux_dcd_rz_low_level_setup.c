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
#include "tx_api.h"
#include "iodefine.h"

#ifdef UX_DCD_RZA2
/* RZ/A2x */
#include "r_intc_lld_rza2m.h"        /* INTC Driver Header   */
#include "r_usb_basic_config.h"
#else
/* RZ/A1H */
#include "r_intc.h"            /* INTC Driver Header */
#include "rza_io_regrw.h"      /* Low level register read/write header */
#include "r_gpio_if.h"
#include "mcu_board_select.h"
#include "riic_cat9554_if.h"

#define USB_PRI_DMA_TX (23)
#define USB_PRI_DMA_RX (24)
#define USB_PRI_USBI0 (28)
#define USB_PRI_USBI1 (27)
#endif

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    ux_rz_dcd_low_level_setup                           PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*     This function sets up the RZ DCD controller.                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*      None                                                              */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*      None                                                              */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*      Set_IntVect                         Set USB interrupt vector      */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*      _ux_hcd_rz_initialize               USBX HCD RZ initialization    */ 
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
void  _ux_dcd_rz_low_level_setup(void)
{
#ifdef UX_DCD_RZA2
/* RZ/A2x */

#if (UX_RZ_USB_BASE==UX_RZ_USB0_BASE)

#ifdef UX_RZ_DCD_USE_DMA
    	R_INTC_Enable(INTC_ID_USB_USBFDMA00); /* D0FIFO0 Enable */
        R_INTC_Enable(INTC_ID_USB_USBFDMA01); /* D1FIFO0 Enable */
#endif
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBFI0, (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBHI0, (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);
        R_INTC_Enable(INTC_ID_USB_USBFI0); /* USBR0 Enable */
        R_INTC_Enable(INTC_ID_USB_USBHI0); /* USBR0 Enable */
        R_INTC_SetPriority(INTC_ID_USB_USBFI0, USB_CFG_INTERRUPT_PRIORITY);
        R_INTC_SetPriority(INTC_ID_USB_USBHI0, USB_CFG_INTERRUPT_PRIORITY);

#else
/* RZ/A2M GR-MANGO Type-C connector */

#ifdef UX_RZ_DCD_USE_DMA
        R_INTC_Enable(INTC_ID_USB_USBFDMA10); /* Enable D0FIFO interrupt */
        R_INTC_Enable(INTC_ID_USB_USBFDMA11); /* Enable D1FIFO interrupt */
#endif
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBFI1, (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);
    	R_INTC_RegistIntFunc(INTC_ID_USB_USBHI1, (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);
        R_INTC_Enable(INTC_ID_USB_USBFI1); /* Enable USB11 interrupt */
        R_INTC_Enable(INTC_ID_USB_USBHI1); /* Enable USB11 interrupt */
        R_INTC_SetPriority(INTC_ID_USB_USBFI1, USB_CFG_INTERRUPT_PRIORITY);
        R_INTC_SetPriority(INTC_ID_USB_USBHI1, USB_CFG_INTERRUPT_PRIORITY);

#endif

#else
/* RZ/A1H */

ULONG   iprreg;
#if (TARGET_BOARD == TARGET_BOARD_RSK)
	uint8_t px_addr, px_data, px_config;
#endif /* TARGET_BOARD */

#if (TARGET_BOARD == TARGET_BOARD_STREAM_IT2)

    gpio_init(P7_1);
    gpio_dir(P7_1, PIN_OUTPUT);
    gpio_write(P7_1, 1);

#elif (TARGET_BOARD == TARGET_BOARD_RSK)

	/* USB VBUS VOLTAGE ACTIVATION  : PX2_USB_PWR_ENB Low Output */
	/* Defines for Port Expander 2 pins
	 PX2_PX1_EN0 = HIGH (DV output from VIO)
	 PX2_PX1_EN1 = HIGH (Ethernet)
	 PX2_TFT_CS = LOW (TFT Chip Select)
	 PX2_PX1_EN3 = HIGH
	 PX2_USB_OVR_CUR - Input
	 PX2_USB_PWR_ENA = LOW (USB Power enabled)
	 PX2_USB_PWR_ENB = LOW (USB Power enabled)
	 PX2_PX1_EN7 = LOW (A18..A21 address lines to NOR)
	 */
	R_RIIC_CAT9554_Open();
	px_addr = CAT9554_I2C_PX2;
	px_data = PX2_PX1_EN0 | PX2_PX1_EN1 | PX2_PX1_EN3;
	px_config = PX2_PX1_EN0 | PX2_PX1_EN1 | PX2_TFT_CS | PX2_PX1_EN3 |
				PX2_USB_PWR_ENA | PX2_USB_PWR_ENB | PX2_PX1_EN7;
	R_RIIC_CAT9554_Write(px_addr, px_data, px_config);
	R_RIIC_CAT9554_Close();

#endif

#if UX_RZ_USB_BASE == UX_RZ_USB0_BASE
/* turn on USB0 clock */
//CPG.STBCR7.BIT.MSTP71 = 0;
CPG.STBCR7 &= (uint8_t)~(CPG_STBCR7_MSTP71);

/* Register USB interrupt handler function */
R_INTC_RegistIntFunc(INTC_ID_USBI0,  (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);

/* Set interrupt priority */
R_INTC_SetPriority(INTC_ID_USBI0, USB_PRI_USBI0);

/* A/D end interrupt enable */
R_INTC_Enable(INTC_ID_USBI0);

#else
    /* turn on USB1 clock */
   // CPG.STBCR7.BIT.MSTP70 = 0;

    /* turn on USB1 clock */
	//CPG.STBCR7 &= 0xFE;
	CPG.STBCR7 &= (uint8_t)~(CPG_STBCR7_MSTP70|CPG_STBCR7_MSTP71);
	dummy = CPG.STBCR7;

    /* Register ETHER interrupt handler function */
    R_INTC_RegistIntFunc(INTC_ID_USBI1, (void (*)(uint32_t))_ux_dcd_rz_interrupt_handler);

    /* Set interrupt priority */
	R_INTC_SetPriority(INTC_ID_USBI1, USB_PRI_USBI1);

	/* A/D end interrupt enable */
	R_INTC_Enable(INTC_ID_USBI1);
#endif

#ifdef UX_RZ_DCD_USE_DMA

    /* Register DMA TX interrupt handler function */
    R_INTC_RegistIntFunc(UX_RZ_DMA_TX_INT_ID, (void (*)(uint32_t))_ux_dcd_rz_dma_tx_interrupt_handler);
    
    /* Set interrupt priority */
	R_INTC_SetPriority(UX_RZ_DMA_TX_INT_ID, USB_PRI_DMA_TX);

	/* A/D end interrupt enable */
	R_INTC_Enable(UX_RZ_DMA_TX_INT_ID);


    /* Register DMA RX interrupt handler function */
    R_INTC_RegistIntFunc(UX_RZ_DMA_RX_INT_ID, (void (*)(uint32_t))_ux_dcd_rz_dma_rx_interrupt_handler);
    
    /* Set interrupt priority */
	R_INTC_SetPriority(UX_RZ_DMA_RX_INT_ID, USB_PRI_DMA_RX);

	 /* Enable DMA interrupt.  */
	R_INTC_Enable(UX_RZ_DMA_RX_INT_ID);
#endif

#endif
}

