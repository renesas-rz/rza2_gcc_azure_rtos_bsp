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
* File Name    : r_dmaca_rz_private.h
* Description  : Specific file for DMACA driver
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.06.2014 1.00    Initial revision
*         : 27.08.2014 1.01    Not change program due to updating application note only.
*         : 30.01.2015 1.02    Added RX71M.
*         : 13.04.2015 1.03    Added RX231 and RX230.
*         : 30.09.2016 1.04    Added RX65N.
*******************************************************************************/
#ifndef DMACA_RX_PRIVATE_H
#define DMACA_RX_PRIVATE_H

/*******************************************************************************
Includes   <System Includes>, "Project Includes"
*******************************************************************************/
/* bool support */
#include <stdbool.h>
/* DMACA driver targets header file */
#include "r_dmaca_rz_target.h"

/*****************************************************************************
Macro definitions
******************************************************************************/


/*****************************************************************************
Typedef definitions
******************************************************************************/
/* CHCTRL.b00:SETEN ... CHSTAT.b00:EN bit set  */
#define DMACA_DMA_ENABLE        (1<<0)
/* CHCTRL.b01:CLREN ... CHSTAT.b00:EN bit clear  */
#define DMACA_DMA_DISABLE       (1<<1)
/* CHCTRL.b02:STG ... CHSTAT.b01:RQST bit set */
#define DMACA_SOFT_TRIG         (1 << 2)
#define DMACA_STAT_RQST         (1 << 2)
/* CHCTRL.b04:CLRRQ ... CHSTAT.b01:RQST bit clear */
#define DMACA_STAT_RQST_CLEAR   (1 << 4)

/*******************************************************************************
Exported global variables and functions (to be accessed by other files)
*******************************************************************************/
extern void *p_DMACI_Handlers[];

bool           r_dmaca_channel_valid_check(uint8_t channel);
void           r_dmaca_module_enable(void);
void           r_dmaca_module_disable(void);
dmaca_return_t r_dmaca_int_enable_set(uint8_t channel, uint8_t priority);
dmaca_return_t r_dmaca_int_disable_set(uint8_t channel);
int32_t r_intc_dummy_read_enable(uint16_t int_id);

#endif /* DMACA_RX_PRIVATE_H */

/* End of File */

