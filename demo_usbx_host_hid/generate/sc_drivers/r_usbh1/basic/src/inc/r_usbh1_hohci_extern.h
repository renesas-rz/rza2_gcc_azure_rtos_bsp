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
* File Name    : r_usbh1_hOhciExtern.c
* Description  : OHCI Extern Header
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/

#ifndef R_USBH1_HOHCI_EXTERN_H
#define R_USBH1_HOHCI_EXTERN_H

/*======================================================================== */
/*                      Prototypes                                         */
/*======================================================================== */
/* function */
/* r_usbh1_hOhciMain.c */
uint32_t                        usbh1_hstd_ohci_init(void);
void                            usbh1_hstd_ohci_deinit(void);
void                            usbh1_hstd_ohci_interrupt_handler(void);
void                            usbh1_hstd_OhciMainRoutine(uint32_t context_info, uint32_t port_reg, uint32_t port_num);
void                            usbh1_hstd_ohci_control_roothub(uint32_t port_reg, uint32_t port_num);
void                            usbh1_hstd_ohci_disconnect(uint32_t devadd);
uint16_t                        usbh1_hstd_ohci_get_device_address_of_rootpoot(uint16_t rootport);
uint16_t                        usbh1_hstd_ohci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t deviceaddr);
void                            usbh1_hstd_ohci_port_reset(uint32_t portnum);
uint16_t                        usbh1_hstd_ohci_get_pid_status(st_usbh1_hci_tr_req_t *tr_req);
void                            usbh1_hstd_ohci_port_suspend(uint16_t port);
void                            usbh1_hstd_ohci_port_resume(uint16_t port);
uint16_t                        usbh1_hstd_ohci_get_transfer_end_status(st_usbh1_hci_tr_req_t *tr_req);
uint32_t                        usbh1_hstd_ohci_remove_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction, boolean_t free_ed);
uint32_t                        usbh1_hstd_ohci_pause_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction);
st_usbh1_ohci_list_entry_p_t    usbh1_hstd_ohci_remove_list_head(st_usbh1_ohci_list_entry_p_t list);
void                            usbh1_hstd_ohci_remove_list_entry(st_usbh1_ohci_list_entry_p_t list);
void                            usbh1_hstd_ohci_initialize_list_head(st_usbh1_ohci_list_entry_p_t list);
uint32_t                        usbh1_hstd_ohci_is_list_empty(st_usbh1_ohci_list_entry_p_t list);
void                            usbh1_hstd_ohci_insert_head_list(st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link);
void                            usbh1_hstd_ohci_insert_tail_list(st_usbh1_ohci_list_entry_p_t list_head, st_usbh1_ohci_list_entry_p_t link);
uint32_t                        usbh1_hstd_ohci_physical_address_of(void *data);
void                            usbh1_hstd_ohci_pause_ed(st_usbh1_ohci_hcd_endpoint_p_t endpoint);
void                            usbh1_hstd_ohci_unschedule_isochronous_or_interrupt_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint, boolean_t free_ed, boolean_t endpoint_processing_required);
uint32_t                        usbh1_hstd_ohci_set_frame_interval(st_usbh1_ohci_hcd_device_data_p_t device_data, boolean_t up_not_down);
uint32_t                        usbh1_hstd_ohci_get_32bit_frame_number(st_usbh1_ohci_hcd_device_data_p_t device_data);
uint32_t                        usbh1_hstd_ohci_check_bandwidth(st_usbh1_ohci_hcd_device_data_p_t device_data, uint32_t list, uint32_t *p_best_list);


/* r_usbh1_hOhciMemory.c */
st_usbh1_ohci_hcca_block_p_t                usbh1_hstd_ohci_get_hcca_address(void);
void                                        usbh1_hstd_ohci_init_request_memory(void);
void                                        usbh1_hstd_ohci_init_device_data(void);
void                                        usbh1_hstd_ohci_clear_list(void);
st_usbh1_ohci_request_p_t                   usbh1_hstd_ohci_alloc_transfer_request(void);
st_usbh1_ohci_hcd_device_data_p_t           usbh1_hstd_ohci_alloc_device_data(void);
st_usbh1_ohci_hcd_endpoint_p_t              usbh1_hstd_ohci_alloc_endpoint(void);
st_usbh1_ohci_hcd_endpoint_descriptor_p_t   usbh1_hstd_ohci_allocate_endpoint_descriptor(void);
st_usbh1_ohci_hcd_transfer_descriptor_p_t   usbh1_hstd_ohci_allocate_transfer_descriptor(void);
st_usbh1_ohci_iso_info_p_t                  usbh1_hstd_ohci_alloc_endpoint_iso(void);
void                                        usbh1_hstd_ohci_free_transfer_request(st_usbh1_ohci_request_p_t ohci_req);
void                                        usbh1_hstd_ohci_free_transfer_descriptor(st_usbh1_ohci_hcd_transfer_descriptor_p_t td);
void                                        usbh1_hstd_ohci_free_endpoint(st_usbh1_ohci_hcd_endpoint_p_t endpoint);
void                                        usbh1_hstd_ohci_free_endpoint_descriptor(st_usbh1_ohci_hcd_endpoint_descriptor_p_t ed);
st_usbh1_ohci_hcd_endpoint_p_t              usbh1_hstd_ohci_scan_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction);
void                                        usbh1_hstd_ohci_store_endpoint(uint8_t device_address, uint8_t endpoint_number, uint8_t direction, st_usbh1_ohci_hcd_endpoint_p_t endpoint);
uint32_t                                    usbh1_hstd_ohci_scan_containing_record(st_usbh1_ohci_list_entry_p_t list, uint8_t type, uint8_t field);
uint32_t                                    usbh1_hstd_ohci_check_remained_td(void);
uint32_t                                    usbh1_hstd_ohci_check_remained_ed(void);
uint32_t                                    usbh1_hstd_ohci_check_remained_ep(void);


/* r_usbh1_hOhciTransfer.c */
void    usbh1_hstd_ohci_make_transfer_request(st_usbh1_hci_tr_req_t *p_tr_req);
void    usbh1_hstd_ohci_cancel_transfer_request(st_usbh1_hci_tr_req_t *p_tr_req);
void    usbh1_hstd_ohci_process_done_queue(uint32_t phys_hc_td);
void    usbh1_hstd_ohci_complete_transfer_request(st_usbh1_ohci_request_p_t ohci_req);

#endif  /* R_USBH1_HOHCI_EXTERN_H */
