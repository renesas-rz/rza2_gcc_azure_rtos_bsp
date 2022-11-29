/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************
* File: decode_sample.c
*     Decode a JPEG image.
* - Device(s)    : RZ/A2M
* - Tool-Chain   : e2Studio 2021-01
*                : GNU ARM Embedded 6.3.1.20170620
* - OS           : FreeRTOS Version See src/freertos/include/FreeRTOS.h
* - H/W Platform : GR-MANGO(mbed-RZ/A2M) X71A-M01-B
* - Description  : -
* - Operation    : None.
* - Limitations  : None.
******************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include  <stdio.h>
#include  <string.h>
#include  "jcu_swap.h"
#include  "lcd_panel.h"
#include  "r_cache_lld_rza2m.h"
#include  "r_jcu.h"
#include  "r_memory_map.h"
#include  "r_rvapi_vdc.h"
#include  "rz_co.h"
#include  "r_os_abstraction_api.h"
#include  "VRAM.h"
#include  "JCU_ExampleImage.h"
#include  "application.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/***********************************************************************
* Defines: Macros in this file scope. Global static defines (GS)
************************************************************************/
#define  GS_N  "\r\n"


/***********************************************************************
* Define: GS_OUTPUT_PIXEL_FORMAT
*    JCU_OUTPUT_RGB565(=2), JCU_OUTPUT_ARGB8888(=1), JCU_OUTPUT_YCbCr422(=0)
************************************************************************/
#define  GS_OUTPUT_PIXEL_FORMAT  (JCU_OUTPUT_RGB565)


/***********************************************************************
* Define: GS_VIDEO_PIXEL_FORMAT
*    VDC_GR_FORMAT_RGB565(=0),  VDC_GR_FORMAT_RGB888(=1),  VDC_GR_FORMAT_YCBCR422(=8)
************************************************************************/
#define  GS_VIDEO_PIXEL_FORMAT  (VDC_GR_FORMAT_RGB565)


/***********************************************************************
* Defines:
************************************************************************/
#define  GS_FRAME_BYTE_PER_PIXEL  (2u)  /* ARGB8888 is 4,  RGB565 is 2 */
#define  GS_FRAME_WIDTH           (800u)
#define  GS_FRAME_HEIGHT          (480u)
#define  GS_ALPHA_VAL_MAX         (0xFF)


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static errnum_t  gs_DisplayImage( uintptr_t  in_PhysicalFrameAddress,  jcu_image_info_t*  in_Image );


/***********************************************************************
* Function Name: R_JCU_SampleDecode
*    Decodes JPEG data to a memory.
*
* Arguments:
*    None.
*
* Returns:
*    Error Code. 0=No Error.
************************************************************************/
errnum_t  
    R_JCU_SampleDecode(void) 
{
    const uint8_t*      address_of_JPEG_image = g_jcu_example_image; 
    size_t const        size_of_JPEG_image = sizeof( g_jcu_example_image );

    errnum_t            e;
    bool_t              s;
    jcu_decode_param_t  decode;
    jcu_buffer_param_t  buffer;
    bool_t              is_JCU_initialized = false;
    uint8_t*            memory_address_of_RAW;
    size_t              memory_size_of_RAW;
    uint8_t*            memory_address_of_JPEG;
    size_t              memory_size_of_JPEG;
    uintptr_t           physical_address_of_RAW;
    uintptr_t           physical_address_of_JPEG;
    jcu_image_info_t    image;
    uint32_t            decoded_event = 0;


    /* Check JPEG data exists */
    {
        static const  uint8_t  num_0xFF_JPEG_header_letter_1 = 0xFFu;
        static const  uint8_t  num_0xD8_JPEG_header_letter_2 = 0xD8u;

        if (!(

            /* Cast to an appropriate type */
            ( (uint32_t) address_of_JPEG_image[0] == num_0xFF_JPEG_header_letter_1 ) &&

            /* Cast to an appropriate type */
            ( (uint32_t) address_of_JPEG_image[1] == num_0xD8_JPEG_header_letter_2 ) ))
                { e = E_OTHER; goto fin; } /* Error: JPEG data is not in ROM */
    }


    /* (variables) = ... */
    {
        size_t    frame_size = GS_FRAME_WIDTH * GS_FRAME_BYTE_PER_PIXEL * GS_FRAME_HEIGHT;
        uint8_t*  memory_address_of_VRAM;
        size_t    size_of_VRAM;

        e= R_SAMPLE_GetVRAM( &memory_address_of_VRAM,  &size_of_VRAM );
        if(e)
        {
            goto fin;
        }

        /* (variables) = ... */
        memory_address_of_RAW  = memory_address_of_VRAM;
        memory_size_of_RAW     = frame_size;
        memory_address_of_JPEG = memory_address_of_VRAM + frame_size;
        memory_size_of_JPEG    = size_of_VRAM - frame_size;

        /* physical_address_of_RAW = ... */
        e= R_MEMORY_ToPhysicalAddress( memory_address_of_RAW,  &physical_address_of_RAW );
        if(e)
        {
            goto fin;
        }

        /* physical_address_of_JPEG = ... */
        e= R_MEMORY_ToPhysicalAddress( memory_address_of_JPEG,  &physical_address_of_JPEG );
        if(e)
        {
            goto fin;
        }

        /* Guard */
        if (!( ((uintptr_t) memory_address_of_RAW  % 8) == 0 ))
        {
            e=__LINE__;
            goto fin;
        }

        /* Cast to an appropriate type */
        if (!( ((uintptr_t) memory_address_of_JPEG % 8) == 0 ))
        {
            e=__LINE__;
            goto fin;
        }
        if ( !( memory_size_of_JPEG > size_of_JPEG_image ))
        {
            e=__LINE__;
            goto fin;
        }

        /* decoded_event = ... */
        s= R_OS_SemaphoreCreate( &decoded_event,  1 );
        if(!s)
        {
            e=__LINE__;
            goto fin;
        }
    }


    /* Write JPEG data in physical memory */
    memcpy( memory_address_of_JPEG,  address_of_JPEG_image,  size_of_JPEG_image );
    memset( memory_address_of_RAW,  0,  memory_size_of_RAW );

    R_CACHE_L1DataCleanInvalidAll();


    /* R_JCU_Initialize() */
    e= R_JCU_Initialize( NULL );
    if(e)
    {
        goto fin;
    }
    is_JCU_initialized = true;

    e= R_JCU_SelectCodec( JCU_DECODE );
    if(e)
    {
        goto fin;
    }


    /* R_JCU_SetDecodeParam() */
    {
        buffer.source.swapSetting       = JCU_SWAP_JPEG_NONE;

        /* Cast to an appropriate type */
        buffer.source.address           = (uint32_t*) physical_address_of_JPEG;

        /* Cast to an appropriate type */
        buffer.destination.address      = (uint32_t*) physical_address_of_RAW;

        /* Cast to an appropriate type */
        buffer.lineOffset               = (int16_t) GS_FRAME_WIDTH;
        decode.decodeFormat             = GS_OUTPUT_PIXEL_FORMAT;

        if ( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_YCbCr422 )
        {
            buffer.destination.swapSetting  = JCU_SWAP_CB_Y0_CR_Y1_PIXEL0123;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_128;
        }
        else if ( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_RGB565 )
        {
            buffer.destination.swapSetting  = JCU_SWAP_RGB565_PIXEL0123;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_0;
        }
        else
        {
            if (!( GS_OUTPUT_PIXEL_FORMAT == JCU_OUTPUT_ARGB8888 ))
            {
                e=E_OTHER;
                goto fin;
            }
            buffer.destination.swapSetting  = JCU_SWAP_ARGB8888_PIXEL01;
            decode.outputCbCrOffset = JCU_CBCR_OFFSET_0;
        }

        decode.alpha                 = GS_ALPHA_VAL_MAX;
        decode.horizontalSubSampling = JCU_SUB_SAMPLING_1_1;
        decode.verticalSubSampling   = JCU_SUB_SAMPLING_1_1;

        e= R_JCU_SetDecodeParam( &decode,  &buffer );
        if(e)
        {
            goto fin;
        }
    }
    e= R_JCU_SetPauseForImageInfo( true );
    if(e)
    {
        goto fin;
    }

    printf( "Decoding:"GS_N );

    /* Cast to an appropriate type */
    printf( "    memory_address_of_JPEG   = 0x%08X"GS_N,  (uintptr_t) memory_address_of_JPEG );
    printf( "    physical_address_of_JPEG = 0x%08X"GS_N,  physical_address_of_JPEG );

    /* Cast to an appropriate type */
    printf( "    memory_address_of_RAW   = 0x%08X"GS_N,  (uintptr_t) memory_address_of_RAW );
    printf( "    physical_address_of_RAW = 0x%08X"GS_N,  physical_address_of_RAW );


    /* image = ... : R_JCU_GetImageInfo() */
    s= R_OS_SemaphoreWait( &decoded_event,  0 ); /* Clear. Time out must be ignored. */

    /* Cast to an appropriate type */
    e= R_JCU_StartAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &decoded_event );
    if(e)
    {
        goto fin;
    } /* Set */
    s= R_OS_SemaphoreWait( &decoded_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
    if(!s)
    {
        e=__LINE__;
        goto fin;
    }
    e= R_JCU_GetAsyncError();
    if(e)
    {
        goto fin;
    }

    e= R_JCU_GetImageInfo( &image );
    if(e)
    {
        goto fin;
    } /* image = . */


    /* R_JCU_ContinueAsync() : Decode JPEG image */
    s= R_OS_SemaphoreWait( &decoded_event,  0 ); /* Clear. Time out must be ignored. */

    /* Cast to an appropriate type */
    e= R_JCU_ContinueAsync( JCU_IMAGE_INFO,  (r_co_function_t) R_OS_SemaphoreRelease,  &decoded_event );
    if(e)
    {
        goto fin;
    } /* Set */
    s= R_OS_SemaphoreWait( &decoded_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
    if(!s)
    {
        e=__LINE__;
        goto fin;
    }
    e= R_JCU_GetAsyncError();
    if(e)
    {
        goto fin;
    }


    /* gs_DisplayImage */
    e= gs_DisplayImage( physical_address_of_RAW,  &image );
    if(e)
    {
        goto fin;
    }


    e=0;
fin:
    /* R_JCU_TerminateAsync() */
    if ( is_JCU_initialized )
    {
        errnum_t  ee;
        uint32_t  terminate_event = 0;
        s= R_OS_SemaphoreCreate( &terminate_event,  1 );
        if((!s) && (e==0))
        {
            e=__LINE__;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  0 ); /* Clear. Time out must be ignored. */

        /* Cast to an appropriate type */
        ee = R_JCU_TerminateAsync( (r_co_function_t) R_OS_SemaphoreRelease,  &terminate_event ); /* Set */
        if(e==0)
        {
            e=ee;
        }
        s= R_OS_SemaphoreWait( &terminate_event,  R_OS_ABSTRACTION_EV_WAIT_INFINITE );
        if((!s) && (e==0))
        {
            e=__LINE__;
        }
        ee= R_JCU_GetAsyncError();
        if(e==0)
        {
            e=ee;
        }
        R_OS_SemaphoreDelete( &terminate_event );
    }

    /* ... */
    R_OS_SemaphoreDelete( &decoded_event );
    R_ERROR_Set( e );
    return  e;
} /* End of function R_JCU_SampleDecode */


/***********************************************************************
* Function Name: gs_DisplayImage
*    Display the JPEG decoded image
*
* Arguments:
*   in_PhysicalFrameAddress - Displaying PhysicalFrameAddress
*   in_Image                - JPEG data
*
* Return Value:
*   Error Code, 0=No Error
************************************************************************/
static errnum_t  gs_DisplayImage 
                    ( uintptr_t  in_PhysicalFrameAddress,  jcu_image_info_t*  in_Image )
{
    errnum_t      e;
    vdc_error_t  error = VDC_OK;


    /* R_RVAPI_InitializeVDC */
    {
        clock_config_t  clock_config;
        vdc_lvds_t     lvds;

        clock_config.panel_clk = LCD_CH0_PANEL_CLK;
        clock_config.panel_clk_div = LCD_CH0_PANEL_CLK_DIV;

        lvds.lvds_vdc_sel = VDC_CHANNEL_0; /* A channel in VDC whose data is to be output through the LVDS */
        lvds.lvds_in_clk_sel = LVDS_PLL_INPUT_CLK; /* The clock input to frequency divider 1 */
        lvds.lvds_idiv_set = LVDS_PLL_NIDV; /* The frequency dividing value (NIDIV) for frequency divider 1 */
        lvds.lvds_odiv_set = LVDS_PLL_NODIV; /* The frequency dividing value (NODIV) for frequency divider 2 */
        lvds.lvdspll_fd = LVDS_PLL_NFD; /* The frequency dividing value (NFD) for the feedback frequency */
        lvds.lvdspll_rd = LVDS_PLL_NRD; /* The frequency dividing value (NRD) for the input frequency */
        lvds.lvdspll_od = LVDS_PLL_NOD; /* The frequency dividing value (NOD) for the output frequency */
        clock_config.lvds = &lvds;

        error = R_RVAPI_InitializeVDC( VDC_CHANNEL_0,  &clock_config );
    }

    /* R_RVAPI_DispControlConfigVDC */
    if (VDC_OK == error)
    {
        qe_config_t q_cnf;

        q_cnf.vps = LCD_CH0_S_VSYNC; /* Vertical Pulse start position                */
        q_cnf.vpw = LCD_CH0_W_VSYNC; /* Vertical Pulse Width(VPW)                    */
        q_cnf.vs = LCD_CH0_DISP_VS; /* LCD display area vertical start position      */
        q_cnf.vdp = LCD_CH0_DISP_VW; /* Vertical Display Period(VDP)                 */
        q_cnf.hps = LCD_CH0_S_HSYNC; /* Horizontal Pulse start position              */
        q_cnf.hpw = LCD_CH0_W_HSYNC; /* Horizontal Pulse Width(HPW)                  */
        q_cnf.hs = LCD_CH0_DISP_HS; /* LCD display area horizontal start position    */
        q_cnf.hdp = LCD_CH0_DISP_HW; /* Horizontal Display Period(HDP                */
        q_cnf.vtp = LCD_CH0_SIG_FV; /* Vertical Total Period(VTP)                    */
        q_cnf.htp = LCD_CH0_SIG_FH; /* Horizontal Total Period(HTP)                  */
        q_cnf.tcon_vsync = LCD_CH0_TCON_PIN_VSYNC; /* Vsync_TCON_select              */
        q_cnf.tcon_hsync = LCD_CH0_TCON_PIN_HSYNC; /* Hsync_TCON_select              */
        q_cnf.tcon_de = LCD_CH0_TCON_PIN_DE; /* DE_TCON_select                       */
        q_cnf.tcon_vsync_inv = LCD_CH0_TOCN_POL_VSYNC; /* Vsync_POL                  */
        q_cnf.tcon_hsync_inv = LCD_CH0_TOCN_POL_HSYNC; /* Hsync_POL                  */
        q_cnf.tcon_de_inv = LCD_CH0_TOCN_POL_DE; /* DE_POL                           */
        q_cnf.tcon_half = LCD_CH0_TCON_HALF; /* TCON reference timing, 1/2fH timing(HTP) */
        q_cnf.tcon_ofset = LCD_CH0_TCON_OFFSET; /* TCON reference timing, offset Hsync signal timing */
        q_cnf.lcd_data_out_edge = LCD_CH0_OUT_EDGE; /* LCD_DATA_OUT_EDGE             */
        q_cnf.lcd_outformat = LCD_CH0_OUT_FORMAT; /* LCD output format select        */
        q_cnf.out_endian_on = VDC_ON;
        q_cnf.out_swap_on = VDC_ON;

        error = R_RVAPI_DispControlConfigVDC ( VDC_CHANNEL_0,  VDC_ON,  &q_cnf );
    }

    /* Graphic Layer */
    if (error == VDC_OK)
    {
        gr_surface_disp_config_t  surface;

        surface.layer_id  = VDC_LAYER_ID_0_RD;

        /* Cast to an appropriate type */
        surface.fb_buff   = (void*) in_PhysicalFrameAddress;
        surface.fb_stride = GS_FRAME_WIDTH * GS_FRAME_BYTE_PER_PIXEL;
        surface.disp_area.hs_rel = 0u;

        /* Cast to an appropriate type */
        surface.disp_area.hw_rel = (uint16_t) in_Image->width;
        surface.disp_area.vs_rel = 0u;

        /* Cast to an appropriate type */
        surface.disp_area.vw_rel = (uint16_t) in_Image->height;
        surface.read_format   = GS_VIDEO_PIXEL_FORMAT;
        surface.read_ycc_swap = VDC_GR_YCCSWAP_Y0CBY1CR;  /* Read Swap for YCbCr422 */
        if ( (GS_FRAME_BYTE_PER_PIXEL == 4)  ||  (GS_VIDEO_PIXEL_FORMAT == VDC_GR_FORMAT_YCBCR422) )
        {
            surface.read_swap = VDC_WR_RD_WRSWA_32BIT;
        }
        else if ( GS_FRAME_BYTE_PER_PIXEL == 2 )
        {
            surface.read_swap = VDC_WR_RD_WRSWA_32_16BIT;
        }
        else
        {
            surface.read_swap = VDC_WR_RD_WRSWA_32_16_8BIT;
        }


        /* Cast to an appropriate type */
        surface.clut_table = NULL;
        surface.disp_mode = VDC_DISPSEL_CURRENT;

        error = R_RVAPI_GraphCreateSurfaceVDC( VDC_CHANNEL_0,  &surface );
    }

    /* Display On */
    if (VDC_OK == error)
    {
        extern void VDC_LcdPortSetting(uint32_t param);

        R_RVAPI_DispPortSettingVDC( VDC_CHANNEL_0,  &VDC_LcdPortSetting );
    }


    if (VDC_OK == error)
    {
        e = 0;
    }
    else
    {
        e = 1;
    }

    return  e;
}/* End of function gs_DisplayImage */


