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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_typedefs.h"
#include "mipi_camera.h"

#include "r_rvapi_mipi.h"
#include "r_bcd_camera.h"
#include "r_intc_drv_api.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define MB_REG_NUM_MAX  (3)
#define BUFF_SIZE       (R_BCD_CAMERA_HEIGHT * R_BCD_CAMERA_WIDTH)
#define MIPI_CAMERA_SUBADD (0x20)
#define GAIN_MAX    (49)

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
/* Static function */
static void mipi_int_isr(e_mipi_interrupt_type_t int_type);
static void swap_point (volatile uint32_t * point, uint32_t max_point);

/* Static variable */
static volatile int32_t vdc_captured = R_BCD_CAMERA_NOT_CAPTURED;
static uint8_t frame_buffer[R_BCD_CAMERA_FRAME_BUF_NUM][BUFF_SIZE] __attribute__ ((section("Capture_RAM"))); 
static volatile uint32_t mb_manage_table[MB_REG_NUM_MAX];   /* Current buffer point for input capture data */
static volatile uint32_t buff_write_point;   /* Current buffer point for input capture data */

static const uint8_t gaintbl[GAIN_MAX][3] = 
{
    { 0x03,0xE8,0x00 }, //   0dB  
    { 0x04,0x23,0x00 }, //   0.5dB
    { 0x04,0x62,0x00 }, //   1dB  
    { 0x04,0xA5,0x00 }, //   1.5dB
    { 0x04,0xEB,0x00 }, //   2dB  
    { 0x05,0x36,0x00 }, //   2.5dB
    { 0x05,0x85,0x00 }, //   3dB  
    { 0x05,0xD8,0x00 }, //   3.5dB
    { 0x06,0x31,0x00 }, //   4dB  
    { 0x06,0x8F,0x00 }, //   4.5dB
    { 0x06,0xF2,0x00 }, //   5dB  
    { 0x07,0x5C,0x00 }, //   5.5dB
    { 0x07,0xCB,0x00 }, //   6dB  
    { 0x08,0x41,0x00 }, //   6.5dB
    { 0x08,0xBF,0x00 }, //   7dB  
    { 0x09,0x43,0x00 }, //   7.5dB
    { 0x09,0xD0,0x00 }, //   8dB  
    { 0x0A,0x65,0x00 }, //   8.5dB
    { 0x0B,0x02,0x00 }, //   9dB  
    { 0x0B,0xA9,0x00 }, //   9.5dB
    { 0x0C,0x5A,0x00 }, //  10dB  
    { 0x0C,0x5A,0x0E }, //  10.5dB(Including analog gain 0.5dB)
    { 0x0C,0x5A,0x1C }, //  11dB  (Including analog gain 1dB)
    { 0x0C,0x5A,0x28 }, //  11.5dB(Including analog gain 1.5dB)
    { 0x0C,0x5A,0x35 }, //  12dB  (Including analog gain 2dB)
    { 0x0C,0x5A,0x40 }, //  12.5dB(Including analog gain 2.5dB)
    { 0x0C,0x5A,0x4B }, //  13dB  (Including analog gain 3dB)
    { 0x0C,0x5A,0x55 }, //  13.5dB(Including analog gain 3.5dB)
    { 0x0C,0x5A,0x5F }, //  14dB  (Including analog gain 4dB)
    { 0x0C,0x5A,0x67 }, //  14.5dB(Including analog gain 4.5dB)
    { 0x0C,0x5A,0x70 }, //  15dB  (Including analog gain 5dB)
    { 0x0C,0x5A,0x78 }, //  15.5dB(Including analog gain 5.5dB)
    { 0x0C,0x5A,0x80 }, //  16dB  (Including analog gain 6dB)
    { 0x0C,0x5A,0x87 }, //  16.5dB(Including analog gain 6.5dB)
    { 0x0C,0x5A,0x8E }, //  17dB  (Including analog gain 7dB)
    { 0x0C,0x5A,0x94 }, //  17.5dB(Including analog gain 7.5dB)
    { 0x0C,0x5A,0x9A }, //  18dB  (Including analog gain 8dB)
    { 0x0C,0x5A,0xA0 }, //  18.5dB(Including analog gain 8.5dB)
    { 0x0C,0x5A,0xA5 }, //  19dB  (Including analog gain 9dB)
    { 0x0C,0x5A,0xAA }, //  19.5dB(Including analog gain 9.5dB)
    { 0x0C,0x5A,0xAF }, //  20dB  (Including analog gain 10dB)
    { 0x0C,0x5A,0xB4 }, //  20.5dB(Including analog gain 10.5dB)
    { 0x0C,0x5A,0xB8 }, //  21dB  (Including analog gain 11dB)
    { 0x0C,0x5A,0xBC }, //  21.5dB(Including analog gain 11.5dB)
    { 0x0C,0x5A,0xC0 }, //  22dB  (Including analog gain 12dB)
    { 0x0C,0x5A,0xC3 }, //  22.5dB(Including analog gain 12.5dB)
    { 0x0C,0x5A,0xC6 }, //  23dB  (Including analog gain 13dB)
    { 0x0C,0x5A,0xCA }, //  23.5dB(Including analog gain 13.5dB)
    { 0x0C,0x5A,0xCD }  //  24dB  (Including analog gain 14dB)
};

/******************************************************************************
* Function Name: R_BCD_CameraGetFrameAddress
* Description  : Returns the address of the frame buffer with the number specified by the argument
* Arguments    : index
*              :   index of the frame buffer
* Return Value : address of the frame buffer
******************************************************************************/
uint8_t *R_BCD_CameraGetFrameAddress(int32_t index)
{
    return &frame_buffer[index][0];
}   /* End of function R_BCD_CameraGetFrameAddress() */

/******************************************************************************
* Function Name: R_BCD_CameraInit
* Description  : Initialization of camera capture processing(MIPI/VIN)
* Arguments    : -
* Return Value : 0  success
*                -1 fail
******************************************************************************/
int32_t R_BCD_CameraInit(void)
{
    int32_t retval = 0;
    e_mipi_error_t result;
    st_mipi_param_t mipi_data;    /* Config data for MIPI */
    st_vin_setup_t vin_setup;     /* Config data for VIN */
    
    /* Initialize MIPI config data */
    mipi_data.mipi_lanenum    = CAM_MIPI_LANE_NUM;
    mipi_data.mipi_vc         = CAM_MIPI_SEL_VC;
    mipi_data.mipi_interlace  = MIPI_PROGRESSIVE;
    mipi_data.mipi_laneswap   = CAM_MIPI_LANE_SWAP;
    mipi_data.mipi_frametop   = 0;
    mipi_data.mipi_outputrate = CAM_MIPI_TRANSFER_RATE;
    mipi_data.mipi_phy_timing.mipi_ths_prepare  = CAM_MIPI_THS_PREPARE;
    mipi_data.mipi_phy_timing.mipi_ths_settle   = CAM_MIPI_THS_SETTLE;
    mipi_data.mipi_phy_timing.mipi_tclk_prepare = CAM_MIPI_TCLK_PREPARE;
    mipi_data.mipi_phy_timing.mipi_tclk_settle  = CAM_MIPI_TCLK_SETTLE;
    mipi_data.mipi_phy_timing.mipi_tclk_miss    = CAM_MIPI_TCLK_MISS;
    mipi_data.mipi_phy_timing.mipi_t_init_slave = CAM_MIPI_T_INIT_SLAVE;
    
    /* Initialize VIN config data */
    vin_setup.vin_preclip.vin_preclip_starty = CAM_VIN_PRECLIP_START_Y;
    vin_setup.vin_preclip.vin_preclip_endy   = ((CAM_VIN_PRECLIP_START_Y + CAM_VIN_PRECLIP_WIDTH_Y) - 1);
    vin_setup.vin_preclip.vin_preclip_startx = CAM_VIN_PRECLIP_START_X;
    vin_setup.vin_preclip.vin_preclip_endx   = ((CAM_VIN_PRECLIP_START_X + CAM_VIN_PRECLIP_WIDTH_X) - 1);
    vin_setup.vin_inputformat  = CAM_VIN_INPUT_FORMAT;
    vin_setup.vin_outputformat = CAM_VIN_OUTPUT_FORMAT;
    vin_setup.vin_outputendian = CAM_VIN_OUTPUT_ENDIAN;
    vin_setup.vin_interlace    = VIN_PROGRESSIVE;
    vin_setup.vin_stride       = CAM_VIN_OUTPUT_IS;
    vin_setup.vin_ycoffset     = (CAM_VIN_OUTPUT_IS * CAM_VIN_PRECLIP_WIDTH_Y);
    vin_setup.vin_input_align  = CAM_VIN_INPUT_YCAL;
    vin_setup.vin_output_swap  = CAM_VIN_OUTPUT_BPSM;
    
    /* Internal static RAM initialize */
    buff_write_point = 0;

    /* MIPI I/F power on */
    Init_Camera();

    /* Initialize MIPI driver */
    R_RVAPI_InitializeMIPI();

    /* Initial setting of MIPI / VIN */
    result = R_RVAPI_OpenMIPI(&mipi_data);
    retval = (result == MIPI_OK) ? 0 : (-1);

    if (retval == 0)
    {
        result = R_RVAPI_SetupMIPI(&vin_setup);
        retval = (result == MIPI_OK) ? 0 : (-1);
    }

    /* Setting of interrupt callback */
    {
        st_mipi_int_t config;
        config.type           = VIN_INT_FIELD;

        /* Setting NULL */
        config.p_mipiCallback = NULL;
        config.p_vinCallback  = mipi_int_isr;
        config.line_num       = 0;
        R_RVAPI_InterruptEnableMIPI( &config );
    }

    return retval;
}   /* End of function R_BCD_CameraInit() */

/******************************************************************************
* Function Name: R_BCD_CameraCaptureStart
* Description  : Start camera capture processing(MIPI/VIN)
* Arguments    : -
* Return Value : 0  success
*                -1 fail
******************************************************************************/
int32_t R_BCD_CameraCaptureStart(void) 
{
    int32_t retval = 0;
    e_mipi_error_t result;
    uint8_t count;

    /* Set all MB register before start capture */
    for (count = 0; count < MB_REG_NUM_MAX; count++)
    {
        R_RVAPI_SetBufferMIPI(count, &frame_buffer[buff_write_point][0]);
        mb_manage_table[count] = buff_write_point;
        swap_point(&buff_write_point, R_BCD_CAMERA_FRAME_BUF_NUM);
    }
    result = R_RVAPI_CaptureStartMIPI();
    retval = (result == MIPI_OK) ? 0 : (-1);

    /* Camera clock start */
    Start_Camera();

    return retval;
}   /* End of function R_BCD_CameraCaptureStart() */

/******************************************************************************
* Function Name: R_BCD_CameraGetCaptureStatus
* Description  : This function returns the number of the frame buffer in
*              : which the capture data is stored.
* Arguments    : -
* Return Value : -1 (R_BCD_CAMERA_NOT_CAPTURED)
*              :   No data has been captured.
*              : 0
*              :   There is captured data in the 1st frame buffer.
*              : 1
*              :   There is captured data in the 2nd frame buffer.
*              : 2
*              :   There is captured data in the 3rd frame buffer.
******************************************************************************/
int32_t R_BCD_CameraGetCaptureStatus(void) 
{
    return vdc_captured;
}   /* End of function R_BCD_CameraGetCaptureStatus() */

/******************************************************************************
* Function Name: R_BCD_CameraClearCaptureStatus
* Description  : This function clears the variable indicating the frame buffer
*              : where the capture data is stored.
* Arguments    : -
* Return Value : -
******************************************************************************/
void R_BCD_CameraClearCaptureStatus(void)
{
    vdc_captured = R_BCD_CAMERA_NOT_CAPTURED;

    return;
}   /* End of function R_BCD_CameraClearCaptureStatus() */

/******************************************************************************
* Function Name: mipi_int_isr
* Description  : Function to handle interrupt of MIPI
*              : Find the frame buffer where the capture data is stored
*              : (Frame buffer is triple buffer)
* Arguments    : int_type
*              :   interrupt type (unused)
* Return Value : -
******************************************************************************/
static void mipi_int_isr(e_mipi_interrupt_type_t int_type)
{
    if ((int_type & VIN_INT_FIELD) == VIN_INT_FIELD)
    {
        st_vin_info_type_t info;

        /* Capture complete */
        R_MIPI_GetInfo(&info);
        vdc_captured = (int32_t)mb_manage_table[info.vin_nowcapturebase]; /* less than R_BCD_CAMERA_FRAME_BUF_NUM */
        /* This value is less than R_BCD_CAMERA_FRAME_BUF_NUM */
        R_RVAPI_SetBufferMIPI(info.vin_nowcapturebase, &frame_buffer[buff_write_point][0]);
        mb_manage_table[info.vin_nowcapturebase] = buff_write_point;
        swap_point(&buff_write_point, R_BCD_CAMERA_FRAME_BUF_NUM);         /* swap buffer point for next capture */
    }
    return;
}   /* End of function mipi_int_isr() */

/**********************************************************************
* Function Name: swap_point
* Description  : Swap Pointer Function
* Arguments    : point        : Address of pointer value
                 max_point    : Max of pointer value
* Return Value : none
**********************************************************************/
static void swap_point (volatile uint32_t * point, uint32_t max_point)
{
    /* This implicit casting is valid because either values are pointer */
    if (point != NULL )
    {
        *point = (*point) + 1;
        if ((*point) >= max_point)
        {
            *point = 0;
        }
    }
}   /* End of function swap_point() */

/**********************************************************************
* Function Name: R_BCD_SetCameraGain
* Description  : Set gain value to mipi camera
* Arguments    : gain        : gain 0(-12dB) to 30(+18dB)
* Return Value : none
**********************************************************************/
void R_BCD_SetCameraGain (uint8_t gain) 
{
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5A, gaintbl[gain][0]);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x5B, gaintbl[gain][1]);
    RIIC_MipiWrite(MIPI_CAMERA_SUBADD, 0x01, 0x57, gaintbl[gain][2]);
}   /* End of function R_BCD_SetCameraGain() */
/* End of file */
