/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2014(2015-2016) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name    : r_dmaca_rx_target.h
* Description  : Functions for DMACA driver
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.06.2014 1.00    Initial revision
*         : 27.08.2014 1.01    Not change program due to updating application note only.
*         : 30.01.2015 1.02    Added RX71M.
*         : 13.04.2015 1.03    Added RX231.
*         : 30.09.2016 1.04    Added #define DMACA_INVALID_LOWER_BIT_MASK.
*         :                    Comment out #define DMACA_NUM_INTERRUPT_SRC
*         :                    because this FIT module not use it.
*******************************************************************************/
#ifndef DMACA_RX_TARGET_H
#define DMACA_RX_TARGET_H

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/

/*******************************************************************************
Macro definitions
*******************************************************************************/
/* Number of activation sources */
/* #define DMACA_NUM_INTERRUPT_SRC         (194) */

/* Maximum value of 16bit count value (normal transfer mode) */
#define DMACA_MAX_16BITS_COUNT_VAL_A    (65535)

/* Definition of number of DMACA channel */
#define DMACA_NUM_CHANNELS              (2)

/* Channel Configuration Register .b15:SDS[3] bit mask */
#define DMACA_INVALID_SRC_SKIP_MASK     (0x1 << 15)
/* Channel Configuration Register .b14-12:SDS[2:0] bit mask */
#define DMACA_INVALID_SRC_SIZE_MASK     (0x7 << 12)
/* Channel Configuration Register .b19:DDS[3] bit mask */
#define DMACA_INVALID_DES_SKIP_MASK     (0x1 << 19)
/* Channel Configuration Register .b18-16:DDS[2:0] bit mask */
#define DMACA_INVALID_DES_SIZE_MASK     (0x7 << 16)
/* CHITVL Register.b15-00:ITVL[15:00]  Channel Interval mask */
#define DMACA_INVALID_INTERVAL_MAX      0xffff

/* IP base address */
#define USB_BASE_ADDR               ((uint32_t*)&USB01 + ((uint32_t*)&USB11-(uint32_t*)&USB01)*USB_CFG_USE_USBIP)
/* DMAC register base address 1 :  N0SA - CRLA */
#define DMACA_BASE_ADDR1            ((uint32_t*)&USB01.N0SA_1.LONG - (uint32_t*)&USB01.N0SA_0.LONG)
/* DMAC register base address 2 :  SCNT - DSKP */
#define DMACA_BASE_ADDR2            ((uint32_t*)&USB01.SCNT_1.LONG - (uint32_t*)&USB01.SCNT_0.LONG)
/* DMAC register offset address */
/* N0SA register offset address */
#define DMACA_N0SA_OFST             ((uint32_t*)&USB01.N0SA_0.LONG - (uint32_t*)&USB01)
/* N0DA register offset address */
#define DMACA_N0DA_OFST             ((uint32_t*)&USB01.N0DA_0.LONG - (uint32_t*)&USB01)
/* N0TB register offset address */
#define DMACA_N0TB_OFST             ((uint32_t*)&USB01.N0TB_0.LONG - (uint32_t*)&USB01)
/* N1SA register offset address */
#define DMACA_N1SA_OFST             ((uint32_t*)&USB01.N1SA_0.LONG - (uint32_t*)&USB01)
/* N1DA register offset address */
#define DMACA_N1DA_OFST             ((uint32_t*)&USB01.N1DA_0.LONG - (uint32_t*)&USB01)
/* N1TB register offset address */
#define DMACA_N1TB_OFST             ((uint32_t*)&USB01.N1TB_0.LONG - (uint32_t*)&USB01)
/* CRSA register offset address */
#define DMACA_CRSA_OFST             ((uint32_t*)&USB01.CRSA_0.LONG - (uint32_t*)&USB01)
/* CRDA register offset address */
#define DMACA_CRDA_OFST             ((uint32_t*)&USB01.CRDA_0.LONG - (uint32_t*)&USB01)
/* CRTB register offset address */
#define DMACA_CRTB_OFST             ((uint32_t*)&USB01.CRTB_0.LONG - (uint32_t*)&USB01)
/* CHSTAT register offset address */
#define DMACA_CHSTAT_OFST           ((uint32_t*)&USB01.CHSTAT_0.LONG - (uint32_t*)&USB01)
/* CHCTRL register offset address */
#define DMACA_CHCTRL_OFST           ((uint32_t*)&USB01.CHCTRL_0.LONG - (uint32_t*)&USB01)
/* CHCFG register offset address */
#define DMACA_CHCFG_OFST            ((uint32_t*)&USB01.CHCFG_0.LONG - (uint32_t*)&USB01)
/* CHITVL register offset address */
#define DMACA_CHITVL_OFST           ((uint32_t*)&USB01.CHITVL_0.LONG - (uint32_t*)&USB01)
/* CHEXT register offset address */
#define DMACA_CHEXT_OFST            ((uint32_t*)&USB01.CHEXT_0.LONG - (uint32_t*)&USB01)
/* NXLA register offset address */
#define DMACA_NXLA_OFST             ((uint32_t*)&USB01.NXLA_0.LONG - (uint32_t*)&USB01)
/* CRLA register offset address */
#define DMACA_CRLA_OFST             ((uint32_t*)&USB01.CRLA_0.LONG - (uint32_t*)&USB01)
/* SCNT register offset address */
#define DMACA_SCNT_OFST             ((uint32_t*)&USB01.SCNT_0.LONG - (uint32_t*)&USB01)
/* SSKP register offset address */
#define DMACA_SSKP_OFST             ((uint32_t*)&USB01.SSKP_0.LONG - (uint32_t*)&USB01)
/* DCNT register offset address */
#define DMACA_DCNT_OFST             ((uint32_t*)&USB01.DCNT_0.LONG - (uint32_t*)&USB01)
/* DSKP register offset address */
#define DMACA_DSKP_OFST             ((uint32_t*)&USB01.DSKP_0.LONG - (uint32_t*)&USB01)
/* DCTRL register offset address */
#define DMACA_DCTRL_OFST            ((uint32_t*)&USB01.DCTRL.LONG - (uint32_t*)&USB01)
/* DSCITVL register offset address */
#define DMACA_DSCITVL_OFST          ((uint32_t*)&USB01.DSCITVL.LONG - (uint32_t*)&USB01)
/* DST_EN register offset address */
#define DMACA_DST_EN_OFST           ((uint32_t*)&USB01.DST_EN.LONG - (uint32_t*)&USB01)
/* DST_ER register offset address */
#define DMACA_DST_ER_OFST           ((uint32_t*)&USB01.DST_ER.LONG - (uint32_t*)&USB01)
/* DST_END register offset address */
#define DMACA_DST_END_OFST          ((uint32_t*)&USB01.DST_END.LONG - (uint32_t*)&USB01)
/* DST_TC register offset address */
#define DMACA_DST_TC_OFST           ((uint32_t*)&USB01.DST_TC.LONG - (uint32_t*)&USB01)
/* DST_SUS register offset address */
#define DMACA_DST_SUS_OFST          ((uint32_t*)&USB01.DST_SUS.LONG - (uint32_t*)&USB01)
/* DMAC register access macro */
/* N0SA register */
#define DMACA_N0SA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N0SA_OFST))
/* N0DA register */
#define DMACA_N0DA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N0DA_OFST))
/* N0TB register */
#define DMACA_N0TB(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N0TB_OFST))
/* N1SA register */
#define DMACA_N1SA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N1SA_OFST))
/* N1DA register */
#define DMACA_N1DA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N1DA_OFST))
/* N1TB register */
#define DMACA_N1TB(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_N1TB_OFST))
/* CRSA register */
#define DMACA_CRSA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CRSA_OFST))
/* CRDA register */
#define DMACA_CRDA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CRDA_OFST))
/* CRTB register */
#define DMACA_CRTB(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CRTB_OFST))
/* CHSTAT register */
#define DMACA_CHSTAT(dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CHSTAT_OFST))
/* CHCTRL register */
#define DMACA_CHCTRL(dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CHCTRL_OFST))
/* CHCFG register */
#define DMACA_CHCFG(dmaca_channel)  (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CHCFG_OFST))
/* CHITVL register */
#define DMACA_CHITVL(dmaca_channel) (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CHITVL_OFST))
/* CHEXT register */
#define DMACA_CHEXT(dmaca_channel)  (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CHEXT_OFST))
/* NXLA register */
#define DMACA_NXLA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_NXLA_OFST))
/* CRLA register */
#define DMACA_CRLA(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR1*dmaca_channel + DMACA_CRLA_OFST))
/* SCNT register */
#define DMACA_SCNT(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR2*dmaca_channel + DMACA_SCNT_OFST))
/* SSKP register */
#define DMACA_SSKP(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR2*dmaca_channel + DMACA_SSKP_OFST))
/* DCNT register */
#define DMACA_DCNT(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR2*dmaca_channel + DMACA_DCNT_OFST))
/* DSKP register */
#define DMACA_DSKP(dmaca_channel)   (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_BASE_ADDR2*dmaca_channel + DMACA_DSKP_OFST))
/* DCTRL register */
#define DMACA_DCTRL                 (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DCTRL_OFST))
/* DSCITVL register */
#define DMACA_DSCITVL               (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DSCITVL_OFST))
/* DST_EN register */
#define DMACA_DST_EN                (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DST_EN_OFST))
/* DST_ER register */
#define DMACA_DST_ER                (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DST_ER_OFST))
/* DST_END register */
#define DMACA_DST_END               (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DST_END_OFST))
/* DST_TC register */
#define DMACA_DST_TC                (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DST_TC_OFST))
/* DST_SUS register */
#define DMACA_DST_SUS               (*(volatile uint32_t *)(USB_BASE_ADDR + DMACA_DST_SUS_OFST))


#endif /* DMACA_RX_TARGET_H */

/* End of File */

