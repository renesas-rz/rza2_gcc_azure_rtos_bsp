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

/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_dcd_rz.h                                         PORTABLE C      */ 
/*                                                           5.8          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron                                                       */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the equivalences for the RZ RENESAS USB           */ 
/*    controller.                                                         */ 
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

#ifndef UX_DCD_RZ_H
#define UX_DCD_RZ_H

//#define UX_RZ_DCD_USE_DMA
/* Define RZ generic equivalences.  */

/* Define for RZ/A2x */
#define UX_DCD_RZA2

#define UX_DCD_RZ_SLAVE_CONTROLLER                          0x80
#define UX_DCD_RZ_MAX_ED                                    10
#define UX_DCD_RZ_ENABLE                                    1
#define UX_DCD_RZ_DISABLE                                   0
#define UX_DCD_RZ_MAX_BULK_PAYLOAD                          512 
#define UX_DCD_RZ_MAX_CONTROL_PAYLOAD                       512 
#define UX_DCD_RZ_MAX_BUF_SIZE                              64
#define UX_DCD_RZ_MAX_BUF_NUM                               127


/* Define RZ HCOR register mapping.  */
#ifdef UX_DCD_RZA2
/* RZ/A2x */
    #define UX_RZ_USB0_BASE                             0xE8219000
    #define UX_RZ_USB1_BASE                             0xE821B000
#else
/* RZ/A1H */
    #define UX_RZ_USB0_BASE                             0xE8010000
    #define UX_RZ_USB1_BASE                             0xE8207000
#endif

#ifdef UX_DCD_RZA2
/* GR-MANGO Type-C connector */
    #define UX_RZ_USB_BASE                              UX_RZ_USB1_BASE
#else
    #define UX_RZ_USB_BASE                              UX_RZ_USB0_BASE
#endif

#define UX_RZ_DCD_SYSCFG_0                          (UX_RZ_USB0_BASE + 0x00)

#define UX_RZ_DCD_SYSCFG                            (UX_RZ_USB_BASE + 0x00)
#define UX_RZ_DCD_BUSWAIT                           (UX_RZ_USB_BASE + 0x02)
#define UX_RZ_DCD_SYSSTS0                           (UX_RZ_USB_BASE + 0x04)
#define UX_RZ_DCD_DVSTCTR0                          (UX_RZ_USB_BASE + 0x08)
#define UX_RZ_DCD_TESTMODE                          (UX_RZ_USB_BASE + 0x0C)
#define UX_RZ_DCD_D0FBCFG                           (UX_RZ_USB_BASE + 0x10)
#define UX_RZ_DCD_D1FBCFG                           (UX_RZ_USB_BASE + 0x12)
#define UX_RZ_DCD_CFIFO                             (UX_RZ_USB_BASE + 0x14)
#define UX_RZ_DCD_D0FIFO                            (UX_RZ_USB_BASE + 0x18)
#define UX_RZ_DCD_D1FIFO                            (UX_RZ_USB_BASE + 0x1C)
#define UX_RZ_DCD_CFIFOSEL                          (UX_RZ_USB_BASE + 0x20)
#define UX_RZ_DCD_CFIFOCTR                          (UX_RZ_USB_BASE + 0x22)
#define UX_RZ_DCD_D0FIFOSEL                         (UX_RZ_USB_BASE + 0x28)
#define UX_RZ_DCD_D0FIFOCTR                         (UX_RZ_USB_BASE + 0x2A)
#define UX_RZ_DCD_D1FIFOSEL                         (UX_RZ_USB_BASE + 0x2C)
#define UX_RZ_DCD_D1FIFOCTR                         (UX_RZ_USB_BASE + 0x2E)
#define UX_RZ_DCD_INTENB0                           (UX_RZ_USB_BASE + 0x30)
#define UX_RZ_DCD_INTENB1                           (UX_RZ_USB_BASE + 0x32)
#define UX_RZ_DCD_BRDYENB                           (UX_RZ_USB_BASE + 0x36)
#define UX_RZ_DCD_NRDYENB                           (UX_RZ_USB_BASE + 0x38)
#define UX_RZ_DCD_BEMPENB                           (UX_RZ_USB_BASE + 0x3A)
#define UX_RZ_DCD_SOFCFG                            (UX_RZ_USB_BASE + 0x3C)
#define UX_RZ_DCD_INTSTS0                           (UX_RZ_USB_BASE + 0x40)
#define UX_RZ_DCD_INTSTS1                           (UX_RZ_USB_BASE + 0x42)
#define UX_RZ_DCD_BRDYSTS                           (UX_RZ_USB_BASE + 0x46)
#define UX_RZ_DCD_NRDYSTS                           (UX_RZ_USB_BASE + 0x48)
#define UX_RZ_DCD_BEMPSTS                           (UX_RZ_USB_BASE + 0x4A)
#define UX_RZ_DCD_FRMNUM                            (UX_RZ_USB_BASE + 0x4C)
#define UX_RZ_DCD_UFRMNUM                           (UX_RZ_USB_BASE + 0x4E)
#define UX_RZ_DCD_USBADDR                           (UX_RZ_USB_BASE + 0x50)
#define UX_RZ_DCD_USBREQ                            (UX_RZ_USB_BASE + 0x54)
#define UX_RZ_DCD_USBVAL                            (UX_RZ_USB_BASE + 0x56)
#define UX_RZ_DCD_USBINDX                           (UX_RZ_USB_BASE + 0x58)
#define UX_RZ_DCD_USBLENG                           (UX_RZ_USB_BASE + 0x5A)
#define UX_RZ_DCD_DCPCFG                            (UX_RZ_USB_BASE + 0x5C)
#define UX_RZ_DCD_DCPMAXP                           (UX_RZ_USB_BASE + 0x5E)
#define UX_RZ_DCD_DCPCTR                            (UX_RZ_USB_BASE + 0x60)
#define UX_RZ_DCD_PIPESEL                           (UX_RZ_USB_BASE + 0x64)
#define UX_RZ_DCD_PIPECFG                           (UX_RZ_USB_BASE + 0x68)
#define UX_RZ_DCD_PIPEBUF                           (UX_RZ_USB_BASE + 0x6A)
#define UX_RZ_DCD_PIPEMAXP                          (UX_RZ_USB_BASE + 0x6C)
#define UX_RZ_DCD_PIPEPERI                          (UX_RZ_USB_BASE + 0x6E)
#define UX_RZ_DCD_PIPE1CTR                          (UX_RZ_USB_BASE + 0x70)
#define UX_RZ_DCD_PIPE2CTR                          (UX_RZ_USB_BASE + 0x72)
#define UX_RZ_DCD_PIPE3CTR                          (UX_RZ_USB_BASE + 0x74)
#define UX_RZ_DCD_PIPE4CTR                          (UX_RZ_USB_BASE + 0x76)
#define UX_RZ_DCD_PIPE5CTR                          (UX_RZ_USB_BASE + 0x78)
#define UX_RZ_DCD_PIPE6CTR                          (UX_RZ_USB_BASE + 0x7A)
#define UX_RZ_DCD_PIPE7CTR                          (UX_RZ_USB_BASE + 0x7C)
#define UX_RZ_DCD_PIPE8CTR                          (UX_RZ_USB_BASE + 0x7E)
#define UX_RZ_DCD_PIPE9CTR                          (UX_RZ_USB_BASE + 0x80)
#define UX_RZ_DCD_PIPEACTR                          (UX_RZ_USB_BASE + 0x82)
#define UX_RZ_DCD_PIPEBCTR                          (UX_RZ_USB_BASE + 0x84)
#define UX_RZ_DCD_PIPECCTR                          (UX_RZ_USB_BASE + 0x86)
#define UX_RZ_DCD_PIPEDCTR                          (UX_RZ_USB_BASE + 0x88)
#define UX_RZ_DCD_PIPEECTR                          (UX_RZ_USB_BASE + 0x8A)
#define UX_RZ_DCD_PIPEFCTR                          (UX_RZ_USB_BASE + 0x8C)
#define UX_RZ_DCD_PIPE1TRE                          (UX_RZ_USB_BASE + 0x90)
#define UX_RZ_DCD_PIPE1TRN                          (UX_RZ_USB_BASE + 0x92)
#define UX_RZ_DCD_PIPE2TRE                          (UX_RZ_USB_BASE + 0x94)
#define UX_RZ_DCD_PIPE2TRN                          (UX_RZ_USB_BASE + 0x96)
#define UX_RZ_DCD_PIPE3TRE                          (UX_RZ_USB_BASE + 0x98)
#define UX_RZ_DCD_PIPE3TRN                          (UX_RZ_USB_BASE + 0x9A)
#define UX_RZ_DCD_PIPE4TRE                          (UX_RZ_USB_BASE + 0x9C)
#define UX_RZ_DCD_PIPE4TRN                          (UX_RZ_USB_BASE + 0x9E)
#define UX_RZ_DCD_PIPE5TRE                          (UX_RZ_USB_BASE + 0xA0)
#define UX_RZ_DCD_PIPE5TRN                          (UX_RZ_USB_BASE + 0xA2)
#define UX_RZ_DCD_PIPEBTRE                          (UX_RZ_USB_BASE + 0xA4)
#define UX_RZ_DCD_PIPEBTRN                          (UX_RZ_USB_BASE + 0xA6)
#define UX_RZ_DCD_PIPECTRE                          (UX_RZ_USB_BASE + 0xA8)
#define UX_RZ_DCD_PIPECTRN                          (UX_RZ_USB_BASE + 0xAA)
#define UX_RZ_DCD_PIPEDTRE                          (UX_RZ_USB_BASE + 0xAC)
#define UX_RZ_DCD_PIPEDTRN                          (UX_RZ_USB_BASE + 0xAE)
#define UX_RZ_DCD_PIPEETRE                          (UX_RZ_USB_BASE + 0xB0)
#define UX_RZ_DCD_PIPEETRN                          (UX_RZ_USB_BASE + 0xB2)
#define UX_RZ_DCD_PIPEFTRE                          (UX_RZ_USB_BASE + 0xB4)
#define UX_RZ_DCD_PIPEFTRN                          (UX_RZ_USB_BASE + 0xB6)
#define UX_RZ_DCD_PIPE9TRE                          (UX_RZ_USB_BASE + 0xB8)
#define UX_RZ_DCD_PIPE9TRN                          (UX_RZ_USB_BASE + 0xBA)
#define UX_RZ_DCD_PIPEATRE                          (UX_RZ_USB_BASE + 0xBC)
#define UX_RZ_DCD_PIPEATRN                          (UX_RZ_USB_BASE + 0xBE)
#define UX_RZ_DCD_DEVADD0                           (UX_RZ_USB_BASE + 0xD0)
#define UX_RZ_DCD_DEVADD1                           (UX_RZ_USB_BASE + 0xD2)
#define UX_RZ_DCD_DEVADD2                           (UX_RZ_USB_BASE + 0xD4)
#define UX_RZ_DCD_DEVADD3                           (UX_RZ_USB_BASE + 0xD6)
#define UX_RZ_DCD_DEVADD4                           (UX_RZ_USB_BASE + 0xD8)
#define UX_RZ_DCD_DEVADD5                           (UX_RZ_USB_BASE + 0xDA)
#define UX_RZ_DCD_DEVADD6                           (UX_RZ_USB_BASE + 0xDC)
#define UX_RZ_DCD_DEVADD7                           (UX_RZ_USB_BASE + 0xDE)
#define UX_RZ_DCD_DEVADD8                           (UX_RZ_USB_BASE + 0xE0)
#define UX_RZ_DCD_DEVADD9                           (UX_RZ_USB_BASE + 0xE2)
#define UX_RZ_DCD_DEVADDA                           (UX_RZ_USB_BASE + 0xE4)
#define UX_RZ_DCD_SUSPMODE                          (UX_RZ_USB_BASE + 0x102)


/* Define RZ control register values.  */
#define UX_RZ_DCD_SYSCFG_HSE                                (1<<7)
#define UX_RZ_DCD_SYSCFG_DCFM                               (1<<6)
#define UX_RZ_DCD_SYSCFG_DRPD                               (1<<5)
#define UX_RZ_DCD_SYSCFG_DPRPU                              (1<<4)
#define UX_RZ_DCD_SYSCFG_UCKSEL                             (1<<2)
#define UX_RZ_DCD_SYSCFG_UPLLE                              (1<<1)
#define UX_RZ_DCD_SYSCFG_USBE                               (1<<0)

#define UX_RZ_DCD_SYSSTS_LNST                               3

#define UX_RZ_DCD_DVSTCTR_WKUP                              (1<<8)
#define UX_RZ_DCD_DVSTCTR_RWUPE                             (1<<7)
#define UX_RZ_DCD_DVSTCTR_USBRST                            (1<<6)
#define UX_RZ_DCD_DVSTCTR_RESUME                            (1<<5)
#define UX_RZ_DCD_DVSTCTR_UACT                              (1<<4)
#define UX_RZ_DCD_DVSTCTR_RHST                              (0x7)

#define UX_RZ_DCD_DVSTCTR_SPEED_LOW                         (1)
#define UX_RZ_DCD_DVSTCTR_SPEED_FULL                        (2)
#define UX_RZ_DCD_DVSTCTR_SPEED_HIGH                        (3)
#define UX_RZ_DCD_DVSTCTR_RESET_IN_PROGRESS                 (4)

#define UX_RZ_DCD_CFIFOSEL_RCNT                             (1<<15)
#define UX_RZ_DCD_CFIFOSEL_REW                              (1<<14)
#define UX_RZ_DCD_CFIFOSEL_MBW_8                            (0<<10)
#define UX_RZ_DCD_CFIFOSEL_MBW_16                           (1<<10)
#define UX_RZ_DCD_CFIFOSEL_MBW_32                           (2<<10)
#define UX_RZ_DCD_CFIFOSEL_MBW_MASK                         (3<<10)
#define UX_RZ_DCD_CFIFOSEL_BIGEND                           (1<<8)
#define UX_RZ_DCD_CFIFOSEL_ISEL                             (1<<5)
#define UX_RZ_DCD_CFIFOSEL_CURPIPE_MASK                     (0xF)

#define UX_RZ_DCD_DFIFOSEL_RCNT                             (1<<15)
#define UX_RZ_DCD_DFIFOSEL_REW                              (1<<14)
#define UX_RZ_DCD_DFIFOSEL_DCLRM                            (1<<13)
#define UX_RZ_DCD_DFIFOSEL_DREQE                            (1<<12)
#define UX_RZ_DCD_DFIFOSEL_MBW_8                            (0<<10)
#define UX_RZ_DCD_DFIFOSEL_MBW_16                           (1<<10)
#define UX_RZ_DCD_DFIFOSEL_MBW_32                           (2<<10)
#define UX_RZ_DCD_DFIFOSEL_MBW_MASK                         (3<<10)
#define UX_RZ_DCD_DFIFOSEL_BIGEND                           (1<<8)
#define UX_RZ_DCD_DFIFOSEL_CURPIPE_MASK                     (0xF)

#define UX_RZ_DCD_FIFOCTR_BVAL                              (1<<15)
#define UX_RZ_DCD_FIFOCTR_BCLR                              (1<<14)
#define UX_RZ_DCD_FIFOCTR_FRDY                              (1<<13)
#define UX_RZ_DCD_FIFOCTR_DTLN                              (0xFFF)
                                                        
#define UX_RZ_DCD_INTENB0_VBSE                              (1<<15)
#define UX_RZ_DCD_INTENB0_RSME                              (1<<14)
#define UX_RZ_DCD_INTENB0_SOFE                              (1<<13)
#define UX_RZ_DCD_INTENB0_DVSE                              (1<<12)
#define UX_RZ_DCD_INTENB0_CTRE                              (1<<11)
#define UX_RZ_DCD_INTENB0_BEMPE                             (1<<10)
#define UX_RZ_DCD_INTENB0_NRDYE                             (1<<9)
#define UX_RZ_DCD_INTENB0_BRDYE                             (1<<8)

#define UX_RZ_DCD_INTENB1_BCHGE                             (1<<14)
#define UX_RZ_DCD_INTENB1_DTCHE                             (1<<12)
#define UX_RZ_DCD_INTENB1_ATTCHE                            (1<<11)
#define UX_RZ_DCD_INTENB1_EOFERRE                           (1<<6)
#define UX_RZ_DCD_INTENB1_SIGNE                             (1<<5)
#define UX_RZ_DCD_INTENB1_SACKE                             (1<<4)

#define UX_RZ_DCD_PIPE0                                     (1<<0)
#define UX_RZ_DCD_PIPE1                                     (1<<1)
#define UX_RZ_DCD_PIPE2                                     (1<<2)
#define UX_RZ_DCD_PIPE3                                     (1<<3)
#define UX_RZ_DCD_PIPE4                                     (1<<4)
#define UX_RZ_DCD_PIPE5                                     (1<<5)
#define UX_RZ_DCD_PIPE6                                     (1<<6)
#define UX_RZ_DCD_PIPE7                                     (1<<7)
#define UX_RZ_DCD_PIPE8                                     (1<<8)
#define UX_RZ_DCD_PIPE9                                     (1<<9)
#define UX_RZ_DCD_PIPE_ALL                                  (0x3FF)

#define UX_RZ_DCD_SOFCFG_TRNENSEL                           (1<<8)
#define UX_RZ_DCD_SOFCFG_BRDYM                              (1<<6)

#define UX_RZ_DCD_INTSTS0_VBINT                             (1<<15)
#define UX_RZ_DCD_INTSTS0_RESM                              (1<<14)
#define UX_RZ_DCD_INTSTS0_SOFR                              (1<<13)
#define UX_RZ_DCD_INTSTS0_DVST                              (1<<12)
#define UX_RZ_DCD_INTSTS0_CTRT                              (1<<11)
#define UX_RZ_DCD_INTSTS0_BEMP                              (1<<10)
#define UX_RZ_DCD_INTSTS0_NRDY                              (1<<9)
#define UX_RZ_DCD_INTSTS0_BRDY                              (1<<8)
#define UX_RZ_DCD_INTSTS0_VBSTS                             (1<<7)
#define UX_RZ_DCD_INTSTS0_DVSQ_MASK                         (7<<4)
#define UX_RZ_DCD_INTSTS0_VALID                             (1<<3)
#define UX_RZ_DCD_INTSTS0_CTSQ_MASK                         (7)

#define UX_RZ_DCD_INTSTS0_DVSQ_POWERED                      0x0000
#define UX_RZ_DCD_INTSTS0_DVSQ_DEFAULT                      0x0010
#define UX_RZ_DCD_INTSTS0_DVSQ_ADDRESS                      0x0020
#define UX_RZ_DCD_INTSTS0_DVSQ_CONFIGURED                   0x0030
#define UX_RZ_DCD_INTSTS0_DVSQ_SUSPENDED                    0x0040

#define UX_RZ_DCD_INTSTS0_CTSQ_SETUP                        0x0000
#define UX_RZ_DCD_INTSTS0_CTSQ_CRDS                         0x0001
#define UX_RZ_DCD_INTSTS0_CTSQ_CRSS                         0x0002
#define UX_RZ_DCD_INTSTS0_CTSQ_CWDS                         0x0003
#define UX_RZ_DCD_INTSTS0_CTSQ_CWSS                         0x0004
#define UX_RZ_DCD_INTSTS0_CTSQ_CWNDSS                       0x0005
#define UX_RZ_DCD_INTSTS0_CTSQ_CTSE                         0x0006

#define UX_RZ_DCD_INTSTS1_BCHG                              (1<<14)
#define UX_RZ_DCD_INTSTS1_DTCH                              (1<<12)
#define UX_RZ_DCD_INTSTS1_ATTCH                             (1<<11)
#define UX_RZ_DCD_INTSTS1_EOFERR                            (1<<6)
#define UX_RZ_DCD_INTSTS1_SIGN                              (1<<5)
#define UX_RZ_DCD_INTSTS1_SACK                              (1<<4)

#define UX_RZ_DCD_FRMNUM_OVRN                               (1<<15)
#define UX_RZ_DCD_FRMNUM_CRCE                               (1<<14)
#define UX_RZ_DCD_FRMNUM_FRNM_MASK                          (0x7FF)
 
#define UX_RZ_DCD_DCPCFG_DIR                                (1<<4)

#define UX_RZ_DCD_DCPMAXP_DEVADDR_SHIFT                     (12)
#define UX_RZ_DCD_DCPMAXP_DEVADDR_MASK                      0xf000

#define UX_RZ_DCD_DCPCTR_BSTS                               (1<<15)
#define UX_RZ_DCD_DCPCTR_SUREQ                              (1<<14)
#define UX_RZ_DCD_DCPCTR_CSCLR                              (1<<13)
#define UX_RZ_DCD_DCPCTR_CSSTS                              (1<<12)
#define UX_RZ_DCD_DCPCTR_SUREQCLR                           (1<<11)
#define UX_RZ_DCD_DCPCTR_SQCLR                              (1<<8)
#define UX_RZ_DCD_DCPCTR_SQSET                              (1<<7)
#define UX_RZ_DCD_DCPCTR_SQMON                              (1<<6)
#define UX_RZ_DCD_DCPCTR_PBUSY                              (1<<5)
#define UX_RZ_DCD_DCPCTR_PINGE                              (1<<4)
#define UX_RZ_DCD_DCPCTR_CCPL                               (1<<2)
#define UX_RZ_DCD_DCPCTR_PID_MASK                           (3)
#define UX_RZ_DCD_DCPCTR_PIDNAK                             (0)
#define UX_RZ_DCD_DCPCTR_PIDBUF                             (1)
#define UX_RZ_DCD_DCPCTR_PIDSTALL                           (2)
#define UX_RZ_DCD_DCPCTR_PIDSTALL2                          (3)


#define UX_RZ_DCD_PIPECFG_TYPE_MASK                         (0xC000)
#define UX_RZ_DCD_PIPECFG_TYPE_BULK                         (1<<14)
#define UX_RZ_DCD_PIPECFG_TYPE_INTERRUPT                    (2<<14)
#define UX_RZ_DCD_PIPECFG_TYPE_ISOCHRONOUS                  (3<<14)
#define UX_RZ_DCD_PIPECFG_BFRE                              (1<<10)
#define UX_RZ_DCD_PIPECFG_DBLB                              (1<<9)
#define UX_RZ_DCD_PIPECFG_CNTMD                             (1<<8)
#define UX_RZ_DCD_PIPECFG_SHTNAK                            (1<<7)
#define UX_RZ_DCD_PIPECFG_DIR                               (1<<4)
#define UX_RZ_DCD_PIPECFG_EPNUM_MASK                        (0xF)

#define UX_RZ_DCD_PIPEBUF_SIZEMASK                          (0x1F<<10)
#define UX_RZ_DCD_PIPEBUF_BUFNMBMASK                        (0xFF<<10)
#define UX_RZ_DCD_PIPEBUF_SHIFT                             (10)

#define UX_RZ_DCD_PIPEMAXP_DEVSELMASK                       (0xF<<12)
#define UX_RZ_DCD_PIPEMAXP_DEVSEL_SHIFT                     (12)
#define UX_RZ_DCD_PIPEMAXP_MXPSMASK                         (0x7FF)

#define UX_RZ_DCD_PIPEPERI_IFIS                             (1<<12)
#define UX_RZ_DCD_PIPEPERI_IIVT_MASK                        (7)

#define UX_RZ_DCD_PIPECTR_BSTS                              (1<<15)
#define UX_RZ_DCD_PIPECTR_INBUFM                            (1<<14)
#define UX_RZ_DCD_PIPECTR_CSCLR                             (1<<13)
#define UX_RZ_DCD_PIPECTR_CSSTS                             (1<<12)
#define UX_RZ_DCD_PIPECTR_ATREPM                            (1<<10)
#define UX_RZ_DCD_PIPECTR_ACLRM                             (1<<9)
#define UX_RZ_DCD_PIPECTR_SQCLR                             (1<<8)
#define UX_RZ_DCD_PIPECTR_SQSET                             (1<<7)
#define UX_RZ_DCD_PIPECTR_SQMON                             (1<<6)
#define UX_RZ_DCD_PIPECTR_PBUSY                             (1<<5)
#define UX_RZ_DCD_PIPECTR_PID_MASK                          (3)
#define UX_RZ_DCD_PIPECTR_PIDNAK                            (0)
#define UX_RZ_DCD_PIPECTR_PIDBUF                            (1)
#define UX_RZ_DCD_PIPECTR_PIDSTALL                          (2)
#define UX_RZ_DCD_PIPECTR_PIDSTALL2                         (3)

#define UX_RZ_DCD_PIPECTR_DATA1                             ( 1<< 7)
#define UX_RZ_DCD_PIPECTR_DATA0                             ( 1<< 8)

#define UX_RZ_DCD_PIPE1TRE_TRCLR                            (1<<8)
#define UX_RZ_DCD_PIPE1TRE_TRENB                            (1<<9)

#define UX_RZ_DCD_DEVADD_UPPHUB_MASK                        (0x7800)
#define UX_RZ_DCD_DEVADD_UPPHUB_SHIFT                       (11)
#define UX_RZ_DCD_DEVADD_HUBPORT_MASK                       (0x0700)
#define UX_RZ_DCD_DEVADD_HUBPORT_SHIFT                      (8)
#define UX_RZ_DCD_DEVADD_USBSPD_MASK                        (0x00C0)
#define UX_RZ_DCD_DEVADD_USBSPD_SHIFT                       (6)
#define UX_RZ_DCD_DEVADDX_SPEED_LOW                         (1<<6)
#define UX_RZ_DCD_DEVADDX_SPEED_FULL                        (2<<6)
#define UX_RZ_DCD_DEVADDX_SPEED_HIGH                        (3<<6)
#define UX_RZ_DCD_DEVADDX_UPPHUB_SHIFT                      11
#define UX_RZ_DCD_DEVADDX_HUBPORT_SHIFT                     8


#define UX_RZ_DCD_FIFO_D0                                   0
#define UX_RZ_DCD_FIFO_D1                                   1
#define UX_RZ_DCD_FIFO_C                                    2


#ifdef UX_RZ_DCD_USE_DMA
/* Define RZ DMA register mapping.  */

#define UX_RZ_DMA_TX_CH                                     10
#define UX_RZ_DMA_RX_CH                                     11

#define UX_RZ_DMA_TX_INT_ID                                 INTC_ID_DMAINT10
#define UX_RZ_DMA_RX_INT_ID                                 INTC_ID_DMAINT11

#define UX_RZ_DMA_BASE(ch)                                  (0xE8200400 + ((ch) - 8) * 0x40)

#define UX_RZ_DMA_N0SA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x00)
#define UX_RZ_DMA_N0DA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x04)
#define UX_RZ_DMA_N0TB(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x08)
#define UX_RZ_DMA_N1SA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x0C)
#define UX_RZ_DMA_N1DA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x10)
#define UX_RZ_DMA_N1TB(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x14)
#define UX_RZ_DMA_CRSA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x18)
#define UX_RZ_DMA_CRDA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x1C)
#define UX_RZ_DMA_CRTB(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x20)
#define UX_RZ_DMA_CHSTAT(ch)                                (UX_RZ_DMA_BASE(ch) + 0x24)
#define UX_RZ_DMA_CHCTRL(ch)                                (UX_RZ_DMA_BASE(ch) + 0x28)
#define UX_RZ_DMA_CHCFG(ch)                                 (UX_RZ_DMA_BASE(ch) + 0x2C)
#define UX_RZ_DMA_CHITVL(ch)                                (UX_RZ_DMA_BASE(ch) + 0x30)
#define UX_RZ_DMA_CHIEXT(ch)                                (UX_RZ_DMA_BASE(ch) + 0x34)
#define UX_RZ_DMA_NXLA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x38)
#define UX_RZ_DMA_CRLA(ch)                                  (UX_RZ_DMA_BASE(ch) + 0x3C)

#define UX_RZ_DMA_RS0                                       (0xFCFE1000)
#define UX_RZ_DMA_RS1                                       (0xFCFE1004)
#define UX_RZ_DMA_RS2                                       (0xFCFE1008)
#define UX_RZ_DMA_RS3                                       (0xFCFE100C)
#define UX_RZ_DMA_RS4                                       (0xFCFE1010)
#define UX_RZ_DMA_RS5                                       (0xFCFE1014)
#define UX_RZ_DMA_RS6                                       (0xFCFE1018)
#define UX_RZ_DMA_RS7                                       (0xFCFE101C)

#define UX_RZ_DMA_TX_RS                                     UX_RZ_DMA_RS5
#define UX_RZ_DMA_RX_RS                                     UX_RZ_DMA_RS5
#define UX_RZ_DMA_TX_RS_VALUE                               0x83
#define UX_RZ_DMA_RX_RS_VALUE                               0x83 << 16

/* Define RZ DMA register values.  */

#define UX_RZ_DMA_CHSTAT_INTMSK                             (1<<16)
#define UX_RZ_DMA_CHSTAT_MODE                               (1<<11)
#define UX_RZ_DMA_CHSTAT_DER                                (1<<10)
#define UX_RZ_DMA_CHSTAT_DW                                 (1<<9)
#define UX_RZ_DMA_CHSTAT_DL                                 (1<<8)
#define UX_RZ_DMA_CHSTAT_SR                                 (1<<7)
#define UX_RZ_DMA_CHSTAT_TC                                 (1<<6)
#define UX_RZ_DMA_CHSTAT_END                                (1<<5)
#define UX_RZ_DMA_CHSTAT_ER                                 (1<<4)
#define UX_RZ_DMA_CHSTAT_SUS                                (1<<3)
#define UX_RZ_DMA_CHSTAT_TACT                               (1<<2)
#define UX_RZ_DMA_CHSTAT_RQST                               (1<<1)
#define UX_RZ_DMA_CHSTAT_EN                                 (1<<0)

#define UX_RZ_DMA_CHCTRL_CLRINTMSK                          (1<<17)
#define UX_RZ_DMA_CHCTRL_SETINTMSK                          (1<<16)
#define UX_RZ_DMA_CHCTRL_CLRSUS                             (1<<9)
#define UX_RZ_DMA_CHCTRL_SETSUS                             (1<<7)
#define UX_RZ_DMA_CHCTRL_CLRTC                              (1<<6)
#define UX_RZ_DMA_CHCTRL_CLREND                             (1<<5)
#define UX_RZ_DMA_CHCTRL_CLRRQ                              (1<<4)
#define UX_RZ_DMA_CHCTRL_SWRST                              (1<<3)
#define UX_RZ_DMA_CHCTRL_STG                                (1<<2)
#define UX_RZ_DMA_CHCTRL_CLREN                              (1<<1)
#define UX_RZ_DMA_CHCTRL_SETEN                              (1<<0)

#define UX_RZ_DMA_CHCFG_DMS                                 (1<<31)
#define UX_RZ_DMA_CHCFG_REN                                 (1<<30)
#define UX_RZ_DMA_CHCFG_RSW                                 (1<<29)
#define UX_RZ_DMA_CHCFG_RSEL                                (1<<28)
#define UX_RZ_DMA_CHCFG_SBE                                 (1<<27)
#define UX_RZ_DMA_CHCFG_DEM                                 (1<<24)
#define UX_RZ_DMA_CHCFG_TM                                  (1<<22)
#define UX_RZ_DMA_CHCFG_DAD                                 (1<<21)
#define UX_RZ_DMA_CHCFG_SAD                                 (1<<20)
#define UX_RZ_DMA_CHCFG_DDS_8                               (0<<16)
#define UX_RZ_DMA_CHCFG_DDS_16                              (1<<16)
#define UX_RZ_DMA_CHCFG_DDS_32                              (2<<16)
#define UX_RZ_DMA_CHCFG_DDS_64                              (3<<16)
#define UX_RZ_DMA_CHCFG_DDS_128                             (4<<16)
#define UX_RZ_DMA_CHCFG_DDS_256                             (5<<16)
#define UX_RZ_DMA_CHCFG_DDS_512                             (6<<16)
#define UX_RZ_DMA_CHCFG_DDS_1024                            (7<<16)
#define UX_RZ_DMA_CHCFG_DDS_MASK                            (0xF<<16)
#define UX_RZ_DMA_CHCFG_SDS_8                               (0<<12)
#define UX_RZ_DMA_CHCFG_SDS_16                              (1<<12)
#define UX_RZ_DMA_CHCFG_SDS_32                              (2<<12)
#define UX_RZ_DMA_CHCFG_SDS_64                              (3<<12)
#define UX_RZ_DMA_CHCFG_SDS_128                             (4<<12)
#define UX_RZ_DMA_CHCFG_SDS_256                             (5<<12)
#define UX_RZ_DMA_CHCFG_SDS_512                             (6<<12)
#define UX_RZ_DMA_CHCFG_SDS_1024                            (7<<12)
#define UX_RZ_DMA_CHCFG_SDS_MASK                            (0xF<<12)
#define UX_RZ_DMA_CHCFG_AM_MASK                             (0x7<<8)
#define UX_RZ_DMA_CHCFG_LVL                                 (1<<6)
#define UX_RZ_DMA_CHCFG_HIEN                                (1<<5)
#define UX_RZ_DMA_CHCFG_LOEN                                (1<<4)
#define UX_RZ_DMA_CHCFG_REQD                                (1<<3)
#define UX_RZ_DMA_CHCFG_SEL_MASK                            (0x7)

#define UX_RZ_DMA_STATUS_IDLE                               0
#define UX_RZ_DMA_STATUS_READING                            1
#define UX_RZ_DMA_STATUS_WRITING                            2
#define UX_RZ_DMA_STATUS_WRITING_SHORT                      3

#endif

/* Define RX command/status bitmaps.  */

#define UX_RZ_DCD_DCP                                       0

#define UX_RZ_DCD_DCPCTR_DATA1                              (1<<7)
#define UX_RZ_DCD_DCPCTR_DATA0                              (1<<8)

/* Define RZ PIPE selection definitions.  */
#define UX_RZ_DCD_PIPESEL_NO_PIPE                           0x000f

/* Define RZ fifo definition.  */
#define UX_RZ_DCD_PIPE0_SIZE                                256
#define UX_RZ_DCD_PIPE_NB_BUFFERS                           64


#define UX_RZ_DCD_SUSPMODE_SUSPM                            (1<<14)

/* Define RZ initialization values.  */

#define UX_RZ_DCD_COMMAND_STATUS_RESET                      0
#define UX_RZ_DCD_INIT_RESET_DELAY                          10
#define UX_RZ_DCD_MAX_BUF_SIZE                              64
#define UX_RZ_DCD_MAX_BUF_NUM                               127

/* Define RZ FIFO write completion code.  */
#define UX_RZ_DCD_FIFO_WRITING                              2
#define UX_RZ_DCD_FIFO_WRITE_END                            3
#define UX_RZ_DCD_FIFO_WRITE_SHORT                          4
#define UX_RZ_DCD_FIFO_WRITE_DMA                            5
#define UX_RZ_DCD_FIFO_WRITE_ERROR                          6

/* Define RZ FIFO read completion code.  */
#define UX_RZ_DCD_FIFO_READING                              2
#define UX_RZ_DCD_FIFO_READ_END                             3
#define UX_RZ_DCD_FIFO_READ_SHORT                           4
#define UX_RZ_DCD_FIFO_READ_DMA                             5
#define UX_RZ_DCD_FIFO_READ_ERROR                           6
#define UX_RZ_DCD_FIFO_READ_OVER                            7

/* Define RZ physical endpoint definitions.  */
#define UX_RZ_DCD_ED_BRDY                                   0x00000001
#define UX_RZ_DCD_ED_NRDY                                   0x00000002
#define UX_RZ_DCD_ED_BEMP                                   0x00000004
#define UX_RZ_DCD_ED_EOFERR                                 0x00000010
#define UX_RZ_DCD_ED_SIGN                                   0x00000020
#define UX_RZ_DCD_ED_SACK                                   0x00000040

/* Define USB RZ physical endpoint status definition.  */

#define UX_DCD_RZ_ED_STATUS_UNUSED                          0
#define UX_DCD_RZ_ED_STATUS_USED                            1
#define UX_DCD_RZ_ED_STATUS_TRANSFER                        2
#define UX_DCD_RZ_ED_STATUS_STALLED                         4

/* Define USB RZ physical endpoint state machine definition.  */

#define UX_DCD_RZ_ED_STATE_IDLE                             0
#define UX_DCD_RZ_ED_STATE_DATA_TX                          1
#define UX_DCD_RZ_ED_STATE_DATA_RX                          2
#define UX_DCD_RZ_ED_STATE_STATUS_TX                        3
#define UX_DCD_RZ_ED_STATE_STATUS_RX                        4

/* Define USB RZ physical endpoint structure.  */

typedef struct UX_DCD_RZ_ED_STRUCT 
{

    ULONG           ux_dcd_rz_ed_status;
    ULONG           ux_dcd_rz_ed_state;
    ULONG           ux_dcd_rz_ed_index;
    ULONG           ux_dcd_rz_ed_fifo_index;
    ULONG           ux_dcd_rz_ed_payload_length;
    ULONG           ux_dcd_rz_ed_packet_length;
    ULONG           ux_dcd_rz_ed_actual_length;
    UCHAR           *ux_dcd_rz_ed_payload_address;
    ULONG           ux_dcd_rz_ed_buffer_number;
    ULONG           ux_dcd_rz_ed_buffer_size;
    struct UX_SLAVE_ENDPOINT_STRUCT             
                    *ux_dcd_rz_ed_endpoint;
} UX_DCD_RZ_ED;


/* Define USB RZ DCD structure definition.  */

typedef struct UX_DCD_RZ_STRUCT
{                                            
    
    struct UX_SLAVE_DCD_STRUCT
                    *ux_dcd_rz_dcd_owner;
    struct UX_DCD_RZ_ED_STRUCT                    
                    ux_dcd_rz_ed[UX_DCD_RZ_MAX_ED];
    ULONG           ux_dcd_rz_pipe[UX_DCD_RZ_MAX_ED];
    ULONG           ux_dcd_rz_base;
    ULONG           ux_dcd_rz_next_available_bufnum;
    ULONG           ux_dcd_rz_debug;
#ifdef UX_RZ_DCD_USE_DMA
    ULONG           ux_dcd_rz_dma_status;
    ULONG           ux_dcd_rz_current_fifo_ctrl;
    TX_SEMAPHORE    *ux_dcd_rz_transfer_semaphore;
#endif
} UX_DCD_RZ;


/* Define USB RZ DCD prototypes.  */

UINT    _ux_dcd_rz_address_set(UX_DCD_RZ *dcd_rz,ULONG address);
#ifdef UX_RZ_DCD_USE_DMA
VOID    _ux_dcd_rz_dma_rx_interrupt_handler(UINT int_sense);
VOID    _ux_dcd_rz_dma_tx_interrupt_handler(UINT int_sense);
VOID    _ux_dcd_rz_dma_register_clear(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
ULONG   _ux_dcd_rz_dma_register_read(UX_DCD_RZ *dcd_rz, ULONG rz_register);
VOID    _ux_dcd_rz_dma_register_set(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
VOID    _ux_dcd_rz_dma_register_write(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
#endif
UINT    _ux_dcd_rz_endpoint_create(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rz_endpoint_destroy(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rz_endpoint_reset(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rz_endpoint_stall(UX_DCD_RZ *dcd_rz, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rz_endpoint_status(UX_DCD_RZ *dcd_rz, ULONG endpoint_index);
UINT    _ux_dcd_rz_frame_number_get(UX_DCD_RZ *dcd_rz, ULONG *frame_number);
UINT    _ux_dcd_rz_function(UX_SLAVE_DCD *dcd, UINT function, VOID *parameter);
UINT    _ux_dcd_rz_initialize(ULONG dcd_io);
UINT    _ux_dcd_rz_initialize_complete(VOID);
VOID    _ux_dcd_rz_interrupt_handler(UINT int_sense);
VOID    _ux_dcd_rz_register_clear(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
ULONG   _ux_dcd_rz_register_read(UX_DCD_RZ *dcd_rz, ULONG rz_register);
VOID    _ux_dcd_rz_register_set(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
VOID    _ux_dcd_rz_register_write(UX_DCD_RZ *dcd_rz, ULONG rz_register, ULONG value);
UINT    _ux_dcd_rz_state_change(UX_DCD_RZ *dcd_rz, ULONG state);
UINT    _ux_dcd_rz_transfer_callback(UX_DCD_RZ *dcd_rz, UX_SLAVE_TRANSFER *transfer_request, 
                                        ULONG interrupt_status, ULONG ctsq_mask);
UINT    _ux_dcd_rz_transfer_request(UX_DCD_RZ *dcd_rz, UX_SLAVE_TRANSFER *transfer_request);
UINT    _ux_dcd_rz_buffer_read(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
UINT    _ux_dcd_rz_fifo_read(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
VOID    _ux_dcd_rz_buffer_notready_interrupt(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG flag);
VOID    _ux_dcd_rz_buffer_empty_interrupt(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG flag);
VOID    _ux_dcd_rz_buffer_ready_interrupt(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG flag);
ULONG   _ux_dcd_rz_fifo_port_change(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG direction);
UINT    _ux_dcd_rz_fifod_write(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
UINT    _ux_dcd_rz_fifoc_write(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
UINT    _ux_dcd_rz_buffer_write(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
ULONG   _ux_dcd_rz_data_buffer_size(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
VOID    _ux_dcd_rz_endpoint_nak_set(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed);
VOID    _ux_dcd_rz_current_endpoint_change(UX_DCD_RZ *dcd_rz, UX_DCD_RZ_ED *ed, ULONG direction);
UINT    _ux_dcd_rz_pipe_select(UX_DCD_RZ *dcd_rz, ULONG ed_index);
void    _ux_dcd_rz_low_level_setup(void);

#ifdef UX_RZ_DCD_USE_DMA
void  BSP_DCache_FlushRange       (void *, unsigned long);
void  BSP_DCache_InvalidateRange  (void *, unsigned long);
#endif

#define ux_dcd_rz_initialize                      _ux_dcd_rz_initialize
#define ux_dcd_rz_interrupt_handler               _ux_dcd_rz_interrupt_handler

#endif

