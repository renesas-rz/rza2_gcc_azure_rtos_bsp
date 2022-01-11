/**************************************************************************/
/**                                                                       */
/**   Core GR-MANGO SD Disk Driver Interface Routine                      */
/**                                                                       */
/**************************************************************************/

/* Include necessary system files.  */
#include <string.h>
#include <stdio.h>

#include "r_typedefs.h"
#include "r_sd_cfg.h"
#include "r_sdif.h"
#include "r_sdhi_simplified_drv_sc_cfg.h"
#include "r_cache_lld_rza2m.h"

#include "tx_api.h"
#include "fx_api.h"

#include "fx_rz_sd_driver.h"

/* The size of the work area used by the SD driver. */
#define _SD_PRV_RW_BUFF_SIZE     (1 * 1024)

/* Work buffers. */
#if defined(__GNUC__)
static ULONG auiSdWork[FX_RZ_SD_CONTROLER_MAX][SD_SIZE_OF_INIT / sizeof(uint32_t)] __attribute__ ((section ("UNCACHED_BSS"), aligned(8)));
static ULONG auiSdRwBuff[FX_RZ_SD_CONTROLER_MAX][_SD_PRV_RW_BUFF_SIZE / sizeof(uint32_t)] __attribute__ ((section ("UNCACHED_BSS"), aligned(8)));
#elif defined(__ICCARM__)
static ULONG auiSdWork[FX_RZ_SD_CONTROLER_MAX][SD_SIZE_OF_INIT / sizeof(uint32_t)] @ "UNCACHED_BSS";
static ULONG auiSdRwBuff[FX_RZ_SD_CONTROLER_MAX][_SD_PRV_RW_BUFF_SIZE / sizeof(uint32_t)] @ "UNCACHED_BSS";
#else
error!!
#endif /* __ICCARM__ */

/* SD information. */
static fx_rz_sd_info atSdInfo[FX_RZ_SD_CONTROLER_MAX] = {
    { SD_MEDIA_UNKNOWN, SD_WP_OFF },
    { SD_MEDIA_UNKNOWN, SD_WP_OFF }
};

/* SD controller register base address. */
static const ULONG auiSdBaseAddr[FX_RZ_SD_CONTROLER_MAX] = {
    SD_CFG_IP0_BASE,
    SD_CFG_IP1_BASE
};

UINT  _fx_partition_offset_calculate(void  *partition_sector, UINT partition,
                                     ULONG *partition_start, ULONG *partition_size);

/****************************************************************************/
/* FUNCTION:    _fx_rz_sd_open_dev                                          */
/*                                                                          */
/* DESCRIPTION: Open device                                                 */
/* INPUT:       iDev:                   device number                       */
/* OUTPUT:      none                                                        */
/*                                                                          */
/* RESULT:      FX_IO_ERROR:            error                               */
/*              FX_SUCCESS:             success                             */
/*                                                                          */
/****************************************************************************/
static int
_fx_rz_sd_open_dev(
    int             iDev)                   /* device number */
{
    USHORT          sType;
    LONG            iRetc;
    LONG            iRet;
    LONG            iPort;

    iRetc = FX_IO_ERROR;

    /* Check device number */
    if ((iDev < 0) || (iDev >= FX_RZ_SD_CONTROLER_MAX)) {
        /* device number bigger */
        return(iRetc);                      /* return error */
    }

    /* Initialize card information. */
    atSdInfo[iDev].iType = SD_MEDIA_UNKNOWN;
    atSdInfo[iDev].iWp = SD_WP_OFF;

    /* Initialize SD driver. */
    iRet = sd_init((LONG)iDev,
                    auiSdBaseAddr[iDev],
                    &auiSdWork[iDev][0],
                    SD_CD_SOCKET);
    if (SD_OK != iRet) {
        return(iRetc);                      /* return error */
    }

    /* Get channel number and set card detect interrupt. */
    iRet = sddev_get_sc_table_config_ch(iDev, &iPort);
    if (SD_OK == iRet) {
        if (SD_CB_USED == SDHI_SC_TABLE[iPort].config.cb) {
            /* Set the card detect interrupt. */
            iRet = sd_cd_int(iDev,
                             SD_CD_INT_ENABLE,
                             SDHI_SC_TABLE[iPort].config.cb_func);
        } else {
            /* Set the card detect interrupt. */
            iRet = sd_cd_int(iDev,
                             SD_CD_INT_ENABLE,
                             NULL);
        }
    }
    if (SD_OK != iRet) {
        return(iRetc);                      /* return error */
    }

    /* Check if the card is inserted. */
    iRet = sd_check_media(iDev);
    if (SD_OK != iRet) {
        /* The card is not inserted. */
        return(iRetc);                      /* return error */
    } else {
        /* The card is inserted. */
        /* Register callback functions. */
        iRet = sd_set_intcallback(iDev, &SD_status_callback_function);
        if (SD_OK != iRet) {
            return(iRetc);                  /* return error */
        }
        iRet = sd_set_dma_intcallback(iDev, &SD_dma_end_callback_function);
        if (SD_OK != iRet) {
            return(iRetc);                  /* return error */
        }

        /* Initialize SD driver work buffer. */
        iRet = sd_set_buffer(iDev,
                             &auiSdRwBuff[iDev][0],
                             _SD_PRV_RW_BUFF_SIZE);
        if (SD_OK != iRet) {
            return(iRetc);                  /* return error */
        }

        /* Mount SD card. */
        iRet = sd_mount(iDev, SD_CFG_DRIVER_MODE, SD_VOLT_3_3);
        if (SD_OK != iRet) {
            return(iRetc);                  /* return error */
        }

        /* Get the card type. */
        iRet = sd_get_type(iDev, &sType, NULL, NULL);
        if (SD_OK != iRet) {
            return(iRetc);                  /* return error */
        }
        atSdInfo[iDev].iType = (ULONG)sType;

        /* Get write protected information. */
        iRet = sd_iswp(iDev);
        if (SD_ERR == iRet) {
            return(iRetc);                  /* return error */
        }
        atSdInfo[iDev].iWp = iRet;
    }

    return(FX_SUCCESS);
}

/****************************************************************************/
/* FUNCTION:    _fx_rz_sd_close_dev                                         */
/*                                                                          */
/* DESCRIPTION: Close device                                                */
/* INPUT:       iDev:                   device number                       */
/* OUTPUT:      None                                                        */
/*                                                                          */
/* RESULT:      FX_SUCCESS:             success                             */
/*                                                                          */
/****************************************************************************/
static int
_fx_rz_sd_close_dev(
    int             iDev)                   /* device number */
{
    return(FX_SUCCESS);
}

/****************************************************************************/
/* FUNCTION:    _fx_rz_sd_read_dev                                          */
/*                                                                          */
/* DESCRIPTION: Read device                                                 */
/* INPUT:       iDev:                   device number                       */
/*              uiDevBlk:               device block number                 */
/*              iCnt:                   I/O count                           */
/* OUTPUT:      pucBuf:                 data read                           */
/*                                                                          */
/* RESULT:      FX_IO_ERROR:            error                               */
/*              FX_SUCCESS:             success                             */
/*                                                                          */
/****************************************************************************/
static LONG
_fx_rz_sd_read_dev(
    int             iDev,                   /* device number */
    ULONG           uiDevBlk,               /* device block number */
    UCHAR           *pucBuf,                /* I/O buffer */
    LONG            iCnt)                   /* I/O count */
{
    LONG            iRetc;
    LONG            iRet;

    iRetc = FX_IO_ERROR;

    /* Check device number. */
    if ((iDev < 0) || (iDev >= FX_RZ_SD_CONTROLER_MAX)) {
        /* Device number bigger. */
        return(iRetc);                      /* return error */
    }

    /* Read sd. */
    iRet = sd_read_sect(iDev,
                        pucBuf,
                        uiDevBlk,
                        iCnt);

    R_CACHE_L1DataCleanInvalidAll();

    if (iRet == SD_OK) {
        iRetc = FX_SUCCESS;
    }

    return(iRetc);
}

/****************************************************************************/
/* FUNCTION:    _fx_rz_sd_write_dev                                         */
/*                                                                          */
/* DESCRIPTION: Write device                                                */
/* INPUT:       iDev:                   device number                       */
/*              uiDevBlk:               device block number                 */
/*              iCnt:                   I/O count                           */
/*              pucBuf:                 data to write                       */
/* OUTPUT:      None                                                        */
/*                                                                          */
/* RESULT:      FX_IO_ERROR:            error                               */
/*              FX_SUCCESS:             success                             */
/*                                                                          */
/****************************************************************************/
static LONG
_fx_rz_sd_write_dev(
    int             iDev,                   /* device number */
    ULONG           uiDevBlk,               /* device block number */
    UCHAR           *pucBuf,                /* data to write */
    LONG            iCnt)                   /* I/O count */
{
    LONG            iRetc;
    LONG            iRet;

    iRetc = FX_IO_ERROR;

    /* Check device number. */
    if ((iDev < 0) || (iDev >= FX_RZ_SD_CONTROLER_MAX)) {
        /* Device number bigger. */
        return(iRetc);                      /* return error */
    }

    if (SD_WP_OFF != atSdInfo[iDev].iWp) {
        /* Write protect on. */
        return(iRetc);                      /* return error */
    }

    R_CACHE_L1DataCleanInvalidAll();

    /* Write sd. */
    iRet = sd_write_sect(iDev,
                         pucBuf,
                         uiDevBlk,
                         iCnt,
                         (int32_t)SD_WRITE_OVERWRITE);

    if (iRet == SD_OK) {
        iRetc = FX_SUCCESS;
    }

    return(iRetc);
}

/****************************************************************************/
/* FUNCTION:    _fx_rz_sd_driver                                            */
/*                                                                          */
/* DESCRIPTION: Device io routine                                           */
/* INPUT:       media_ptr:              media information                   */
/* OUTPUT:      media_ptr               media information                   */
/*                                                                          */
/* RESULT:      none                                                        */
/*                                                                          */
/****************************************************************************/
VOID  _fx_rz_sd_driver(FX_MEDIA *media_ptr)
{
    int             iDivCnt;
    LONG            iSecNum;
    LONG            iPatSize;
    LONG            iRetc;
    UINT            uiDevBlk;
    UCHAR           *source_buffer;
    UCHAR           *pucBuf;

#if 0
    switch(media_ptr -> fx_media_driver_request) {
        case FX_DRIVER_READ:
            printf("FX_DRIVER_READ\n");
            break;

        case FX_DRIVER_WRITE:
            printf("FX_DRIVER_WRITE\n");
            break;

        case FX_DRIVER_FLUSH:
            printf("FX_DRIVER_FLUSH\n");
            break;

        case FX_DRIVER_ABORT:
            printf("FX_DRIVER_ABORT\n");
            break;

        case FX_DRIVER_INIT:
            printf("FX_DRIVER_INIT\n");
            break;

        case FX_DRIVER_UNINIT:
            printf("FX_DRIVER_UNINIT\n");
            break;

        case FX_DRIVER_BOOT_READ:
            printf("FX_DRIVER_BOOT_READ\n");
            break;

        case FX_DRIVER_BOOT_WRITE:
            printf("FX_DRIVER_BOOT_WRITE\n");
            break;

        default:
            printf("default\n");
            break;
    }

    printf("memsz:%d sec:%d cnt:%d adr:0x%08x\n",
            media_ptr->fx_media_memory_size,
            media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,
            media_ptr->fx_media_driver_sectors,
            media_ptr->fx_media_driver_buffer);
#endif /* 0 */

    /* Process the driver request specified in the media control block.  */
    switch(media_ptr -> fx_media_driver_request) {
        case FX_DRIVER_READ:
            /****************************************************/
            /* check sector number                              */
            /****************************************************/
            if (media_ptr->fx_media_driver_sectors <= 0) {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }

            /****************************************************/
            /* set parametor                                    */
            /****************************************************/
            pucBuf   = (UCHAR *)media_ptr->fx_media_driver_buffer;
            uiDevBlk = media_ptr->fx_media_driver_logical_sector
                       + media_ptr->fx_media_hidden_sectors;
            iDivCnt  = media_ptr->fx_media_driver_sectors;

            /****************************************************/
            /* device read                                      */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_read_dev(FX_RZ_SD_PORT,
                                                                   uiDevBlk,
                                                                   pucBuf,
                                                                   iDivCnt);
            break;

        case FX_DRIVER_WRITE:
            /****************************************************/
            /* check sector number                              */
            /****************************************************/
            if (media_ptr->fx_media_driver_sectors <= 0) {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }

            /****************************************************/
            /* set parametor                                    */
            /****************************************************/
            pucBuf   = (UCHAR *)media_ptr->fx_media_driver_buffer;
            uiDevBlk = media_ptr->fx_media_driver_logical_sector
                       + media_ptr->fx_media_hidden_sectors;
            iDivCnt  = media_ptr->fx_media_driver_sectors;

            /****************************************************/
            /* device write                                     */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_write_dev(FX_RZ_SD_PORT,
                                                                    uiDevBlk,
                                                                    pucBuf,
                                                                    iDivCnt);
            break;

        case FX_DRIVER_FLUSH:
            /* Return driver success.  */
            media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;

        case FX_DRIVER_ABORT:
            /* Return driver success.  */
            media_ptr -> fx_media_driver_status =  FX_SUCCESS;
            break;

        case FX_DRIVER_INIT:
            /****************************************************/
            /* device open                                      */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_open_dev(FX_RZ_SD_PORT);
            if (FX_SUCCESS != media_ptr->fx_media_driver_status) {
                break;
            }

            /****************************************************/
            /* set write protect status                         */
            /****************************************************/
            if (SD_WP_OFF != atSdInfo[0].iWp) {
                media_ptr->fx_media_driver_write_protect = FX_TRUE;
            } else {
                media_ptr->fx_media_driver_write_protect = FX_FALSE;
            }
            break;

        case FX_DRIVER_UNINIT:
            /****************************************************/
            /* device close                                     */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_close_dev(FX_RZ_SD_PORT);
            break;

        case FX_DRIVER_BOOT_READ:
            /****************************************************/
            /* check sector number                              */
            /****************************************************/
            if (media_ptr->fx_media_driver_sectors <= 0) {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }

            /****************************************************/
            /* set parametor                                    */
            /****************************************************/
            pucBuf   = (UCHAR*)media_ptr->fx_media_driver_buffer;
            uiDevBlk = 0;
            iDivCnt  = 1;

            /****************************************************/
            /* device read                                      */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_read_dev(FX_RZ_SD_PORT,
                                                                   uiDevBlk,
                                                                   pucBuf,
                                                                   iDivCnt);
            if (FX_SUCCESS != media_ptr->fx_media_driver_status) {
                break;
            }

            /* Check partition read error.  */
            iSecNum = 0;
            if (_fx_partition_offset_calculate(pucBuf,
                                               0,
                                               &iSecNum,
                                               &iPatSize)) {
                /* Unsuccessful driver request.  */
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                break;
            }

            /* Now determine if there is a partition...   */
            if (0 == iSecNum) {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
                break;
            }

            /****************************************************/
            /* set parametor                                    */
            /****************************************************/
            pucBuf   = (UCHAR *)media_ptr->fx_media_driver_buffer;
            uiDevBlk = iSecNum;
            iDivCnt  = 1;

            /****************************************************/
            /* device read                                      */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_read_dev(FX_RZ_SD_PORT,
                                                                   uiDevBlk,
                                                                   pucBuf,
                                                                   iDivCnt);
            break;

        case FX_DRIVER_BOOT_WRITE:
            /****************************************************/
            /* set parametor                                    */
            /****************************************************/
            pucBuf   = (UCHAR *)media_ptr->fx_media_driver_buffer;
            uiDevBlk = media_ptr->fx_media_hidden_sectors;
            iDivCnt  = 1;

            /****************************************************/
            /* device write                                     */
            /****************************************************/
            media_ptr->fx_media_driver_status = _fx_rz_sd_write_dev(FX_RZ_SD_PORT,
                                                                    uiDevBlk,
                                                                    pucBuf,
                                                                    iDivCnt);
            break;

        default:
            /* Invalid driver request.  */
            media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
            break;
    }
}
