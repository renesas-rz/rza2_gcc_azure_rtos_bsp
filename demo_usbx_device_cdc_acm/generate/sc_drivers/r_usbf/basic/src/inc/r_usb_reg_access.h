/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2014(2018) Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usb_reg_access.h
 * Description  : USB IP Register control code
 ***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.21 Support DMAC Technical Update for RX71M/RX64M USBA.
 *         : 30.09.2017 1.22 RX65N-2MB is added.
 *         : 20.12.2017 1.23 Delete support for Host 2port.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/
#ifndef HW_USB_REG_ACCESS_H
#define HW_USB_REG_ACCESS_H

/******************************************************************************
 Macro definitions
 ******************************************************************************/
#define USB_BUFSIZE_BIT   (10u)
#define USB_SUSPEND_MODE  (1u)
#define USB_NORMAL_MODE   (0)

   #if USB_CFG_ENDIAN == USB_CFG_LITTLE
#define USB0_CFIFO8     (USB_M0.CFIFO.BYTE.LL)
#define USB0_CFIFO16    (USB_M0.CFIFO.WORD.L)
   #endif /* USB_CFG_ENDIAN == USB_CFG_LITTLE */

   #if USB_CFG_ENDIAN == USB_CFG_BIG
#define USB0_CFIFO8     (USB_M0.CFIFO.BYTE.HH)
#define USB0_CFIFO16    (USB_M0.CFIFO.WORD.H)
   #endif /* USB_CFG_ENDIAN == USB_CFG_BIG */

   #if USB_CFG_ENDIAN == USB_CFG_LITTLE
#define USB1_CFIFO8     (USB_M1.CFIFO.BYTE.LL)
#define USB1_CFIFO16    (USB_M1.CFIFO.WORD.L)
   #endif /* USB_CFG_ENDIAN == USB_CFG_LITTLE */

   #if USB_CFG_ENDIAN == USB_CFG_BIG
#define USB1_CFIFO8     (USB_M1.CFIFO.BYTE.HH)
#define USB1_CFIFO16    (USB_M1.CFIFO.WORD.H)
   #endif /* USB_CFG_ENDIAN == USB_CFG_BIG */

#define USB1_CFIFO32    (USB_M1.CFIFO.LONG)
#define USB1_D0FIFO32   (USB_M1.D0FIFO.LONG)
#define USB1_D1FIFO32   (USB_M1.D1FIFO.LONG)

/****************/
/*  INITIARIZE  */
/****************/
void         hw_usb_pmodule_init( void );


/************/
/*  SYSCFG  */
/************/
uint16_t hw_usb_read_syscfg (usb_utr_t *ptr);
void hw_usb_write_syscfg (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_cnen (usb_utr_t *ptr);
void hw_usb_set_hse (usb_utr_t *ptr);
void hw_usb_clear_hse (usb_utr_t *ptr);
void hw_usb_set_dcfm (void);
void hw_usb_clear_dcfm (usb_utr_t *ptr);
void hw_usb_clear_drpd (usb_utr_t *ptr);
void hw_usb_set_usbe (usb_utr_t *ptr);
void hw_usb_clear_usbe (usb_utr_t *ptr);
void hw_usb_set_cnen (void);
void hw_usb_pset_dprpu (void);
void hw_usb_pclear_dprpu (void);
void hw_usb_pclear_drpd(void);


/************/
/*  BUSWAIT */
/************/
void hw_usb_set_buswait (usb_utr_t *ptr);

/************/
/*  SYSSTS0 */
/************/
uint16_t hw_usb_read_syssts (usb_utr_t *ptr);

/**************/
/*  DVSTCTR0  */
/**************/
uint16_t hw_usb_read_dvstctr (usb_utr_t *ptr);
void hw_usb_write_dvstctr (usb_utr_t *ptr, uint16_t data);
void hw_usb_rmw_dvstctr (usb_utr_t *ptr, uint16_t data, uint16_t width);
void hw_usb_clear_dvstctr (usb_utr_t *ptr, uint16_t data);
void hw_usb_set_vbout (usb_utr_t *ptr);
void hw_usb_clear_vbout (usb_utr_t *ptr);
void hw_usb_pset_wkup (void);

/**************/
/*  TESTMODE  */
/**************/
void hw_usb_set_utst (usb_utr_t *ptr, uint16_t data);

/***************************/
/*  CFIFO, D0FIFO, D1FIFO  */
/***************************/
uint32_t hw_usb_read_fifo32 (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_write_fifo32 (usb_utr_t *ptr, uint16_t pipemode, uint32_t data);
uint16_t hw_usb_read_fifo16 (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_write_fifo16 (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);
void hw_usb_write_fifo8 (usb_utr_t *ptr, uint16_t pipemode, uint8_t data);

/************************************/
/*  CFIFOSEL, D0FIFOSEL, D1FIFOSEL  */
/************************************/
uint16_t hw_usb_read_fifosel (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_rmw_fifosel (usb_utr_t *ptr, uint16_t pipemode, uint16_t data, uint16_t width);
void hw_usb_set_dclrm (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_clear_dclrm (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_set_dreqe (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_clear_dreqe (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_set_mbw (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);
void hw_usb_set_curpipe (usb_utr_t *ptr, uint16_t pipemode, uint16_t pipeno);

/**********************************/
/* CFIFOCTR, D0FIFOCTR, D1FIFOCTR */
/**********************************/
uint16_t hw_usb_read_fifoctr (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_set_bval (usb_utr_t *ptr, uint16_t pipemode);
void hw_usb_set_bclr (usb_utr_t *ptr, uint16_t pipemode);

/*************/
/*  INTENB0  */
/*************/
uint16_t hw_usb_ReadIntenb (usb_utr_t *ptr);
void hw_usb_write_intenb (usb_utr_t *ptr, uint16_t data);
void hw_usb_set_intenb (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_enb_vbse (usb_utr_t *ptr);
void hw_usb_clear_enb_sofe (usb_utr_t *ptr);
void hw_usb_pset_enb_rsme (void);
void hw_usb_pclear_enb_rsme (void);

/*************/
/*  BRDYENB  */
/*************/
void hw_usb_write_brdyenb (usb_utr_t *ptr, uint16_t data);
void hw_usb_set_brdyenb (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_clear_brdyenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  NRDYENB  */
/*************/
void hw_usb_write_nrdyenb (usb_utr_t *ptr, uint16_t data);
void hw_usb_set_nrdyenb (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_clear_nrdyenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  BEMPENB  */
/*************/
void hw_usb_write_bempenb (usb_utr_t *ptr, uint16_t data);
void hw_usb_set_bempenb (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_clear_bempenb (usb_utr_t *ptr, uint16_t pipeno);

/*************/
/*  SOFCFG   */
/*************/
void hw_usb_set_sofcfg (usb_utr_t *ptr, uint16_t data);

/*************/
/*  INTSTS0  */
/*************/
void hw_usb_write_intsts (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_sts_sofr (usb_utr_t *ptr);
uint16_t hw_usb_read_intsts (void);
void hw_usb_pclear_sts_resm (void);
void hw_usb_pclear_sts_valid (void);

/************/
/* BRDYSTS  */
/************/
uint16_t hw_usb_read_brdysts (usb_utr_t *ptr);
void hw_usb_write_brdysts (usb_utr_t *pt, uint16_t data);
void hw_usb_clear_sts_brdy (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* NRDYSTS  */
/************/
void hw_usb_write_nrdy_sts (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_status_nrdy (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* BEMPSTS  */
/************/
void hw_usb_write_bempsts (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_status_bemp (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* FRMNUM   */
/************/
uint16_t hw_usb_read_frmnum (usb_utr_t *ptr);

/************/
/* USBREQ   */
/************/
uint16_t hw_usb_read_usbreq (void);

/************/
/* USBVAL   */
/************/
uint16_t hw_usb_read_usbval (void);

/************/
/* USBINDX  */
/************/
uint16_t hw_usb_read_usbindx (void);

/************/
/* USBLENG  */
/************/
uint16_t hw_usb_read_usbleng (void);

/************/
/* DCPCFG   */
/************/
uint16_t hw_usb_read_dcpcfg (usb_utr_t *ptr);
void hw_usb_write_dcpcfg (usb_utr_t *ptr, uint16_t data);

/************/
/* DCPMAXP  */
/************/
uint16_t hw_usb_read_dcpmaxp (usb_utr_t *ptr);
void hw_usb_write_dcpmxps (usb_utr_t *ptr, uint16_t data);

/************/
/* DCPCTR   */
/************/
uint16_t hw_usb_read_dcpctr (void);
void hw_usb_pset_ccpl (void);

/************/
/* PIPESEL  */
/************/
void hw_usb_write_pipesel (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPECFG  */
/************/
uint16_t hw_usb_read_pipecfg (usb_utr_t *ptr);
void hw_usb_write_pipecfg (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPEBUF  */
/************/
void hw_usb_write_pipebuf (usb_utr_t *ptr, uint16_t data);
uint16_t hw_usb_read_pipebuf (usb_utr_t *ptr);

/************/
/* PIPEMAXP */
/************/
uint16_t hw_usb_read_pipemaxp (usb_utr_t *ptr);
void hw_usb_write_pipemaxp (usb_utr_t *ptr, uint16_t data);

/************/
/* PIPEPERI */
/************/
void hw_usb_write_pipeperi (usb_utr_t *ptr, uint16_t data);

/********************/
/* DCPCTR, PIPEnCTR */
/********************/
uint16_t hw_usb_read_pipectr (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_write_pipectr (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);
void hw_usb_set_csclr (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_set_aclrm (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_clear_aclrm (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_set_sqclr (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_set_sqset (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_set_pid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);
void hw_usb_clear_pid (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);

/************/
/* PIPEnTRE */
/************/
void hw_usb_set_trenb (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_clear_trenb (usb_utr_t *ptr, uint16_t pipeno);
void hw_usb_set_trclr (usb_utr_t *ptr, uint16_t pipeno);

/************/
/* PIPEnTRN */
/************/
void hw_usb_write_pipetrn (usb_utr_t *ptr, uint16_t pipeno, uint16_t data);

/************/
/* BCCTRL   */
/************/
void hw_usb_set_bcctrl (usb_utr_t *ptr, uint16_t data);
void hw_usb_clear_bcctrl (usb_utr_t *ptr, uint16_t data);
uint16_t hw_usb_read_bcctrl (usb_utr_t *ptr);
void hw_usb_set_vdmsrce (usb_utr_t *ptr);
void hw_usb_clear_vdmsrce (usb_utr_t *ptr);
void hw_usb_set_idpsinke (usb_utr_t *ptr);
void hw_usb_set_suspendm(void);
void hw_usb_clear_suspm(void);
void hw_usb_clear_idpsinke (usb_utr_t *ptr);

/**********************************/
/*  DMA0CFG, DMA1CFG  for 597ASSP */
/**********************************/
void hw_usb_write_dmacfg (usb_utr_t *ptr, uint16_t pipemode, uint16_t data);


#endif /* HW_USB_REG_ACCESS_H */
/******************************************************************************
 End of file
 ******************************************************************************/
