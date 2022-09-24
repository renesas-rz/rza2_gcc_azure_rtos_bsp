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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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
#define MB_REG_NUM_MAX (3)

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
static void MipiInt_isr(e_mipi_interrupt_type_t int_type);
static void SwapPoint (volatile uint32_t * point, uint32_t max_point);

/* Static variable */
volatile static int32_t vdc_captured = R_BCD_CAMERA_NOT_CAPTURED;
static uint8_t frame_buffer[R_BCD_CAMERA_FRAME_BUF_NUM][R_BCD_CAMERA_HEIGHT * R_BCD_CAMERA_WIDTH]
                    __attribute__ ((section("Capture_RAM")));
volatile static uint32_t mb_manage_table[MB_REG_NUM_MAX];   /* Current buffer point for input capture data */
volatile static uint32_t buff_write_point;   /* Current buffer point for input capture data */

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
        config.type           = VIN_INT_SCANLINE;

        /* Cast to an appropriate type */
        config.p_mipiCallback = NULL;
        config.p_vinCallback  = MipiInt_isr;
        config.line_num       = R_BCD_CAMERA_HEIGHT - 1;
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
    uint32_t count;

    /* Set all MB register before start capture */
    for (count = 0; count < MB_REG_NUM_MAX; count++)
    {
        R_RVAPI_SetBufferMIPI(count, &frame_buffer[buff_write_point][0]);
        mb_manage_table[count] = buff_write_point;
        SwapPoint(&buff_write_point, R_BCD_CAMERA_FRAME_BUF_NUM);
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
* Function Name: MipiInt_isr
* Description  : Function to handle interrupt of MIPI
*              : Find the frame buffer where the capture data is stored
*              : (Frame buffer is triple buffer)
* Arguments    : int_type
*              :   interrupt type (unused)
* Return Value : -
******************************************************************************/
static void MipiInt_isr(e_mipi_interrupt_type_t int_type)
{
    if ((int_type & VIN_INT_SCANLINE) == VIN_INT_SCANLINE)
    {
        st_vin_info_type_t info;

        /* Capture complete */
        R_MIPI_GetInfo(&info);
        vdc_captured = mb_manage_table[info.vin_nowcapturebase];
        R_RVAPI_SetBufferMIPI(info.vin_nowcapturebase, &frame_buffer[buff_write_point][0]);
        mb_manage_table[info.vin_nowcapturebase] = buff_write_point;
        SwapPoint(&buff_write_point, R_BCD_CAMERA_FRAME_BUF_NUM);         /* swap buffer point for next capture */
    }
    return;
}   /* End of function MipiInt_isr() */

/**********************************************************************
* Function Name: SwapPoint
* Description  : Swap Pointer Function
* Arguments    : point        : Address of pointer value
                 max_point    : Max of pointer value
* Return Value : none
**********************************************************************/
static void SwapPoint (volatile uint32_t * point, uint32_t max_point)
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
}   /* End of function SwapPoint() */

/* End of file */
