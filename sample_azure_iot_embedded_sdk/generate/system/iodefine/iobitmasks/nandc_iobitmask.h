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
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/ 
/*******************************************************************************
* Rev: 3.01
* Description : IO bitmask header
*******************************************************************************/

#ifndef NANDC_IOBITMASK_H
#define NANDC_IOBITMASK_H


/* ==== Mask values for IO registers ==== */

#define NANDC_COMMAND_CMD_SEQ                                                  (0x0000003Fu)
#define NANDC_COMMAND_CMD_SEQ_SHIFT                                            (0u)
#define NANDC_COMMAND_INPUT_SEL                                                (0x00000040u)
#define NANDC_COMMAND_INPUT_SEL_SHIFT                                          (6u)
#define NANDC_COMMAND_DATA_SEL                                                 (0x00000080u)
#define NANDC_COMMAND_DATA_SEL_SHIFT                                           (7u)
#define NANDC_COMMAND_CMD_0                                                    (0x0000FF00u)
#define NANDC_COMMAND_CMD_0_SHIFT                                              (8u)
#define NANDC_COMMAND_CMD_1                                                    (0x00FF0000u)
#define NANDC_COMMAND_CMD_1_SHIFT                                              (16u)
#define NANDC_COMMAND_CMD_2                                                    (0xFF000000u)
#define NANDC_COMMAND_CMD_2_SHIFT                                              (24u)
#define NANDC_CONTROL_READ_STATUS_EN                                           (0x00000001u)
#define NANDC_CONTROL_READ_STATUS_EN_SHIFT                                     (0u)
#define NANDC_CONTROL_ECC_BLOCK_SIZE                                           (0x00000006u)
#define NANDC_CONTROL_ECC_BLOCK_SIZE_SHIFT                                     (1u)
#define NANDC_CONTROL_INT_EN                                                   (0x00000010u)
#define NANDC_CONTROL_INT_EN_SHIFT                                             (4u)
#define NANDC_CONTROL_ECC_EN                                                   (0x00000020u)
#define NANDC_CONTROL_ECC_EN_SHIFT                                             (5u)
#define NANDC_CONTROL_BLOCK_SIZE                                               (0x000000C0u)
#define NANDC_CONTROL_BLOCK_SIZE_SHIFT                                         (6u)
#define NANDC_CONTROL_BBM_EN                                                   (0x00002000u)
#define NANDC_CONTROL_BBM_EN_SHIFT                                             (13u)
#define NANDC_CONTROL_PROT_EN                                                  (0x00004000u)
#define NANDC_CONTROL_PROT_EN_SHIFT                                            (14u)
#define NANDC_CONTROL_ADDR0_AUTO_INCR                                          (0x00010000u)
#define NANDC_CONTROL_ADDR0_AUTO_INCR_SHIFT                                    (16u)
#define NANDC_CONTROL_ADDR1_AUTO_INCR                                          (0x00020000u)
#define NANDC_CONTROL_ADDR1_AUTO_INCR_SHIFT                                    (17u)
#define NANDC_CONTROL_SMALL_BLOCK_EN                                           (0x00200000u)
#define NANDC_CONTROL_SMALL_BLOCK_EN_SHIFT                                     (21u)
#define NANDC_CONTROL_MLUN_EN                                                  (0x00400000u)
#define NANDC_CONTROL_MLUN_EN_SHIFT                                            (22u)
#define NANDC_CONTROL_AUTO_READ_STAT_EN                                        (0x00800000u)
#define NANDC_CONTROL_AUTO_READ_STAT_EN_SHIFT                                  (23u)
#define NANDC_STATUS_MEM0_ST                                                   (0x00000001u)
#define NANDC_STATUS_MEM0_ST_SHIFT                                             (0u)
#define NANDC_STATUS_CTRL_STAT                                                 (0x00000100u)
#define NANDC_STATUS_CTRL_STAT_SHIFT                                           (8u)
#define NANDC_STATUS_DATASIZE_ERROR_ST                                         (0x00000200u)
#define NANDC_STATUS_DATASIZE_ERROR_ST_SHIFT                                   (9u)
#define NANDC_STATUS_DATA_REG_ST                                               (0x00000400u)
#define NANDC_STATUS_DATA_REG_ST_SHIFT                                         (10u)
#define NANDC_STATUS_CMD_ID                                                    (0x00FF0000u)
#define NANDC_STATUS_CMD_ID_SHIFT                                              (16u)
#define NANDC_STATUS_MASK_STATE_MASK                                           (0x000000FFu)
#define NANDC_STATUS_MASK_STATE_MASK_SHIFT                                     (0u)
#define NANDC_STATUS_MASK_ERROR_MASK                                           (0x0000FF00u)
#define NANDC_STATUS_MASK_ERROR_MASK_SHIFT                                     (8u)
#define NANDC_INT_MASK_PROT_INT_EN                                             (0x00000001u)
#define NANDC_INT_MASK_PROT_INT_EN_SHIFT                                       (0u)
#define NANDC_INT_MASK_CMD_END_INT_EN                                          (0x00000002u)
#define NANDC_INT_MASK_CMD_END_INT_EN_SHIFT                                    (1u)
#define NANDC_INT_MASK_DATA_REG_INT_EN                                         (0x00000004u)
#define NANDC_INT_MASK_DATA_REG_INT_EN_SHIFT                                   (2u)
#define NANDC_INT_MASK_DMA_INT_EN                                              (0x00000008u)
#define NANDC_INT_MASK_DMA_INT_EN_SHIFT                                        (3u)
#define NANDC_INT_MASK_TRANS_ERR_EN                                            (0x00000010u)
#define NANDC_INT_MASK_TRANS_ERR_EN_SHIFT                                      (4u)
#define NANDC_INT_MASK_PG_SZ_ERR_INT_EN                                        (0x00000040u)
#define NANDC_INT_MASK_PG_SZ_ERR_INT_EN_SHIFT                                  (6u)
#define NANDC_INT_MASK_MEM0_RDY_INT_EN                                         (0x00000100u)
#define NANDC_INT_MASK_MEM0_RDY_INT_EN_SHIFT                                   (8u)
#define NANDC_INT_MASK_STAT_ERR_INT0_EN                                        (0x00010000u)
#define NANDC_INT_MASK_STAT_ERR_INT0_EN_SHIFT                                  (16u)
#define NANDC_INT_MASK_ECC_INT0_EN                                             (0x01000000u)
#define NANDC_INT_MASK_ECC_INT0_EN_SHIFT                                       (24u)
#define NANDC_INT_STATUS_PROT_INT_FL                                           (0x00000001u)
#define NANDC_INT_STATUS_PROT_INT_FL_SHIFT                                     (0u)
#define NANDC_INT_STATUS_CMD_END_INT_FL                                        (0x00000002u)
#define NANDC_INT_STATUS_CMD_END_INT_FL_SHIFT                                  (1u)
#define NANDC_INT_STATUS_DATA_REG_INT_FL                                       (0x00000004u)
#define NANDC_INT_STATUS_DATA_REG_INT_FL_SHIFT                                 (2u)
#define NANDC_INT_STATUS_DMA_INT_FL                                            (0x00000008u)
#define NANDC_INT_STATUS_DMA_INT_FL_SHIFT                                      (3u)
#define NANDC_INT_STATUS_TRANS_ERR_FL                                          (0x00000010u)
#define NANDC_INT_STATUS_TRANS_ERR_FL_SHIFT                                    (4u)
#define NANDC_INT_STATUS_PG_SZ_ERR_INT_FL                                      (0x00000040u)
#define NANDC_INT_STATUS_PG_SZ_ERR_INT_FL_SHIFT                                (6u)
#define NANDC_INT_STATUS_MEM0_RDY_INT_FL                                       (0x00000100u)
#define NANDC_INT_STATUS_MEM0_RDY_INT_FL_SHIFT                                 (8u)
#define NANDC_INT_STATUS_STAT_ERR_INT0_FL                                      (0x00010000u)
#define NANDC_INT_STATUS_STAT_ERR_INT0_FL_SHIFT                                (16u)
#define NANDC_INT_STATUS_ECC_INT0_FL                                           (0x01000000u)
#define NANDC_INT_STATUS_ECC_INT0_FL_SHIFT                                     (24u)
#define NANDC_ECC_CTRL_ECC_CAP                                                 (0x00000007u)
#define NANDC_ECC_CTRL_ECC_CAP_SHIFT                                           (0u)
#define NANDC_ECC_CTRL_ERR_THRESHOLD                                           (0x00003F00u)
#define NANDC_ECC_CTRL_ERR_THRESHOLD_SHIFT                                     (8u)
#define NANDC_ECC_CTRL_ECC_SEL                                                 (0x00030000u)
#define NANDC_ECC_CTRL_ECC_SEL_SHIFT                                           (16u)
#define NANDC_ECC_OFFSET_ECC_OFFSET                                            (0x0000FFFFu)
#define NANDC_ECC_OFFSET_ECC_OFFSET_SHIFT                                      (0u)
#define NANDC_ECC_STAT_ECC_ERROR_0                                             (0x00000001u)
#define NANDC_ECC_STAT_ECC_ERROR_0_SHIFT                                       (0u)
#define NANDC_ECC_STAT_ECC_UNC_0                                               (0x00000100u)
#define NANDC_ECC_STAT_ECC_UNC_0_SHIFT                                         (8u)
#define NANDC_ECC_STAT_ECC_OVER_0                                              (0x00010000u)
#define NANDC_ECC_STAT_ECC_OVER_0_SHIFT                                        (16u)
#define NANDC_ADDR0_COL_ADDR0_COL                                              (0x0000FFFFu)
#define NANDC_ADDR0_COL_ADDR0_COL_SHIFT                                        (0u)
#define NANDC_ADDR0_ROW_ADDR0_ROW                                              (0x00FFFFFFu)
#define NANDC_ADDR0_ROW_ADDR0_ROW_SHIFT                                        (0u)
#define NANDC_ADDR1_COL_ADDR1_COL                                              (0x0000FFFFu)
#define NANDC_ADDR1_COL_ADDR1_COL_SHIFT                                        (0u)
#define NANDC_ADDR1_ROW_ADDR1_ROW                                              (0x00FFFFFFu)
#define NANDC_ADDR1_ROW_ADDR1_ROW_SHIFT                                        (0u)
#define NANDC_FIFO_DATA_FIFO_DATA                                              (0xFFFFFFFFu)
#define NANDC_FIFO_DATA_FIFO_DATA_SHIFT                                        (0u)
#define NANDC_DATA_REG_DATA_REG                                                (0xFFFFFFFFu)
#define NANDC_DATA_REG_DATA_REG_SHIFT                                          (0u)
#define NANDC_DATA_REG_SIZE_DATA_REG_SIZE                                      (0x00000003u)
#define NANDC_DATA_REG_SIZE_DATA_REG_SIZE_SHIFT                                (0u)
#define NANDC_DEV0_PTR_PTR_ADDR                                                (0x00000FFCu)
#define NANDC_DEV0_PTR_PTR_ADDR_SHIFT                                          (2u)
#define NANDC_DMA_ADDR_L_DMA_ADDR_L                                            (0xFFFFFFFFu)
#define NANDC_DMA_ADDR_L_DMA_ADDR_L_SHIFT                                      (0u)
#define NANDC_DMA_CNT_CNT_INIT                                                 (0xFFFFFFFFu)
#define NANDC_DMA_CNT_CNT_INIT_SHIFT                                           (0u)
#define NANDC_DMA_CTRL_DMA_READY                                               (0x00000001u)
#define NANDC_DMA_CTRL_DMA_READY_SHIFT                                         (0u)
#define NANDC_DMA_CTRL_DMA_BURST                                               (0x0000001Cu)
#define NANDC_DMA_CTRL_DMA_BURST_SHIFT                                         (2u)
#define NANDC_DMA_CTRL_DMA_MODE                                                (0x00000020u)
#define NANDC_DMA_CTRL_DMA_MODE_SHIFT                                          (5u)
#define NANDC_DMA_CTRL_DMA_START                                               (0x00000080u)
#define NANDC_DMA_CTRL_DMA_START_SHIFT                                         (7u)
#define NANDC_BBM_CTRL_RMP_INIT                                                (0x00000001u)
#define NANDC_BBM_CTRL_RMP_INIT_SHIFT                                          (0u)
#define NANDC_DATA_SIZE_DATA_SIZE                                              (0x00007FFFu)
#define NANDC_DATA_SIZE_DATA_SIZE_SHIFT                                        (0u)
#define NANDC_TIMINGS_ASYN_TRWP                                                (0x0000000Fu)
#define NANDC_TIMINGS_ASYN_TRWP_SHIFT                                          (0u)
#define NANDC_TIMINGS_ASYN_TRWH                                                (0x000000F0u)
#define NANDC_TIMINGS_ASYN_TRWH_SHIFT                                          (4u)
#define NANDC_TIME_SEQ_0_TCCS                                                  (0x0000003Fu)
#define NANDC_TIME_SEQ_0_TCCS_SHIFT                                            (0u)
#define NANDC_TIME_SEQ_0_TADL                                                  (0x00003F00u)
#define NANDC_TIME_SEQ_0_TADL_SHIFT                                            (8u)
#define NANDC_TIME_SEQ_0_TRHW                                                  (0x003F0000u)
#define NANDC_TIME_SEQ_0_TRHW_SHIFT                                            (16u)
#define NANDC_TIME_SEQ_0_TWHR                                                  (0x3F000000u)
#define NANDC_TIME_SEQ_0_TWHR_SHIFT                                            (24u)
#define NANDC_TIME_SEQ_1_TWB                                                   (0x0000003Fu)
#define NANDC_TIME_SEQ_1_TWB_SHIFT                                             (0u)
#define NANDC_TIME_SEQ_1_TRR                                                   (0x00003F00u)
#define NANDC_TIME_SEQ_1_TRR_SHIFT                                             (8u)
#define NANDC_TIME_GEN_SEQ_0_t0_d0                                             (0x0000003Fu)
#define NANDC_TIME_GEN_SEQ_0_t0_d0_SHIFT                                       (0u)
#define NANDC_TIME_GEN_SEQ_0_t0_d1                                             (0x00003F00u)
#define NANDC_TIME_GEN_SEQ_0_t0_d1_SHIFT                                       (8u)
#define NANDC_TIME_GEN_SEQ_0_t0_d2                                             (0x003F0000u)
#define NANDC_TIME_GEN_SEQ_0_t0_d2_SHIFT                                       (16u)
#define NANDC_TIME_GEN_SEQ_0_t0_d3                                             (0x3F000000u)
#define NANDC_TIME_GEN_SEQ_0_t0_d3_SHIFT                                       (24u)
#define NANDC_TIME_GEN_SEQ_1_t0_d4                                             (0x0000003Fu)
#define NANDC_TIME_GEN_SEQ_1_t0_d4_SHIFT                                       (0u)
#define NANDC_TIME_GEN_SEQ_1_t0_d5                                             (0x00003F00u)
#define NANDC_TIME_GEN_SEQ_1_t0_d5_SHIFT                                       (8u)
#define NANDC_TIME_GEN_SEQ_1_t0_d6                                             (0x003F0000u)
#define NANDC_TIME_GEN_SEQ_1_t0_d6_SHIFT                                       (16u)
#define NANDC_TIME_GEN_SEQ_1_t0_d7                                             (0x3F000000u)
#define NANDC_TIME_GEN_SEQ_1_t0_d7_SHIFT                                       (24u)
#define NANDC_TIME_GEN_SEQ_2_t0_d8                                             (0x0000003Fu)
#define NANDC_TIME_GEN_SEQ_2_t0_d8_SHIFT                                       (0u)
#define NANDC_TIME_GEN_SEQ_2_t0_d9                                             (0x00003F00u)
#define NANDC_TIME_GEN_SEQ_2_t0_d9_SHIFT                                       (8u)
#define NANDC_TIME_GEN_SEQ_2_t0_d10                                            (0x003F0000u)
#define NANDC_TIME_GEN_SEQ_2_t0_d10_SHIFT                                      (16u)
#define NANDC_TIME_GEN_SEQ_2_t0_d11                                            (0x3F000000u)
#define NANDC_TIME_GEN_SEQ_2_t0_d11_SHIFT                                      (24u)
#define NANDC_FIFO_INIT_FIFO_INIT                                              (0x00000001u)
#define NANDC_FIFO_INIT_FIFO_INIT_SHIFT                                        (0u)
#define NANDC_FIFO_STATE_DF_R_EMPTY                                            (0x00000001u)
#define NANDC_FIFO_STATE_DF_R_EMPTY_SHIFT                                      (0u)
#define NANDC_FIFO_STATE_DF_W_FULL                                             (0x00000002u)
#define NANDC_FIFO_STATE_DF_W_FULL_SHIFT                                       (1u)
#define NANDC_FIFO_STATE_CF_EMPTY                                              (0x00000004u)
#define NANDC_FIFO_STATE_CF_EMPTY_SHIFT                                        (2u)
#define NANDC_FIFO_STATE_CF_FULL                                               (0x00000008u)
#define NANDC_FIFO_STATE_CF_FULL_SHIFT                                         (3u)
#define NANDC_FIFO_STATE_CF_ACCPT_R                                            (0x00000010u)
#define NANDC_FIFO_STATE_CF_ACCPT_R_SHIFT                                      (4u)
#define NANDC_FIFO_STATE_CF_ACCPT_W                                            (0x00000020u)
#define NANDC_FIFO_STATE_CF_ACCPT_W_SHIFT                                      (5u)
#define NANDC_FIFO_STATE_DF_R_FULL                                             (0x00000040u)
#define NANDC_FIFO_STATE_DF_R_FULL_SHIFT                                       (6u)
#define NANDC_FIFO_STATE_DF_W_EMPTY                                            (0x00000080u)
#define NANDC_FIFO_STATE_DF_W_EMPTY_SHIFT                                      (7u)
#define NANDC_GEN_SEQ_CTRL_CMD0_EN                                             (0x00000001u)
#define NANDC_GEN_SEQ_CTRL_CMD0_EN_SHIFT                                       (0u)
#define NANDC_GEN_SEQ_CTRL_CMD1_EN                                             (0x00000002u)
#define NANDC_GEN_SEQ_CTRL_CMD1_EN_SHIFT                                       (1u)
#define NANDC_GEN_SEQ_CTRL_CMD2_EN                                             (0x00000004u)
#define NANDC_GEN_SEQ_CTRL_CMD2_EN_SHIFT                                       (2u)
#define NANDC_GEN_SEQ_CTRL_CMD3_EN                                             (0x00000008u)
#define NANDC_GEN_SEQ_CTRL_CMD3_EN_SHIFT                                       (3u)
#define NANDC_GEN_SEQ_CTRL_COL_A0                                              (0x00000030u)
#define NANDC_GEN_SEQ_CTRL_COL_A0_SHIFT                                        (4u)
#define NANDC_GEN_SEQ_CTRL_COL_A1                                              (0x000000C0u)
#define NANDC_GEN_SEQ_CTRL_COL_A1_SHIFT                                        (6u)
#define NANDC_GEN_SEQ_CTRL_ROW_A0                                              (0x00000300u)
#define NANDC_GEN_SEQ_CTRL_ROW_A0_SHIFT                                        (8u)
#define NANDC_GEN_SEQ_CTRL_ROW_A1                                              (0x00000C00u)
#define NANDC_GEN_SEQ_CTRL_ROW_A1_SHIFT                                        (10u)
#define NANDC_GEN_SEQ_CTRL_DATA_EN                                             (0x00001000u)
#define NANDC_GEN_SEQ_CTRL_DATA_EN_SHIFT                                       (12u)
#define NANDC_GEN_SEQ_CTRL_DELAY_EN                                            (0x00006000u)
#define NANDC_GEN_SEQ_CTRL_DELAY_EN_SHIFT                                      (13u)
#define NANDC_GEN_SEQ_CTRL_IMD_SEQ                                             (0x00008000u)
#define NANDC_GEN_SEQ_CTRL_IMD_SEQ_SHIFT                                       (15u)
#define NANDC_GEN_SEQ_CTRL_CMD_3                                               (0x00FF0000u)
#define NANDC_GEN_SEQ_CTRL_CMD_3_SHIFT                                         (16u)
#define NANDC_MLUN_MLUN_IDX                                                    (0x00000007u)
#define NANDC_MLUN_MLUN_IDX_SHIFT                                              (0u)
#define NANDC_MLUN_LUN_SEL                                                     (0x00000300u)
#define NANDC_MLUN_LUN_SEL_SHIFT                                               (8u)
#define NANDC_DEV0_SIZE_DEV_SIZE                                               (0x00000FFFu)
#define NANDC_DEV0_SIZE_DEV_SIZE_SHIFT                                         (0u)
#define NANDC_DMA_TRIG_TLVL_DMA_TRIG_TLVL                                      (0x000000FFu)
#define NANDC_DMA_TRIG_TLVL_DMA_TRIG_TLVL_SHIFT                                (0u)
#define NANDC_CMD_MARK_CMD_ID                                                  (0x000000FFu)
#define NANDC_CMD_MARK_CMD_ID_SHIFT                                            (0u)
#define NANDC_LUN_STATUS0_MEM0_LUN                                             (0x000000FFu)
#define NANDC_LUN_STATUS0_MEM0_LUN_SHIFT                                       (0u)
#define NANDC_TIME_GEN_SEQ_3_t0_d12                                            (0x0000003Fu)
#define NANDC_TIME_GEN_SEQ_3_t0_d12_SHIFT                                      (0u)
#define NANDC_ECC_CNT_ERR_LVL                                                  (0x0000003Fu)
#define NANDC_ECC_CNT_ERR_LVL_SHIFT                                            (0u)

#endif
