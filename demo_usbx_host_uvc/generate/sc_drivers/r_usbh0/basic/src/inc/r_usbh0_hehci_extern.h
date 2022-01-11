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
* File Name    : r_usbh0_hEhciExtern.c
* Description  : EHCI Extern Header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

#ifndef	R_USBH0_HEHCIEXTERN_H
#define R_USBH0_HEHCIEXTERN_H

/*======================================================================== */
/*                      Prototypes                                         */
/*======================================================================== */
/* function */
/* r_usbh0_hEhciMain.c */
uint32_t        usbh0_hstd_ehci_init(void);
void            usbh0_hstd_ehci_deinit(void);
//extern void     usbh0_hstd_EhciInterruptEnable(void);
//extern void     usbh0_hstd_EhciInterruptDisable(void);
uint32_t        usbh0_hstd_ehci_port_reset(uint32_t portnum);
void            usbh0_hstd_ehci_disconnect(uint32_t devadd);
uint32_t        usbh0_hstd_ehci_get_frame_list_size(void);
void            usbh0_hstd_ehci_wait_micro_frame(uint32_t uframe_no);
void            usbh0_hstd_ehci_add_async(st_usbh0_ehci_qh_t *qh);
void            usbh0_hstd_ehci_add_periodic(u_usbh0_ehci_flep_t *new_element, uint32_t type, uint32_t pollrate,
                                                uint32_t pollrate_offset);
void            usbh0_hstd_ehci_link_qtd(st_usbh0_ehci_qtd_t *qtd1st, st_usbh0_ehci_qtd_t *qtd2nd);
void            usbh0_hstd_ehci_clear_qtd(st_usbh0_ehci_qtd_t *qtd_head);
void            usbh0_hstd_ehci_inactive_qtd(st_usbh0_ehci_qtd_t *qtd_head);
void            usbh0_hstd_ehci_unlink_qh_for_async(st_usbh0_ehci_qh_t *qh);
void            usbh0_hstd_ehci_unlink_qh_for_periodic(st_usbh0_ehci_qh_t *qh);
void            usbh0_hstd_ehci_unlink_itd(st_usbh0_ehci_itd_t *itd);
void            usbh0_hstd_ehci_unlink_sitd(st_usbh0_ehci_sitd_t *sitd);
uint32_t        usbh0_hstd_ehci_update_periodic_scheduling_score(uint32_t max_period, uint32_t pollrate);
void            usbh0_hstd_ehci_int_transfer_end(void);
void            usbh0_hstd_ehci_int_port_change(void);
uint32_t        usbh0_hstd_ehci_get_port_current_connect_status(uint16_t rootport);
void            usbh0_hstd_ehci_clear_enumalation_wait_flag(void);
void            usbh0_hstd_ehci_interrupt_handler(void);
uint16_t        usbh0_hstd_ehci_get_device_address_of_rootpoot(uint16_t rootport);
uint16_t        usbh0_hstd_ehci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t deviceaddr);
uint16_t        usbh0_hstd_ehci_get_transfer_end_status(st_usbh0_hci_tr_req_t *tr_req);
uint16_t        usbh0_hstd_ehci_get_pid_status(st_usbh0_hci_tr_req_t *tr_req);
void            usbh0_hstd_ehci_port_suspend(uint16_t port);
void            usbh0_hstd_ehci_port_resume(uint16_t port);
void            usbh0_hstd_ehci_cancel_transfer_request(st_usbh0_hci_tr_req_t *tr_req);

/* r_usbh0_hEhciMemory.c */
void                    usbh0_hstd_ehci_init_qh_memory(void);
void                    usbh0_hstd_ehci_init_itd_memory(void);
void                    usbh0_hstd_ehci_init_sitd_memory(void);
void                    usbh0_hstd_ehci_init_qtd_memory(void);
void                    usbh0_hstd_ehci_init_periodic_frame_list(void);
uint32_t                usbh0_hstd_ehci_get_periodic_frame_list_addr(void);
st_usbh0_ehci_qh_t     *usbh0_hstd_ehci_alloc_qh(void);
st_usbh0_ehci_itd_t     *usbh0_hstd_ehci_alloc_itd(void);
st_usbh0_ehci_sitd_t    *usbh0_hstd_ehci_alloc_sitd(void);
st_usbh0_ehci_qtd_t     *usbh0_hstd_ehci_alloc_qtd(void);
void                    usbh0_hstd_ehci_free_qtd(st_usbh0_ehci_qtd_t *free_qtd);
st_usbh0_ehci_qh_t     *usbh0_hstd_ehci_search_qh(uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype);
st_usbh0_ehci_itd_t     *usbh0_hstd_ehci_search_itd(uint32_t devadd, uint32_t epnum, uint32_t direction);
st_usbh0_ehci_sitd_t    *usbh0_hstd_ehci_search_sitd(uint32_t devadd, uint32_t epnum, uint32_t direction);
void                    usbh0_hstd_ehci_clear_device_qh(uint32_t devadd);
void                    usbh0_hstd_ehci_clear_qh(st_usbh0_ehci_qh_t *qh);
void                    usbh0_hstd_ehci_clear_device_itd(uint32_t devadd);
void                    usbh0_hstd_ehci_clear_device_sitd(uint32_t devadd);

/* r_usbh0_hEhciTransfer.c */
void            usbh0_hstd_ehci_make_transfer_request(st_usbh0_hci_tr_req_t *p_tr_req);
//extern void     usbh0_hstd_EhciSetAsyncQh(st_usbh0_hci_tr_req_t *tr_req, st_usbh0_ehci_qh_t *qh);
void            usbh0_hstd_ehci_transfer_end_qh(st_usbh0_hci_tr_req_t *p_tr_req);
void            usbh0_hstd_ehci_transfer_end_itd(st_usbh0_hci_tr_req_t *p_tr_req);
void            usbh0_hstd_ehci_transfer_end_sitd(st_usbh0_hci_tr_req_t *p_tr_req);

#endif  /* R_USBH0_HEHCIEXTERN_H */
