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
* Copyright (C) 2016(2018) Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usb_usbif_api.c
* Description  : USB Host and Peripheral Driver API code. HCD(Host Control Driver) PCD (Peripheral Control Driver)
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 30.09.2015 1.00 First Release
*         : 30.09.2017 1.22 Update Argument Checking
*         : 31.03.2018 1.23 Supporting Smart Configurator
***********************************************************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <string.h>

#include "r_usb_basic_if.h"
#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_bitdefine.h"
#include "r_usb_reg_access.h"
#include "r_usb_basic_config.h"

#if defined(USB_CFG_PHID_USE)
#include "r_usb_phid_if.h"
#endif /* defined(USB_CFG_PHID_USE) */

#if defined(USB_CFG_PCDC_USE)
#include "r_usb_pcdc_if.h"
#endif /* defined(USB_CFG_PCDC_USE) */

/******************************************************************************
 Exported global variables (to be accessed by other files)
******************************************************************************/
const uint8_t g_usb_pipe_peri[] =
{
    /* OUT pipe */          /* IN pipe */
#if defined(USB_CFG_PCDC_USE)
    USB_CFG_PCDC_BULK_OUT,  USB_CFG_PCDC_BULK_IN,   /* USB_PCDC */
    USB_NULL,               USB_CFG_PCDC_INT_IN,    /* USB_PCDCC */
#else
    USB_NULL,               USB_NULL,
    USB_NULL,               USB_NULL,
#endif

#if defined(USB_CFG_PHID_USE)
    USB_CFG_PHID_INT_OUT,   USB_CFG_PHID_INT_IN,    /* USB_PHID */
#else
    USB_NULL,               USB_NULL,
#endif
};

//uint8_t  is_init __attribute__ ((section ("USB_DATA_RAM_INIT"))) = USB_NO;

uint32_t  g_usb_read_request_size[USB_NUM_USBIP][USB_MAXPIPE_NUM+1] __attribute__ ((section ("USB_DATA_RAM_INIT"))); //((section ("USB_DATA_RAM")));
	usb_ctrl_t      g_usb_cstd_event[USB_EVENT_MAX] __attribute__ ((section ("USB_DATA_RAM")));

usb_utr_t       g_usb_pdata[USB_MAXPIPE_NUM + 1] __attribute__ ((section ("USB_DATA_RAM")));

volatile uint16_t        g_usb_usbmode __attribute__ ((section ("USB_DATA_RAM")));
uint16_t        g_usb_open_class __attribute__ ((section ("USB_DATA_RAM")));

/************* 20181228 ADD sawada *****************************/
//uint16_t        g_usb_usbmode;
uint16_t        g_usb_cstd_driver_open = USB_FALSE;
//uint16_t        g_usb_open_class[USB_NUM_USBIP];      //20180107 COMMOUT sawada
uint16_t        g_usb_change_device_state[USB_NUM_USBIP];
/************* 20181228 ADD sawada *****************************/
uint8_t  is_init __attribute__ ((section ("USB_DATA_RAM_INIT"))) = USB_NO; //20181221 CHANGE sawada
#if USB_CFG_BC == USB_CFG_ENABLE
extern uint16_t        g_usb_bc_detect;
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */

usb_callback_t  *g_usb_apl_callback __attribute__ ((section ("USB_DATA_RAM")));

/******************************************************************************
Renesas Abstracted Peripheral Driver API functions
******************************************************************************/

/*****************************************************************************
* Function Name: R_USB_GetVersion
* Description  : Returns the version of this module. The version number is
*                encoded such that the top two bytes are the major version
*                number and the bottom two bytes are the minor version number.
* Arguments    : none
* Return Value : version number
******************************************************************************/
#pragma inline(R_USB_GetVersion)
uint32_t R_USB_GetVersion(void)
{
    uint32_t version = 0;

    version = (USB_VERSION_MAJOR << 16) | USB_VERSION_MINOR;

    return version;
}
/******************************************************************************
 End of function R_USB_GetVersion
 ******************************************************************************/


/******************************************************************************
Function Name   : R_USB_GetEvent
Description     : Get event.
Arguments       : usb_ctrl_t *p_ctrl: control structure for USB API.
Return value    : event code.
******************************************************************************/
uint16_t R_USB_GetEvent(usb_ctrl_t *p_ctrl)
{
    uint16_t    event = USB_STS_NONE;
    usb_cstd_usb_task();
    return event;
}  /* End of function R_USB_GetEvent() */
/******************************************************************************
 End of function R_USB_GetEvent
 ******************************************************************************/


/******************************************************************************
Function Name   : R_USB_Callback
Description     : Registration of the callback function
Arguments       : usb_callback_t    *p_callback : Pointer to the callback function for the application
Return value    : None
******************************************************************************/
void    R_USB_Callback(usb_callback_t *p_callback)
{
    g_usb_apl_callback = p_callback;
} /* End of function R_USB_Callback() */

/******************************************************************************
Function Name   : R_USB_Open
Description     : Start of USB Driver.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : usb_cfg_t     *cfg    : Pointer to usb_cfg_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Open( usb_ctrl_t *ctrl, usb_cfg_t *cfg )
{
    usb_err_t   err;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    /* Argument Checking */
    if ((USB_IP0 != ctrl->module) && (USB_IP1 != ctrl->module))
    {
        return USB_ERR_PARA;
    }

    if ( ( USB_HS != cfg->usb_speed ) && ( USB_FS != cfg->usb_speed ) )
    {
        return USB_ERR_PARA;
    }
#endif /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    if (USB_NO == is_init)
    {
        g_usb_usbmode = 0;
        g_usb_open_class = 0;
        memset((void *)&g_usb_cstd_event, 0, sizeof(g_usb_cstd_event));
        is_init = USB_YES;
    }
    g_usb_cur_task_hdl[ctrl->module]    = (usb_hdl_t)USB_NULL;
    g_usb_change_device_state[ctrl->module] = 0; //20190110  ADD sawada
    memset((void *)&g_usb_read_request_size[ctrl->module], 0, ((USB_MAXPIPE_NUM+1)*4));
    memset((void *)&g_usb_pdata, 0, ((USB_MAXPIPE_NUM+1) * sizeof(usb_utr_t)));
    memset((void *)&g_usb_pstd_usb_int, 0, sizeof(usb_int_t));

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    switch (ctrl->type)
    {
        case USB_PCDC:
        case USB_PCDCC:
        case USB_PHID:
        case USB_PVND:
        case USB_PMSC:
            if (USB_PERI != cfg->usb_mode)
            {
                return USB_ERR_PARA;
            }
            g_usb_usbmode = USB_PERI;
        break;
        default:
            return USB_ERR_PARA;
        break;
    }
#else /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */ //20181228 ADD sawada

    switch (ctrl->type)
    {
        case USB_PCDC:
        case USB_PCDCC:
        case USB_PHID:
        case USB_PVND:
        case USB_PMSC:
            g_usb_usbmode = USB_PERI;
        break;
        default:
        break;
    }
#endif /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */ //20181228 ADD sawada

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
        if ( (USB_LS == cfg->usb_speed) || (USB_NULL == cfg->p_usb_reg) )
        {
            return USB_ERR_PARA;
        }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

        /* USB module start setting */
        err = usb_module_start(USB_CFG_USE_USBIP);
        if (USB_SUCCESS == err)
        {
            /* USB driver initialization */
            usb_pdriver_init( ctrl, cfg );

            /* Setting MCU(USB interrupt init) register */
            usb_cpu_usbint_init(USB_CFG_USE_USBIP);

            /* Setting USB relation register  */
            hw_usb_pmodule_init();

            if (USB_HS == cfg->usb_speed)
            {
                hw_usb_set_hse(USB_NULL);
            }
            hw_usb_pclear_drpd();
            if ( USB_ATTACH == usb_pstd_chk_vbsts() )
            {
                hw_usb_set_cnen();
                usb_cpu_delay_xms((uint16_t)10);
                hw_usb_pset_dprpu();
            }
        }

    if (USB_SUCCESS == err)
    {
        g_usb_open_class |= (1 << ctrl->type);      /* Set USB Open device class */
        if (USB_PCDC == ctrl->type)
        {
            g_usb_open_class |= (1 << USB_PCDCC);   /* Set USB Open device class */
        }
    }

    return err;
}
/******************************************************************************
 End of function R_USB_Open
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Close
Description     : End of USB Driver.
Arguments       : usb_ctrl_t *p_ctrl    : Pointer to usb_ctrl_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Close(usb_ctrl_t *p_ctrl)
{
    usb_err_t   ret_code;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if ((USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module))
    {
        return USB_ERR_PARA;
    }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    ret_code = usb_module_stop(p_ctrl->module);
    if (USB_SUCCESS == ret_code)
    {
        usb_pstd_driver_release();   /* Clear the information registered in the structure usb_pcdreg_t. */
        usb_pstd_clr_pipe_table();
    }
    else
    {
        ret_code = USB_ERR_NOT_OPEN;
    }

    if (USB_SUCCESS == ret_code)
    {
        g_usb_open_class &= (~(1 << p_ctrl->type));     /* Clear USB Open device class */
        if (USB_PCDC == p_ctrl->type)
        {
            g_usb_open_class &= (~(1 << USB_PCDCC));    /* Clear USB Open device class */
        }
    }

    return ret_code;
}
/******************************************************************************
 End of function R_USB_Close
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Read
Description     : USB Data Receive process.
Arguments       : usb_ctrl_t    *p_ctrl     : Pointer to usb_ctrl_t structure
                : uint8_t       *buf        : Pointer to the data buffer areatransfer data address
                : uint32_t      size        : Read size
Return value    : usb_err_t error code      : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Read(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_info_t  info;
    usb_er_t    err;
/************ 20190128 ADD S sawada *********************/
    usb_err_t   result = USB_ERR_NG;
/************ 20190128 ADD E sawada *********************/

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (((USB_NULL == p_ctrl) || (USB_NULL == buf)) || (USB_NULL == size))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if ((USB_PCDCC == p_ctrl->type) || (USB_PVND == p_ctrl->type) || (USB_PMSC == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }

    if (USB_REQUEST != p_ctrl->type)
    {
        if (USB_NULL == (g_usb_open_class & (1 << p_ctrl->type)))      /* Check USB Open device class */
        {
            return USB_ERR_PARA;
        }
    }
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */



    R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED == info.status)
    {
        if (USB_REQUEST == p_ctrl->type)
        {
            err = usb_ctrl_read( p_ctrl, buf, size );
        }
        else
        {
            err = usb_data_read( p_ctrl, buf, size );
        }

        if (USB_OK == err)
        {
            result = USB_SUCCESS;
        }
        else if (USB_QOVR == err)
        {
            result = USB_ERR_BUSY;
        }
        else
        {
            /* Noting */
        }
    }

    return result;
/************ 20190128 CHANGE E sawada *********************/
    
}
/******************************************************************************
 End of function R_USB_Read
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Write
Description     : USB Data send process.
Arguments       : usb_ctrl_t    *p_ctrl     : Pointer to usb_ctrl_t structure
                : uint8_t       *buf        : Pointer to the data buffer areatransfer data address
                : uint32_t      size        : Write size
Return value    : usb_err_t error code      : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t R_USB_Write(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_info_t  info;
    usb_er_t    err;
/************ 20190128 ADD S sawada *********************/
    usb_err_t   result = USB_ERR_NG;
/************ 20190128 ADD E sawada *********************/

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if ((USB_NULL == p_ctrl) || ((USB_NULL == buf) && (0 != size)))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if ( (USB_PVND == p_ctrl->type) || (USB_PMSC == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }

    if (USB_REQUEST != p_ctrl->type)
    {
        if (USB_NULL == (g_usb_open_class & (1 << p_ctrl->type)))      /* Check USB Open device class */
        {
            return USB_ERR_PARA;
        }
    }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */



    R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED == info.status)
    {
        if (USB_REQUEST == p_ctrl->type)
        {
        //	printf("DATA_READ\r\n");
            err = usb_ctrl_write( p_ctrl, buf, size );
        }
        else
        {
        //	printf("DATA_WRITE\r\n");
            err = usb_data_write( p_ctrl, buf, size );
        }

        if (USB_OK == err)
        {
            result = USB_SUCCESS;
        }
        else if (USB_QOVR == err)
        {
            result = USB_ERR_BUSY;
        }
        else
        {
            /* error */
        }
    }

    return result;
/************ 20190128 CHANGE S sawada *********************/
}
/******************************************************************************
 End of function R_USB_Write
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Stop
Description     : USB Transfer stop
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : uint16_t      type    : Read(0)/Write(1)
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Stop(usb_ctrl_t *p_ctrl, uint16_t type)
{
    usb_info_t  info;
    usb_er_t    err;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }
    if ((USB_WRITE != type) && (USB_READ != type))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if ((USB_PVND == p_ctrl->type) || (USB_PMSC == p_ctrl->type))
    {
        return USB_ERR_PARA;
    }
    
    if ((USB_PCDCC == p_ctrl->type ) && (USB_READ == type))
    {
        return USB_ERR_PARA;
    }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    err = R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED != info.status)
    {
        return USB_ERR_NG;
    }

    if (USB_REQUEST == p_ctrl->type)
    {
        err = usb_ctrl_stop( p_ctrl);
    }
    else
    {
        err = usb_data_stop( p_ctrl, type);
    }

    if (USB_OK == err)
    {
        return USB_SUCCESS;
    }
    else if (USB_QOVR == err)
    {
        return USB_ERR_BUSY;
    }
    else
    {
        /* error */
    }

    return USB_ERR_NG;
}
/******************************************************************************
 End of function R_USB_Stop
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_Suspend
Description     : USB Suspend process for USB Host.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Suspend(usb_ctrl_t *p_ctrl)
{
    usb_err_t   ret_code = USB_SUCCESS;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (USB_PERI == g_usb_usbmode)
    {
        return USB_ERR_NG;              /* Support Host only. */
    }

    /* Argument Checking */
    if ((USB_IP0 != p_ctrl->module) && (USB_IP1 != p_ctrl->module))
    {
        return USB_ERR_PARA;
    }
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */
    return ret_code;
}

/******************************************************************************
 End of function R_USB_Suspend
 ******************************************************************************/


/******************************************************************************
Function Name   : R_USB_Resume
Description     : USB Resume process(Host) and Remote wakeup process(Peri).
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_Resume(usb_ctrl_t *p_ctrl)
{
    usb_err_t   ret_code = USB_SUCCESS;
    usb_info_t  info;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (USB_NULL == p_ctrl)
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
    if (USB_PHID != p_ctrl->type)
    {
        return USB_ERR_PARA;
    }
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    ret_code = R_USB_GetInformation(p_ctrl, &info);
    if (USB_SUCCESS != ret_code)
    {
        return USB_ERR_NG;
    }

    if (USB_STS_SUSPEND != info.status)
    {
        return USB_ERR_NOT_SUSPEND;
    }

#if defined(USB_CFG_PHID_USE)
    usb_pstd_change_device_state(USB_DO_REMOTEWAKEUP, USB_NULL, (usb_cb_t)usb_pstd_dummy_function);

#else   /* defined(USB_CFG_PHID_USE) */
    ret_code = USB_ERR_PARA;
#endif  /* defined(USB_CFG_PHID_USE) */
    return ret_code;
}
/******************************************************************************
 End of function R_USB_Resume
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_VbusSetting
Description     : Contol of USB VBUS.(USB Host only)
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : uint16_t      state   : HI/LOW assertion
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t   R_USB_VbusSetting( usb_ctrl_t *p_ctrl, uint16_t state )
{
    return USB_ERR_NG;
}
/******************************************************************************
 End of function R_USB_VbusSetting
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_GetInformation
Description     : Get USB Informatio.(Device class,Connect speed,Device status)
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : usb_info_t    *p_info : Pointer to usb_info_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t   R_USB_GetInformation( usb_ctrl_t *p_ctrl, usb_info_t *p_info)
{
    usb_err_t   ret_code = USB_SUCCESS;
    uint16_t    status;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    /* Argument Checking */
    if (USB_NULL == p_info)
    {
        return USB_ERR_PARA;
    }
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

        p_info->type   = USB_NULL;              /* Device class */
        switch(usb_cstd_port_speed(USB_NULL))
        {
            case USB_NOCONNECT:
                p_info->speed  = USB_NULL;
            break;
            case USB_HSCONNECT:
                p_info->speed  = USB_HS;
            break;
            case USB_FSCONNECT:
                p_info->speed  = USB_FS;
            break;
            case USB_LSCONNECT:
                p_info->speed  = USB_LS;
            break;
            default:
                p_info->speed  = USB_NULL;
            break;
        }
        status = hw_usb_read_intsts();

        switch ((uint16_t)(status & USB_DVSQ))
        {
            case USB_DS_POWR:                           /* Power state  */
                p_info->status = USB_STS_DETACH;
            break;
            case USB_DS_DFLT:                           /* Default state  */
                p_info->status = USB_STS_DEFAULT;
            break;
            case USB_DS_ADDS:                           /* Address state  */
                p_info->status = USB_STS_ADDRESS;
            break;
            case USB_DS_CNFG:                           /* Configured state  */
                p_info->status = USB_STS_CONFIGURED;
            break;
            case USB_DS_SPD_POWR:                       /* Power suspend state */
            case USB_DS_SPD_DFLT:                       /* Default suspend state */
            case USB_DS_SPD_ADDR:                       /* Address suspend state */
            case USB_DS_SPD_CNFG:                       /* Configured Suspend state */
                p_info->status = USB_STS_SUSPEND;
            break;
            default:                                    /* Error */
            break;                                  /* p_info->status = USB_STS_ERROR; */
        }
#if USB_CFG_BC == USB_CFG_ENABLE
        p_info->port   = g_usb_bc_detect;           /* USB_SDP/USB_CDP/USB_DCP */
#else   /* USB_CFG_BC == USB_CFG_ENABLE */
        p_info->port   = USB_SDP;                   /* USB_SDP */
#endif  /* USB_CFG_BC == USB_CFG_ENABLE */
        ret_code = USB_SUCCESS;

    return ret_code;
}
/******************************************************************************
 End of function R_USB_GetInformation
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_PipeRead
Description     : USB Data receive process for Vendor class.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : uint8_t       *buf    : Pointer to the data buffer area
                : uint32_t      size    : Read size
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t  R_USB_PipeRead(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
    usb_err_t   ret_code = USB_ERR_NG;
    
    usb_er_t    err;
#if !defined(USB_CFG_PVND_USE)
    return USB_ERR_NG;
#else /* !defined(USB_CFG_PVND_USE) */


#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if (((USB_NULL == p_ctrl) || (USB_PIPE0 == p_ctrl->pipe)) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

    if ((USB_NULL == buf) || (USB_NULL == size))
    {
        return USB_ERR_PARA;
    }

    /* Argument Checking */
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */
#if !defined(USB_CFG_PVND_USE)
    return USB_ERR_NG;
#else /* #if !defined(USB_CFG_PVND_USE) */
        if (USB_NULL != g_p_usb_pstd_pipe[p_ctrl->pipe])
        {
            ret_code = USB_ERR_BUSY;
        }
        g_usb_pdata[p_ctrl->pipe].keyword    = p_ctrl->pipe;        /* Pipe No */
        g_usb_pdata[p_ctrl->pipe].p_tranadr    = buf;                 /* Data address */
        g_usb_pdata[p_ctrl->pipe].tranlen    = size;                /* Data Size */
#if defined(USB_CFG_PVND_USE)
        g_usb_pdata[p_ctrl->pipe].complete   = (usb_cb_t)&usb_pvnd_read_complete;   /* Callback function */
#endif  /* defined(USB_CFG_PCDC_USE) */

        g_usb_read_request_size[USB_CFG_USE_USBIP][p_ctrl->pipe] = size;
        err = usb_pstd_transfer_start(&g_usb_pdata[p_ctrl->pipe]);   /* USB Transfer Start */

#endif  /* !defined(USB_CFG_PVND_USE) */

    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }
    else if (USB_QOVR == err)
    {
        ret_code = USB_ERR_BUSY;
    }
    else
    {
        ret_code = USB_ERR_NG;
    }
    return ret_code;
#endif /* !defined(USB_CFG_PVND_USE) */
}
/******************************************************************************
 End of function R_USB_PipeRead
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_PipeWrite
Description     : USB Data send process for Vendor class.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : uint8_t       *buf    : Pointer to the data buffer area
                : uint32_t      size    : Write size
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t  R_USB_PipeWrite(usb_ctrl_t *p_ctrl, uint8_t *buf, uint32_t size)
{
#if !defined(USB_CFG_PVND_USE)
    return USB_ERR_NG;
#else /* !defined(USB_CFG_PVND_USE) */

    usb_er_t    err;
/************ 20190128 CHANGE S sawada *********************/
//    usb_err_t   ret_code;
    usb_err_t   ret_code = USB_ERR_NG;
/************ 20190128 CHANGE E sawada *********************/
    usb_info_t  info;
    
#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
        /* Argument Checking */
    if (((USB_NULL == p_ctrl) || (USB_NULL == buf) || (USB_PIPE0 == p_ctrl->pipe)) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

    if (USB_PERI == g_usb_usbmode)
    {
        if ((USB_NULL == buf) && (USB_NULL != size))
        {
            return USB_ERR_PARA;
        }
    }

    if (USB_NULL == (g_usb_open_class & (1 << p_ctrl->type)))      /* Check USB Open device class */
    {
        return USB_ERR_PARA;
    }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */



    R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED == info.status)
    {
        /* PIPE Transfer set */

#if defined(USB_CFG_PVND_USE)
            if (USB_NULL != g_p_usb_pstd_pipe[p_ctrl->pipe])
            {
                ret_code = USB_ERR_BUSY;
            }
//            else
//            {
                g_usb_pdata[p_ctrl->pipe].keyword    = p_ctrl->pipe;        /* Pipe No */
                g_usb_pdata[p_ctrl->pipe].p_tranadr  = buf;                 /* Data address */
                g_usb_pdata[p_ctrl->pipe].tranlen    = size;                /* Data Size */
                g_usb_pdata[p_ctrl->pipe].complete   = (usb_cb_t)&usb_pvnd_write_complete;  /* Callback function */

                err = usb_pstd_transfer_start(&g_usb_pdata[p_ctrl->pipe]);   /* USB Transfer Start */
                if (USB_OK == err)
                {
                    ret_code = USB_SUCCESS;
                }
                else if (USB_QOVR == err)
                {
                    ret_code = USB_ERR_BUSY;
                }
                else
                {
                	ret_code = USB_ERR_NG;
                }
//            }
    }
        #endif  /* defined(USB_CFG_PVND_USE) */

/************ 20190128 CHANGE E sawada *********************/

    return ret_code;
#endif /* #if !defined(USB_CFG_PVND_USE) */

}
/******************************************************************************
 End of function R_USB_PipeWrite
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_PipeStop
Description     : USB transfer stop process for Vendor class.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_PipeStop(usb_ctrl_t *p_ctrl)
{
#if !defined(USB_CFG_PVND_USE)
    return USB_ERR_NG;
#else /* !defined(USB_CFG_PVND_USE) */
    
    usb_er_t    err;
    usb_err_t   ret_code = USB_ERR_NG;
    usb_info_t  info;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    /* Argument Checking */
    if ((USB_PIPE0 == p_ctrl->pipe) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }

    if (USB_NULL == (g_usb_open_class & (1 << p_ctrl->type)))      /* Check USB Open device class */
    {
        return USB_ERR_PARA;
    }

#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    err = R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED != info.status)
    {
        return USB_ERR_NG;
    }

    err = usb_pstd_transfer_end(p_ctrl->pipe);
    if (USB_OK == err)
    {
        ret_code = USB_SUCCESS;
    }

    return ret_code;
#endif /* !defined(USB_CFG_PVND_USE) */
}
/******************************************************************************
 End of function R_USB_PipeStop
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_GetUsePipe
Description     : Get Information for Use pipe.
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : uint16_t      *p_pipe : Pointer to the area to store the used pipe bitmap
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_GetUsePipe(usb_ctrl_t *p_ctrl, uint16_t *p_pipe)
{
    usb_info_t  info;
    uint16_t    pipe_no;

    R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED != info.status)
    {
        return USB_ERR_NG;
    }

    /* Get PIPE Number from Endpoint address */
    *p_pipe = ((uint16_t)1 << USB_PIPE0);
    for (pipe_no = USB_MIN_PIPE_NO; pipe_no < (USB_MAX_PIPE_NO +1); pipe_no++)
    {
        if (USB_TRUE == g_usb_pipe_table[USB_CFG_USE_USBIP][pipe_no].use_flag)
        {
            (*p_pipe) |= ((uint16_t)1 << pipe_no);
        }
    }

    return USB_SUCCESS;
}
/******************************************************************************
 End of function R_USB_GetUsePipe
 ******************************************************************************/

/******************************************************************************
Function Name   : R_USB_GetPipeInfo
Description     : Get Infomation for pipe setting.(DIR,Transfer Type,MAXPS)
Arguments       : usb_ctrl_t    *p_ctrl : Pointer to usb_ctrl_t structure
                : usb_pipe_t    *p_info : Pointer to the area to store the pipe information
Return value    : usb_err_t error code  : USB_SUCCESS,USB_ERR_NG etc.
******************************************************************************/
usb_err_t    R_USB_GetPipeInfo(usb_ctrl_t *p_ctrl, usb_pipe_t *p_info)
{
    usb_info_t  info;
    uint16_t    pipe_type;

#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    /* Argument Checking */
    if (((USB_NULL == p_ctrl) || (0 == p_ctrl->pipe)) || (USB_MAXPIPE_NUM < p_ctrl->pipe))
    {
        return USB_ERR_PARA;
    }
#endif  /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

    R_USB_GetInformation(p_ctrl, &info);
    if (USB_STS_CONFIGURED != info.status)
    {
        return USB_ERR_NG;
    }

    /* Get PIPE Number from Endpoint address */
    p_info->ep   = (uint8_t)(g_usb_pipe_table[USB_CFG_USE_USBIP][p_ctrl->pipe].pipe_cfg & USB_EPNUMFIELD);  /* Set EP num. */
    if (USB_DIR_P_IN == (g_usb_pipe_table[USB_CFG_USE_USBIP][p_ctrl->pipe].pipe_cfg & USB_DIRFIELD))        /* Check dir */
    {
        p_info->ep |= USB_EP_DIR_IN;                            /* Set DIR IN */
    }
    pipe_type = usb_cstd_get_pipe_type(USB_NULL, p_ctrl->pipe);
    switch (pipe_type)
    {
        case USB_TYPFIELD_ISO:
            p_info->type = USB_ISO;                                 /* Set Isochronous */
        break;
            
        case USB_TYPFIELD_BULK:
            p_info->type = USB_BULK;                                /* Set Bulk */
        break;
            
        case USB_TYPFIELD_INT:
            p_info->type = USB_INT;                                 /* Set Interrupt */
        break;
            
        default:
            return USB_ERR_NG;
        break;
    }
    p_info->mxps = usb_cstd_get_maxpacket_size(USB_NULL, p_ctrl->pipe); /* Set Max packet size */
    return USB_SUCCESS;
}
/******************************************************************************
 End of function R_USB_GetPipeInfo
 ******************************************************************************/

usb_err_t R_USB_PDriverInit(usb_ctrl_t *ctrl, usb_cfg_t *cfg)
{
#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    /* Argument Checking */
    if ((USB_IP0 != ctrl->module) && (USB_IP1 != ctrl->module))
    {
        return USB_ERR_PARA;
    }

    if ( ( USB_HS != cfg->usb_speed ) && ( USB_FS != cfg->usb_speed ) )
    {
        return USB_ERR_PARA;
    }

    switch (ctrl->type)
    {
        case USB_PCDC:
        case USB_PCDCC:
        case USB_PHID:
        case USB_PVND:
        case USB_PMSC:
            if (USB_PERI != cfg->usb_mode)
            {
                return USB_ERR_PARA;
            }
            g_usb_usbmode = USB_PERI;
        break;
        default:
            return USB_ERR_PARA;
        break;
    }

    if ( (USB_LS == cfg->usb_speed) || (USB_NULL == cfg->p_usb_reg) )
    {
        return USB_ERR_PARA;
    }
#endif /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

	usb_pdriver_init (ctrl, cfg);
    return USB_SUCCESS;
}

usb_err_t  R_USB_CPUIntInit()
{
#if USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE
    if( USB_CFG_IP0 != USB_CFG_USE_USBIP && USB_CFG_IP1 != USB_CFG_USE_USBIP )
    {
        return USB_ERR_PARA;
    }
#endif /* USB_CFG_PARAM_CHECKING == USB_CFG_ENABLE */

	usb_cpu_usbint_init (USB_CFG_USE_USBIP);
    return USB_SUCCESS;
}

/******************************************************************************
End  Of File
******************************************************************************/
