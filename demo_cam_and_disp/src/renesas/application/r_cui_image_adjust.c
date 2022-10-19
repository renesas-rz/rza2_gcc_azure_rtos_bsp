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
/*******************************************************************************
* File Name   : r_cui_image_adjust.c
* Version     : 1.00 <- Optional as long as history is shown below
* Description : This module provides function of register value changing
                regarding image quality adjustment.
******************************************************************************/
/*****************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.27.2018 1.01 Improved CUI operation (operated with CR data)
*         : 09.14.2018 1.00 first version
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <iodefine.h>
#include "r_typedefs.h"
#include "r_cui_image_adjust.h"
#include "r_os_abstraction_api.h"
#include "tx_api.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#define SDK_PRV_MAX_PRESET_NUM        (5)
#define SDK_PRV_KEY_INPUT_BUFF_NUM    (12)

#define SDK_PRV_SHIFT_3BYTES          (24)
#define SDK_PRV_SHIFT_2BYTES          (16)
#define SDK_PRV_SHIFT_1BYTES          (8)
#define SDK_PRV_SHIFT_0BYTES          (0)

#define SDK_PRV_MAX_REG_BIT_VAL_DIGIT (5)

#define MILISEC_WAIT_1          (2000000)

static TX_THREAD p_graphics_thread;
static CHAR p_graphics_thread_stack[1024];

/******************************************************************************
Typedef definitions
******************************************************************************/
/* register(bit) specification */
typedef struct reg
{
    char_t *             reg_name;    /*  string of register-bit  */ 
    volatile uint32_t *  reg_addr;    /*  address of register     */ 
    int32_t              reg_min;     /*  Maximum value           */
    int32_t              reg_max;     /*  Minimum value           */
    int32_t              reg_default; /*  Default value           */
    uint32_t             bitshift;    /*  Position of bit         */
    uint32_t             bitmask;     /*  Mask value              */
    uint32_t             preset[SDK_PRV_MAX_PRESET_NUM]; /*  Preset value            */
}st_reg_t;

/* register(set) specification */
typedef struct all_reg{
    char_t *             reg_name;    /*  string of register-bit  */ 
    volatile uint32_t *  reg_addr;    /*  address of register     */ 
    int32_t              reg_default; /*  Default value           */
}st_all_reg_t;

/* menu specification */
typedef struct menu{
    uint32_t         stateid;     /*  menu ID                         */
    uint32_t         pattern;     /*  Kind of menu                    */
    uint32_t         selectnum;   /*  Number of list to move          */
    char_t const *   content;     /*  Title string                    */ 
    char_t const *   explanation; /*  Explanation string              */ 
    char_t const **  selectlist;  /*  Pointer to list string          */ 
    st_reg_t const * regstruct;   /*  Pointer to register struct      */ 
    uint32_t         regnum;      /*  Number of register struct list  */
}st_menu_t;

/* header information attribute specification */
typedef enum
{
    NOT_READ = 0,                                /* No need to read register */
    READ_REG                                     /* Need to read register    */
}e_header_attribute_t;

/* header information specification */
typedef struct header_info
{
    e_header_attribute_t    attribute;   /* Need to confirm register value or not     */
    st_reg_t const *        reg_struct;  /* Pointer to register struct                */  
    char_t *                string;      /* String to be output as header information */  
    char_t const **      string_sub;     /* Sub string for define macro name          */  
}st_header_info_t;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static uint32_t gs_cur_status; /* current menu */
static uint32_t gs_cur_index;  /* index of current_menu[] */

static char_t   gs_key_input_data[SDK_PRV_KEY_INPUT_BUFF_NUM];   /* input data */
static uint32_t gs_preset_value[CUI_MAX_MENUID];   /* current chosen setting */

/* Title of menu */
static char_t const gs_top_content[]                  = "Display Setting Top Menu";
static char_t const gs_bright_content[]               = "Brightness Setting";
static char_t const gs_bright_in_content[]            = "Brightness Setting(Input controller)";
static char_t const gs_bright_out_content[]           = "Brightness Setting(Output controller)";
static char_t const gs_contra_content[]               = "Contrast Setting";
static char_t const gs_contra_in_content[]            = "Contrast Setting(Input controller)";
static char_t const gs_contra_out_content[]           = "Contrast Setting(Output controller)";
static char_t const gs_sharp_content[]                = "Sharpness Setting";
static char_t const gs_gamma_content[]                = "Gamma correction";
static char_t const gs_dither_content[]               = "Dither Process";
static char_t const gs_rotation_content[]             = "Horizontal Mirroring and Rotation";
static char_t const gs_all_custom[]                   = "Custom Register Settings";
static char_t const gs_all_custom_reg[]               = "Register Setting of the  ";

/* Text string to show what user operate */
static char_t const gs_explanation_top_br_co[]        = "Please select the item you want to set.";
static char_t const gs_explanation_preset[]           = "Please select preset.";
static char_t const gs_explanation_customreg[]        = "Please select the register you want to set.";
static char_t const gs_explanation_custom[]           
    = "Please Input Value or Select Item\r\nConfirm by pressing the Enter key.";

/* list of preset */
static char_t const * gsp_top_select_list[]           
    = { "Brightness","Contrast","Sharpness","Gamma Correction","Dither process","Horizontal Mirroring and Rotation"};
static char_t const * gsp_controller_select_list[]    
    = { "Input controller","Output controller"};
static char_t const * gsp_hml_select_list[]           
    = { "High","Middle","Low","Default(Hardware Initial Value Refer to the Hardware Manual)","Custom"};
static char_t const * gsp_sharp_select_list[]         
    = { "Strong Sharpness","Semi-Strong Sharpness","Weak Sharpness","Default(Sharpness  OFF)","Custom"};
static char_t const * gsp_gamma_select_list[]         
    = { "Pattern0","Pattern1","Pattern2","Pattern3","Default(Gamma correction Off)","Custom"};
static char_t const * gsp_dither_select_list[]        
    = { "Cutoff","Round off","2x2 Pattern Dither","Random Pattern Dither","Default(Cutoff)"};
static char_t const * gsp_rotation_select_list[]      
    = { "Horizontal Mirroring  ON", "Rotation(180 Degrees)", "Default(Hardware Initial Value Refer to the Hardware Manual)","Custom"}; 

/* list of param output sub table */
static char_t const * gsp_bright_input_mode_list[]    
    = {"NOT SUPPORT","NOT SUPPORT","VDC_COLORMTX_YCBCR_GBR","VDC_COLORMTX_YCBCR_YCBCR"};
static char_t const * gsp_function_on_off_list[]      
    = {"VDC_OFF","VDC_ON"};
static char_t const * gsp_median_tap_sel_list []     
    = {"VDC_LTI_MDFIL_SEL_ADJ2","VDC_LTI_MDFIL_SEL_ADJ1"};
static char_t const * gsp_dither_mode_list []        
    = {"VDC_PDTH_MD_TRU","VDC_PDTH_MD_RDOF","VDC_PDTH_MD_2X2","VDC_PDTH_MD_RAND"};

/* registers */

/*  Brightness Setting(Input controller) */
static st_reg_t const gs_reg_bright_in[CUI_REGNUM_BRIGHT_IN] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "IMGCNT_MTX_YG_ADJ0->IMGCNT_MTX_YG[7:0]",   &VDC6.IMGCNT_MTX_YG_ADJ0.LONG,   0,      255,    128,    16, 0x000000FF, {   200,    150,    50,     128,    0    }   }/* Cast to an appropriate type */ 
};

/*  Brightness Setting(Output controller)   */
static st_reg_t const gs_reg_bright_out[CUI_REGNUM_BRIGHT_OUT] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "OUT_BRIGHT1->PBRT_G[9:0]",                 &VDC6.OUT_BRIGHT1.LONG,          0,      1023,   512,    0,  0x000003FF, {   756,    550,    256,    512,    0     }   }/* Cast to an appropriate type */ 
,   {   "OUT_BRIGHT2->PBRT_B[9:0]",                 &VDC6.OUT_BRIGHT2.LONG,          0,      1023,   512,    16, 0x000003FF, {   756,    550,    256,    512,    0     }   }/* Cast to an appropriate type */ 
,   {   "OUT_BRIGHT2->PBRT_R[9:0]",                 &VDC6.OUT_BRIGHT2.LONG,          0,      1023,   512,    0,  0x000003FF, {   756,    550,    256,    512,    0     }   }/* Cast to an appropriate type */ 
};

/*  Contrast Setting(Input controller)  */
static st_reg_t const gs_reg_cont_in[CUI_REGNUM_CONTRAST_IN] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "IMGCNT_MTX_YG_ADJ0->IMGCNT_MTX_GG[10:0]",  &VDC6.IMGCNT_MTX_YG_ADJ0.LONG,   -1024,  1023,   256,    0,  0x000007FF, {   512,    300,    150,    256,    0    }   }/* Cast to an appropriate type */ 
};

/*  Contrast Setting(Output controller) */
static st_reg_t const gs_reg_cont_out[CUI_REGNUM_CONTRAST_OUT] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "OUT_CONTRAST->CONT_G[7:0]",                &VDC6.OUT_CONTRAST.LONG,         0,      255,    128,    16, 0x000000FF, {   255,    160,    64,     128,    0     }   }/* Cast to an appropriate type */ 
,   {   "OUT_CONTRAST->CONT_B[7:0]",                &VDC6.OUT_CONTRAST.LONG,         0,      255,    128,    8,  0x000000FF, {   255,    160,    64,     128,    0     }   }/* Cast to an appropriate type */ 
,   {   "OUT_CONTRAST->CONT_R[7:0]",                &VDC6.OUT_CONTRAST.LONG,         0,      255,    128,    0,  0x000000FF, {   255,    160,    64,     128,    0     }   }/* Cast to an appropriate type */ 
};

/*  Sharpness Setting   */
static st_reg_t const gs_reg_sharpness[CUI_REGNUM_SHARP] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "ADJ0_ENH_SHP1->SHP_H_ON",                  &VDC6.ADJ0_ENH_SHP1.LONG,        0,      1,      0,      16, 0x00000001, {   1,      1,      1,       0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP1->SHP_H1_CORE[6:0]",          &VDC6.ADJ0_ENH_SHP1.LONG,        0,      127,    0,      0,  0x0000007F, {   10,     10,     10,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP2->SHP_H1_CLIP_O[7:0]",        &VDC6.ADJ0_ENH_SHP2.LONG,        0,      255,    0,      24, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP2->SHP_H1_CLIP_U[7:0]",        &VDC6.ADJ0_ENH_SHP2.LONG,        0,      255,    0,      16, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP2->SHP_H1_GAIN_O[7:0]",        &VDC6.ADJ0_ENH_SHP2.LONG,        0,      255,    0,      8,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP2->SHP_H1_GAIN_U[7:0]",        &VDC6.ADJ0_ENH_SHP2.LONG,        0,      255,    0,      0,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP3->SHP_H2_LPF_SEL",            &VDC6.ADJ0_ENH_SHP3.LONG,        0,      1,      0,      16, 0x00000001, {   0,      0,      0,       0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP3->SHP_H2_CORE[6:0]",          &VDC6.ADJ0_ENH_SHP3.LONG,        0,      127,    0,      0,  0x0000007F, {   10,     10,     10,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP4->SHP_H2_CLIP_O[7:0]",        &VDC6.ADJ0_ENH_SHP4.LONG,        0,      255,    0,      24, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP4->SHP_H2_CLIP_U[7:0]",        &VDC6.ADJ0_ENH_SHP4.LONG,        0,      255,    0,      16, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP4->SHP_H2_GAIN_O[7:0]",        &VDC6.ADJ0_ENH_SHP4.LONG,        0,      255,    0,      8,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP4->SHP_H2_GAIN_U[7:0]",        &VDC6.ADJ0_ENH_SHP4.LONG,        0,      255,    0,      0,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP5->SHP_H3_CORE[6:0]",          &VDC6.ADJ0_ENH_SHP5.LONG,        0,      127,    0,      0,  0x0000007F, {   10,     10,     10,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP6->SHP_H3_CLIP_O[7:0]",        &VDC6.ADJ0_ENH_SHP6.LONG,        0,      255,    0,      24, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP6->SHP_H3_CLIP_U[7:0]",        &VDC6.ADJ0_ENH_SHP6.LONG,        0,      255,    0,      16, 0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP6->SHP_H3_GAIN_O[7:0]",        &VDC6.ADJ0_ENH_SHP6.LONG,        0,      255,    0,      8,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_SHP6->SHP_H3_GAIN_U[7:0]",        &VDC6.ADJ0_ENH_SHP6.LONG,        0,      255,    0,      0,  0x000000FF, {   191,    100,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI1->LTI_H_ON",                  &VDC6.ADJ0_ENH_LTI1.LONG,        0,      1,      0,      31, 0x00000001, {   1,      1,      1,       0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI1->LTI_H2_LPF_SEL",            &VDC6.ADJ0_ENH_LTI1.LONG,        0,      1,      0,      24, 0x00000001, {   0,      0,      0,       0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI1->LTI_H2_INC_ZERO[7:0]",      &VDC6.ADJ0_ENH_LTI1.LONG,        0,      255,    10,     16, 0x000000FF, {   10,     10,     10,      10,     0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI1->LTI_H2_GAIN[7:0]",          &VDC6.ADJ0_ENH_LTI1.LONG,        0,      255,    0,      8,  0x000000FF, {   255,    128,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI1->LTI_H2_CORE[7:0]",          &VDC6.ADJ0_ENH_LTI1.LONG,        0,      255,    0,      0,  0x000000FF, {   20,     20,     20,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI2->LTI_H4_MEDIAN_TAP_SEL",     &VDC6.ADJ0_ENH_LTI2.LONG,        0,      1,      0,      24, 0x00000001, {   0,      0,      0,       0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI2->LTI_H4_INC_ZERO[7:0]",      &VDC6.ADJ0_ENH_LTI2.LONG,        0,      255,    10,     16, 0x000000FF, {   10,     10,     10,      10,     0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI2->LTI_H4_GAIN[7:0]",          &VDC6.ADJ0_ENH_LTI2.LONG,        0,      255,    0,      8,  0x000000FF, {   255,    128,    64,      0,      0       }   }/* Cast to an appropriate type */ 
,   {   "ADJ0_ENH_LTI2->LTI_H4_CORE[7:0]",          &VDC6.ADJ0_ENH_LTI2.LONG,        0,      255,    0,      0,  0x000000FF, {   20,     20,     20,      0,      0       }   }/* Cast to an appropriate type */ 
};

/*  Gamma correction    */
static st_reg_t const gs_reg_gamma[CUI_REGNUM_GAMMA] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3      pr4     pr5     */ 
    {   "GAM_SW->AM_ON",                            &VDC6.GAM_SW.LONG,               0,      1,      0,      0,  0x00000001, {   1,      1,      1,       1,      0       }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA1->GAM_G_TH_01[7:0]",            &VDC6.GAM_G_AREA1.LONG,          0,      255,    8,     16,  0x000000FF, {   8,      8,      8,       8,      8,      }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA1->GAM_G_TH_02[7:0]",            &VDC6.GAM_G_AREA1.LONG,          0,      255,    16,     8,  0x000000FF, {   16,     16,     16,      16,     16,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA1->GAM_G_TH_03[7:0]",            &VDC6.GAM_G_AREA1.LONG,          0,      255,    24,     0,  0x000000FF, {   24,     24,     24,      24,     24,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA2->GAM_G_TH_04[7:0]",            &VDC6.GAM_G_AREA2.LONG,          0,      255,    32,    24,  0x000000FF, {   32,     32,     32,      32,     32,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA2->GAM_G_TH_05[7:0]",            &VDC6.GAM_G_AREA2.LONG,          0,      255,    40,    16,  0x000000FF, {   40,     40,     40,      40,     40,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA2->GAM_G_TH_06[7:0]",            &VDC6.GAM_G_AREA2.LONG,          0,      255,    48,     8,  0x000000FF, {   48,     48,     48,      48,     48,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA2->GAM_G_TH_07[7:0]",            &VDC6.GAM_G_AREA2.LONG,          0,      255,    56,     0,  0x000000FF, {   56,     56,     56,      56,     56,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA3->GAM_G_TH_08[7:0]",            &VDC6.GAM_G_AREA3.LONG,          0,      255,    64,    24,  0x000000FF, {   64,     64,     64,      64,     64,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA3->GAM_G_TH_09[7:0]",            &VDC6.GAM_G_AREA3.LONG,          0,      255,    72,    16,  0x000000FF, {   72,     72,     72,      72,     72,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA3->GAM_G_TH_10[7:0]",            &VDC6.GAM_G_AREA3.LONG,          0,      255,    80,     8,  0x000000FF, {   80,     80,     80,      80,     80,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA3->GAM_G_TH_11[7:0]",            &VDC6.GAM_G_AREA3.LONG,          0,      255,    88,     0,  0x000000FF, {   88,     88,     88,      88,     88,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA4->GAM_G_TH_12[7:0]",            &VDC6.GAM_G_AREA4.LONG,          0,      255,    96,    24,  0x000000FF, {   96,     96,     96,      96,     96,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA4->GAM_G_TH_13[7:0]",            &VDC6.GAM_G_AREA4.LONG,          0,      255,    104,   16,  0x000000FF, {   104,    104,    104,     104,    104,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA4->GAM_G_TH_14[7:0]",            &VDC6.GAM_G_AREA4.LONG,          0,      255,    112,    8,  0x000000FF, {   112,    112,    112,     112,    112,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA4->GAM_G_TH_15[7:0]",            &VDC6.GAM_G_AREA4.LONG,          0,      255,    120,    0,  0x000000FF, {   120,    120,    120,     120,    120,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA5->GAM_G_TH_16[7:0]",            &VDC6.GAM_G_AREA5.LONG,          0,      255,    128,   24,  0x000000FF, {   128,    128,    128,     128,    128,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA5->GAM_G_TH_17[7:0]",            &VDC6.GAM_G_AREA5.LONG,          0,      255,    136,   16,  0x000000FF, {   136,    136,    136,     136,    136,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA5->GAM_G_TH_18[7:0]",            &VDC6.GAM_G_AREA5.LONG,          0,      255,    144,    8,  0x000000FF, {   144,    144,    144,     144,    144,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA5->GAM_G_TH_19[7:0]",            &VDC6.GAM_G_AREA5.LONG,          0,      255,    152,    0,  0x000000FF, {   152,    152,    152,     152,    152,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA6->GAM_G_TH_20[7:0]",            &VDC6.GAM_G_AREA6.LONG,          0,      255,    160,   24,  0x000000FF, {   160,    160,    160,     160,    160,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA6->GAM_G_TH_21[7:0]",            &VDC6.GAM_G_AREA6.LONG,          0,      255,    168,   16,  0x000000FF, {   168,    168,    168,     168,    168,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA6->GAM_G_TH_22[7:0]",            &VDC6.GAM_G_AREA6.LONG,          0,      255,    176,    8,  0x000000FF, {   176,    176,    176,     176,    176,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA6->GAM_G_TH_23[7:0]",            &VDC6.GAM_G_AREA6.LONG,          0,      255,    184,    0,  0x000000FF, {   184,    184,    184,     184,    184,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA7->GAM_G_TH_24[7:0]",            &VDC6.GAM_G_AREA7.LONG,          0,      255,    192,   24,  0x000000FF, {   192,    192,    192,     192,    192,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA7->GAM_G_TH_25[7:0]",            &VDC6.GAM_G_AREA7.LONG,          0,      255,    200,   16,  0x000000FF, {   200,    200,    200,     200,    200,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA7->GAM_G_TH_26[7:0]",            &VDC6.GAM_G_AREA7.LONG,          0,      255,    208,    8,  0x000000FF, {   208,    208,    208,     208,    208,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA7->GAM_G_TH_27[7:0]",            &VDC6.GAM_G_AREA7.LONG,          0,      255,    216,    0,  0x000000FF, {   216,    216,    216,     216,    216,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA8->GAM_G_TH_28[7:0]",            &VDC6.GAM_G_AREA8.LONG,          0,      255,    224,   24,  0x000000FF, {   224,    224,    224,     224,    224,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA8->GAM_G_TH_29[7:0]",            &VDC6.GAM_G_AREA8.LONG,          0,      255,    232,   16,  0x000000FF, {   232,    232,    232,     232,    232,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA8->GAM_G_TH_30[7:0]",            &VDC6.GAM_G_AREA8.LONG,          0,      255,    240,    8,  0x000000FF, {   240,    240,    240,     240,    240,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_AREA8->GAM_G_TH_31[7:0]",            &VDC6.GAM_G_AREA8.LONG,          0,      255,    248,    0,  0x000000FF, {   248,    248,    248,     248,    248,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT1->GAM_G_GAIN_00[10:0]",          &VDC6.GAM_G_LUT1.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1952,   32,     864,     1184,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT1->GAM_G_GAIN_01[10:0]",          &VDC6.GAM_G_LUT1.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1504,   96,     224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT2->GAM_G_GAIN_02[10:0]",          &VDC6.GAM_G_LUT2.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1344,   160,    288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT2->GAM_G_GAIN_03[10:0]",          &VDC6.GAM_G_LUT2.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1280,   224,    352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT3->GAM_G_GAIN_04[10:0]",          &VDC6.GAM_G_LUT3.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1216,   320,    448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT3->GAM_G_GAIN_05[10:0]",          &VDC6.GAM_G_LUT3.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1152,   352,    544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT4->GAM_G_GAIN_06[10:0]",          &VDC6.GAM_G_LUT4.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1120,   416,    640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT4->GAM_G_GAIN_07[10:0]",          &VDC6.GAM_G_LUT4.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1088,   512,    800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT5->GAM_G_GAIN_08[10:0]",          &VDC6.GAM_G_LUT5.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1088,   544,    960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT5->GAM_G_GAIN_09[10:0]",          &VDC6.GAM_G_LUT5.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1056,   640,    1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT6->GAM_G_GAIN_10[10:0]",          &VDC6.GAM_G_LUT6.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1024,   672,    1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT6->GAM_G_GAIN_11[10:0]",          &VDC6.GAM_G_LUT6.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    736,    1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT7->GAM_G_GAIN_12[10:0]",          &VDC6.GAM_G_LUT7.LONG,           0,      2047,   1024,  16,  0x000007FF, {   992,    832,    1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT7->GAM_G_GAIN_13[10:0]",          &VDC6.GAM_G_LUT7.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    864,    1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT8->GAM_G_GAIN_14[10:0]",          &VDC6.GAM_G_LUT8.LONG,           0,      2047,   1024,  16,  0x000007FF, {   960,    928,    1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT8->GAM_G_GAIN_15[10:0]",          &VDC6.GAM_G_LUT8.LONG,           0,      2047,   1024,   0,  0x000007FF, {   960,    1024,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT9->GAM_G_GAIN_16[10:0]",          &VDC6.GAM_G_LUT9.LONG,           0,      2047,   1024,  16,  0x000007FF, {   928,    1056,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT9->GAM_G_GAIN_17[10:0]",          &VDC6.GAM_G_LUT9.LONG,           0,      2047,   1024,   0,  0x000007FF, {   928,    1120,   1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT10->GAM_G_GAIN_18[10:0]",         &VDC6.GAM_G_LUT10.LONG,          0,      2047,   1024,  16,  0x000007FF, {   928,    1216,   1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT10->GAM_G_GAIN_19[10:0]",         &VDC6.GAM_G_LUT10.LONG,          0,      2047,   1024,   0,  0x000007FF, {   928,    1248,   1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT11->GAM_G_GAIN_20[10:0]",         &VDC6.GAM_G_LUT11.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1312,   1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT11->GAM_G_GAIN_21[10:0]",         &VDC6.GAM_G_LUT11.LONG,          0,      2047,   1024,   0,  0x000007FF, {   896,    1376,   1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT12->GAM_G_GAIN_22[10:0]",         &VDC6.GAM_G_LUT12.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1472,   1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT12->GAM_G_GAIN_23[10:0]",         &VDC6.GAM_G_LUT12.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1504,   960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT13->GAM_G_GAIN_24[10:0]",         &VDC6.GAM_G_LUT13.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1568,   800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT13->GAM_G_GAIN_25[10:0]",         &VDC6.GAM_G_LUT13.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1632,   640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT14->GAM_G_GAIN_26[10:0]",         &VDC6.GAM_G_LUT14.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1696,   544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT14->GAM_G_GAIN_27[10:0]",         &VDC6.GAM_G_LUT14.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1760,   448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT15->GAM_G_GAIN_28[10:0]",         &VDC6.GAM_G_LUT15.LONG,          0,      2047,   1024,  16,  0x000007FF, {   832,    1824,   352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT15->GAM_G_GAIN_29[10:0]",         &VDC6.GAM_G_LUT15.LONG,          0,      2047,   1024,   0,  0x000007FF, {   832,    1888,   288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT16->GAM_G_GAIN_30[10:0]",         &VDC6.GAM_G_LUT16.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1952,   224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_G_LUT16->GAM_G_GAIN_31[10:0]",         &VDC6.GAM_G_LUT16.LONG,          0,      2047,   1024,   0,  0x000007FF, {   841,    2011,   1024,    1097,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA1->GAM_B_TH_01[7:0]",            &VDC6.GAM_B_AREA1.LONG,          0,      255,    8,     16,  0x000000FF, {   8,      8,      8,       8,      8,      }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA1->GAM_B_TH_02[7:0]",            &VDC6.GAM_B_AREA1.LONG,          0,      255,    16,     8,  0x000000FF, {   16,     16,     16,      16,     16,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA1->GAM_B_TH_03[7:0]",            &VDC6.GAM_B_AREA1.LONG,          0,      255,    24,     0,  0x000000FF, {   24,     24,     24,      24,     24,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA2->GAM_B_TH_04[7:0]",            &VDC6.GAM_B_AREA2.LONG,          0,      255,    32,    24,  0x000000FF, {   32,     32,     32,      32,     32,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA2->GAM_B_TH_05[7:0]",            &VDC6.GAM_B_AREA2.LONG,          0,      255,    40,    16,  0x000000FF, {   40,     40,     40,      40,     40,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA2->GAM_B_TH_06[7:0]",            &VDC6.GAM_B_AREA2.LONG,          0,      255,    48,     8,  0x000000FF, {   48,     48,     48,      48,     48,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA2->GAM_B_TH_07[7:0]",            &VDC6.GAM_B_AREA2.LONG,          0,      255,    56,     0,  0x000000FF, {   56,     56,     56,      56,     56,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA3->GAM_B_TH_08[7:0]",            &VDC6.GAM_B_AREA3.LONG,          0,      255,    64,    24,  0x000000FF, {   64,     64,     64,      64,     64,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA3->GAM_B_TH_09[7:0]",            &VDC6.GAM_B_AREA3.LONG,          0,      255,    72,    16,  0x000000FF, {   72,     72,     72,      72,     72,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA3->GAM_B_TH_10[7:0]",            &VDC6.GAM_B_AREA3.LONG,          0,      255,    80,     8,  0x000000FF, {   80,     80,     80,      80,     80,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA3->GAM_B_TH_11[7:0]",            &VDC6.GAM_B_AREA3.LONG,          0,      255,    88,     0,  0x000000FF, {   88,     88,     88,      88,     88,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA4->GAM_B_TH_12[7:0]",            &VDC6.GAM_B_AREA4.LONG,          0,      255,    96,    24,  0x000000FF, {   96,     96,     96,      96,     96,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA4->GAM_B_TH_13[7:0]",            &VDC6.GAM_B_AREA4.LONG,          0,      255,    104,   16,  0x000000FF, {   104,    104,    104,     104,    104,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA4->GAM_B_TH_14[7:0]",            &VDC6.GAM_B_AREA4.LONG,          0,      255,    112,    8,  0x000000FF, {   112,    112,    112,     112,    112,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA4->GAM_B_TH_15[7:0]",            &VDC6.GAM_B_AREA4.LONG,          0,      255,    120,    0,  0x000000FF, {   120,    120,    120,     120,    120,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA5->GAM_B_TH_16[7:0]",            &VDC6.GAM_B_AREA5.LONG,          0,      255,    128,   24,  0x000000FF, {   128,    128,    128,     128,    128,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA5->GAM_B_TH_17[7:0]",            &VDC6.GAM_B_AREA5.LONG,          0,      255,    136,   16,  0x000000FF, {   136,    136,    136,     136,    136,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA5->GAM_B_TH_18[7:0]",            &VDC6.GAM_B_AREA5.LONG,          0,      255,    144,    8,  0x000000FF, {   144,    144,    144,     144,    144,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA5->GAM_B_TH_19[7:0]",            &VDC6.GAM_B_AREA5.LONG,          0,      255,    152,    0,  0x000000FF, {   152,    152,    152,     152,    152,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA6->GAM_B_TH_20[7:0]",            &VDC6.GAM_B_AREA6.LONG,          0,      255,    160,   24,  0x000000FF, {   160,    160,    160,     160,    160,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA6->GAM_B_TH_21[7:0]",            &VDC6.GAM_B_AREA6.LONG,          0,      255,    168,   16,  0x000000FF, {   168,    168,    168,     168,    168,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA6->GAM_B_TH_22[7:0]",            &VDC6.GAM_B_AREA6.LONG,          0,      255,    176,    8,  0x000000FF, {   176,    176,    176,     176,    176,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA6->GAM_B_TH_23[7:0]",            &VDC6.GAM_B_AREA6.LONG,          0,      255,    184,    0,  0x000000FF, {   184,    184,    184,     184,    184,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA7->GAM_B_TH_24[7:0]",            &VDC6.GAM_B_AREA7.LONG,          0,      255,    192,   24,  0x000000FF, {   192,    192,    192,     192,    192,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA7->GAM_B_TH_25[7:0]",            &VDC6.GAM_B_AREA7.LONG,          0,      255,    200,   16,  0x000000FF, {   200,    200,    200,     200,    200,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA7->GAM_B_TH_26[7:0]",            &VDC6.GAM_B_AREA7.LONG,          0,      255,    208,    8,  0x000000FF, {   208,    208,    208,     208,    208,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA7->GAM_B_TH_27[7:0]",            &VDC6.GAM_B_AREA7.LONG,          0,      255,    216,    0,  0x000000FF, {   216,    216,    216,     216,    216,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA8->GAM_B_TH_28[7:0]",            &VDC6.GAM_B_AREA8.LONG,          0,      255,    224,   24,  0x000000FF, {   224,    224,    224,     224,    224,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA8->GAM_B_TH_29[7:0]",            &VDC6.GAM_B_AREA8.LONG,          0,      255,    232,   16,  0x000000FF, {   232,    232,    232,     232,    232,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA8->GAM_B_TH_30[7:0]",            &VDC6.GAM_B_AREA8.LONG,          0,      255,    240,    8,  0x000000FF, {   240,    240,    240,     240,    240,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_AREA8->GAM_B_TH_31[7:0]",            &VDC6.GAM_B_AREA8.LONG,          0,      255,    248,    0,  0x000000FF, {   248,    248,    248,     248,    248,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT1->GAM_B_GAIN_00[10:0]",          &VDC6.GAM_B_LUT1.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1952,   32,     864,     1184,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT1->GAM_B_GAIN_01[10:0]",          &VDC6.GAM_B_LUT1.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1504,   96,     224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT2->GAM_B_GAIN_02[10:0]",          &VDC6.GAM_B_LUT2.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1344,   160,    288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT2->GAM_B_GAIN_03[10:0]",          &VDC6.GAM_B_LUT2.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1280,   224,    352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT3->GAM_B_GAIN_04[10:0]",          &VDC6.GAM_B_LUT3.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1216,   320,    448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT3->GAM_B_GAIN_05[10:0]",          &VDC6.GAM_B_LUT3.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1152,   352,    544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT4->GAM_B_GAIN_06[10:0]",          &VDC6.GAM_B_LUT4.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1120,   416,    640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT4->GAM_B_GAIN_07[10:0]",          &VDC6.GAM_B_LUT4.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1088,   512,    800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT5->GAM_B_GAIN_08[10:0]",          &VDC6.GAM_B_LUT5.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1088,   544,    960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT5->GAM_B_GAIN_09[10:0]",          &VDC6.GAM_B_LUT5.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1056,   640,    1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT6->GAM_B_GAIN_10[10:0]",          &VDC6.GAM_B_LUT6.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1024,   672,    1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT6->GAM_B_GAIN_11[10:0]",          &VDC6.GAM_B_LUT6.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    736,    1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT7->GAM_B_GAIN_12[10:0]",          &VDC6.GAM_B_LUT7.LONG,           0,      2047,   1024,  16,  0x000007FF, {   992,    832,    1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT7->GAM_B_GAIN_13[10:0]",          &VDC6.GAM_B_LUT7.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    864,    1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT8->GAM_B_GAIN_14[10:0]",          &VDC6.GAM_B_LUT8.LONG,           0,      2047,   1024,  16,  0x000007FF, {   960,    928,    1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT8->GAM_B_GAIN_15[10:0]",          &VDC6.GAM_B_LUT8.LONG,           0,      2047,   1024,   0,  0x000007FF, {   960,    1024,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT9->GAM_B_GAIN_16[10:0]",          &VDC6.GAM_B_LUT9.LONG,           0,      2047,   1024,  16,  0x000007FF, {   928,    1056,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT9->GAM_B_GAIN_17[10:0]",          &VDC6.GAM_B_LUT9.LONG,           0,      2047,   1024,   0,  0x000007FF, {   928,    1120,   1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT10->GAM_B_GAIN_18[10:0]",         &VDC6.GAM_B_LUT10.LONG,          0,      2047,   1024,  16,  0x000007FF, {   928,    1216,   1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT10->GAM_B_GAIN_19[10:0]",         &VDC6.GAM_B_LUT10.LONG,          0,      2047,   1024,   0,  0x000007FF, {   928,    1248,   1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT11->GAM_B_GAIN_20[10:0]",         &VDC6.GAM_B_LUT11.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1312,   1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT11->GAM_B_GAIN_21[10:0]",         &VDC6.GAM_B_LUT11.LONG,          0,      2047,   1024,   0,  0x000007FF, {   896,    1376,   1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT12->GAM_B_GAIN_22[10:0]",         &VDC6.GAM_B_LUT12.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1472,   1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT12->GAM_B_GAIN_23[10:0]",         &VDC6.GAM_B_LUT12.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1504,   960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT13->GAM_B_GAIN_24[10:0]",         &VDC6.GAM_B_LUT13.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1568,   800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT13->GAM_B_GAIN_25[10:0]",         &VDC6.GAM_B_LUT13.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1632,   640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT14->GAM_B_GAIN_26[10:0]",         &VDC6.GAM_B_LUT14.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1696,   544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT14->GAM_B_GAIN_27[10:0]",         &VDC6.GAM_B_LUT14.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1760,   448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT15->GAM_B_GAIN_28[10:0]",         &VDC6.GAM_B_LUT15.LONG,          0,      2047,   1024,  16,  0x000007FF, {   832,    1824,   352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT15->GAM_B_GAIN_29[10:0]",         &VDC6.GAM_B_LUT15.LONG,          0,      2047,   1024,   0,  0x000007FF, {   832,    1888,   288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT16->GAM_B_GAIN_30[10:0]",         &VDC6.GAM_B_LUT16.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1952,   224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_B_LUT16->GAM_B_GAIN_31[10:0]",         &VDC6.GAM_B_LUT16.LONG,          0,      2047,   1024,   0,  0x000007FF, {   841,    2011,   1024,    1097,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA1->GAM_R_TH_01[7:0]",            &VDC6.GAM_R_AREA1.LONG,          0,      255,    8,     16,  0x000000FF, {   8,      8,      8,       8,      8,      }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA1->GAM_R_TH_02[7:0]",            &VDC6.GAM_R_AREA1.LONG,          0,      255,    16,     8,  0x000000FF, {   16,     16,     16,      16,     16,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA1->GAM_R_TH_03[7:0]",            &VDC6.GAM_R_AREA1.LONG,          0,      255,    24,     0,  0x000000FF, {   24,     24,     24,      24,     24,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA2->GAM_R_TH_04[7:0]",            &VDC6.GAM_R_AREA2.LONG,          0,      255,    32,    24,  0x000000FF, {   32,     32,     32,      32,     32,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA2->GAM_R_TH_05[7:0]",            &VDC6.GAM_R_AREA2.LONG,          0,      255,    40,    16,  0x000000FF, {   40,     40,     40,      40,     40,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA2->GAM_R_TH_06[7:0]",            &VDC6.GAM_R_AREA2.LONG,          0,      255,    48,     8,  0x000000FF, {   48,     48,     48,      48,     48,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA2->GAM_R_TH_07[7:0]",            &VDC6.GAM_R_AREA2.LONG,          0,      255,    56,     0,  0x000000FF, {   56,     56,     56,      56,     56,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA3->GAM_R_TH_08[7:0]",            &VDC6.GAM_R_AREA3.LONG,          0,      255,    64,    24,  0x000000FF, {   64,     64,     64,      64,     64,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA3->GAM_R_TH_09[7:0]",            &VDC6.GAM_R_AREA3.LONG,          0,      255,    72,    16,  0x000000FF, {   72,     72,     72,      72,     72,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA3->GAM_R_TH_10[7:0]",            &VDC6.GAM_R_AREA3.LONG,          0,      255,    80,     8,  0x000000FF, {   80,     80,     80,      80,     80,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA3->GAM_R_TH_11[7:0]",            &VDC6.GAM_R_AREA3.LONG,          0,      255,    88,     0,  0x000000FF, {   88,     88,     88,      88,     88,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA4->GAM_R_TH_12[7:0]",            &VDC6.GAM_R_AREA4.LONG,          0,      255,    96,    24,  0x000000FF, {   96,     96,     96,      96,     96,     }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA4->GAM_R_TH_13[7:0]",            &VDC6.GAM_R_AREA4.LONG,          0,      255,    104,   16,  0x000000FF, {   104,    104,    104,     104,    104,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA4->GAM_R_TH_14[7:0]",            &VDC6.GAM_R_AREA4.LONG,          0,      255,    112,    8,  0x000000FF, {   112,    112,    112,     112,    112,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA4->GAM_R_TH_15[7:0]",            &VDC6.GAM_R_AREA4.LONG,          0,      255,    120,    0,  0x000000FF, {   120,    120,    120,     120,    120,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA5->GAM_R_TH_16[7:0]",            &VDC6.GAM_R_AREA5.LONG,          0,      255,    128,   24,  0x000000FF, {   128,    128,    128,     128,    128,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA5->GAM_R_TH_17[7:0]",            &VDC6.GAM_R_AREA5.LONG,          0,      255,    136,   16,  0x000000FF, {   136,    136,    136,     136,    136,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA5->GAM_R_TH_18[7:0]",            &VDC6.GAM_R_AREA5.LONG,          0,      255,    144,    8,  0x000000FF, {   144,    144,    144,     144,    144,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA5->GAM_R_TH_19[7:0]",            &VDC6.GAM_R_AREA5.LONG,          0,      255,    152,    0,  0x000000FF, {   152,    152,    152,     152,    152,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA6->GAM_R_TH_20[7:0]",            &VDC6.GAM_R_AREA6.LONG,          0,      255,    160,   24,  0x000000FF, {   160,    160,    160,     160,    160,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA6->GAM_R_TH_21[7:0]",            &VDC6.GAM_R_AREA6.LONG,          0,      255,    168,   16,  0x000000FF, {   168,    168,    168,     168,    168,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA6->GAM_R_TH_22[7:0]",            &VDC6.GAM_R_AREA6.LONG,          0,      255,    176,    8,  0x000000FF, {   176,    176,    176,     176,    176,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA6->GAM_R_TH_23[7:0]",            &VDC6.GAM_R_AREA6.LONG,          0,      255,    184,    0,  0x000000FF, {   184,    184,    184,     184,    184,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA7->GAM_R_TH_24[7:0]",            &VDC6.GAM_R_AREA7.LONG,          0,      255,    192,   24,  0x000000FF, {   192,    192,    192,     192,    192,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA7->GAM_R_TH_25[7:0]",            &VDC6.GAM_R_AREA7.LONG,          0,      255,    200,   16,  0x000000FF, {   200,    200,    200,     200,    200,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA7->GAM_R_TH_26[7:0]",            &VDC6.GAM_R_AREA7.LONG,          0,      255,    208,    8,  0x000000FF, {   208,    208,    208,     208,    208,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA7->GAM_R_TH_27[7:0]",            &VDC6.GAM_R_AREA7.LONG,          0,      255,    216,    0,  0x000000FF, {   216,    216,    216,     216,    216,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA8->GAM_R_TH_28[7:0]",            &VDC6.GAM_R_AREA8.LONG,          0,      255,    224,   24,  0x000000FF, {   224,    224,    224,     224,    224,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA8->GAM_R_TH_29[7:0]",            &VDC6.GAM_R_AREA8.LONG,          0,      255,    232,   16,  0x000000FF, {   232,    232,    232,     232,    232,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA8->GAM_R_TH_30[7:0]",            &VDC6.GAM_R_AREA8.LONG,          0,      255,    240,    8,  0x000000FF, {   240,    240,    240,     240,    240,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_AREA8->GAM_R_TH_31[7:0]",            &VDC6.GAM_R_AREA8.LONG,          0,      255,    248,    0,  0x000000FF, {   248,    248,    248,     248,    248,    }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT1->GAM_R_GAIN_00[10:0]",          &VDC6.GAM_R_LUT1.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1952,   32,     864,     1184,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT1->GAM_R_GAIN_01[10:0]",          &VDC6.GAM_R_LUT1.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1504,   96,     224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT2->GAM_R_GAIN_02[10:0]",          &VDC6.GAM_R_LUT2.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1344,   160,    288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT2->GAM_R_GAIN_03[10:0]",          &VDC6.GAM_R_LUT2.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1280,   224,    352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT3->GAM_R_GAIN_04[10:0]",          &VDC6.GAM_R_LUT3.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1216,   320,    448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT3->GAM_R_GAIN_05[10:0]",          &VDC6.GAM_R_LUT3.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1152,   352,    544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT4->GAM_R_GAIN_06[10:0]",          &VDC6.GAM_R_LUT4.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1120,   416,    640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT4->GAM_R_GAIN_07[10:0]",          &VDC6.GAM_R_LUT4.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1088,   512,    800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT5->GAM_R_GAIN_08[10:0]",          &VDC6.GAM_R_LUT5.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1088,   544,    960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT5->GAM_R_GAIN_09[10:0]",          &VDC6.GAM_R_LUT5.LONG,           0,      2047,   1024,   0,  0x000007FF, {   1056,   640,    1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT6->GAM_R_GAIN_10[10:0]",          &VDC6.GAM_R_LUT6.LONG,           0,      2047,   1024,  16,  0x000007FF, {   1024,   672,    1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT6->GAM_R_GAIN_11[10:0]",          &VDC6.GAM_R_LUT6.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    736,    1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT7->GAM_R_GAIN_12[10:0]",          &VDC6.GAM_R_LUT7.LONG,           0,      2047,   1024,  16,  0x000007FF, {   992,    832,    1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT7->GAM_R_GAIN_13[10:0]",          &VDC6.GAM_R_LUT7.LONG,           0,      2047,   1024,   0,  0x000007FF, {   992,    864,    1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT8->GAM_R_GAIN_14[10:0]",          &VDC6.GAM_R_LUT8.LONG,           0,      2047,   1024,  16,  0x000007FF, {   960,    928,    1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT8->GAM_R_GAIN_15[10:0]",          &VDC6.GAM_R_LUT8.LONG,           0,      2047,   1024,   0,  0x000007FF, {   960,    1024,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT9->GAM_R_GAIN_16[10:0]",          &VDC6.GAM_R_LUT9.LONG,           0,      2047,   1024,  16,  0x000007FF, {   928,    1056,   1952,    96,     1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT9->GAM_R_GAIN_17[10:0]",          &VDC6.GAM_R_LUT9.LONG,           0,      2047,   1024,   0,  0x000007FF, {   928,    1120,   1888,    160,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT10->GAM_R_GAIN_18[10:0]",         &VDC6.GAM_R_LUT10.LONG,          0,      2047,   1024,  16,  0x000007FF, {   928,    1216,   1824,    224,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT10->GAM_R_GAIN_19[10:0]",         &VDC6.GAM_R_LUT10.LONG,          0,      2047,   1024,   0,  0x000007FF, {   928,    1248,   1632,    416,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT11->GAM_R_GAIN_20[10:0]",         &VDC6.GAM_R_LUT11.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1312,   1504,    544,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT11->GAM_R_GAIN_21[10:0]",         &VDC6.GAM_R_LUT11.LONG,          0,      2047,   1024,   0,  0x000007FF, {   896,    1376,   1312,    800,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT12->GAM_R_GAIN_22[10:0]",         &VDC6.GAM_R_LUT12.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1472,   1120,    928,    1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT12->GAM_R_GAIN_23[10:0]",         &VDC6.GAM_R_LUT12.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1504,   960,     1088,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT13->GAM_R_GAIN_24[10:0]",         &VDC6.GAM_R_LUT13.LONG,          0,      2047,   1024,  16,  0x000007FF, {   896,    1568,   800,     1248,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT13->GAM_R_GAIN_25[10:0]",         &VDC6.GAM_R_LUT13.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1632,   640,     1408,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT14->GAM_R_GAIN_26[10:0]",         &VDC6.GAM_R_LUT14.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1696,   544,     1504,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT14->GAM_R_GAIN_27[10:0]",         &VDC6.GAM_R_LUT14.LONG,          0,      2047,   1024,   0,  0x000007FF, {   864,    1760,   448,     1600,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT15->GAM_R_GAIN_28[10:0]",         &VDC6.GAM_R_LUT15.LONG,          0,      2047,   1024,  16,  0x000007FF, {   832,    1824,   352,     1696,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT15->GAM_R_GAIN_29[10:0]",         &VDC6.GAM_R_LUT15.LONG,          0,      2047,   1024,   0,  0x000007FF, {   832,    1888,   288,     1760,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT16->GAM_R_GAIN_30[10:0]",         &VDC6.GAM_R_LUT16.LONG,          0,      2047,   1024,  16,  0x000007FF, {   864,    1952,   224,     1824,   1024,   }   }/* Cast to an appropriate type */ 
,   {   "GAM_R_LUT16->GAM_R_GAIN_31[10:0]",         &VDC6.GAM_R_LUT16.LONG,          0,      2047,   1024,   0,  0x000007FF, {   841,    2011,   1024,    1097,   1024,   }   }/* Cast to an appropriate type */ 
};

/*  Dither Process  */
static st_reg_t const gs_reg_dither[CUI_REGNUM_DITHER] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5                  */ 
    {   "OUT_PDTHA->PDTH_SEL[1:0]",                 &VDC6.OUT_PDTHA.LONG,            0,      3,      0,      20, 0x00000003, {   0,      1,      2,      3,      0           }   }/* Cast to an appropriate type */ 
};

/*  Horizontal Mirroring and Rotation   */
/*  Caution: These palameters are under consideration */
static st_reg_t const gs_reg_rotation[CUI_REGNUM_ROTATION] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "SC0_SCL1_WR1->SC0_RES_DS_WR_MD[2:0]",      &VDC6.SC0_SCL1_WR1.LONG,         0,      4,      0,      4,  0x00000007, {   1,      3,      0,      0,      0           }   }/* Cast to an appropriate type */ 
};

/*  INPUT Matrix Mode   */
static st_reg_t const gs_reg_input_mtx_mode[CUI_REGNUM_IN_MTX_MODE] =
{
        /* register string                          register addr                    min     max     default pos mask            pr1     pr2     pr3     pr4     pr5(not use)         */ 
    {   "IMGCNT_MTX_MODE->IMGCNT_MTX_MD[1:0]",      &VDC6.IMGCNT_MTX_MODE.LONG,      0,      3,      3,      0,  0x00000003, {   3,      3,      3,      3,      3           }   }/* Cast to an appropriate type */ 
};

/* whole adjustable registers */
static st_header_info_t const gs_allparam [] =
{
    /*  attribute    reg_struct               string                                                                                                                                    string_sub */ 
    {   NOT_READ,    NULL,                    "/*******************************************************************************",                                                               NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* DISCLAIMER",                                                                                                                               NULL  }    /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* This software is supplied by Renesas Electronics Corporation and is only",                                                                     NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* intended for use with Renesas products. No other uses are authorized. This",                                                                   NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* software is owned by Renesas Electronics Corporation and is protected under",                                                              NULL  }    /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* all applicable laws, including copyright laws.",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* THIS SOFTWARE IS PROVIDED \"AS IS\" AND RENESAS MAKES NO WARRANTIES REGARDING",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT",                                                                      NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE",                                                                   NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.",                                                                          NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS",                                                                       NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE",                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR",                                                                   NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE",                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.",                                                                                             NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* Renesas reserves the right, without notice, to make changes to this software",                                                                 NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* and to discontinue the availability of this software. By using this software,",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* you agree to the additional terms and conditions found by accessing the",                                                                      NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* following link:",                                                                                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* http://www.renesas.com/disclaimer",                                                                                                            NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "*",                                                                                                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* Copyright (C) 2018 Renesas Electronics Corporation. All rights reserved.",                                                                     NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "*******************************************************************************/",                                                               NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "/******************************************************************************",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* File Name    : r_image_config.h",                                                                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "* Description  : Defines for Image Correction",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "******************************************************************************/",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#ifndef IMG_CONFIG_H",                                                                                                                           NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMG_CONFIG_H",                                                                                                                           NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/******************************************************************************",                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "Includes   <System Includes> , \"Project Includes\"",                                                                                            NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "******************************************************************************/",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#include    \"r_vdc.h\"",                                                                                                                       NULL  } /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#include    \"lcd_panel.h\"",                                                                                                                    NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/******************************************************************************",                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "Macro definitions",                                                                                                                              NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "******************************************************************************/",                                                                NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "/* Brightness Setting */",                                                                                                                       NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "/* Contrast Setting */",                                                                                                                         NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_BRIGHT_INPUTCTL_MODULE          (VDC_COLORMTX_IMGCNT)      /* This value is fixed */",                                             NULL  } /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_input_mtx_mode[0],"#define IMGC_BRIGHT_INPUTCTL_MODE            ",                                                                           gsp_bright_input_mode_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_bright_in[0],    "#define IMGC_BRIGHT_INPUTCTL_OFFSET_YG       ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_BRIGHT_INPUTCTL_OFFSET_B        (128u)                      /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_BRIGHT_INPUTCTL_OFFSET_R        (128u)                      /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_cont_in[0],         "#define IMGC_CONTRAST_INPUTCTL_GAIN_GG       ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_GB       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_GR       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_BG       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_BB       (256u)                      /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_BR       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_RG       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_RB       (0u)                        /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_CONTRAST_INPUTCTL_GAIN_RR       (256u)                      /* This value is default. User can modify based on user settings. */",  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n#define IMGC_OUTCTL_CALIB_ROUTE              (VDC_CALIBR_ROUTE_BCG)     /* This value is default. User can modify based on user settings. */",NULL  } /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_bright_out[0],   "#define IMGC_BRIGHT_OUTCTL_OFFSET_G          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_bright_out[1],   "#define IMGC_BRIGHT_OUTCTL_OFFSET_B          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_bright_out[2],   "#define IMGC_BRIGHT_OUTCTL_OFFSET_R          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_cont_out[0],     "#define IMGC_CONTRAST_OUTCTL_GAIN_G          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_cont_out[1],     "#define IMGC_CONTRAST_OUTCTL_GAIN_B          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_cont_out[2],     "#define IMGC_CONTRAST_OUTCTL_GAIN_R          ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/* Sharpness Setting */",                                                                                                                      NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_IMG_IMPRV_ID                    (VDC_IMG_IMPRV_0)          /* This value is fixed */",                                             NULL  } /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[0],    "#define IMGC_SHARPNESS_ACTIVE                ",                                                                              gsp_function_on_off_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[1],    "#define IMGC_SHARPNESS_H1_CLIP_O             ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[2],       "#define IMGC_SHARPNESS_H1_CLIP_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[3],       "#define IMGC_SHARPNESS_H1_GAIN_O             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[4],       "#define IMGC_SHARPNESS_H1_GAIN_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[5],       "#define IMGC_SHARPNESS_H1_CORE               ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[6],       "#define IMGC_SHARPNESS_H2_LPF                ",                                                                           gsp_function_on_off_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[7],       "#define IMGC_SHARPNESS_H2_CLIP_O             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[8],       "#define IMGC_SHARPNESS_H2_CLIP_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[9],       "#define IMGC_SHARPNESS_H2_GAIN_O             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[10],      "#define IMGC_SHARPNESS_H2_GAIN_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[11],      "#define IMGC_SHARPNESS_H2_CORE               ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[12],      "#define IMGC_SHARPNESS_H3_CLIP_O             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[13],      "#define IMGC_SHARPNESS_H3_CLIP_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[14],      "#define IMGC_SHARPNESS_H3_GAIN_O             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[15],      "#define IMGC_SHARPNESS_H3_GAIN_U             ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[16],      "#define IMGC_SHARPNESS_H3_CORE               ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[17],      "#define IMGC_SHARPNESS_LTI_ACTIVE            ",                                                                           gsp_function_on_off_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[18],      "#define IMGC_SHARPNESS_LTI_H2_LPF            ",                                                                           gsp_function_on_off_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[19],      "#define IMGC_SHARPNESS_LTI_H2_ZERO           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[20],      "#define IMGC_SHARPNESS_LTI_H2_GAIN           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[21],      "#define IMGC_SHARPNESS_LTI_H2_CORE           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[22],      "#define IMGC_SHARPNESS_LTI_H4_MEDTAP_SEL     ",                                                                            gsp_median_tap_sel_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[23],      "#define IMGC_SHARPNESS_LTI_H4_ZERO           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[24],      "#define IMGC_SHARPNESS_LTI_H4_GAIN           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_sharpness[25],      "#define IMGC_SHARPNESS_LTI_H4_CORE           ",                                                                                               NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_SHARPNESS_VS                    (LCD_CH0_DISP_VS)           /* This value is fixed */",                                             NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_SHARPNESS_VW                    (LCD_CH0_DISP_VW)           /* This value is fixed */",                                             NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_SHARPNESS_HS                    (LCD_CH0_DISP_HS)           /* This value is fixed */",                                             NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_SHARPNESS_HW                    (LCD_CH0_DISP_HW)           /* This value is fixed */",                                             NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/* Gamma correction */",                                                                                                                       NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[0],        "#define IMGC_GAMMA_ACTIVE                    ",                                                                              gsp_function_on_off_list  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[32],       "#define IMGC_GAMMA_G_GAIN_00                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[33],       "#define IMGC_GAMMA_G_GAIN_01                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[34],       "#define IMGC_GAMMA_G_GAIN_02                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[35],       "#define IMGC_GAMMA_G_GAIN_03                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[36],       "#define IMGC_GAMMA_G_GAIN_04                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[37],       "#define IMGC_GAMMA_G_GAIN_05                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[38],       "#define IMGC_GAMMA_G_GAIN_06                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[39],       "#define IMGC_GAMMA_G_GAIN_07                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[40],       "#define IMGC_GAMMA_G_GAIN_08                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[41],       "#define IMGC_GAMMA_G_GAIN_09                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[42],       "#define IMGC_GAMMA_G_GAIN_10                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[43],       "#define IMGC_GAMMA_G_GAIN_11                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[44],       "#define IMGC_GAMMA_G_GAIN_12                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[45],       "#define IMGC_GAMMA_G_GAIN_13                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[46],       "#define IMGC_GAMMA_G_GAIN_14                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[47],       "#define IMGC_GAMMA_G_GAIN_15                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[48],       "#define IMGC_GAMMA_G_GAIN_16                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[49],       "#define IMGC_GAMMA_G_GAIN_17                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[50],       "#define IMGC_GAMMA_G_GAIN_18                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[51],       "#define IMGC_GAMMA_G_GAIN_19                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[52],       "#define IMGC_GAMMA_G_GAIN_20                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[53],       "#define IMGC_GAMMA_G_GAIN_21                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[54],       "#define IMGC_GAMMA_G_GAIN_22                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[55],       "#define IMGC_GAMMA_G_GAIN_23                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[56],       "#define IMGC_GAMMA_G_GAIN_24                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[57],       "#define IMGC_GAMMA_G_GAIN_25                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[58],       "#define IMGC_GAMMA_G_GAIN_26                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[59],       "#define IMGC_GAMMA_G_GAIN_27                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[60],       "#define IMGC_GAMMA_G_GAIN_28                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[61],       "#define IMGC_GAMMA_G_GAIN_29                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[62],       "#define IMGC_GAMMA_G_GAIN_30                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[63],       "#define IMGC_GAMMA_G_GAIN_31                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[1],        "#define IMGC_GAMMA_G_TH_01                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[2],        "#define IMGC_GAMMA_G_TH_02                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[3],        "#define IMGC_GAMMA_G_TH_03                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[4],        "#define IMGC_GAMMA_G_TH_04                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[5],        "#define IMGC_GAMMA_G_TH_05                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[6],        "#define IMGC_GAMMA_G_TH_06                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[7],        "#define IMGC_GAMMA_G_TH_07                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[8],        "#define IMGC_GAMMA_G_TH_08                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[9],        "#define IMGC_GAMMA_G_TH_09                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[10],       "#define IMGC_GAMMA_G_TH_10                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[11],       "#define IMGC_GAMMA_G_TH_11                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[12],       "#define IMGC_GAMMA_G_TH_12                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[13],       "#define IMGC_GAMMA_G_TH_13                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[14],       "#define IMGC_GAMMA_G_TH_14                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[15],       "#define IMGC_GAMMA_G_TH_15                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[16],       "#define IMGC_GAMMA_G_TH_16                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[17],       "#define IMGC_GAMMA_G_TH_17                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[18],       "#define IMGC_GAMMA_G_TH_18                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[19],       "#define IMGC_GAMMA_G_TH_19                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[20],       "#define IMGC_GAMMA_G_TH_20                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[21],       "#define IMGC_GAMMA_G_TH_21                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[22],       "#define IMGC_GAMMA_G_TH_22                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[23],       "#define IMGC_GAMMA_G_TH_23                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[24],       "#define IMGC_GAMMA_G_TH_24                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[25],       "#define IMGC_GAMMA_G_TH_25                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[26],       "#define IMGC_GAMMA_G_TH_26                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[27],       "#define IMGC_GAMMA_G_TH_27                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[28],       "#define IMGC_GAMMA_G_TH_28                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[29],       "#define IMGC_GAMMA_G_TH_29                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[30],       "#define IMGC_GAMMA_G_TH_30                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[31],       "#define IMGC_GAMMA_G_TH_31                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[95],       "#define IMGC_GAMMA_B_GAIN_00                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[96],       "#define IMGC_GAMMA_B_GAIN_01                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[97],       "#define IMGC_GAMMA_B_GAIN_02                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[98],       "#define IMGC_GAMMA_B_GAIN_03                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[99],       "#define IMGC_GAMMA_B_GAIN_04                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[100],      "#define IMGC_GAMMA_B_GAIN_05                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[101],      "#define IMGC_GAMMA_B_GAIN_06                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[102],      "#define IMGC_GAMMA_B_GAIN_07                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[103],      "#define IMGC_GAMMA_B_GAIN_08                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[104],      "#define IMGC_GAMMA_B_GAIN_09                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[105],      "#define IMGC_GAMMA_B_GAIN_10                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[106],      "#define IMGC_GAMMA_B_GAIN_11                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[107],      "#define IMGC_GAMMA_B_GAIN_12                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[108],      "#define IMGC_GAMMA_B_GAIN_13                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[109],      "#define IMGC_GAMMA_B_GAIN_14                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[110],      "#define IMGC_GAMMA_B_GAIN_15                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[111],      "#define IMGC_GAMMA_B_GAIN_16                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[112],      "#define IMGC_GAMMA_B_GAIN_17                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[113],      "#define IMGC_GAMMA_B_GAIN_18                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[114],      "#define IMGC_GAMMA_B_GAIN_19                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[115],      "#define IMGC_GAMMA_B_GAIN_20                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[116],      "#define IMGC_GAMMA_B_GAIN_21                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[117],      "#define IMGC_GAMMA_B_GAIN_22                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[118],      "#define IMGC_GAMMA_B_GAIN_23                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[119],      "#define IMGC_GAMMA_B_GAIN_24                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[120],      "#define IMGC_GAMMA_B_GAIN_25                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[121],      "#define IMGC_GAMMA_B_GAIN_26                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[122],      "#define IMGC_GAMMA_B_GAIN_27                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[123],      "#define IMGC_GAMMA_B_GAIN_28                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[124],      "#define IMGC_GAMMA_B_GAIN_29                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[125],      "#define IMGC_GAMMA_B_GAIN_30                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[126],      "#define IMGC_GAMMA_B_GAIN_31                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[64],       "#define IMGC_GAMMA_B_TH_01                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[65],       "#define IMGC_GAMMA_B_TH_02                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[66],       "#define IMGC_GAMMA_B_TH_03                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[67],       "#define IMGC_GAMMA_B_TH_04                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[68],       "#define IMGC_GAMMA_B_TH_05                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[69],       "#define IMGC_GAMMA_B_TH_06                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[70],       "#define IMGC_GAMMA_B_TH_07                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[71],       "#define IMGC_GAMMA_B_TH_08                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[72],       "#define IMGC_GAMMA_B_TH_09                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[73],       "#define IMGC_GAMMA_B_TH_10                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[74],       "#define IMGC_GAMMA_B_TH_11                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[75],       "#define IMGC_GAMMA_B_TH_12                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[76],       "#define IMGC_GAMMA_B_TH_13                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[77],       "#define IMGC_GAMMA_B_TH_14                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[78],       "#define IMGC_GAMMA_B_TH_15                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[79],       "#define IMGC_GAMMA_B_TH_16                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[80],       "#define IMGC_GAMMA_B_TH_17                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[81],       "#define IMGC_GAMMA_B_TH_18                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[82],       "#define IMGC_GAMMA_B_TH_19                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[83],       "#define IMGC_GAMMA_B_TH_20                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[84],       "#define IMGC_GAMMA_B_TH_21                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[85],       "#define IMGC_GAMMA_B_TH_22                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[86],       "#define IMGC_GAMMA_B_TH_23                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[87],       "#define IMGC_GAMMA_B_TH_24                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[88],       "#define IMGC_GAMMA_B_TH_25                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[89],       "#define IMGC_GAMMA_B_TH_26                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[90],       "#define IMGC_GAMMA_B_TH_27                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[91],       "#define IMGC_GAMMA_B_TH_28                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[92],       "#define IMGC_GAMMA_B_TH_29                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[93],       "#define IMGC_GAMMA_B_TH_30                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[94],       "#define IMGC_GAMMA_B_TH_31                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[158],      "#define IMGC_GAMMA_R_GAIN_00                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[159],      "#define IMGC_GAMMA_R_GAIN_01                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[160],      "#define IMGC_GAMMA_R_GAIN_02                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[161],      "#define IMGC_GAMMA_R_GAIN_03                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[162],      "#define IMGC_GAMMA_R_GAIN_04                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[163],      "#define IMGC_GAMMA_R_GAIN_05                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[164],      "#define IMGC_GAMMA_R_GAIN_06                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[165],      "#define IMGC_GAMMA_R_GAIN_07                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[166],      "#define IMGC_GAMMA_R_GAIN_08                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[167],      "#define IMGC_GAMMA_R_GAIN_09                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[168],      "#define IMGC_GAMMA_R_GAIN_10                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[169],      "#define IMGC_GAMMA_R_GAIN_11                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[170],      "#define IMGC_GAMMA_R_GAIN_12                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[171],      "#define IMGC_GAMMA_R_GAIN_13                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[172],      "#define IMGC_GAMMA_R_GAIN_14                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[173],      "#define IMGC_GAMMA_R_GAIN_15                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[174],      "#define IMGC_GAMMA_R_GAIN_16                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[175],      "#define IMGC_GAMMA_R_GAIN_17                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[176],      "#define IMGC_GAMMA_R_GAIN_18                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[177],      "#define IMGC_GAMMA_R_GAIN_19                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[178],      "#define IMGC_GAMMA_R_GAIN_20                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[179],      "#define IMGC_GAMMA_R_GAIN_21                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[180],      "#define IMGC_GAMMA_R_GAIN_22                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[181],      "#define IMGC_GAMMA_R_GAIN_23                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[182],      "#define IMGC_GAMMA_R_GAIN_24                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[183],      "#define IMGC_GAMMA_R_GAIN_25                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[184],      "#define IMGC_GAMMA_R_GAIN_26                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[185],      "#define IMGC_GAMMA_R_GAIN_27                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[186],      "#define IMGC_GAMMA_R_GAIN_28                 ",                                                                                              NULL  }    /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[187],      "#define IMGC_GAMMA_R_GAIN_29                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[188],      "#define IMGC_GAMMA_R_GAIN_30                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[189],      "#define IMGC_GAMMA_R_GAIN_31                 ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[127],      "#define IMGC_GAMMA_R_TH_01                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[128],      "#define IMGC_GAMMA_R_TH_02                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[129],      "#define IMGC_GAMMA_R_TH_03                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[130],      "#define IMGC_GAMMA_R_TH_04                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[131],      "#define IMGC_GAMMA_R_TH_05                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[132],      "#define IMGC_GAMMA_R_TH_06                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[133],      "#define IMGC_GAMMA_R_TH_07                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[134],      "#define IMGC_GAMMA_R_TH_08                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[135],      "#define IMGC_GAMMA_R_TH_09                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[136],      "#define IMGC_GAMMA_R_TH_10                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[137],      "#define IMGC_GAMMA_R_TH_11                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[138],      "#define IMGC_GAMMA_R_TH_12                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[139],      "#define IMGC_GAMMA_R_TH_13                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[140],      "#define IMGC_GAMMA_R_TH_14                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[141],      "#define IMGC_GAMMA_R_TH_15                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[142],      "#define IMGC_GAMMA_R_TH_16                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[143],      "#define IMGC_GAMMA_R_TH_17                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[144],      "#define IMGC_GAMMA_R_TH_18                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[145],      "#define IMGC_GAMMA_R_TH_19                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[146],      "#define IMGC_GAMMA_R_TH_20                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[147],      "#define IMGC_GAMMA_R_TH_21                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[148],      "#define IMGC_GAMMA_R_TH_22                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[149],      "#define IMGC_GAMMA_R_TH_23                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[150],      "#define IMGC_GAMMA_R_TH_24                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[151],      "#define IMGC_GAMMA_R_TH_25                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[152],      "#define IMGC_GAMMA_R_TH_26                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[153],      "#define IMGC_GAMMA_R_TH_27                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[154],      "#define IMGC_GAMMA_R_TH_28                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[155],      "#define IMGC_GAMMA_R_TH_29                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[156],      "#define IMGC_GAMMA_R_TH_30                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_gamma[157],      "#define IMGC_GAMMA_R_TH_31                   ",                                                                                                  NULL  }/* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/* Dither Process */",                                                                                                  NULL  }                       /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_dither[0],       "#define IMGC_DITHER_MODE",                                                                                               gsp_dither_mode_list  }        /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_DITHER_2X2_PA                   (3u)                     /* This value is default. User can modify based on user settings. */",  NULL  }   /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_DITHER_2X2_PB                   (0u)                     /* This value is default. User can modify based on user settings. */",  NULL  }   /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_DITHER_2X2_PC                   (2u)                     /* This value is default. User can modify based on user settings. */",  NULL  }   /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "#define IMGC_DITHER_2X2_PD                   (1u)                     /* This value is default. User can modify based on user settings. */",  NULL  }   /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n/* Horizontal Mirroring and Rotation */",                                                                                                  NULL  }    /* Cast to an appropriate type */ 
,   {   READ_REG,    &gs_reg_rotation[0],     "#define IMGC_ROTATION_MODE ",                                                                                                NULL  }                    /* Cast to an appropriate type */ 
,   {   NOT_READ,    NULL,                    "\n#endif  /* IMG_CONFIG_H */",                                                                                                               NULL  }    /* Cast to an appropriate type */ 
};

/* whole update registers */
static st_all_reg_t const gs_update_reg [] =
{
    /* Cast to an appropriate type */
    {   "VDC6.INP_UPDATE",          &VDC6.INP_UPDATE.LONG,           0x00000011  }/* Cast to an appropriate type */
,   {   "VDC6.IMGCNT_UPDATE",       &VDC6.IMGCNT_UPDATE.LONG,        0x00000001  }/* Cast to an appropriate type */
,   {   "VDC6.SC0_SCL0_UPDATE",     &VDC6.SC0_SCL0_UPDATE.LONG,      0x00003111  }/* Cast to an appropriate type */
,   {   "VDC6.SC0_SCL1_UPDATE",     &VDC6.SC0_SCL1_UPDATE.LONG,      0x00110011  }/* Cast to an appropriate type */
,   {   "VDC6.ADJ0_UPDATE",         &VDC6.ADJ0_UPDATE.LONG,          0x00000001  }/* Cast to an appropriate type */
,   {   "VDC6.OUT_UPDATE",          &VDC6.OUT_UPDATE.LONG,           0x00000001  }/* Cast to an appropriate type */
,   {   "VDC6.GAM_G_UPDATE",        &VDC6.GAM_G_UPDATE.LONG,         0x00000001  }/* Cast to an appropriate type */
,   {   "VDC6.GAM_B_UPDATE",        &VDC6.GAM_B_UPDATE.LONG,         0x00000001  }/* Cast to an appropriate type */
,   {   "VDC6.GAM_R_UPDATE",        &VDC6.GAM_R_UPDATE.LONG,         0x00000001  }/* Cast to an appropriate type */
};

/* menu specification */
static const st_menu_t current_menu[CUI_MAX_MENUID]=
{
        /* menu ID                  kind of menu                number of branch            text1                           text2                               text of content                     register                        number of registers     */ 
    {   CUI_STATE_TOP,              CUI_PATTERN_TRANSITION,     CUI_SELNUM_TOP,             gs_top_content,                    gs_explanation_top_br_co,              gsp_top_select_list,            NULL,                         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT,           CUI_PATTERN_TRANSITION,     CUI_SELNUM_BRIGHT,          gs_bright_content,                 gs_explanation_top_br_co,              gsp_controller_select_list,     NULL,                         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_IN,        CUI_PATTERN_PRESET,         CUI_SELNUM_HML,             gs_bright_in_content,              gs_explanation_preset,                 gsp_hml_select_list,            &gs_reg_bright_in[0],          CUI_REGNUM_BRIGHT_IN    }/* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_OUT,       CUI_PATTERN_PRESET,         CUI_SELNUM_HML,             gs_bright_out_content,             gs_explanation_preset,                 gsp_hml_select_list,            &gs_reg_bright_out[0],         CUI_REGNUM_BRIGHT_OUT   }/* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_IN_C,      CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_BRIGHT_IN,       gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_bright_in[0],          CUI_REGNUM_BRIGHT_IN    } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_IN_C_0,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_bright_in[0],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_OUT_C,     CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_BRIGHT_OUT,      gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_bright_out[0],         CUI_REGNUM_BRIGHT_OUT   } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_OUT_C_0,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_bright_out[0],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_OUT_C_1,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_bright_out[1],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_BRIGHT_OUT_C_2,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_bright_out[2],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST,         CUI_PATTERN_TRANSITION,     CUI_SELNUM_CONTRAST,        gs_contra_content,                 gs_explanation_top_br_co,              gsp_controller_select_list,     NULL,                         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_IN,      CUI_PATTERN_PRESET,         CUI_SELNUM_HML,             gs_contra_in_content,              gs_explanation_preset,                 gsp_hml_select_list,            &gs_reg_cont_in[0],            CUI_REGNUM_CONTRAST_IN  }/* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_OUT,     CUI_PATTERN_PRESET,         CUI_SELNUM_HML,             gs_contra_out_content,             gs_explanation_preset,                 gsp_hml_select_list,            &gs_reg_cont_out[0],           CUI_REGNUM_CONTRAST_OUT }/* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_IN_C,    CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_CONTRAST_IN,     gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_cont_in[0],            CUI_REGNUM_CONTRAST_IN  } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_IN_C_0,  CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_cont_in[0],            0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_OUT_C,   CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_CONTRAST_OUT,    gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_cont_out[0],           CUI_REGNUM_CONTRAST_OUT } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_OUT_C_0, CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_cont_out[0],           0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_OUT_C_1, CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_cont_out[1],           0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_CONTRAST_OUT_C_2, CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_cont_out[2],           0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS,        CUI_PATTERN_PRESET,         CUI_SELNUM_SHARP,           gs_sharp_content,                  gs_explanation_preset,                 gsp_sharp_select_list,          &gs_reg_sharpness[0],          CUI_REGNUM_SHARP        }/* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C,      CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_SHARP,           gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_sharpness[0],          CUI_REGNUM_SHARP        } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_0,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[0],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_1,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[1],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_2,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[2],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_3,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[3],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_4,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[4],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_5,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[5],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_6,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[6],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_7,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[7],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_8,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[8],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_9,    CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[9],          0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_10,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[10],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_11,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[11],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_12,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[12],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_13,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[13],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_14,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[14],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_15,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[15],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_16,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[16],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_17,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[17],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_18,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[18],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_19,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[19],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_20,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[20],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_21,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[21],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_22,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[22],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_23,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[23],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_24,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[24],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_SHARPNESS_C_25,   CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_sharpness[25],         0                       } /* Cast to an appropriate type */ 
,   {   CUI_STATE_GAMMA,            CUI_PATTERN_PRESET,         CUI_SELNUM_GAMMA,           gs_gamma_content,                  gs_explanation_preset,                 gsp_gamma_select_list,          &gs_reg_gamma[0],              CUI_REGNUM_GAMMA        }/* Cast to an appropriate type */ 
,   {   CUI_STATE_DITHER,           CUI_PATTERN_PRESET,         CUI_SELNUM_DITHER,          gs_dither_content,                 gs_explanation_preset,                 gsp_dither_select_list,         &gs_reg_dither[0],             CUI_REGNUM_DITHER       }/* Cast to an appropriate type */ 
,   {   CUI_STATE_ROTATION,         CUI_PATTERN_PRESET,         CUI_SELNUM_ROTATION,        gs_rotation_content,               gs_explanation_preset,                 gsp_rotation_select_list,       &gs_reg_rotation[0],           CUI_REGNUM_ROTATION     }/* Cast to an appropriate type */ 
,   {   CUI_STATE_ROTATION_C,       CUI_PATTERN_REGISTERLIST,   CUI_REGNUM_ROTATION,        gs_all_custom,                     gs_explanation_customreg,              NULL,                          &gs_reg_rotation[0],           CUI_REGNUM_ROTATION     } /* Cast to an appropriate type */ 
,   {   CUI_STATE_ROTATION_C_0,     CUI_PATTERN_REGISTERSET,    0,                          gs_all_custom_reg,                 gs_explanation_custom,                 NULL,                          &gs_reg_rotation[0],           0                       } /* Cast to an appropriate type */ 
};

void cui_task(void *params);  
static void cui_local_init(void);
static uint32_t cui_menu_index_get(void);
static void cui_menu_disp(void);
static void cui_menu_disp_title(st_reg_t const * p_regstruct);
static void cui_menu_disp_explanation(st_reg_t const * p_regstruct);
static int32_t cui_get_target_bit_value(st_reg_t const *p_reg_info);
static void cui_serial_input(char_t * key_input);
static uint32_t cui_input_judge(char_t * key_input);
static uint32_t cui_state_transition(char_t * key_input);
static uint32_t cui_conv_num_to_char (uint32_t conv_num , char_t* buff, uint32_t* digit);
static uint32_t cui_conv_char_to_num (int32_t* conv_num , char_t* key_input);
static void cui_registerset(uint32_t reg_value, volatile uint32_t * reg_addr, uint32_t bit_mask, uint32_t bit_shift);
static void cui_set_update_register (void);


/******************************************************************************
 * Function Name: CreateCuiTask
 * Description  : .
 * Arguments    : .
 * Return Value : .
 *****************************************************************************/
void CreateCuiTask(void) 
{
	UINT    status;

    printf("Camera and Display Application \r\n");

    status = tx_thread_create (&p_graphics_thread, "Graphics Thread", cui_task, 0,
            &p_graphics_thread_stack, sizeof(p_graphics_thread_stack), 1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Cast to an appropriate type */
    if (status!=TX_SUCCESS)
    {
        printf("Camera driver can not be loaded, demo system does not work correctly\r\n");
    }
    R_OS_TaskSleep(1);
}
/*******************************************************************************
 End of function CreateCuiTask
 ******************************************************************************/

/******************************************************************************
* Function Name:    cui_task
* Description :     main task of CUI operation
* Arguments :       none
* Return Value :    none
******************************************************************************/
void cui_task(void *params) 
{
    uint32_t result = RESULT_OK;
    uint8_t test = 0;
    static uint8_t count_var = 0;
    char instruction_array[11] = {'0','1','2','3','4','5','D','C','b','T'};
    uint8_t instruction_order_array[91] = {/* Brightness */0,0,0,7,8,1,7,8,2,7,8,6,7,8,8,1,0,7,8,1,7,8,2,7,8,6,7,8,8,8,
                                            /* Contrast */ 1,0,0,7,8,1,7,8,2,7,8,6,7,8,8,1,0,7,8,1,7,8,2,7,8,6,7,8,8,8,
                                            /* Sharpness */                        2,0,7,8,1,7,8,2,7,8,6,7,8,8,
                                            /* Gamma Correction */                 3,0,1,2,3,6,8,
                                            /* Dither */                           4,0,1,2,3,6,8,
                                            /* Horizontal Mirroring and Rotation */5,8

                                            };

    /* Unused parameter */
    UNUSED_PARAM(params);

    /* Initialise */
    cui_local_init();


    while(1)
    {
        /* Get index of menu */
        gs_cur_index = cui_menu_index_get();

        /* Display menu */
        cui_menu_disp();

        do
        {
            R_OS_TaskSleep(1);

            /* Get command from serial */
            cui_serial_input(gs_key_input_data);

            /* Check string */
            result = cui_input_judge(gs_key_input_data);

            if (RESULT_OK == result)
            {
                /* action */
                result = cui_state_transition(gs_key_input_data);
            }
            else
            {
                __asm("nop");
            }

            if (RESULT_OK == result)
            {
                __asm("nop");
            }
            else
            {
                printf("ERROR : Invalid value\r\n\r\n");
            }

        }
        while (RESULT_OK != result);
    }
}
/*******************************************************************************
 End of function cui_task
 ******************************************************************************/

/******************************************************************************
* Function Name:    cui_local_init
* Description :     initialize local variables
* Arguments :       none
* Return Value :    none
******************************************************************************/
static void cui_local_init(void)
{
    int32_t i;
    uint32_t work_buff[CUI_REGNUM_GAMMA] = {0};
    uint32_t work_loop = 0;
    uint32_t work_loop_2 = 0;
    uint32_t work_tmp = 0;
    uint32_t preset_check_result = RESULT_OK;

    /* current menu */
    gs_cur_status = CUI_STATE_TOP;

    /* index of current_menu[] */
    gs_cur_index = cui_menu_index_get();

    /* input data */
    for(i = 0; i < SDK_PRV_KEY_INPUT_BUFF_NUM; i++)
    {
        gs_key_input_data[i] = 0;
    }

    /* current chosen setting */
    for(i = 0; i < CUI_MAX_MENUID; i++)
    {
        if (CUI_PATTERN_PRESET == current_menu[i].pattern )
        {
            st_reg_t const * p_regstruct = current_menu[i].regstruct;

            /* read current reg val */
            for (work_loop = 0; work_loop < current_menu[i].regnum; work_loop++)
            {
                work_tmp = *(p_regstruct[work_loop].reg_addr);
                work_buff[work_loop] = ((work_tmp >> p_regstruct[work_loop].bitshift)
                        &  p_regstruct[work_loop].bitmask);
            }

            /* check preset/default/custum */
            for (work_loop_2 = 0; work_loop_2 <= current_menu[i].selectnum; work_loop_2++)
            {
                preset_check_result = RESULT_OK;
                for (work_loop = 0; work_loop < current_menu[i].regnum; work_loop++)
                {
                    if (work_buff[work_loop] != p_regstruct[work_loop].preset[work_loop_2])
                    {
                        preset_check_result = RESULT_NG;
                        break;
                    }
                }
                if (RESULT_OK == preset_check_result)
                {
                    /* discovered preset num or custom */
                    break;
                }
                else
                {
                    __asm("nop");
                }
            }

            if (work_loop_2 > current_menu[i].selectnum)
            {
                /* set custom */
                gs_preset_value[i] = current_menu[i].selectnum + 1;
            }
            else
            {
                /* set preset num or default */
                gs_preset_value[i] = work_loop_2;
            }
        }
        else
        {
            gs_preset_value[i] = current_menu[i].selectnum;
        }
    }
}
/*******************************************************************************
 End of function cui_local_init
 ******************************************************************************/

/******************************************************************************
* Function Name:    cui_menu_index_get
* Description :     get index of current menu
* Arguments :       none
* Return Value :    none
******************************************************************************/
static uint32_t cui_menu_index_get (void)
{
    uint32_t i;

    for (i = 0; i < CUI_MAX_MENUID; i++)
    {
        if (current_menu[i].stateid == gs_cur_status)
        {
            break;
        }
        else
        {
            __asm("nop");
        }
    }
    if (CUI_MAX_MENUID == i)
    {
        i = 0;
        gs_cur_status = CUI_STATE_TOP;
    }
    else
    {
        __asm("nop");
    }

    return i;
}
/*******************************************************************************
 End of function cui_menu_index_get
 ******************************************************************************/

/******************************************************************************
* Function Name:    cui_menu_disp
* Description :     Display menu
* Arguments :       none
* Return Value :    none
******************************************************************************/
static void cui_menu_disp(void)
{
    /* Menu title */
    cui_menu_disp_title(current_menu[gs_cur_index].regstruct);

    /* Menu explanation */
    cui_menu_disp_explanation(current_menu[gs_cur_index].regstruct);

    printf("R : Show each value of the customizable registers.\r\n");

    if (CUI_STATE_TOP == gs_cur_status)
    {
        printf("D : All Default Settings.\r\n");
    }
    else
    {
        printf("B : Go to Back Menu.\r\n");
        printf("T : Go to Top Menu.\r\n");
    }
    R_OS_TaskSleep(1);
}
/******************************************************************************
End of function cui_menu_disp
******************************************************************************/

/******************************************************************************
* Function Name:    cui_menu_disp_title
* Description :     Display each menu title part.
* Arguments :       st_reg_t const * p_regstruct: Pointer to register struct of
                                               each menu.
* Return Value :    none
******************************************************************************/
static void cui_menu_disp_title(st_reg_t const * p_regstruct)
{
    /*** start printing menu ***/
    printf("\r\n");
    printf("---------------------------------------\r\n");

    /* Menu title */
    if (CUI_PATTERN_PRESET == current_menu[gs_cur_index].pattern)
    {
        printf("%s\r\n", current_menu[gs_cur_index].content);
        printf("Current Setting : %s\r\n", current_menu[gs_cur_index].selectlist[gs_preset_value[gs_cur_index]]);
    }
    else if (CUI_PATTERN_REGISTERSET == current_menu[gs_cur_index].pattern)
    {
        /* calculate value to print */
        int32_t target_val;
        target_val = cui_get_target_bit_value(p_regstruct);
        printf("%s %s\r\n", current_menu[gs_cur_index].content, p_regstruct->reg_name);

        /* Cast to an appropriate type */
        printf("Current Setting : %d\r\n", (int) target_val);
    }
    else
    {
        printf("%s\r\n", current_menu[gs_cur_index].content);
    }
    printf("---------------------------------------\r\n");
    R_OS_TaskSleep(1);
}
/******************************************************************************
End of function cui_menu_disp_title
******************************************************************************/

/******************************************************************************
* Function Name:    cui_menu_disp_explanation
* Description :     Display each menu explanation part.
* Arguments :       st_reg_t const * p_regstruct: Pointer to register struct of
                                               each menu.
* Return Value :    none
******************************************************************************/
static void cui_menu_disp_explanation(st_reg_t const * p_regstruct)
{
    uint32_t i = 0; /* Loop counter */

#if (TARGET_INPUT == INPUT_CMOS_CAM_1)
    printf("%s\r\n", current_menu[gs_cur_index].explanation);
#else
    if (CUI_STATE_ROTATION == current_menu[gs_cur_index].stateid)
    {
        /* DO NOTHING */
        ;
    }
    else
    {
        printf("%s\r\n", current_menu[gs_cur_index].explanation);
    }
#endif
    if ((CUI_STATE_BRIGHT == current_menu[gs_cur_index].stateid) ||
        (CUI_STATE_CONTRAST == current_menu[gs_cur_index].stateid))
    {
        printf("Please refer to the Application Note with regard to the Register setting block.\r\n");
        printf("(Input controller - Output controller)\r\n");
    }
    else if (CUI_STATE_GAMMA == current_menu[gs_cur_index].stateid)
    {
        printf("Please refer the startup guide about Pattern0-Pattern3\r\n");
    }
    else
    {
        __asm("nop");
    }

    /* list of registers */
    if (CUI_PATTERN_REGISTERLIST == current_menu[gs_cur_index].pattern)
    {
        /* print list */
        for (i = 0; i < current_menu[gs_cur_index].regnum; i++)
        {
            /* calculate value to print */
            int32_t target_val;
            target_val = cui_get_target_bit_value(p_regstruct + i);

            /* Cast to an appropriate type */
            printf("%d : %s = %d\r\n", (int)i, p_regstruct[i].reg_name, (int)target_val);
        }
    }
    /* menu to set custom register value */
    else if (CUI_PATTERN_REGISTERSET == current_menu[gs_cur_index].pattern)
    {
        /* max value and min value */
        printf("%d - %d :Input Value\r\n", (int)p_regstruct->reg_min, (int)p_regstruct->reg_max);
        printf("D : Default(Hardware Initial Value Refer to the Hardware Manual)\r\n");
    }
    /* menu to select preset settings */
    else if (CUI_PATTERN_PRESET == current_menu[gs_cur_index].pattern)
    {
#if (TARGET_INPUT == INPUT_CMOS_CAM_1)
        for (i = 0; i < current_menu[gs_cur_index].selectnum; i++)
        {
            printf("%d : %s\r\n", i, current_menu[gs_cur_index].selectlist[i]);
        }
        /* print command D and C. */
        printf("D : %s\r\n", current_menu[gs_cur_index].selectlist[i]);
        /* GAMMA does not have Custom menu */
        if ((CUI_STATE_GAMMA  != current_menu[gs_cur_index].stateid) &&
            (CUI_STATE_DITHER != current_menu[gs_cur_index].stateid))
        {
            printf("C : Custom\r\n");
        }
        else
        {
            /* DO NOTHING */
            ;
        }
#else
        if (CUI_STATE_ROTATION == current_menu[gs_cur_index].stateid)
        {
            printf("This function can not be used in case of camera input INPUT_CMOS_CAM_2 or INPUT_CMOS_CAM_3.\r\n");
            printf("If you would like to to use this function, you need to change the TARGET_INPUT setting\r\n");
            printf("from INPUT_CMOS_CAM_2 or INPUT_CMOS_CAM_3 to INPUT_CMOS_CAM_1.\r\n");
        }
        else
        {
            for (i = 0; i < current_menu[gs_cur_index].selectnum; i++)
            {
                /* Cast to an appropriate type */
                printf("%d : %s\r\n", (int)i, current_menu[gs_cur_index].selectlist[i]);
            }
            /* print command D and C. */
            printf("D : %s\r\n", current_menu[gs_cur_index].selectlist[i]);
            /* GAMMA does not have Custom menu */
            if ((CUI_STATE_GAMMA  != current_menu[gs_cur_index].stateid) &&
                (CUI_STATE_DITHER != current_menu[gs_cur_index].stateid))
            {
                printf("C : Custom\r\n");
            }
            else
            {
                /* DO NOTHING */
                ;
            }
        }
#endif
    }
    /* CUI_PATTERN_TRANSITION */
    else
    {
        for (i = 0; i < current_menu[gs_cur_index].selectnum; i++)
        {
            /* Cast to an appropriate type */
            printf("%d : %s\r\n", (int)i, current_menu[gs_cur_index].selectlist[i]);
        }
    }
    printf("----------------------------------------\r\n");
    R_OS_TaskSleep(1);
}
/******************************************************************************
End of function cui_menu_disp_explanation
******************************************************************************/

/******************************************************************************
* Function Name:    cui_get_target_bit_value
* Description :     Get target bit value of target register and return
                    the value by Dec
* Arguments :       st_reg_t const * p_reg_info: Pointer to register struct that
                                              include target bit.
* Return Value :    The value of target bit by Dec.
******************************************************************************/
static int32_t cui_get_target_bit_value(st_reg_t const *p_reg_info)
{
    /* calculate value of target bits */
    int32_t target_bit_val = *(p_reg_info->reg_addr);
    target_bit_val >>= p_reg_info->bitshift;
    target_bit_val &= p_reg_info->bitmask;

    /* minus value */
    if ((p_reg_info->reg_min < 0) && (p_reg_info->reg_max < target_bit_val))
    {
        target_bit_val = (target_bit_val - p_reg_info->bitmask) - 1;
    }
    else
    {
        __asm("nop");
    }
    return(target_bit_val);
}
/******************************************************************************
End of function cui_get_target_bit_value
******************************************************************************/

/******************************************************************************
* Function Name:    cui_serial_input
* Description :     receive serial input
* Arguments :       char_t * key_input (O)  pointer to store input.
* Return Value :    none
******************************************************************************/
static void cui_serial_input(char_t * key_input)
{
    uint32_t index = 0;
    int input_data = 0; 
    uint32_t result = RESULT_OK;

    /* Ensure that the parameter is not null */
    if (NULL == key_input)
    {
        __asm("nop");
    }
    else
    {
        do
        {
            /* print prompt */
            printf(">>");

            /* initialize */
            for(index = 0; index < SDK_PRV_KEY_INPUT_BUFF_NUM; index++)
            {
                key_input[index] = '\0';
            }

            index = 0;
            result = RESULT_OK;

            while(index < SDK_PRV_KEY_INPUT_BUFF_NUM)
            {
                /* Get character from UART */
                input_data = getchar();

                /* 0x08 (BS) 0x7F(DEL)*/
                if ((ASCII_BS == input_data) || (ASCII_DEL == input_data))
                {
                    if (index > 0)
                    {
                        key_input[index] = '\0';
                        index--;

                        putchar(ASCII_BS);
                        putchar(ASCII_SP);
                        putchar(ASCII_BS);

                    }
                    else
                    {
                        __asm("nop");
                    }
                }
#ifdef CR_MODE
                /* 0x0D (CR) */
                else if (ASCII_CR == input_data)
                {
                    if (index == 0)
                    {
                        /* Only CR case */
                        result = RESULT_NG;
                    }
                    else
                    {
                        /* Normal case */
                        key_input[index] = (char_t)input_data;
                    }
                    printf("\r\n");
                    break;
                }

#else
                /* 0x0D (CR) */
                else if (ASCII_CR == input_data)
                {
                    key_input[index] = (char_t)input_data;
                    putchar(key_input[index]);
                    index++;
                }

                /* 0x0A (LF) */
                else if (ASCII_LF == input_data)
                {
                    if (index < 1)
                    {
                        /* Only LF case */
                        result = RESULT_NG;
                    }
                    else if (key_input[index-1] != ASCII_CR )
                    {
                        /* Without CR case */
                        result = RESULT_NG;
                    }
                    else
                    {
                        if(index == 1)
                        {
                            /* Only CR+LF case */
                            result = RESULT_NG;
                        }
                        else
                        {
                            /* DO NOTHING */
                            ;
                        }

                        /* Normal case */
                        key_input[index] = (char_t)input_data;
                        putchar(key_input[index]);
                    }
                    break;
                }
#endif /* CR_MODE */

                /* 0x20(SPACE)-0x7E(~) */
                else if ((input_data >= ASCII_SP) && (input_data <= ASCII_TILDE))
                {
                    if (index < CUI_MAX_INPUT_NUM)
                    {
                        /* Cast to an appropriate type */
                        key_input[index] = (char_t)input_data;
                        putchar(key_input[index]);
                        index++;
                    }
                    else
                    {
#ifdef CR_MODE
                        /* Key input over MAX */
                        key_input[SDK_PRV_KEY_INPUT_BUFF_NUM - 1] = ASCII_CR;
                        printf("\r\n");
                        break;
#else
                        /* Key input over MAX */
                        key_input[SDK_PRV_KEY_INPUT_BUFF_NUM - 2] = ASCII_CR;
                        key_input[SDK_PRV_KEY_INPUT_BUFF_NUM - 1] = ASCII_LF;
                        printf("\r\n");
                        break;
#endif
                    }
                }
                else
                {
                    __asm("nop");
                }
            }
        } while (result == RESULT_NG);
    }

    return;
}
/******************************************************************************
End of function cui_serial_input
******************************************************************************/

/******************************************************************************
* Function Name:    cui_input_judge
* Description :     Judge input data
* Arguments :       char_t * key_input (I)  pointer to store input.
* Return Value :    uint32_t RESULT_OK / RESULT_NG
******************************************************************************/
static uint32_t cui_input_judge(char_t * key_input)
{
    uint32_t result = RESULT_OK;
    uint32_t index = 0;
    uint32_t num_figure = 0;

    /* Checking for NULL key_input */
    if (NULL == key_input)
    {
        __asm("nop");
    }
    else
    {
        switch(key_input[0])
        {
            /* These cases are intentionally combined. */
            case ASCII_B:
            case ASCII_b:
            case ASCII_C:
            case ASCII_c:
            case ASCII_D:
            case ASCII_d:
            case ASCII_R:
            case ASCII_r:
            case ASCII_T:
            case ASCII_t:
            {
                /* only 1 character is acceptable */
                if (ASCII_CR == key_input[1])
                {
                    result = RESULT_OK;
                }
                else
                {
                    result = RESULT_NG;
                }
                break;
            }

            /* These cases are intentionally combined. */
            case ASCII_HYPHEN:
            {

                if (ASCII_CR == key_input[1])
                {
                    result = RESULT_NG;
                    break;
                }
                else
                {
                    index = 1;
                }
                break;
            }

            /* GO THROUGH */
            case ASCII_0:
            case ASCII_1:
            case ASCII_2:
            case ASCII_3:
            case ASCII_4:
            case ASCII_5:
            case ASCII_6:
            case ASCII_7:
            case ASCII_8:
            case ASCII_9:
            {

                /* check numbers are under 10000 */
                num_figure = 0;
                result = RESULT_NG;
                for (num_figure = 0; num_figure <= 4; num_figure++)
                {
                    if (ASCII_CR == key_input[index])
                    {
                        result = RESULT_OK;
                        break;
                    }
                    else if ((ASCII_0 <= key_input[index])&&(key_input[index] <= ASCII_9))
                    {
                        index++;
                    }
                    else
                    {
                        result = RESULT_NG;
                        break;
                    }
                }
                break;
            }

            default:
            {
                result = RESULT_NG;
                break;
            }
        }
    }
    return result;
}
/******************************************************************************
End of function cui_input_judge
******************************************************************************/
/******************************************************************************
* Function Name:    cui_state_transition
* Description :     work depend on input
* Arguments :       char_t * key_input (I)  pointer to store input.
* Return Value :    uint32_t RESULT_OK / RESULT_NG
******************************************************************************/
static uint32_t cui_state_transition(char_t * key_input)
{
    uint32_t i = 0;
    uint32_t check = 0x000000FF;
    uint32_t result = 0;
    uint32_t conv_num = 0;

    /* Checking for NULL parameters */
    if (NULL == key_input)
    {
        result = RESULT_NG;
    }
    else
    {
        switch(key_input[0])
        {
            /* Go to TOP menu */
            case ASCII_T:
            case ASCII_t:
            {
                if (CUI_STATE_TOP == gs_cur_status)
                {
                    result = RESULT_NG;
                }
                else
                {
                    gs_cur_status = CUI_STATE_TOP;
                }
                break;
            }
            /* Back to parent menu */
            case ASCII_B:
            case ASCII_b:
            {
                if(gs_cur_status == CUI_STATE_TOP)
                {
                    result = RESULT_NG;
                }
                else
                {
                    check = 0x000000FF;
                    result = RESULT_NG;
                    for (i = 0; i <= 3; i++)
                    {
                        if((gs_cur_status & check) != check)
                        {
                            gs_cur_status |= check;
                            result = RESULT_OK;
                            break;
                        }
                        else
                        {
                            check = (check << SDK_PRV_SHIFT_1BYTES);
                        }
                    }
                }
                break;
            }

            /* Move to custom menu */
            case ASCII_C:
            case ASCII_c:
            {
                if(current_menu[gs_cur_index].pattern != CUI_PATTERN_PRESET)
                {
                    result = RESULT_NG;
                }
                #if    (TARGET_INPUT == INPUT_CMOS_CAM_1)
                else if((gs_cur_status == CUI_STATE_GAMMA)||(gs_cur_status == CUI_STATE_DITHER))
                #else
                else if(((gs_cur_status == CUI_STATE_GAMMA)||(gs_cur_status == CUI_STATE_DITHER))
                        ||(gs_cur_status == CUI_STATE_ROTATION))
                #endif
                {
                    result = RESULT_NG;
                }
                else
                {
                    gs_cur_status = (gs_cur_status & 0xFFFF00FF) | 0x00000C00;
                    result = RESULT_OK;
                }
                break;
            }
            /* Set default Value */
            case ASCII_D:
            case ASCII_d:
            {
                /* TOP menu: initialize whole registers */
                if(CUI_STATE_TOP == gs_cur_status)
                {
                    for (i = 0; i < ((sizeof(current_menu))/(sizeof(st_menu_t))); i++)
                    {
                        if (current_menu[i].pattern == CUI_PATTERN_PRESET)
                        {
                            uint32_t regset_loop = 0;
                            st_reg_t const * p_regstruct = current_menu[i].regstruct;
                            for (regset_loop = 0; regset_loop < current_menu[i].regnum; regset_loop++)
                            {
                                /* Cast to an appropriate type */
                                cui_registerset((uint32_t)p_regstruct[regset_loop].reg_default,
                                        p_regstruct[regset_loop].reg_addr, p_regstruct[regset_loop].bitmask,
                                        p_regstruct[regset_loop].bitshift);
                            }
                        }
                    }
                        cui_set_update_register();
                        for(i = 0; i < CUI_MAX_MENUID; i++)
                        {
                            gs_preset_value[i] = current_menu[i].selectnum;
                        }
                        result = RESULT_OK;
                }
                    /* PRESET menu: initialize whole registers of selected register group*/
                    else if(current_menu[gs_cur_index].pattern == CUI_PATTERN_PRESET)
                    {
                        st_reg_t const * p_regstruct = current_menu[gs_cur_index].regstruct;
                        #if (TARGET_INPUT == INPUT_CMOS_CAM_1)
                        for (i = 0; i < current_menu[gs_cur_index].regnum; i++)
                        {
                            cui_registerset(p_regstruct[i].reg_default, p_regstruct[i].reg_addr,
                                    p_regstruct[i].bitmask, p_regstruct[i].bitshift);
                        }
                            cui_set_update_register();
                            gs_preset_value[gs_cur_index] = current_menu[gs_cur_index].selectnum;
                            result = RESULT_OK;
                            #else
                            if(gs_cur_status == CUI_STATE_ROTATION)
                            {
                                result = RESULT_NG;
                            }
                            else
                            {
                                for (i = 0; i < current_menu[gs_cur_index].regnum; i++)
                                {
                                /* Cast to an appropriate type */
                                cui_registerset((uint32_t)p_regstruct[i].reg_default, p_regstruct[i].reg_addr,
                                        p_regstruct[i].bitmask, p_regstruct[i].bitshift);
                                }
                                cui_set_update_register();
                                gs_preset_value[gs_cur_index] = current_menu[gs_cur_index].selectnum;
                                result = RESULT_OK;
                            }
                            #endif
                }
                /* CUSTOM REGISTER setting menu: initialize the selected register */
                        else if(current_menu[gs_cur_index].pattern == CUI_PATTERN_REGISTERSET)
                        {
                            st_reg_t const *p_regstruct = current_menu[gs_cur_index].regstruct;

                                /* Cast to an appropriate type */
                                cui_registerset((uint32_t)p_regstruct[0].reg_default, p_regstruct[0].reg_addr,
                                        p_regstruct[0].bitmask, p_regstruct[0].bitshift);
                                    cui_set_update_register();
                                    gs_preset_value[gs_cur_index] = current_menu[gs_cur_index].selectnum + 1;
                                    result = RESULT_OK;
                        }
                                else
                                {
                                    result = RESULT_NG;
                                }
                break;
            }
            /* print whole registers */
            case ASCII_R:
            case ASCII_r:
            {
                char_t   work_char_buff[SDK_PRV_MAX_REG_BIT_VAL_DIGIT] = {0};
                uint32_t work_digit = 0;
                uint32_t work_val = 0;
                uint32_t work_loop = 0;
                for (i = 0; i < ((sizeof(gs_allparam))/(sizeof(st_header_info_t))); i++)
                {
                    printf("%s", gs_allparam[i].string);
                    if (gs_allparam[i].attribute != NOT_READ)
                    {
                        printf("(");
                        work_val = *(gs_allparam[i].reg_struct->reg_addr);
                        work_val = ((work_val >> gs_allparam[i].reg_struct->bitshift)
                                & gs_allparam[i].reg_struct->bitmask);

                        /*Ensuring that NULL items aren't processed */
                        if (gs_allparam[i].string_sub == NULL)
                        {
                            result = cui_conv_num_to_char(work_val, work_char_buff, &work_digit);
                            if (result == RESULT_OK)
                            {
                                for (work_loop = work_digit; work_loop < SDK_PRV_MAX_REG_BIT_VAL_DIGIT; work_loop++)
                                {
                                    printf("%c", work_char_buff[work_loop]);
                                }
                                printf("u");
                            }
                            else
                            {
                                printf("REG_READ_ERR!!");
                            }
                        }
                        else
                        {
                            printf("%s", gs_allparam[i].string_sub[work_val]);
                        }
                        printf(")");
                    }
                    printf("\r\n");

                    R_OS_TaskSleep(200);
                }
                result = RESULT_OK;
            }
                break;
            case ASCII_P:
            case ASCII_p:
            {
                result = RESULT_OK;
            }
                break;
            /* Number is received */
            case ASCII_0:
            case ASCII_1:
            case ASCII_2:
            case ASCII_3:
            case ASCII_4:
            case ASCII_5:
            case ASCII_6:
            case ASCII_7:
            case ASCII_8:
            case ASCII_9:
            case ASCII_HYPHEN:
            {

                /* get received number */
                result = cui_conv_char_to_num(&conv_num, key_input);
                if (result == RESULT_NG)
                {
                    __asm("nop");
                }
                /* set value of custom regiter */
                else if (current_menu[gs_cur_index].pattern == CUI_PATTERN_REGISTERSET)
                {
                    st_reg_t const *p_regstruct = current_menu[gs_cur_index].regstruct;

                    /* Cast to an appropriate type */
                    if ((p_regstruct->reg_min <= (int)conv_num) && ((int)conv_num <= p_regstruct->reg_max))
                    {
                        cui_registerset(conv_num, p_regstruct->reg_addr, p_regstruct->bitmask, p_regstruct->bitshift);
                        cui_set_update_register();
                        gs_preset_value[gs_cur_index] = current_menu[gs_cur_index].selectnum+1;
                        result = RESULT_OK;
                    }
                    else
                    {
                        result = RESULT_NG;
                    }
                }
                /* check num of branches */
                else if (current_menu[gs_cur_index].selectnum <= conv_num)
                {
                    result = RESULT_NG;
                }
                /* set preset values */
                else if (current_menu[gs_cur_index].pattern == CUI_PATTERN_PRESET)
                {
                    #if (TARGET_INPUT == INPUT_CMOS_CAM_1)
                    st_reg_t const *p_regstruct = current_menu[gs_cur_index].regstruct;
                    for (i = 0; i < current_menu[gs_cur_index].regnum; i++)
                    {
                        cui_registerset(p_regstruct[i].preset[conv_num], p_regstruct[i].reg_addr,
                                p_regstruct[i].bitmask, p_regstruct[i].bitshift);
                    }
                    cui_set_update_register();
                    gs_preset_value[gs_cur_index] = conv_num;
                    result = RESULT_OK;
                    #else
                    st_reg_t const *p_regstruct = current_menu[gs_cur_index].regstruct;
                    if (CUI_STATE_ROTATION == current_menu[gs_cur_index].stateid)
                    {
                        result = RESULT_NG;
                    }
                    else
                    {
                        for (i = 0; i < current_menu[gs_cur_index].regnum; i++)
                        {
                            cui_registerset(p_regstruct[i].preset[conv_num], p_regstruct[i].reg_addr,
                                    p_regstruct[i].bitmask, p_regstruct[i].bitshift);
                        }
                        cui_set_update_register();
                        gs_preset_value[gs_cur_index] = conv_num;
                        result = RESULT_OK;
                    }
                    #endif
                }
                /* move to selected menu */
                else
                {
                    /* Check Custom menu or not */
                    if( (gs_cur_status & 0x0000FF00) == 0x00000C00)
                    {
                        /* search LSB only */
                        i = 3;
                        check = 0x000000FF;
                        conv_num <<= SDK_PRV_SHIFT_0BYTES;
                    }
                    else
                    {
                        /* search from MSB to LSB */
                        i = 0;
                        check = 0xFF000000;
                        conv_num <<= SDK_PRV_SHIFT_3BYTES;
                    }

                    result = RESULT_NG;
                    for( ; i < 4; i++)
                    {
                        if( (gs_cur_status & check) == check)
                        {
                            gs_cur_status = (gs_cur_status & (~check)) | conv_num;
                            result = RESULT_OK;
                            break;
                        }
                        else
                        {
                            check    >>= SDK_PRV_SHIFT_1BYTES;
                            conv_num >>= SDK_PRV_SHIFT_1BYTES;
                        }
                    }
                }
                break;
            }

            default:
            {
                result = RESULT_NG;
                break;
            }
        }
    }
    return result;
}

/******************************************************************************
End of function cui_state_transition
******************************************************************************/

/******************************************************************************
* Function Name:    cui_conv_num_to_char
* Description :     Conversion from number to character
*                   The higher digit will be stored in the lower address
* Arguments :       uint32_t conv_num: (I) Conversion target number
*                   char_t*  buff:     (O) Conversion result character string
*                   uint32_t* digit:   (O) Number of significant digits
* Return Value :    uint32_t RESULT_OK/RESULT_NG
******************************************************************************/
static uint32_t cui_conv_num_to_char (uint32_t conv_num , char_t* buff, uint32_t* digit)
{
    uint32_t    loop;
    uint32_t    result = RESULT_OK;
    uint32_t    calc_digits = SDK_PRV_MAX_REG_BIT_VAL_DIGIT;
    uint32_t    div_val = 10000;
    uint32_t    div_result = 0;

    /* checking for NULL input parameters */
    if ((NULL == buff) || (NULL == digit))
    {
        result = RESULT_NG;
    }
    else
    {
        /* convert num to char */
        for (loop = 0; loop < calc_digits; loop++)
        {
            div_result = (conv_num / div_val);

            /* Cast to an appropriate type */
            buff[loop] = (char_t)(div_result + ASCII_0);
            conv_num   = conv_num - (div_result * div_val);
            div_val    = (div_val / 10);
        }

        /* check effective digit */
        for (loop = 0; loop < calc_digits; loop++)
        {
            if (ASCII_0 != buff[loop])
            {
                /*Escape from loop */
                loop = calc_digits+1;
            }
        }
        if (loop >= calc_digits)
        {
            loop = calc_digits - 1;
        }
        else
        {
            __asm("nop");
        }
        *digit = loop;
    }

    return result;
}
/******************************************************************************
End of function cui_conv_num_to_char
******************************************************************************/

/******************************************************************************
* Function Name:    cui_conv_char_to_num
* Description :     Conversion from the character to a number
* Arguments :       int32_t * conv_num: (O) Result number
*                   char_t * key_input:  (I) input string
* Return Value :    uint32_t RESULT_OK/RESULT_NG
******************************************************************************/
static uint32_t cui_conv_char_to_num (int32_t* conv_num , char_t* key_input)
{
    uint32_t    index = 0;
    int32_t    minus = 1;
    uint32_t    result = RESULT_OK;
    int32_t     sum = 0;

    /* Checking for num parameters */
    if ((NULL == conv_num) || (NULL == key_input))
    {
        result = RESULT_NG;
    }
    else
    {
        /* minus value? */
        if (ASCII_HYPHEN == key_input[0])
        {
            index = 1;
            minus = -1;
        }
        else
        {
            index = 0;
            minus = 1;
        }

        while (ASCII_CR != key_input[index])
        {
            /* number? */
            if ((ASCII_0 <= key_input[index]) && (key_input[index] <= ASCII_9))
            {
                sum = (sum * 10) + (key_input[index] - ASCII_0);
                index++;
            }
            else
            {
                result = RESULT_NG;
                break;
            }
        }

        if (RESULT_OK == result)
        {
            *conv_num = sum * minus;
        }
        else
        {
            __asm("nop");
        }
    }

    return result;
}
/******************************************************************************
End of function cui_conv_char_to_num
******************************************************************************/

/******************************************************************************
* Function Name:    cui_registerset
* Description :     set registers
* Arguments :       uint32_t reg_value  value to set
*                   int32_t * reg_addr  address of register
*                   uint32_t bit_mask   mask value
*                   uint32_t bit_shift  shift to set the target bit
* Return Value :    none
******************************************************************************/
static void cui_registerset(uint32_t reg_value, volatile uint32_t * reg_addr, uint32_t bit_mask, uint32_t bit_shift)
{
    uint32_t tmp = (*reg_addr) & (~(bit_mask << bit_shift));

    *reg_addr = tmp | ((reg_value & bit_mask) << bit_shift);

    return;
}
/******************************************************************************
End of function cui_registerset
******************************************************************************/

/******************************************************************************
* Function Name:    cui_set_update_register
* Description :     set update registers
* Arguments :       none
* Return Value :    none
******************************************************************************/
static void cui_set_update_register (void)
{
    uint32_t i;

    for (i = 0; i < ((sizeof(gs_update_reg))/(sizeof(st_all_reg_t))); i++)
    {
        /* Cast to an appropriate type */
        cui_registerset((uint32_t)gs_update_reg[i].reg_default, gs_update_reg[i].reg_addr, 0xFFFFFFFF, 0);
    }
    printf("Register Changed\r\n");
    R_OS_TaskSleep(100);
    return;
}
/******************************************************************************
End of function cui_set_update_register
******************************************************************************/
