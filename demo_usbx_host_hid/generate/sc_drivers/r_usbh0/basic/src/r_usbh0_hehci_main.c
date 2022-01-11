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
* File Name    : r_usbh0_hEhciMain.c
* Description  : EHCI Main Code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh0_hhci_local.h"

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
/* for debug maclo */
//#define DEBUG_PRINT_PORT_INFOMATION
//#define DEBUG_PRINT_ASYNC_SCHEDULING
//#define DEBUG_PRINT_PERIODIC_SCHEDULING

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static uint32_t     usbh0_hstd_ehci_hw_reset(void);
static void         usbh0_hstd_ehci_set_port_route(void);
static uint32_t     usbh0_hstd_ehci_port_enable(uint32_t portnum, uint32_t *p_hs_enable_flag);
static void         usbh0_hstd_ehci_delete_periodec_scheduling_score(uint32_t speed, uint32_t pollrate,
                                                                        uint32_t pollrate_offset);
static uint32_t     usbh0_hstd_ehci_unlink_periodic(u_usbh0_ehci_flep_t *p_dataptr, uint32_t elementnum);
static void         usbh0_hstd_ehci_clear_device_trreq(uint32_t devadd);

/* For debug */
#ifdef DEBUG_PRINT_PORT_INFOMATION
static void         usbh0_hstd_ehci_print_port_information();
#endif

#ifdef DEBUG_PRINT_ASYNC_SCHEDULING
static void         usbh0_hstd_ehci_print_async_scheduling(st_usbh0_ehci_qh_t *p_h_qh);
#endif

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
static void         usbh0_hstd_ehci_print_periodec_scheduling_list(void);
static void         usbh0_hstd_ehci_print_periodic_scheduling_score(void);
#endif

static uint8_t      gs_usbh0_ehci_root_device_address[USBH0_EHCI_MAXROOTPORTS];
static uint8_t      gs_usbh0_ehci_periodic_scheduling_score[USBH0_EHCI_PFL_SIZE * 8];


/***********************************************************************************************************************
 * Function     : Initialize EHCI
 * Declaration  : uint32_t usbh0_hstd_ehci_init( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_init(void)
{
    st_usbh0_ehci_qh_t  *p_qh;
    uint32_t            timeout;
    uint32_t            i;
    uint32_t            addr;

    /* EHCI version check */
    if (USBH0_EHCI_VERSON != USB00.CAPL_VERSION.BIT.InterfaceVersionNumber)
    {
        USBH0_HCI_PRINTF1_R("HCIVERSION Error : %04x\n", USB00.CAPL_VERSION.BIT.InterfaceVersionNumber);
        return USBH0_ERROR;
    }

    /* Stop & Reset eHC */
    usbh0_hstd_ehci_hw_reset();

    for (i = 0; i < USBH0_EHCI_MAXROOTPORTS; i++)
    {
        gs_usbh0_ehci_root_device_address[i] = USBH0_HCI_NODEVICE;
    }

    /* =================== */
    /*  Set Int Threshold  */
    /* =================== */
    /*  */
    USB00.USBCMD.BIT.InterruptThresholdControl = 1;  /* Interrupt Threshold == 1 micro-frame */

    /* ============================== */
    /*  Set Periodic Frame List Size  */
    /* ============================== */
    if (USB00.HCCPARAMS.BIT.ProgramableFrameListFlag)
    {
        if (USBH0_EHCI_PFL_SIZE == 512)
        {
            USB00.USBCMD.BIT.FrameListSize = 1;  /* 512 */
        }
        else if (USBH0_EHCI_PFL_SIZE == 256)
        {
            USB00.USBCMD.BIT.FrameListSize = 2;  /* 256 */
        }
    }
    else
    {
        if (USBH0_EHCI_PFL_SIZE != 1024)
        {
            return USBH0_ERROR;   /* error */
        }
    }

    /* ========= */
    /*  Run eHC  */
    /* ========= */
    USB00.USBCMD.BIT.RS = 1;   /* Run/Stop = 1 */

    /* ============================= */
    /*  Initialize QH/qTD/iTD Array  */
    /* ============================= */
    usbh0_hstd_ehci_init_qh_memory();
    usbh0_hstd_ehci_init_qtd_memory();
    usbh0_hstd_ehci_init_itd_memory();
    usbh0_hstd_ehci_init_sitd_memory();

    /* ========================== */
    /*  Initialize Periodic List  */
    /* ========================== */
    usbh0_hstd_ehci_init_periodic_frame_list();
    memset (&gs_usbh0_ehci_periodic_scheduling_score[0], 0, sizeof(gs_usbh0_ehci_periodic_scheduling_score));

    /* Link PeriodicFrameList to PeriodicSchedule */
    addr = usbh0_hstd_ehci_get_periodic_frame_list_addr();
    R_MMU_VAtoPA(addr,&USB00.PERIODICLISTBASE.LONG);

    /* Enable Periodic Schedule */
    USB00.USBCMD.BIT.PeriodicScheduleEnable = 1;

    timeout = 0;
    while (0 == USB00.USBSTS.BIT.PeriodicScheduleStatus)
    {
        if (timeout > USBH0_EHCI_TIMEOUT)
        {
            USBH0_HCI_PRINTF0_R("Error : usbh0_hstd_EhciInit TIMEOUT\n");
            return USBH0_ERROR;
        }

        r_usbh0_hstd_hci_wait_time(1);
        timeout++;
    }

    /* ======================= */
    /*  Initialize Async List  */
    /* ======================= */
    p_qh = usbh0_hstd_ehci_alloc_qh();
    if(NULL == p_qh)
    {
        return( USBH0_ERROR );
    }

    R_MMU_VAtoPA((uint32_t)p_qh,&p_qh->queue_head_horizontal_link.address);
    p_qh->queue_head_horizontal_link.bit.typ = USBH0_EHCI_TYP_QH;
    p_qh->endpoint1.bit.h = 1;

    /* Link QH to AsyncSchuedule */
    R_MMU_VAtoPA((uint32_t)p_qh,&USB00.ASYNCLISTADDR.LONG);

    /* Enable Asynchronous Schedule */
    USB00.USBCMD.BIT.AsynchronousScheduleEnable = 1;

#ifdef DEBUG_PRINT_ASYNC_SCHEDULING
    usbh0_hstd_ehci_print_async_scheduling(p_qh);
#endif

    /* ================ */
    /*  Set Int Enable  */
    /* ================ */
    USB00.USBINTR.BIT.InterruptonAsyncAdvanceEnable = 1;
    USB00.USBINTR.BIT.HostSystemErrorEnable         = 1;
/*    USB00.USBINTR.BIT.FrameListRolloverEnable       = 1;  */
    USB00.USBINTR.BIT.PortChangeDetectEnable        = 1;
    USB00.USBINTR.BIT.USBErrorInterruptEnable       = 1;
    USB00.USBINTR.BIT.USBInterruptEnable            = 1;

    USB00.CONFIGFLAG.BIT.CF = 1;    /* CF */

    /* ================== */
    /*  Set Port Routing  */
    /* ================== */
    usbh0_hstd_ehci_set_port_route();

    return USBH0_OK;
} /* End of function usbh0_hstd_ehci_init() */

/***********************************************************************************************************************
 * Function     : Deinitialize EHCI
 * Declaration  : void usbh0_hstd_ehci_deinit( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_deinit(void)
{
    usbh0_hstd_ehci_hw_reset();     /* Stop & Reset eHC */
} /* End of function usbh0_hstd_ehci_deinit() */

/***********************************************************************************************************************
 * Function     : EHCI Port Reset
 * Declaration  : static uint32_t usbh0_hstd_ehci_port_reset( uint32_t portnum )
 * Argument     : uint32_t portnum ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_port_reset(uint32_t portnum)
{
    uint32_t    timeout;
    uint32_t                portsc1_save;

    if (0 == portnum)
    {
        /* USB00.PORTSC1.BIT.PortReset = 1; */   /* Set Port Reset */
    	portsc1_save = USB00.PORTSC1.LONG;
    	USB00.PORTSC1.LONG = 0x00000100 | (portsc1_save & 0xFFFFFFD5);
        r_usbh0_hstd_hci_wait_time(10);
        /* USB00.PORTSC1.BIT.PortReset = 0; */    /* Clear Port Reset */
    	USB00.PORTSC1.LONG &= 0xFFFFFED5;

        /* Wait Port Reset bit was cleared */
        timeout = 0;
        while (1 == USB00.PORTSC1.BIT.PortReset)
        {
            if (timeout > USBH0_EHCI_TIMEOUT)
            {
                USBH0_HCI_PRINTF0_R("Error : usbh0_hstd_ehci_port_enable TIMEOUT\n");
                return FALSE;
            }
             r_usbh0_hstd_hci_wait_time(1);
             timeout++;
        }

         r_usbh0_hstd_hci_wait_time(10);

         return TRUE;
    }
    else
    {
         USBH0_HCI_PRINTF0_R("Error : portsetting error\n");
         return FALSE;
    }
} /* End of function usbh0_hstd_ehci_port_reset() */

/***********************************************************************************************************************
 * Function     : EHCI Device Disconnect
 * Declaration  : void usbh0_hstd_ehci_disconnect( uint32_t devadd )
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_disconnect(uint32_t devadd)
{
    usbh0_hstd_ehci_clear_device_trreq(devadd);
    usbh0_hstd_ehci_clear_device_qh(devadd);
    usbh0_hstd_ehci_clear_device_itd(devadd);
    usbh0_hstd_ehci_clear_device_sitd(devadd);
} /* End of function usbh0_hstd_ehci_disconnect() */

/***********************************************************************************************************************
 * Function     : Getting of Frame list size
 * Declaration  : uint32_t usbh0_hstd_ehci_get_frame_list_size( void )
 * Argument     : void
 * Return       : uint32_t ; frame list size(1024/512/256)
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_get_frame_list_size(void)
{
    uint32_t    frame_list_size = 0;
    uint32_t    data;

    data = USB00.USBCMD.BIT.FrameListSize;
    switch (data)
    {
        case 0:
            frame_list_size = 1024;
        break;

        case 1:
            frame_list_size = 512;
        break;

        case 2:
            frame_list_size = 256;
        break;

        default:
        break;
    }

    return frame_list_size;
} /* End of function usbh0_hstd_ehci_get_frame_list_size() */

/***********************************************************************************************************************
 * Function     : This function is waited for until becoming a specified micro frame number.
 * Declaration  : void usbh0_hstd_ehci_wait_micro_frame( uint32_t uframe_no )
 * Argument     : uint32_t uframe_no ; micro frame number
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_wait_micro_frame(uint32_t uframe_no)
{
    while ((USB00.FRINDEX.LONG & 0x00000003) == uframe_no);
} /* End of function usbh0_hstd_ehci_wait_micro_frame() */

/***********************************************************************************************************************
 * Function     : Add QH to AsyncSchedule
 * Declaration  : void usbh0_hstd_ehci_add_async( st_usbh0_ehci_qh_t *qh )
 * Argument     : st_usbh0_ehci_qh_t *qh ; QH pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_add_async(st_usbh0_ehci_qh_t *p_qh)
{
    uint32_t            async_list_addr;
    st_usbh0_ehci_qh_t  *p_qh_base;
    st_usbh0_ehci_qh_t  *p_qh_base_temp;
    u_usbh0_ehci_flep_t tmp;

    async_list_addr = USB00.ASYNCLISTADDR.LONG;
    if (0x00000000 == async_list_addr)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_async Error : ASYNCLISTADDR is not registered. \n");
        return;
    }
    else
    {
        p_qh_base = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va(async_list_addr);
        if (p_qh != p_qh_base)
        {
            /* QH addition */
            tmp.address = r_usbh0_pa_to_va(p_qh_base->queue_head_horizontal_link.address);
            tmp.bit.typ = USBH0_EHCI_TYP_QH;
            R_MMU_VAtoPA((uint32_t)tmp.address,&p_qh->queue_head_horizontal_link.address);

            tmp.address = (uint32_t)r_usbh0_pa_to_va(p_qh);
            tmp.bit.typ = USBH0_EHCI_TYP_QH;
            R_MMU_VAtoPA((uint32_t)tmp.address,&p_qh_base->queue_head_horizontal_link.address);
        }
        else
        {
            USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_async Error : The QH has already been registered. \n");
            return;
        }
    }

#ifdef DEBUG_PRINT_ASYNC_SCHEDULING
    usbh0_hstd_ehci_print_async_scheduling(NULL);
#endif
} /* End of function usbh0_hstd_ehci_add_async() */

/***********************************************************************************************************************
 * Function     : Add QH/ITD/SITD to PeriodicSchedule
 * Declaration  : void usbh0_hstd_ehci_add_periodic( u_usbh0_ehci_flep_t *new_element, uint32_t type, uint32_t pollrate, uint32_t pollrate_offset )
 * Argument     : u_usbh0_ehci_flep_t *new_element   ; QH/ITD/SITD pointer
 *              : uint32_t type            ; element type( QH/ITD/SITD )
 *              : uint32_t pollrate        ; pollrate
 *              : uint32_t pollrate_offset ; pollrate offset
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_add_periodic(u_usbh0_ehci_flep_t *p_new_element,uint32_t type, uint32_t pollrate, uint32_t pollrate_offset)
{
    uint32_t                        frame_list_size;
    uint32_t                        n;
    uint32_t                        periodic_list_addr;
    uint32_t                        pollrate_ms;
    uint32_t                        offset_ms;
    uint32_t                        current_element_pollrate;
    uint32_t                        tmp_addr;
    u_usbh0_ehci_flep_t             *p_listbase;
    volatile u_usbh0_ehci_flep_t    *p_current_element;
    volatile u_usbh0_ehci_flep_t    tmp_element;

    if (0 == pollrate)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_periodic Error: pollrate = 0\n");
        return;
    }

    if (USBH0_EHCI_TYP_FSTN == type)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_periodic Error: USBH0_EHCI_TYP_FSTN is not support\n");
        return;
    }

    if (NULL == p_new_element)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_periodic Error: Element Pointer is NULL\n");
        return;
    }

    /* Get frame list size */
    frame_list_size = usbh0_hstd_ehci_get_frame_list_size();

    periodic_list_addr = USB00.PERIODICLISTBASE.LONG & 0xFFFFF000;
    if (0x00000000 == periodic_list_addr)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_periodic Error : PERIODICLISTBASE is not registered. \n");
        return;
    }
    else
    {
        periodic_list_addr = r_usbh0_pa_to_va(periodic_list_addr);
    }

    offset_ms = pollrate_offset / 8;
    pollrate_ms = pollrate / 8;
    if (pollrate_ms == 0)
    {
        pollrate_ms = 1;
    }

    /*-----------*/
    /* Interrupt */
    /*-----------*/
    /* The content of the periodic frame list is changed.  */
    /* Attention: In this schedule, the Paul rate should be an involution value of two. */
    p_listbase = (u_usbh0_ehci_flep_t *)periodic_list_addr;
    for (n = offset_ms; n < frame_list_size; n += pollrate_ms)
    {
        p_current_element = &p_listbase[n];

        while (1)
        {
            if (NULL == p_current_element)
            {
                USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_add_periodic Error: Current Element Pointer is NULL\n");
                return;
            }

            /* If T bit is 1, the new element is added. */
            if (1 == p_current_element->bit.t)
            {
                p_new_element->address = 0x00000001;
                R_MMU_VAtoPA((uint32_t)p_new_element,&tmp_element.address);
                tmp_element.bit.typ = type;
                *p_current_element = tmp_element;
                break;
            }

            tmp_addr = r_usbh0_pa_to_va((uint32_t)(p_current_element->address & 0xFFFFFFE0));

            /* If a current element is the same as the new element, loop termination.  */
            if (tmp_addr == (uint32_t)p_new_element)
            {
                break;
            }

            /* The paulrate of a current element is acquired. */
            switch (p_current_element->bit.typ)
            {
                case USBH0_EHCI_TYP_ITD:
                    current_element_pollrate = ((st_usbh0_ehci_itd_t *)tmp_addr)->info.pollrate;
                break;

                case USBH0_EHCI_TYP_QH:
                    current_element_pollrate = ((st_usbh0_ehci_qh_t *)tmp_addr)->info.pollrate;
                break;

                case USBH0_EHCI_TYP_SITD:
                    current_element_pollrate = ((st_usbh0_ehci_sitd_t *)tmp_addr)->info.pollrate;
                break;

                default:
                    current_element_pollrate = 0xFFFFFFFF;
                break;
            }

            /* If paulrates are compared, and the value of the new element is large, the new element is added. */
            if (pollrate > current_element_pollrate)
            {
            	R_MMU_VAtoPA(tmp_addr,&tmp_element.address);
                tmp_element.bit.typ = p_current_element->bit.typ;
                *p_new_element = tmp_element;

                R_MMU_VAtoPA((uint32_t)p_new_element,&tmp_element.address);
                tmp_element.bit.typ = type;
                *p_current_element = tmp_element;
                break;
            }

            /* It shifts to the next element.  */
            p_current_element = (u_usbh0_ehci_flep_t *)r_usbh0_pa_to_va((uint32_t)(p_current_element->address & 0xFFFFFFE0));
        }
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodec_scheduling_list();
#endif
} /* End of function usbh0_hstd_ehci_add_periodic() */

/***********************************************************************************************************************
 * Function     : Link qTDs
 * Declaration  : void usbh0_hstd_ehci_link_qtd( st_usbh0_ehci_qtd_t *qtd1st, st_usbh0_ehci_qtd_t *qtd2nd )
 * Argument     : st_usbh0_ehci_qtd_t *qtd1st ; QTD pointer
 *              : st_usbh0_ehci_qtd_t *qtd2nd ; QTD pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_link_qtd(st_usbh0_ehci_qtd_t *p_qtd1st, st_usbh0_ehci_qtd_t *p_qtd2nd)
{
	R_MMU_VAtoPA((uint32_t)p_qtd2nd,&p_qtd1st->next_qtd.address);
    p_qtd1st->alternate_next_qtd.address = p_qtd1st->next_qtd.address;
} /* End of function usbh0_hstd_ehci_link_qtd() */

/***********************************************************************************************************************
 * Function     : Clear qTDs
 * Declaration  : void usbh0_hstd_ehci_clear_qtd( st_usbh0_ehci_qtd_t *qtd_head )
 * Argument     : st_usbh0_ehci_qtd_t *qtd_head ; QTD head pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_qtd(st_usbh0_ehci_qtd_t *p_qtd_head)
{
    st_usbh0_ehci_qtd_t *p_clear_qtd;
    st_usbh0_ehci_qtd_t *p_next_qtd;

    p_clear_qtd = r_usbh0_pa_to_va((uint32_t)p_qtd_head);

    while (1)
    {
        if (NULL == p_clear_qtd)
        {
            USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_clear_qtd Error : QTD pointer is NULL. \n");
            return;
        }

        if (1 == p_clear_qtd->next_qtd.bit.t)
        {
            memset(p_clear_qtd, 0, sizeof(st_usbh0_ehci_qtd_t));
            usbh0_hstd_ehci_free_qtd(p_clear_qtd);
            break;
        }
        else
        {
            p_next_qtd = (st_usbh0_ehci_qtd_t *)r_usbh0_pa_to_va((uint32_t)(p_clear_qtd->next_qtd.address & 0xFFFFFFE0));
            memset(p_clear_qtd, 0, sizeof(st_usbh0_ehci_qtd_t));
            usbh0_hstd_ehci_free_qtd(p_clear_qtd);
            p_clear_qtd = p_next_qtd;
        }
    }
} /* End of function usbh0_hstd_ehci_clear_qtd() */

/***********************************************************************************************************************
 * Function     : Inactive qTDs
 * Declaration  : void usbh0_hstd_ehci_inactive_qtd( st_usbh0_ehci_qtd_t *qtd_head )
 * Argument     : st_usbh0_ehci_qtd_t *qtd_head ; QTD head pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_inactive_qtd(st_usbh0_ehci_qtd_t *p_qtd_head)
{
    st_usbh0_ehci_qtd_t *p_clear_qtd;

    p_clear_qtd = r_usbh0_pa_to_va((uint32_t)p_qtd_head);

    while(1)
    {
        if(NULL == p_clear_qtd)
        {
            USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_inactive_qtd Error : QTD pointer is NULL. \n");
            return;
        }

        /* Clear active bit */
        p_clear_qtd->transfer_info.bit.status_active = 0;

        if (1 == p_clear_qtd->next_qtd.bit.t)
        {
            break;
        }

        p_clear_qtd = (st_usbh0_ehci_qtd_t *)r_usbh0_pa_to_va((uint32_t)(p_clear_qtd->next_qtd.address & 0xFFFFFFE0));
    }
} /* End of function usbh0_hstd_ehci_inactive_qtd() */

/***********************************************************************************************************************
 * Function     : Unlink Async-QH( CNT/BULK ) from AsyncList
 * Declaration  : void usbh0_hstd_ehci_unlink_qh_for_async( st_usbh0_ehci_qh_t *qh )
 * Argument     : st_usbh0_ehci_qh_t *qh ; QH pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_unlink_qh_for_async(st_usbh0_ehci_qh_t *p_qh)
{
    st_usbh0_ehci_qh_t  *p_asynclist;
    st_usbh0_ehci_qh_t  *p_current_element;
    uint32_t            current_contents;
    uint32_t            timeout;

    if (NULL == p_qh)
    {
        return;
    }

    if ((USB00.ASYNCLISTADDR.LONG & 0xFFFFFFE0) == 0x00000000)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_unlink_qh_for_async Error : ASYNCLISTADDR is not registered. \n");
        return;
    }

    p_asynclist = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va((uint32_t)(USB00.ASYNCLISTADDR.LONG & 0xFFFFFFE0));

    /*-----------------*/
    /* check asynclist */
    /*-----------------*/
    /* When the next link of QH is oneself */
    if (((uint32_t)p_qh == r_usbh0_pa_to_va((uint32_t)(p_qh->queue_head_horizontal_link.address & 0xFFFFFFE0))))
    {
        /* The termination of the asynchronization schedule is executed */
        if (p_qh != p_asynclist)
        {
            USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_unlink_qh_for_async Error : Illegal case 1\n");
            return;
        }

        /* disable Asynchronous Schedule */
        USB00.USBCMD.BIT.AsynchronousScheduleParkModeEnable = 0;

        /* wait async schedule disabled */
        timeout = 0;
        while (1 == USB00.USBSTS.BIT.PeriodicScheduleStatus)
        {
            if (timeout > USBH0_EHCI_TIMEOUT)
            {
                break;
            }
            r_usbh0_hstd_hci_wait_time(1);
            timeout++;
        }

        /* clear AsyncSchuedule */
        USB00.ASYNCLISTADDR.LONG = 0;
        return;
    }

    /* When you delete qh specified with asynclist by chance */
    if (p_qh == p_asynclist)
    {
        /* The setting of asynclist is changed to the next QH */
        p_asynclist = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va((uint32_t)(p_asynclist->queue_head_horizontal_link.address & 0xFFFFFFE0));
    }

    /*---------*/
    /* scan QH */
    /*---------*/
    p_current_element = p_asynclist;
    current_contents = r_usbh0_pa_to_va((uint32_t)(p_current_element->queue_head_horizontal_link.address & 0xFFFFFFE0));
    while (current_contents != (uint32_t)p_asynclist)
    {
        if (1 == p_current_element->queue_head_horizontal_link.bit.t)
        {
            /* The terminal is shown though it is a list of the round loop (This case is usually impossible) */
            USBH0_HCI_PRINTF0_R( "usbh0_hstd_ehci_unlink_qh_for_async Error : Illegal case 2\n" );
            return;
        }
        else if (current_contents == (uint32_t)p_qh)
        {
        	R_MMU_VAtoPA((uint32_t)p_qh->queue_head_horizontal_link.address,&p_current_element->queue_head_horizontal_link.address);
            break;
        }
        else
        {
            p_current_element = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va((uint32_t)(p_current_element->queue_head_horizontal_link.address & 0xFFFFFFE0));
            if (NULL == p_current_element)
            {
                USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_unlink_qh_for_async Error: Current Element Pointer is NULL\n");
                return;
            }

            current_contents = r_usbh0_pa_to_va((uint32_t)(p_current_element->queue_head_horizontal_link.address & 0xFFFFFFE0));
        }
    }

    /*-----------*/
    /* unlink QH */
    /*-----------*/
    /* doorbell */
    USB00.USBCMD.BIT.InterruptonAsyncAdvanceDoorbell = 1;

    /* It is necessary to wait here for 1ms */
    r_usbh0_hstd_hci_wait_time(1);
    timeout = 0;
    while (1 == USB00.USBSTS.BIT.InterruptonAsyncAdvance)
    {
        if (timeout > USBH0_EHCI_TIMEOUT)
        {
            break;
        }
        r_usbh0_hstd_hci_wait_time(1);
        timeout++;
    }

    /* USB00.USBSTS.BIT.InterruptonAsyncAdvance = 1; */
    USB00.USBSTS.LONG &= 0x0001F020;

#ifdef DEBUG_PRINT_ASYNC_SCHEDULING
    usbh0_hstd_ehci_print_async_scheduling(NULL);
#endif
} /* End of function usbh0_hstd_ehci_unlink_qh_for_async() */

/***********************************************************************************************************************
 * Function     : Unlink Periodic-QH( INT ) from Periodic Frame List
 * Declaration  : void usbh0_hstd_ehci_unlink_qh_for_periodic( st_usbh0_ehci_qh_t *qh )
 * Argument     : st_usbh0_ehci_qh_t *qh ; QH pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_unlink_qh_for_periodic(st_usbh0_ehci_qh_t *p_qh)
{
    uint32_t    n;
    uint32_t    frame_list_size;
    uint32_t    max_period;

    if(p_qh == NULL)
    {
        return;
    }

    /* Get frame list size */
    frame_list_size = usbh0_hstd_ehci_get_frame_list_size();

    /* unlink qh */
    for (n = 0; n < frame_list_size; n++)
    {
        usbh0_hstd_ehci_unlink_periodic(&p_qh->queue_head_horizontal_link, n);
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodec_scheduling_list();
#endif

    if (p_qh->info.pollrate != 0)
    {
        if (p_qh->endpoint1.bit.eps == USBH0_HCI_DEVSPEED_HS)
        {
            max_period = 1;
        }
        else
        {
            /* The maximum term of 5 frames is set for the split transaction of the interrupt forwarding */
            max_period = 5;
        }

        usbh0_hstd_ehci_delete_periodec_scheduling_score(max_period, p_qh->info.pollrate, p_qh->info.pollrate_offset);
    }
} /* End of function usbh0_hstd_get_rootport() */

/***********************************************************************************************************************
 * Function     : Unlink Periodic-ITD( HS-ISO ) from Periodic Frame List
 * Declaration  : void usbh0_hstd_ehci_unlink_itd( st_usbh0_ehci_itd_t *itd )
 * Argument     : st_usbh0_ehci_itd_t *itd ; ITD pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_unlink_itd(st_usbh0_ehci_itd_t *p_itd)
{
    uint32_t    n;
    uint32_t    frame_list_size;
    uint32_t    max_period;

    if (NULL == p_itd)
    {
        return;
    }

    /* Get frame list size */
    frame_list_size = usbh0_hstd_ehci_get_frame_list_size();

    /* unlink itd */
    for(n = 0; n < frame_list_size; n++)
    {
        usbh0_hstd_ehci_unlink_periodic(&p_itd->next_link, n);
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodec_scheduling_list();
#endif

    if (0 != p_itd->info.pollrate)
    {
        max_period = 1;
        usbh0_hstd_ehci_delete_periodec_scheduling_score(max_period, p_itd->info.pollrate, p_itd->info.pollrate_offset);
    }
} /* End of function usbh0_hstd_ehci_unlink_itd() */

/***********************************************************************************************************************
 * Function     : Unlink Periodic-SITD( FS-ISO ) from Periodic Frame List
 * Declaration  : void usbh0_hstd_ehci_unlink_sitd( st_usbh0_ehci_sitd_t *sitd )
 * Argument     : st_usbh0_ehci_sitd_t *sitd ; SITD pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_unlink_sitd(st_usbh0_ehci_sitd_t *p_sitd)
{
    uint32_t    n;
    uint32_t    frame_list_size;
    uint32_t    max_period;

    if (NULL == p_sitd)
    {
        return;
    }

    /* Get frame list size */
    frame_list_size = usbh0_hstd_ehci_get_frame_list_size();

    /* unlink sitd */
    for (n = 0; n < frame_list_size; n++)
    {
        usbh0_hstd_ehci_unlink_periodic(&p_sitd->next_link, n);
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodec_scheduling_list();
#endif

    if (0 != p_sitd->info.pollrate)
    {
        /* The number of micro frames necessary for transfer is calculated */
        max_period = (p_sitd->mps + USBH0_EHCI_SITD_DATA_SIZE - 1) / USBH0_EHCI_SITD_DATA_SIZE;

        if (USBH0_HCI_DIRECTION_IN == p_sitd->info.direction)
        {
            /* The IN transfer adds 2 micro frame */
            max_period += 2;
        }

        usbh0_hstd_ehci_delete_periodec_scheduling_score(max_period, p_sitd->info.pollrate, p_sitd->info.pollrate_offset);
    }
} /* End of function usbh0_hstd_ehci_unlink_sitd() */

/***********************************************************************************************************************
 * Function     : Update Periodeic Scheuling Score
 * Declaration  : uint32_t usbh0_hstd_ehci_update_periodic_scheduling_score( uint32_t max_period, uint32_t pollrate )
 * Argument     : uint32_t max_period ; max period
 *              : uint32_t pollrate   ; pollrate
 * Return       : uint32_t            ; pollrate offset
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_update_periodic_scheduling_score(uint32_t max_period, uint32_t pollrate)
{
    uint32_t    uframe_list_size;
    uint32_t    n;
    uint32_t    i;
    uint32_t    j;
    uint32_t    score;
    uint32_t    score_min;
    uint32_t    score_min_n;
    uint32_t    pollrate_offset;

    /* Get micro frame list size */
    uframe_list_size = usbh0_hstd_ehci_get_frame_list_size();
    uframe_list_size *= 8;

    /* Search for minimum score */
    score_min = 0xFFFFFFFF;
    for (n = 0; n < pollrate; )
    {
        /* Acquisition of score */
        score = 0;
        for (j = 0; j < max_period; j++)
        {
            for (i = n + j; i < uframe_list_size; i += pollrate)
            {
                score += gs_usbh0_ehci_periodic_scheduling_score[i];
            }
        }

        if (score_min > score)
        {
            /* Because a minimum score at present was found, the value is kept */
            score_min = score;
            score_min_n = n;
        }

        if (0 == score_min)
        {
            /* Because minimum score 0 was found, it ends here */
            break;
        }

        if (1 == max_period)
        {
            n++;
        }
        else
        {
            /* The split transaction that steps over the frame is not scheduled */
            if ((n % 8) == (8 - max_period))
            {
                n += max_period;
            }
            else
            {
                n++;
            }
        }
    }

    pollrate_offset = score_min_n % pollrate;

    /* Update Score */
    for (n = pollrate_offset; n < uframe_list_size; n += pollrate)
    {
        for (j = 0; j < max_period; j++)
        {
            gs_usbh0_ehci_periodic_scheduling_score[n + j]++;
        }
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodic_scheduling_score();
#endif

    return pollrate_offset;
} /* End of function usbh0_hstd_get_rootport() */

/***********************************************************************************************************************
 * Function     : EHCI Transfer End Interrupt Handler
 * Declaration  : void usbh0_hstd_ehci_int_transfer_end( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_int_transfer_end(void)
{
    st_usbh0_hci_tr_req_t   *p_tr_req;
    st_usbh0_ehci_qh_t      *p_qh;
    st_usbh0_ehci_itd_t     *p_itd;
    st_usbh0_ehci_sitd_t    *p_sitd;
    uint32_t                i;
    uint8_t                 error_status;
    st_usbh0_ehci_qtd_t     *tmp_qtd_end;

    p_tr_req = &g_usbh0_hstd_hci_transfer_request[0];
    for (i = 0; i < USBH0_HCI_TRREQ_NUM; i++, p_tr_req++)
    {
        if (TRUE == p_tr_req->bit.enable)
        {
            if (NULL == p_tr_req->devinfo)
            {
                continue;
            }

            if (USBH0_HCI_HCTYPE_EHCI == p_tr_req->devinfo->bit.hctype)
            {
                if (NULL != r_usbh0_pa_to_va(p_tr_req->hci_info))
                {
                    if (USBH0_EP_ISO == p_tr_req->bit.eptype)
                    {
                        /* for isochronous */
                        if (USBH0_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
                        {
                            p_itd = (st_usbh0_ehci_itd_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);
                            if (TRUE == p_itd->info.tr_req_flag)
                            {
                                /* check status */
                                if (1 == p_itd->transaction[p_itd->next_trend_uframe].bit.status_data_buffer_error)
                                {
                                    USBH0_HCI_PRINTF1_R("Error ITD(%d): Data Buffer Error\n", p_tr_req->bit.devadrs);
                                }

                                if (1 == p_itd->transaction[p_itd->next_trend_uframe].bit.status_babble_detected)
                                {
                                    USBH0_HCI_PRINTF1_R("Error ITD(%d): Babble Detected\n", p_tr_req->bit.devadrs);
                                }

                                if (1 == p_itd->transaction[p_itd->next_trend_uframe].bit.status_transaction_error)
                                {
                                    USBH0_HCI_PRINTF1_R("Error ITD(%d): Transaction Error\n", p_tr_req->bit.devadrs);
                                }

                                /* High-speed isochronous */
                                if (0 == p_itd->transaction[p_itd->next_trend_uframe].bit.status_active)
                                {
                                    usbh0_hstd_ehci_transfer_end_itd(p_tr_req);
                                }
                            }
                        }
                        else
                        {
                            p_sitd = (st_usbh0_ehci_sitd_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);
                            if (TRUE == p_sitd->info.tr_req_flag)
                            {
                                /* check status */
                                if (1 == p_sitd->state.bit.status_err)
                                {
                                    USBH0_HCI_PRINTF1_R("Error SITD(%d): ERR Respons\n", p_tr_req->bit.devadrs);
                                }

                                if (1 == p_sitd->state.bit.status_data_buffer_error)
                                {
                                    USBH0_HCI_PRINTF1_R("Error SITD(%d): Data Buffer Error\n", p_tr_req->bit.devadrs);
                                }

                                if (1 == p_sitd->state.bit.status_babble_detected)
                                {
                                    USBH0_HCI_PRINTF1_R("Error SITD(%d): Babble Detected\n", p_tr_req->bit.devadrs);
                                }

                                if(1 == p_sitd->state.bit.status_transaction_error)
                                {
                                    USBH0_HCI_PRINTF1_R("Error SITD(%d): Transaction Error\n", p_tr_req->bit.devadrs);
                                }

                                if(1 == p_sitd->state.bit.status_missed_micro_frame)
                                {
                                  /*  USBH0_HCI_PRINTF1_R("Error SITD(%d): Missed Micro Frame\n", p_tr_req->bit.devadrs); */
                                }

                                /* Full-speed isochronous( Split transaction ) */
                                if ((1 == p_sitd->state.bit.ioc) && (0 == p_sitd->state.bit.status_active))
                                {
                                    usbh0_hstd_ehci_transfer_end_sitd(p_tr_req);
                                }
                            }
                        }
                    }
                    else
                    {
                        /* Error Status Clear */
                        error_status = 0x00;

                        /* for interrupt/control/bulk */
                        p_qh = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);
                        
                        tmp_qtd_end = (st_usbh0_ehci_qtd_t *)r_usbh0_pa_to_va((uint32_t)(p_qh->qtd_end));
                        
                        if (TRUE == p_qh->info.tr_req_flag)
                        {
                            /* check status */
                            if (3 != p_qh->transfer_info.bit.cerr)
                            {
                                if (0 == p_qh->transfer_info.bit.cerr)
                                {
                                    error_status |= 0x01;
                                }
                                USBH0_HCI_PRINTF2_R("Error QH(%d): CERR=%d\n", p_tr_req->bit.devadrs, p_qh->transfer_info.bit.cerr);
                            }

                            if (1 == p_qh->transfer_info.bit.status_halted)
                            {
                                error_status |= 0x02;
                                USBH0_HCI_PRINTF1_R("Error QH(%d): Halted\n", p_tr_req->bit.devadrs);
                            }

                            if (1 == p_qh->transfer_info.bit.status_data_buffer_error)
                            {
                                error_status |= 0x04;
                                USBH0_HCI_PRINTF1_R("Error QH(%d): Data Buffer Error\n", p_tr_req->bit.devadrs);
                            }

                            if (1 == p_qh->transfer_info.bit.status_babble_detected)
                            {
                                error_status |= 0x08;
                                USBH0_HCI_PRINTF1_R("Error QH(%d): Babble Detected\n", p_tr_req->bit.devadrs);
                            }

                            if (1 == p_qh->transfer_info.bit.status_transaction_error)
                            {
                                error_status |= 0x10;
                                USBH0_HCI_PRINTF1_R("Error QH(%d): Transaction Error\n", p_tr_req->bit.devadrs);
                                p_tr_req->bit.enable = FALSE;
                            }

                            if (1 == p_qh->transfer_info.bit.status_missed_micro_frame)
                            {
                              /* USBH0_HCI_PRINTF1_R("Error QH(%d): Missed Micro Frame\n", p_tr_req->bit.devadrs); */
                            }

                            if (1 == p_qh->transfer_info.bit.status_halted)
                            {
                                /* Transfer completion when abnormality operating */
                                usbh0_hstd_ehci_transfer_end_qh(p_tr_req);
                            }
                            else if ((1 == tmp_qtd_end->transfer_info.bit.ioc) &&
                                       (0 == tmp_qtd_end->transfer_info.bit.status_active))
                            {
                                /* Transfer completion when normality operating */
                                usbh0_hstd_ehci_transfer_end_qh(p_tr_req);
                            }
                            else if (0x00 != error_status)
                            {
                                /* Error Check */
                                if (0 == p_qh->current_qtd.pointer->transfer_info.bit.status_active)
                                {
                                    USBH0_HCI_PRINTF2_R("Error QH(%d): Error Stop! Error Status=0x%x\n", p_tr_req->bit.devadrs, error_status);
                                    usbh0_hstd_ehci_transfer_end_qh(p_tr_req);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
} /* End of function usbh0_hstd_ehci_int_transfer_end() */

/***********************************************************************************************************************
 * Function     : EHCI Port Change Interrupt Handler
 * Declaration  : void usbh0_hstd_ehci_int_port_change( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_int_port_change(void)
{
    st_usbh0_hci_device_t   *p_devinfo;
    uint32_t                n;
    uint32_t                connect_flag;
    uint32_t                hs_enable_flag;
    uint32_t                count;
    uint32_t                portsc1_save;

    n = 0;

#ifdef DEBUG_PRINT_PORT_INFOMATION
    usbh0_hstd_ehci_print_port_information();
#endif

    if (1 == USB00.PORTSC1.BIT.PortOwner)
    {
        return;
    }

    if (1 == USB00.PORTSC1.BIT.OvercurrentChange)
    {
        if (1 == USB00.PORTSC1.BIT.OvercurrentActive)
        {
            /* over current callback */
            g_usbh0_hstd_hci_callback.over_current_cb(n);
        }
    }

    if (1 == USB00.PORTSC1.BIT.PortEnable_DisableChange)
    {
        if (1 == USB00.PORTSC1.BIT.PortEnabled_Disabled)
        {
            /* port enable */
        }
        else
        {
            /* port disable */
        }
    }

    if (1 == USB00.PORTSC1.BIT.ConnectStatusChange)
    {
        /* Here is waited for until Current_Connect_Status is fixed */
        count = 0;
        while (count < 11)
        {
            connect_flag = USB00.PORTSC1.BIT.CurrentConnectStatus;
            r_usbh0_hstd_hci_wait_time(10);
            if (connect_flag == USB00.PORTSC1.BIT.CurrentConnectStatus)
            {
                count++;
            }
            else
            {
                count = 0;
            }
        }

        if ((0 == USB00.PORTSC1.BIT.CurrentConnectStatus) && (0 == USB00.PORTSC1.BIT.PortOwner))
        {
            if(USBH0_HCI_NODEVICE != gs_usbh0_ehci_root_device_address[n])
            {
                r_usbh0_hstd_hci_disconnect(gs_usbh0_ehci_root_device_address[n]);
                gs_usbh0_ehci_root_device_address[n] = USBH0_HCI_NODEVICE;

                /* detach callback */
                g_usbh0_hstd_hci_callback.detach_cb(n);
            }
        }
        else
        {
            if (USBH0_HCI_NODEVICE != gs_usbh0_ehci_root_device_address[n])
            {
                r_usbh0_hstd_hci_disconnect(gs_usbh0_ehci_root_device_address[n]);
                gs_usbh0_ehci_root_device_address[n] = USBH0_HCI_NODEVICE;
            }

            if (TRUE == usbh0_hstd_ehci_port_enable(n, &hs_enable_flag))
            {
                if (TRUE == hs_enable_flag)
                {
                    /* For an enable port */
                    p_devinfo = usbh0_hstd_hci_alloc_dev_info();
                    if (NULL != p_devinfo)
                    {
                        p_devinfo->bit.devnum = 0;
                        gs_usbh0_ehci_root_device_address[n] = p_devinfo->bit.devnum;
                        p_devinfo->bit.devspeed = USBH0_HCI_DEVSPEED_HS;
                        p_devinfo->bit.hctype = USBH0_HCI_HCTYPE_EHCI;

                        /* attach callback */
                        g_usbh0_hstd_hci_callback.attach_cb( n );
                    }
                }
                else
                {

                    /* The port owner is changed to OHCI in case of the port disable */
                    /* USB00.PORTSC1.BIT.PortOwner = 1; */  /* Port Owner : OHCI */
                	portsc1_save = USB00.PORTSC1.LONG;
                	USB00.PORTSC1.LONG = 0x00002000 | (portsc1_save & 0xFFFFFFD5);
                    r_usbh0_hstd_hci_wait_time(50);
                }
            }
        }
    }
} /* End of function usbh0_hstd_ehci_int_port_change() */

/***********************************************************************************************************************
 * Function     :
 * Declaration  : uint32_t usbh0_hstd_ehci_get_port_current_connect_status( uint16_t rootport )
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint32_t          ; Current Connect Status( 1:Connected, 0:Not Connect );
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_get_port_current_connect_status(uint16_t rootport)
{
    uint32_t    n_ports;
    uint32_t    status;

    n_ports = USB00.HCSPARAMS.BIT.N_PORTS;
    if(rootport >= n_ports)
    {
        return 0;
    }

    status = USB00.PORTSC1.BIT.CurrentConnectStatus;
    status |= USB00.PORTSC1.BIT.PortOwner;

    return status;
} /* End of function usbh0_hstd_ehci_get_port_current_connect_status() */

/***********************************************************************************************************************
 * Function     : EHCI Clear Enumalation Wait Flag
 * Declaration  : void usbh0_hstd_ehci_clear_enumalation_wait_flag( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_enumalation_wait_flag(void)
{
} /* End of function usbh0_hstd_ehci_clear_enumalation_wait_flag() */

/***********************************************************************************************************************
 * Function     : EHCI Interrupt Handler
 * Declaration  : void usbh0_hstd_ehci_interrupt_handler( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_interrupt_handler(void)
{

    if (0 != USB00.USBSTS.LONG && 0 != USB00.USBINTR.LONG)
    {
        if ((1 == USB00.USBSTS.BIT.HostSystemError) && (1 == USB00.USBINTR.BIT.HostSystemErrorEnable))
        {
            usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_HOST_SYSTEM_ERROR, 0, 0, 0);
            /* USB00.USBSTS.BIT.HostSystemError = 1; */
            USB00.USBSTS.LONG &= 0x0001F010;
        }

        if ((1 == USB00.USBSTS.BIT.PortChangeDetect) && (1 == USB00.USBINTR.BIT.PortChangeDetectEnable))
        {
            usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_PORT_CHANGE_DETECT, 0, 0, 0);
            /* USB00.USBSTS.BIT.PortChangeDetect = 1; */
            USB00.USBSTS.LONG &= 0x0001F004;
        }

        if ((1 == USB00.USBSTS.BIT.USBERRINT) && (1 == USB00.USBINTR.BIT.USBErrorInterruptEnable))
        {
            usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_USBERRINT, 0, 0, 0);
            /* USB00.USBSTS.BIT.USBERRINT = 1; */
            USB00.USBSTS.LONG &= 0x0001F0002;
        }

        if ((1 == USB00.USBSTS.BIT.InterruptonAsyncAdvance) && (1 == USB00.USBINTR.BIT.InterruptonAsyncAdvanceEnable))
        {
            usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_INTERRUPT_ON_ASYNC_ADVANCE, 0, 0, 0);
            /* USB00.USBSTS.BIT.InterruptonAsyncAdvance = 1; */
            USB00.USBSTS.LONG &= 0x0001F020;
        }

        if ((1 == USB00.USBSTS.BIT.FrameListRollover) && (1 == USB00.USBINTR.BIT.FrameListRolloverEnable))
        {
            usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_FRAME_LIST_ROLLOVER, 0, 0, 0);
            /* USB00.USBSTS.BIT.FrameListRollover = 1; */
            USB00.USBSTS.LONG &= 0x0001F008;
        }

        if ((1 == USB00.USBSTS.BIT.USBINT) && (1 == USB00.USBINTR.BIT.USBInterruptEnable))
        {
        	usbh0_hstd_hci_send_message_from_int(USBH0_HCI_TASK, USBH0_HCI_MESS_EHCI_USBINT, 0, 0, 0);
        	/* USB00.USBSTS.BIT.USBINT = 1; */
        	USB00.USBSTS.LONG &= 0x0001F001;
        }

    }
} /* End of function usbh0_hstd_ehci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : EHCI Getting of device address of rootpoot
 * Declaration  : uint16_t usbh0_hstd_ehci_get_device_address_of_rootpoot( uint16_t rootport )
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t usbh0_hstd_ehci_get_device_address_of_rootpoot(uint16_t rootport)
{
    if(USBH0_EHCI_MAXROOTPORTS > rootport)
    {
        return gs_usbh0_ehci_root_device_address[rootport];
    }

    return USBH0_HCI_NODEVICE;
} /* End of function usbh0_hstd_ehci_get_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : The device address of EHCI route port is set.
 * Declaration  : uint16_t usbh0_hstd_ehci_set_device_address_of_rootpoot( uint16_t rootport, uint16_t devaddr )
 * Argument     : uint16_t rootport ; rootport no
 *              : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t usbh0_hstd_ehci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t deviceaddr)
{
    st_usbh0_hci_device_t *p_devinfo;

    if (USBH0_EHCI_MAXROOTPORTS <= rootport)
    {
        return USBH0_HCI_NODEVICE;
    }

    if (0 != gs_usbh0_ehci_root_device_address[rootport])
    {
        return USBH0_HCI_NODEVICE;
    }

    /* The device address of device information is updated */
    p_devinfo = r_usbh0_hstd_hci_get_device_infomation(gs_usbh0_ehci_root_device_address[rootport]);
    if(NULL == p_devinfo)
    {
        return USBH0_HCI_NODEVICE;
    }

    p_devinfo->bit.devnum = deviceaddr;

    gs_usbh0_ehci_root_device_address[rootport] = deviceaddr;

    return deviceaddr;
} /* End of function usbh0_hstd_ehci_set_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : Getting of EHCI Transfer End Status
 * Declaration  : uint16_t usbh0_hstd_ehci_get_transfer_end_status( st_usbh0_hci_tr_req_t *tr_req )
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : uint16_t               ; USB Standard F/W Transfer End Status
 * Note         :
***********************************************************************************************************************/
uint16_t usbh0_hstd_ehci_get_transfer_end_status(st_usbh0_hci_tr_req_t *p_tr_req)
{
    u_usbh0_ehci_transfer_info_t    status;
    uint16_t                        ret = USBH0_DATA_ERR;

    if (NULL != p_tr_req)
    {
        status.dword = p_tr_req->status;

        if (FALSE == p_tr_req->bit.enable)
        {
            ret = USBH0_DATA_STOP;
        }
        else if (USBH0_EP_ISO == p_tr_req->bit.eptype)
        {
            if (1 == status.bit.status_halted)
            {
            }
            else if (1 == status.bit.status_data_buffer_error)
            {
            }
            else if (1 == status.bit.status_babble_detected)
            {
            }
            else if (1 == status.bit.status_transaction_error)
            {
            }
//            else if (1 == status.bit.status_missed_micro_frame)
//            {
//            }
            else
            {
                /* For isochronous */
                if (USBH0_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
                {
                    ret = USBH0_DATA_WRITING;
                }
                else
                {
                    ret = USBH0_DATA_READING;
                }
            }
        }
        else
        {
            if (1 == status.bit.status_halted)
            {
                ret = USBH0_DATA_STOP;
                if (0 == status.bit.cerr)
                {
                }
                else if (1 == status.bit.status_data_buffer_error)
                {
                }
                else if (1 == status.bit.status_babble_detected)
                {
                }
                else if (1 == status.bit.status_transaction_error)
                {
                }
                else if (1 == status.bit.status_missed_micro_frame)
                {
                }
                else
                {
                    if (3 == status.bit.cerr)
                    {
                        /* For STALL */
                        ret = USBH0_DATA_STALL;
                    }
                }
            }
            else
            {
                if (USBH0_EP_CNTRL == p_tr_req->bit.eptype)
                {
                    /* For control */
                    ret = USBH0_CTRL_END;
                }
                else
                {
                    if (USBH0_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
                    {
                        /* When the data sending ends normally */
                        ret = USBH0_DATA_NONE;
                    }
                    else
                    {
                        if (p_tr_req->actual_size > p_tr_req->trsize)
                        {
                            /* When receive data is size over */
                            ret = USBH0_DATA_OVR;
                        }
                        else if (p_tr_req->actual_size < p_tr_req->trsize)
                        {
                            /* When receive data is short size */
                            ret = USBH0_DATA_SHT;
                        }
                        else
                        {
                            /* When the data reception ends normally */
                            ret = USBH0_DATA_OK;
                        }
                    }
                }
            }
        }
    }
    return ret;
} /* End of function usbh0_hstd_ehci_get_transfer_end_status() */

/***********************************************************************************************************************
 * Function     : Getting of EHCI PID Status
 * Declaration  : uint16_t usbh0_hstd_ehci_get_pid_status( st_usbh0_hci_tr_req_t *tr_req )
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : uint16_t               ; PID Status
 * Note         :
***********************************************************************************************************************/
uint16_t usbh0_hstd_ehci_get_pid_status(st_usbh0_hci_tr_req_t *p_tr_req)
{
    st_usbh0_ehci_qh_t  *p_qh;

    if (NULL != p_tr_req)
    {
        if (TRUE == p_tr_req->bit.enable)
        {
            if (USBH0_EP_ISO != p_tr_req->bit.eptype)
            {
                p_qh = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va(p_tr_req->hci_info);
                if (0 == p_qh->transfer_info.bit.status_halted)
                {
                    return USBH0_PID_BUF;
                }
                else
                {
                    if (3 == p_qh->transfer_info.bit.cerr)
                    {
                        return USBH0_PID_STALL;
                    }
                }
            }
        }
    }

    return USBH0_PID_NAK;
} /* End of function usbh0_hstd_ehci_get_pid_status() */

/***********************************************************************************************************************
 * Function     : EHCI Port Suspend
 * Declaration  : void usbh0_hstd_ehci_port_suspend( uint16_t port )
 * Argument     : uint16_t port ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_port_suspend(uint16_t port)
{
	uint32_t portsc1_save;

    if (USBH0_EHCI_MAXROOTPORTS <= port)
    {
        USBH0_HCI_PRINTF1_R("usbh0_hstd_ehci_port_suspend Error: Illegal port no(%d)\n", port);
        return;
    }

    /* USB00.PORTSC1.BIT.WKOC_E        = 1; */    /* Over-Current Detect Enable */
    /* USB00.PORTSC1.BIT.WKDSCNNT_E    = 1; */    /* DisConnect Enable */
    /* USB00.PORTSC1.BIT.WKCNNT_E      = 1; */    /* Device Connect Enable */
	portsc1_save = USB00.PORTSC1.LONG;
	USB00.PORTSC1.LONG = 0x00700000 | (portsc1_save & 0xFFFFFFD5);
    /* USB00.PORTSC1.BIT.Suspend       = 1; */    /* Set Suspend */
	portsc1_save = USB00.PORTSC1.LONG;
	USB00.PORTSC1.LONG = 0x00000080 | (portsc1_save & 0xFFFFFFD5);

} /* End of function usbh0_hstd_ehci_port_suspend() */

/***********************************************************************************************************************
 * Function     : EHCI Port Resume
 * Declaration  : void usbh0_hstd_ehci_port_resume( uint16_t port )
 * Argument     : uint16_t port ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_port_resume(uint16_t port)
{

	uint32_t                portsc1_save;

    if (USBH0_EHCI_MAXROOTPORTS <= port)
    {
        USBH0_HCI_PRINTF1_R("usbh0_hstd_ehci_port_resume Error: Illegal port no(%d)\n", port);
        return;
    }

    if ((1 == USB00.PORTSC1.BIT.Suspend) && (1 == USB00.PORTSC1.BIT.PortEnabled_Disabled))
    {
        /* Set Force Port Resume */
        /* USB00.PORTSC1.BIT.ForcePortResume = 1; */
    	portsc1_save = USB00.PORTSC1.LONG;
    	USB00.PORTSC1.LONG = 0x00000040 | (portsc1_save & 0xFFFFFFD5);
        r_usbh0_hstd_hci_wait_time(20);
        do
        {
            /* Set Force Port Resume */
            /* USB00.PORTSC1.BIT.ForcePortResume = 0; */
        	USB00.PORTSC1.LONG &= 0xFFFFFF95;
            r_usbh0_hstd_hci_wait_time(1);
        } while ((USB00.PORTSC1.BIT.Suspend != 0) && (USB00.PORTSC1.BIT.PortEnabled_Disabled != 0));
    }
} /* End of function usbh0_hstd_ehci_port_resume() */

/***********************************************************************************************************************
 * Function     : EHCI Chancel Transfer Request
 * Declaration  : usbh0_hstd_ehci_cancel_transfer_request( st_usbh0_hci_tr_req_t *tr_req )
 * Argument     : st_usbh0_hci_tr_req_t *tr_req ; Transfer Request Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_cancel_transfer_request(st_usbh0_hci_tr_req_t *p_tr_req)
{
    st_usbh0_ehci_qh_t      *p_qh;
    st_usbh0_ehci_itd_t     *p_itd;
    st_usbh0_ehci_sitd_t    *p_sitd;
    uint32_t                n;

    if (NULL == p_tr_req)
    {
        return;
    }

    if (NULL != r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info))
    {
        if (USBH0_EP_ISO == p_tr_req->bit.eptype)
        {
            if (USBH0_HCI_DEVSPEED_HS == p_tr_req->devinfo->bit.devspeed)
            {
                p_itd = (st_usbh0_ehci_itd_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);

                /* inactivate itd */
                for (n = 0; n < 8; n++)
                {
                    p_itd->transaction[n].bit.status_active = 0;
                }
            }
            else
            {
                p_sitd = (st_usbh0_ehci_sitd_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);

                /* inactivate sitd */
                p_sitd->state.bit.status_active = 0;
            }
        }
        else
        {
            p_qh = (st_usbh0_ehci_qh_t *)r_usbh0_pa_to_va((uint32_t)p_tr_req->hci_info);

            /* inactivate qh */
            if (NULL != p_qh->qtd_head)
            {
                /* inactive qtd */
                usbh0_hstd_ehci_inactive_qtd(p_qh->qtd_head);
            }

            p_qh->transfer_info.bit.status_active = 0;

            /* delete qtd */
            if (NULL != p_qh->qtd_head)
            {
                usbh0_hstd_ehci_clear_qtd(p_qh->qtd_head);
                p_qh->qtd_head = NULL;
            }
        }
    }

    /* delete tr-req */
    p_tr_req->bit.enable = FALSE;
} /* End of function usbh0_hstd_ehci_cancel_transfer_request() */

/* Static Function */
/***********************************************************************************************************************
 * Function     : Reset EHCI H/W
 * Declaration  : void usbh0_hstd_ehci_hw_reset( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh0_hstd_ehci_hw_reset(void)
{
    uint32_t    timeout;

    /* ========== */
    /*  Stop eHC  */
    /* ========== */
    USB00.USBCMD.BIT.RS = 0;    /* Clear Run/Stop bit. Stop HC */

    timeout = 0;
    while (0 == USB00.USBSTS.BIT.HCHalted)  /* Wait Halted == 1 */
    {
        if (timeout > USBH0_EHCI_TIMEOUT)
        {
            USBH0_HCI_PRINTF0_R("Error : usbh0_hstd_ehci_init TIMEOUT\n");
            return USBH0_ERROR;
        }

        r_usbh0_hstd_hci_wait_time(1);
        timeout++;
    }

    /* =========== */
    /*  Reset eHC  */
    /* =========== */
    USB00.USBCMD.BIT.HCRESET = 1;   /* Reset */

    timeout = 0;
    while (1 == USB00.USBCMD.BIT.HCRESET)
    {
        if (timeout > USBH0_EHCI_TIMEOUT)
        {
            USBH0_HCI_PRINTF0_R("Error : usbh0_hstd_ehci_init TIMEOUT\n");
            return USBH0_ERROR;
        }

        r_usbh0_hstd_hci_wait_time(1);
        timeout++;
    }

    return USBH0_OK;
} /* End of function usbh0_hstd_ehci_hw_reset() */

/***********************************************************************************************************************
 * Function     : Set Port Routing
 * Declaration  : static void usbh0_hstd_ehci_set_port_route( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh0_hstd_ehci_set_port_route(void)
{
    uint32_t    n_ports;
    uint32_t    n;
    uint32_t    portsc1_save;

    /* =============== */
    /*  Set Ownership  */
    /* =============== */
    n_ports = USB00.HCSPARAMS.BIT.N_PORTS;

    for (n = 0; n < n_ports; n++)
    {
        /* USB00.PORTSC1.BIT.PortOwner = 0; */    /* Port Owner : EHCI */
        /* USB00.PORTSC1.BIT.PP        = 1; */    /* Port Power : ON */
    	portsc1_save = USB00.PORTSC1.LONG;
    	USB00.PORTSC1.LONG = 0x00001000 | (portsc1_save & 0xFFFFDFD5);
        PORTC.PODR.BIT.PODR6 = 0x0;     /* debug */

        if (1 == USB00.PORTSC1.BIT.OvercurrentChange)
        {
            /* USB00.PORTSC1.BIT.OvercurrentChange = 1; */   /* Clear Over-curent */
        	portsc1_save = USB00.PORTSC1.LONG;
        	USB00.PORTSC1.LONG = 0x00000020 | (portsc1_save & 0xFFFFFFD5);
        }

        r_usbh0_hstd_hci_wait_time(100);    /* Wait port power stabilization time */
    }
} /* End of function usbh0_hstd_ehci_set_port_route() */

/***********************************************************************************************************************
 * Function     : EHCI Port Enable
 * Declaration  : uint32_t usbh0_hstd_ehci_port_enable(uint32_t portnum, uint32_t *hs_enable_flag)
 * Argument     : uint32_t portnum ; port no
 * Return       : uint32_t *hs_enable_flag ; HS Device Enable Flag pointer
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh0_hstd_ehci_port_enable(uint32_t portnum, uint32_t *p_hs_enable_flag)
{
    *p_hs_enable_flag = FALSE;

    /* Device existence confirmation */
    if (0 == USB00.PORTSC1.BIT.CurrentConnectStatus)
    {
        return FALSE;
    }

    /* For Low-Speed Device */
    if (USBH0_EHCI_LINE_STATE_K == USB00.PORTSC1.BIT.LineStatus)
    {
        return TRUE;
    }

    /* Port Reset */
    if (FALSE == usbh0_hstd_ehci_port_reset(portnum))
    {
        return FALSE;
    }

    /* Device existence confirmation */
    if (0 == USB00.PORTSC1.BIT.CurrentConnectStatus)
    {
        return FALSE;
    }

    /* HS Device Enable confirmation */
    if (1 == USB00.PORTSC1.BIT.PortEnabled_Disabled)
    {
        *p_hs_enable_flag = TRUE;
    }

    return TRUE;
} /* End of function usbh0_hstd_ehci_port_enable() */

/***********************************************************************************************************************
 * Function     : Delete Periodeic Scheuling Score
 * Declaration  : void usbh0_hstd_ehci_delete_periodec_scheduling_score( uint32_t max_period, uint32_t pollrate, uint32_t pollrate_offset )
 * Argument     : uint32_t max_period ; max period
 *              : uint32_t pollrate   ; pollrate
 *              : uint32_t            ; pollrate offset
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh0_hstd_ehci_delete_periodec_scheduling_score(uint32_t max_period, uint32_t pollrate, uint32_t pollrate_offset)
{
    uint32_t    uframe_list_size;
    uint32_t    n;
    uint32_t    j;

    /* Get micro frame list size */
    uframe_list_size = usbh0_hstd_ehci_get_frame_list_size();
    uframe_list_size *= 8;

    /* Update of score */
    for (n = pollrate_offset; n < uframe_list_size; n += pollrate)
    {
        for (j = 0; j < max_period; j++)
        {
            if (0 != gs_usbh0_ehci_periodic_scheduling_score[n + j])
            {
                gs_usbh0_ehci_periodic_scheduling_score[n + j]--;
            }
            else
            {
                USBH0_HCI_PRINTF0_R("Error: Can't delete Periodic Scheduing Score\n");
            }
        }
    }

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
    usbh0_hstd_ehci_print_periodic_scheduling_score();
#endif
} /* End of function usbh0_hstd_ehci_delete_periodec_scheduling_score() */

/***********************************************************************************************************************
 * Function     : Unlink data structure from periodic frame list
 * Declaration  : uint32_t usbh0_hstd_ehci_unlink_periodic( u_usbh0_ehci_flep_t *data, uint32_t elementnum )
 * Argument     : u_usbh0_ehci_flep_t *data ; data structure pointer
 *              : uint32_t elementnum ; periodic frame list no
 * Return       : uint32_t status     ; unlink flag
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh0_hstd_ehci_unlink_periodic(u_usbh0_ehci_flep_t *p_data, uint32_t elementnum)
{
    uint32_t            periodic_list_addr;
    uint32_t            tmp_addr;
    u_usbh0_ehci_flep_t *p_current_element;

    periodic_list_addr = USB00.PERIODICLISTBASE.LONG & 0xFFFFF000;
    if (0x00000000 == periodic_list_addr)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_unlink_periodic Error : PERIODICLISTBASE is not registered. \n");
        return 0;
    }
    else
    {
        periodic_list_addr = r_usbh0_pa_to_va(periodic_list_addr);
    }

    /* The address of the periodic frame list specified with elementnum is calculated.  */
    tmp_addr = elementnum * sizeof(uint32_t);
    tmp_addr &= 0x00000FFC;
    tmp_addr |= periodic_list_addr;
    p_current_element = (u_usbh0_ehci_flep_t *)tmp_addr;

    while (1)
    {
        if (NULL == p_current_element)
        {
            USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_unlink_periodic Error: Current Element Pointer is NULL\n");
            return 0;
        }

        if (1 == p_current_element->bit.t)
        {
            break;
        }
        if (r_usbh0_pa_to_va((uint32_t)(p_current_element->address & 0xFFFFFFE0)) == (uint32_t)p_data)
        {
            /* The value of the next link pointer of qh/itd/sitd is set */
            *p_current_element = *p_data;
            return 1;
        }
        else
        {
            /* The nest qh/itd/sitd is retrieved referring to the next link pointer.  */
            p_current_element = (u_usbh0_ehci_flep_t *)r_usbh0_pa_to_va((uint32_t)(p_current_element->address & 0xFFFFFFE0));
        }
    }

    return 0;
} /* End of function usbh0_hstd_ehci_unlink_periodic() */

/***********************************************************************************************************************
 * Function     : EHCI Clear transfer request for disconnected device
 * Declaration  : void usbh0_hstd_ehci_clear_device_trreq( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh0_hstd_ehci_clear_device_trreq(uint32_t devadd)
{
    uint8_t     n;

    for (n = 0; n < USBH0_HCI_TRREQ_NUM; n++)
    {
        if ((1 == g_usbh0_hstd_hci_transfer_request[n].bit.enable)
                && (devadd == g_usbh0_hstd_hci_transfer_request[n].bit.devadrs))
        {
            usbh0_hstd_ehci_cancel_transfer_request(&g_usbh0_hstd_hci_transfer_request[n]);
        }
    }
} /* End of function usbh0_hstd_ehci_clear_device_trreq() */

#ifdef DEBUG_PRINT_PORT_INFOMATION
/***********************************************************************************************************************
 * Function     : EHCI Port Infomation Print
 * Declaration  : void usbh0_hstd_ehci_print_port_information( )
 * Argument     : none
 * Return       : void
 * Note         : This function is used for debugging.
***********************************************************************************************************************/
static void usbh0_hstd_ehci_print_port_information()
{
    USBH0_HCI_PRINTF0("-------------------------------\n");
    USBH0_HCI_PRINTF1("  EHCI Port No.%d Infomation\n", 0);
    USBH0_HCI_PRINTF0("-------------------------------\n");
//  USBH0_HCI_PRINTF1("WKOC_E                     : %d\n", USB00.PORTSC1.BIT.WKOC_E);
//  USBH0_HCI_PRINTF1("WKDSCNNT_E                 : %d\n", USB00.PORTSC1.BIT.WKDSCNNT_E);
//  USBH0_HCI_PRINTF1("WKCNNT_E                   : %d\n", USB00.PORTSC1.BIT.WKCNNT_E);
//  USBH0_HCI_PRINTF1("Port_Test_Control          : %d\n", USB00.PORTSC1.BIT.PortTestControl);
//  USBH0_HCI_PRINTF1("Port_IndicatorControl      : %d\n", USB00.PORTSC1.BIT.PortIndicatorControl);
    USBH0_HCI_PRINTF1("Port_Owner                 : %d\n", USB00.PORTSC1.BIT.PortOwner);
    USBH0_HCI_PRINTF1("PP                         : %d\n", USB00.PORTSC1.BIT.PP);
    USBH0_HCI_PRINTF1("Line_Status                : %d\n", USB00.PORTSC1.BIT.LineStatus);
    USBH0_HCI_PRINTF1("Port_Reset                 : %d\n", USB00.PORTSC1.BIT.PortReset);
    USBH0_HCI_PRINTF1("Suspend                    : %d\n", USB00.PORTSC1.BIT.Suspend);
    USBH0_HCI_PRINTF1("Force_Port_Resume          : %d\n", USB00.PORTSC1.BIT.ForcePortResume);
    USBH0_HCI_PRINTF1("Over_Current_Change        : %d\n", USB00.PORTSC1.BIT.OvercurrentChange);
    USBH0_HCI_PRINTF1("Over_Current_Active        : %d\n", USB00.PORTSC1.BIT.OvercurrentActive);
    USBH0_HCI_PRINTF1("Port_Enable_Disable_Change : %d\n", USB00.PORTSC1.BIT.PortEnable_DisableChange);
    USBH0_HCI_PRINTF1("Port_Enabled               : %d\n", USB00.PORTSC1.BIT.PortEnabled_Disabled);
    USBH0_HCI_PRINTF1("Connect_Status_Change      : %d\n", USB00.PORTSC1.BIT.ConnectStatusChange);
    USBH0_HCI_PRINTF1("Current_Connect_Status     : %d\n", USB00.PORTSC1.BIT.CurrentConnectStatus);
    USBH0_HCI_PRINTF0("\n" );
} /* End of function usbh0_hstd_ehci_print_port_information() */
#endif

#ifdef DEBUG_PRINT_ASYNC_SCHEDULING
/***********************************************************************************************************************
 * Function     : Asynchronization scheduling print
 * Declaration  : void usbh0_hstd_ehci_print_async_scheduling( st_usbh0_ehci_qh_t *h_qh )
 * Argument     : st_usbh0_ehci_qh_t *qh ; QH pointer
 * Return       : void
 * Note         : This function is used for debugging.
***********************************************************************************************************************/
static void usbh0_hstd_ehci_print_async_scheduling(st_usbh0_ehci_qh_t *p_h_qh)
{
    static st_usbh0_ehci_qh_t   *p_keep_h_qh = NULL;
    st_usbh0_ehci_qh_t          *p_qh;
    uint32_t                    cnt = 0;
    uint32_t                    tmp_addr;

    if (NULL != p_h_qh)
    {
        p_keep_h_qh = p_h_qh;
    }

    USBH0_HCI_PRINTF0("\n--------------------------------------- Async Scheduling Start \n");

    p_qh = p_keep_h_qh;
    if (NULL == p_qh)
    {
        return;
    }
    else
    {
        while (1)
        {
            USBH0_HCI_PRINTF1("0x%X", (uint32_t)p_qh);
            if (1 == p_qh->endpoint1.bit.h)
            {
                USBH0_HCI_PRINTF0(" H = 1");
                cnt++;
                if (2 == cnt)
                {
                    USBH0_HCI_PRINTF0("\n");
                    break;
                }
            }
            USBH0_HCI_PRINTF0("\n");
            tmp_addr = r_usbh0_pa_to_va((uint32_t)p_qh->queue_head_horizontal_link.address & 0xFFFFFFE0);
            p_qh = (st_usbh0_ehci_qh_t *)tmp_addr;
        }
    }

    USBH0_HCI_PRINTF0("----------------------------------------- Async Scheduling End \n\n");
} /* End of function usbh0_hstd_ehci_print_async_scheduling() */
#endif

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
/***********************************************************************************************************************
 * Function     : Print EHCI Periodec Scheduling List
 * Declaration  : void usbh0_hstd_ehci_print_periodec_scheduling_list( void )
 * Argument     : void
 * Return       : void
 * Note         : This function is used for debugging.
***********************************************************************************************************************/
static void usbh0_hstd_ehci_print_periodec_scheduling_list(void)
{
    uint32_t                        frame_list_size;
    uint32_t                        n;
    uint32_t                        periodic_list_addr;
    uint32_t                        tmp_addr;
    u_usbh0_ehci_flep_t             *p_listbase;
    volatile u_usbh0_ehci_flep_t    *p_current_element;

    /* Get frame list size */
    frame_list_size = usbh0_hstd_ehci_get_frame_list_size();


    periodic_list_addr = USB00.PERIODICLISTBASE.LONG & 0xFFFFF000;
    if (0x00000000 == periodic_list_addr)
    {
        USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_print_periodec_scheduling_list Error : PERIODICLISTBASE is not registered. \n");
        return;
    }
    else if (0x00000000 != periodic_list_addr)
    {
        periodic_list_addr = r_usbh0_pa_to_va(periodic_list_addr);
    }

    p_listbase = (u_usbh0_ehci_flep_t *)periodic_list_addr;
    for (n = 0; n < frame_list_size; n++)
    {
        p_current_element = &p_listbase[n];
        USBH0_HCI_PRINTF1("FLEP(%4d):", n);
        while (1)
        {
            if (NULL == p_current_element)
            {
                USBH0_HCI_PRINTF0_R("usbh0_hstd_ehci_print_periodec_scheduling_list Error: Current Element Pointer is NULL\n");
                return;
            }

            if (1 == p_current_element->bit.t)
            {
                break;
            }

            tmp_addr = r_usbh0_pa_to_va((uint32_t)p_current_element->Address & 0xFFFFFFE0);

            switch (p_current_element->bit.typ)
            {
                case USBH0_EHCI_TYP_ITD:
                    USBH0_HCI_PRINTF2(" (%d,%d)", ((st_usbh0_ehci_itd_t *)tmp_addr)->buffer[0].bit0.device_address, ((st_usbh0_ehci_itd_t *)tmp_addr)->info.pollrate);
                break;

                case USBH0_EHCI_TYP_QH:
                    USBH0_HCI_PRINTF2(" (%d,%d)", ((st_usbh0_ehci_qh_t *)tmp_addr)->endpoint1.bit.device_address, ((st_usbh0_ehci_qh_t *)tmp_addr)->info.pollrate);
                break;

                case USBH0_EHCI_TYP_SITD:
                    USBH0_HCI_PRINTF2(" (%d,%d)", ((st_usbh0_ehci_sitd_t *)tmp_addr)->endpoint1.bit.device_address, ((st_usbh0_ehci_sitd_t *)tmp_addr)->info.pollrate);
                break;

                default:
                break;
            }

                tmp_addr = r_usbh0_pa_to_va((uint32_t)p_current_element->address & 0xFFFFFFE0);
                p_qh = (u_usbh0_ehci_flep_t *)tmp_addr;
        }

        USBH0_HCI_PRINTF0("\n");
    }
} /* End of function usbh0_hstd_ehci_print_periodec_scheduling_list() */
#endif

#ifdef DEBUG_PRINT_PERIODIC_SCHEDULING
/***********************************************************************************************************************
 * Function     : EHCI Periodec Scheduling Score Print
 * Declaration  : void usbh0_hstd_ehci_print_periodic_scheduling_score( void )
 * Argument     : void
 * Return       : void
 * Note         : This function is used for debugging.
***********************************************************************************************************************/
static void usbh0_hstd_ehci_print_periodic_scheduling_score(void)
{
    uint32_t    uframe_list_size;
    uint32_t    n;

    /* Get micro frame list size */
    uframe_list_size  = usbh0_hstd_ehci_get_frame_list_size();
    uframe_list_size *= 8;

    for (n = 0; n < uframe_list_size; n++)
    {
        if (0 == (n % 8))
        {
            USBH0_HCI_PRINTF1("Score(%4d):", n / 8);
        }

        USBH0_HCI_PRINTF1(" %d", gs_usbh0_ehci_periodic_scheduling_score[n]);

        if (7 == (n % 8))
        {
            USBH0_HCI_PRINTF0("\n");
        }
    }
} /* End of function usbh0_hstd_ehci_print_periodic_scheduling_score() */
#endif

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
