/**********************************************************************************************************************
 * File Name    : ux_rz_dcd_hwsetup.c
 * Version      : x.xx
 * Description  : USB deice setup functions for USBX
 *********************************************************************************************************************/
#include "ux_api.h"
#include "ux_rz_dcd_hwsetup.h"

#include "r_usb_typedef.h"
#include "r_usb_extern.h"
#include "r_usb_reg_access.h"
#include "r_usb_basic_config.h"
#include "r_usb_basic_define.h"
#include "r_riic_drv_api.h"

#define USE_USBIP           (USB_CFG_USE_USBIP)

static void ux_PFSRegWriteProtectOFF(void);
static void ux_PFSRegWriteProtectON(void);

/******************************************************************************
 Function Name   : uz_rz_dcd_pin_setting
 Description     : USB port mode and Switch mode Initialize
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void uz_rz_dcd_pin_setting(void)
{
#if(BSP_CFG_BOARD == BSP_CFG_BOARD_RZA2_EVB)
    /* SW3 : IRQ6 */
    PORTJ.PDR.BIT.PDR1 = 0x0;
    PORTJ.PMR.BIT.PMR1 = 0x0;
//    GPIO.PJ1PFS.BIT.PSEL = 0x06;   /* ISEL = b'0, PSEL = b'110 */
#else /* GR-MANGO */
    /* SW3 : IRQ7 */
    PORTD.PDR.BIT.PDR7 = 0x0;
    PORTD.PMR.BIT.PMR7 = 0x0;
#endif
    ux_PFSRegWriteProtectOFF();
#if USE_USBIP == USB_IP0
    /* VBUSIN0 */
    PORT5.PMR.BIT.PMR2  = 0x0;
    GPIO.P52PFS.BYTE    = 0x03; /* VBUSIN0 */
    PORT5.PMR.BIT.PMR2 = 0x1;
#else   /* USE_USBIP == USE_USBIP0 */
    /* VBUSIN1 */
    PORT2.PMR.BIT.PMR2 = 0x0;
    GPIO.P22PFS.BYTE   = 0x5; /* VBUSIN1 */
    PORT2.PMR.BIT.PMR2 = 0x1;
#endif  /* USE_USBIP == USE_USBIP0 */
    ux_PFSRegWriteProtectON();

} /* End of function uz_rz_dcd_pin_setting */

/******************************************************************************
 Function Name   : uz_rz_dcd_open
 Description     : Setup of USB Driver
 Arguments       : none
 Return value    : none
 ******************************************************************************/
void uz_rz_dcd_open(int high_speed)
{
    usb_err_t   err;

//R_USB_Open

    /* USB module start setting */
    err = usb_module_start(USB_CFG_USE_USBIP);
    if (USB_SUCCESS == err)
    {
        /* Setting USB relation register  */
        hw_usb_pmodule_init();

		if(high_speed!=0)
		{
            hw_usb_set_hse(USB_NULL);
		}

        hw_usb_pclear_drpd();
        if ( USB_ATTACH == usb_pstd_chk_vbsts() )
        {
            hw_usb_set_cnen();
            usb_cpu_delay_xms((uint16_t)10);
            hw_usb_pset_dprpu();
        }
    }
} /* End of function uz_rz_dcd_open */

/***********************************************************************************************************************
* Function Name: ux_PFSRegWriteProtectOFF
* Description  : PFS register write protect OFF
* Arguments    : -
* Return Value : -
***********************************************************************************************************************/
static void ux_PFSRegWriteProtectOFF(void)
{
    GPIO.PWPR.BIT.B0WI = 0;
    GPIO.PWPR.BIT.PFSWE = 1;
}

/***********************************************************************************************************************
* Function Name: ux_PFSRegWriteProtectON
* Description  : PFS register write protect ON
* Arguments    : -
* Return Value : -
***********************************************************************************************************************/
static void ux_PFSRegWriteProtectON(void)
{
    GPIO.PWPR.BIT.PFSWE = 0;
    GPIO.PWPR.BIT.B0WI = 1;
}

