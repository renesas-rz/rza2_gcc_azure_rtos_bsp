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
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
**********************************************************************************************************************/
/**********************************************************************************************************************
* System Name  : DRP Driver
* File Name    : r_dk2_if.c
* Device       : RZ/A2M
* Abstract     : Control software of DRP.
* Tool-Chain   : Renesas e2 studio
* OS           : None
* H/W Platform : RZ/A2M Evaluation Board
* Description  : Interface of DRP Driver.
* Limitation   : R_DK2_Uninitialize and R_DK2_Inactivate are not implemented.
**********************************************************************************************************************/
/**********************************************************************************************************************
* History      : History is managed by Revision Control System.
**********************************************************************************************************************/
/**********************************************************************************************************************
Includes <System Includes> , "Project Includes"
**********************************************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"
#include "r_mmu_lld_rza2m.h"
#include "r_cache_l1_rza2m_asm.h"
#include "r_intc_lld_rza2m.h"
#include "r_os_abstraction_api.h"
#include "r_dk2_core.h"
#include "r_dk2_if.h"

/**********************************************************************************************************************
Macro definitions
**********************************************************************************************************************/
#define R_DK2_VAR_MAX_INT
#define USE_UNCACHE_AREA

#define STATUS_UNINITIALIZED    (0)
#define STATUS_INITIALIZED      (1)
#define STANBYOUT_WAIT_COUNT    (10000)
#define TILE_PATTERN_NUM        (11)
#define MAX_FREQ                (66000)
#define MIN_FREQ                (2063)
#define INVALID_FREQ            (0xFFFFFFFF)
#define ORIGINAL_FREQ           (264000)
#define ROUND_UP_DIV(x, y)      (((x) + (y) - 1) / (y))

/* for config_parser */
#define CONFIG_ALIGN            (32)
#define CONFIG_ALIGN_MASK       (0x1F)
#define SECTION_SIZE_LEN        (4)
#define SECTION_ID_LEN          (4)
#define COMMON_SECTION_ID       ("dk2c")
#define STP_SECTION_ID          ("stpd")
#define BYTE_NUM_OF_BYTE        (1)
#define BYTE_NUM_OF_HWORD       (2)
#define BYTE_NUM_OF_WORD        (4)
#define TARGET_DEVIVCE_LEN      (16)
#define COMMON_RESERVE_LEN      (25)
#define CONTEXT_MAX             (64)
#define STATE_MAX               (256)
#define SECTION_NAME_LEN        (32)
#define NAME_BUF_SIZE           (SECTION_NAME_LEN + 1)
#define U64_DATA_SIZE           (8)
#define SECTION_CRC_LEN         (4)
#define SECTION_DII_LEN         (32)
#define INT_STATUS_SHIFT        (16)
#define INT_CH_MASK             (0x3F)

/* for CRC */
#define CRC32_GEN_POLYNOMIAL    (0xEDB88320u)
#define CRC32_BIT_LOOP_NUM      (8)
#define CRC32_INITIAL_VALUE     (0xFFFFFFFFu)
#define CRC32_XOR_VALUE         (0xFFFFFFFFu)

#define MUTEX_WAIT              (100)

/**********************************************************************************************************************
Typedef definitions
**********************************************************************************************************************/
typedef char dk2_char_t;

typedef struct config_parser_info_st
{
    uint8_t *p_config;
    uint32_t index;
    uint32_t section_index;
    uint32_t section_size;
} config_parser_info_t;

typedef struct config_dat_info_st
{
    uint32_t *p_array;
    uint32_t array_num;
    uint32_t cid;
    uint16_t minor_ver;
    uint8_t tile_num;
    uint32_t freq;
    uint8_t del_zero;
    uint8_t dfc;
    uint16_t context;
    uint16_t state;
} config_dat_info_t;

/**********************************************************************************************************************
Imported global variables and functions (from other files)
**********************************************************************************************************************/

/**********************************************************************************************************************
Private variables and functions(prototypes)
**********************************************************************************************************************/
static uint8_t status = STATUS_UNINITIALIZED;
static uint8_t aid[R_DK2_TILE_NUM];
static uint8_t adfc[R_DK2_TILE_NUM];
static uint32_t amax[R_DK2_TILE_NUM];
static uint32_t afreq[R_DK2_TILE_NUM];
static uint8_t astart[R_DK2_TILE_NUM];
static uint8_t acount[R_DK2_TILE_NUM]; /* Count of interrupt */
static load_cb_t apload[R_DK2_TILE_NUM];
static int_cb_t apint[R_DK2_TILE_NUM];
#ifdef __GNUC__
#ifdef USE_UNCACHE_AREA
__attribute__ ((aligned(16), section("UNCACHED_BSS")))
#else
__attribute__ ((aligned(16)))
#endif
#endif
static uint8_t work_load[R_DK2_TILE_NUM][16];
#ifdef __GNUC__
#ifdef USE_UNCACHE_AREA
__attribute__ ((aligned(16), section("UNCACHED_BSS")))
#else
__attribute__ ((aligned(16)))
#endif
#endif
static uint8_t work_start[R_DK2_TILE_NUM][32];

static p_mutex_t mutex_hdl;

static dk2_char_t name_buf[NAME_BUF_SIZE];

static const uint32_t apattern[TILE_PATTERN_NUM] = 
{
    R_DK2_TILE_PATTERN_1_1_1_1_1_1,
    R_DK2_TILE_PATTERN_2_1_1_1_1,
    R_DK2_TILE_PATTERN_2_2_1_1,
    R_DK2_TILE_PATTERN_2_2_2,
    R_DK2_TILE_PATTERN_3_1_1_1,
    R_DK2_TILE_PATTERN_3_2_1,
    R_DK2_TILE_PATTERN_3_3,
    R_DK2_TILE_PATTERN_4_1_1,
    R_DK2_TILE_PATTERN_4_2,
    R_DK2_TILE_PATTERN_5_1,
    R_DK2_TILE_PATTERN_6,
};
static const uint8_t ahead[TILE_PATTERN_NUM] =
{
    0x3F, /* R_DK2_TILE_PATTERN_1_1_1_1_1_1 */
    0x3D, /* R_DK2_TILE_PATTERN_2_1_1_1_1 */
    0x35, /* R_DK2_TILE_PATTERN_2_2_1_1 */
    0x15, /* R_DK2_TILE_PATTERN_2_2_2 */
    0x39, /* R_DK2_TILE_PATTERN_3_1_1_1 */
    0x29, /* R_DK2_TILE_PATTERN_3_2_1 */
    0x09, /* R_DK2_TILE_PATTERN_3_3 */
    0x31, /* R_DK2_TILE_PATTERN_4_1_1 */
    0x11, /* R_DK2_TILE_PATTERN_4_2 */
    0x21, /* R_DK2_TILE_PATTERN_5_1 */
    0x01, /* R_DK2_TILE_PATTERN_6 */
};
static const uint8_t atail[TILE_PATTERN_NUM] =
{
    0x3F, /* R_DK2_TILE_PATTERN_1_1_1_1_1_1 */
    0x3E, /* R_DK2_TILE_PATTERN_2_1_1_1_1 */
    0x3A, /* R_DK2_TILE_PATTERN_2_2_1_1 */
    0x2A, /* R_DK2_TILE_PATTERN_2_2_2 */
    0x3C, /* R_DK2_TILE_PATTERN_3_1_1_1 */
    0x34, /* R_DK2_TILE_PATTERN_3_2_1 */
    0x24, /* R_DK2_TILE_PATTERN_3_3 */
    0x38, /* R_DK2_TILE_PATTERN_4_1_1 */
    0x28, /* R_DK2_TILE_PATTERN_4_2 */
    0x30, /* R_DK2_TILE_PATTERN_5_1 */
    0x20, /* R_DK2_TILE_PATTERN_6 */
};

/* Prototypes */
static int32_t get_tile_index(const uint8_t id);
#if defined(R_DK2_VAR_MAX_INT) || defined(R_DK2_ENABLE_INTN)
static int32_t get_tile_num(const uint8_t id);
#endif
static int32_t get_status(const uint8_t id);
static int32_t analyze_config(const void *const pconfig, const bool crc_check, dk2_char_t *const pname,
    config_dat_info_t *const presult);
static int32_t analyze_common_section(const bool crc_check, config_parser_info_t *const pinfo,
    config_dat_info_t *const presult);
static int32_t analyze_stp_section(dk2_char_t *const pname, const bool crc_check, config_parser_info_t *const pinfo,
    config_dat_info_t *const presult);
static int32_t init_config_parser(const void *const pconfig, config_parser_info_t *const pinfo);
static int32_t read_config_section(const dk2_char_t *const pid, config_parser_info_t *const pinfo);
static int32_t read_config_word(uint32_t *const pword, const uint32_t *const pexpected,
    config_parser_info_t *const pinfo);
static int32_t read_config_hword(uint16_t *const phword, const uint16_t *const pexpected,
    config_parser_info_t *const pinfo);
static int32_t read_config_byte(uint8_t *const pbyte, const uint8_t *const pexpected,
    config_parser_info_t *const pinfo);
static int32_t read_config_str(dk2_char_t *const pstr, const dk2_char_t *const pexpeced, const uint32_t size,
    config_parser_info_t *const pinfo);
static int32_t read_config_data(const uint32_t **const pdata, const uint32_t size, config_parser_info_t *const pinfo);
static uint32_t calc_crc(const uint8_t *const buf, const uint32_t len);
static void dk2_nmlint_isr(uint32_t int_sense);

/**********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Initialize
* Description  : This function causes DRP to be initialized.
* Arguments    : None.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Initialize(void)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    volatile uint8_t dummy_buf;
    int32_t wait_count;
    
    if (STATUS_UNINITIALIZED != status)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        aid[tile_index] = 0;
        adfc[tile_index] = 0;
        amax[tile_index] = MAX_FREQ;
        afreq[tile_index] = INVALID_FREQ;
        astart[tile_index] = 0;
        acount[tile_index] = 0;
        apload[tile_index] = NULL;
        apint[tile_index] = NULL;
    }
    
    mutex_hdl = R_OS_MutexCreate();
    if (NULL == mutex_hdl)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    R_INTC_SetPriority(INTC_ID_DRP_NMLINT, DRP_INTERRUPT_PRIORITY);
    R_INTC_RegistIntFunc(INTC_ID_DRP_NMLINT, &dk2_nmlint_isr);
    R_INTC_Enable(INTC_ID_DRP_NMLINT);
    
    /* DRP Clock on */
    CPG.STBCR9.BIT.MSTP90 = 0;
    dummy_buf = CPG.STBCR9.BYTE; /* Dummy read */
    
    /* DRP standby out */
    wait_count = 0;
    CPG.STBREQ2.BIT.STBRQ24 = 0;
    while(0 != CPG.STBACK2.BIT.STBAK24)
    {
        if (STANBYOUT_WAIT_COUNT < wait_count)
        {
            result = R_DK2_ERR_STATUS;
            goto func_end;
        }
        wait_count++;
    }
    
    R_DK2_CORE_Initialize();
    
    status = STATUS_INITIALIZED;
    
    goto func_end;
    
func_end:
    return result;
}
/**********************************************************************************************************************
End of function R_DK2_Initialize
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Uninitialize
* Description  : This function causes DRP to be uninitialized.
* Arguments    : None.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Uninitialize(void)
{
    /* Future planned implementation */
    return R_DK2_ERR_INTERNAL;
}
/**********************************************************************************************************************
End of function R_DK2_Uninitialize
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Load
* Description  : This function loads DRP circuit to DRP.
* Arguments    : pconfig - Address of configuration data.
                 top_tiles - Top tiles of DRP circuit to be loaded.
                 tile_pat - Pattern of tile layout.
                 pload - Callback function to notify completion of loading.
                 pint - Callback function to notify interrupt from DRP circuit.
                 pid - Array represents tile layout of DRP circuit.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Load(const void *const pconfig, const uint8_t top_tiles, const uint32_t tile_pattern,
    const load_cb_t pload, const int_cb_t pint, uint8_t *const paid)
{
    int32_t result;
    uint8_t bottom_tiles;
    int32_t index;
    int32_t tile_index;
    int32_t tile_pos;
    int32_t tile_config;
    void *work_uc;
    uint8_t id;
    uint8_t loaded_id[R_DK2_TILE_NUM];
    int32_t loaded_id_index;
    bool mutex_result = false;
    uint32_t freq_khz;
    bool del_zero;
    config_dat_info_t analyze_result;

    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* write paid before loading */
    if (NULL != paid)
    {
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            paid[tile_index] = aid[tile_index];
        }
    }
    
    if ((NULL == pconfig) || (0 != ((uint32_t)pconfig & 0x0000001F)) || (0 == top_tiles)
                                                                                    || (0 != (top_tiles & (~0x3F))))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /***** Unimplemented *****/
    if (NULL != pload)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }
    
    /* analyze configuration data */
    result = analyze_config(pconfig, false, NULL, &analyze_result);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    
    /* check tile pattern */
    bottom_tiles = (uint8_t)(top_tiles << (analyze_result.tile_num - 1));
    for (index = 0; index < TILE_PATTERN_NUM; index++)
    {
        if (tile_pattern == apattern[index])
        {
            if ((top_tiles & ahead[index]) != top_tiles)
            {
                result = R_DK2_ERR_TILE_PATTERN;
                goto func_end;
            }
            if ((bottom_tiles & atail[index]) != bottom_tiles)
            {
                result = R_DK2_ERR_TILE_PATTERN;
                goto func_end;
            }
            break;
        }
    }
    if (index >= TILE_PATTERN_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* get tile position */
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (0 != ((top_tiles >> tile_index) & 1))
        {
            break;
        }
    }
    if (tile_index >= R_DK2_TILE_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    tile_pos = tile_index;
    
    /* check free tiles  */
    tile_config = R_DK2_TILE_NUM;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (tile_config < R_DK2_TILE_NUM)
        {
            if (0 != aid[tile_index])
            {
                result = R_DK2_ERR_OVERWRITE;
                goto func_end;
            }
        }
        else if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            if (0 != aid[tile_index])
            {
                result = R_DK2_ERR_OVERWRITE;
                goto func_end;
            }
            tile_config = tile_index;
        }
        else
        {
            /* DO NOTHING */
        }
        tile_config++;
        if (tile_config >= (tile_pos + analyze_result.tile_num))
        {
            tile_config = R_DK2_TILE_NUM;
        }
        else if (0 != ((top_tiles >> tile_config) & R_DK2_TILE_0))
        {
            result = R_DK2_ERR_OVERWRITE;
            goto func_end;
        }
        else
        {
            /* DO NOTHING */
        }
    }
    
#ifdef USE_UNCACHE_AREA
    work_uc = (void *)&work_load[tile_pos][0];
#else
    work_uc = (void *)((uint32_t)&work_load[tile_pos][0] | 0x2000000);
#endif
    id = 0;

    {
        uint32_t paddr;

        del_zero = (0 != analyze_result.del_zero) ? true : false;

        R_MMU_VAtoPA((uint32_t)analyze_result.p_array, &paddr);
        result = R_DK2_CORE_PreLoad(analyze_result.tile_num, top_tiles, paddr, analyze_result.array_num * 8, del_zero,
            analyze_result.context, analyze_result.state, work_uc, &id);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    
        R_MMU_VAtoPA((uint32_t)&work_load[tile_pos][0], &paddr);
        result = R_DK2_CORE_Load(id, top_tiles, tile_pattern, analyze_result.state, paddr, &loaded_id[0]);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    }
    
    /* write paid after loading */
    loaded_id_index = 0;
    tile_pos = R_DK2_TILE_NUM;
    if ((1 <= analyze_result.minor_ver) && (0 != analyze_result.freq))
    {
        freq_khz = 1000000000 / analyze_result.freq;
    }
    else
    {
        freq_khz = MAX_FREQ;
    }
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if ((tile_pos < R_DK2_TILE_NUM) && ((tile_index - tile_pos) < analyze_result.tile_num))
        {
            aid[tile_index] = aid[tile_index - 1];
            adfc[tile_index] = analyze_result.dfc;
            amax[tile_index] = freq_khz;
            if (NULL != pload)
            {
                apload[tile_index] = pload;
            }
            if (NULL != pint)
            {
                apint[tile_index] = pint;
            }
            if (NULL != paid)
            {
                paid[tile_index] = aid[tile_index];
            }
        }
        else if (0 != ((top_tiles >> tile_index) & R_DK2_TILE_0))
        {
            tile_pos = tile_index;
            aid[tile_index] = loaded_id[loaded_id_index++];
            adfc[tile_index] = analyze_result.dfc;
            amax[tile_index] = freq_khz;
            if (NULL != pload)
            {
                apload[tile_index] = pload;
            }
            if (NULL != pint)
            {
                apint[tile_index] = pint;
            }
            if (NULL != paid)
            {
                paid[tile_index] = aid[tile_index];
            }
        }
        else
        {
            /* DO NOTHING */
        }
    }

    goto func_end;
    
func_end:
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Load = %d: pconfig = %08Xh, top_tiles = %02X, tile_pattern = %02X\n  id =", result,
                                                                        (uint32_t)pconfig, top_tiles, tile_pattern);
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            printf(" %02X", paid[tile_index]);
        }
        printf("\n");
    }
#endif /* DRP_DEBUG */

    return result;
}
/**********************************************************************************************************************
End of function R_DK2_Load
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Unload
* Description  : This function unloads DRP circuit from DRP.
* Arguments    : id - ID of DRP circuit to be unloaded.
                 pid - Array represents tile layout of DRP circuits.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Unload(const uint8_t id, uint8_t *const paid)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    bool mutex_result = false;
    int32_t id_index;
    int32_t id_num;
    uint8_t id_target;
    uint8_t aid_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    /* Exclusion control */
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* write paid before unloading */
    if (NULL != paid)
    {
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            paid[tile_index] = aid[tile_index];
        }
    }
    
    /* Search target ID. */
    if (0 == id) /* target is all circuits. */
    {
        /* Search target from all circuits. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (0 != aid[tile_index])
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    id_num++;
                }
                else
                {
                    /* DO NOTHING */
                }
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    else /* Target is specified by argument id. */
    {
        /* Search target from argument id. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && ((id & aid[tile_index]) == aid[tile_index]))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    id_num++;
                }
                else
                {
                    /* DO NOTHING */
                }
            }
            else if ((id & aid[tile_index]) != 0)
            {
                result = R_DK2_ERR_ARG;
                goto func_end;
            }
            else
            {
                /* DO NOTHING */
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    
    /* Make target ID */
    id_target = 0;
    for (id_index = 0; id_index < id_num; id_index++)
    {
        id_target |= aid_target[id_index];
    }
    if ((0 != id) && (id != id_target))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    for (id_index = 0; id_index < id_num; id_index++)
    {
        result = R_DK2_CORE_Unload(aid_target[id_index]);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
        
        /* write paid after unloading */
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (aid[tile_index] == aid_target[id_index])
            {
                aid[tile_index] = 0;
                adfc[tile_index] = 0;
                amax[tile_index] = MAX_FREQ;
                afreq[tile_index] = INVALID_FREQ;
                astart[tile_index] = 0;
                acount[tile_index] = 0;
                apload[tile_index] = NULL;
                apint[tile_index] = NULL;
                if (NULL != paid)
                {
                    paid[tile_index] = 0;
                }
            }
            else
            {
                paid[tile_index] = aid[tile_index];
            }
        }
    }
    
func_end:
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Unload = %d: id = %02Xh\n  id =", result, id);
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            printf(" %02X", paid[tile_index]);
        }
        printf("\n");
    }
#endif /* DRP_DEBUG */

    return result;
}
/**********************************************************************************************************************
End of function R_DK2_Unload
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Activate
* Description  : This function activates and set up DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit to be started.
                 freq - Frequency of DRP circuit to be started.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Activate(const uint8_t id, const uint32_t freq)
{
    int32_t result = R_DK2_SUCCESS;
    int32_t tile_index;
    uint8_t div;
    bool mutex_result = false;
    int32_t id_index;
    int32_t id_num;
    uint8_t id_target;
    uint8_t aid_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    int32_t aindex_target[R_DK2_TILE_NUM] = { 0, 0, 0, 0, 0, 0 };
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    /* Exclusion control */
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    /* Search target ID. */
    if (0 == id) /* target is all circuits. */
    {
        /* Search target from all circuits. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && (R_DK2_STATUS_LOADED == get_status(aid[tile_index])))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    aindex_target[0] = tile_index;
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    aindex_target[id_num] = tile_index;
                    id_num++;
                }
                else
                {
                    /* DO NOTHING */
                }
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_STATUS;
            goto func_end;
        }
    }
    else /* Target is specified by argument id. */
    {
        /* Search target from argument id. */
        id_num = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((0 != aid[tile_index]) && ((id & aid[tile_index]) == aid[tile_index]))
            {
                if (0 == id_num)
                {
                    aid_target[0] = aid[tile_index];
                    aindex_target[0] = tile_index;
                    id_num = 1;
                }
                else if (aid[tile_index] != aid_target[id_num - 1])
                {
                    aid_target[id_num] = aid[tile_index];
                    aindex_target[id_num] = tile_index;
                    id_num++;
                }
                else
                {
                    /* DO NOTHING */
                }
            }
            else if ((id & aid[tile_index]) != 0)
            {
                result = R_DK2_ERR_ARG;
                goto func_end;
            }
            else
            {
                /* DO NOTHING */
            }
        }
        
        /* Target is not found. */
        if (0 == id_num)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
        
        /* Check status of circuits. */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (R_DK2_STATUS_LOADED != get_status(aid_target[id_index]))
            {
                result = R_DK2_ERR_STATUS;
                goto func_end;
            }
        }
    }
    
    /* Make target ID */
    id_target = 0;
    for (id_index = 0; id_index < id_num; id_index++)
    {
        id_target |= aid_target[id_index];
    }
    if ((0 != id) && (id != id_target))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    /* Check frequency and culculate divison factor */
    if (0 == freq)
    {
        /* dfc check */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (0 == adfc[aindex_target[id_index]])
            {
                result = R_DK2_ERR_FORMAT;
                goto func_end;
            }
        }
        
        div = 0;
    }
    else if (freq < MIN_FREQ)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    else
    {
        uint32_t max_freq = MAX_FREQ;
        uint32_t u32div;
        
        /* decide maximum of frequency */
        for (id_index = 0; id_index < id_num; id_index++)
        {
            if (max_freq > amax[aindex_target[id_index]])
            {
                max_freq = amax[aindex_target[id_index]];
            }
        }
        
        if (freq > max_freq)
        {
            result = R_DK2_ERR_FREQ;
            goto func_end;
        }
        
        u32div = ROUND_UP_DIV(ORIGINAL_FREQ, freq);
        if ((128 < u32div) || (4 > u32div))
        {
            result = R_DK2_ERR_FREQ;
            goto func_end;
        }
        div = (uint8_t)u32div;
    }
    
    for (id_index = 0; id_index < id_num; id_index++)
    {
        result = R_DK2_CORE_Activate(aid_target[id_index], div);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
        
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (aid[tile_index] == aid_target[id_index])
            {
                afreq[tile_index] = freq;
            }
        }
    }

func_end:
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Activate = %d: id = %02Xh, freq = %d\n", result, id, freq);
    }
#endif /* DRP_DEBUG */

    return result;
}
/**********************************************************************************************************************
End of function R_DK2_Activate
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Inactivate
* Description  : This function inactivates DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit to be started.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_Inactivate(const uint8_t id)
{
    /* Future planned implementation */
    return R_DK2_ERR_INTERNAL;
}
/**********************************************************************************************************************
End of function R_DK2_Inactivate
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_Start
* Description  : This function starts DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit.
                 pparam - Address of parameters.
                 size - Size of parameters.
                 intn - Number of interrupts.
* Return Value : Error code.
**********************************************************************************************************************/
#ifdef R_DK2_ENABLE_INTN
int32_t R_DK2_Start(const uint8_t id, const void *const pparam, const uint32_t size, const uint8_t intn)
#else
int32_t R_DK2_Start(const uint8_t id, const void *const pparam, const uint32_t size)
#endif
{
    int32_t result;
    int32_t circuit_status;
    int32_t tile_index;
    int32_t tile_pos;
#if defined(R_DK2_VAR_MAX_INT) || defined(R_DK2_ENABLE_INTN)
    int32_t tile_num;
#endif
    uint8_t max_int;
    void *work_uc;
    bool mutex_result = false;
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    if ((0 == id) || (NULL == pparam) || (0 == size))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    circuit_status = get_status(id);
    if (0 > circuit_status)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    if (R_DK2_STATUS_ACTIVATED != circuit_status)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    tile_pos = get_tile_index(id);
    if (tile_pos >= R_DK2_TILE_NUM)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
#if defined(R_DK2_VAR_MAX_INT) && !defined(R_DK2_ENABLE_INTN)
    tile_num = get_tile_num(id);
    if ((tile_num == 0) || (tile_num > R_DK2_TILE_NUM))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }
    
    if (tile_num == 1)
    {
        max_int = 1;
    }
    else if (tile_num == 6)
    {
        max_int = 4;
    }
    else /* tile_num = 2, 3, 4, 5 */
    {
        max_int = 2;
    }
#elif defined(R_DK2_ENABLE_INTN)
    max_int = intn;
    if (max_int == 0)
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }

    tile_num = get_tile_num(id);
    if ((tile_num == 0) || (tile_num > R_DK2_TILE_NUM))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }

    if (tile_num == 1)
    {
        if (max_int > 1)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    else if (tile_num == 6)
    {
        if (max_int > 4)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
    else /* tile_num = 2, 3, 4, 5 */
    {
        if (max_int > 2)
        {
            result = R_DK2_ERR_ARG;
            goto func_end;
        }
    }
#else
    max_int = 1;
#endif
    
#ifdef USE_UNCACHE_AREA
    work_uc = (void *)&work_start[tile_pos][0];
#else
    work_uc = (void *)((uint32_t)&work_start[tile_pos][0] | 0x2000000);
#endif
    {
        uint32_t paddr;

        R_MMU_VAtoPA((uint32_t)pparam, &paddr);
        result = R_DK2_CORE_PreStart(id, work_uc, paddr, size);
        if (R_DK2_SUCCESS != result)
        {
            goto func_end;
        }
    }

    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (id == aid[tile_index])
        {
            astart[tile_index] = id;
            acount[tile_index] = max_int;
        }
    }
    
    {
        uint32_t paddr;

        R_MMU_VAtoPA((uint32_t)&work_start[tile_pos][0], &paddr);
        result = R_DK2_CORE_Start(id, paddr);
        if (R_DK2_SUCCESS != result)
        {
            for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
            {
                if (id == aid[tile_index])
                {
                    astart[tile_index] = 0;
                    acount[tile_index] = 0;
                }
            }
            goto func_end;
        }
    }
    
func_end:
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
    
#ifdef DRP_DEBUG
    if (result < 0)
    {
        printf("R_DK2_Start = %d: id = %02Xh, pparam = %08X, size = %d\n", result, id, (uint32_t)pparam, size);
    }
#endif /* DRP_DEBUG */

    return result;
}
/**********************************************************************************************************************
End of function R_DK2_Start
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_GetStatus
* Description  : This function gets status of DRP circuit loaded in DRP.
* Arguments    : id - ID of DRP circuit.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_GetStatus(const uint8_t id)
{
    int32_t result;
    bool mutex_result = false;
    
    /* Check status of DRP Driver */
    if (status == STATUS_UNINITIALIZED)
    {
        result = R_DK2_ERR_STATUS;
        goto func_end;
    }
    
    mutex_result = R_OS_MutexWait(&mutex_hdl, MUTEX_WAIT);
    if (false == mutex_result)
    {
        result = R_DK2_ERR_OS;
        goto func_end;
    }
    
    result = get_status(id);
    
    goto func_end;
    
func_end:
    if (false != mutex_result)
    {
        R_OS_MutexRelease(mutex_hdl);
    }
    
    return result;
}
/**********************************************************************************************************************
End of function R_DK2_GetStatus
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_GetInfo
* Description  : This function gets infomation of DRP configuration data.
* Arguments    : pconfig - Address of DRP configuration data.
                 pinfo - Address of infomation of DRP configuration data.
                 crc_check - Whether or not to check CRC.
* Return Value : Error code.
**********************************************************************************************************************/
int32_t R_DK2_GetInfo(const void *const pconfig, config_info_t *const pinfo, const bool crc_check)
{
    int32_t result = R_DK2_SUCCESS;
    uint32_t index;
    uint32_t ver;
    uint32_t *p_data;
    uint32_t data0;
    uint32_t data1;
    config_dat_info_t analyze_result;

    if ((NULL == pconfig) || (NULL == pinfo))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }

    result = analyze_config(pconfig, crc_check, &name_buf[0], &analyze_result);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    ver = 0;
    p_data = analyze_result.p_array;
    for (index = 0; index < analyze_result.array_num; index++)
    {
        data0 = *p_data++;
        data1 = *p_data++;
        if (data1 == 0xF01D2300)
        {
            ver = data0;
        }
    }

    pinfo->type = 0;
    pinfo->pname = &name_buf[0];
    pinfo->cid = analyze_result.cid;
    pinfo->ver = ver;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function R_DK2_GetInfo
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: R_DK2_GetVersion
* Description  : This function gets version of DRP driver.
* Arguments    : None.
* Return Value : Version of DRP driver.
**********************************************************************************************************************/
uint32_t R_DK2_GetVersion(void)
{
    return R_DK2_CORE_GetVersion();
}
/**********************************************************************************************************************
End of function R_DK2_GetVersion
**********************************************************************************************************************/

/**********************************************************************************************************************
Private functions
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: get_tile_index
* Description  : This function gets index of DRP circuit.
* Arguments    : id - ID of circuit.
* Return Value : Index of DRP circuit.
**********************************************************************************************************************/
static int32_t get_tile_index(const uint8_t id)
{
    int32_t tile_index;

    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (aid[tile_index] == id)
        {
            break;
        }
    }

    return tile_index;
}
/**********************************************************************************************************************
End of function get_tile_index
**********************************************************************************************************************/

#if defined(R_DK2_VAR_MAX_INT) || defined(R_DK2_ENABLE_INTN)
/**********************************************************************************************************************
* Function Name: get_tile_num
* Description  : This function gets number of DRP tiles.
* Arguments    : id - ID of circuit.
* Return Value : Number of DRP tiles.
**********************************************************************************************************************/
static int32_t get_tile_num(const uint8_t id)
{
    int32_t tile_index;
    int32_t tile_num;

    tile_num = 0;
    for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
    {
        if (aid[tile_index] == id)
        {
            tile_num++;
        }
    }

    return tile_num;
}
/**********************************************************************************************************************
End of function get_tile_num
**********************************************************************************************************************/
#endif /* R_DK2_VAR_MAX_INT || R_DK2_ENABLE_INTN */

/**********************************************************************************************************************
* Function Name: get_status
* Description  : This function gets status of DRP circuit.
* Arguments    : id - ID of circuit.
* Return Value : Status of DRP circuit.
**********************************************************************************************************************/
static int32_t get_status(const uint8_t id)
{
    int32_t result;
    int32_t tile_index;
    
    tile_index = get_tile_index(id);
    
    if ((0 == id) || (tile_index >= R_DK2_TILE_NUM))
    {
        result = R_DK2_ERR_ARG;
    }
    else if (INVALID_FREQ == afreq[tile_index])
    {
        result = R_DK2_STATUS_LOADED;
    }
    else if (0 == astart[tile_index])
    {
        result = R_DK2_STATUS_ACTIVATED;
    }
    else
    {
        result = R_DK2_STATUS_STARTED;
    }
    
    return result;
}
/**********************************************************************************************************************
End of function get_status
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: analyze_config
* Description  : This function stops DRP circuit loaded in DRP.
* Arguments    : pconf - Address of configuration data.
*                crc_check - Whether or not to check CRC.
*                pname - Address of name buffer.
*                presult - Result of analysis.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t analyze_config(
    const void *const pconfig, const bool crc_check, dk2_char_t *const pname, config_dat_info_t *const presult)
{
    int32_t result = R_DK2_SUCCESS;
    config_parser_info_t parser_info;
    int32_t index;

    if (NULL == presult)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    presult->p_array = NULL;
    presult->array_num = 0;
    presult->cid = 0;
    presult->minor_ver = 0;
    presult->tile_num = 0;
    presult->freq = 0;
    presult->del_zero = 0;
    presult->dfc = 0;
    presult->context = 0;
    presult->state = 0;

    if (NULL != pname)
    {
        for (index = 0; index < NAME_BUF_SIZE; index++)
        {
            pname[index] = 0;
        }
    }
    
    result = init_config_parser(pconfig, &parser_info);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    
    result = analyze_common_section(crc_check, &parser_info, presult);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    
    result = analyze_stp_section(pname, crc_check, &parser_info, presult);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    if ((NULL == presult->p_array) || (0 == presult->array_num) ||
        (0 == presult->tile_num) || (presult->tile_num > R_DK2_TILE_NUM) ||
        (presult->del_zero > 1) || (presult->dfc > 1) ||
        (presult->context > CONTEXT_MAX) || (presult->state > STATE_MAX))
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    goto func_end;
    
func_end:
    return result;
}
/**********************************************************************************************************************
End of function analyze_config
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: analyze_common_section
* Description  : Read common section from configuration data.
* Arguments    : crc_check - Whether or not to check CRC.
*                pinfo - Address of parser information.
*                presult - Result of analysis.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t analyze_common_section(
    const bool crc_check, config_parser_info_t *const pinfo, config_dat_info_t *const presult)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint16_t expected_major_ver = 2;
    uint8_t expected_fixed_num = 0;
    uint8_t expected_reloc_num = 0;
    uint8_t expected_stp_num = 1;
    uint32_t crc_calc = 0;
    uint32_t crc_dat;

    if ((NULL == pinfo) || (NULL == presult))
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    result = read_config_section(COMMON_SECTION_ID, pinfo);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    if (false != crc_check)
    {
        crc_calc = calc_crc(&p_config[pinfo->index - pinfo->section_index], pinfo->section_size - SECTION_CRC_LEN);
    }

    result = read_config_hword(&presult->minor_ver, NULL, pinfo); /* minor version */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(NULL, &expected_major_ver, pinfo); /* major version */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, BYTE_NUM_OF_WORD, pinfo); /* vendor ID */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, TARGET_DEVIVCE_LEN, pinfo); /* target device */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_fixed_num, pinfo); /* number of fixed data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_reloc_num, pinfo); /* number of relocatable data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(NULL, &expected_stp_num, pinfo); /* number of STP-type data num */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, COMMON_RESERVE_LEN, pinfo); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    result = read_config_word(&crc_dat, NULL, pinfo); /* CRC */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    if ((false != crc_check) && (crc_dat != crc_calc))
    {
        result = R_DK2_ERR_CRC;
        goto func_end;
    }

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function analyze_common_section
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: analyze_stp_section
* Description  : Read STP-type data section from configuration data.
* Arguments    : pname - Address of name buffer.
*                crc_check - Whether or not to check CRC.
*                pinfo - Address of parser information.
*                presult - Result of analysis.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t analyze_stp_section(dk2_char_t *const pname, const bool crc_check, config_parser_info_t *const pinfo,
    config_dat_info_t *const presult)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint32_t crc_calc = 0;
    uint32_t crc_dat;
    uint32_t padding;

    if ((NULL == pinfo) || (NULL == presult))
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    result = read_config_section(STP_SECTION_ID, pinfo);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    if (false != crc_check)
    {
        crc_calc = calc_crc(&p_config[pinfo->index - pinfo->section_index],
            (pinfo->section_size - SECTION_CRC_LEN) - SECTION_DII_LEN);
    }

    result = read_config_word(&presult->cid, NULL, pinfo); /* configuration ID */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_word(&presult->array_num, NULL, pinfo); /* number of data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&presult->tile_num, NULL, pinfo); /* number of tiles */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&presult->del_zero, NULL, pinfo); /* delete zero data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_byte(&presult->dfc, NULL, pinfo); /* dfc */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, 1, pinfo); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_word(&presult->freq, NULL, pinfo); /* frequency */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(&presult->context, NULL, pinfo); /* context */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_hword(&presult->state, NULL, pinfo); /* state */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_data(NULL, 4, pinfo); /* reserved */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    result = read_config_str(&pname[0], NULL, SECTION_NAME_LEN, pinfo);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    presult->p_array = (uint32_t *)&p_config[pinfo->index];

    result = read_config_data(NULL, presult->array_num * U64_DATA_SIZE, pinfo); /* data */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    padding = ((CONFIG_ALIGN - (pinfo->section_index & CONFIG_ALIGN_MASK)) - SECTION_CRC_LEN) & CONFIG_ALIGN_MASK;
    result = read_config_data(NULL, padding, pinfo); /* padding */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    result = read_config_word(&crc_dat, NULL, pinfo); /* CRC */
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }
    if ((false != crc_check) && (crc_dat != crc_calc))
    {
        result = R_DK2_ERR_CRC;
        goto func_end;
    }

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function analyze_stp_section
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: init_config_parser
* Description  : Initialize parser of configuration data.
* Arguments    : pconfig - Address of configuration data.
*                pinfo - Address of parser information.
* Return Value : None.
**********************************************************************************************************************/
static int32_t init_config_parser(const void *const pconfig, config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((NULL == pconfig) || (0 != ((uint32_t)pconfig & CONFIG_ALIGN_MASK)))
    {
        result = R_DK2_ERR_ARG;
        goto func_end;
    }

    pinfo->p_config = (uint8_t *)pconfig;
    pinfo->index = 0;
    pinfo->section_index = 0;
    pinfo->section_size = 0;

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function init_config_parser
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_section
* Description  : Read a section of configuration data.
* Arguments    : pid - string of section ID.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_section(const dk2_char_t *const pid, config_parser_info_t *const pinfo)
{
    int32_t result;
    const uint8_t *p_config;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    /* Skip inter-section gap */
    if (pinfo->section_index < pinfo->section_size)
    {
        pinfo->index += (pinfo->section_size - pinfo->section_index);
    }

    if (0 != (pinfo->index & CONFIG_ALIGN_MASK))
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    pinfo->section_size = *(const uint32_t *)&p_config[pinfo->index];

    pinfo->index += SECTION_SIZE_LEN;
    pinfo->section_index = SECTION_SIZE_LEN;

    /* read section id */
    result = read_config_str(NULL, pid, SECTION_ID_LEN, pinfo);
    if (R_DK2_SUCCESS != result)
    {
        goto func_end;
    }

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_section
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_word
* Description  : Read 32 bit data from configuration data.
* Arguments    : pword - Address of 32 bit data.
*                pexpeced - Address of expected data.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_word(uint32_t *const pword, const uint32_t *const pexpected,
    config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint32_t word;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((pinfo->section_index + sizeof(uint32_t)) > pinfo->section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    word = *(const uint32_t *)&p_config[pinfo->index];

    if (NULL != pword)
    {
        *pword = word;
    }

    if (NULL != pexpected)
    {
        if (word != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    pinfo->index += sizeof(uint32_t);
    pinfo->section_index += sizeof(uint32_t);

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_word
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_hword
* Description  : Read 16 bit data from configuration data.
* Arguments    : pword - Address of 16 bit data.
*                pexpeced - Address of expected data.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_hword(uint16_t *const phword, const uint16_t *const pexpected,
    config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint16_t hword;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((pinfo->section_index + sizeof(uint16_t)) > pinfo->section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    hword = *(const uint16_t *)&p_config[pinfo->index];

    if (NULL != phword)
    {
        *phword = hword;
    }

    if (NULL != pexpected)
    {
        if (hword != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    pinfo->index += sizeof(uint16_t);
    pinfo->section_index += sizeof(uint16_t);

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_hword
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_byte
* Description  : Read 8 bit data from configuration data.
* Arguments    : pword - Address of 8 bit data.
*                pexpeced - Address of expected data.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_byte(uint8_t *const pbyte, const uint8_t *const pexpected,
    config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint8_t byte;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((pinfo->section_index + sizeof(uint8_t)) > pinfo->section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    byte = p_config[pinfo->index];

    if (NULL != pbyte)
    {
        *pbyte = byte;
    }

    if (NULL != pexpected)
    {
        if (byte != (*pexpected))
        {
            result = R_DK2_ERR_FORMAT;
            goto func_end;
        }
    }

    pinfo->index += sizeof(uint8_t);
    pinfo->section_index += sizeof(uint8_t);

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_byte
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_str
* Description  : Read string data from configuration data.
* Arguments    : pstr - Address of string data.
*                pexpeced - Address of expected data.
*                size - Size of string data.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_str(dk2_char_t *const pstr, const dk2_char_t *const pexpeced, const uint32_t size,
    config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;
    uint32_t index;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((pinfo->section_index + size) > pinfo->section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    for (index = 0;index < size; index++)
    {
        /* if pstr is not NULL, copy to pstr. */
        if (NULL != pstr)
        {
            pstr[index] = p_config[pinfo->index + index];
        }

        /* if pexpeced is not NULL, compare with expected data. */
        if (NULL != pexpeced)
        {
            if (p_config[pinfo->index + index] != pexpeced[index])
            {
                result = R_DK2_ERR_FORMAT;
                goto func_end;
            }
        }
    }

    pinfo->index += size;
    pinfo->section_index += size;

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_str
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: read_config_data
* Description  : Get current address and skip "size" bytes.
* Arguments    : pdata - Address of configuration data pointer.
*                size - Size of skip.
*                pinfo - Address of parser information.
* Return Value : Error code.
**********************************************************************************************************************/
static int32_t read_config_data(const uint32_t **const pdata, const uint32_t size, config_parser_info_t *const pinfo)
{
    int32_t result = R_DK2_SUCCESS;
    const uint8_t *p_config;

    if (NULL == pinfo)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    p_config = pinfo->p_config;

    if (NULL == p_config)
    {
        result = R_DK2_ERR_INTERNAL;
        goto func_end;
    }

    if ((pinfo->section_index + size) > pinfo->section_size)
    {
        result = R_DK2_ERR_FORMAT;
        goto func_end;
    }

    if (NULL != pdata)
    {
        *pdata = (const uint32_t *)&p_config[pinfo->index];
    }
    pinfo->index += size;
    pinfo->section_index += size;

    goto func_end;

func_end:
    return result;
}
/**********************************************************************************************************************
End of function read_config_data
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: calc_crc
* Description  : Calculate CRC.
* Arguments    : buf - Data pointer.
*                len - Length of CRC data.
* Return Value : CRC value.
**********************************************************************************************************************/
static uint32_t calc_crc(const uint8_t *const buf, const uint32_t len)
{
    uint32_t result = CRC32_INITIAL_VALUE;
    uint32_t index;
    uint32_t bit;

    for (index = 0; index < len; index++)
    {
        result = result ^ buf[index];
        for (bit = 0; bit < CRC32_BIT_LOOP_NUM; bit++)
        {
            if (0 != (result & 1))
            {
                result >>= 1;
                result ^= CRC32_GEN_POLYNOMIAL;
            }
            else
            {
                result >>= 1;
            }
        }
    }
    return result ^ CRC32_XOR_VALUE;
}
/**********************************************************************************************************************
End of function calc_crc
**********************************************************************************************************************/

/**********************************************************************************************************************
* Function Name: dk2_nmlint_isr
* Description  : Interrupt function of DRP.
* Arguments    : int_sense - Sense of interrupt.
* Return Value : None.
**********************************************************************************************************************/
static void dk2_nmlint_isr(uint32_t int_sense)
{
    uint32_t int_status;
    uint8_t id;
    int32_t tile_index;
    uint8_t int_count;
    int32_t int_tile = R_DK2_TILE_NUM;

    int_status = R_DK2_CORE_GetInt();
    int_status = (int_status >> INT_STATUS_SHIFT) & INT_CH_MASK;

    while (int_status != 0)
    {
        /* Search ID and interrupt count of DRP circuit which cause interrupt. */
        id = 0;
        int_count = 0;
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if ((int_status & (uint32_t)(R_DK2_TILE_0 << tile_index)) != 0)
            {
                int_status &= ~((uint32_t)(R_DK2_TILE_0 << tile_index));
                id = astart[tile_index];
                int_count = acount[tile_index];
                break;
            }
            else
            {
                /* DO NOTHING */
            }
        }
        if ((id == 0) || (int_count == 0))
        {
            continue;
        }
        else
        {
            /* DO NOTHING */
        }

        int_tile = tile_index;  /* Save tile number where interrupt occurs. */
        int_count--;

        /* Update acount. */
        for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
        {
            if (astart[tile_index] == id)
            {
                acount[tile_index] = int_count;
            }
            else
            {
                /* DO NOTHING */
            }
        }

        /* Update astart and callback. */
        if (int_count == 0)
        {
            for (tile_index = 0; tile_index < R_DK2_TILE_NUM; tile_index++)
            {
                if (astart[tile_index] == id)
                {
                    astart[tile_index] = 0;
                }
                else
                {
                    /* DO NOTHING */
                }
            }
            if((int_tile < R_DK2_TILE_NUM) && (apint[int_tile] != NULL))
            {
                apint[int_tile](id);
            }
            else
            {
                /* DO NOTHING */
            }
        }
        else
        {
            /* DO NOTHING */
        }
    }
}
/**********************************************************************************************************************
End of function dk2_nmlint_isr
**********************************************************************************************************************/

/* End of File */
