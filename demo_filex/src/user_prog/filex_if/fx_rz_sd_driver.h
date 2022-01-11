#ifndef __FX_RZ_SD_DRIVER_H__
#define __FX_RZ_SD_DRIVER_H__

#define FX_RZ_SD_PORT				(1)

#define FX_RZ_SD_CONTROLER_MAX		(2)

typedef struct fx_rz_sd_info_t {
	ULONG				iType;
	LONG				iWp;
} fx_rz_sd_info;

VOID  _fx_rz_sd_driver(FX_MEDIA *media_ptr);

#endif /* __FX_RZ_SD_DRIVER_H__ */

