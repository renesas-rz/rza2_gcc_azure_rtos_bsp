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
* File Name    : r_usbh1_hHubsys.c
* Description  : USB Host Hub system code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh1_basic_if.h"
#include "r_usbh1_hhci.h"

/* Condition compilation by the difference of USB function */
/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
#define USBH1_MAXHUB                    (uint16_t)5

#define USBH1_HUB_P1                    (uint16_t)0x0001
#define USBH1_HUB_P2                    (uint16_t)0x0002
#define USBH1_HUB_P3                    (uint16_t)0x0003
#define USBH1_HUB_P4                    (uint16_t)0x0004

/* HUB spec */
#define USBH1_FSHUB                     (1u)
#define USBH1_HSHUBS                    (2u)
#define USBH1_HSHUBM                    (3u)

/* Interface number */
#define USBH1_HUB_INTNUMFS              (1u)
#define USBH1_HUB_INTNUMHSS             (1u)
#define USBH1_HUB_INTNUMHSM             (1u)

/* USB HUB task message command */
#define USBH1_MSG_HUB_EVENT             (0x0135u)
#define USBH1_MSG_HUB_ATTACH            (0x0136u)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    uint16_t        up_addr;        /* Up-address  */
    uint16_t        up_port;        /* Up-port num */
    uint16_t        port_num;       /* Port number */
    uint16_t        pipe_id;        /* Pipe number */
    uint16_t        class_seq;       /* usbh1_hhub_enumeration() Sequence */
    uint16_t        init_seq;        /* usbh1_hhub_init_down_port() Sequence */
    uint16_t        init_port;       /* usbh1_hhub_init_down_port() Process Port */
    uint16_t        event_seq;       /* usbh1_hhub_event() Sequence */
    uint16_t        event_port;      /* usbh1_hhub_event() Process Port */
    uint16_t        attach_seq;      /* usbh1_hhub_port_attach() Sequence */
    uint16_t        attach_port;     /* usbh1_hhub_port_attach() Process Port */
    uint16_t        process;        /* R_USBH1_HhubTask() msginfo */
} st_usbh1_hub_info_t;

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
} st_usbh1_hub_status_t;

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/
void                usbh1_hhub_registration(st_usbh1_hcdreg_t *p_callback);

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static void         r_usbh1_hstd_hub_tsk(void);
static void         usbh1_hhub_open(uint16_t hubaddr);
static void         usbh1_hhub_close(uint16_t hubaddr);
static void         usbh1_hhub_suspend(uint16_t hubaddr);
static void         usbh1_hhub_resume(uint16_t hubaddr);
static void         usbh1_hhub_enumeration(st_usbh1_utr_t *mess);
static void         usbh1_hhub_init_down_port(st_usbh1_utr_t *mess);
static void         usbh1_hhub_port_attach(st_usbh1_utr_t *mess);
static void         usbh1_hhub_event(st_usbh1_utr_t *mess);
static void         usbh1_hhub_check_class(uint16_t **table);
static void         usbh1_hhub_trans_start(uint16_t hubaddr, uint32_t size, uint8_t *table, usbh1_utr_cb_t complete);
static uint16_t     usbh1_hhub_get_hub_descriptor(uint16_t hubaddr, usbh1_utr_cb_t complete);
static uint16_t     usbh1_hhub_get_status(uint16_t hubaddr, uint16_t port, usbh1_utr_cb_t complete);
static uint16_t     usbh1_hhub_port_set_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh1_utr_cb_t complete);
static uint16_t     usbh1_hhub_port_clr_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh1_utr_cb_t complete);
static void         usbh1_hhub_trans_complete(st_usbh1_utr_t *utr);
static void         usbh1_hhub_class_request_complete(st_usbh1_utr_t *utr);
static void         usbh1_hhub_new_connect(void);
static uint16_t     usbh1_hhub_get_cnn_dev_addr(uint16_t hubaddr, uint16_t portnum);
static void         usbh1_hhub_port_detach(uint16_t hubaddr, uint16_t portnum);
static void         usbh1_hhub_selective_detach(uint16_t devaddr);
static void         usbh1_hhub_specified_path(uint16_t result);
static void         usbh1_hhub_specified_path_wait(uint16_t result, uint16_t times);

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

#ifdef __ICCARM__
#pragma location="USB_HOST_1"

#endif  /* __ICCARM__ */

#ifdef __GNUC__
static st_usbh1_setup_t         gs_usbh1_hhub_class_request __attribute__ ((section ("USB_HOST_1")));                       /* Control transfer setup */
static uint8_t                  gs_usbh1_hhub_descriptor[USBH1_CONFIGSIZE] __attribute__ ((section ("USB_HOST_1")));        /* HUB descriptor */
static st_usbh1_hub_status_t    gs_usbh1_hhub_status[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));       /* HUB status data */
static uint8_t                  gs_usbh1_hhub_data[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));         /* HUB receive data */
static uint8_t                  *gsp_usbh1_hhub_device_table __attribute__ ((section ("USB_HOST_1")));                      /* Device Descriptor Pointer */
static uint8_t                  *gsp_usbh1_hhub_config_table __attribute__ ((section ("USB_HOST_1")));                      /* Configuration Descriptor Pointer */
static uint8_t                  *gsp_usbh1_hhub_interface_table __attribute__ ((section ("USB_HOST_1")));                   /* Interface Descriptor Pointer */
static st_usbh1_utr_t           gs_usbh1_hhub_control_mess __attribute__ ((section ("USB_HOST_1")));                        /* Control transfer message */
static st_usbh1_utr_t           gs_usbh1_hhub_data_mess[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));    /* Data transfer message */
static uint16_t                 gs_usbh1_hhub_down_port[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));    /* HUB downport status */
static uint16_t                 gs_usbh1_hhub_remote[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));       /* Downport remotewakeup */
static st_usbh1_hub_info_t      gs_usbh1_hhub_info[USBH1_MAXDEVADDR + 1u] __attribute__ ((section ("USB_HOST_1")));         /* HUB Information */
static uint16_t                 gs_usbh1_hhub_number __attribute__ ((section ("USB_HOST_1")));                              /* HUB Connection Number */
static uint16_t                 gs_usbh1_hhub_hub_addr __attribute__ ((section ("USB_HOST_1")));                            /* Processing HUB Address */
#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "USB_HOST_1"

#endif  /* __CC_ARM */

/* Target Peripheral List */
static const uint16_t   gs_usbh1_hhub_tpl[4] =
{
    1,                      /* Number of list */
    0,                      /* Reserved */
    USBH1_NOVENDOR,         /* Vendor ID  : no-check */
    USBH1_NOPRODUCT,        /* Product ID : no-check */
};

/***********************************************************************************************************************
Renesas Abstracted Hub Driver API functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Function Name   : R_USBH1_HhubTask
Description     : Call HUB task
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void R_USBH1_HhubTask(void)
{
#if (BSP_CFG_RTOS_USED == 1)
	st_usbh1_utr_t  *p_mess;
	while(1)
	{
		USBH1_RTOS_RCV_MSG(USBH1_HUB_MSG, (usbh1_msg_t **)&p_mess);
		r_usbh1_hstd_hub_tsk();
		g_usbh1_msg_check &= ~(1<<USBH1_HUB_TSK);
	}
#else /* (BSP_CFG_RTOS_USED == 1) */
	r_usbh1_hstd_hub_tsk();
#endif /* (BSP_CFG_RTOS_USED == 1) */

} /* End of function R_USBH1_HhubTask() */

/***********************************************************************************************************************
Function Name   : r_usbh1_hstd_hub_tsk
Description     : Call HUB task
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void r_usbh1_hstd_hub_tsk(void)
{
    st_usbh1_utr_t    *p_mess;
    usbh1_er_t        err;

    /* Receive message */
    err = USBH1_RCV_MSG(USBH1_HUB_MBX, (usbh1_msg_t**)&p_mess);
    if (USBH1_OK != err)
    {
        return;
    }

    if (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process != p_mess->msginfo)
    {
//        USBH1_PRINTF0("CE\n");
    }
    else
    {
        switch (p_mess->msginfo)
        {
            case USBH1_MSG_CLS_CHECKREQUEST:
                usbh1_hhub_enumeration(p_mess);     /* USB HUB Class Enumeration */
//              USBH1_PRINTF2("enumeration, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq);
            break;

            case USBH1_MSG_CLS_INIT:
                usbh1_hhub_init_down_port(p_mess);      /* Down port initialize */
//              USBH1_PRINTF2("InitDownPort, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq);
            break;

            case USBH1_MSG_HUB_EVENT:
                usbh1_hhub_event(p_mess);
//              USBH1_PRINTF2("event, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq);
            break;

            case USBH1_MSG_HUB_ATTACH:
                usbh1_hhub_port_attach(p_mess);        /* Hub Port attach */
//              USBH1_PRINTF2("PortAttach, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq);
            break;

            default:
                USBH1_PRINTF0("HUB_TASK [0]\n");
            break;
        }
    }

    err = USBH1_REL_BLK(USBH1_HUB_MPL, p_mess);
    if (USBH1_OK != err)
    {
        USBH1_PRINTF0("### USB HUB Task rel_blk error\n");
    }
} /* End of function r_usbh1_hstd_hub_tsk() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_registration
Description     : HUB driver
Arguments       : st_usbh1_hcdreg_t *callback
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_registration(st_usbh1_hcdreg_t *p_callback)
{
    st_usbh1_hcdreg_t   driver;
    uint8_t             i;

    /* Driver registration */
    if (USBH1_NULL == p_callback)
    {
        /* Target peripheral list */
        driver.p_tpl = (uint16_t*)&gs_usbh1_hhub_tpl[0];
    }
    else
    {
        /* Target peripheral list */
        driver.p_tpl = p_callback->p_tpl;
    }

    driver.ifclass      = (uint16_t)USBH1_IFCLS_HUB;    /* Interface class */
    driver.classcheck   = &usbh1_hhub_check_class;      /* Driver check */
    driver.devconfig    = &usbh1_hhub_open;             /* Device configured */
    driver.devdetach    = &usbh1_hhub_close;            /* Device detach */
    driver.devsuspend   = &usbh1_hhub_suspend;          /* Device suspend */
    driver.devresume    = &usbh1_hhub_resume;           /* Device resume */

    for (i = 0; i < USBH1_MAXHUB; i++)
    {
        R_USBH1_HstdDriverRegistration((st_usbh1_hcdreg_t *)&driver);
    }

    for (i = 0; i < (USBH1_MAXDEVADDR + 1u); i++)
    {
        gs_usbh1_hhub_info[i].up_addr       = 0;
        gs_usbh1_hhub_info[i].up_port       = 0;
        gs_usbh1_hhub_info[i].port_num      = 0;
        gs_usbh1_hhub_info[i].pipe_id       = 0;
        gs_usbh1_hhub_info[i].init_seq      = USBH1_SEQ_0;
        gs_usbh1_hhub_info[i].init_port     = USBH1_HUB_P1;
        gs_usbh1_hhub_info[i].attach_seq    = USBH1_SEQ_0;
        gs_usbh1_hhub_info[i].attach_port   = USBH1_HUB_P1;
        gs_usbh1_hhub_info[i].event_seq     = USBH1_SEQ_0;
        gs_usbh1_hhub_info[i].event_port    = USBH1_HUB_P1;
    }

    gs_usbh1_hhub_number = 0;
} /* End of function usbh1_hhub_registration() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_open
Description     : HUB sys open
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_open(uint16_t hub_addr)
{
    if (USBH1_MAXHUB != gs_usbh1_hhub_number)
    {
        /* Pipe number set */
        gs_usbh1_hhub_info[hub_addr].pipe_id = R_USBH1_HstdGetPipeID(hub_addr, USBH1_EP_INT, USBH1_EP_IN, 0);

        /* HUB downport status */
        gs_usbh1_hhub_down_port[hub_addr] = 0;

        /* Downport remotewakeup */
        gs_usbh1_hhub_remote[hub_addr] = 0;

        gs_usbh1_hhub_info[hub_addr].process = USBH1_MSG_CLS_INIT;

        usbh1_hhub_specified_path(USBH1_CTRL_END);

        gs_usbh1_hhub_hub_addr = hub_addr;

        gs_usbh1_hhub_number++;
    }
} /* End of function usbh1_hhub_open() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_close
Description     : HUB sys close
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_close(uint16_t hub_addr)
{
    st_usbh1_hcdreg_t   *p_driver;
    uint16_t            dev_addr;
    uint16_t            md;
    uint16_t            i;

    for (i = 1; i <= gs_usbh1_hhub_info[hub_addr].port_num; i++)
    {
        /* Now downport device search */
        dev_addr = usbh1_hhub_get_cnn_dev_addr(hub_addr, i);
        if (0 != dev_addr)
        {
            /* HUB down port selective disconnect */
            usbh1_hhub_selective_detach(dev_addr);
            for (md = 0; md < g_usbh1_hstd_device_num; md++)
            {
                p_driver = &g_usbh1_hstd_device_drv[md];
                if (dev_addr == p_driver->devaddr)
                {
                    (*p_driver->devdetach)(p_driver->devaddr);

                    p_driver->rootport  = USBH1_NOPORT;     /* Root port */
                    p_driver->devaddr   = USBH1_NODEVICE;   /* Device devaddress */
                    p_driver->devstate  = USBH1_DETACHED;   /* Device state */
                }
            }
        }
    }

    gs_usbh1_hhub_number--;

    if(0 != gs_usbh1_hhub_info[hub_addr].pipe_id)
    {
        R_USBH1_HstdTransferEnd(gs_usbh1_hhub_info[hub_addr].pipe_id);
        R_USBH1_HstdClearPipe(hub_addr);
    }

    gs_usbh1_hhub_info[hub_addr].class_seq      = USBH1_SEQ_0;
    gs_usbh1_hhub_info[hub_addr].init_seq       = USBH1_SEQ_0;
    gs_usbh1_hhub_info[hub_addr].init_port      = USBH1_HUB_P1;
    gs_usbh1_hhub_info[hub_addr].attach_seq     = USBH1_SEQ_0;
    gs_usbh1_hhub_info[hub_addr].attach_port    = USBH1_HUB_P1;
    gs_usbh1_hhub_info[hub_addr].event_seq      = USBH1_SEQ_0;
    gs_usbh1_hhub_info[hub_addr].event_port     = USBH1_HUB_P1;
    gs_usbh1_hhub_info[hub_addr].process        = USBH1_SEQ_0;

    gs_usbh1_hhub_down_port[hub_addr] = 0;
    gs_usbh1_hhub_remote[hub_addr]    = 0;
} /* End of function usbh1_hhub_close() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_suspend
Description     : HUB sys suspend
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_suspend(uint16_t hub_addr)
{
} /* End of function usbh1_hhub_suspend() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_resume
Description     : HUB sys resume
Arguments       : uint16_t hubaddr          : hub address
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_resume(uint16_t hub_addr)
{
} /* End of function usbh1_hhub_resume() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_enumeration
Description     : USB HUB Class Enumeration
Arguments       : st_usbh1_utr_t *mess    : USB system internal message.
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_enumeration(st_usbh1_utr_t *p_mess)
{
    uint16_t    checkerr;
    uint8_t     string;
    uint16_t    port;

    /* Manager Mode Change */
    switch(gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq)
    {
        case USBH1_SEQ_0:
            /* String descriptor */
            usbh1_hstd_get_string_desc(gs_usbh1_hhub_hub_addr, 0, &usbh1_hhub_class_request_complete);
        break;

        case USBH1_SEQ_1:
            /* String descriptor check */
            if (USBH1_CTRL_END == p_mess->result)
            {
                string = gsp_usbh1_hhub_device_table[USBH1_DEV_I_PRODUCT];

                /* String descriptor */
                usbh1_hstd_get_string_desc(gs_usbh1_hhub_hub_addr, (uint16_t)string, &usbh1_hhub_class_request_complete);
            }
            else
            {
                /* Next sequence */
                usbh1_hhub_specified_path(USBH1_CTRL_END);
            }
        break;

        case USBH1_SEQ_2:
            /* Get HUB descriptor */
            usbh1_hhub_get_hub_descriptor(gs_usbh1_hhub_hub_addr, usbh1_hhub_class_request_complete);
        break;

        case USBH1_SEQ_3:
            /* Get HUB descriptor Check */
            if (USBH1_CTRL_END == p_mess->result)
            {
                if(gs_usbh1_hhub_descriptor[2] > USBH1_HUBDOWNPORT)
                {
                    USBH1_PRINTF0("### HUB Port number over\n");
                    checkerr = USBH1_ERROR;
                }
                else
                {
                    USBH1_PRINTF1("    Attached %d port HUB\n", gs_usbh1_hhub_descriptor[2]);
                    checkerr = USBH1_OK;
                }
            }
            else
            {
                USBH1_PRINTF0("### HUB Descriptor over\n");
                checkerr = USBH1_ERROR;
            }

            /* Port number set */
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].port_num = gs_usbh1_hhub_descriptor[2];

            /* Return to MGR */
            R_USBH1_HstdReturnEnuMGR(checkerr);

            usbh1_hhub_specified_path(USBH1_CTRL_END);
        break;

        case USBH1_SEQ_4:
            port = usbh1_hstd_get_rootport(gs_usbh1_hhub_hub_addr);
            if (USBH1_CONFIGURED == g_usbh1_hstd_mgr_mode[port])
            {
                /* Enumeration End */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq    = USBH1_SEQ_0;
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process      = USBH1_SEQ_0;
            }
            else if (USBH1_DEFAULT == g_usbh1_hstd_mgr_mode[port])
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq    = USBH1_SEQ_3;    /* Loop Sequencer */
                usbh1_hhub_specified_path_wait(p_mess->result, 20u);
            }
            else
            {
                /* Resume up-HUB Sequence */
                if(0 != gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].up_addr)
                {
                    gs_usbh1_hhub_hub_addr = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].up_addr;
                    usbh1_hhub_specified_path(USBH1_CTRL_END);
                }
                else
                {
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_SEQ_0;
                }
            }
        break;

        default:
        break;
    }

    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq++;
} /* End of function usbh1_hhub_enumeration() */


/***********************************************************************************************************************
Function Name   : usbh1_hhub_init_down_port
Description     : Down port initialized
Arguments       : st_usbh1_utr_t *mess
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_init_down_port(st_usbh1_utr_t *p_mess)
{
    if (USBH1_CTRL_END != p_mess->result)
    {
        USBH1_PRINTF2("InitDownPort result ERROR, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq);
        gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_6;
    }

    switch (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq)
    {
        case USBH1_SEQ_0:  /* Send SetFeature */
            usbh1_hhub_port_set_feature(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port,
                                        (uint16_t)USBH1_HUB_PORT_POWER, usbh1_hhub_class_request_complete);

            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_1;    /* Next Sequence */
        break;

        case USBH1_SEQ_1:  /* Check */
            if (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port >= gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].port_num)
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port    = USBH1_HUB_P1;   /* Port Clear */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq     = USBH1_SEQ_2;    /* Next Sequence */
            }
            else
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port++;             /* Next Port */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_0;  /* Loop Sequence */
            }

            usbh1_hhub_specified_path(p_mess->result);    /* Next process Selector */
        break;

        case USBH1_SEQ_2:  /* Send ClearFeasture */
            usbh1_hhub_port_clr_feature(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port,
                                        (uint16_t)USBH1_HUB_C_PORT_CONNECTION, usbh1_hhub_class_request_complete);

            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_3;    /* Next Sequence */
        break;

        case USBH1_SEQ_3:  /* Check */
            if (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port >= gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].port_num)
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port    = USBH1_HUB_P1;   /* Port Clear */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq     = USBH1_SEQ_4;    /* Next Sequence */
            }
            else
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port++;             /* Next Port */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_2;  /* Loop Sequence */
            }

            usbh1_hhub_specified_path(p_mess->result);      /* Next process Selector */
        break;

        case USBH1_SEQ_4:  /* Send GetStatus */
            usbh1_hhub_get_status(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port,
                                    usbh1_hhub_class_request_complete);

            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_5;
        break;

        case USBH1_SEQ_5:  /* Check PORT_CONNECTION */
            USBH1_PRINTF2(" *** address %d downport %d \t", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port);
            USBH1_PRINTF1(" [status] : 0x%04x\t", gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.word);
            USBH1_PRINTF1(" [change] : 0x%04x\n", gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.word);

            if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.connection)
            {
                /* Wait Power stabilization */
                R_USBH1_HstdDelayXms((uint16_t)100);
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port;
                usbh1_hhub_new_connect();   /* Go to Attach Sequence */
            }
            else
            {
                usbh1_hhub_specified_path(p_mess->result);
            }
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_6;  /* Next Attach sequence */
        break;

        case USBH1_SEQ_6:  /*  */
            if (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port >= gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].port_num)
            {   /* Port check end */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port    = USBH1_HUB_P1;   /* Port Clear */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq     = USBH1_SEQ_0;    /* Sequence Clear */

                usbh1_hhub_trans_start(gs_usbh1_hhub_hub_addr, (uint32_t)1,
                                        (uint8_t *)&gs_usbh1_hhub_data[gs_usbh1_hhub_hub_addr], &usbh1_hhub_trans_complete);

                /* Resume up-HUB Sequence */
                if (0 != gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].up_addr)
                {
                    gs_usbh1_hhub_hub_addr = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].up_addr;
                    usbh1_hhub_specified_path(p_mess->result);
                }
                else
                {
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_SEQ_0;
                }

            }
            else
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_port++;             /* Next port check */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq = USBH1_SEQ_4;  /* Loop Sequence */
                usbh1_hhub_specified_path(p_mess->result);
            }
        break;

        default:
        break;
    }
} /* End of function usbh1_hhub_init_down_port() */


/***********************************************************************************************************************
Function Name   : usbh1_hhub_port_attach
Description     : port attach
Arguments       : st_usbh1_utr_t *mess
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_port_attach(st_usbh1_utr_t *p_mess)
{
    uint16_t    rootport;
    uint16_t    speed;

    if (USBH1_CTRL_END != p_mess->result)
    {
        USBH1_PRINTF2("PortAttach result ERROR, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq);
        usbh1_hhub_port_detach(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port);
        gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_0;

        /* Return process */
        if (USBH1_SEQ_0 == gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq)
        {
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_HUB_EVENT;
        }
        else
        {
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_CLS_INIT;
        }

        usbh1_hhub_specified_path(USBH1_CTRL_END);
        return;
    }

    switch (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq)
    {
        case USBH1_SEQ_0: /* Hub port SetFeature(RESET) */
            usbh1_hhub_port_set_feature(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                        (uint16_t)USBH1_HUB_PORT_RESET, usbh1_hhub_class_request_complete);

            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_1;
        break;

        case USBH1_SEQ_1: /* GetStatus */
            /* Keep Reset */
            R_USBH1_HstdDelayXms((uint16_t)20);

            /* Send GetStatus */
            usbh1_hhub_get_status(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                    usbh1_hhub_class_request_complete);
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_2;
        break;

        case USBH1_SEQ_2: /* Check Reset Change */
            /* Check Reset Change(C_PORT_RESET) */
            if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_reset)
            {
                /* Hub port ClearFeature */
                usbh1_hhub_port_clr_feature(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                            (uint16_t)USBH1_HUB_C_PORT_RESET, usbh1_hhub_class_request_complete);

                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_3;
            }
            else
            {
                if (0 == gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.connection)
                {
                    /* Already Disconnect */
                    USBH1_PRINTF0("Reset Error\n");
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_4;
                }
                else
                {
                    /* Retry */
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_0;
                }
                usbh1_hhub_specified_path(p_mess->result);
            }
        break;

        case USBH1_SEQ_3: /* Device Enumeration */
            /* Now downport device search */
            g_usbh1_hstd_device_addr = usbh1_hhub_get_cnn_dev_addr(gs_usbh1_hhub_hub_addr,
                                                                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port);

            /* Set HUB port information to HCI */
            if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.high_speed)
            {
                g_usbh1_hstd_device_speed = USBH1_HSCONNECT;
                USBH1_PRINTF0(" Hi-Speed Device\n");
                r_usbh1_hstd_hci_set_hub_info(
                                                gs_usbh1_hhub_hub_addr,
                                                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                                g_usbh1_hstd_device_addr,
                                                USBH1_HCI_DEVSPEED_HS
                                             );
            }
            else if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.low_speed)
            {
                g_usbh1_hstd_device_speed = USBH1_LSCONNECT;
                USBH1_PRINTF0(" Low-Speed Device\n");
             r_usbh1_hstd_hci_set_hub_info(
                                                gs_usbh1_hhub_hub_addr,
                                                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                                g_usbh1_hstd_device_addr,
                                                USBH1_HCI_DEVSPEED_LS
                                             );
            }
            else
            {
                g_usbh1_hstd_device_speed = USBH1_FSCONNECT;
                USBH1_PRINTF0(" Full-Speed Device\n");
                r_usbh1_hstd_hci_set_hub_info(
                                                gs_usbh1_hhub_hub_addr,
                                                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port,
                                                g_usbh1_hstd_device_addr,
                                                USBH1_HCI_DEVSPEED_FS
                                             );
            }

            rootport = usbh1_hstd_get_rootport(gs_usbh1_hhub_hub_addr);
            g_usbh1_hstd_mgr_mode[rootport] = USBH1_DEFAULT;

            /* Set Device address of the HUB port to HCI */
            r_usbh1_hstd_hci_set_device_address_of_hub_port(gs_usbh1_hhub_hub_addr, g_usbh1_hstd_device_addr);

            /* Start Enumeration */
            usbh1_hstd_enum_get_descriptor((uint16_t)USBH1_DEVICE_0, (uint16_t)0);

            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_4;
            usbh1_hhub_specified_path(p_mess->result);
        break;

        case USBH1_SEQ_4:  /* Wait Enumeration End */
            rootport = usbh1_hstd_get_rootport(gs_usbh1_hhub_hub_addr);
            if (USBH1_DEFAULT != g_usbh1_hstd_mgr_mode[rootport])
            {
                /* Enumeration End */
                gs_usbh1_hhub_down_port[gs_usbh1_hhub_hub_addr] |= USBH1_BITSET(gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port);
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_0;

                /* Return process */
                if(USBH1_SEQ_0 == gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].init_seq)
                {
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_HUB_EVENT;
                }
                else
                {
                    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_CLS_INIT;
                }

                usbh1_hhub_specified_path(p_mess->result);
            }
            else
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_4;  /* Loop Sequencer */
                usbh1_hhub_specified_path_wait(p_mess->result, 10u);
            }
        break;

        default:
        break;
    }
} /* End of function usbh1_hhub_port_attach() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_event
Description     : USB Hub Event process.
Arguments       : st_usbh1_utr_t *mess    : USB system internal message.
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_event(st_usbh1_utr_t *p_mess)
{
    uint16_t    port_clr_feature_type;

    if (USBH1_CTRL_END != p_mess->result)
    {
        USBH1_PRINTF2("event result ERROR, Addr:%d, Seq:%d\n", gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq);
    }

    switch (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq)
    {
        case USBH1_SEQ_0:   /* Request */
            /* Hub and Port Status Change Bitmap(b0:Hub,b1:DownPort1change detected,b2:DownPort2,...) */
            if (0 != ( gs_usbh1_hhub_data[gs_usbh1_hhub_hub_addr] & USBH1_BITSET(gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port)))
            {
                USBH1_PRINTF1(" *** HUB port %d \t",gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port);

                /* GetStatus request */
                usbh1_hhub_get_status(
                                        gs_usbh1_hhub_hub_addr,
                                        gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port,
                                        usbh1_hhub_class_request_complete
                                     );

                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_1;
            }
            else
            {
                /* Port check end */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_4;
                usbh1_hhub_specified_path(p_mess->result);
            }
        break;

        case USBH1_SEQ_1:   /* Request Result Check & Request */
            if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_connection)   /* C_PORT_CONNECTION */
            {
                usbh1_hhub_port_clr_feature(
                                                gs_usbh1_hhub_hub_addr,
                                                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port,
                                                (uint16_t)USBH1_HUB_C_PORT_CONNECTION,
                                                usbh1_hhub_class_request_complete
                                            );

                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_3;   /* Attach Sequence */
            }
            else
            {
                if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_enable)               /* PORT_ENABLE */
                {
                    USBH1_PRINTF0(" C_PORT_ENABLE\n");
                    port_clr_feature_type = USBH1_HUB_C_PORT_ENABLE;
                }
                else if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_suspend)         /* PORT_SUSPEND */
                {
                    USBH1_PRINTF0(" C_PORT_SUSPEND\n");
                    port_clr_feature_type = USBH1_HUB_C_PORT_SUSPEND;
                }
                else if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_over_current)    /* PORT_OVER_CURRENT */
                {
                    USBH1_PRINTF0(" C_PORT_OVER_CURRENT\n");
                    port_clr_feature_type = USBH1_HUB_C_PORT_OVER_CURRENT;
                }
                else if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.bit.c_reset)           /* PORT_RESET */
                {
                    USBH1_PRINTF0(" C_PORT_RESET\n");
                    port_clr_feature_type = USBH1_HUB_C_PORT_RESET;
                }

                /* ClearFeature request */
                usbh1_hhub_port_clr_feature(
                                            gs_usbh1_hhub_hub_addr,
                                            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port,
                                            port_clr_feature_type,
                                            usbh1_hhub_class_request_complete
                                           );
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_2;     /* Next Sequence */
            }
        break;

        case USBH1_SEQ_2:   /* Request Result Check */
            if (gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.suspend)
            {
                /* HUB downport status */
                gs_usbh1_hhub_remote[gs_usbh1_hhub_hub_addr] |= USBH1_BITSET(gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port);
            }
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_4; /* This port event End */
            usbh1_hhub_specified_path(p_mess->result);
        break;

        case USBH1_SEQ_3:   /* check PORT_CONNECTION */
            USBH1_PRINTF1(" [status] : 0x%04x\t", gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.word);
            USBH1_PRINTF1(" [change] : 0x%04x\n", gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].change.word);

            /* connection check */
			if(gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.connection)
			{
				/* PortDisable */
				if(0 == gs_usbh1_hhub_status[gs_usbh1_hhub_hub_addr].status.bit.enable)
				{
					/* new connection */
					R_USBH1_HstdDelayXms((uint16_t)100);
					gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port;
					usbh1_hhub_new_connect();
				}
				else
				{
					/* already connected */
					usbh1_hhub_specified_path(p_mess->result);
				}
			}
			else
			{
				/* no connection */
				gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_seq = USBH1_SEQ_0;
				usbh1_hhub_port_detach(gs_usbh1_hhub_hub_addr, gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port);
				usbh1_hhub_specified_path(p_mess->result);
			}
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_4; /* This port event End */
        break;

        case USBH1_SEQ_4:   /* Attach */
            if (gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port >= gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].port_num)
            {   /* Port check end */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port   = USBH1_HUB_P1;   /* Port Clear */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq    = USBH1_SEQ_0;    /* Sequence Clear */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process      = USBH1_SEQ_0;
                usbh1_hhub_trans_start(
                                        gs_usbh1_hhub_hub_addr,
                                        1,
                                        (uint8_t*)&gs_usbh1_hhub_data[gs_usbh1_hhub_hub_addr],
                                        &usbh1_hhub_trans_complete
                                      );
            }
            else
            {
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_port++;            /* Next port check */
                gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].event_seq = USBH1_SEQ_0; /* Sequence Clear */
                usbh1_hhub_specified_path(p_mess->result);
            }
        break;

        default:
        break;
    }
} /* End of function usbh1_hhub_event() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_check_class
Description     : HUB Class driver check
Arguments       : uint16_t **table          : Descriptor, etc
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_check_class(uint16_t **pp_table)
{
    usbh1_er_t  err;
    uint8_t     hub_spec;

    gsp_usbh1_hhub_device_table     = (uint8_t*)(pp_table[0]);
    gsp_usbh1_hhub_config_table     = (uint8_t*)(pp_table[1]);
    gsp_usbh1_hhub_interface_table  = (uint8_t*)(pp_table[2]);

    gs_usbh1_hhub_hub_addr = *pp_table[7];

    /* Device class check */
    if (0x00 == gsp_usbh1_hhub_device_table[USBH1_DEV_B_DEVICE_SUBCLASS])
    {
        if (0x00 == gsp_usbh1_hhub_device_table[USBH1_DEV_B_DEVICE_PROTOCOL])
        {
            USBH1_PRINTF0("*** Full-Speed HUB device.\n\n");
            hub_spec = USBH1_FSHUB;
        }
        else if (0x01 == gsp_usbh1_hhub_device_table[USBH1_DEV_B_DEVICE_PROTOCOL])
        {
            USBH1_PRINTF0("*** High-Speed single TT device.\n\n");
            hub_spec = USBH1_HSHUBS;
        }
        else if (0x02 == gsp_usbh1_hhub_device_table[USBH1_DEV_B_DEVICE_PROTOCOL])
        {
            USBH1_PRINTF0("*** High-Speed multiple TT device.\n\n");
            hub_spec = USBH1_HSHUBM;
        }
    }

    /* Check interface number */
    switch (hub_spec)
    {
        case USBH1_FSHUB:     /* Full Speed Hub */
            if (USBH1_HUB_INTNUMFS != gsp_usbh1_hhub_config_table[USBH1_CON_B_NUM_INTERFACES])
            {
                USBH1_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        case USBH1_HSHUBS:    /* Hi Speed Hub(Single) */
            if (USBH1_HUB_INTNUMHSS != gsp_usbh1_hhub_config_table[USBH1_CON_B_NUM_INTERFACES])
            {
                USBH1_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        case USBH1_HSHUBM:    /* Hi Speed Hub(Multi) */
            if (USBH1_HUB_INTNUMHSM != gsp_usbh1_hhub_config_table[USBH1_CON_B_NUM_INTERFACES])
            {
                USBH1_PRINTF0("### HUB configuration descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        default:
            err = USBH1_ERROR;
        break;
    }

    /* Check interface class */
    if (USBH1_IFCLS_HUB != gsp_usbh1_hhub_interface_table[USBH1_IF_B_INTERFACECLASS])
    {
        USBH1_PRINTF0("### HUB interface descriptor error !\n");
        err = USBH1_ERROR;
    }

    /* Check interface number */
    switch (hub_spec)
    {
        case USBH1_FSHUB:     /* Full Speed Hub */
            if (gsp_usbh1_hhub_interface_table[2] != (USBH1_HUB_INTNUMFS - 1u))
            {
                USBH1_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        case USBH1_HSHUBS:    /* Hi Speed Hub(Single) */
            if (gsp_usbh1_hhub_interface_table[2] != (USBH1_HUB_INTNUMHSS - 1u))
            {
                USBH1_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        case USBH1_HSHUBM:    /* Hi Speed Hub(Multi) */
            if (gsp_usbh1_hhub_interface_table[2] != (USBH1_HUB_INTNUMHSM - 1u))
            {
                USBH1_PRINTF0("### HUB interface descriptor error !\n");
                err = USBH1_ERROR;
            }
        break;

        default:
            err = USBH1_ERROR;
        break;
    }

    err = R_USBH1_HstdSetPipe(pp_table);

    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].class_seq    = USBH1_SEQ_0;
    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process      = USBH1_MSG_CLS_CHECKREQUEST;
    usbh1_hhub_specified_path(err);
} /* End of function usbh1_hhub_check_class() */


/***********************************************************************************************************************
Function Name   : usbh1_hhub_trans_start
Description     : HUB sys data transfer / control transfer
Arguments       : uint16_t hubaddr          : hub address
                : uint32_t size             : Data Transfer size
                : uint8_t  *table           : Receive Data area
                : usbh1_utr_cb_t complete         : Callback function
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_trans_start(uint16_t hubaddr, uint32_t size, uint8_t *p_table, usbh1_utr_cb_t complete)
{
    usbh1_er_t  err;

    /* Transfer structure setting */
    gs_usbh1_hhub_data_mess[hubaddr].keyword    = gs_usbh1_hhub_info[hubaddr].pipe_id;
    gs_usbh1_hhub_data_mess[hubaddr].p_tranadr  = p_table;
    gs_usbh1_hhub_data_mess[hubaddr].tranlen    = size;
    gs_usbh1_hhub_data_mess[hubaddr].p_setup    = 0;
    gs_usbh1_hhub_data_mess[hubaddr].complete   = complete;

    /* Transfer start */
    err = R_USBH1_HstdTransferStart(&gs_usbh1_hhub_data_mess[hubaddr]);
    if(USBH1_OK != err)
    {
        /* Send Message failure */
        USBH1_PRINTF1("### usbh1_hhub_trans_start error (%ld)\n", err);
    }
} /* End of function usbh1_hhub_trans_start() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_get_hub_descriptor
Description     : Read HUB-Descriptor
Arguments       : uint16_t hubaddr          : hub address
                : usbh1_utr_cb_t complete         : callback function
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh1_hhub_get_hub_descriptor(uint16_t hubaddr, usbh1_utr_cb_t complete)
{
    usbh1_er_t    err;

    /* Request */
    gs_usbh1_hhub_class_request.type    = USBH1_GET_DESCRIPTOR | USBH1_DEV_TO_HOST | USBH1_CLASS | USBH1_DEVICE;
    gs_usbh1_hhub_class_request.value   = USBH1_HUB_DESCRIPTOR;
    gs_usbh1_hhub_class_request.index   = 0;
    gs_usbh1_hhub_class_request.length  = 0x0047;
    gs_usbh1_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* HUB Descriptor */
    gs_usbh1_hhub_control_mess.keyword      = USBH1_PIPE0;
    gs_usbh1_hhub_control_mess.p_tranadr    = (void*)&gs_usbh1_hhub_descriptor[0];
    gs_usbh1_hhub_control_mess.tranlen      = (uint32_t)gs_usbh1_hhub_class_request.length;
    gs_usbh1_hhub_control_mess.p_setup      = &gs_usbh1_hhub_class_request;
    gs_usbh1_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = usbh1_hstd_transfer_start(&gs_usbh1_hhub_control_mess);

    return err;
} /* End of function usbh1_hhub_get_hub_descriptor() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_get_status
Description     : GetStatus request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : usbh1_utr_cb_t complete
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh1_hhub_get_status(uint16_t hubaddr, uint16_t port, usbh1_utr_cb_t complete)
{
    usbh1_er_t    err;

    /* Request */
    gs_usbh1_hhub_class_request.type    = USBH1_GET_STATUS| USBH1_DEV_TO_HOST | USBH1_CLASS | USBH1_OTHER;
    gs_usbh1_hhub_class_request.value   = 0;
    gs_usbh1_hhub_class_request.index   = port;     /* Port number */
    gs_usbh1_hhub_class_request.length  = 4;
    gs_usbh1_hhub_class_request.devaddr = hubaddr;  /* Device address */

    /* Port GetStatus */
    gs_usbh1_hhub_control_mess.keyword      = USBH1_PIPE0;
    gs_usbh1_hhub_control_mess.p_tranadr    = (void*)&gs_usbh1_hhub_status[hubaddr];
    gs_usbh1_hhub_control_mess.tranlen      = (uint32_t)gs_usbh1_hhub_class_request.length;
    gs_usbh1_hhub_control_mess.p_setup      = &gs_usbh1_hhub_class_request;
    gs_usbh1_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH1_HstdTransferStart(&gs_usbh1_hhub_control_mess);

    return err;
} /* End of function usbh1_hhub_get_status() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_port_set_feature
Description     : SetFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh1_hhub_port_set_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh1_utr_cb_t complete)
{
    usbh1_er_t    err;

    /* Request */
    gs_usbh1_hhub_class_request.type    = USBH1_SET_FEATURE | USBH1_HOST_TO_DEV | USBH1_CLASS | USBH1_OTHER;
    gs_usbh1_hhub_class_request.value   = command;
    gs_usbh1_hhub_class_request.index   = port;         /* Port number */
    gs_usbh1_hhub_class_request.length  = 0;
    gs_usbh1_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* Port SetFeature */
    gs_usbh1_hhub_control_mess.keyword      = USBH1_PIPE0;
    gs_usbh1_hhub_control_mess.p_tranadr    = (void*)&gs_usbh1_hhub_status[hubaddr];
    gs_usbh1_hhub_control_mess.tranlen      = (uint32_t)gs_usbh1_hhub_class_request.length;
    gs_usbh1_hhub_control_mess.p_setup      = &gs_usbh1_hhub_class_request;
    gs_usbh1_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH1_HstdTransferStart(&gs_usbh1_hhub_control_mess);

    return err;
} /* End of function usbh1_hhub_port_set_feature() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_port_clr_feature
Description     : ClearFeature request
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t port             : down port number
                : uint16_t command          : request command
Return value    : uint16_t                  : DONE/ERROR
***********************************************************************************************************************/
uint16_t usbh1_hhub_port_clr_feature(uint16_t hubaddr, uint16_t port, uint16_t command, usbh1_utr_cb_t complete)
{
    usbh1_er_t    err;

    /* Request */
    gs_usbh1_hhub_class_request.type    = USBH1_CLEAR_FEATURE | USBH1_HOST_TO_DEV | USBH1_CLASS | USBH1_OTHER;
    gs_usbh1_hhub_class_request.value   = command;
    gs_usbh1_hhub_class_request.index   = port;         /* Port number */
    gs_usbh1_hhub_class_request.length  = 0;
    gs_usbh1_hhub_class_request.devaddr = hubaddr;      /* Device address */

    /* Port ClearFeature */
    gs_usbh1_hhub_control_mess.keyword      = USBH1_PIPE0;
    gs_usbh1_hhub_control_mess.p_tranadr    = (void*)&gs_usbh1_hhub_status[hubaddr];
    gs_usbh1_hhub_control_mess.tranlen      = (uint32_t)gs_usbh1_hhub_class_request.length;
    gs_usbh1_hhub_control_mess.p_setup      = &gs_usbh1_hhub_class_request;
    gs_usbh1_hhub_control_mess.complete     = complete;

    /* Transfer start */
    err = R_USBH1_HstdTransferStart(&gs_usbh1_hhub_control_mess);

    return err;
} /* End of function usbh1_hhub_port_clr_feature() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_trans_complete
Description     : Recieve complete Hub and Port Status Change Bitmap
Arguments       : st_usbh1_utr_t *utr : Transfer Request Pointer
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_trans_complete(st_usbh1_utr_t* utr)
{
    uint16_t    pipe_id;

    pipe_id = utr->keyword;
    gs_usbh1_hhub_hub_addr = usbh1_hstd_get_dev_addr(pipe_id);

    gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_SEQ_0;

    switch (utr->result)
    {
        case USBH1_DATA_SHT:
            USBH1_PRINTF1("*** receive short(pipe %d : HUB) !\n", pipe_id);
            /* Continue */
        /* Direction is in then data receive end */
        case USBH1_DATA_OK:
            gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_HUB_EVENT;
            usbh1_hhub_specified_path(USBH1_CTRL_END);
        break;

        case USBH1_DATA_STALL:
            USBH1_PRINTF0("*** Data Read error. ( STALL ) !\n");

            /* CLEAR_FEATURE */
            usbh1_hstd_clear_stall(pipe_id, &usbh1_hstd_dummy_function);
        break;

        case USBH1_DATA_OVR:
            USBH1_PRINTF0("### receiver over. !\n");
        break;

        case USBH1_DATA_STOP:
            R_USBH1_HstdTransferEnd(gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].pipe_id);
            USBH1_PRINTF0("### receiver stop. !\n");
        break;

        default:
            USBH1_PRINTF0("### HUB Class Data Read error !\n");
        break;
    }
} /* End of function usbh1_hhub_trans_complete() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_class_request_complete
Description     : Hub class check result
Arguments       : st_usbh1_utr_t *utr : Transfer Request Pointer
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_class_request_complete(st_usbh1_utr_t *p_utr)
{
    if (USBH1_DATA_STALL == p_utr->result)
    {
        USBH1_PRINTF0("*** HUB Request STALL !\n");
    }
    else if (USBH1_CTRL_END != p_utr->result)
    {
        USBH1_PRINTF0("*** HUB Request ERROR !\n");
    }
    else
    {
    }

    /* Next process Selector */
    usbh1_hhub_specified_path(p_utr->result);
} /* End of function usbh1_hhub_class_request_complete() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_new_connect
Description     : new connect
Arguments       : none
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_new_connect(void)
{
    uint16_t    devaddr;
    uint16_t    i;

    /* New downport device search */
    for (i = (USBH1_HUBDPADDR); i < (USBH1_MAXDEVADDR + 1u); i++)
    {
        /* Protect Overlap */
        if ((gs_usbh1_hhub_info[i].up_addr == gs_usbh1_hhub_hub_addr) &&
            (gs_usbh1_hhub_info[i].up_port == gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port))
        {
            devaddr = i;
            break;
        }

        if (0 == gs_usbh1_hhub_info[i].up_addr)
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
        USBH1_PRINTF1(" Hubport connect address%d\n", devaddr);
        gs_usbh1_hhub_info[devaddr].up_addr = gs_usbh1_hhub_hub_addr;                                   /* Up-hubaddr set */
        gs_usbh1_hhub_info[devaddr].up_port = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].attach_port;   /* Up-hubport set */
        gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process = USBH1_MSG_HUB_ATTACH;

        /* Next process Selector */
        usbh1_hhub_specified_path(USBH1_CTRL_END);
    }
    else
    {
        USBH1_PRINTF0("### device count over !\n");
    }
} /* End of function usbh1_hhub_new_connect() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_get_cnn_dev_addr
Description     : Get the connected device address
                : from the HUB address and the down port number of a HUB
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : uint16_t                  : Connected device address
***********************************************************************************************************************/
uint16_t usbh1_hhub_get_cnn_dev_addr(uint16_t hubaddr, uint16_t portnum)
{
    uint16_t    i;

    for (i = USBH1_HUBDPADDR; i < (USBH1_MAXDEVADDR + 1u); i++)
    {
        if ((gs_usbh1_hhub_info[i].up_addr == hubaddr) && (gs_usbh1_hhub_info[i].up_port == portnum))
        {
            /* Device address */
            return i;
        }
    }

    return 0;
} /* End of function usbh1_hhub_get_cnn_dev_addr() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_port_detach
Description     : HUB down port disconnect
Arguments       : uint16_t hubaddr          : hub address
                : uint16_t portnum          : down port number
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_port_detach(uint16_t hubaddr, uint16_t portnum)
{
    uint16_t            md;
    st_usbh1_hcdreg_t   *p_driver;
    uint16_t            devaddr;        /* Device number --> DeviceAddress */

    /* HUB downport status */
    gs_usbh1_hhub_down_port[hubaddr] &= (uint16_t)(~USBH1_BITSET(portnum));

    /* HUB downport RemoteWakeup */
    gs_usbh1_hhub_remote[hubaddr] &= (uint16_t)(~USBH1_BITSET(portnum));

    /* Now downport device search */
    devaddr = usbh1_hhub_get_cnn_dev_addr(hubaddr, portnum);

    for(md = 0; md < g_usbh1_hstd_device_num; md++)
    {
        p_driver = &g_usbh1_hstd_device_drv[md];
        if(devaddr == p_driver->devaddr)
        {
            (*p_driver->devdetach)(p_driver->devaddr);
            p_driver->rootport  = USBH1_NOPORT;         /* Root port */
            p_driver->devaddr   = USBH1_NODEVICE;       /* Device address */
            p_driver->devstate  = USBH1_DETACHED;       /* Device state */
        }
    }

    /* Selective detach */
    usbh1_hhub_selective_detach(devaddr);
} /* End of function usbh1_hhub_port_detach() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_selective_detach
Description     : HUB down port Selective disconnect
Arguments       : uint16_t devaddr          : device address
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_selective_detach(uint16_t devaddr)
{
    st_usbh1_hci_device_t   *p_devinfo;

	p_devinfo = r_usbh1_hstd_hci_get_device_infomation(devaddr);
	if(USBH1_HCI_HCTYPE_OHCI == p_devinfo->bit.hctype)
	{
		usbh1_disconnect_done_queue(devaddr);
	}
    r_usbh1_hstd_hci_disconnect(devaddr);

    usbh1_hstd_set_rootport(devaddr, 0x0000);

    gs_usbh1_hhub_info[devaddr].up_addr = 0;    /* Up-address clear */
    gs_usbh1_hhub_info[devaddr].up_port = 0;    /* Up-port num clear */

    USBH1_PRINTF1("*** Device address %d clear.\n\n", devaddr);
} /* End of function usbh1_hhub_selective_detach() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_specified_path
Description     : Next process Selector
Arguments       : uint16_t result        : utr result
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_specified_path( uint16_t result)
{
    st_usbh1_utr_t  *p_blf;
    usbh1_er_t      err;

    /* Get mem pool blk */
    err = USBH1_PGET_BLK(USBH1_HUB_MPL, &p_blf);
    if (USBH1_OK == err)
    {
        p_blf->msginfo = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process;
        p_blf->result  = result;

        /* Send message */
        err = USBH1_SND_MSG(USBH1_HUB_MBX, (usbh1_msg_t*)p_blf);
        if (USBH1_OK != err)
        {
            /* Send message failure */
            err = USBH1_REL_BLK(USBH1_HUB_MPL, p_blf);
            USBH1_PRINTF0("### SpecifiedPass function snd_msg error\n");
        }
    }
    else
    {
        /* Get memoryblock failure */
        USBH1_PRINTF0("### SpecifiedPass function pget_blk error\n");
    }
} /* End of function usbh1_hhub_specified_path() */

/***********************************************************************************************************************
Function Name   : usbh1_hhub_specified_path_wait
Description     : Next process Selector
Arguments       : uint16_t result        : utr result
                : uint16_t times         : Timeout value.
Return value    : none
***********************************************************************************************************************/
void usbh1_hhub_specified_path_wait(uint16_t result, uint16_t times)
{
    st_usbh1_utr_t  *p_blf;
    usbh1_er_t      err;

    /* Get mem pool blk */
    err = USBH1_PGET_BLK(USBH1_HUB_MPL, &p_blf);
    if (USBH1_OK == err)
    {
        p_blf->msginfo = gs_usbh1_hhub_info[gs_usbh1_hhub_hub_addr].process;
        p_blf->result  = result;

        /* Wait message */
        err = USBH1_WAI_MSG(USBH1_HUB_MBX, (usbh1_msg_t*)p_blf, times);
        if (USBH1_OK != err)
        {
            /* Wait message failure */
            err = USBH1_REL_BLK(USBH1_HUB_MPL, p_blf);
            USBH1_PRINTF0("### SpecifiedPassWait function snd_msg error\n");
        }
    }
    else
    {
        USBH1_PRINTF0("### SpecifiedPassWait function pget_blk error\n");
    }
} /* End of function usbh1_hhub_specified_path_wait() */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
