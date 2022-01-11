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
* File Name    : r_usbh1_hOhciMain.c
* Description  : OHCI Main Code
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "r_usbh1_hhci_local.h"

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
static void     usbh1_hstd_ohci_hw_setup(void);
static uint32_t usbh1_hstd_ohci_hw_reset(void);
static void     usbh1_hstd_ohci_initailize_interrupt_lists(st_usbh1_ohci_hcd_device_data_p_t device_data);
static void     usbh1_hstd_ohci_clear_hcca(st_usbh1_ohci_hcd_device_data_p_t device_data);
static void     usbh1_hstd_ohci_clear_device_tr_req(uint32_t devadd);
static void     usbh1_hstd_ohci_rootport_connection(st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t devspeed, uint8_t portnum);
static void     usbh1_hstd_ohci_rootport_disconnection(st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t portnum);
static void     usbh1_hstd_ohci_remove_device(uint8_t device_address, boolean_t free_ed);
static uint8_t  usbh1_hstd_ohci_get_root_device_speed(st_usbh1_ohci_hcd_device_data_p_t device_data, uint8_t portnum);
static void     usbh1_hstd_ohci_process_paused_ed(st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed);
static void     usbh1_hstd_ohci_remove_ed(st_usbh1_ohci_hcd_endpoint_p_t endpoint, boolean_t free_ed);

void usbh1_disconnect_done_queue(uint16_t devadr);

#ifdef __ICCARM__
#pragma location="USB_HOST_1"

#endif  /* __ICCARM__ */

#ifdef __GNUC__
static st_usbh1_ohci_hcd_device_data_p_t   usbh1_hstd_ohci_device_data __attribute__ ((section ("USB_HOST_1")));
#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "USB_HOST_1"

#endif  /* __CC_ARM */

/***********************************************************************************************************************
 * Function     : Initialize OHCI
 * Declaration  : uint32_t usbh1_hstd_ohci_init( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_init(void)
{
    usbh1_hstd_ohci_init_device_data();

    usbh1_hstd_ohci_clear_list();

    usbh1_hstd_ohci_init_request_memory();

    usbh1_hstd_ohci_hw_setup();

    return USBH1_OK;
} /* End of function usbh1_hstd_ohci_init() */

/***********************************************************************************************************************
 * Function     : Deinitialize OHCI
 * Declaration  : void usbh1_hstd_ohci_deinit( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_deinit(void)
{
    usbh1_hstd_ohci_hw_reset();     /* H/W reset */
} /* End of function usbh1_hstd_ohci_deinit() */

/***********************************************************************************************************************
 * Function     : OHCI Detect interrupt
 * Declaration  : void usbh1_hstd_ohci_interrupt_handler( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_interrupt_handler(void)
{
    uint32_t                        context_info = 0;
    uint32_t                        port_reg     = 0;
    uint32_t                        port_num     = 0;
    uint32_t                        hc_int_enable;
    uint32_t                        hc_int_status;
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;
    uint32_t                        tmp_hcca_done_head;

    hc_int_status = USB10.HCINTERRUPTSTATUS.LONG;
    hc_int_enable = USB10.HCINTERRUPTENABLE.LONG;
    /* It is our interrupt, prevent HC from doing it to us again until we are finished */
    USB10.HCINTERRUPTDISABLE.LONG = USBH1_OHCI_IS_MASTERINTENABLE;

    USB10.HCINTERRUPTSTATUS.LONG = (~USBH1_OHCI_IS_WRITEBACKDONEHEAD & hc_int_status);

    port_reg = usbh1_hcrh_port_status1;
    port_num = usbh1_hcrh_descriptor_a;

    /* Is this our interrupt? */
    tmp_hcca = (st_usbh1_ohci_hcca_block_p_t)r_usbh1_pa_to_va((uint32_t)usbh1_hstd_ohci_device_data->hcca);
    tmp_hcca_done_head = r_usbh1_pa_to_va((uint32_t)tmp_hcca->hcca_done_head);
    if (0 != tmp_hcca_done_head)
    {
        context_info = USBH1_OHCI_IS_WRITEBACKDONEHEAD;   /* note interrupt processing required */

        if (tmp_hcca_done_head & 1)
        {
            context_info |= (hc_int_status & hc_int_enable);
        }
    }
    else
    {
       	context_info |= (hc_int_status & hc_int_enable);
        if(0 == context_info)
        {
            return;     /* not my interrupt */
        }
    }


    usbh1_hstd_hci_send_message_from_int(USBH1_HCI_TASK, USBH1_HCI_MESS_OHCI_INT, context_info, port_reg, port_num);
} /* End of function usbh1_hstd_ohci_interrupt_handler() */

/***********************************************************************************************************************
 * Function     : OHCI Main Routine
 * Declaration  : void usbh1_hstd_OhciMainRoutine( uint32_t context_info )
 * Argument     : uint32_t context_info ; Interrupt Context Infomation
 * Return       : void
 * Note         : This function is based on HcdInterruptService() of OHCI specification
***********************************************************************************************************************/
void usbh1_hstd_OhciMainRoutine(uint32_t context_info, uint32_t port_reg, uint32_t port_num)
{
    uint32_t                        temp;
    uint32_t                        temp2;
    uint32_t                        frame;
    uint32_t                        err;
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;
    uint32_t                        *tmp_hcca_done_head;

    if (context_info & USBH1_OHCI_IS_UNRECOVERABLERROR)
    {
        /* The controller is hung, maybe resetting it can get it going again */
        /* But that code is left as an exercise tothe reader */
    }

    if (context_info & (USBH1_OHCI_IS_SCHEDULINGOVERRUN | USBH1_OHCI_IS_WRITEBACKDONEHEAD | USBH1_OHCI_IS_STARTOFFRAME | USBH1_OHCI_IS_FRAMENUMBEROVERRUN))
    {
        context_info |= USBH1_OHCI_IS_MASTERINTENABLE;       /* flag for end of frame type interrupts */
    }

#if 0
    /* Check for Schedule Overrun */
    if (context_info & USBH1_OHCI_IS_SCHEDULINGOVERRUN)
    {
        frame = usbh1_hstd_ohci_get_32bit_frame_number(usbh1_hstd_ohci_device_data);
        temp2 = USB10.HCCOMMANDSTATUS & EFC_Mask;
        temp = temp2 - (usbh1_hstd_ohci_device_data->SOCount & EFC_Mask);
        temp = (((temp >> EFC_Position) - 1) % EFC_Size) + 1;

        /* number of bad frames since last error */
        if (!(usbh1_hstd_ohci_device_data->SOCount & SOC_Mask) || ((usbh1_hstd_ohci_device_data->SOCount & SOC_Mask) + usbh1_hstd_ohci_device_data->SOStallFrame + temp) != frame)   /* start a new count? */
        {
            usbh1_hstd_ohci_device_data->SOLimitFrame = usbh1_hstd_ohci_device_data->SOStallFrame = frame - temp;
            usbh1_hstd_ohci_device_data->SOCount = temp | temp2;
        }
        else    /* got a running count */
        {
            usbh1_hstd_ohci_device_data->SOCount = (usbh1_hstd_ohci_device_data->SOCount + temp) & SOC_Mask | temp2;
            while (frame - usbh1_hstd_ohci_device_data->SOLimitFrame >= 100)
            {
                usbh1_hstd_ohci_device_data->SOLimitHit++;
                usbh1_hstd_ohci_device_data->SOLimitFrame += 100;
            }
            if (frame - usbh1_hstd_ohci_device_data->SOStallFrame >= 32740)
            {
                USB10.HCCONTROL  &= ~USBH1_OHCI_ISOCHRONOUSENABLE;    /* stop isochronous transfers */
                usbh1_hstd_ohci_device_data->SOStallHit = TRUE;
                usbh1_hstd_ohci_device_data->SOCount        = temp2;                            /* clear error counter */
            }
        }

        USB10.HcInterruptStatus = USBH1_OHCI_IS_SCHEDULINGOVERRUN;    /* acknowledge interrupt */
        context_info &= ~USBH1_OHCI_IS_SCHEDULINGOVERRUN;
    }
    else    /* no schedule overrun, check for good frame */
    {
        if (context_info & USBH1_OHCI_IS_MASTERINTENABLE)
        {
            usbh1_hstd_ohci_device_data->SOCount &= EFC_MASK;       /* clear counter */
    }
    }
#endif

    /* Check for frame Number Overflow */
    /* Note: the formula below prevents a debugger break from making the 32-bit frame number run backward */
    if (context_info & USBH1_OHCI_IS_FRAMENUMBEROVERRUN)
    {
        tmp_hcca = r_usbh1_pa_to_va((uint32_t)usbh1_hstd_ohci_device_data->hcca);
        usbh1_hstd_ohci_device_data->frame_high_part += 0x10000 - ((tmp_hcca->hcca_frame_number ^ usbh1_hstd_ohci_device_data->frame_high_part) & 0x8000);
        /* USB10.HCINTERRUPTSTATUS.LONG  = USBH1_OHCI_IS_FRAMENUMBEROVERRUN; */  /* acknowledge interrupt */
        context_info &= ~USBH1_OHCI_IS_FRAMENUMBEROVERRUN;
    }

    /* Processor interrupts could be enabled here and the interrupt dismissed at the interrupt */
    /* controller, but for simplicity this code won't */
    if (context_info & USBH1_OHCI_IS_RESUMEDETECTED)
    {
        // Resume has been requested by a device on USB. HCD must wait 20ms then put controller */
        // in the UsbOperational state. This code is left as an exercise to the reader */
        context_info &= ~USBH1_OHCI_IS_RESUMEDETECTED;
        /* USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_IS_RESUMEDETECTED; */
    }

    /* Process the Done Queue */
    if (context_info & USBH1_OHCI_IS_WRITEBACKDONEHEAD)
    {
    	tmp_hcca = (st_usbh1_ohci_hcca_block_p_t)r_usbh1_pa_to_va((uint32_t)usbh1_hstd_ohci_device_data->hcca);
        tmp_hcca_done_head = r_usbh1_pa_to_va((uint32_t)tmp_hcca->hcca_done_head);

        usbh1_hstd_ohci_process_done_queue(tmp_hcca_done_head);

        /* Done Queue processing complete, notify controller */
        tmp_hcca->hcca_done_head = 0;
        USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_IS_WRITEBACKDONEHEAD;
        context_info &= ~USBH1_OHCI_IS_WRITEBACKDONEHEAD;
    }

    /* Process Root Hub changes */
    if (context_info & USBH1_OHCI_IS_ROOTHUBSTATUSCHANGE)
    {
        /* EmulateRootHubInterruptXfer will complete a USBH1_OHCI_HCD_TRANSFER_DESCRIPTOR which */
        /* we then pass to usbh1_hstd_ohci_process_done_queue to emulate an HC completion */

        /* leave RootHubStatusChange set in context_info so that it will be masked off */
        /* (it won't be unmaskedagain until another TD is queued for the emulated endpoint) */
        usbh1_hstd_ohci_control_roothub(port_reg, port_num);
        /* USB10.HCINTERRUPTSTATUS.LONG = 0x00000040; */    /* Clear RHSC bit */
        context_info &= ~USBH1_OHCI_IS_ROOTHUBSTATUSCHANGE;
    }

    if (context_info & USBH1_OHCI_IS_OWNERSHIPCHANGE)
    {
        /* Only SMM drivers need implement this */
        /* See Sections 5.1.1.3.3 and 5.1.1.3.6 for descriptions of what the code here must do */
    }

    /* Any interrupts left should just be masked out */
    /* (This is normal processing for StartOfFrame and RootHubStatusChange) */
    if (context_info & ~USBH1_OHCI_IS_MASTERINTENABLE)    /* any unprocessed interrupts? */
    {
        USB10.HCINTERRUPTDISABLE.LONG = context_info;     /* yes, mask them */
    }

    /* We've completed the actual service of the HC interrupts, now we must deal with the effects */
#if 0
    /* Check for Scheduling Overrun limit */
    if (usbh1_hstd_ohci_device_data->SOLimitHit)
    {
        do
        {
            st_usbh1_ohci_hcd_endpoint_descriptor_p_t ED;
            if (usbh1_hstd_ohci_is_list_empty(&usbh1_hstd_ohci_device_data->ed_list[USBH1_OHCI_ED_ISOCHRONOUS].head) == TRUE)
            {
                break;  /* Isochronous List is empty */
            }

            ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                    usbh1_hstd_ohci_device_data->ed_list[USBH1_OHCI_ED_ISOCHRONOUS].head.b_link,
                    USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR,USBH1_OHCI_CR_LINK);

            if (USBH1_EP_ISO != ED->Endpoint->Type)
            {
                break;  /* Only 1ms Interrupts left on list */
            }
            usbh1_hstd_ohci_device_data->available_bandwidth = usbh1_hstd_ohci_device_data->max_bandwidth_inuse - 64;

            /* It is recommended that the above bandwidth be saved in non-volatile memory for future use */
            usbh1_hstd_ohci_remove_ed(ED->Endpoint);
        } while (--usbh1_hstd_ohci_device_data->SOLimitHit);

        usbh1_hstd_ohci_device_data->SOLimitHit = 0;
    }
#endif

    /* look for things on the PausedEDRestart list */
    frame = usbh1_hstd_ohci_get_32bit_frame_number(usbh1_hstd_ohci_device_data);
    while (FALSE == usbh1_hstd_ohci_is_list_empty(&usbh1_hstd_ohci_device_data->paused_ed_restart))
    {
        volatile st_usbh1_ohci_hcd_endpoint_descriptor_p_t ED;
/*
        ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                usbh1_hstd_ohci_device_data->paused_ed_restart.f_link, USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_PAUSEDLINK);
*/
        ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                r_usbh1_pa_to_va(usbh1_hstd_ohci_device_data->paused_ed_restart.f_link), USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_PAUSEDLINK);

        if ((int32_t)(ED->reclamation_frame - frame) > 0)
        {
            break;
        }

        if(NULL == ED)
        {
            usbh1_hstd_ohci_remove_list_entry(&usbh1_hstd_ohci_device_data->paused_ed_restart);
            break;
        }

        usbh1_hstd_ohci_remove_list_entry(&ED->paused_link);
        ED->paused_flag = FALSE;
        usbh1_hstd_ohci_process_paused_ed(ED);
    }

    /* look for things on the StalledEDReclamation list */
    err = usbh1_hstd_ohci_is_list_empty(&usbh1_hstd_ohci_device_data->stalled_ed_reclamation);
    if((context_info & USBH1_OHCI_IS_MASTERINTENABLE) && (FALSE == err))
    {
        temp = USB10.HCCONTROLCURRENTED.LONG;
        temp2 = USB10.HCBULKCURRENTED.LONG;
        while (FALSE == usbh1_hstd_ohci_is_list_empty(&usbh1_hstd_ohci_device_data->stalled_ed_reclamation))
        {
             volatile st_usbh1_ohci_hcd_endpoint_descriptor_p_t ED;
/*
            ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                    usbh1_hstd_ohci_device_data->stalled_ed_reclamation.f_link, USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);
*/
            ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
            		r_usbh1_pa_to_va(usbh1_hstd_ohci_device_data->stalled_ed_reclamation.f_link), USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);

            if (NULL == ED)
            {
                usbh1_hstd_ohci_remove_list_entry(&usbh1_hstd_ohci_device_data->stalled_ed_reclamation);
                break;
            }

            usbh1_hstd_ohci_remove_list_entry(&ED->link);
            if (ED->physical_address == temp)
            {
                USB10.HCCONTROLCURRENTED.LONG = temp = usbh1_hstd_ohci_physical_address_of(ED->hc_ed.next_ed);
            }
            else if (ED->physical_address == temp2)
            {
                USB10.HCBULKCURRENTED.LONG = temp2 = usbh1_hstd_ohci_physical_address_of(ED->hc_ed.next_ed);
            }

            if (NULL != ED->endpoint)
            {
                usbh1_hstd_ohci_process_paused_ed(ED);          /* cancel any outstanding TDs */
                usbh1_hstd_ohci_free_endpoint_descriptor(ED);
            }
            else
            {
                usbh1_hstd_ohci_free_endpoint_descriptor(ED);
            }
        }

        USB10.HCCONTROL.LONG |= (USBH1_OHCI_CONTROLLISTENABLE | USBH1_OHCI_BULKLISTENABLE | USBH1_OHCI_ISOCHRONOUSENABLE | USBH1_OHCI_PERIODICLISTENABLE);   /* restart queues */
    }

    /* look for things on the RunningEDReclamation list */
    frame = usbh1_hstd_ohci_get_32bit_frame_number(usbh1_hstd_ohci_device_data);
    while (FALSE == usbh1_hstd_ohci_is_list_empty(&usbh1_hstd_ohci_device_data->running_ed_reclamation))
    {
        volatile st_usbh1_ohci_hcd_endpoint_descriptor_p_t ED;
/*
        ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                usbh1_hstd_ohci_device_data->running_ed_reclamation.f_link, USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);
*/
        ED = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
        		r_usbh1_pa_to_va(usbh1_hstd_ohci_device_data->running_ed_reclamation.f_link), USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);

        if ((int32_t)(ED->reclamation_frame - frame) > 0)
        {
            break;
        }

        if (NULL == ED)
        {
            usbh1_hstd_ohci_remove_list_entry( &usbh1_hstd_ohci_device_data->running_ed_reclamation );
            break;
        }

        usbh1_hstd_ohci_remove_list_entry(&ED->link);
        if (NULL != ED->endpoint)
        {
            usbh1_hstd_ohci_process_paused_ed(ED);      /* cancel any outstanding TDs */
            usbh1_hstd_ohci_free_endpoint_descriptor(ED);
        }
        else
        {
            usbh1_hstd_ohci_free_endpoint_descriptor(ED);
        }
    }

    /* If processor interrupts were enabled earlier then they must be disabled here before we re-enable the interrupts at the controller */
    USB10.HCINTERRUPTENABLE.LONG = USBH1_OHCI_IS_MASTERINTENABLE;

    return;
} /* End of function usbh1_hstd_OhciMainRoutine() */

/***********************************************************************************************************************
 * Function     : OHCI Control RootHub
 * Declaration  : void usbh1_hstd_ohci_control_roothub( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_control_roothub(uint32_t port_reg, uint32_t port_num)
{
    uint32_t    portnum;
    uint32_t    numports;
    uint32_t    * volatile p_port_status_reg;
    uint8_t     dev_speed;
    uint8_t     i;
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;
    uint32_t                        *tmp_hcca_done_head;

  /* r_usbh1_hstd_hci_wait_time(5); */


    numports = port_num & 0x000000FF;

    for (portnum = 0; portnum < numports; portnum++)
    {
        /*  Detect DisConnection */
        if ((port_reg & 0x00020002) == 0x00020000)
        {
            r_usbh1_hstd_hci_wait_time(32);
        	tmp_hcca = (st_usbh1_ohci_hcca_block_p_t)r_usbh1_pa_to_va((uint32_t)usbh1_hstd_ohci_device_data->hcca);
        	tmp_hcca_done_head = r_usbh1_pa_to_va((uint32_t)tmp_hcca->hcca_done_head);
        	if(0 != tmp_hcca_done_head)
        	{
        		usbh1_hstd_ohci_process_done_queue(tmp_hcca_done_head);
				/* Done Queue processing complete, notify controller */
				tmp_hcca->hcca_done_head = 0;
				USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_IS_WRITEBACKDONEHEAD;
        	}
        	/* r_usbh1_hstd_hci_wait_time(1); */
            usbh1_hstd_ohci_rootport_disconnection(usbh1_hstd_ohci_device_data, portnum);
            r_usbh1_hstd_hci_wait_time(1);

            port_reg = 0x00030000;      /* Clear PESC & CSC bit */
        }
        else
        {
            p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
            p_port_status_reg += portnum;

        	if ((*p_port_status_reg & 0x00010001) == 0x00010001)
        	{
				/*  Detect Connection */
				*p_port_status_reg = 0x00010000;      /* Clear CSC bit */

				usbh1_hstd_ohci_port_reset(portnum);

				if ((*p_port_status_reg & 0x00000001) == 0x00000000)
				{
					return;
				}

				/* Report USBD-Layer */
				dev_speed = usbh1_hstd_ohci_get_root_device_speed(usbh1_hstd_ohci_device_data, portnum);
				usbh1_hstd_ohci_rootport_connection(usbh1_hstd_ohci_device_data, dev_speed, portnum);
        	}

            if ((*p_port_status_reg & 0x00080000) == 0x00080000)
            {
                /*  Detect OverCurrent */
                *p_port_status_reg = 0x00080000;      /* Clear OCIC bit */

                r_usbh1_hstd_hci_wait_time(10);          /* Wait 10ms */

                /* over current callback */
                g_usbh1_hstd_hci_callback.over_current_cb(portnum);
            }
        }
    }
} /* End of function usbh1_hstd_ohci_control_roothub() */

/***********************************************************************************************************************
 * Function     : OHCI Device Disconnect
 * Declaration  : void usbh1_hstd_ohci_disconnect( uint32_t devadd )
 * Argument     : uint32_t devadd ; device address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_disconnect(uint32_t devadd)
{
//    uint8_t     free_ed = FALSE;
    uint8_t     free_ed = TRUE;

    if (USBH1_HCI_NODEVICE == devadd)
    {
        return;
    }

    usbh1_hstd_ohci_clear_device_tr_req(devadd);

    usbh1_hstd_ohci_remove_device(devadd, free_ed);
} /* End of function usbh1_hstd_ohci_disconnect() */

/***********************************************************************************************************************
 * Function     : OHCI Getting of device address of rootpoot
 * Declaration  : uint16_t usbh1_hstd_ohci_get_device_address_of_rootpoot( uint16_t rootport )
 * Argument     : uint16_t rootport ; rootport no
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t usbh1_hstd_ohci_get_device_address_of_rootpoot(uint16_t rootport)
{
    if (USBH1_OHCI_MAXROOTPORTS > rootport)
    {
        return usbh1_hstd_ohci_device_data->root_device[rootport];
    }

    return USBH1_HCI_NODEVICE;
} /* End of function usbh1_hstd_ohci_get_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : The device address of OHCI route port is set.
 * Declaration  : uint16_t usbh1_hstd_ohci_set_device_address_of_rootpoot( uint16_t rootport, uint16_t devaddr )
 * Argument     : uint16_t rootport ; rootport no
 *              : uint16_t devaddr  ; device address
 * Return       : uint16_t          ; device address
 * Note         :
***********************************************************************************************************************/
uint16_t usbh1_hstd_ohci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t deviceaddr)
{
    st_usbh1_hci_device_t   *p_devinfo;

    if (USBH1_OHCI_MAXROOTPORTS <= rootport)
    {
        return USBH1_HCI_NODEVICE;
    }

    if (0 != usbh1_hstd_ohci_device_data->root_device[rootport])
    {
//        USBH1_HCI_PRINTF1_R("Error: The device address has already been allocated to the route port %d.\n", rootport);
        return USBH1_HCI_NODEVICE;
    }

    /* The device address of device information is updated */
    p_devinfo = r_usbh1_hstd_hci_get_device_infomation(usbh1_hstd_ohci_device_data->root_device[rootport]);
    if (NULL == p_devinfo)
    {
        return USBH1_HCI_NODEVICE;
    }

    p_devinfo->bit.devnum = deviceaddr;

    usbh1_hstd_ohci_device_data->root_device[rootport] = deviceaddr;

    return deviceaddr;
} /* End of function usbh1_hstd_ohci_set_device_address_of_rootpoot() */

/***********************************************************************************************************************
 * Function     : OHCI Port Reset
 * Declaration  : void usbh1_hstd_ohci_port_reset( uint32_t portnum )
 * Argument     : uint32_t portnum ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_port_reset(uint32_t portnum)
{
    uint32_t    * volatile p_port_status_reg;

    p_port_status_reg = (uint32_t *)&( USB10.HCRHPORTSTATUS1.LONG );
    p_port_status_reg += portnum;

    /* Wait of ECHI ehough */
//    r_usbh1_hstd_hci_wait_time(100);     /* Wait 100ms */

    /* Port* Reset */
    *p_port_status_reg = 0x00000010;
    while ((*p_port_status_reg & 0x00100000) == 0x00000000)    /* Wait Port*-Enable */
    {
        if ((*p_port_status_reg & 0x00000001) == 0x00000000)   /* if device is dis-connect */
        {
            break;
        }

        r_usbh1_hstd_hci_wait_time(1);
    }

    *p_port_status_reg = 0x00100000;  /* Clear PRSC bit */

    r_usbh1_hstd_hci_wait_time(10);      /* Wait 10ms */
} /* End of function usbh1_hstd_ohci_port_reset() */

/***********************************************************************************************************************
 * Function     : Getting of OHCI PID Status
 * Declaration  : uint16_t usbh1_hstd_ohci_get_pid_status( st_usbh1_hci_tr_req_t *tr_req )
 * Argument     : st_usbh1_hci_tr_req_t *tr_req ; transfer request pointer
 * Return       : uint16_t               ; PID Status
 * Note         :
***********************************************************************************************************************/
uint16_t usbh1_hstd_ohci_get_pid_status(st_usbh1_hci_tr_req_t *p_tr_req)
{
    st_usbh1_ohci_request_p_t   ohci_req;
    uint32_t                    status;

    if (NULL != p_tr_req)
    {
        if (TRUE == p_tr_req->bit.enable)
        {
            ohci_req = (st_usbh1_ohci_request_p_t)r_usbh1_pa_to_va((uint32_t)p_tr_req->hci_info);
            status = ohci_req->status;
            if (USBH1_OHCI_CC_STALL == status)
            {
                return USBH1_PID_STALL;
            }

            if (USBH1_OHCI_CC_NOERROR == status)
            {
                return USBH1_PID_BUF;
            }
        }
    }

    return USBH1_PID_NAK;
} /* End of function usbh1_hstd_ohci_get_pid_status() */

/***********************************************************************************************************************
 * Function     : OHCI Port Suspend
 * Declaration  : void usbh1_hstd_ohci_port_suspend( uint16_t port )
 * Argument     : uint16_t port ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_port_suspend(uint16_t port)
{
    uint32_t    *p_port_status_reg;

    p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
    p_port_status_reg += port;

    *p_port_status_reg = 0x00000004;    /* Port* Suspend */
} /* End of function usbh1_hstd_ohci_port_suspend() */

/***********************************************************************************************************************
 * Function     : OHCI Port Resume
 * Declaration  : void usbh1_hstd_ohci_port_resume( uint16_t port )
 * Argument     : uint16_t port ; port no
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_port_resume(uint16_t port)
{
    uint32_t    *p_port_status_reg;

    p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
    p_port_status_reg += port;

    /* Port* Resume */
    *p_port_status_reg = 0x00000008;
    while ((*p_port_status_reg & 0x00040000) == 0x00000000)     /* Wait Port*-Resume wait */
    {
        if ((*p_port_status_reg & 0x00000001) == 0x00000000)    /* if device is dis-connect */
        {
            break;
        }

        r_usbh1_hstd_hci_wait_time(1);
    }

    *p_port_status_reg = 0x00040000;      /* Clear PSSC bit */
} /* End of function usbh1_hstd_ohci_port_resume() */

/***********************************************************************************************************************
 * Function     : Getting of OHCI Transfer End Status
 * Declaration  : uint16_t usbh1_hstd_ohci_get_transfer_end_status( st_usbh1_hci_tr_req_t *tr_req )
 * Argument     : st_usbh1_hci_tr_req_t *tr_req ; HCI transfer request pointer
 * Return       : uint16_t               ; USB Standard F/W Transfer End Status
 * Note         :
***********************************************************************************************************************/
uint16_t usbh1_hstd_ohci_get_transfer_end_status(st_usbh1_hci_tr_req_t *p_tr_req)
{
    uint32_t    status;
    uint16_t    ret = USBH1_DATA_ERR;

    status = p_tr_req->status;

    if (USBH1_OHCI_CC_NOERROR == status)
    {
        if (USBH1_EP_CNTRL == p_tr_req->bit.eptype)
        {
            /* For control */
            ret = USBH1_CTRL_END;
        }
        else if (USBH1_EP_ISO == p_tr_req->bit.eptype)
        {
            /* For isochronous */
            if (USBH1_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
            {
                ret = USBH1_DATA_WRITING;
            }
            else
            {
                ret = USBH1_DATA_READING;
            }
        }
        else
        {
            /* For bulk/interrupt */
            if (USBH1_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
            {
                /* When the data sending ends normally */
                ret = USBH1_DATA_NONE;
            }
            else
            {
                if (p_tr_req->actual_size > p_tr_req->trsize)
                {
                    /* When receive data is size over */
                    ret = USBH1_DATA_OVR;
                }
                else if (p_tr_req->actual_size < p_tr_req->trsize)
                {
                    /* When receive data is short size */
                    ret = USBH1_DATA_SHT;
                }
                else
                {
                    /* When the data reception ends normally */
                    ret = USBH1_DATA_OK;
                }
            }
        }
    }
    else
    {
        if (USBH1_EP_ISO == p_tr_req->bit.eptype)
        {
            if (USBH1_OHCI_CC_DATAOVERRUN == status)
            {
                /* For isochronous */
                if (USBH1_HCI_DIRECTION_OUT == p_tr_req->bit.direction)
                {
                    ret = USBH1_DATA_WRITING;
                }
                else
                {
                    /* When receive data is size over */
                    ret = USBH1_DATA_OVR;
                }
            }
        }
        else
        {
            if (USBH1_OHCI_CC_DATAUNDERRUN == status)
            {
                ret = USBH1_DATA_OK;
            }
            else if (USBH1_OHCI_CC_STALL == status)
            {
                /* For STALL */
                ret = USBH1_DATA_STALL;
            }
            else if (USBH1_OHCI_CC_DEVICENOTRESPOND == status)
            {
                /* Transfer stop by Detach */
                ret = USBH1_DATA_STOP;
            }
            else if (USBH1_OHCI_RS_CANCELED == status)
            {
                /* Transfer stop by Cancel */
                ret = USBH1_DATA_STOP;
            }
        }
    }

    return ret;
} /* End of function usbh1_hstd_ohci_get_transfer_end_status() */

/***********************************************************************************************************************
 * Function     : OHCI Remove Endpoint/ED
 * Declaration  : uint32_t usbh1_hstd_ohci_remove_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction, boolean_t free_ed )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 *              : boolean_t free_ed       ; free flag
 * Return       : uint32_t                : result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_remove_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction, boolean_t free_ed)
{
    st_usbh1_ohci_hcd_endpoint_p_t  endpoint;
    uint32_t                        ret = FALSE;

    endpoint = usbh1_hstd_ohci_scan_endpoint(device_address, endpoint_number, direction);
    if (NULL != endpoint)
    {
        usbh1_hstd_ohci_remove_ed(endpoint, free_ed);
        if (TRUE == free_ed)
        {
            usbh1_hstd_ohci_free_endpoint(endpoint);

        }

        usbh1_hstd_ohci_store_endpoint(device_address, endpoint_number, direction, NULL);

        ret = TRUE;
    }

    return ret;
} /* End of function usbh1_hstd_ohci_remove_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Pause ED
 * Declaration  : uint32_t usbh1_hstd_ohci_pause_endpoint( uint8_t device_address, uint8_t endpoint_number, uint8_t direction )
 * Argument     : uint8_t device_address  ; device address
 *              : uint8_t endpoint_number ; endpoint no
 *              : uint8_t direction       ; direction
 * Return       : uint32_t                : result( TRUE:Nomal End, FALSE:Fail )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_pause_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction)
{
    st_usbh1_ohci_hcd_endpoint_p_t  endpoint;
    uint32_t                        ret = FALSE;

    endpoint = usbh1_hstd_ohci_scan_endpoint(device_address, endpoint_number, direction);
    if (NULL != endpoint)
    {
        usbh1_hstd_ohci_pause_ed(endpoint);
        ret = TRUE;
    }

    return ret;
} /* End of function usbh1_hstd_ohci_pause_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Remove ListHead
 * Declaration  : st_usbh1_ohci_list_entry_p_t usbh1_hstd_ohci_remove_list_head( st_usbh1_ohci_list_entry_p_t list )
 * Argument     : st_usbh1_ohci_list_entry_p_t list ; remove list head pointer
 * Return       : st_usbh1_ohci_list_entry_p_t      ; next link list pointer
 * Note         :
***********************************************************************************************************************/
st_usbh1_ohci_list_entry_p_t usbh1_hstd_ohci_remove_list_head( st_usbh1_ohci_list_entry_p_t list )
{
    st_usbh1_ohci_list_entry_t      *p_ret_list;
    st_usbh1_ohci_list_entry_p_t    tmp_f_link;
    st_usbh1_ohci_list_entry_p_t    tmp_b_link;

    p_ret_list = r_usbh1_pa_to_va((uint32_t)list->f_link);

    tmp_f_link = r_usbh1_pa_to_va((uint32_t)list->f_link);
    tmp_b_link = r_usbh1_pa_to_va((uint32_t)list->b_link);

    if (tmp_f_link == tmp_b_link)
    {
    	tmp_f_link = tmp_b_link = NULL;
        list->f_link = list->b_link = NULL;
    }
    else
    {
        list->f_link = usbh1_hstd_ohci_physical_address_of(tmp_f_link->f_link);
    }

    return p_ret_list;
} /* End of function usbh1_hstd_ohci_remove_list_head() */

/***********************************************************************************************************************
 * Function     : OHCI Remove ListEntry
 * Declaration  : void usbh1_hstd_ohci_remove_list_entry( st_usbh1_ohci_list_entry_p_t list )
 * Argument     : st_usbh1_ohci_list_entry_p_t list ; remove list pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_remove_list_entry(st_usbh1_ohci_list_entry_p_t list)
{
    st_usbh1_ohci_list_entry_t      *p_list_head;
    st_usbh1_ohci_list_entry_p_t    tmp_f_link;
    st_usbh1_ohci_list_entry_p_t    tmp_b_link;

    tmp_f_link = r_usbh1_pa_to_va((uint32_t)list->f_link);
    tmp_b_link = r_usbh1_pa_to_va((uint32_t)list->b_link);

    if ((NULL == tmp_b_link) && (NULL == tmp_f_link))
    {
        return;
    }

    if (tmp_b_link == tmp_f_link)
    {
        p_list_head = r_usbh1_pa_to_va((uint32_t)list->b_link);
        p_list_head->b_link = p_list_head->f_link = NULL;
    }
    else
    {
        tmp_b_link->f_link = usbh1_hstd_ohci_physical_address_of(list->f_link);
        tmp_f_link->b_link = usbh1_hstd_ohci_physical_address_of(list->b_link);
    }

    tmp_b_link = tmp_f_link = NULL;
    list->f_link = list->b_link = NULL;
} /* End of function usbh1_hstd_ohci_remove_list_entry() */

/***********************************************************************************************************************
 * Function     : OHCI Initialize ListHead
 * Declaration  : void usbh1_hstd_ohci_initialize_list_head( st_usbh1_ohci_list_entry_p_t list )
 * Argument     : st_usbh1_ohci_list_entry_p_t list ; list pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_initialize_list_head(st_usbh1_ohci_list_entry_p_t list)
{
    list->f_link = NULL;
    list->b_link = NULL;
} /* End of function usbh1_hstd_ohci_initialize_list_head() */

/***********************************************************************************************************************
 * Function     : Judge List is empty or not
 * Declaration  : uint32_t usbh1_hstd_ohci_is_list_empty( st_usbh1_ohci_list_entry_p_t list )
 * Argument     : st_usbh1_ohci_list_entry_p_t list ; List pointer
 * Return       : uint32_t                   ; result( TRUE:Empty, FALSE:Not Empty )
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_is_list_empty(st_usbh1_ohci_list_entry_p_t list)
{
    if (NULL == r_usbh1_pa_to_va((uint32_t)list->f_link))
    {
        return TRUE;
    }

    return FALSE;
} /* End of function usbh1_hstd_ohci_is_list_empty() */

/***********************************************************************************************************************
 * Function     : OHCI Insert list to the head of list
 * Declaration  : void usbh1_hstd_ohci_insert_head_list( st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link )
 * Argument     : st_usbh1_ohci_list_entry_p_t list_head ; List Head pointer
 *              : st_usbh1_ohci_list_entry_p_t link      ; insert list pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_insert_head_list(st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link)
{
    st_usbh1_ohci_list_entry_p_t    tmp_f_link;

    tmp_f_link = r_usbh1_pa_to_va((uint32_t)list_head->f_link);
    if (NULL == tmp_f_link)
    {
        /* list_head is Empty */
        list_head->f_link = list_head->b_link = usbh1_hstd_ohci_physical_address_of(link);
        link->f_link = link->b_link = usbh1_hstd_ohci_physical_address_of(list_head);
    }
    else
    {
        tmp_f_link->b_link  = usbh1_hstd_ohci_physical_address_of(link);
        link->f_link        = usbh1_hstd_ohci_physical_address_of(list_head->f_link);
        list_head->f_link   = usbh1_hstd_ohci_physical_address_of(link);
        link->b_link        = usbh1_hstd_ohci_physical_address_of(list_head);
    }
} /* End of function usbh1_hstd_ohci_insert_head_list() */

/***********************************************************************************************************************
 * Function     : OHCI Insert list to tail of list
 * Declaration  : void usbh1_hstd_ohci_insert_tail_list( st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link )
 * Argument     : st_usbh1_ohci_list_entry_p_t list_head ; List Head pointer
 *              : st_usbh1_ohci_list_entry_p_t link      ; insert list pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_insert_tail_list(st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link)
{
    st_usbh1_ohci_list_entry_p_t    tmp_b_link;

    tmp_b_link = r_usbh1_pa_to_va((uint32_t)list_head->b_link);
    if (NULL == tmp_b_link)
    {
        /* list_head is Empty */
        list_head->b_link = list_head->f_link = usbh1_hstd_ohci_physical_address_of(link);
        link->f_link = link->b_link = usbh1_hstd_ohci_physical_address_of(list_head);
    }
    else
    {
        tmp_b_link->f_link  = usbh1_hstd_ohci_physical_address_of(link);
        link->b_link        = usbh1_hstd_ohci_physical_address_of(list_head->b_link);
        list_head->b_link   = usbh1_hstd_ohci_physical_address_of(link);
        link->f_link        = usbh1_hstd_ohci_physical_address_of(list_head);
    }
} /* End of function usbh1_hstd_ohci_insert_tail_list() */

/***********************************************************************************************************************
 * Function     : Return PhysicalAddress
 * Declaration  : uint32_t usbh1_hstd_ohci_physical_address_of( void *Data )
 * Argument     : void *Data ; Data Address
 * Return       : uint32_t   ; Physical Address
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_physical_address_of(void *p_data)
{
#if USBH1_OHCI_SHAREDMEMORYTYPE == USBH1_OHCI_SHAREDMEMORYTYPE_EXCLUSIVE
    return ((uint32_t)p_data - USBH1_OHCI_SHAREDMEMORY_OFFSET);
#elif USBH1_OHCI_SHAREDMEMORYTYPE == USBH1_OHCI_SHAREDMEMORYTYPE_PCI
    uint32_t retadrr;

    R_MMU_VAtoPA((uint32_t)p_data,&retadrr);

    return retadrr;
#else
    return (uint32_t)p_data;
#endif
} /* End of function usbh1_hstd_ohci_physical_address_of() */

/***********************************************************************************************************************
 * Function     : OHCI Pause Endpoint
 * Declaration  : void usbh1_hstd_ohci_pause_ed( st_usbh1_ohci_hcd_endpoint_p_t endpoint )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_pause_ed(st_usbh1_ohci_hcd_endpoint_p_t endpoint)
{
    st_usbh1_ohci_hcd_device_data_p_t           device_data;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;

    device_data = r_usbh1_pa_to_va(endpoint->device_data);
    ed = r_usbh1_pa_to_va(endpoint->hcd_ed);
    ed->hc_ed.control.k = TRUE;

    if (ed->paused_flag)
    {
        /* already awaiting pause processing */
        return;
    }
    if (0 == (ed->hc_ed.head_p & USBH1_OHCI_HCED_HEADP_HALT))
    {
        /* Endpoint is active in Host Controller, wait for SOF before processing the endpoint */
        ed->paused_flag = TRUE;
        USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_HC_INT_SOF;  /* clear SOF interrupt pending */
        ed->reclamation_frame = usbh1_hstd_ohci_get_32bit_frame_number(device_data) + 1;
        usbh1_hstd_ohci_insert_tail_list(&device_data->paused_ed_restart, &ed->paused_link);

        USB10.HCINTERRUPTENABLE.LONG = USBH1_OHCI_HC_INT_SOF; /* interrupt on next SOF */
        return;
    }

    /* Endpoint already paused, do processing now */
    usbh1_hstd_ohci_process_paused_ed(ed);
} /* End of function usbh1_hstd_ohci_pause_ed() */

/***********************************************************************************************************************
 * Function     : OHCI Unschedule Iso,Int Endpoint
 * Declaration  : void usbh1_hstd_ohci_unschedule_isochronous_or_interrupt_endpoint( st_usbh1_ohci_hcd_endpoint_p_t endpoint, boolean_t free_ed, boolean_t endpoint_processing_required )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint       ; Endpoint Pointer
 *              : boolean_t free_ed                      ; endpoint free flag
 *              : boolean_t endpoint_processing_required ; endpoint processing required flag
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_hstd_ohci_unschedule_isochronous_or_interrupt_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint,
                                                                boolean_t free_ed, boolean_t endpoint_processing_required)
{
//  st_usbh1_ohci_hcd_device_data_p_t  device_data;

//  device_data = endpoint->device_data;

    usbh1_hstd_ohci_remove_ed(endpoint, free_ed);      /* see sample code in Section 5.2.7.1.2 */
    if (TRUE == endpoint_processing_required)
    {
        USB10.HCINTERRUPTENABLE.LONG = USBH1_OHCI_HC_INT_SOF;     /* interrupt on next SOF */
    }
} /* End of function usbh1_hstd_ohci_unschedule_isochronous_or_interrupt_endpoint() */

/***********************************************************************************************************************
 * Function     : OHCI Set FrameInterval
 * Declaration  : uint32_t usbh1_hstd_ohci_set_frame_interval( st_usbh1_ohci_hcd_device_data_p_t device_data, boolean_t up_not_down )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data Pointer
 *              ; boolean_t up_not_down                  ; up or down flag( TRUE:up, FALSE:down )
 * Return       : uint32_t                               ; frame number
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_set_frame_interval(st_usbh1_ohci_hcd_device_data_p_t device_data, boolean_t up_not_down)
{
    uint32_t    frame_number;
    uint32_t    interval;

//    interval |= (device_data->HC->HCFMINTERVAL & 0x00003FFF);
//    interval = (device_data->HC->HCFMINTERVAL & 0x00003FFF);
    interval = (USB10.HCFMINTERVAL.LONG & 0x00003FFF);
    if (TRUE == up_not_down)
    {
        ++interval;
    }
    else
    {
        --interval;
    }

    interval |= (((interval - USBH1_OHCI_MAXIMUM_OVERHEAD) * 6) / 7) << 16;
//  interval |= 0x80000000 & (0x80000000 ^ (device_data->HC->HcFrameRemaining));
    interval |= 0x80000000 & (0x80000000 ^ (USB10.HCFMREMAINING.LONG));
    frame_number = usbh1_hstd_ohci_get_32bit_frame_number(device_data);
//  device_data->HC->HCFMINTERVAL = interval;
    USB10.HCFMINTERVAL.LONG = interval;
//    if (0x80000000 & (device_data->HC->HcFrameRemaining ^ interval))
    if (0x80000000 & (USB10.HCFMREMAINING.LONG ^ interval))
    {
        frame_number += 1;
    }
    else
    {
        frame_number = usbh1_hstd_ohci_get_32bit_frame_number(device_data);
    }
    return frame_number;      /* return frame number new interval takes effect */
} /* End of function usbh1_hstd_ohci_set_frame_interval() */

/***********************************************************************************************************************
 * Function     : OHCI Get 32bit FrameNumber
 * Declaration  : uint32_t usbh1_hstd_ohci_get_32bit_frame_number( st_usbh1_ohci_hcd_device_data_p_t device_data )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data Pointer
 * Return       : uint32_t                               ; 32bit frame number
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_get_32bit_frame_number(st_usbh1_ohci_hcd_device_data_p_t device_data)
{
    uint32_t                        fm;
    uint32_t                        hp;
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;

    /* This code accounts for the fact that hcca_frame_number is updated by the HC before the HCD gets an interrupt that will adjust frame_high_part */
    tmp_hcca = r_usbh1_pa_to_va((uint32_t)device_data->hcca);
    hp = device_data->frame_high_part;
    fm = tmp_hcca->hcca_frame_number;

    return (((fm & 0x7FFF) | hp) + ((fm ^ hp) & 0x8000));
} /* End of function usbh1_hstd_ohci_get_32bit_frame_number() */

/***********************************************************************************************************************
 * Function     : OHCI Check bandwidth
 * Declaration  : uint32_t usbh1_hstd_ohci_check_bandwidth( st_usbh1_ohci_hcd_device_data_p_t device_data, uint32_t list, uint32_t *best_list )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data Pointer
 *              ; uint32_t list                          ; list
 *              ; uint32_t *best_list                    ; pointer of return best list
 * Return       : uint32_t                               ; Worst Bandwidth
 * Note         :
***********************************************************************************************************************/
uint32_t usbh1_hstd_ohci_check_bandwidth(st_usbh1_ohci_hcd_device_data_p_t device_data, uint32_t list, uint32_t *p_best_list)
{
/*++
    This routine scans all the scheduling lists of frequency determined by
    the base List passed in and returns the worst bandwidth found (i.e., max
    in use by any given scheduling list) and the list which had the least
    bandwidth in use.
    List - must be a base scheduling list. I.e., it must be one of
    USB_OHCI_ED_INTERRUPT_1ms, USB_OHCI_ED_INTERRUPT_2ms,USB_OHCI_ED_INTERRUPT_4ms, ..., USB_OHCI_ED_INTERRUPT_32ms.
    All lists of the appropriate frequency are checked
--*/

    uint32_t    last_list;
    uint32_t    index;
    uint32_t    best_bandwidth;
    uint32_t    worst_bandwidth;
    uint32_t    bandwidth;

    worst_bandwidth = 0;
    best_bandwidth = ~0;

    for (last_list = list + list; list <= last_list; list++)
    {
        /* Sum bandwidth in use in this scheduling time */
        bandwidth = 0;
        for (index = list; index != USBH1_OHCI_ED_EOF; index = device_data->ed_list[index].next)
        {
            bandwidth += device_data->ed_list[index].bandwidth;
        }

        /* Remember best and worst */
        if (bandwidth < best_bandwidth)
        {
            best_bandwidth = bandwidth;
            *p_best_list = list;
        }

        if (bandwidth > worst_bandwidth)
        {
            worst_bandwidth = bandwidth;
        }
    }

    return worst_bandwidth;
} /* End of function usbh1_hstd_ohci_check_bandwidth() */

/***********************************************************************************************************************
 * Function     : OHCI H/W Setup
 * Declaration  : uint32_t usbh1_hstd_ohci_hw_setup( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_hw_setup( void )
{
    uint32_t    n;
    uint32_t    portnum;
    uint32_t    numports;
    uint32_t    *p_port_status_reg;
    uint32_t    timeout;
    uint32_t    err;

    /* Setup usbh1_hstd_ohci_device_data */
    usbh1_hstd_ohci_device_data = usbh1_hstd_ohci_alloc_device_data();
    if (NULL == usbh1_hstd_ohci_device_data)
    {
        return;
    }

    /* Register setting */
//    usbh1_hstd_ohci_device_data->HC = (USBH1_OHCI_HC_OPERATIONAL_REGISTER_P)USBH1_OHCI_BASE;

    /* H/W reset */
    err = usbh1_hstd_ohci_hw_reset();
    if (USBH1_OK != err)
    {
        return;
    }

    USB10.HCHCCA.LONG = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_get_hcca_address());

    usbh1_hstd_ohci_device_data->hcca = usbh1_hstd_ohci_physical_address_of(usbh1_hstd_ohci_get_hcca_address());

    usbh1_hstd_ohci_clear_hcca(usbh1_hstd_ohci_device_data);

    usbh1_hstd_ohci_initailize_interrupt_lists(usbh1_hstd_ohci_device_data);

    numports = USB10.HCRHDESCRIPTORA.LONG & 0x000000FF;

    /* Set USB-Operational state */
    USB10.HCCONTROL.LONG = (USB10.HCCONTROL.LONG & ~0x000000C0) | 0x00000080;

    /*  Clear HcInterruptStatus */
    USB10.HCINTERRUPTSTATUS.LONG = 0xFFFFFFFF;

    USB10.HCFMINTERVAL.LONG = 0x27782EDF;
    USB10.HCPERIODICSTART.LONG = 0x2A2F;

    /*  Set PowerControl-related Reg */
    USB10.HCRHDESCRIPTORA.LONG    = 0x02001902;   /* Enable Power-SW, Disable OverCurrent */
    for (n = 0; n < numports; n++)
    {
        USB10.HCRHDESCRIPTORB.LONG |= (0x00020000 << n);
    }

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;

        *p_port_status_reg = 0x00000100;
        timeout = 0;
        while (*p_port_status_reg & 0x00000100 == 0x00000000)
        {
            if (timeout > USBH1_OHCI_TIMEOUT)
            {
                USBH1_HCI_PRINTF0_R("Error : usbh1_hstd_ohci_hw_setup TIMEOUT\n");
                return;
            }

            r_usbh1_hstd_hci_wait_time(1);
            timeout++;
        }
    }

#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    /* Enable OverCurrent */
    USB10.HCRHDESCRIPTORA.LONG &= ~0x00001000;
#endif /* (BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB) */

    for (portnum = 0; portnum < numports; portnum++)
    {
        p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
        p_port_status_reg += portnum;
        *p_port_status_reg = 0xFFFF0000;
    }

    usbh1_hstd_ohci_device_data->ed_list[USBH1_OHCI_ED_CONTROL].physical_head = (uint32_t *)&USB10.HCCONTROLHEADED.LONG;
    usbh1_hstd_ohci_device_data->ed_list[USBH1_OHCI_ED_BULK].physical_head = (uint32_t *)&USB10.HCBULKHEADED.LONG;

    /*  Enable MasterInt,RHSC,FNO,WBDH */
    USB10.HCINTERRUPTENABLE.LONG = 0x80000062;

    /* Control,Bulk,Iso,Periodic List Enable */
    USB10.HCCONTROL.LONG |= 0x0000003C;

    return;
} /* End of function usbh1_hstd_ohci_hw_setup() */

/***********************************************************************************************************************
 * Function     : Reset OHCI H/W
 * Declaration  : void usbh1_hstd_EhciHwReset( void )
 * Argument     : void
 * Return       : uint32_t ; status
 * Note         :
***********************************************************************************************************************/
static uint32_t usbh1_hstd_ohci_hw_reset(void)
{
    uint32_t    timeout;

    USB10.HCCOMMANDSTATUS.LONG = 0x00000001;  /* Reset HC */
    timeout = 0;
    while (USB10.HCCOMMANDSTATUS.LONG & 0x00000001)
    {
        if (timeout > USBH1_OHCI_TIMEOUT)
        {
            USBH1_HCI_PRINTF0_R("Error : usbh1_hstd_ohci_hw_setup TIMEOUT\n");
            return USBH1_ERROR;
        }

        r_usbh1_hstd_hci_wait_time(1);
        timeout++;
    }

    return USBH1_OK;
} /* End of function usbh1_hstd_ohci_hw_reset() */

/***********************************************************************************************************************
 * Function     : OHCI Initialize InterruptED List
 * Declaration  : void usbh1_hstd_ohci_initailize_interrupt_lists( st_usbh1_ohci_hcd_device_data_p_t device_data )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_initailize_interrupt_lists(st_usbh1_ohci_hcd_device_data_p_t device_data)
{
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   static_ed[USBH1_OHCI_ED_INTERRUPT_32ms];
    uint32_t                                    i;
    uint32_t                                    j;
    uint32_t                                    k;
    st_usbh1_ohci_hcca_block_p_t                tmp_hcca;

//    static uint8_t Balance[16] =
    uint8_t Balance[16] =
    {
        0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF
    };

    /* Allocate satirically disabled EDs, and set head pointers for scheduling lists */
    for (i = 0; i < USBH1_OHCI_ED_INTERRUPT_32ms; i++)
    {
        ed = usbh1_hstd_ohci_allocate_endpoint_descriptor();
        ed->physical_address = usbh1_hstd_ohci_physical_address_of(&ed->hc_ed);
        device_data->ed_list[i].physical_head = usbh1_hstd_ohci_physical_address_of(&ed->hc_ed.next_ed);

        ed->hc_ed.control.k = 1;     /* mark ED as disabled */
        ed->hc_ed.control.s = 0;

        usbh1_hstd_ohci_initialize_list_head(&device_data->ed_list[i].head);
        static_ed[i] = ed;
        if (i > 0)
        {
            device_data->ed_list[i].next = (i - 1) / 2;
            ed->hc_ed.next_ed = static_ed[(i - 1) / 2]->physical_address;
        }
        else
        {
            device_data->ed_list[i].next = USBH1_OHCI_ED_EOF;
            ed->hc_ed.next_ed = 0;
        }
    }

    /* Set head pointers for 32ms scheduling lists which start from HCCA */
    for (i = 0, j = USBH1_OHCI_ED_INTERRUPT_32ms ; i < 32; i++, j++)
    {
        tmp_hcca = r_usbh1_pa_to_va((uint32_t)device_data->hcca);
        device_data->ed_list[j].physical_head = usbh1_hstd_ohci_physical_address_of(&tmp_hcca->hcca_interrupt_list[i]);
        usbh1_hstd_ohci_initialize_list_head(&device_data->ed_list[j].head);
        k = Balance[i & 0xF] + USBH1_OHCI_ED_INTERRUPT_16ms;
        device_data->ed_list[j].next = k;
        tmp_hcca->hcca_interrupt_list[i] = static_ed[k]->physical_address;
    }
} /* End of function usbh1_hstd_ohci_initailize_interrupt_lists() */

/***********************************************************************************************************************
 * Function     : OHCI Clear HCCA
 * Declaration  : void usbh1_hstd_ohci_clear_hcca( st_usbh1_ohci_hcd_device_data_p_t device_data )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_clear_hcca(st_usbh1_ohci_hcd_device_data_p_t device_data)
{
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;

    tmp_hcca = r_usbh1_pa_to_va((uint32_t)device_data->hcca);

    tmp_hcca->hcca_frame_number = 0x0;
    tmp_hcca->hcca_pad1         = 0x0;
    tmp_hcca->hcca_done_head    = 0x0;
} /* End of function usbh1_hstd_ohci_clear_hcca() */

/***********************************************************************************************************************
 * Function     : OHCI Clear transfer request for disconnected device
 * Declaration  : void usbh1_hstd_ohci_clear_device_tr_req( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_clear_device_tr_req(uint32_t devadd)
{
    uint32_t    i;
    uint32_t    n;
//    uint8_t     free_ed = FALSE;
    uint8_t     free_ed = TRUE;

    for (i = 0; i < USBH1_HCI_TRREQ_NUM; i++)
    {
        if ((TRUE == g_usbh1_hstd_hci_transfer_request[i].bit.enable) && (devadd == g_usbh1_hstd_hci_transfer_request[i].bit.devadrs))
        {
            /* Cancel Request */
            usbh1_hstd_ohci_cancel_transfer_request(&g_usbh1_hstd_hci_transfer_request[i]);

            /* Remove ED */
            usbh1_hstd_ohci_remove_device(devadd, free_ed);

            /* Check downstream port */
            for (n = 0; n < USBH1_HCI_HUBPORT_NUM; n++)
            {
                if (0 != g_usbh1_hstd_hci_transfer_request[i].devinfo->hubdevinfo.devnum[n])
                {
                    usbh1_hstd_ohci_clear_device_tr_req(g_usbh1_hstd_hci_transfer_request[i].devinfo->hubdevinfo.devnum[n]);
                }
            }
        }
    }
} /* End of function usbh1_hstd_ohci_clear_device_tr_req() */

/***********************************************************************************************************************
 * Function     : OHCI Root Port Device Connection
 * Declaration  : void usbh1_hstd_ohci_rootport_connection( st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t devspeed, uint8_t portnum )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t devicedata ; Device Data Pointer
 *              : uint8_t devspeed                      ; Device Speed
 *              : uint8_t portnum                       ; Port No
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_rootport_connection(st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t devspeed, uint8_t portnum)
{
    st_usbh1_hci_device_t   *p_devinfo;
//    uint8_t                 free_ed = FALSE;
    uint8_t                 free_ed = TRUE;

    usbh1_hstd_ohci_remove_device(0, free_ed);

    p_devinfo = usbh1_hstd_hci_alloc_dev_info();
    if(p_devinfo != NULL)
    {
        p_devinfo->bit.devspeed = devspeed;
        p_devinfo->bit.devnum   = 0;
        p_devinfo->bit.hctype   = USBH1_HCI_HCTYPE_OHCI;
        p_devinfo->hci_device   = devicedata;
        devicedata->root_device[portnum] = p_devinfo->bit.devnum;

        /* attach callback */
        g_usbh1_hstd_hci_callback.attach_cb(portnum);
    }
} /* End of function usbh1_hstd_ohci_rootport_connection() */

/***********************************************************************************************************************
 * Function     : OHCI Root Port Device Disconnection
 * Declaration  : void usbh1_hstd_ohci_rootport_disconnection( st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t portnum )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t devicedata ; Device Data Pointer
 *              : uint8_t portnum                       ; Port No
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_rootport_disconnection(st_usbh1_ohci_hcd_device_data_p_t devicedata, uint8_t portnum)
{
    r_usbh1_hstd_hci_disconnect(devicedata->root_device[portnum]);

    devicedata->root_device[portnum] = USBH1_HCI_NODEVICE;

    /* detach callback */
    g_usbh1_hstd_hci_callback.detach_cb(portnum);
} /* End of function usbh1_hstd_ohci_rootport_disconnection() */

/***********************************************************************************************************************
 * Function     : OHCI Remove Device
 * Declaration  : void usbh1_hstd_ohci_remove_device( uint8_t device_address, boolean_t free_ed )
 * Argument     : uint8_t device_address  ; device address
 *              : boolean_t free_ed       ; free flag
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_remove_device(uint8_t device_address, boolean_t free_ed)
{
    uint32_t    n;

    for (n = 0; n < USBH1_OHCI_MAXENDPOINT; n++)
    {
        usbh1_hstd_ohci_remove_endpoint(device_address, n, USBH1_HCI_DIRECTION_OUT, free_ed);
        usbh1_hstd_ohci_remove_endpoint(device_address, n, USBH1_HCI_DIRECTION_IN, free_ed);
    }
} /* End of function usbh1_hstd_ohci_remove_device() */

/***********************************************************************************************************************
 * Function     : OHCI Return speed information
 * Declaration  : uint8_t usbh1_hstd_ohci_get_root_device_speed( st_usbh1_ohci_hcd_device_data_p_t device_data, uint8_t portnum )
 * Argument     : st_usbh1_ohci_hcd_device_data_p_t device_data ; Device Data pointer
 *              : uint8_t portnum                        ; Port No
 * Return       : uint8_t                                ; Speed
 * Note         :
***********************************************************************************************************************/
static uint8_t usbh1_hstd_ohci_get_root_device_speed(st_usbh1_ohci_hcd_device_data_p_t device_data, uint8_t portnum)
{
    uint32_t    *p_port_status_reg;

    p_port_status_reg = (uint32_t *)&(USB10.HCRHPORTSTATUS1.LONG);
    p_port_status_reg += portnum;

    if (0 == (*p_port_status_reg & 0x00000200))
    {
        return USBH1_HCI_DEVSPEED_FS;
    }
    return USBH1_HCI_DEVSPEED_LS;
} /* End of function usbh1_hstd_ohci_get_root_device_speed() */

/***********************************************************************************************************************
 * Function     : OHCI Pause Endpoint
 * Declaration  : void usbh1_hstd_ohci_process_paused_ed( st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed )
 * Argument     : st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed ; Endpoint Descriptor Pointer
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_process_paused_ed(st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed)
{
    st_usbh1_ohci_hcd_endpoint_p_t              endpoint;
    st_usbh1_ohci_request_p_t                     ohci_req;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   last = NULL;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   *p_previous;
    boolean_t                                   b4head = TRUE;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   tmp_last_hc_td;
    st_usbh1_ohci_hcd_transfer_descriptor_p_t   tmp_endpoint_hcd_head_p;

    endpoint = r_usbh1_pa_to_va(ed->endpoint);

    if (NULL == endpoint)
    {
        return;
    }

    td = r_usbh1_pa_to_va(endpoint->hcd_head_p);            /* set head TD */
    p_previous = r_usbh1_pa_to_va(&endpoint->hcd_head_p);

    while (td != r_usbh1_pa_to_va(endpoint->hcd_tail_p))
    {

        if ((ed->hc_ed.head_p & ~0xF) == td->physical_address)
        {
            b4head = FALSE;
        }

        if ((USBH1_OHCI_ED_EOF == ed->list_index) || (td->cancel_pending))  /* cancel TD */
        {
#if 0
//          ohci_req = td->usb_drequest;
            usbh1_hstd_ohci_remove_list_entry(&td->request_list);
//            if(usbh1_hstd_ohci_is_list_empty(&ohci_req->hcd_list) == TRUE)
//            if(usbh1_hstd_ohci_is_list_empty(&ohci_req->hcd_list) == FALSE)
//          {
//              ohci_req->status = USBH1_OHCI_RS_CANCELED;
//              usbh1_hstd_ohci_complete_transfer_request(ohci_req);
//          }
#else
        	ohci_req = r_usbh1_pa_to_va(&td->usb_drequest);
            usbh1_hstd_ohci_remove_list_entry(&td->request_list);
          if(usbh1_hstd_ohci_is_list_empty(&ohci_req->hcd_list) == TRUE)
          {
              ohci_req->status = USBH1_OHCI_RS_CANCELED;
              usbh1_hstd_ohci_complete_transfer_request(ohci_req);
          }
#endif

            p_previous = r_usbh1_pa_to_va(&td->next_hcd_td);      /* point around TD */
            if (NULL != last)
            {
            }

            if(TRUE == b4head)              /* TD on delayed Done List */
            {
                td->td_status = USBH1_OHCI_TD_CANCELED;
            }
            else
            {
                usbh1_hstd_ohci_free_transfer_descriptor(td);
            }
        }
        else                                                            /* don't cancel TD */
        {
            p_previous = r_usbh1_pa_to_va(&td->next_hcd_td);
            if (FALSE == b4head)
            {
                last = td;
            }
        }

        td = r_usbh1_pa_to_va(*p_previous);


    }

    tmp_endpoint_hcd_head_p = r_usbh1_pa_to_va((uint32_t)endpoint->hcd_head_p);
    ed->hc_ed.head_p    = tmp_endpoint_hcd_head_p->physical_address | (ed->hc_ed.head_p & USBH1_OHCI_HCED_HEADP_CARRY);
    ed->hc_ed.control.k = FALSE;
} /* End of function usbh1_hstd_ohci_process_paused_ed() */

/***********************************************************************************************************************
 * Function     : OHCI Remove Endpoint
 * Declaration  : void usbh1_hstd_ohci_remove_ed( st_usbh1_ohci_hcd_endpoint_p_t endpoint, boolean_t free_ed )
 * Argument     : st_usbh1_ohci_hcd_endpoint_p_t endpoint ; Endpoint Pointer
 *              : boolean_t free_ed                ; endpoint free flag
 * Return       : void
 * Note         :
***********************************************************************************************************************/
static void usbh1_hstd_ohci_remove_ed( st_usbh1_ohci_hcd_endpoint_p_t endpoint, boolean_t free_ed )
{
    st_usbh1_ohci_hcd_device_data_p_t           device_data;
    st_usbh1_ohci_hcd_ed_list_p_t               list;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   ed;
    st_usbh1_ohci_hcd_endpoint_descriptor_p_t   previous_ed;
    uint32_t                                    list_disable;
    st_usbh1_ohci_list_entry_p_t                tmp_link_b_link;
    uint32_t                                    *tmp_list_physical_head;

    device_data = r_usbh1_pa_to_va(endpoint->device_data);
    list = &device_data->ed_list[endpoint->list_index];
    ed = r_usbh1_pa_to_va(endpoint->hcd_ed);

    /* Prevent Host controller from processing this ED */
    ed->hc_ed.control.k = TRUE;

    /* Unlink the ED from the physical ED list */
/*    if (ed->link.b_link == &list->head)*/   /* Removing ED is located in head of ED List */
    if (r_usbh1_pa_to_va(ed->link.b_link) == &list->head)   /* Removing ED is located in head of ED List */
    {
        /* Remove ED from head */
        tmp_list_physical_head = r_usbh1_pa_to_va(list->physical_head);
        *tmp_list_physical_head = ed->hc_ed.next_ed;
        previous_ed = NULL;

    }
    else
    {
        /* Remove ED from list */
/*
    	previous_ed = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
                        ed->link.b_link, USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);
*/
        previous_ed = (st_usbh1_ohci_hcd_endpoint_descriptor_p_t)usbh1_hstd_ohci_scan_containing_record(
        		r_usbh1_pa_to_va(ed->link.b_link), USBH1_OHCI_CR_ENDPOINT_DESCRIPTOR, USBH1_OHCI_CR_LINK);
        if (NULL != previous_ed)
        {
            previous_ed->hc_ed.next_ed = ed->hc_ed.next_ed;
        }
    }

    /* Unlink ED from HCD list */
    usbh1_hstd_ohci_remove_list_entry(&ed->link);

    /* If freeing the endpoint, remove the descriptor */
    if (TRUE == free_ed)    /* TD queue must already be empty */
    {
        endpoint->hcd_ed = NULL;
        ed->endpoint = NULL;
    }

    /* Check to see if interrupt processing is required to free the ED */
    switch (endpoint->type)
    {
        case USBH1_EP_CNTRL:
            list_disable = ~USBH1_OHCI_CONTROLLISTENABLE;
        break;

        case USBH1_EP_BULK:
            list_disable = ~USBH1_OHCI_BULKLISTENABLE;
        break;

        case USBH1_EP_INT:
        case USBH1_EP_ISO:
        	list_disable = ~(USBH1_OHCI_ISOCHRONOUSENABLE | USBH1_OHCI_INTERRUPTLISTENABLE); 
        	break;
        default:
            device_data->ed_list[endpoint->list_index].bandwidth -= endpoint->bandwidth;
            device_data->max_bandwidth_inuse = usbh1_hstd_ohci_check_bandwidth(device_data,
                                                                               USBH1_OHCI_ED_INTERRUPT_32ms,
                                                                               &list_disable);
            list_disable = 0;
        break;
    }

    ed->list_index = USBH1_OHCI_ED_EOF;     /* ED is not on a list now */

    /* Set ED for reclamation */
    USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_HC_INT_SOF;   /* Clear SOF interrupt pending */

    if (0 != list_disable)
    {
        USB10.HCCONTROL.LONG &= list_disable;
        ed->reclamation_frame = usbh1_hstd_ohci_get_32bit_frame_number(device_data) + 1;
        usbh1_hstd_ohci_insert_tail_list(&device_data->stalled_ed_reclamation, &ed->link);
        USB10.HCINTERRUPTENABLE.LONG = USBH1_OHCI_HC_INT_SOF;   /* interrupt on next SOF */
    }
    else
    {
        ed->reclamation_frame = usbh1_hstd_ohci_get_32bit_frame_number(device_data) + 1;
        usbh1_hstd_ohci_insert_tail_list(&device_data->running_ed_reclamation, &ed->link);
    }
} /* End of function usbh1_hstd_ohci_remove_ed() */

/***********************************************************************************************************************
 * Function     : usbh1_disconnect_done_queue
 * Argument     : uint16_t devadr
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh1_disconnect_done_queue(uint16_t devadr)
{
    uint8_t     i;
    st_usbh1_ohci_hcca_block_p_t    tmp_hcca;
    uint32_t                        *tmp_hcca_done_head;

	r_usbh1_hstd_hci_wait_time(32);
		tmp_hcca = (st_usbh1_ohci_hcca_block_p_t)r_usbh1_pa_to_va((uint32_t)usbh1_hstd_ohci_device_data->hcca);
		tmp_hcca_done_head = r_usbh1_pa_to_va((uint32_t)tmp_hcca->hcca_done_head);
		if(0 != tmp_hcca_done_head)
		{
			usbh1_hstd_ohci_process_done_queue(tmp_hcca_done_head);
			/* Done Queue processing complete, notify controller */
			tmp_hcca->hcca_done_head = 0;
			if(USB10.HCINTERRUPTSTATUS.LONG & USBH1_OHCI_IS_WRITEBACKDONEHEAD)
			{
				USB10.HCINTERRUPTSTATUS.LONG = USBH1_OHCI_IS_WRITEBACKDONEHEAD;
			}
		}


 	/* r_usbh1_hstd_hci_wait_time(1); */
}

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
