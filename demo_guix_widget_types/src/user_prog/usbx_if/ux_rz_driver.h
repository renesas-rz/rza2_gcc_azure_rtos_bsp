/**********************************************************************************************************************
 * File Name    : ux_rz_driver.h
 * Version      : x.xx
 * Description  : USBX driver for RZ
 *********************************************************************************************************************/
#ifndef __UX_RZ_DRIVER_H__
#define __UX_RZ_DRIVER_H__

UINT ux_rz_usbh_initialize(VOID);

UINT ux_rz_usbh0_start(VOID);
UINT ux_rz_usbh1_start(VOID);

UINT ux_rz_usbh0_stop(VOID);
UINT ux_rz_usbh1_stop(VOID);

#endif /* __UX_RZ_DRIVER_H__ */
