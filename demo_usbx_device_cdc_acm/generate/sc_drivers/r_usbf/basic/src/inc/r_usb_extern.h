/**********************************************************************************************************************
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
 * Copyright (C) 2015(2018) Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : r_usb_extern.h
 * Description  : USB common extern header
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version Description
 *         : 08.01.2014 1.00 First Release
 *         : 26.12.2014 1.10 RX71M is added
 *         : 30.09.2015 1.11 RX63N/RX631 is added.
 *         : 30.09.2016 1.20 RX65N/RX651 is added.
 *         : 26.01.2017 1.21 "usb_check_use_usba_module()" is added.
 *         : 30.09.2017 1.22 change Functions name and variable name.
 *                           "usb_hstd_buf2fifo" -> "usb_hstd_buf_to_fifo"
 *                           "usb_pstd_buf2fifo" -> "usb_pstd_buf_to_fifo"
 *                           "class_driver_start" -> "usb_class_driver_start"
 *                           "pcdc_read_complete" -> "usb_pcdc_read_complete"
 *                           "pcdc_write_complete" -> "usb_pcdc_write_complete"
 *                           "phid_read_complete" -> "usb_phid_read_complete"
 *                           "phid_write_complete" -> "usb_phid_write_complete"
 *                           "hcdc_read_complete" -> "usb_hcdc_read_complete"
 *                           "hcdc_write_complete" -> "usb_hcdc_write_complete"
 *                           "hhid_read_complete" -> "usb_hhid_read_complete"
 *                           "hhid_write_complete" -> "usb_hhid_write_complete"
 *                           "g_usb_eptbl" -> "g_usb_pstd_eptbl"
 *                           "R_USB_HcdcTask" is added.
 *                           "R_USB_HcdcDriverStart" is added.
 *                           "R_USB_HhidTask" is added.
 *                           "R_USB_HhidDriverStart" is added.
 *                           "R_USB_HmscDriverStart"  is added.
 *                           "R_USB_HmscTask" is added.
 *                           "R_USB_HmscStrgDriveTask" is added.
 *                           "R_USB_HmscStrgDriveSearch" is added.
 *         : 31.03.2018 1.23 Supporting Smart Configurator
 ***********************************************************************************************************************/

#ifndef R_USB_EXTERN_H
#define R_USB_EXTERN_H

#include "stdbool.h"
#include "r_os_abstraction_typedef.h"

/*****************************************************************************
 Public Variables
 ******************************************************************************/
/* r_usbif_api.c */
extern uint32_t g_usb_read_request_size[USB_NUM_USBIP][USB_MAXPIPE_NUM + 1];

/* r_usb_cdataio.c */
extern usb_utr_t *g_p_usb_pstd_pipe[USB_MAX_PIPE_NO + 1u];
extern uint32_t g_usb_pstd_data_cnt[USB_MAX_PIPE_NO + 1u]; /* PIPEn Buffer counter */
extern uint8_t  *g_p_usb_pstd_data[USB_MAX_PIPE_NO + 1u]; /* PIPEn Buffer pointer(8bit) */

/* r_usb_pdriver.c */
extern uint16_t g_usb_pstd_stall_pipe[USB_MAX_PIPE_NO + 1u]; /* Stall Pipe info */
extern usb_cb_t g_usb_pstd_stall_cb; /* Stall Callback function */
extern uint16_t g_usb_pstd_config_num; /* Configuration Number */
extern uint16_t g_usb_pstd_alt_num[]; /* Alternate */
extern uint16_t g_usb_pstd_remote_wakeup; /* Remote Wake up Enable Flag */
extern uint16_t g_usb_pstd_remote_wakeup_state; /* Result for Remote wake up */
extern uint16_t g_usb_pstd_test_mode_select; /* Test Mode Selectors */
extern uint16_t g_usb_pstd_test_mode_flag; /* Test Mode Flag */
extern uint16_t g_usb_pstd_eptbl_index[2][USB_MAX_EP_NO + 1u]; /* Index of Endpoint Information table */
extern uint16_t g_usb_pstd_req_type; /* Request type */
extern uint16_t g_usb_pstd_req_value; /* Value */
extern uint16_t g_usb_pstd_req_index; /* Index */
extern uint16_t g_usb_pstd_req_length; /* Length */
extern usb_pcdreg_t g_usb_pstd_driver; /* Driver registration */
extern usb_setup_t g_usb_pstd_req_reg; /* Request variable */
extern usb_int_t g_usb_pstd_usb_int;
extern uint16_t g_usb_pstd_pipe0_request;
extern uint16_t g_usb_peri_connected;       /* Peri CDC application enable */
/* r_usb_usbif_api.c */
extern volatile uint16_t g_usb_usbmode __attribute__ ((section ("USB_DATA_RAM")));                                  /* USB mode HOST/PERI */ //20190124 ADD sawada
extern usb_utr_t g_usb_pdata[USB_MAXPIPE_NUM + 1];



extern  usb_ctrl_t      g_usb_cstd_event[];
extern  usb_hdl_t       g_usb_cur_task_hdl[];

extern usb_pipe_table_t    g_usb_pipe_table[USB_NUM_USBIP][USB_MAXPIPE_NUM+1];

extern usb_utr_t *get_usb_int_buf(void);
extern usb_callback_t       *g_usb_apl_callback;

/*****************************************************************************
 Public Functions
 ******************************************************************************/
/* r_usb_rx_mcu.c */
usb_err_t usb_module_start (uint8_t ip_type);
usb_err_t usb_module_stop (uint8_t ip_type);
void      usb_cpu_delay_xms (uint16_t time);
void      usb_cpu_delay_1us (uint16_t time);
void      usb_cpu_usbint_init (uint8_t ip_type);
uint16_t  usb_chattaring (uint16_t *syssts);

bool      usb_check_use_usba_module(usb_utr_t *ptr);

/* r_usb_cdataio.c */
uint8_t   usb_get_usepipe (usb_ctrl_t *p_ctrl, uint8_t dir);
usb_er_t  usb_data_read (usb_ctrl_t *p_ctrl, uint8_t  *buf, uint32_t size);
usb_er_t  usb_ctrl_read (usb_ctrl_t *p_ctrl, uint8_t  *buf, uint32_t size);
usb_er_t  usb_data_write (usb_ctrl_t *p_ctrl, uint8_t  *buf, uint32_t size);
usb_er_t  usb_ctrl_write (usb_ctrl_t *p_ctrl, uint8_t  *buf, uint32_t size);
usb_er_t  usb_data_stop (usb_ctrl_t *p_ctrl, uint16_t type);
usb_er_t  usb_ctrl_stop (usb_ctrl_t *p_ctrl);
void      usb_cstd_debug_hook (uint16_t error_code);
void      usb_cstd_select_nak (usb_utr_t *ptr, uint16_t pipe);

uint16_t  usb_cstd_get_pipe_buf_value (uint16_t pipe_no);

usb_er_t  usb_pstd_transfer_start (usb_utr_t *ptr);
usb_er_t  usb_pstd_transfer_end (uint16_t pipe);
void      usb_pstd_change_device_state (uint16_t state, uint16_t keyword, usb_cb_t complete);
void      usb_pstd_driver_registration (usb_pcdreg_t *callback);
void      usb_pstd_driver_release (void);

void      usb_pstd_send_start (uint16_t pipe);
void      usb_pstd_buf_to_fifo (uint16_t pipe, uint16_t  useport);
uint16_t  usb_pstd_write_data (uint16_t pipe, uint16_t pipemode);
void      usb_pstd_receive_start (uint16_t pipe);
void      usb_pstd_fifo_to_buf (uint16_t pipe, uint16_t  useport);
uint16_t  usb_pstd_read_data (uint16_t pipe, uint16_t pipemode);
void      usb_pstd_data_end (uint16_t pipe, uint16_t status);

uint8_t   usb_pstd_set_pipe_table (uint8_t  *descriptor);
void      usb_pstd_clr_pipe_table (void);
void      usb_pstd_set_pipe_reg (void);
void      usb_pstd_clr_pipe_reg (void);
uint8_t   usb_pstd_get_pipe_no (uint8_t type, uint8_t dir);


/* r_usb_cintfifo.c */
void      usb_pstd_nrdy_pipe_process(uint16_t bitsts);
void      usb_pstd_bemp_pipe_process(uint16_t bitsts);
void      usb_pstd_brdy_pipe_process(uint16_t bitsts);

/* r_usb_clibusbip.c */
void      usb_cstd_nrdy_enable (usb_utr_t *ptr, uint16_t pipe);
uint16_t  usb_cstd_get_pid (usb_utr_t *ptr, uint16_t pipe);
uint16_t  usb_cstd_port_speed (usb_utr_t *ptr);
uint16_t  usb_cstd_get_maxpacket_size (usb_utr_t *ptr, uint16_t pipe);
uint16_t  usb_cstd_get_pipe_dir (usb_utr_t *ptr, uint16_t pipe);
uint16_t  usb_cstd_get_pipe_type (usb_utr_t *ptr, uint16_t pipe);
void      usb_cstd_do_aclrm (usb_utr_t *ptr, uint16_t pipe);
void      usb_cstd_set_buf (usb_utr_t *ptr, uint16_t pipe);
void      usb_cstd_clr_stall (usb_utr_t *ptr, uint16_t pipe);
void      usb_cstd_usb_task (void);
void      usb_class_task (void);
void      usb_set_event (uint16_t event, usb_ctrl_t *ctrl);
void      usb_pin_setting (void);

uint16_t  usb_pstd_epadr2pipe (uint16_t Dir_Ep);
uint16_t  usb_pstd_pipe2fport (uint16_t pipe);
uint16_t  usb_pstd_hi_speed_enable (void);
void      usb_pstd_dummy_function (usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_pstd_dummy_trn (usb_setup_t * preq, uint16_t ctsq);

/* r_usb_creg_abs.c */
uint16_t  usb_cstd_is_set_frdy (usb_utr_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel);
void      usb_cstd_chg_curpipe (usb_utr_t *ptr, uint16_t pipe, uint16_t fifosel, uint16_t isel);
void      usb_cstd_set_transaction_counter (usb_utr_t *ptr, uint16_t trnreg, uint16_t trncnt);
void      usb_cstd_clr_transaction_counter (usb_utr_t *ptr, uint16_t trnreg);
void      usb_cstd_pipe_init (usb_utr_t *ptr, uint16_t pipe);
void      usb_cstd_clr_pipe_cnfg (usb_utr_t *ptr, uint16_t pipeno);
void      usb_cstd_set_nak (usb_utr_t *ptr, uint16_t pipe);
uint16_t  usb_cstd_get_buf_size (usb_utr_t *ptr, uint16_t pipe);

uint8_t  *usb_pstd_write_fifo (uint16_t count, uint16_t pipemode, uint8_t  * write_p);
uint8_t  *usb_pstd_read_fifo (uint16_t count, uint16_t pipemode, uint8_t  * read_p);
void      usb_pstd_forced_termination (uint16_t pipe, uint16_t status);
void      usb_pstd_interrupt_clock (void);
void      usb_pstd_self_clock (void);
void      usb_pstd_stop_clock (void);

/* r_usb_hscheduler.c */
usb_er_t  usb_cstd_snd_msg (uint8_t id, p_os_msg_t* mess);
usb_er_t  usb_cstd_rec_msg (uint8_t id, p_os_msg_t** mess, usb_tm_t tm);
usb_er_t  usb_cstd_isnd_msg (uint8_t id, p_os_msg_t* mess);

/* r_usb_pinthandler_usbip0.c */
void      usb_pstd_usb_handler (void);

/* r_usb_pdriver.c */
void      usb_pstd_pcd_task (void);
usb_er_t  usb_pstd_set_submitutr (usb_utr_t * utrmsg);
void      usb_pstd_clr_alt (void);
void      usb_pstd_clr_mem (void);
void      usb_pstd_set_config_num (uint16_t Value);
void      usb_pstd_clr_eptbl_index (void);
uint16_t  usb_pstd_get_interface_num (void);
uint16_t  usb_pstd_get_alternate_num (uint16_t int_num);
void      usb_pstd_set_eptbl_index (uint16_t int_num, uint16_t alt_num);
uint16_t  usb_pstd_chk_remote (void);
uint8_t   usb_pstd_get_current_power (void);

/* r_usb_pcontrolrw.c */
uint16_t  usb_pstd_ctrl_read (uint32_t bsize, uint8_t  *table);
void      usb_pstd_ctrl_write (uint32_t bsize, uint8_t  *table);
void      usb_pstd_ctrl_end (uint16_t status);

/* r_usb_pintfifo.c */
void      usb_pstd_brdy_pipe (uint16_t bitsts);
void      usb_pstd_nrdy_pipe (uint16_t bitsts);
void      usb_pstd_bemp_pipe (uint16_t bitsts);

/* r_usb_pstdfunction.c */
void      usb_pstd_busreset_function (void);
void      usb_pstd_suspend_function (void);
void      usb_p_registration (usb_ctrl_t *ctrl, usb_cfg_t *cfg);
void      usb_pdriver_init( usb_ctrl_t *ctrl, usb_cfg_t *cfg  );

/* r_usb_preg_abs.c */
void      usb_pstd_interrupt_handler (uint16_t * type, uint16_t * status);
void      usb_pstd_save_request (void);
uint16_t  usb_pstd_chk_configured (void);
void      usb_pstd_bus_reset (void);
void      usb_pstd_remote_wakeup (void);
void      usb_pstd_test_mode (void);
void      usb_pstd_attach_process (void);
void      usb_pstd_detach_process (void);
void      usb_pstd_suspend_process (void);
void      usb_pstd_resume_process (void);
uint16_t  usb_pstd_chk_vbsts (void);
void      usb_pstd_set_stall (uint16_t pipe);
void      usb_pstd_set_stall_pipe0 (void);

/* r_usb_pstdrequest.c */
void      usb_pstd_stand_req0 (void);
void      usb_pstd_stand_req1 (void);
void      usb_pstd_stand_req2 (void);
void      usb_pstd_stand_req3 (void);
void      usb_pstd_stand_req4 (void);
void      usb_pstd_stand_req5 (void);
void      usb_pstd_set_feature_function (void);

/* peri_processing.c */
void      usb_peri_registration(usb_ctrl_t *ctrl, usb_cfg_t *cfg);
void      usb_peri_devdefault (usb_utr_t *ptr, uint16_t mode, uint16_t data2);
uint16_t  usb_peri_pipe_info (uint8_t  *table, uint16_t speed, uint16_t length);
void      usb_peri_configured (usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_peri_detach (usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_peri_suspended(usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_peri_resume(usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_peri_interface(usb_utr_t *ptr, uint16_t data1, uint16_t data2);
void      usb_peri_class_request(usb_setup_t *preq, uint16_t ctsq);
void      usb_peri_class_request_ioss (usb_setup_t *req);
void      usb_peri_class_request_rwds (usb_setup_t * req);
void      usb_peri_class_request_wds (usb_setup_t * req);
void      usb_peri_other_request (usb_setup_t *req);
void      usb_peri_class_request_wnss (usb_setup_t *req);
void      usb_peri_class_request_rss (usb_setup_t *req);
void      usb_peri_class_request_wss (usb_setup_t *req);

#if defined(USB_CFG_PVND_USE)
extern void      usb_pvnd_read_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void      usb_pvnd_write_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);

#endif /* defined(USB_CFG_PVND_USE) */

#if defined(USB_CFG_PCDC_USE)
extern void      usb_pcdc_read_complete( usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void      usb_pcdc_write_complete( usb_utr_t *mess, uint16_t data1, uint16_t data2);

#endif /* defined(USB_CFG_PCDC_USE) */

#if defined(USB_CFG_PHID_USE)
extern void      usb_phid_read_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);
extern void      usb_phid_write_complete(usb_utr_t *mess, uint16_t data1, uint16_t data2);

#endif /* defined(USB_CFG_PHID_USE) */

#if defined(USB_CFG_PMSC_USE)
extern void      usb_pmsc_task (void);

#endif /* defined(USB_CFG_PMSC_USE) */

extern uint16_t g_usb_pstd_eptbl[];

#if defined(USB_CFG_PMSC_USE)
extern void      usb_pmsc_receive_cbw (void);
extern void      usb_pmsc_get_max_lun (uint16_t value, uint16_t index, uint16_t length);
extern void      usb_pmsc_mass_strage_reset (uint16_t value, uint16_t index, uint16_t length);
#endif


void      usb_pstd_device_information (usb_utr_t *ptr, uint16_t *tbl);
usb_er_t  usb_pstd_set_stall_clr_feature (usb_utr_t *ptr, usb_cb_t complete, uint16_t pipe);

#if USB_CFG_COMPLIANCE == USB_CFG_ENABLE
void      usb_compliance_disp (void *);

#endif /* USB_CFG_COMPLIANCE == USB_CFG_ENABLE */


/* r_usb_cstd_rtos.c */
void    usb_cstd_pipe_msg_clear (usb_utr_t *ptr, uint16_t pipe_no);
void    usb_cstd_pipe0_msg_clear (usb_utr_t *ptr, uint16_t dev_addr);
void    usb_cstd_pipe_msg_re_forward (uint16_t ip_no, uint16_t pipe_no);
void    usb_cstd_pipe0_msg_re_forward (uint16_t dev_addr);
void    usb_cstd_pipe_msg_forward (usb_utr_t *ptr, uint16_t pipe_no);
void    usb_cstd_pipe0_msg_forward (usb_utr_t *ptr, uint16_t dev_addr);

#endif  /* R_USB_EXTERN_H */
/******************************************************************************
 End  Of File
 ******************************************************************************/
