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
* File Name    : r_usbh0_hmanager.c
* Description  : USB Host Control Manager
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*         : 31.08.2015 1.00    First Release
***********************************************************************************************************************/


/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <string.h>
#include "r_usbh0_basic_local.h"
#include "r_usbh0_hhci.h"

/***********************************************************************************************************************
 Macro definitions
***********************************************************************************************************************/
#define CLS_DATA_SIZE                   (512u)

/* Device state define */
#define USBH0_NONDEVICE                 (0u)
#define USBH0_NOTTPL                    (1u)
#define USBH0_ATTACHDEVICE              (2u)
#define USBH0_DEVICEENUMERATION         (3u)
#define USBH0_DEVICEADDRESSED           (4u)
#define USBH0_DEVICECONFIGURED          (5u)
#define USBH0_COMPLETEPIPESET           (10u)
#define USBH0_DEVICESUSPENDED           (20u)
#define USBH0_ELECTRICALTEST            (30u)

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
st_usbh0_hcdreg_t   g_usbh0_hstd_device_drv[USBH0_MAXDEVADDR + 1u];     /* Device driver (registration) */
uint16_t            g_usbh0_hstd_mgr_mode[1u];                          /* Manager mode */
uint16_t            g_usbh0_hstd_device_addr;                           /* Device address */
uint16_t            g_usbh0_hstd_device_speed;                          /* reset handshake result */
uint16_t            g_usbh0_hstd_device_num;                            /* Device driver number */
uint16_t            g_usbh0_hstd_check_enu_result;

#ifdef USBH0_HOST_COMPLIANCE_MODE
uint8_t             g_usbh0_hstd_test_packet_parameter_flag = 0;
#endif /* USBH0_HOST_COMPLIANCE_MODE */

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
#ifdef __ICCARM__
#pragma location="USB_HOST_0"

#endif  /* __ICCARM__ */

#ifdef __GNUC__
static uint16_t         gs_usbh0_hstd_device_descriptor[USBH0_DEVICESIZE / 2u] __attribute__ ((section ("USB_HOST_0")));
static uint16_t         gs_usbh0_hstd_configuration_descriptor[USBH0_CONFIGSIZE / 2u] __attribute__ ((section ("USB_HOST_0")));
static uint8_t          gs_usbh0_hstd_class_data[CLS_DATA_SIZE] __attribute__ ((section ("USB_HOST_0")));
static st_usbh0_setup_t gs_usbh0_hstd_class_request __attribute__ ((section ("USB_HOST_0")));   /* bRequest/wValue/wIndex/wLength/Data */
static st_usbh0_setup_t gs_usbh0_hstd_std_request __attribute__ ((section ("USB_HOST_0")));     /* bRequest/wValue/wIndex/wLength/Data */
static uint16_t         gs_usbh0_hstd_dummy_data __attribute__ ((section ("USB_HOST_0")));
static uint16_t         gs_usbh0_hstd_enum_seq __attribute__ ((section ("USB_HOST_0")));   /* Enumeration request */
static st_usbh0_utr_t   gs_usbh0_hstd_class_control __attribute__ ((section ("USB_HOST_0")));
static st_usbh0_utr_t   gs_usbh0_hstd_std_req_msg __attribute__ ((section ("USB_HOST_0")));
static uint16_t         gs_usbh0_hstd_suspend_seq __attribute__ ((section ("USB_HOST_INIT_0"))) = 0;
static uint16_t         gs_usbh0_hstd_resume_seq __attribute__ ((section ("USB_HOST_INIT_0"))) = 0;
#endif  /* __GNUC__ */

#ifdef __CC_ARM
#pragma arm section zidata = "USB_HOST_0"

#endif  /* __CC_ARM */


static uint16_t         usbh0_hstd_enumeration(uint16_t result);
static void             usbh0_hstd_enumeration_err(uint16_t EnumSeq);
static uint16_t         usbh0_hstd_chk_device_class(st_usbh0_hcdreg_t *p_driver, uint16_t port);
static void             usbh0_hstd_submit_result(st_usbh0_utr_t *p_utr);
static void             usbh0_hstd_enum_set_address(uint16_t addr, uint16_t setaddr);
static void             usbh0_hstd_enum_set_configuration(uint16_t addr, uint16_t confnum);
static void             usbh0_hstd_mgr_suspend(uint16_t devaddr);
static void             usbh0_hstd_mgr_resume(uint16_t devaddr);
static void             usbh0_hstd_susp_cont(uint16_t devaddr, uint16_t result);
static void             usbh0_hstd_resu_cont(uint16_t devaddr, uint16_t result);
static uint16_t         usbh0_hstd_cmd_submit(usbh0_utr_cb_t complete);
static uint16_t         usbh0_hstd_get_config_desc(uint16_t addr, uint16_t length, usbh0_utr_cb_t complete);
static uint16_t         usbh0_hstd_std_req_check(uint16_t err_check);

/***********************************************************************************************************************
Function Name   : usbh0_hstd_enumeration
Description     : Execute enumeration on the connected USB device.
Arguments       : result         : Control Transfer result
Return          : uint16_t       : Enumeration status.
***********************************************************************************************************************/
static uint16_t usbh0_hstd_enumeration(uint16_t result)
{
    uint16_t                retval;
    uint16_t                md;
    st_usbh0_hcdreg_t       *p_driver;
    uint16_t                enume_mode;             /* Enumeration mode (device state) */
    uint8_t                 *p_descriptor_table;
    uint16_t                rootport;
    st_usbh0_hci_device_t   *p_dev_info;

    static uint32_t i;

#ifdef USBH0_HOST_COMPLIANCE_MODE
    uint16_t                vendor_id;
    uint16_t                product_id;
#endif  /* USBH0_HOST_COMPLIANCE_MODE */

    /* Attach Detect Mode */
    enume_mode = USBH0_NONDEVICE;

    /* Get root port number from device addr */
    rootport = usbh0_hstd_get_rootport(g_usbh0_hstd_device_addr);

    /* Manager Mode Change */
    switch (result)
    {
        case USBH0_CTRL_END:
            enume_mode = USBH0_DEVICEENUMERATION;
            switch(gs_usbh0_hstd_enum_seq)
            {
                case 0:     /* Receive Device Descriptor */
                    usbh0_hstd_enum_set_address((uint16_t)USBH0_DEVICE_0, g_usbh0_hstd_device_addr);
                break;

                case 1:     /* Set Address Result */
                    /* Device descriptor information is copied */
                    p_dev_info = r_usbh0_hstd_hci_get_device_infomation(g_usbh0_hstd_device_addr);
                    memcpy(&p_dev_info->devicedesc,gs_usbh0_hstd_device_descriptor,sizeof(p_dev_info->devicedesc));

                    /* Set device speed */
                    usbh0_hstd_set_rootport(g_usbh0_hstd_device_addr, rootport);

                    /* Get Descriptor */
                    usbh0_hstd_enum_get_descriptor(g_usbh0_hstd_device_addr, gs_usbh0_hstd_enum_seq);
                break;

                case 2:     /* Receive Device Descriptor(18) */
                    usbh0_hstd_enum_get_descriptor(g_usbh0_hstd_device_addr, gs_usbh0_hstd_enum_seq);
                break;

                case 3:     /* Receive Configuration Descriptor(9) */
                    usbh0_hstd_enum_get_descriptor(g_usbh0_hstd_device_addr, gs_usbh0_hstd_enum_seq);
                break;

                case 4:     /* Receive Configuration Descriptor(xx) */
#ifdef USBH0_HOST_COMPLIANCE_MODE
                    p_descriptor_table = (uint8_t*)gs_usbh0_hstd_device_descriptor;
                    /* If 'vendor_id' and 'product_id' value is defined by PET, */
                    /*    system works for compliance test mode. */
                    /* Values ??defined by PET is the following. */
                    /* vendor_id : 0x1A0A  */
                    /* product_id : 0x0101 - 0x0108 , 0x0200 */

                    vendor_id = (uint16_t)(p_descriptor_table[USBH0_DEV_ID_VENDOR_L]
                              + ((uint16_t)p_descriptor_table[USBH0_DEV_ID_VENDOR_H] << 8));

                    if (0x1A0A == vendor_id)
                    {
                        product_id = (uint16_t)(p_descriptor_table[USBH0_DEV_ID_PRODUCT_L]
                                   + ((uint16_t)p_descriptor_table[USBH0_DEV_ID_PRODUCT_H] << 8));

                        if ((product_id >= 0x0101) && (product_id <= 0x0108))
                        {
                            gs_usbh0_hstd_enum_seq = 6;  /* Enumeration End */
                            usbh0_hstd_electrical_test_mode(product_id, rootport);
                            enume_mode = USBH0_NOTTPL;
                            break;
                        }

                        if (0x0200 == product_id)
                        {
                            gs_usbh0_hstd_enum_seq = 5;  /* Skip EnumSeq 5 */
                            p_descriptor_table = (uint8_t*)gs_usbh0_hstd_configuration_descriptor;
                            usbh0_hstd_enum_set_configuration(g_usbh0_hstd_device_addr,
                                                    (uint16_t)(p_descriptor_table[USBH0_CON_B_CONFIGURATION_VALUE]));
                            break;
                        }
                    }
#endif  /* USBH0_HOST_COMPLIANCE_MODE */

                    /* Device descriptor and Config descriptor information is copied. */
                    p_dev_info = r_usbh0_hstd_hci_get_device_infomation(g_usbh0_hstd_device_addr);

                    /* Device Descriptor copy */
                    memcpy(&p_dev_info->devicedesc, gs_usbh0_hstd_device_descriptor, sizeof(p_dev_info->devicedesc));

                    /* Configuration Descroptor copy */
                    memcpy(&p_dev_info->rawconfigdesc,
                            gs_usbh0_hstd_configuration_descriptor,
                            sizeof(p_dev_info->rawconfigdesc));

                    /* Driver open */
                    retval = USBH0_ERROR;
                    for (md = 0; md < g_usbh0_hstd_device_num; md++)
                    {
                        p_driver = &g_usbh0_hstd_device_drv[md];
                        if (USBH0_DETACHED == p_driver->devstate)
                        {
                            /* In this function, check device class of enumeration flow move to class */
                            retval = usbh0_hstd_chk_device_class(p_driver, rootport);

                            /* "R_USBH0_HstdReturnEnuMGR()" is used to return */
                            if (USBH0_OK == retval)
                            {
                                p_driver->rootport = rootport;
                                p_driver->devaddr  = g_usbh0_hstd_device_addr;
                                break;
                            }
                        }
                    }

                    if (USBH0_ERROR == retval)
                    {
#ifdef USBH0_HOST_COMPLIANCE_MODE
                        gs_usbh0_hstd_enum_seq = 6;                          /* Enumeration End */
                        enume_mode = USBH0_NOTTPL;
#else /* USBH0_HOST_COMPLIANCE_MODE */
                        gs_usbh0_hstd_enum_seq = 5;                          /* Skip EnumSeq 5 */
                        p_descriptor_table = (uint8_t*)gs_usbh0_hstd_configuration_descriptor;
                        usbh0_hstd_enum_set_configuration(g_usbh0_hstd_device_addr,
                                                        (uint16_t)(p_descriptor_table[USBH0_CON_B_CONFIGURATION_VALUE]));
#endif /* USBH0_HOST_COMPLIANCE_MODE */
                    }
                break;

                case 5:     /* Class Check Result */
                    switch(g_usbh0_hstd_check_enu_result)
                    {
                        case USBH0_OK:
                            p_descriptor_table = (uint8_t*)gs_usbh0_hstd_configuration_descriptor;
                            usbh0_hstd_enum_set_configuration(g_usbh0_hstd_device_addr,
                                                            (uint16_t)(p_descriptor_table[USBH0_CON_B_CONFIGURATION_VALUE]));
                        break;

                        case USBH0_ERROR:
                            enume_mode = USBH0_NOTTPL;
                        break;

                        default:
                            enume_mode = USBH0_NONDEVICE;
                        break;
                    }
                break;

                case 6:     /* Set Configuration Result */
                    /* Device enumeration function */
                    /* USBH0_PRINTF0(" Configured Device\n"); */
                    for (md = 0; md < g_usbh0_hstd_device_num; md++)
                    {
                        p_driver = &g_usbh0_hstd_device_drv[md];
                        if (g_usbh0_hstd_device_addr == p_driver->devaddr)
                        {
                            p_driver->devstate = USBH0_CONFIGURED;
                            (*p_driver->devconfig)(g_usbh0_hstd_device_addr);   /* Call Back */
                            break;
                        }
                    }
                    enume_mode = USBH0_COMPLETEPIPESET;
                break;

                default:
                break;
            }

            gs_usbh0_hstd_enum_seq++;
        break;

        case USBH0_DATA_ERR:
            USBH0_PRINTF0("### Enumeration is stoped(SETUP or DATA-ERROR)\n");
            usbh0_hstd_enumeration_err(gs_usbh0_hstd_enum_seq);
        break;

        case USBH0_DATA_OVR:
            USBH0_PRINTF0("### Enumeration is stoped(receive data over)\n");
            usbh0_hstd_enumeration_err(gs_usbh0_hstd_enum_seq);
        break;

        case USBH0_DATA_STALL:
            USBH0_PRINTF0("### Enumeration is stoped(SETUP or DATA-STALL)\n");
            usbh0_hstd_enumeration_err(gs_usbh0_hstd_enum_seq);
        break;

        default:
            USBH0_PRINTF0("### Enumeration is stoped(result error)\n");
            usbh0_hstd_enumeration_err(gs_usbh0_hstd_enum_seq);
        break;
    }
    return enume_mode;
} /* End of function usbh0_hstd_enumeration() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_enumeration_err
Description     : Output error information when enumeration error occurred.
Argument        : uint16_t enum_seq             : enumeration sequence
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_enumeration_err(uint16_t enum_seq)
{
    switch (enum_seq)
    {
        case 0:
            USBH0_PRINTF0(" Get_DeviceDescrip(8)\n");
        break;

        case 1:
            USBH0_PRINTF0(" Set_Address\n");
        break;

        case 2:
            USBH0_PRINTF0(" Get_DeviceDescrip(18)\n");
        break;

        case 3:
            USBH0_PRINTF0(" Get_ConfigDescrip(9)\n");
        break;

        case 4:
            USBH0_PRINTF0(" Get_ConfigDescrip(xx)\n");
        break;

        case 5:
            USBH0_PRINTF0(" Get_DeviceDescrip(8-2)\n");
        break;

        case 6:
            USBH0_PRINTF0(" Set_Configuration\n");
        break;

        default:
        break;
    }
} /* End of function usbh0_hstd_enumeration_err() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_chk_device_class
Description     : Interface class search
Arguments       : st_usbh0_hcdreg_t *p_driver      : Class driver
                : uint16_t port             : Port no.
Return          : uint16_t                  : USBH0_OK / USBH0_ERROR
***********************************************************************************************************************/
static uint16_t usbh0_hstd_chk_device_class(st_usbh0_hcdreg_t *p_driver, uint16_t port)
{
    uint8_t     *p_descriptor_table;
    uint16_t    total_length1;
    uint16_t    total_length2;
    uint16_t    result;
    uint16_t    hub_device;
    uint16_t    *p_table[9];
#ifdef USBH0_HOST_COMPLIANCE_MODE
    uint16_t    vendor_id;
    uint16_t    product_id;
    uint16_t    id_check;
    uint16_t    i;
#endif /* USBH0_HOST_COMPLIANCE_MODE */

    p_descriptor_table = (uint8_t*)gs_usbh0_hstd_device_descriptor;

#ifdef USBH0_HOST_COMPLIANCE_MODE
    id_check = USBH0_ERROR;
    for (i = 0; i < p_driver->p_tpl[0]; i++)
    {
        vendor_id = (uint16_t)(p_descriptor_table[USBH0_DEV_ID_VENDOR_L]
                        + ((uint16_t)p_descriptor_table[USBH0_DEV_ID_VENDOR_H] << 8));

        if ((USBH0_NOVENDOR == p_driver->p_tpl[(i * 2) + 2]) || (vendor_id == p_driver->tpl[(i * 2) + 2]))
        {
            product_id = (uint16_t)(p_descriptor_table[USBH0_DEV_ID_PRODUCT_L]
                            + ((uint16_t)p_descriptor_table[USBH0_DEV_ID_PRODUCT_H] << 8));

            if ((USBH0_NOPRODUCT == p_driver->p_tpl[(i * 2) + 3]) || (product_id == p_driver->tpl[(i * 2) + 3]))
            {
                id_check = USBH0_OK;
                USBH0_COMPLIANCE_DISP(USBH0_COMP_TPL, USBH0_NO_ARG);
                USBH0_COMPLIANCE_DISP(USBH0_COMP_VID, vendor_id);
                USBH0_COMPLIANCE_DISP(USBH0_COMP_PID, product_id);
            }
        }
    }

    if (USBH0_ERROR == id_check)
    {
        USBH0_PRINTF0("### Not support device\n");
        if (USBH0_IFCLS_HUB == p_descriptor_table[USBH0_DEV_B_DEVICE_CLASS])
        {
            USBH0_COMPLIANCE_DISP(USBH0_COMP_HUB, vendor_id);
        }
        else
        {
            USBH0_COMPLIANCE_DISP(USBH0_COMP_NOTTPL, vendor_id);
        }

        return USBH0_ERROR;
    }
#endif /* USBH0_HOST_COMPLIANCE_MODE */

    p_descriptor_table = (uint8_t*)gs_usbh0_hstd_configuration_descriptor;
    total_length1 = 0;
    total_length2 = (uint16_t)(p_descriptor_table[USBH0_CON_W_TOTAL_LENGTH_L]
                     + ((uint16_t)p_descriptor_table[USBH0_CON_W_TOTAL_LENGTH_H] << 8));

    if (total_length2 > USBH0_CONFIGSIZE)
    {
        total_length2 = USBH0_CONFIGSIZE;
    }

    /* Search within configuration total-length */
    while (total_length1 < total_length2)
    {
        switch (p_descriptor_table[total_length1 + 1])
        {
            /* Configuration Descriptor ? */
            case USBH0_DT_CONFIGURATION:
                p_table[1] = (uint16_t *)&p_descriptor_table[total_length1];
            break;

            /* Interface Descriptor ? */
            case USBH0_DT_INTERFACE:
                if (p_driver->ifclass == (uint16_t)p_descriptor_table[total_length1 + 5])
                {
                    result      = USBH0_ERROR;
                    p_table[0]  = (uint16_t*)&gs_usbh0_hstd_device_descriptor;
                    p_table[2]  = (uint16_t*)&p_descriptor_table[total_length1];
                    p_table[3]  = &result;
                    p_table[4]  = &hub_device;
                    p_table[5]  = &port;
                    p_table[6]  = &g_usbh0_hstd_device_speed;
                    p_table[7]  = &g_usbh0_hstd_device_addr;

                    /* ClassCheck Callback */
                    (*p_driver->classcheck)((uint16_t**)&p_table);

                    return result;
                }
                else
                {
                    USBH0_PRINTF2("*** Interface class is 0x%02x (not 0x%02x)\n",
                                    p_descriptor_table[total_length1 + 5], p_driver->ifclass);
                }
            break;

            default:
            break;
        }

        total_length1 += p_descriptor_table[total_length1];
        if (0 == p_descriptor_table[total_length1])
        {
            break;
        }
    }

    return USBH0_ERROR;
} /* End of function usbh0_hstd_chk_device_class() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_submit_result
Description     : Callback after completion of a standard request.
Argument        : uint16_t port     : Port no.
                : uint16_t result   : Result.
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_submit_result(st_usbh0_utr_t *p_utr)
{
#ifdef USBH0_HOST_COMPLIANCE_MODE
    g_usbh0_hstd_test_packet_parameter_flag = 0;
#endif /* USBH0_HOST_COMPLIANCE_MODE */

    usbh0_hstd_mgr_snd_mbx((uint16_t)USBH0_MSG_MGR_SUBMITRESULT, p_utr->p_setup->devaddr, p_utr->result);
} /* End of function usbh0_hstd_submit_result() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_enum_get_descriptor
Description     : Send GetDescriptor to the connected USB device.
Argument        : uint16_t addr             : Device Address
                : uint16_t cnt_value        : Enumeration sequence
Return          : none
***********************************************************************************************************************/
void usbh0_hstd_enum_get_descriptor(uint16_t addr, uint16_t cnt_value)
{
    uint8_t     *p_data_table;

    switch (cnt_value)
    {
        case 0:
            gs_usbh0_hstd_enum_seq = 0;
            /* continue */
        case 4:
            gs_usbh0_hstd_std_request.type      = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
            gs_usbh0_hstd_std_request.value     = (uint16_t)USBH0_DEV_DESCRIPTOR;
            gs_usbh0_hstd_std_request.index     = (uint16_t)0x0000;
            gs_usbh0_hstd_std_request.length    = (uint16_t)0x0040;
            if (gs_usbh0_hstd_std_request.length > USBH0_DEVICESIZE)
            {
                gs_usbh0_hstd_std_request.length = USBH0_DEVICESIZE;
            }
            gs_usbh0_hstd_std_req_msg.p_tranadr = gs_usbh0_hstd_device_descriptor;
        break;

        case 1:
            gs_usbh0_hstd_std_request.type      = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
            gs_usbh0_hstd_std_request.value     = (uint16_t)USBH0_DEV_DESCRIPTOR;
            gs_usbh0_hstd_std_request.index     = (uint16_t)0x0000;
            gs_usbh0_hstd_std_request.length    = (uint16_t)0x0012;
            if (gs_usbh0_hstd_std_request.length > USBH0_DEVICESIZE)
            {
                gs_usbh0_hstd_std_request.length = USBH0_DEVICESIZE;
            }
            gs_usbh0_hstd_std_req_msg.p_tranadr = gs_usbh0_hstd_device_descriptor;
        break;

        case 2:
            gs_usbh0_hstd_std_request.type      = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
            gs_usbh0_hstd_std_request.value     = (uint16_t)USBH0_CONF_DESCRIPTOR;
            gs_usbh0_hstd_std_request.index     = (uint16_t)0x0000;
            gs_usbh0_hstd_std_request.length    = (uint16_t)0x0009;
            gs_usbh0_hstd_std_req_msg.p_tranadr = gs_usbh0_hstd_configuration_descriptor;
        break;

        case 3:
            p_data_table = (uint8_t*)gs_usbh0_hstd_configuration_descriptor;
            gs_usbh0_hstd_std_request.type      = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
            gs_usbh0_hstd_std_request.value     = (uint16_t)USBH0_CONF_DESCRIPTOR;
            gs_usbh0_hstd_std_request.index     = (uint16_t)0x0000;
            gs_usbh0_hstd_std_request.length    = (uint16_t)(((uint16_t)p_data_table[3] << 8) + (uint16_t)p_data_table[2]);
            if (gs_usbh0_hstd_std_request.length > USBH0_CONFIGSIZE)
            {
                gs_usbh0_hstd_std_request.length = USBH0_CONFIGSIZE;
                USBH0_PRINTF0("***WARNING Descriptor size over !\n");
            }
            gs_usbh0_hstd_std_req_msg.p_tranadr = gs_usbh0_hstd_configuration_descriptor;
        break;

        default:
            return;
        break;
    }

    gs_usbh0_hstd_std_request.devaddr = addr;

    gs_usbh0_hstd_std_req_msg.keyword   = (uint16_t)USBH0_PIPE0;
    gs_usbh0_hstd_std_req_msg.tranlen   = gs_usbh0_hstd_std_request.length;
    gs_usbh0_hstd_std_req_msg.p_setup   = &gs_usbh0_hstd_std_request;
    gs_usbh0_hstd_std_req_msg.complete  = &usbh0_hstd_submit_result;

    usbh0_hstd_transfer_start(&gs_usbh0_hstd_std_req_msg);
} /* End of function usbh0_hstd_enum_get_descriptor() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_enum_set_address
Description     : Send SetAddress to the connected USB device.
Arguments       : uint16_t addr     : Device Address.
                : uint16_t setaddr  : New address.
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_enum_set_address(uint16_t addr, uint16_t set_addr)
{
    gs_usbh0_hstd_std_request.type      = USBH0_SET_ADDRESS | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_DEVICE;
    gs_usbh0_hstd_std_request.value     = set_addr;
    gs_usbh0_hstd_std_request.index     = 0x0000;
    gs_usbh0_hstd_std_request.length    = 0x0000;
    gs_usbh0_hstd_std_request.devaddr   = addr;

    gs_usbh0_hstd_std_req_msg.keyword   = (uint16_t)USBH0_PIPE0;
    gs_usbh0_hstd_std_req_msg.p_tranadr = (void *)&gs_usbh0_hstd_dummy_data;
    gs_usbh0_hstd_std_req_msg.tranlen   = gs_usbh0_hstd_std_request.length;
    gs_usbh0_hstd_std_req_msg.p_setup   = &gs_usbh0_hstd_std_request;
    gs_usbh0_hstd_std_req_msg.complete  = &usbh0_hstd_submit_result;

    usbh0_hstd_transfer_start(&gs_usbh0_hstd_std_req_msg);
} /* End of function usbh0_hstd_enum_set_address() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_enum_set_configuration
Description     : Send SetConfiguration to the connected USB device.
Argument        : uint16_t addr     : Device Address.
                : uint16_t confnum  : Configuration number.
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_enum_set_configuration(uint16_t addr, uint16_t confnum)
{
    gs_usbh0_hstd_std_request.type    = USBH0_SET_CONFIGURATION | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_DEVICE;
    gs_usbh0_hstd_std_request.value   = confnum;
    gs_usbh0_hstd_std_request.index   = 0x0000;
    gs_usbh0_hstd_std_request.length  = 0x0000;
    gs_usbh0_hstd_std_request.devaddr = addr;

    gs_usbh0_hstd_std_req_msg.keyword   = (uint16_t)USBH0_PIPE0;
    gs_usbh0_hstd_std_req_msg.p_tranadr = (void *)&gs_usbh0_hstd_dummy_data;
    gs_usbh0_hstd_std_req_msg.tranlen   = gs_usbh0_hstd_std_request.length;
    gs_usbh0_hstd_std_req_msg.p_setup   = &gs_usbh0_hstd_std_request;
    gs_usbh0_hstd_std_req_msg.complete  = &usbh0_hstd_submit_result;

    usbh0_hstd_transfer_start(&gs_usbh0_hstd_std_req_msg);
} /* End of function usbh0_hstd_enum_set_configuration() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_mgr_suspend
Description     : Suspend request
Argument        : uint16_t dev_addr  : Device address.
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_mgr_suspend(uint16_t dev_addr)
{
    uint16_t    rootport;

    rootport = usbh0_hstd_get_rootport(dev_addr);

    if (USBH0_NOPORT != rootport)
    {
        g_usbh0_hstd_mgr_mode[rootport] = USBH0_SUSPENDED_PROCESS;
        gs_usbh0_hstd_suspend_seq = 0;
        usbh0_hstd_susp_cont(dev_addr, 0);
    }
} /* End of function usbh0_hstd_mgr_suspend() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_mgr_resume
Description     : Request HCD (Host Control Device) to send RESUME signal.
Argument        : uint16_t dev_addr  : Device address.
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_mgr_resume(uint16_t dev_addr)
{
    uint16_t    rootport;

    rootport = usbh0_hstd_get_rootport(dev_addr);

    if (USBH0_NOPORT != rootport)
    {
        g_usbh0_hstd_mgr_mode[rootport] = USBH0_RESUME_PROCESS;
        gs_usbh0_hstd_resume_seq = 0;
        r_usbh0_hstd_hci_port_resume(rootport);
        usbh0_hstd_resu_cont(dev_addr, 0);
    }
} /* End of function usbh0_hstd_mgr_resume() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_susp_cont
Description     : Suspend the connected USB Device (Function for nonOS)
Arguments       : uint16_t dev_addr          : Device Address
                : uint16_t result           : Transfer Result
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_susp_cont(uint16_t dev_addr, uint16_t result)
{
    uint16_t            md;
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            rootport;
    uint16_t            err;

    rootport = usbh0_hstd_get_rootport(dev_addr);

    switch (gs_usbh0_hstd_suspend_seq)
    {
        case 0:
            usbh0_hstd_get_config_desc(dev_addr, (uint16_t)0x09, (usbh0_utr_cb_t)&usbh0_hstd_submit_result);
            gs_usbh0_hstd_suspend_seq++;
        break;

        case 1:
            err = usbh0_hstd_std_req_check(result);
            if (USBH0_OK == err)
            {
                /* Check Remote Wakeup */
                if (USBH0_CF_RWUPON == (gs_usbh0_hstd_class_data[USBH0_CON_BM_ATTRIBUTES] & USBH0_CF_RWUPON))
                {
                    usbh0_hstd_set_feature(dev_addr, (uint16_t)0xFF, (usbh0_utr_cb_t)&usbh0_hstd_submit_result);
                    gs_usbh0_hstd_suspend_seq++;
                }
                else
                {
                    USBH0_PRINTF0("### Remote wakeup disable\n");
                    r_usbh0_hstd_hci_port_suspend(rootport);
                    g_usbh0_hstd_mgr_mode[rootport] = USBH0_SUSPENDED;
                }
            }
        break;

        case 2:
            err = usbh0_hstd_std_req_check(result);
            if (USBH0_OK == err)
            {
                r_usbh0_hstd_hci_port_suspend(rootport);
                g_usbh0_hstd_mgr_mode[rootport] = USBH0_SUSPENDED;
            }
        break;

        default:
        break;
    }

    if (USBH0_SUSPENDED == g_usbh0_hstd_mgr_mode[rootport])
    {
        for (md = 0; md < g_usbh0_hstd_device_num; md++)
        {
            p_driver = &g_usbh0_hstd_device_drv[md];
            if (p_driver->devaddr == (rootport + USBH0_DEVICEADDR))
            {
                (*p_driver->devsuspend)(p_driver->devaddr);

                /* Device state */
                p_driver->devstate = USBH0_SUSPENDED;
            }
        }
    }
} /* End of function usbh0_hstd_susp_cont() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_resu_cont
Description     : Resume the connected USB Device (Function for nonOS)
Argument        : uint16_t dev_addr          : Device Address
                : uint16_t result           : Transfer Result
Return          : none
***********************************************************************************************************************/
static void usbh0_hstd_resu_cont(uint16_t dev_addr, uint16_t result)
{
    uint16_t            md;
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            rootport;
    uint16_t            err;

    rootport = usbh0_hstd_get_rootport(dev_addr);

    switch (gs_usbh0_hstd_resume_seq)
    {
        case 0:
            usbh0_hstd_get_config_desc(dev_addr, (uint16_t)0x09, (usbh0_utr_cb_t)&usbh0_hstd_submit_result);
            gs_usbh0_hstd_resume_seq++;
        break;

        case 1:
            err = usbh0_hstd_std_req_check(result);
            if (USBH0_OK == err)
            {
                /* Check Remote Wakeup */
                if (USBH0_CF_RWUPON == (gs_usbh0_hstd_class_data[USBH0_CON_BM_ATTRIBUTES] & USBH0_CF_RWUPON))
                {
                    usbh0_hstd_clear_feature(dev_addr, (uint16_t)0xFF, (usbh0_utr_cb_t)&usbh0_hstd_submit_result);
                    gs_usbh0_hstd_resume_seq++;
                }
                else
                {
                    g_usbh0_hstd_mgr_mode[rootport] = USBH0_CONFIGURED;
                }
            }
        break;

        case 2:
            err = usbh0_hstd_std_req_check(result);
            if (USBH0_OK == err)
            {
                g_usbh0_hstd_mgr_mode[rootport] = USBH0_CONFIGURED;
            }
        break;

        default:
        break;
    }

    if (USBH0_CONFIGURED == g_usbh0_hstd_mgr_mode[rootport])
    {
        for (md = 0; md < g_usbh0_hstd_device_num; md++)
        {
            p_driver  = &g_usbh0_hstd_device_drv[md];
            if (p_driver->devaddr == (rootport+USBH0_DEVICEADDR))
            {
                (*p_driver->devresume)(p_driver->devaddr);

                /* Device state */
                p_driver->devstate = USBH0_CONFIGURED;
            }
        }
    }
} /* End of function usbh0_hstd_resu_cont() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_cmd_submit
Description     : command submit
Arguments       : usbh0_utr_cb_t complete     : callback info
Return value    : uint16_t                  : TransferStart Result
***********************************************************************************************************************/
static uint16_t usbh0_hstd_cmd_submit(usbh0_utr_cb_t complete)
{
    gs_usbh0_hstd_class_control.p_tranadr   = (void*)gs_usbh0_hstd_class_data;
    gs_usbh0_hstd_class_control.complete    = complete;
    gs_usbh0_hstd_class_control.tranlen     = (uint32_t)gs_usbh0_hstd_class_request.length;
    gs_usbh0_hstd_class_control.keyword     = USBH0_PIPE0;
    gs_usbh0_hstd_class_control.p_setup     = &gs_usbh0_hstd_class_request;

    return usbh0_hstd_transfer_start(&gs_usbh0_hstd_class_control);
} /* End of function usbh0_hstd_cmd_submit() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_set_feature
Description     : Set SetFeature
Arguments       : uint16_t addr            : device address
                : uint16_t epnum           : endpoint number
                : usbh0_utr_cb_t complete        : callback function
Return value    : uint16_t                 : error info
***********************************************************************************************************************/
uint16_t usbh0_hstd_set_feature(uint16_t addr, uint16_t epnum, usbh0_utr_cb_t complete)
{
    if (0xFF == epnum)
    {
        /* SetFeature(Device) */
        gs_usbh0_hstd_class_request.type    = USBH0_SET_FEATURE | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_DEVICE;
        gs_usbh0_hstd_class_request.value   = USBH0_DEV_REMOTE_WAKEUP;
        gs_usbh0_hstd_class_request.index   = 0x0000;
    }
    else
    {
        /* SetFeature(endpoint) */
        gs_usbh0_hstd_class_request.type    = USBH0_SET_FEATURE | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_ENDPOINT;
        gs_usbh0_hstd_class_request.value   = USBH0_ENDPOINT_HALT;
        gs_usbh0_hstd_class_request.index   = epnum;
    }

    gs_usbh0_hstd_class_request.length  = 0x0000;
    gs_usbh0_hstd_class_request.devaddr = addr;

    return usbh0_hstd_cmd_submit(complete);
} /* End of function usbh0_hstd_set_feature() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_clear_feature
Description     : Send ClearFeature command to the connected USB device.
Arguments       : uint16_t addr     : Device address.
                : uint16_t epnum    : Endpoint number.
                : usbh0_utr_cb_t complete : Callback function.
Return value    : uint16_t          : Error info.
***********************************************************************************************************************/
uint16_t usbh0_hstd_clear_feature(uint16_t addr, uint16_t epnum, usbh0_utr_cb_t complete)
{
    if (0xFF == epnum)
    {
        /* ClearFeature(Device) */
        gs_usbh0_hstd_class_request.type    = USBH0_CLEAR_FEATURE | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_DEVICE;
        gs_usbh0_hstd_class_request.value   = USBH0_DEV_REMOTE_WAKEUP;
        gs_usbh0_hstd_class_request.index   = 0x0000;
    }
    else
    {
        /* ClearFeature(endpoint) */
        gs_usbh0_hstd_class_request.type    = USBH0_CLEAR_FEATURE | USBH0_HOST_TO_DEV | USBH0_STANDARD | USBH0_ENDPOINT;
        gs_usbh0_hstd_class_request.value   = USBH0_ENDPOINT_HALT;
        gs_usbh0_hstd_class_request.index   = epnum;
    }
    gs_usbh0_hstd_class_request.length  = 0x0000;
    gs_usbh0_hstd_class_request.devaddr = addr;

    return usbh0_hstd_cmd_submit(complete);
} /* End of function usbh0_hstd_clear_feature() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_clear_stall
Description     : Clear Stall
Arguments       : uint16_t pipe      : Pipe number.
                : usbh0_utr_cb_t complete  : Callback function
Return value    : uint16_t           : Error info.
***********************************************************************************************************************/
usbh0_er_t usbh0_hstd_clear_stall(uint16_t pipe, usbh0_utr_cb_t complete)
{
    usbh0_er_t  err;
    uint16_t    epnum;
    uint16_t    dev_addr;

    epnum       = usbh0_hstd_get_epnum(pipe);
    epnum      |= usbh0_hstd_get_pipe_dir(pipe);
    dev_addr    = usbh0_hstd_get_dev_addr(pipe);

    err = usbh0_hstd_clear_feature(dev_addr, epnum, complete);

    return err;
} /* End of function usbh0_hstd_clear_stall() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_config_desc
Description     : Set GetConfigurationDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t length          : descriptor length
                : usbh0_utr_cb_t complete        : callback function
Return value    : uint16_t                 : error info
***********************************************************************************************************************/
static uint16_t usbh0_hstd_get_config_desc(uint16_t addr, uint16_t length, usbh0_utr_cb_t complete)
{
    uint16_t    i;

    /* Get Configuration Descriptor */
    gs_usbh0_hstd_class_request.type    = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
    gs_usbh0_hstd_class_request.value   = USBH0_CONF_DESCRIPTOR;
    gs_usbh0_hstd_class_request.index   = 0x0000;
    gs_usbh0_hstd_class_request.length  = length;

    if (gs_usbh0_hstd_class_request.length > CLS_DATA_SIZE)
    {
        gs_usbh0_hstd_class_request.length = (uint16_t)CLS_DATA_SIZE;
        USBH0_PRINTF0("***WARNING Descriptor size over !\n");
    }
    gs_usbh0_hstd_class_request.devaddr = addr;

    for (i = 0; i < gs_usbh0_hstd_class_request.length; i++)
    {
        gs_usbh0_hstd_class_data[i] = (uint8_t)0xFF;
    }

    return usbh0_hstd_cmd_submit(complete);
} /* End of function usbh0_hstd_get_config_desc() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_std_req_check
Description     : Sample Standard Request Check
Arguments       : uint16_t err_check        : error
Return value    : uint16_t                 : error info
***********************************************************************************************************************/
static uint16_t usbh0_hstd_std_req_check(uint16_t err_check)
{
    if (USBH0_DATA_STALL == err_check)
    {
        USBH0_PRINTF0("*** Standard Request STALL !\n");
        return USBH0_ERROR;
    }
    else if (USBH0_CTRL_END != err_check)
    {
        USBH0_PRINTF0("*** Standard Request error !\n");
        return USBH0_ERROR;
    }
    else
    {
    }

    return USBH0_OK;
} /* End of function usbh0_hstd_std_req_check() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_get_string_desc
Description     : Set GetDescriptor
Arguments       : uint16_t addr            : device address
                : uint16_t string          : descriptor index
                : usbh0_utr_cb_t complete        : callback function
Return value    : uint16_t                 : error info
***********************************************************************************************************************/
uint16_t usbh0_hstd_get_string_desc(uint16_t addr, uint16_t string, usbh0_utr_cb_t complete)
{
    uint16_t    i;

    if (0 == string)
    {
        gs_usbh0_hstd_class_request.index   = 0x0000;
        gs_usbh0_hstd_class_request.length  = 0x0004;
    }
    else
    {
        /* Set LanguageID */
        gs_usbh0_hstd_class_request.index   = (uint16_t)(gs_usbh0_hstd_class_data[2]);
        gs_usbh0_hstd_class_request.index  |= (uint16_t)((uint16_t)(gs_usbh0_hstd_class_data[3]) << 8);
        gs_usbh0_hstd_class_request.length  = (uint16_t)CLS_DATA_SIZE;
    }

    gs_usbh0_hstd_class_request.type    = USBH0_GET_DESCRIPTOR | USBH0_DEV_TO_HOST | USBH0_STANDARD | USBH0_DEVICE;
    gs_usbh0_hstd_class_request.value   = (uint16_t)(USBH0_STRING_DESCRIPTOR + string);
    gs_usbh0_hstd_class_request.devaddr = addr;

    for (i = 0; i < gs_usbh0_hstd_class_request.length; i++)
    {
        gs_usbh0_hstd_class_data[i] = (uint8_t)0xFF;
    }

    return usbh0_hstd_cmd_submit(complete);
} /* End of function usbh0_hstd_get_string_desc() */

/***********************************************************************************************************************
Function Name   : usbh0_hstd_mgr_task
Description     : The host manager (MGR) task.
Argument        : none
Return          : none
***********************************************************************************************************************/
void usbh0_hstd_mgr_task(void)
{
    st_usbh0_utr_t      *p_mess;
    st_usbh0_hcdreg_t   *p_driver;
    uint16_t            rootport;
    uint16_t            dev_addr;
    uint16_t            pipenum;
    uint16_t            md;
    uint16_t            enume_mode;     /* Enumeration mode (device state) */
    usbh0_er_t          err;

    r_usbh0_hstd_hci_task();

    /* Receive message */
    err = USBH0_RCV_MSG(USBH0_MGR_MBX, (usbh0_msg_t**)&p_mess);
    if (USBH0_OK != err)
    {
        return;
    }
    else
    {
        switch (p_mess->msginfo)
        {
            /* USB-bus control (change device state) */
            case USBH0_MSG_MGR_STATUSRESULT:
                rootport = p_mess->keyword;
                switch (g_usbh0_hstd_mgr_mode[rootport])
                {
                    case USBH0_DEFAULT:   /* End of reset signal */
                        g_usbh0_hstd_device_speed = p_mess->result;

                        /* Set device speed */
                        usbh0_hstd_set_rootport((uint16_t)USBH0_DEVICE_0, rootport);
                        switch (g_usbh0_hstd_device_speed)
                        {
                            case USBH0_HSCONNECT: /* Hi Speed Device Connect */
                                USBH0_PRINTF0(" Hi-Speed Device\n");
                            	usbh0_hstd_enum_get_descriptor((uint16_t)USBH0_DEVICE_0, (uint16_t)0);
                            break;

                            case USBH0_FSCONNECT: /* Full Speed Device Connect */
                                USBH0_PRINTF0(" Full-Speed Device\n");
                            	usbh0_hstd_enum_get_descriptor((uint16_t)USBH0_DEVICE_0, (uint16_t)0);
                            break;

                            case USBH0_LSCONNECT: /* Low Speed Device Connect */
                                USBH0_PRINTF0(" Low-Speed Device\n");
                                USBH0_PRINTF0(" Not Supported\n");
                                usbh0_hstd_enum_get_descriptor((uint16_t)USBH0_DEVICE_0, (uint16_t)0);
                            break;

                            default:
                                USBH0_PRINTF0(" Device/Detached\n");
                                g_usbh0_hstd_mgr_mode[rootport] = USBH0_DETACHED;
                            break;
                        }
                    break;

                    case USBH0_DETACHED:
                    break;

                    default:
                    break;
                }
            break;

            case USBH0_MSG_MGR_SUBMITRESULT:
                dev_addr = p_mess->keyword;
                rootport = usbh0_hstd_get_rootport(dev_addr);
                switch (g_usbh0_hstd_mgr_mode[rootport])
                {
                    case USBH0_RESUME_PROCESS:        /* Resume */
                        /* Resume Sequence Number is 1 to 2 */
                        usbh0_hstd_resu_cont(dev_addr, p_mess->result);
                    break;

                    case USBH0_SUSPENDED_PROCESS:     /* Suspend */
                        /* Suspend Sequence Number is 1 to 2 */
                        usbh0_hstd_susp_cont(dev_addr, p_mess->result);
                    break;

                    case USBH0_DEFAULT:               /* Enumeration */
                        enume_mode = usbh0_hstd_enumeration(p_mess->result);
                        switch (enume_mode)
                        {
                            case USBH0_NONDEVICE:     /* Detach Mode */
                                USBH0_PRINTF1("### Enumeration error (address%d)\n", g_usbh0_hstd_device_addr);
                                g_usbh0_hstd_mgr_mode[rootport] = USBH0_DETACHED;
                            break;

                            case USBH0_NOTTPL:
                                USBH0_PRINTF1("### Not support device (address%d)\n", g_usbh0_hstd_device_addr);
                                g_usbh0_hstd_mgr_mode[rootport] = USBH0_DETACHED;
                            break;

                            case USBH0_COMPLETEPIPESET:
                                g_usbh0_hstd_mgr_mode[rootport] = USBH0_CONFIGURED;
                            break;

                            default:
                            break;
                        }
                    break;

                    default:
                    break;
                }
            break;

            case USBH0_MSG_MGR_AORDETACH:
                rootport = p_mess->keyword;
                switch (p_mess->result)
                {
                    case USBH0_DETACH:
#ifdef USBH0_HOST_COMPLIANCE_MODE
                        USBH0_COMPLIANCE_DISP(USBH0_COMP_DETACH, USBH0_NO_ARG);
#endif /* USBH0_HOST_COMPLIANCE_MODE */
                        USBH0_PRINTF1(" [Detach Device port%d] \n", rootport);
                        g_usbh0_hstd_mgr_mode[rootport]   = USBH0_DETACHED;
                        g_usbh0_hstd_device_speed         = USBH0_NOCONNECT;

                        for (md = 0; md < g_usbh0_hstd_device_num; md++)
                        {
                            p_driver = &g_usbh0_hstd_device_drv[md];
                            if (p_driver->devaddr == (rootport + USBH0_DEVICEADDR))
                            {
                                (*p_driver->devdetach)(p_driver->devaddr);

                                p_driver->rootport  = USBH0_NOPORT;       /* Root port */
                                p_driver->devaddr   = USBH0_NODEVICE;     /* Device address */
                                p_driver->devstate  = USBH0_DETACHED;     /* Device state */
                            }
                        }
                    break;

                    case USBH0_ATTACH:
#ifdef USBH0_HOST_COMPLIANCE_MODE
                        USBH0_COMPLIANCE_DISP(USBH0_COMP_ATTACH, USBH0_NO_ARG);
#endif /* USBH0_HOST_COMPLIANCE_MODE */
                        if (USBH0_DETACHED == g_usbh0_hstd_mgr_mode[rootport])
                        {
                            g_usbh0_hstd_device_addr = (uint16_t)(rootport + USBH0_DEVICEADDR);
                            if (USBH0_MAXDEVADDR < g_usbh0_hstd_device_addr)
                            {
                                /* For port1 */
                                USBH0_PRINTF0("Device address error\n");
                            }
                            else
                            {
                                g_usbh0_hstd_mgr_mode[rootport] = USBH0_DEFAULT;
                                r_usbh0_hstd_hci_set_device_address_of_rootpoot(rootport, g_usbh0_hstd_device_addr);
                                r_usbh0_hstd_hci_port_reset(rootport);
                            }
                        }
                    break;

                    default:
                    break;
                }
            break;

            case USBH0_MSG_MGR_OVERCURRENT:
                rootport = p_mess->keyword;
                USBH0_PRINTF0(" Please detach device \n ");
                USBH0_PRINTF1("VBUS off port%d\n", rootport);

                g_usbh0_hstd_mgr_mode[rootport] = USBH0_DEFAULT;
                for (md = 0; md < g_usbh0_hstd_device_num; md++)
                {
                    p_driver = &g_usbh0_hstd_device_drv[md];
                    if (p_driver->rootport == rootport)
                    {
                        USBH0_OVERCURRENT(rootport);

                        p_driver->rootport  = USBH0_NOPORT;       /* Root port */
                        p_driver->devaddr   = USBH0_NODEVICE;     /* Device address */
                        p_driver->devstate  = USBH0_DETACHED;     /* Device state */
                    }
                }
            break;

            case USBH0_DO_GLOBAL_SUSPEND:
                dev_addr = p_mess->keyword;
                usbh0_hstd_mgr_suspend(dev_addr);
            break;

            case USBH0_DO_GLOBAL_RESUME:
                dev_addr = p_mess->keyword;
                usbh0_hstd_mgr_resume(dev_addr);
            break;

            case USBH0_DO_CLR_STALL:
                pipenum = p_mess->keyword;
                usbh0_hstd_clear_stall(pipenum, p_mess->complete);
            break;

            default:
                USBH0_PRINTF0(" Unsupported MGR Message \n ");
            break;
        }
    }

    err = USBH0_REL_BLK(USBH0_MGR_MPL, p_mess);
    if (USBH0_OK != err)
    {
        USBH0_PRINTF1("### USB MGR rel_blk error: %d\n", p_mess->msginfo);
    }
} /* End of function usbh0_hstd_mgr_task() */

#ifdef USBH0_HOST_COMPLIANCE_MODE
/***********************************************************************************************************************
Function Name   : usbh0_hstd_electrical_test_mode
Description     : Host electrical test mode function
Argument        : product_id          : Task Start Code
                : port                : rootport number
Return          : none
***********************************************************************************************************************/
void usbh0_hstd_electrical_test_mode(uint16_t product_id, uint16_t port)
{
    uint16_t    dev_addr;

    dev_addr = r_usbh0_hstd_hci_get_device_address_of_rootpoot(port);

    switch (product_id)
    {
        case 0x0101:    /* Test_SE0_NAK */
            r_usbh0_hstd_hci_electrical_test(port, 0);
        break;

        case 0x0102:    /* Test_J */
            r_usbh0_hstd_hci_electrical_test(port, 1);
        break;

        case 0x0103:    /* Test_K */
            r_usbh0_hstd_hci_electrical_test(port, 2);
        break;

        case 0x0104:    /* Test_Packet */
            r_usbh0_hstd_hci_electrical_test(port, 3);
        break;

        case 0x0105:    /* Reserved */
        break;

        case 0x0106:    /* HS_HOST_PORT_SUSPEND_RESUME */
            r_usbh0_hstd_hci_electrical_test(port, 4);
        break;

        case 0x0107:    /* SINGLE_STEP_GET_DEV_DESC */
            R_USBH0_HstdDelayXms(15000);            /* wait 15sec */
            usbh0_hstd_enum_get_descriptor(dev_addr, 0);
        break;

        case 0x0108:    /* SINGLE_STEP_GET_DEV_DESC_DATA */
            g_usbh0_hstd_test_packet_parameter_flag = 1;
            usbh0_hstd_enum_get_descriptor(dev_addr, 0);
        break;

        default:
        break;
    }
} /* End of function usbh0_hstd_electrical_test_mode() */
#endif /* USBH0_HOST_COMPLIANCE_MODE */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
