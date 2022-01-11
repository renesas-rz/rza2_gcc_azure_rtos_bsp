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
* File Name    : r_usbh0_hEhciMemory.c
* Description  : EHCI Memory Control Code
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

/***********************************************************************************************************************
External variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/

/***********************************************************************************************************************
Static variables and functions
***********************************************************************************************************************/
#ifdef __ICCARM__
#pragma location="EHCI_PFL_0"
static uint32_t             gs_usbh0_hstd_ehci_periodic_frame_list[USBH0_EHCI_PFL_SIZE];    /* 4KByte alignment */
#pragma location="EHCI_QH_0"
static st_usbh0_ehci_qh_t   gs_usbh0_hstd_ehci_qh[USBH0_EHCI_NUM_QH];                       /* 32Byte alignment */
#pragma location="EHCI_QTD_0"
static st_usbh0_ehci_qtd_t  gs_usbh0_hstd_ehci_qtd[USBH0_EHCI_NUM_QTD];                     /* 32Byte alignment */
#pragma location="EHCI_ITD_0"
static st_usbh0_ehci_itd_t  gs_usbh0_hstd_ehci_itd[USBH0_EHCI_NUM_ITD];                     /* 32Byte alignment */
#pragma location="EHCI_SITD_0"
static st_usbh0_ehci_sitd_t gs_usbh0_hstd_ehci_sitd[USBH0_EHCI_NUM_SITD];                   /* 32Byte alignment */
#endif  /* __ICCARM__ */


#ifdef __GNUC__
static uint32_t             gs_usbh0_hstd_ehci_periodic_frame_list[USBH0_EHCI_PFL_SIZE]     /* 4KByte alignment */
        __attribute__ ((section ("EHCI_PFL_0")));
static st_usbh0_ehci_qh_t   gs_usbh0_hstd_ehci_qh[USBH0_EHCI_NUM_QH]                        /* 32Byte alignment */
        __attribute__ ((section ("EHCI_QH_0")));
static st_usbh0_ehci_qtd_t  gs_usbh0_hstd_ehci_qtd[USBH0_EHCI_NUM_QTD]                      /* 32Byte alignment */
        __attribute__ ((section ("EHCI_QTD_0")));
static st_usbh0_ehci_itd_t  gs_usbh0_hstd_ehci_itd[USBH0_EHCI_NUM_ITD]                      /* 32Byte alignment */
        __attribute__ ((section ("EHCI_ITD_0")));
static st_usbh0_ehci_sitd_t gs_usbh0_hstd_ehci_sitd[USBH0_EHCI_NUM_SITD]                    /* 32Byte alignment */
        __attribute__ ((section ("EHCI_SITD_0")));
#endif  /* __GNUC__ */


#ifdef __CC_ARM
#pragma arm section zidata = "EHCI_PFL_0"
static uint32_t             gs_usbh0_hstd_ehci_periodic_frame_list[USBH0_EHCI_PFL_SIZE];    /* 4KByte alignment */
#pragma arm section zidata
#pragma arm section zidata = "EHCI_QH_0"
static st_usbh0_ehci_qh_t   gs_usbh0_hstd_ehci_qh[USBH0_EHCI_NUM_QH];                       /* 32Byte alignment */
#pragma arm section zidata
#pragma arm section zidata = "EHCI_QTD_0"
static st_usbh0_ehci_qtd_t  gs_usbh0_hstd_ehci_qtd[USBH0_EHCI_NUM_QTD];                     /* 32Byte alignment */
#pragma arm section zidata
#pragma arm section zidata = "EHCI_ITD_0"
static st_usbh0_ehci_itd_t  gs_usbh0_hstd_ehci_itd[USBH0_EHCI_NUM_ITD];                     /* 32Byte alignment */
#pragma arm section zidata
#pragma arm section zidata = "EHCI_SITD_0"
static st_usbh0_ehci_sitd_t gs_usbh0_hstd_ehci_sitd[USBH0_EHCI_NUM_SITD];                   /* 32Byte alignment */
#pragma arm section zidata
#endif  /* __CC_ARM */

static st_usbh0_ehci_qtd_t  *gsp_usbh0_hstd_ehci_qtd_top __attribute__ ((section ("USB_HOST_0")));


/***********************************************************************************************************************
 * Function     : Initialize QH
 * Declaration  : usbh0_hstd_ehci_init_qh_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_init_qh_memory(void)
{
    usbh0_hstd_hci_lock_resouce();

    memset(&gs_usbh0_hstd_ehci_qh[0], 0, sizeof(gs_usbh0_hstd_ehci_qh));

    usbh0_hstd_hci_unlock_resouce();
} /* End of function usbh0_hstd_ehci_init_qh_memory() */

/***********************************************************************************************************************
 * Function     : Initialize iTD
 * Declaration  : usbh0_hstd_ehci_init_itd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_init_itd_memory(void)
{
    usbh0_hstd_hci_lock_resouce();

    memset(&gs_usbh0_hstd_ehci_itd[0], 0, sizeof(gs_usbh0_hstd_ehci_itd));

    usbh0_hstd_hci_unlock_resouce();
} /* End of function usbh0_hstd_ehci_init_itd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize siTD
 * Declaration  : usbh0_hstd_ehci_init_sitd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_init_sitd_memory(void)
{
    usbh0_hstd_hci_lock_resouce();

    memset(&gs_usbh0_hstd_ehci_sitd[0], 0, sizeof(gs_usbh0_hstd_ehci_sitd));

    usbh0_hstd_hci_unlock_resouce();
} /* End of function usbh0_hstd_ehci_init_sitd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize qTD
 * Declaration  : usbh0_hstd_ehci_init_qtd_memory(void)
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_init_qtd_memory(void)
{
    st_usbh0_ehci_qtd_t *p_qtd;
    uint32_t            n;

    usbh0_hstd_hci_lock_resouce();

    gsp_usbh0_hstd_ehci_qtd_top = NULL;
    memset(&gs_usbh0_hstd_ehci_qtd[0], 0, sizeof(gs_usbh0_hstd_ehci_qtd));

    usbh0_hstd_hci_unlock_resouce();

    p_qtd = &gs_usbh0_hstd_ehci_qtd[0];

    for (n = 0; n < USBH0_EHCI_NUM_QTD; n++, p_qtd++)
    {
        usbh0_hstd_ehci_free_qtd(p_qtd);
    }
} /* End of function usbh0_hstd_ehci_init_qtd_memory() */

/***********************************************************************************************************************
 * Function     : Initialize Periodic Frame List
 * Declaration  : usbh0_hstd_ehci_init_periodic_frame_list( void )
 * Argument     : void
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_init_periodic_frame_list(void)
{
    uint32_t    *p_list;
    uint32_t    n;

    p_list = &gs_usbh0_hstd_ehci_periodic_frame_list[0];
    for (n = 0; n < USBH0_EHCI_PFL_SIZE; n++)
    {
        *p_list++ = 0x00000001;
    }
} /* End of function usbh0_hstd_ehci_init_periodic_frame_list() */

/***********************************************************************************************************************
 * Function     : Get Periodic Frame List Address
 * Declaration  : usbh0_hstd_ehci_get_periodic_frame_list_addr( void )
 * Argument     : void
 * Return       : uint32_t ; Priodic Frame List Address
 * Note         :
***********************************************************************************************************************/
uint32_t usbh0_hstd_ehci_get_periodic_frame_list_addr(void)
{
    return ((uint32_t)&gs_usbh0_hstd_ehci_periodic_frame_list[0]);
} /* End of function usbh0_hstd_ehci_get_periodic_frame_list_addr() */

/***********************************************************************************************************************
 * Function     : Allocate QH
 * Declaration  : usbh0_hstd_ehci_alloc_qh(void)
 * Argument     : void
 * Return       : st_usbh0_ehci_qh_t* ; QH Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_qh_t* usbh0_hstd_ehci_alloc_qh(void)
{
    st_usbh0_ehci_qh_t  *p_ret_qh = NULL;
    st_usbh0_ehci_qh_t  *p_qh;
    uint32_t            n;

    usbh0_hstd_hci_lock_resouce();
    
    p_qh = &gs_usbh0_hstd_ehci_qh[0];
    for (n = 0; n < USBH0_EHCI_NUM_QH; n++, p_qh++)
    {
        if (FALSE == p_qh->info.enable)
        {
            p_qh->info.enable                   = TRUE;
            p_qh->next_qtd.address              = 0x00000001;
            p_qh->alternate_next_qtd.address    = 0x00000001;
            p_qh->qtd_head                      = NULL;
            p_qh->qtd_end                       = NULL;

            p_ret_qh = p_qh;
            break;
        }
    }

    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret_qh)
    {
        USBH0_HCI_PRINTF0_R("-----------------------------------------------\n");
        USBH0_HCI_PRINTF0_R("Memory Allocation Error: usbh0_hstd_ehci_alloc_qh\n");
        USBH0_HCI_PRINTF0_R("-----------------------------------------------\n");
    }

    return p_ret_qh;
} /* End of function usbh0_hstd_ehci_alloc_qh() */

/***********************************************************************************************************************
 * Function     : Allocate iTD
 * Declaration  : usbh0_hstd_ehci_alloc_itd(void)
 * Argument     : void
 * Return       : st_usbh0_ehci_itd_t* ; ITD Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_itd_t* usbh0_hstd_ehci_alloc_itd(void)
{
    st_usbh0_ehci_itd_t *p_ret_itd = NULL;
    st_usbh0_ehci_itd_t *p_itd;
    uint32_t            n;

    usbh0_hstd_hci_lock_resouce();

    p_itd = &gs_usbh0_hstd_ehci_itd[0];
    for (n = 0; n < USBH0_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (FALSE == p_itd->info.enable)
        {
            p_itd->info.enable = TRUE;

            p_ret_itd = p_itd;
            break;
        }
    }

    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret_itd)
    {
        USBH0_HCI_PRINTF0_R( "------------------------------------------------\n" );
        USBH0_HCI_PRINTF0_R( "Memory Allocation Error: usbh0_hstd_ehci_alloc_itd\n" );
        USBH0_HCI_PRINTF0_R( "------------------------------------------------\n" );
    }

    return p_ret_itd;
} /* End of function usbh0_hstd_ehci_alloc_itd() */

/***********************************************************************************************************************
 * Function     : Allocate siTD
 * Declaration  : usbh0_hstd_ehci_alloc_sitd(void)
 * Argument     : void
 * Return       : st_usbh0_ehci_sitd_t* ; SITD Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_sitd_t* usbh0_hstd_ehci_alloc_sitd(void)
{
    st_usbh0_ehci_sitd_t    *p_ret_sitd = NULL;
    st_usbh0_ehci_sitd_t    *p_sitd;
    uint32_t                n;

    usbh0_hstd_hci_lock_resouce();

    p_sitd = &gs_usbh0_hstd_ehci_sitd[0];
    for (n = 0; n < USBH0_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (FALSE == p_sitd->info.enable)
        {
            p_sitd->info.enable = TRUE;

            p_ret_sitd = p_sitd;
            break;
        }
    }
    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret_sitd)
    {
        USBH0_HCI_PRINTF0_R( "-------------------------------------------------\n" );
        USBH0_HCI_PRINTF0_R( "Memory Allocation Error: usbh0_hstd_ehci_alloc_sitd\n" );
        USBH0_HCI_PRINTF0_R( "-------------------------------------------------\n" );
    }

    return p_ret_sitd;
} /* End of function usbh0_hstd_ehci_alloc_sitd() */

/***********************************************************************************************************************
 * Function     : Allocate qTD
 * Declaration  : usbh0_hstd_ehci_alloc_qtd(void)
 * Argument     : void
 * Return       : st_usbh0_ehci_qtd_t* ; QTD Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_qtd_t* usbh0_hstd_ehci_alloc_qtd(void)
{
    st_usbh0_ehci_qtd_t *p_ret_qtd = NULL;
    
    usbh0_hstd_hci_lock_resouce();
    
    if (NULL != gsp_usbh0_hstd_ehci_qtd_top)
    {
        p_ret_qtd = r_usbh0_pa_to_va(gsp_usbh0_hstd_ehci_qtd_top);

        gsp_usbh0_hstd_ehci_qtd_top = (st_usbh0_ehci_qtd_t *)r_usbh0_pa_to_va((uint32_t)gsp_usbh0_hstd_ehci_qtd_top->next_qtd.address);
    }

    usbh0_hstd_hci_unlock_resouce();

    if (NULL == p_ret_qtd)
    {
        USBH0_HCI_PRINTF0_R( "------------------------------------------------\n" );
        USBH0_HCI_PRINTF0_R( "Memory Allocation Error: usbh0_hstd_ehci_alloc_qtd\n" );
        USBH0_HCI_PRINTF0_R( "------------------------------------------------\n" );
    }

    return p_ret_qtd;
} /* End of function usbh0_hstd_ehci_alloc_qtd() */

/***********************************************************************************************************************
 * Function     : Free qTD
 * Declaration  : usbh0_hstd_ehci_free_qtd( st_usbh0_ehci_qtd_t *free_qtd )
 * Argument     : st_usbh0_ehci_qtd_t *free_qtd ; QTD Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_free_qtd(st_usbh0_ehci_qtd_t *p_free_qtd)
{
    usbh0_hstd_hci_lock_resouce();

    (uint32_t)R_MMU_VAtoPA(gsp_usbh0_hstd_ehci_qtd_top,&p_free_qtd->next_qtd.address);

    gsp_usbh0_hstd_ehci_qtd_top = r_usbh0_pa_to_va(p_free_qtd);

    usbh0_hstd_hci_unlock_resouce();
} /* End of function usbh0_hstd_ehci_free_qtd() */

/***********************************************************************************************************************
 * Function     : Search QH
 * Declaration  : usbh0_hstd_ehci_search_qh( uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 *              : uint32_t eptype       ; Endpoint Type
 * Return       : st_usbh0_ehci_qh_t*          ; QH Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_qh_t* usbh0_hstd_ehci_search_qh(uint32_t devadd, uint32_t epnum, uint32_t direction, uint32_t eptype)
{
    st_usbh0_ehci_qh_t  *p_ret_qh = NULL;
    st_usbh0_ehci_qh_t  *p_qh;
    uint32_t            n;

    p_qh = &gs_usbh0_hstd_ehci_qh[1];
    for (n = 1; n < USBH0_EHCI_NUM_QH; n++, p_qh++)
    {
        if (TRUE == p_qh->info.enable)
        {
            if ((p_qh->endpoint1.bit.device_address == devadd) && (p_qh->endpoint1.bit.endpt == epnum))
            {
                if ((USBH0_EP_CNTRL == eptype) || (p_qh->info.direction == direction))
                {
                    p_ret_qh = p_qh;
                }
            }
        }
    }

    return p_ret_qh;
} /* End of function usbh0_hstd_ehci_search_qh() */

/***********************************************************************************************************************
 * Function     : Search ITD
 * Declaration  : usbh0_hstd_ehci_search_itd( uint32_t devadd, uint32_t epnum, uint32_t direction )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 * Return       : st_usbh0_ehci_itd_t*         ; ITD Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_itd_t* usbh0_hstd_ehci_search_itd(uint32_t devadd, uint32_t epnum, uint32_t direction)
{
    st_usbh0_ehci_itd_t *p_ret_itd = NULL;
    st_usbh0_ehci_itd_t *p_itd;
    uint32_t            n;

    p_itd = &gs_usbh0_hstd_ehci_itd[0];
    for (n = 0; n < USBH0_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (TRUE == p_itd->info.enable)
        {
            if ((p_itd->buffer[0].bit0.device_address == devadd) 
                && (p_itd->buffer[0].bit0.endpt == epnum) 
                && (p_itd->buffer[1].bit1.direction == direction))
            {
                p_ret_itd = p_itd;
            }
        }
    }

    return p_ret_itd;
} /* End of function usbh0_hstd_ehci_search_itd() */

/***********************************************************************************************************************
 * Function     : Search SITD
 * Declaration  : usbh0_hstd_ehci_search_sitd( uint32_t devadd, uint32_t epnum, uint32_t direction )
 * Argument     : uint32_t devadd       ; Device Address
 *              : uint32_t epnum        ; Endpoint Number
 *              : uint32_t direction    ; Direction
 * Return       : st_usbh0_ehci_sitd_t*        ; SITD Address
 * Note         :
***********************************************************************************************************************/
st_usbh0_ehci_sitd_t* usbh0_hstd_ehci_search_sitd(uint32_t devadd, uint32_t epnum, uint32_t direction)
{
    st_usbh0_ehci_sitd_t    *p_ret_sitd = NULL;
    st_usbh0_ehci_sitd_t    *p_sitd;
    uint32_t                n;

    p_sitd = &gs_usbh0_hstd_ehci_sitd[0];
    for (n = 0; n < USBH0_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (TRUE == p_sitd->info.enable)
        {
            if ((p_sitd->endpoint1.bit.device_address == devadd) 
                && (p_sitd->endpoint1.bit.endpt == epnum) 
                && (p_sitd->endpoint1.bit.direction == direction))
            {
                p_ret_sitd = p_sitd;
            }
        }
    }

    return p_ret_sitd;
} /* End of function usbh0_hstd_ehci_search_sitd() */

/***********************************************************************************************************************
 * Function     : Clear QH for disconnected device
 * Declaration  : usbh0_hstd_ehci_clear_device_qh( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_device_qh(uint32_t devadd)
{
    st_usbh0_ehci_qh_t  *p_qh;
    uint32_t            n;

    p_qh = &gs_usbh0_hstd_ehci_qh[1];
    for (n = 1; n < USBH0_EHCI_NUM_QH; n++, p_qh++)
    {
        if (TRUE == p_qh->info.enable)
        {
            if (p_qh->endpoint1.bit.device_address == devadd)
            {
                usbh0_hstd_ehci_clear_qh(p_qh);
            }
        }
    }
} /* End of function usbh0_hstd_ehci_clear_device_qh() */

/***********************************************************************************************************************
 * Function     : Clear QH
 * Declaration  : void usbh0_hstd_ehci_clear_qh( st_usbh0_ehci_qh_t *qh )
 * Argument     : st_usbh0_ehci_qh_t *qh ; QH Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_qh(st_usbh0_ehci_qh_t *p_qh)
{
    if (NULL != p_qh)
    {
        if (TRUE == p_qh->info.enable)
        {
            /* Unlink qh */
            if (0 != p_qh->info.pollrate)
            {
                /* Interrupt */
                usbh0_hstd_ehci_unlink_qh_for_periodic(p_qh);
            }
            else
            {
                /* Control/Bulk */
                usbh0_hstd_ehci_unlink_qh_for_async(p_qh);
            }

            /* Clear qh( enable = FALSE ) */
            memset(p_qh, 0, sizeof(st_usbh0_ehci_qh_t));
        }
    }
} /* End of function usbh0_hstd_ehci_clear_qh() */

/***********************************************************************************************************************
 * Function     : Clear ITD for disconnected device
 * Declaration  : usbh0_hstd_ehci_clear_device_itd( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_device_itd(uint32_t devadd)
{
    st_usbh0_ehci_itd_t *p_itd;
    uint32_t            n;

    p_itd = &gs_usbh0_hstd_ehci_itd[0];
    for (n = 0; n < USBH0_EHCI_NUM_ITD; n++, p_itd++)
    {
        if (TRUE == p_itd->info.enable)
        {
            if (p_itd->buffer[0].bit0.device_address == devadd)
            {
                /* Unlink itd */
                usbh0_hstd_ehci_unlink_itd(p_itd);

                /* Clear itd( enable = FALSE ) */
                memset(p_itd, 0, sizeof(st_usbh0_ehci_itd_t));
            }
        }
    }
} /* End of function usbh0_hstd_ehci_clear_device_itd() */

/***********************************************************************************************************************
 * Function     : Clear SITD for disconnected device
 * Declaration  : usbh0_hstd_ehci_clear_device_sitd( uint32_t devadd )
 * Argument     : uint32_t devadd ; Device Address
 * Return       : void
 * Note         :
***********************************************************************************************************************/
void usbh0_hstd_ehci_clear_device_sitd(uint32_t devadd)
{
    st_usbh0_ehci_sitd_t    *p_sitd;
    uint32_t                n;

    p_sitd = &gs_usbh0_hstd_ehci_sitd[0];
    for (n = 0; n < USBH0_EHCI_NUM_SITD; n++, p_sitd++)
    {
        if (TRUE == p_sitd->info.enable)
        {
            if (p_sitd->endpoint1.bit.device_address == devadd)
            {
                /* Unlink sitd */
                usbh0_hstd_ehci_unlink_sitd(p_sitd);

                /* Clear sitd( enable = FALSE ) */
                memset(p_sitd, 0, sizeof(st_usbh0_ehci_sitd_t));
            }
        }
    }
} /* End of function usbh0_hstd_ehci_clear_device_sitd() */

/***********************************************************************************************************************
End Of File
***********************************************************************************************************************/
