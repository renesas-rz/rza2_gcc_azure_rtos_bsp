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
* Copyright (C) 2014(2015) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

/*******************************************************************************
* File Name    : r_dmaca_rx.c
* Device(s)    : RX
* Tool-Chain   : Renesas RXC Toolchain v2.05.00
* OS           : not use
* H/W Platform : not use
* Description  : Functions for DMACA driver
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.06.2014 1.00    Initial release
*         : 27.08.2014 1.01    Not change program due to updating application note only.
*         : 30.01.2015 1.02    Added RX71M.
*         : 13.04.2015 1.03    Added RX231 and RX230.
*         : 30.09.2016 1.04    Added transfer_count and block_size to "case DMACA_CMD_STATUS_GET"
*         :                    of r_dmaca_control().
*******************************************************************************/

/*******************************************************************************
Includes   <System Includes>, "Project Includes"
*******************************************************************************/
/* Definitions for DMACA support */
#include "r_dmaca_rz_if.h"
#include "r_dmaca_rz_private.h"
#include "r_usb_bitdefine.h"

/*******************************************************************************
Macro definitions
*******************************************************************************/

/*******************************************************************************
Typedef definitions
*******************************************************************************/


/*******************************************************************************
Exported global variables (to be accessed by other files)
*******************************************************************************/
/* This array holds callback functions. */
void *p_DMACI_Handlers[DMACA_NUM_CHANNELS] __attribute__ ((section ("USB_DATA_RAM")));

/*******************************************************************************
Private variables and functions
*******************************************************************************/
static bool r_dmaca_set_transfer_data(uint8_t channel, dmaca_transfer_data_cfg_t *p_cfg);

/*******************************************************************************
* Function Name: r_dmaca_create
* Description  : Sets registers of the specified channel and the interrupt source.
* Arguments    : channel -
*                    Which channel to use
*                *p_data_cfg -
*                    Pointer to information of transfer data
* Return Value : DMACA_SUCCESS -
*                    Successful operation
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*                DMACA_ERR_INVALID_ARG -
*                    Parameters are invalid.
*                DMACA_ERR_NULL_PTR -
*                    Argument pointers are NULL.
*******************************************************************************/
dmaca_return_t r_dmaca_create(uint8_t channel, dmaca_transfer_data_cfg_t *p_data_cfg)
{
#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_dmaca_channel_valid_check(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }

    if ((uint32_t)NULL == (uint32_t)p_data_cfg)
    {
        return DMACA_ERR_NULL_PTR;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    if (false == r_dmaca_set_transfer_data(channel, p_data_cfg))
    { /* Fail to apply configurations for Transfer data. */
        return DMACA_ERR_INVALID_ARG;
    }

    return DMACA_SUCCESS;
}

/*******************************************************************************
* Function Name: r_dmaca_control
* Description  : Starts/Stops DMACA and checks an interrupt source as DMACA activation.
* Arguments    : channel -
*                    Which channel to use
*                command -
*                Control command
*                *p_stat -
*                    Pointer information of DMACA driver status
*                    When a command is DMACA_CMD_STATUS_GET, casts to void *.
* Return Value : DMACA_SUCCESS -
*                    Successful operation
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*                DMACA_ERR_INVALID_COMMAND -
*                    Command is invalid.
*                DMACA_ERR_NULL_PTR -
*                    Argument pointers are NULL.
*                DMACA_ERR_SOFTWARE_REQUESTED -
*                    DMA transfer request by software is generated (DMREQ.SWREQ=1b) already.
*                    So that cannot execute command. 
*                DMACA_ERR_SOFTWARE_REQUEST_DISABLED -
*                    Transfer Request Source is not Software (DMTMD.DCTG[1:0]=01b).
*                    So that cannot execute command. 
*******************************************************************************/
dmaca_return_t r_dmaca_control(uint8_t channel, dmaca_command_t command, dmaca_stat_t *p_stat)
{
    uint32_t dmaca_channel_status = 0x0000;

#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_dmaca_channel_valid_check(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    switch (command)
    {
        case DMACA_CMD_ENABLE: /* Enable DMA transfer of the specified channel. */
        case DMACA_CMD_RESUME: /* Resume DMA transfer of the specified channel. */
            DMACA_CHCTRL(channel) = DMACA_DMA_ENABLE;
            if (DMACA_DMA_ENABLE != (DMACA_CHSTAT(channel)&DMACA_DMA_ENABLE))
            {
                /* do nothing */
            }
        break;

        case DMACA_CMD_DISABLE: /* Disable DMA transfer of the specified channel. */
            DMACA_CHCTRL(channel) = DMACA_DMA_DISABLE;
            if (DMACA_DMA_ENABLE == (DMACA_CHSTAT(channel)&DMACA_DMA_ENABLE))
            {
                /* do nothing */
            }
        break;

        case DMACA_CMD_SOFT_REQ: /* DMA transfer by Software using STG */
            DMACA_CHCTRL(channel) = DMACA_SOFT_TRIG;

            if (DMACA_STAT_RQST != (DMACA_CHSTAT(channel) & DMACA_STAT_RQST))
            {
                /* do nothing */
            }
        break;
        
        case DMACA_CMD_SOFT_REQ_CLR: /* Clear DMREQ.SWREQ bit. */
            DMACA_CHCTRL(channel) = DMACA_CHSTAT_RQST_CLEAR;

            if (DMACA_STAT_RQST == (DMACA_CHSTAT(channel) & DMACA_STAT_RQST))
            {
                /* do nothing */
            }
        break;

        case DMACA_CMD_STATUS_GET:

#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)
            if ((uint32_t)NULL == (uint32_t)p_stat)
            {
                return DMACA_ERR_NULL_PTR;
            }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

            dmaca_channel_status = DMACA_CHSTAT(channel);

            if (0 == (dmaca_channel_status & DMACA_CHSTAT_EN_MASK)) /* Get the current status of DMACA. */
            {   /* DMACA operation is suspended. */
                p_stat->act_stat = false;
            }
            else
            {  /* DMACA is operating. */
                p_stat->act_stat = true;
            }

            if (0 == (dmaca_channel_status & DMACA_CHSTAT_END_MASK)) /* Get the transfer end interrupt flag. */
            {  /* A transfer end interrupt has not been generated. */
                p_stat->end_stat = false;
            }
            else
            {  /* A transfer end interrupt has been generated. */
                p_stat->end_stat = true;
            }

            if (0 == (dmaca_channel_status & DMACA_CHSTAT_ER_MASK)) /* Get the error end interrupt flag. */
            {  /* A transfer escape end interrupt has not been generated. */
                p_stat->er_stat = false;
            }
            else
            {  /* A transfer escape end interrupt has been generated. */
                p_stat->er_stat = true;
            }
            p_stat->transfer_count = DMACA_N0TB(channel);

            break;


        case DMACA_CMD_CH_STATUS_CLR:
            DMACA_CHCTRL(channel) = DMACA_CHSTAT_CLEAR | DMACA_CHSTAT_RQST_CLEAR | DMACA_CHSTAT_END_CLEAR | DMACA_CHSTAT_TC_CLEAR | DMACA_CHSTAT_USBFDMA_CLEAR;
            if (0x00 == (DMACA_CHSTAT(channel)&(DMACA_CHSTAT_END_CLEAR | DMACA_CHSTAT_TC_CLEAR | DMACA_CHSTAT_USBFDMA_CLEAR)))
            {
                /* do nothing */
            }
        break;

        case DMACA_CMD_REG_SET_FIX:
            DMACA_CHCTRL(channel) &= ~DMACA_CHCTRL_CLEAR;
            if (0x00 == (DMACA_CHCTRL(channel) & DMACA_CHCTRL_CLEAR))
            {
                /* do nothing */
            }
            DMACA_CHCFG(channel) &= ~(DMACA_CONFIG_WONLY_MASK | DMACA_CONFIG_SBE_MASK | DMACA_CONFIG_RSEL_MASK | DMACA_CONFIG_RSW_MASK | DMACA_CONFIG_REN_MASK | DMACA_CONFIG_DMS_MASK);
            if (0x00 == (DMACA_CHCFG(channel) & (DMACA_CONFIG_WONLY_MASK | DMACA_CONFIG_SBE_MASK | DMACA_CONFIG_RSEL_MASK | DMACA_CONFIG_RSW_MASK | DMACA_CONFIG_REN_MASK | DMACA_CONFIG_DMS_MASK)))
            {
                /* do nothing */
            }
            DMACA_DCTRL |= DMACA_INT_DETECT_LEVEL;
        break;

        default:
            return DMACA_ERR_INVALID_COMMAND;
        break;
    }

    return DMACA_SUCCESS;
}

/*******************************************************************************
* Function Name: r_dmaca_init
* Description  : Initializes a callback function array for DMACmI interrupt.
*                                                          (m = 0 to 3, or 74)
* Arguments    : None
* Return Value : None
*******************************************************************************/
void r_dmaca_init(void)
{
    uint32_t i;

    for (i = 0; i < DMACA_NUM_CHANNELS; i++)
    {
        p_DMACI_Handlers[i] = NULL;
    }
}

/********************************************************************************
* Function Name: r_dmaca_int_callback
* Description  : Registers a callback function for DMACmI interrupts. 
*                                                     (m = 0 to 3, or 74)
*                If 'FIT_NO_FUNC' or 'NULL' is specified for argument *p_callback then
                 any previously registered callback is unregistered.
* Arguments    : channel -
*                    Which channel to use
*                *p_callback -
*                    Pointer to function to call when DMACmI interrupt occurs.
* Return Value : DMACA_SUCCESS -
*                    Callback function is registered.
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*                DMACA_ERR_INVALID_HANDLER_ADDR -
*                    Invalid function address is set and any previous function
*                    has been unregistered.
*******************************************************************************/
dmaca_return_t r_dmaca_int_callback(uint8_t channel,  void * p_callback)
{
#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_dmaca_channel_valid_check(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }

    /* Check for valid address. */
    if ( (uint32_t)NULL == (uint32_t)p_callback )
    {
        p_DMACI_Handlers[channel] = (void *)NULL;
        return DMACA_ERR_INVALID_HANDLER_ADDR;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    p_DMACI_Handlers[channel] = p_callback;
    return DMACA_SUCCESS;
}

/*******************************************************************************
* Function Name: r_dmaca_int_enable
* Description  : Enables DMACmI interrupt. (m = 0 to 3, or 74)
* Arguments    : channel -
*                    Which channel to use
*              : priority -
*                    DMACmI interrupt priority level
* Return Value : DMACA_SUCCESS -
*                    Successful operation
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*******************************************************************************/
dmaca_return_t r_dmaca_int_enable(uint8_t channel, uint8_t priority)
{
#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_dmaca_channel_valid_check(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Set the DMACmI priority level. */
    /* Set the DMACmI interrupt Enable bit. */
    if (DMACA_SUCCESS != r_dmaca_int_enable_set(channel, priority))
    {
        return DMACA_ERR_INVALID_CH;
    }

    return DMACA_SUCCESS;
}

/*******************************************************************************
* Function Name: r_dmaca_int_disable
* Description  : Disables DMACmI interrupt. (m = 0 to 3, or 74)
* Arguments    : channel -
*                    Which channel to use
* Return Value : DMACA_SUCCESS -
*                    Successful operation
*                DMACA_ERR_INVALID_CH -
*                    Channel is invalid.
*******************************************************************************/
dmaca_return_t r_dmaca_int_disable(uint8_t channel)
{
#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)

    /* Check argument. */
    if (false == r_dmaca_channel_valid_check(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Clear the DMACmI interrupt Enable bit. */
    /* Clear the DMACmI priority level. */
    if (DMACA_SUCCESS != r_dmaca_int_disable_set(channel))
    {
        return DMACA_ERR_INVALID_CH;
    }

    return DMACA_SUCCESS;
}

/*******************************************************************************
* Function Name: r_dmaca_getversion
* Description  : Returns the version of this module. The version number is 
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
*******************************************************************************/
uint32_t r_dmaca_getversion(void)
{
    uint32_t version = 0;

    version = (DMACA_VERSION_MAJOR << 16) | DMACA_VERSION_MINOR;

    return version;
}

/*******************************************************************************
* Function Name: r_dmaca_set_transfer_data
* Description  : Applies configurations to Transfer data area.
*                It is an internal function called by r_dmaca_create().
*                and all arguments are validated in r_dmaca_create().
* Arguments    : channel -
*                    Which channel to use
*                *p_data_cfg -
*                    Pointer to information of Transfer data
* Return Value : true -
*                    Apply configurations for Transfer data successfully.
*                false -
*                    Fail to apply configurations for Transfer data.
*******************************************************************************/
static bool r_dmaca_set_transfer_data(uint8_t channel, dmaca_transfer_data_cfg_t *p_cfg)
{
	void * p_src_pa_adr;
	void * p_des_pa_adr;

#if (1 == DMACA_CFG_PARAM_CHECKING_ENABLE)
    if ( DMACA_CONFIG_SRC_SIZE_LWORD < (p_cfg->src_size & DMACA_INVALID_SRC_SIZE_MASK) )
    {
        return false;
    }    

    if ( DMACA_CONFIG_DES_SIZE_LWORD < (p_cfg->des_size & DMACA_INVALID_DES_SIZE_MASK) )
    {
        return false;
    }

    if ( DMACA_INVALID_INTERVAL_MAX < p_cfg->ch_interval )
    {
        return false;
    }
#endif /* DMACA_CFG_PARAM_CHECKING_ENABLE */

    /* Clear CHCTRL_n register. CLRRQ bit  */
    DMACA_CHCTRL(channel) = DMACA_STAT_RQST_CLEAR;

    /* Disable DMA transfers. */
    DMACA_CHCTRL(channel) = DMACA_DMA_DISABLE;

    DMACA_CHCFG(channel) |= 0x00000060;  /* debug *//* b'6-5 : 1 fix */

    /* CHCFG Register. SEL bit. Select D0FIFO0 or D1FIFO */
    DMACA_CHCFG(channel) &= ~DMACA_CHCFG_SEL_MASK;
    DMACA_CHCFG(channel) |= (uint32_t)p_cfg->use_fifo;

    /* CHCFG Register. SAD/DAD bit. */
    DMACA_CHCFG(channel) &= ~(DMACA_CHCFG_SAD_MASK | DMACA_CHCFG_DAD_MASK | DMACA_CHCFG_REQD_MASK);
    DMACA_CHCFG(channel) |= (uint32_t)(p_cfg->src_adr_dir | p_cfg->des_adr_dir | p_cfg->req_dir);

    /* CHCFG Register. SDS/DDS bit. */
    DMACA_CHCFG(channel) &= ~(DMACA_CHCFG_SDS_MASK | DMACA_CHCFG_DDS_MASK);
    DMACA_CHCFG(channel) |= (uint32_t)(p_cfg->src_size | p_cfg->des_size);

    /* Set N0SA_0 register. */
    if( 0 == p_cfg->p_src_addr )
    {
    	p_src_pa_adr = 0;
    }
    else
    {
        R_MMU_VAtoPA((uint32_t)p_cfg->p_src_addr, &p_src_pa_adr);
        //p_src_pa_adr = (uint32_t)p_cfg->p_src_addr & ~0x02000000;
    }
    /*DMACA_N0SA(channel) = (uint32_t)p_cfg->p_src_addr;*/
    DMACA_N0SA(channel) = (uint32_t)p_src_pa_adr;

    /* Set N0DA_0 register. */
    if( 0 == p_cfg->p_des_addr )
    {
    	p_des_pa_adr = 0;
    }
    else
    {
        R_MMU_VAtoPA((uint32_t)p_cfg->p_des_addr, &p_des_pa_adr);
        //p_des_pa_adr = (uint32_t)p_cfg->p_des_addr & ~0x02000000;
    }
    /*DMACA_N0DA(channel) = (uint32_t)p_cfg->p_des_addr;*/
    DMACA_N0DA(channel) = (uint32_t)p_des_pa_adr;

    /* Set N0TB_0 register. */
    DMACA_N0TB(channel) = (uint32_t)p_cfg->transfer_count;

    DMACA_CHCFG(channel) &= ~(DMACA_CHCFG_DEM_MASK | DMACA_CHCFG_TCM_MASK);
    DMACA_CHCFG(channel) |= (uint32_t)(p_cfg->dmaint_msk | p_cfg->dmatc_msk);

    if ( (uint32_t)(p_cfg->dmaint_msk | p_cfg->dmatc_msk) == (DMACA_CHCFG(channel) & (DMACA_CHCFG_DEM_MASK | DMACA_CHCFG_TCM_MASK)) )
    {
        /* do nothing */
    }

    return true;
}

/*******************************************************************************
* Function Name: r_dmaca_byteset_trans_enable
* Description  : received data byte set to transmit data byte and transmit enable.
* Arguments    : buffer : received data byte
* Return Value : -
*******************************************************************************/
void r_dmaca_byteset_trans_enable(uint16_t buffer, uint8_t dma_ch)
{
	DMACA_N0TB(dma_ch) = (uint32_t)(buffer & USB_DTLN);
	DMACA_CHCTRL(dma_ch) |= DMACA_DMA_ENABLE;
}

/* End of File */

