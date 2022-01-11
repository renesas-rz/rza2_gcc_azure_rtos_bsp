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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_usbh0_hHubsys.c
* Description  : USB Host Hub system code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh0_basic_if.h"
#include "r_usbh0_hhci.h"

/* Condition compilation by the difference of USB function */
/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
#define USBH0_MAXHUB                    (uint16_t)5

#define USBH0_HUB_P1                    (uint16_t)0x0001
#define USBH0_HUB_P2                    (uint16_t)0x0002
#define USBH0_HUB_P3                    (uint16_t)0x0003
#define USBH0_HUB_P4                    (uint16_t)0x0004

/* HUB spec */
#define USBH0_FSHUB                     (1u)
#define USBH0_HSHUBS                    (2u)
#define USBH0_HSHUBM                    (3u)

/* Interface number */
#define USBH0_HUB_INTNUMFS              (1u)
#define USBH0_HUB_INTNUMHSS             (1u)
#define USBH0_HUB_INTNUMHSM             (1u)

/* USB HUB task message command */
#define USBH0_MSG_HUB_EVENT             (0x0135u)
#define USBH0_MSG_HUB_ATTACH            (0x0136u)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    uint16_t        up_addr;        /* Up-address  */
    uint16_t        up_port;        /* Up-port num */
    uint16_t        port_num;       /* Port number */
    uint16_t        pipe_id;        /* Pipe number */
    uint16_t        class_seq;       /* usbh0_hhub_enumeration() Sequence */
    uint16_t        init_seq;        /* usbh0_hhub_init_down_port() Sequence */
    uint16_t        init_port;       /* usbh0_hhub_init_down_port() Process Port */
    uint16_t        event_seq;       /* usbh0_hhub_event() Sequence */
    uint16_t        event_port;      /* usbh0_hhub_event() Process Port */
    uint16_t        attach_seq;      /* usbh0_hhub_port_attach() Sequence */
    uint16_t        attach_port;     /* usbh0_hhub_port_attach() Process Port */
    uint16_t        process;        /* R_USBH0_HhubTask() msginfo */
} st_usbh0_hub_info_t;

typedef struct
{
    union
    {
        uint16_t word;
        struct
        {
            uint16_t connection:1;
            uint16_t enable:1;
            uint16_t suspend:1;
            uint16_t over_current:1;
            uint16_t reset:1;
            uint16_t l1:1;
            uint16_t :2;
            uint16_t power:1;
            uint16_t low_speed:1;
            uint16_t high_speed:1;
            uint16_t test:1;
            uint16_t indicator_ctrl:1;
            uint16_t :3;
        } bit;
    } status;
    union
    {
        uint16_t word;
        struct
        {
            uint16_t c_connection:1;
            uint16_t c_enable:1;
            uint16_t c_suspend:1;
            uint16_t c_over_current:1;
            uint16_t c_reset:1;
            uint16_t :11;
        } bit;
    } change;
} st_usbh0_hub_status_t;

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/
void                usbh0_hhub_registration(st_usbh0_hcdreg_t *p_callback);

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static void         r_usbh0_hstd_hub_tsk(void);
static void         usbh0_hhub_open(uint16_t hubaddr);
static void         usbh0_hhub_close(uint16_t hubaddr);
static void         usbh0_hhub_suspend(uint16_t hubaddr);
static void         usbh0_hhub_resume(uint16_t hubaddr);
static void         usbh0_hhub_enumeration(st_usbh0_utr_t *mess);
static void         usbh0_hhub_init_down_port(st_usbh0_utr_t *mess);
static void         usbh0_hhub_port_attach(st_usbh0_utr_t *mess);
static void         usbh0_hhub_event(st_usbh0_utr_t *mess);
static void         usbh0_hhub_check_class(uint16_t **table);
static void         usbh0_hhub_trans_start(uint16_t hubaddr, uint32_t size, uint8_t *table, usbh0_utr_cb_t complete);
static uint16_t     usbh0_hhub_get_hub_descriptor(uint16_t hubaddr, usbh0_utr_cb_t complete);
static uint16_t     usbh0_hhub_get_status(uint16_t hubaddr, uint16_t port, usbh0_utr_cb_t complete);
static uint16_t     usbh0_hhub_port_set_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh0_utr_cb_t complete);
static uint16_t     usbh0_hhub_port_clr_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh0_utr_cb_t complete);
static void         usbh0_hhub_trans_complete(st_usbh0_utr_t *utr);
static void         usbh0_hhub_class_request_complete(st_usbh0_utr_t *utr);
static void         usbh0_hhub_new_connect(void);
static uint16_t     usbh0_hhub_get_cnn_dev_addr(uint16_t hubaddr, uint16_t portnum);
static void         usbh0_hhub_port_detach(uint16_t hubaddr, uint16_t portnum);
static void         usbh0_hhub_selective_detach(uint16_t devaddr);
static void         usbh0_hhub_specified_path(uint16_t result);
static void         usbh0_hhub_specified_path_wait(uint16_t result, uint16_t times);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

#ifdef __ICCARM__
#pragma location="USB_HOST_0"

#endif  /* __ICCARM__ */

#ifdef __GNUC__
static st_usbh0_setup_t         gs_usbh0_hhub_class_request __attribute__ ((section ("USB_HOST_0")));                       /* Control transfer setup */
static uint8_t                  gs_usbh0_hhub_descriptor[USBH0_CONFIGSIZE] __attribute__ ((section ("USB_HOST_0")));        /* HUB descriptor */
static st_usbh0_hub_status_t    gs_usbh0_hhub_status[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));       /* HUB status data */
static uint8_t                  gs_usbh0_hhub_data[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));         /* HUB receive data */
static uint8_t                  *gsp_usbh0_hhub_device_table __attribute__ ((section ("USB_HOST_0")));                      /* Device Descriptor Pointer */
static uint8_t                  *gsp_usbh0_hhub_config_table __attribute__ ((section ("USB_HOST_0")));                      /* Configuration Descriptor Pointer */
static uint8_t                  *gsp_usbh0_hhub_interface_table __attribute__ ((section ("USB_HOST_0")));                   /* Interface Descriptor Pointer */
static st_usbh0_utr_t           gs_usbh0_hhub_control_mess __attribute__ ((section ("USB_HOST_0")));                        /* Control transfer message */
static st_usbh0_utr_t           gs_usbh0_hhub_data_mess[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));    /* Data transfer message */
static uint16_t                 gs_usbh0_hhub_down_port[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));    /* HUB downport status */
static uint16_t                 gs_usbh0_hhub_remote[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));       /* Downport remotewakeup */
static st_usbh0_hub_info_t      gs_usbh0_hhub_info[USBH0_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_0")));         /* HUB Information */
static uint16_t                 gs_usbh0_hhub_number __attribute__ ((section ("USB_HOST_0")));                              /* HUB Connection Number */
static uint16_t                 gs_usbh0_hhub_hub_addr __attribute__ ((section ("USB_HOST_0")));                            /* Processing HUB Address */
#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "USB_HOST_0"

#endif  /* __CC_ARM */

/* Target Peripheral List */
static const uint16_t   gs_usbh0_hhub_tpl[4] =
{
    1,                      /* Number of list */
    0,                      /* Reserved */
    USBH0_NOVENDOR,         /* Vendor ID  : no-check */
    USBH0_NOPRODUCT,        /* Product ID : no-check */
};

/***********************************************************************************************************************
Renesas Abstracted Hub Driver API functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Name   : R_USBH0_HhubTask
Description     : Call HUB task
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void R_USBH0_HhubTask(void)
{
#if (BSP_CFG_RTOS_USED == 1)
	st_usbh0_utr_t  *p_mess;
	while(1)
	{
		USBH0_RTOS_RCV_MSG(USBH0_HUB_MSG, (usbh0_msg_t **)&p_mess);
		r_usbh0_hstd_hub_tsk();
		g_usbh0_msg_check &= ~(1<<USBH0_HUB_TSK);
	}
#else /* (BSP_CFG_RTOS_USED == 1) */
	r_usbh0_hstd_hub_tsk();
#endif /* (BSP_CFG_RTOS_USED == 1) */

} /* End of function R_USBH0_HhubTask() */

/***********************************************************************************************************************
Function Name   : r_usbh0_hstd_hub_tsk
Description     : Call HUB task
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void r_usbh0_hstd_hub_tsk(void)
{
    st_usbh0_utr_t    *p_mess;
    usbh0_er_t        err;

    /* Receive message */
    err = USBH0_RCV_MSG(USBH0_HUB_MBX, (usbh0_msg_t**)&p_mess);
    if (USBH0_OK != err)
    {
        return;
    }

    if (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process != p_mess->msginfo)
    {
//        USBH0_PRINTF0("CE\n");
    }
    else
    {
        switch (p_mess->msginfo)
        {
            case USBH0_MSG_CLS_CHECKREQUEST:
                usbh0_hhub_enumeration(p_mess);     /* USB HUB Class Enumeration */
//              USBH0_PRINTF2("enumeration, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq);
            break;

            case USBH0_MSG_CLS_INIT:
                usbh0_hhub_init_down_port(p_mess);      /* Down port initialize */
//              USBH0_PRINTF2("InitDownPort, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq);
            break;

            case USBH0_MSG_HUB_EVENT:
                usbh0_hhub_event(p_mess);
//              USBH0_PRINTF2("event, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq);
            break;

            case USBH0_MSG_HUB_ATTACH:
                usbh0_hhub_port_attach(p_mess);        /* Hub Port attach */
//              USBH0_PRINTF2("PortAttach, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq);
            break;

            default:
                USBH0_PRINTF0("HUB_TASK [0]\n");
            break;
        }
    }

    err = USBH0_REL_BLK(USBH0_HUB_MPL, p_mess);
    if (USBH0_OK != err)
    {
        USBH0_PRINTF0("### USB HUB Task rel_blk error\n");
    }
} /* End of function r_usbh0_hstd_hub_tsk() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_registration
Description     : HUB driver
Arguments       : st_usbh0_hcdreg_t *callback
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_registration(st_usbh0_hcdreg_t *p_callback)
{
    st_usbh0_hcdreg_t   driver;
    uint8_t             i;

    /* Driver registration */
    if (USBH0_NULL == p_callback)
    {
        /* Target peripheral list */
        driver.p_tpl = (uint16_t*)&gs_usbh0_hhub_tpl[0];
    }
    else
    {
        /* Target peripheral list */
        driver.p_tpl = p_callback->p_tpl;
    }

    driver.ifclass      = (uint16_t)USBH0_IFCLS_HUB;    /* Interface class */
    driver.classcheck   = &usbh0_hhub_check_class;      /* Driver check */
    driver.devconfig    = &usbh0_hhub_open;             /* Device configured */
    driver.devdetach    = &usbh0_hhub_close;            /* Device detach */
    driver.devsuspend   = &usbh0_hhub_suspend;          /* Device suspend */
    driver.devresume    = &usbh0_hhub_resume;           /* Device resume */

    for (i = 0; i < USBH0_MAXHUB; i++)
    {
        R_USBH0_HstdDriverRegistration((st_usbh0_hcdreg_t *)&driver);
    }

    for (i = 0; i < (USBH0_MAXDEVADDR + 1u); i++)
    {
        gs_usbh0_hhub_info[i].up_addr       = 0;
        gs_usbh0_hhub_info[i].up_port       = 0;
        gs_usbh0_hhub_info[i].port_num      = 0;
        gs_usbh0_hhub_info[i].pipe_id       = 0;
        gs_usbh0_hhub_info[i].init_seq      = USBH0_SEQ_0;
        gs_usbh0_hhub_info[i].init_port     = USBH0_HUB_P1;
        gs_usbh0_hhub_info[i].attach_seq    = USBH0_SEQ_0;
        gs_usbh0_hhub_info[i].attach_port   = USBH0_HUB_P1;
        gs_usbh0_hhub_info[i].event_seq     = USBH0_SEQ_0;
        gs_usbh0_hhub_info[i].event_port    = USBH0_HUB_P1;
    }

    gs_usbh0_hhub_number = 0;
} /* End of function usbh0_hhub_registration() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_open
Description     : HUB sys open
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_open(uint16_t hub_addr)
{
    if (USBH0_MAXHUB != gs_usbh0_hhub_number)
    {
        /* Pipe number set */
        gs_usbh0_hhub_info[hub_addr].pipe_id = R_USBH0_HstdGetPipeID(hub_addr, USBH0_EP_INT, USBH0_EP_IN, 0);

        /* HUB downport status */
        gs_usbh0_hhub_down_port[hub_addr] = 0;

        /* Downport remotewakeup */
        gs_usbh0_hhub_remote[hub_addr] = 0;

        gs_usbh0_hhub_info[hub_addr].process = USBH0_MSG_CLS_INIT;

        usbh0_hhub_specified_path(USBH0_CTRL_END);

        gs_usbh0_hhub_hub_addr = hub_addr;

        gs_usbh0_hhub_number++;
    }
} /* End of function usbh0_hhub_open() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_close
Description     : HUB sys close
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_close(uint16_t hub_addr)
{
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            dev_addr;
    uint16_t            md;
    uint16_t            i;

    for (i = 1; i <= gs_usbh0_hhub_info[hub_addr].port_num; i++)
    {
        /* Now downport device search */
        dev_addr = usbh0_hhub_get_cnn_dev_addr(hub_addr, i);
        if (0 != dev_addr)
        {
            /* HUB down port selective disconnect */
            usbh0_hhub_selective_detach(dev_addr);
            for (md = 0; md < g_usbh0_hstd_device_num; md++)
            {
                p_driver = &g_usbh0_hstd_device_drv[md];
                if (dev_addr == p_driver->devaddr)
                {
                    (*p_driver->devdetach)(p_driver->devaddr);

                    p_driver->rootport  = USBH0_NOPORT;     /* Root port */
                    p_driver->devaddr   = USBH0_NODEVICE;   /* Device devaddress */
                    p_driver->devstate  = USBH0_DETACHED;   /* Device state */
                }
            }
        }
    }

    gs_usbh0_hhub_number--;

    if(0 != gs_usbh0_hhub_info[hub_addr].pipe_id)
    {
        R_USBH0_HstdTransferEnd(gs_usbh0_hhub_info[hub_addr].pipe_id);
        R_USBH0_HstdClearPipe(hub_addr);
    }

    gs_usbh0_hhub_info[hub_addr].class_seq      = USBH0_SEQ_0;
    gs_usbh0_hhub_info[hub_addr].init_seq       = USBH0_SEQ_0;
    gs_usbh0_hhub_info[hub_addr].init_port      = USBH0_HUB_P1;
    gs_usbh0_hhub_info[hub_addr].attach_seq     = USBH0_SEQ_0;
    gs_usbh0_hhub_info[hub_addr].attach_port    = USBH0_HUB_P1;
    gs_usbh0_hhub_info[hub_addr].event_seq      = USBH0_SEQ_0;
    gs_usbh0_hhub_info[hub_addr].event_port     = USBH0_HUB_P1;
    gs_usbh0_hhub_info[hub_addr].process        = USBH0_SEQ_0;

    gs_usbh0_hhub_down_port[hub_addr] = 0;
    gs_usbh0_hhub_remote[hub_addr]    = 0;
} /* End of function usbh0_hhub_close() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_suspend
Description     : HUB sys suspend
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_suspend(uint16_t hub_addr)
{
} /* End of function usbh0_hhub_suspend() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_resume
Description     : HUB sys resume
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_resume(uint16_t hub_addr)
{
} /* End of function usbh0_hhub_resume() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_enumeration
Description     : USB HUB Class Enumeration
Arguments       : st_usbh0_utr_t *mess    : USB system internal message.
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_enumeration(st_usbh0_utr_t *p_mess)
{
    uint16_t    checkerr;
    uint8_t     string;
    uint16_t    port;

    /* Manager Mode Change */
    switch(gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq)
    {
        case USBH0_SEQ_0:
            /* String descriptor */
            usbh0_hstd_get_string_desc(gs_usbh0_hhub_hub_addr, 0, &usbh0_hhub_class_request_complete);
        break;

        case USBH0_SEQ_1:
            /* String descriptor check */
            if (USBH0_CTRL_END == p_mess->result)
            {
                string = gsp_usbh0_hhub_device_table[USBH0_DEV_I_PRODUCT];

                /* String descriptor */
                usbh0_hstd_get_string_desc(gs_usbh0_hhub_hub_addr, (uint16_t)string, &usbh0_hhub_class_request_complete);
            }
            else
            {
                /* Next sequence */
                usbh0_hhub_specified_path(USBH0_CTRL_END);
            }
        break;

        case USBH0_SEQ_2:
            /* Get HUB descriptor */
            usbh0_hhub_get_hub_descriptor(gs_usbh0_hhub_hub_addr, usbh0_hhub_class_request_complete);
        break;

        case USBH0_SEQ_3:
            /* Get HUB descriptor Check */
            if (USBH0_CTRL_END == p_mess->result)
            {
                if(gs_usbh0_hhub_descriptor[2] > USBH0_HUBDOWNPORT)
                {
                    USBH0_PRINTF0("### HUB Port number over\n");
                    checkerr = USBH0_ERROR;
                }
                else
                {
                    USBH0_PRINTF1("    Attached %d port HUB\n", gs_usbh0_hhub_descriptor[2]);
                    checkerr = USBH0_OK;
                }
            }
            else
            {
                USBH0_PRINTF0("### HUB Descriptor over\n");
                checkerr = USBH0_ERROR;
            }

            /* Port number set */
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].port_num = gs_usbh0_hhub_descriptor[2];

            /* Return to MGR */
            R_USBH0_HstdReturnEnuMGR(checkerr);

            usbh0_hhub_specified_path(USBH0_CTRL_END);
        break;

        case USBH0_SEQ_4:
            port = usbh0_hstd_get_rootport(gs_usbh0_hhub_hub_addr);
            if (USBH0_CONFIGURED == g_usbh0_hstd_mgr_mode[port])
            {
                /* Enumeration End */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq    = USBH0_SEQ_0;
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process      = USBH0_SEQ_0;
            }
            else if (USBH0_DEFAULT == g_usbh0_hstd_mgr_mode[port])
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq    = USBH0_SEQ_3;    /* Loop Sequencer */
                usbh0_hhub_specified_path_wait(p_mess->result, 20u);
            }
            else
            {
                /* Resume up-HUB Sequence */
                if(0 != gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].up_addr)
                {
                    gs_usbh0_hhub_hub_addr = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].up_addr;
                    usbh0_hhub_specified_path(USBH0_CTRL_END);
                }
                else
                {
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_SEQ_0;
                }
            }
        break;

        default:
        break;
    }

    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq++;
} /* End of function usbh0_hhub_enumeration() */


/***********************************************************************************************************************
Function Name   : usbh0_hhub_init_down_port
Description     : Down port initialized
Arguments       : st_usbh0_utr_t *mess
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_init_down_port(st_usbh0_utr_t *p_mess)
{
    if (USBH0_CTRL_END != p_mess->result)
    {
        USBH0_PRINTF2("InitDownPort result ERROR, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq);
        gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_6;
    }

    switch (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq)
    {
        case USBH0_SEQ_0:  /* Send SetFeature */
            usbh0_hhub_port_set_feature(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port,
                                        (uint16_t)USBH0_HUB_PORT_POWER, usbh0_hhub_class_request_complete);

            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_1;    /* Next Sequence */
        break;

        case USBH0_SEQ_1:  /* Check */
            if (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port >= gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].port_num)
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port    = USBH0_HUB_P1;   /* Port Clear */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq     = USBH0_SEQ_2;    /* Next Sequence */
            }
            else
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port++;             /* Next Port */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_0;  /* Loop Sequence */
            }

            usbh0_hhub_specified_path(p_mess->result);    /* Next process Selector */
        break;

        case USBH0_SEQ_2:  /* Send ClearFeasture */
            usbh0_hhub_port_clr_feature(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port,
                                        (uint16_t)USBH0_HUB_C_PORT_CONNECTION, usbh0_hhub_class_request_complete);

            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_3;    /* Next Sequence */
        break;

        case USBH0_SEQ_3:  /* Check */
            if (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port >= gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].port_num)
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port    = USBH0_HUB_P1;   /* Port Clear */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq     = USBH0_SEQ_4;    /* Next Sequence */
            }
            else
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port++;             /* Next Port */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_2;  /* Loop Sequence */
            }

            usbh0_hhub_specified_path(p_mess->result);      /* Next process Selector */
        break;

        case USBH0_SEQ_4:  /* Send GetStatus */
            usbh0_hhub_get_status(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port,
                                    usbh0_hhub_class_request_complete);

            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_5;
        break;

        case USBH0_SEQ_5:  /* Check PORT_CONNECTION */
            USBH0_PRINTF2(" *** address %d downport %d \t", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port);
            USBH0_PRINTF1(" [status] : 0x%04x\t", gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.word);
            USBH0_PRINTF1(" [change] : 0x%04x\n", gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.word);

            if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.connection)
            {
                /* Wait Power stabilization */
                R_USBH0_HstdDelayXms((uint16_t)100);
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port;
                usbh0_hhub_new_connect();   /* Go to Attach Sequence */
            }
            else
            {
                usbh0_hhub_specified_path(p_mess->result);
            }
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_6;  /* Next Attach sequence */
        break;

        case USBH0_SEQ_6:  /*  */
            if (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port >= gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].port_num)
            {   /* Port check end */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port    = USBH0_HUB_P1;   /* Port Clear */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq     = USBH0_SEQ_0;    /* Sequence Clear */

                usbh0_hhub_trans_start(gs_usbh0_hhub_hub_addr, (uint32_t)1,
                                        (uint8_t *)&gs_usbh0_hhub_data[gs_usbh0_hhub_hub_addr], &usbh0_hhub_trans_complete);

                /* Resume up-HUB Sequence */
                if (0 != gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].up_addr)
                {
                    gs_usbh0_hhub_hub_addr = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].up_addr;
                    usbh0_hhub_specified_path(p_mess->result);
                }
                else
                {
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_SEQ_0;
                }

            }
            else
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_port++;             /* Next port check */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq = USBH0_SEQ_4;  /* Loop Sequence */
                usbh0_hhub_specified_path(p_mess->result);
            }
        break;

        default:
        break;
    }
} /* End of function usbh0_hhub_init_down_port() */


/***********************************************************************************************************************
Function Name   : usbh0_hhub_port_attach
Description     : port attach
Arguments       : st_usbh0_utr_t *mess
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_port_attach(st_usbh0_utr_t *p_mess)
{
    uint16_t    rootport;
    uint16_t    speed;

    if (USBH0_CTRL_END != p_mess->result)
    {
        USBH0_PRINTF2("PortAttach result ERROR, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq);
        usbh0_hhub_port_detach(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port);
        gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_0;

        /* Return process */
        if (USBH0_SEQ_0 == gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq)
        {
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_HUB_EVENT;
        }
        else
        {
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_CLS_INIT;
        }

        usbh0_hhub_specified_path(USBH0_CTRL_END);
        return;
    }

    switch (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq)
    {
        case USBH0_SEQ_0: /* Hub port SetFeature(RESET) */
            usbh0_hhub_port_set_feature(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                        (uint16_t)USBH0_HUB_PORT_RESET, usbh0_hhub_class_request_complete);

            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_1;
        break;

        case USBH0_SEQ_1: /* GetStatus */
            /* Keep Reset */
            R_USBH0_HstdDelayXms((uint16_t)20);

            /* Send GetStatus */
            usbh0_hhub_get_status(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                    usbh0_hhub_class_request_complete);
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_2;
        break;

        case USBH0_SEQ_2: /* Check Reset Change */
            /* Check Reset Change(C_PORT_RESET) */
            if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_reset)
            {
                /* Hub port ClearFeature */
                usbh0_hhub_port_clr_feature(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                            (uint16_t)USBH0_HUB_C_PORT_RESET, usbh0_hhub_class_request_complete);

                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_3;
            }
            else
            {
                if (0 == gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.connection)
                {
                    /* Already Disconnect */
                    USBH0_PRINTF0("Reset Error\n");
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_4;
                }
                else
                {
                    /* Retry */
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_0;
                }
                usbh0_hhub_specified_path(p_mess->result);
            }
        break;

        case USBH0_SEQ_3: /* Device Enumeration */
            /* Now downport device search */
            g_usbh0_hstd_device_addr = usbh0_hhub_get_cnn_dev_addr(gs_usbh0_hhub_hub_addr,
                                                                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port);

            /* Set HUB port information to HCI */
            if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.high_speed)
            {
                g_usbh0_hstd_device_speed = USBH0_HSCONNECT;
                USBH0_PRINTF0(" Hi-Speed Device\n");
                r_usbh0_hstd_hci_set_hub_info(
                                                gs_usbh0_hhub_hub_addr,
                                                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                                g_usbh0_hstd_device_addr,
                                                USBH0_HCI_DEVSPEED_HS
                                             );
            }
            else if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.low_speed)
            {
                g_usbh0_hstd_device_speed = USBH0_LSCONNECT;
                USBH0_PRINTF0(" Low-Speed Device\n");
             r_usbh0_hstd_hci_set_hub_info(
                                                gs_usbh0_hhub_hub_addr,
                                                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                                g_usbh0_hstd_device_addr,
                                                USBH0_HCI_DEVSPEED_LS
                                             );
            }
            else
            {
                g_usbh0_hstd_device_speed = USBH0_FSCONNECT;
                USBH0_PRINTF0(" Full-Speed Device\n");
                r_usbh0_hstd_hci_set_hub_info(
                                                gs_usbh0_hhub_hub_addr,
                                                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port,
                                                g_usbh0_hstd_device_addr,
                                                USBH0_HCI_DEVSPEED_FS
                                             );
            }

            rootport = usbh0_hstd_get_rootport(gs_usbh0_hhub_hub_addr);
            g_usbh0_hstd_mgr_mode[rootport] = USBH0_DEFAULT;

            /* Set Device address of the HUB port to HCI */
            r_usbh0_hstd_hci_set_device_address_of_hub_port(gs_usbh0_hhub_hub_addr, g_usbh0_hstd_device_addr);

            /* Start Enumeration */
            usbh0_hstd_enum_get_descriptor((uint16_t)USBH0_DEVICE_0, (uint16_t)0);

            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_4;
            usbh0_hhub_specified_path(p_mess->result);
        break;

        case USBH0_SEQ_4:  /* Wait Enumeration End */
            rootport = usbh0_hstd_get_rootport(gs_usbh0_hhub_hub_addr);
            if (USBH0_DEFAULT != g_usbh0_hstd_mgr_mode[rootport])
            {
                /* Enumeration End */
                gs_usbh0_hhub_down_port[gs_usbh0_hhub_hub_addr] |= USBH0_BITSET(gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port);
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_0;

                /* Return process */
                if(USBH0_SEQ_0 == gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].init_seq)
                {
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_HUB_EVENT;
                }
                else
                {
                    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_CLS_INIT;
                }

                usbh0_hhub_specified_path(p_mess->result);
            }
            else
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_4;  /* Loop Sequencer */
                usbh0_hhub_specified_path_wait(p_mess->result, 10u);
            }
        break;

        default:
        break;
    }
} /* End of function usbh0_hhub_port_attach() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_event
Description     : USB Hub Event process.
Arguments       : st_usbh0_utr_t *mess    : USB system internal message.
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_event(st_usbh0_utr_t *p_mess)
{
    uint16_t    port_clr_feature_type;

    if (USBH0_CTRL_END != p_mess->result)
    {
        USBH0_PRINTF2("event result ERROR, Addr:%d, Seq:%d\n", gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq);
    }

    switch (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq)
    {
        case USBH0_SEQ_0:   /* Request */
            /* Hub and Port Status Change Bitmap(b0:Hub,b1:DownPort1change detected,b2:DownPort2,...) */
            if (0 != ( gs_usbh0_hhub_data[gs_usbh0_hhub_hub_addr] & USBH0_BITSET(gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port)))
            {
                USBH0_PRINTF1(" *** HUB port %d \t",gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port);

                /* GetStatus request */
                usbh0_hhub_get_status(
                                        gs_usbh0_hhub_hub_addr,
                                        gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port,
                                        usbh0_hhub_class_request_complete
                                     );

                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_1;
            }
            else
            {
                /* Port check end */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_4;
                usbh0_hhub_specified_path(p_mess->result);
            }
        break;

        case USBH0_SEQ_1:   /* Request Result Check & Request */
            if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_connection)   /* C_PORT_CONNECTION */
            {
                usbh0_hhub_port_clr_feature(
                                                gs_usbh0_hhub_hub_addr,
                                                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port,
                                                (uint16_t)USBH0_HUB_C_PORT_CONNECTION,
                                                usbh0_hhub_class_request_complete
                                            );

                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_3;   /* Attach Sequence */
            }
            else
            {
                if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_enable)               /* PORT_ENABLE */
                {
                    USBH0_PRINTF0(" C_PORT_ENABLE\n");
                    port_clr_feature_type = USBH0_HUB_C_PORT_ENABLE;
                }
                else if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_suspend)         /* PORT_SUSPEND */
                {
                    USBH0_PRINTF0(" C_PORT_SUSPEND\n");
                    port_clr_feature_type = USBH0_HUB_C_PORT_SUSPEND;
                }
                else if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_over_current)    /* PORT_OVER_CURRENT */
                {
                    USBH0_PRINTF0(" C_PORT_OVER_CURRENT\n");
                    port_clr_feature_type = USBH0_HUB_C_PORT_OVER_CURRENT;
                }
                else if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.bit.c_reset)           /* PORT_RESET */
                {
                    USBH0_PRINTF0(" C_PORT_RESET\n");
                    port_clr_feature_type = USBH0_HUB_C_PORT_RESET;
                }

                /* ClearFeature request */
                usbh0_hhub_port_clr_feature(
                                            gs_usbh0_hhub_hub_addr,
                                            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port,
                                            port_clr_feature_type,
                                            usbh0_hhub_class_request_complete
                                           );
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_2;     /* Next Sequence */
            }
        break;

        case USBH0_SEQ_2:   /* Request Result Check */
            if (gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.suspend)
            {
                /* HUB downport status */
                gs_usbh0_hhub_remote[gs_usbh0_hhub_hub_addr] |= USBH0_BITSET(gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port);
            }
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_4; /* This port event End */
            usbh0_hhub_specified_path(p_mess->result);
        break;

        case USBH0_SEQ_3:   /* check PORT_CONNECTION */
            USBH0_PRINTF1(" [status] : 0x%04x\t", gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.word);
            USBH0_PRINTF1(" [change] : 0x%04x\n", gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].change.word);

            /* connection check */
			if(gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.connection)
			{
				/* PortDisable */
				if(0 == gs_usbh0_hhub_status[gs_usbh0_hhub_hub_addr].status.bit.enable)
				{
					/* new connection */
					R_USBH0_HstdDelayXms((uint16_t)100);
					gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port;
					usbh0_hhub_new_connect();
				}
				else
				{
					/* already connected */
					usbh0_hhub_specified_path(p_mess->result);
				}
			}
			else
			{
				/* no connection */
				gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_seq = USBH0_SEQ_0;
				usbh0_hhub_port_detach(gs_usbh0_hhub_hub_addr, gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port);
				usbh0_hhub_specified_path(p_mess->result);
			}
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_4; /* This port event End */
        break;

        case USBH0_SEQ_4:   /* Attach */
            if (gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port >= gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].port_num)
            {   /* Port check end */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port   = USBH0_HUB_P1;   /* Port Clear */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq    = USBH0_SEQ_0;    /* Sequence Clear */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process      = USBH0_SEQ_0;
                usbh0_hhub_trans_start(
                                        gs_usbh0_hhub_hub_addr,
                                        1,
                                        (uint8_t*)&gs_usbh0_hhub_data[gs_usbh0_hhub_hub_addr],
                                        &usbh0_hhub_trans_complete
                                      );
            }
            else
            {
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_port++;            /* Next port check */
                gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].event_seq = USBH0_SEQ_0; /* Sequence Clear */
                usbh0_hhub_specified_path(p_mess->result);
            }
        break;

        default:
        break;
    }
} /* End of function usbh0_hhub_event() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_check_class
Description     : HUB Class driver check
Arguments       : uint16_t **table          : Descriptor, etc
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_check_class(uint16_t **pp_table)
{
    usbh0_er_t  err;
    uint8_t     hub_spec;

    gsp_usbh0_hhub_device_table     = (uint8_t*)(pp_table[0]);
    gsp_usbh0_hhub_config_table     = (uint8_t*)(pp_table[1]);
    gsp_usbh0_hhub_interface_table  = (uint8_t*)(pp_table[2]);

    gs_usbh0_hhub_hub_addr = *pp_table[7];

    /* Device class check */
    if (0x00 == gsp_usbh0_hhub_device_table[USBH0_DEV_B_DEVICE_SUBCLASS])
    {
        if (0x00 == gsp_usbh0_hhub_device_table[USBH0_DEV_B_DEVICE_PROTOCOL])
        {
            USBH0_PRINTF0("*** Full-Speed HUB device.\n\n");
            hub_spec = USBH0_FSHUB;
        }
        else if (0x01 == gsp_usbh0_hhub_device_table[USBH0_DEV_B_DEVICE_PROTOCOL])
        {
            USBH0_PRINTF0("*** High-Speed single TT device.\n\n");
            hub_spec = USBH0_HSHUBS;
        }
        else if (0x02 == gsp_usbh0_hhub_device_table[USBH0_DEV_B_DEVICE_PROTOCOL])
        {
            USBH0_PRINTF0("*** High-Speed multiple TT device.\n\n");
            hub_spec = USBH0_HSHUBM;
        }
    }

    /* Check interface number */
    switch (hub_spec)
    {
        case USBH0_FSHUB:     /* Full Speed Hub */
            if (USBH0_HUB_INTNUMFS != gsp_usbh0_hhub_config_table[USBH0_CON_B_NUM_INTERFACES])
            {
                USBH0_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        case USBH0_HSHUBS:    /* Hi Speed Hub(Single) */
            if (USBH0_HUB_INTNUMHSS != gsp_usbh0_hhub_config_table[USBH0_CON_B_NUM_INTERFACES])
            {
                USBH0_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        case USBH0_HSHUBM:    /* Hi Speed Hub(Multi) */
            if (USBH0_HUB_INTNUMHSM != gsp_usbh0_hhub_config_table[USBH0_CON_B_NUM_INTERFACES])
            {
                USBH0_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        default:
            err = USBH0_ERROR;
        break;
    }

    /* Check interface class */
    if (USBH0_IFCLS_HUB != gsp_usbh0_hhub_interface_table[USBH0_IF_B_INTERFACECLASS])
    {
        USBH0_PRINTF0("### HUB interface descriptor error !\n");
        err = USBH0_ERROR;
    }

    /* Check interface number */
    switch (hub_spec)
    {
        case USBH0_FSHUB:     /* Full Speed Hub */
            if (gsp_usbh0_hhub_interface_table[2] != (USBH0_HUB_INTNUMFS - 1u))
            {
                USBH0_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        case USBH0_HSHUBS:    /* Hi Speed Hub(Single) */
            if (gsp_usbh0_hhub_interface_table[2] != (USBH0_HUB_INTNUMHSS - 1u))
            {
                USBH0_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        case USBH0_HSHUBM:    /* Hi Speed Hub(Multi) */
            if (gsp_usbh0_hhub_interface_table[2] != (USBH0_HUB_INTNUMHSM - 1u))
            {
                USBH0_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH0_ERROR;
            }
        break;

        default:
            err = USBH0_ERROR;
        break;
    }

    err = R_USBH0_HstdSetPipe(pp_table);

    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].class_seq    = USBH0_SEQ_0;
    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process      = USBH0_MSG_CLS_CHECKREQUEST;
    usbh0_hhub_specified_path(err);
} /* End of function usbh0_hhub_check_class() */


/***********************************************************************************************************************
Function Name   : usbh0_hhub_trans_start
Description     : HUB sys data transfer / control transfer
Arguments       : uint16_t hubaddr          : hub address
                : uint32_t size             : Data Transfer size
                : uint8_t  *table           : Receive Data area
                : usbh0_utr_cb_t complete         : Callback function
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_trans_start(uint16_t hubaddr, uint32_t size, uint8_t *p_table, usbh0_utr_cb_t complete)
{
    usbh0_er_t  err;

    /* Transfer structure setting */
    gs_usbh0_hhub_data_mess[hubaddr].keyword    = gs_usbh0_hhub_info[hubaddr].pipe_id;
    gs_usbh0_hhub_data_mess[hubaddr].p_tranadr  = p_table;
    gs_usbh0_hhub_data_mess[hubaddr].tranlen    = size;
    gs_usbh0_hhub_data_mess[hubaddr].p_setup    = 0;
    gs_usbh0_hhub_data_mess[hubaddr].complete   = complete;

    /* Transfer start */
    err = R_USBH0_HstdTransferStart(&gs_usbh0_hhub_data_mess[hubaddr]);
    if(USBH0_OK != err)
    {
        /* Send Message failure */
        USBH0_PRINTF1("### usbh0_hhub_trans_start error (%ld)\n", err);
    }
} /* End of function usbh0_hhub_trans_start() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_get_hub_descriptor
Description     : Read HUB-Descriptor
Arguments       : uint16_t hubaddr          : hub address
                : usbh0_utr_cb_t complete         : callback function
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh0_hhub_get_hub_descriptor(uint16_t hubaddr, usbh0_utr_cb_t complete)
{
    usbh0_er_t    err;

    /* Request */
    gs_usbh0_hhub_class_request.type    = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_CLASS | USBH0_DEVICE;
    gs_usbh0_hhub_class_request.value   = USBH0_HUB_DESCRIPTOR;
    gs_usbh0_hhub_class_request.index   = 0;
    gs_usbh0_hhub_class_request.length  = 0x0047;
    gs_usbh0_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* HUB Descriptor */
    gs_usbh0_hhub_control_mess.keyword      = USBH0_PIPE0;
    gs_usbh0_hhub_control_mess.p_tranadr    = (void*)&gs_usbh0_hhub_descriptor[0];
    gs_usbh0_hhub_control_mess.tranlen      = (uint32_t)gs_usbh0_hhub_class_request.length;
    gs_usbh0_hhub_control_mess.p_setup      = &gs_usbh0_hhub_class_request;
    gs_usbh0_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = usbh0_hstd_transfer_start(&gs_usbh0_hhub_control_mess);

    return err;
} /* End of function usbh0_hhub_get_hub_descriptor() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_get_status
Description     : GetStatus request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : usbh0_utr_cb_t complete
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh0_hhub_get_status(uint16_t hubaddr, uint16_t port, usbh0_utr_cb_t complete)
{
    usbh0_er_t    err;

    /* Request */
    gs_usbh0_hhub_class_request.type    = USBH0_GET_STATUS| USBH0_DEV_TO_HOST | USBH0_CLASS | USBH0_OTHER;
    gs_usbh0_hhub_class_request.value   = 0;
    gs_usbh0_hhub_class_request.index   = port;     /* Port number */
    gs_usbh0_hhub_class_request.length  = 4;
    gs_usbh0_hhub_class_request.devaddr = hubaddr;  /* Device address */

    /* Port GetStatus */
    gs_usbh0_hhub_control_mess.keyword      = USBH0_PIPE0;
    gs_usbh0_hhub_control_mess.p_tranadr    = (void*)&gs_usbh0_hhub_status[hubaddr];
    gs_usbh0_hhub_control_mess.tranlen      = (uint32_t)gs_usbh0_hhub_class_request.length;
    gs_usbh0_hhub_control_mess.p_setup      = &gs_usbh0_hhub_class_request;
    gs_usbh0_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH0_HstdTransferStart(&gs_usbh0_hhub_control_mess);

    return err;
} /* End of function usbh0_hhub_get_status() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_port_set_feature
Description     : SetFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh0_hhub_port_set_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh0_utr_cb_t complete)
{
    usbh0_er_t    err;

    /* Request */
    gs_usbh0_hhub_class_request.type    = USBH0_SET_FEATURE | USBH0_HOST_TO_DEV | USBH0_CLASS | USBH0_OTHER;
    gs_usbh0_hhub_class_request.value   = command;
    gs_usbh0_hhub_class_request.index   = port;         /* Port number */
    gs_usbh0_hhub_class_request.length  = 0;
    gs_usbh0_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* Port SetFeature */
    gs_usbh0_hhub_control_mess.keyword      = USBH0_PIPE0;
    gs_usbh0_hhub_control_mess.p_tranadr    = (void*)&gs_usbh0_hhub_status[hubaddr];
    gs_usbh0_hhub_control_mess.tranlen      = (uint32_t)gs_usbh0_hhub_class_request.length;
    gs_usbh0_hhub_control_mess.p_setup      = &gs_usbh0_hhub_class_request;
    gs_usbh0_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH0_HstdTransferStart(&gs_usbh0_hhub_control_mess);

    return err;
} /* End of function usbh0_hhub_port_set_feature() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_port_clr_feature
Description     : ClearFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh0_hhub_port_clr_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh0_utr_cb_t complete)
{
    usbh0_er_t    err;

    /* Request */
    gs_usbh0_hhub_class_request.type    = USBH0_CLEAR_FEATURE | USBH0_HOST_TO_DEV | USBH0_CLASS | USBH0_OTHER;
    gs_usbh0_hhub_class_request.value   = command;
    gs_usbh0_hhub_class_request.index   = port;         /* Port number */
    gs_usbh0_hhub_class_request.length  = 0;
    gs_usbh0_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* Port ClearFeature */
    gs_usbh0_hhub_control_mess.keyword      = USBH0_PIPE0;
    gs_usbh0_hhub_control_mess.p_tranadr    = (void*)&gs_usbh0_hhub_status[hubaddr];
    gs_usbh0_hhub_control_mess.tranlen      = (uint32_t)gs_usbh0_hhub_class_request.length;
    gs_usbh0_hhub_control_mess.p_setup      = &gs_usbh0_hhub_class_request;
    gs_usbh0_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH0_HstdTransferStart(&gs_usbh0_hhub_control_mess);

    return err;
} /* End of function usbh0_hhub_port_clr_feature() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_trans_complete
Description     : Recieve complete Hub and Port Status Change Bitmap
Arguments       : st_usbh0_utr_t *utr : Transfer Request Pointer
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_trans_complete(st_usbh0_utr_t* utr)
{
    uint16_t    pipe_id;

    pipe_id = utr->keyword;
    gs_usbh0_hhub_hub_addr = usbh0_hstd_get_dev_addr(pipe_id);

    gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_SEQ_0;

    switch (utr->result)
    {
        case USBH0_DATA_SHT:
            USBH0_PRINTF1("*** receive short(pipe %d : HUB) !\n", pipe_id);
            /* Continue */
        /* Direction is in then data receive end */
        case USBH0_DATA_OK:
            gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_HUB_EVENT;
            usbh0_hhub_specified_path(USBH0_CTRL_END);
        break;

        case USBH0_DATA_STALL:
            USBH0_PRINTF0("*** Data Read error. ( STALL ) !\n");

            /* CLEAR_FEATURE */
            usbh0_hstd_clear_stall(pipe_id, &usbh0_hstd_dummy_function);
        break;

        case USBH0_DATA_OVR:
            USBH0_PRINTF0("### receiver over. !\n");
        break;

        case USBH0_DATA_STOP:
            R_USBH0_HstdTransferEnd(gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].pipe_id);
            USBH0_PRINTF0("### receiver stop. !\n");
        break;

        default:
            USBH0_PRINTF0("### HUB Class Data Read error !\n");
        break;
    }
} /* End of function usbh0_hhub_trans_complete() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_class_request_complete
Description     : Hub class check result
Arguments       : st_usbh0_utr_t *utr : Transfer Request Pointer
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_class_request_complete(st_usbh0_utr_t *p_utr)
{
    if (USBH0_DATA_STALL == p_utr->result)
    {
        USBH0_PRINTF0("*** HUB Request STALL !\n");
    }
    else if (USBH0_CTRL_END != p_utr->result)
    {
        USBH0_PRINTF0("*** HUB Request ERROR !\n");
    }
    else
    {
    }

    /* Next process Selector */
    usbh0_hhub_specified_path(p_utr->result);
} /* End of function usbh0_hhub_class_request_complete() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_new_connect
Description     : new connect
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_new_connect(void)
{
    uint16_t    devaddr;
    uint16_t    i;

    /* New downport device search */
    for (i = (USBH0_HUBDPADDR); i < (USBH0_MAXDEVADDR + 1u); i++)
    {
        /* Protect Overlap */
        if ((gs_usbh0_hhub_info[i].up_addr == gs_usbh0_hhub_hub_addr) &&
            (gs_usbh0_hhub_info[i].up_port == gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port))
        {
            devaddr = i;
            break;
        }

        if (0 == gs_usbh0_hhub_info[i].up_addr)
        {
            /* New device address */
            devaddr = i;
            break;
        }
        else
        {
            devaddr = 0;
        }
    }

    if(0 != devaddr)
    {
        USBH0_PRINTF1(" Hubport connect address%d\n", devaddr);
        gs_usbh0_hhub_info[devaddr].up_addr = gs_usbh0_hhub_hub_addr;                                   /* Up-hubaddr set */
        gs_usbh0_hhub_info[devaddr].up_port = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].attach_port;   /* Up-hubport set */
        gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process = USBH0_MSG_HUB_ATTACH;

        /* Next process Selector */
        usbh0_hhub_specified_path(USBH0_CTRL_END);
    }
    else
    {
        USBH0_PRINTF0("### device count over !\n");
    }
} /* End of function usbh0_hhub_new_connect() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_get_cnn_dev_addr
Description     : Get the connected device address
                : from the HUB address and the down port number of a HUB
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : uint16_t                  : Connected device address
***********************************************************************************************************************/
uint16_t usbh0_hhub_get_cnn_dev_addr(uint16_t hubaddr, uint16_t portnum)
{
    uint16_t    i;

    for (i = USBH0_HUBDPADDR; i < (USBH0_MAXDEVADDR + 1u); i++)
    {
        if ((gs_usbh0_hhub_info[i].up_addr == hubaddr) && (gs_usbh0_hhub_info[i].up_port == portnum))
        {
            /* Device address */
            return i;
        }
    }

    return 0;
} /* End of function usbh0_hhub_get_cnn_dev_addr() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_port_detach
Description     : HUB down port disconnect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_port_detach(uint16_t hubaddr, uint16_t portnum)
{
    uint16_t            md;
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            devaddr;        /* Device number --> DeviceAddress */

    /* HUB downport status */
    gs_usbh0_hhub_down_port[hubaddr] &= (uint16_t)(~USBH0_BITSET(portnum));

    /* HUB downport RemoteWakeup */
    gs_usbh0_hhub_remote[hubaddr] &= (uint16_t)(~USBH0_BITSET(portnum));

    /* Now downport device search */
    devaddr = usbh0_hhub_get_cnn_dev_addr(hubaddr, portnum);

    for(md = 0; md < g_usbh0_hstd_device_num; md++)
    {
        p_driver = &g_usbh0_hstd_device_drv[md];
        if(devaddr == p_driver->devaddr)
        {
            (*p_driver->devdetach)(p_driver->devaddr);
            p_driver->rootport  = USBH0_NOPORT;         /* Root port */
            p_driver->devaddr   = USBH0_NODEVICE;       /* Device address */
            p_driver->devstate  = USBH0_DETACHED;       /* Device state */
        }
    }

    /* Selective detach */
    usbh0_hhub_selective_detach(devaddr);
} /* End of function usbh0_hhub_port_detach() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_selective_detach
Description     : HUB down port Selective disconnect
Arguments       : uint16_t devaddr          : device address
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_selective_detach(uint16_t devaddr)
{
    st_usbh0_hci_device_t   *p_devinfo;

	p_devinfo = r_usbh0_hstd_hci_get_device_infomation(devaddr);
	if(USBH0_HCI_HCTYPE_OHCI == p_devinfo->bit.hctype)
	{
		usbh0_disconnect_done_queue(devaddr);
	}
    r_usbh0_hstd_hci_disconnect(devaddr);

    usbh0_hstd_set_rootport(devaddr, 0x0000);

    gs_usbh0_hhub_info[devaddr].up_addr = 0;    /* Up-address clear */
    gs_usbh0_hhub_info[devaddr].up_port = 0;    /* Up-port num clear */

    USBH0_PRINTF1("*** Device address %d clear.\n\n", devaddr);
} /* End of function usbh0_hhub_selective_detach() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_specified_path
Description     : Next process Selector
Arguments       : uint16_t result        : utr result
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_specified_path( uint16_t result)
{
    st_usbh0_utr_t  *p_blf;
    usbh0_er_t      err;

    /* Get mem pool blk */
    err = USBH0_PGET_BLK(USBH0_HUB_MPL, &p_blf);
    if (USBH0_OK == err)
    {
        p_blf->msginfo = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process;
        p_blf->result  = result;

        /* Send message */
        err = USBH0_SND_MSG(USBH0_HUB_MBX, (usbh0_msg_t*)p_blf);
        if (USBH0_OK != err)
        {
            /* Send message failure */
            err = USBH0_REL_BLK(USBH0_HUB_MPL, p_blf);
            USBH0_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USBH0_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
} /* End of function usbh0_hhub_specified_path() */

/***********************************************************************************************************************
Function Name   : usbh0_hhub_specified_path_wait
Description     : Next process Selector
Arguments       : uint16_t result        : utr result
                : uint16_t times         : Timeout value.
Return value    : none
***********************************************************************************************************************/
void usbh0_hhub_specified_path_wait(uint16_t result, uint16_t times)
{
    st_usbh0_utr_t  *p_blf;
    usbh0_er_t      err;

    /* Get mem pool blk */
    err = USBH0_PGET_BLK(USBH0_HUB_MPL, &p_blf);
    if (USBH0_OK == err)
    {
        p_blf->msginfo = gs_usbh0_hhub_info[gs_usbh0_hhub_hub_addr].process;
        p_blf->result  = result;

        /* Wait message */
        err = USBH0_WAI_MSG(USBH0_HUB_MBX, (usbh0_msg_t*)p_blf, times);
        if (USBH0_OK != err)
        {
            /* Wait message failure */
            err = USBH0_REL_BLK(USBH0_HUB_MPL, p_blf);
            USBH0_PRINTF0("### SpecifiedPassWait function snd_msg error\n");
        }
    }
    else
    {
        USBH0_PRINTF0("### SpecifiedPassWait function pget_blk error\n");
    }
} /* End of function usbh0_hhub_specified_path_wait() */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
