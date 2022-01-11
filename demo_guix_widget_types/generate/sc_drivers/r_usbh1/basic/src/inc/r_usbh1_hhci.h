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
 * Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_usbh1_hhci.h
 * Version      : 1.0
 * Description  : Header file USB HCI
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 15.01.2017 1.00     First Release
 **********************************************************************************************************************/

#ifndef R_USBH1_HHCI_H
#define R_USBH1_HHCI_H

#include "r_usbh1_basic_local.h"

/***********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 **********************************************************************************************************************/
#define USBH1_HCI_TRREQ_NUM             (USBH1_MAXPIPE)       /* Max Transfer Request Number */
#define USBH1_HCI_DEVICE_NUM            (USBH1_MAXDEVADDR)    /* Max Device Number */

#define USBH1_HCI_CONFIGDESC_BUFFSIZE   (USBH1_CONFIGSIZE)    /* Configuration Descriptor Buffer Size */

#define USBH1_HCI_HUBPORT_NUM           (USBH1_HUBDOWNPORT)   /* Hub port Number */


#define USBH1_HCI_HCTYPE_OHCI           (1u)        /* HCI type is OHCI */
#define USBH1_HCI_HCTYPE_EHCI           (2u)        /* HCI type is EHCI */

#define USBH1_HCI_DIRECTION_OUT         (0u)        /* Direction is output */
#define USBH1_HCI_DIRECTION_IN          (1u)        /* Direction is input */

#define USBH1_HCI_DEVSPEED_FS           (0u)        /* Full speed */
#define USBH1_HCI_DEVSPEED_LS           (1u)        /* Low speed */
#define USBH1_HCI_DEVSPEED_HS           (2u)        /* High speed */

#define USBH1_HCI_SETUP_DATA_SIZE       (8u)        /* SetUp data size */

#define USBH1_HCI_NODEVICE              (0xFF)      /* No Device Define */

#define USBH1_HCI_GET_ENDIAN            (0u)        /* Endian Setting */

#define RZA2_USBH1_OHCI_IP (1)

/***************************************(*******)*************************************************************************
 Typedef definitions
 **********************************************************************************************************************/
typedef struct
{
    uint8_t     blength;
    uint8_t     bdescriptor_type;
    uint16_t    bcd_usb;
    uint8_t     bdevice_class;
    uint8_t     bdevice_sub_class;
    uint8_t     bdevice_protocol;
    uint8_t     bmax_packet_size_0;
    uint16_t    id_vendor;
    uint16_t    id_product;
    uint16_t    bcd_device;
    uint8_t     imanufacturer;
    uint8_t     iproduct;
    uint8_t     iserial_number;
    uint8_t     bnum_configurations;
} st_usbh1_hci_device_descriptor_t;

typedef struct
{
    uint8_t     blength;
    uint8_t     bdescriptor_type;
    uint16_t    wTotalLength;
    uint8_t     bNumInterfaces;
    uint8_t     bConfigurationValue;
    uint8_t     iConfiguration;
    uint8_t     bmAttributes;
    uint8_t     bMaxPower;
} st_usbh1_hci_config_descriptor_t;

typedef struct
{
    uint8_t     blength;
    uint8_t     bdescriptor_type;
    uint8_t     bInterfaceNumber;
    uint8_t     bAlternateSetting;
    uint8_t     bNumEndpoints;
    uint8_t     bInterfaceClass;
    uint8_t     bInterfaceSubClass;
    uint8_t     bInterfaceProtocol;
    uint8_t     iInterface;
} st_usbh1_hci_interface_descriptor_t;

typedef struct
{
    uint8_t     blength;
    uint8_t     bdescriptor_type;
    uint8_t     bEndpointAddress;
    uint8_t     bmAttributes;
    uint16_t    wMaxPacketSize;
    uint8_t     bInterval;
} st_usbh1_hci_endpoint_descriptor_t;

typedef struct
{
    struct
    {
        uint32_t    devnum     : 7;             /* Device Number */
        uint32_t    disconnect_req : 1;         /* Disconnect Request Flag */
        uint32_t    hubaddr    : 7;             /* Hub Address */
        uint32_t    sprit_flag : 1;             /* Sprit Transaction Flag */
        uint32_t    hubportnum : 7;             /* Hub Port Number */
        uint32_t               : 1;
        uint32_t    hctype     : 2;             /* HCI Type */
        uint32_t    devspeed   : 2;             /* Device Speed */
        uint32_t               : 3;
        uint32_t    enable     : 1;             /* Enbale Flag */
    } bit;
    st_usbh1_hci_device_descriptor_t    devicedesc;                         /* Device Discriptor */
    uint32_t            rawconfigdesc[USBH1_HCI_CONFIGDESC_BUFFSIZE / 4];   /* Configuration Descriptor Raw Data */
    struct {
        uint8_t     devnum[USBH1_HCI_HUBPORT_NUM];      /* Device Number for HUB downport device */
        uint8_t     devspeed[USBH1_HCI_HUBPORT_NUM];    /* Device Speed for HUB downport device */
    } hubdevinfo;
    void                *hci_device;            /* HCI Characteristic Device Infomation pointer */
} st_usbh1_hci_device_t;

typedef struct
{
    struct
    {
        uint32_t    devadrs    : 7;             /* Device Address */
        uint32_t    cancel_req : 1;             /* Cancel Transfer Request Flag */
        uint32_t    epnum      : 7;             /* Endpoint Number */
        uint32_t    direction  : 1;             /* Direction */
        uint32_t    mps        : 13;            /* Max Packet Size */
        uint32_t    eptype     : 2;             /* Endpoint Type */
        uint32_t    enable     : 1;             /* Enable Flag */
    } bit;
    uint32_t                status;             /* Status */
    st_usbh1_hci_device_t   *devinfo;           /* Device Infomation Pointer */
    uint32_t                pollrate;           /* Polling rate(for interrupt/isochronous transfer) */
    uint32_t                *setupbuf;          /* SETUP Data Pointer(for control transfer only) */
    uint32_t                databuf;            /* Transfer Data Buffer */
    uint32_t                trsize;             /* Transfer Request Size */
    uint32_t                actual_size;        /* Actual Transfer Size */
    void                    *utr_p;             /* UTR data pointer */
    void                    *hci_info;          /* HCI Characteristic Infomation pointer */
} st_usbh1_hci_tr_req_t;

typedef struct
{
    void    (*attach_cb)(uint16_t port_no);             /* Attach Call Back Function Pointer */
    void    (*detach_cb)(uint16_t port_no);             /* Detach Call Back Function Pointer */
    void    (*over_current_cb)(uint16_t port_no);       /* Over Current Call Back Function Pointer */
    void    (*port_reset_cb)(uint16_t port_no);         /* Port Reset Call Back Function Pointer */
    void    (*tr_end_cb)(void *p_utr, uint32_t actual_size, uint16_t status);   /* Transfer End Call Back Function Pointer */
} st_usbh1_hci_cb_info_t;


/***********************************************************************************************************************
 Exported global variables
 **********************************************************************************************************************/
extern uint32_t usbh1_hcrh_port_status1;
extern uint32_t usbh1_hcrh_descriptor_a;
extern uint32_t usbh1_hcrh_descriptor_b;
extern uint32_t usbh1_portsc1;


/***********************************************************************************************************************
 Exported global functions (to be accessed by other files)
 **********************************************************************************************************************/
/* function */
void                    r_usbh1_hstd_hci_task(void);
int32_t                 r_usbh1_hstd_hci_init(st_usbh1_hci_cb_info_t *cb_info);
uint16_t                r_usbh1_hstd_hci_make_transfer_request(void* utr_p, void* setup, uint32_t devaddr, uint32_t epnum,
                                    uint32_t eptype, uint32_t epdir, uint32_t tranlen, uint32_t tranadr, uint32_t mps);
void                    r_usbh1_hstd_hci_cancel_transfer_request(uint32_t devaddr, uint32_t epnum,
                                                                    uint32_t eptype, uint32_t epdir);
void                    r_usbh1_hstd_hci_disconnect(uint32_t devadd);
uint16_t                r_usbh1_hstd_hci_get_device_address_of_rootpoot(uint16_t rootport);
uint16_t                r_usbh1_hstd_hci_set_device_address_of_rootpoot(uint16_t rootport, uint16_t devaddr);
uint32_t                r_usbh1_hstd_hci_get_device_speed(uint16_t devaddr);
st_usbh1_hci_device_t   *r_usbh1_hstd_hci_get_device_infomation(uint16_t devaddr);
void                    r_usbh1_hstd_hci_port_reset(uint16_t rootport);
void                    r_usbh1_hstd_hci_port_suspend(uint16_t rootport);
void                    r_usbh1_hstd_hci_port_resume(uint16_t rootport);
void                    r_usbh1_hstd_hci_set_hub_info(uint32_t hubaddr, uint32_t hubportnum,
                                                        uint32_t devaddr, uint32_t devspeed);
void                    r_usbh1_hstd_hci_set_device_address_of_hub_port(uint16_t hubaddr, uint16_t devaddr);
void                    r_usbh1_hstd_hci_electrical_test(uint16_t rootport, uint8_t mode);
void                    r_usbh1_hstd_hci_wait_time(uint32_t ms);


void                    r_usbh1_module_init(void);
void                    r_usbh1_tusb320_host_setting(void);

#endif  /* #ifndef R_USBH1_HHCI_H */
