
#include    <stdio.h>
#include    <string.h>

#include    "r_typedefs.h"
#include    "r_rvapi_vdc.h"

#include    "lcd_panel.h"
#include    "r_bcd_lcd.h"

#include    "gx_api.h"

#include    "gr_mango_display_driver.h"

#if (COLOR_FORMAT == GX_COLOR_FORMAT_24XRGB)

#define     DISPLAY_XRES R_BCD_LCD_WIDTH
#define     DISPLAY_YRES R_BCD_LCD_HEIGHT
#define     RZ_SCREEN_HANDLE 0x525A0000

#define     LAYER0_DISP_AREA_HS         (0u)
#define     LAYER0_DISP_AREA_VS         (0u)
#define     LAYER0_DISP_AREA_HW         (R_BCD_LCD_WIDTH)
#define     LAYER0_DISP_AREA_VW         (R_BCD_LCD_HEIGHT)

#define     VIDEO_BUFFER_STRIDE         ((LAYER0_DISP_AREA_HW + (R_BCD_LCD_STRIDE - 1)) & ~(R_BCD_LCD_STRIDE - 1))
#define     VIDEO_BUFFER_HEIGHT         (LAYER0_DISP_AREA_VW)

#pragma data_alignment=32
static GX_COLOR video_buffer[(VIDEO_BUFFER_STRIDE * VIDEO_BUFFER_HEIGHT)] __attribute__ ((section("Video_RAM")));

extern void IntCallbackFunc (vdc_int_type_t int_type);
extern vdc_error_t r_display_init(const vdc_channel_t vdc_ch);
extern vdc_error_t r_image_quality_adjustment(vdc_channel_t vdc_ch);
extern vdc_error_t r_video_quality_adjustment(vdc_channel_t vdc_ch);
extern void VDC_LcdPortSetting(uint32_t param);

void CopyCanvasToBackBuffer24xrgb(GX_CANVAS *canvas, GX_RECTANGLE *copy)
{
    GX_RECTANGLE display_size;
    GX_RECTANGLE copy_clip;
    void   *flushaddress;
    ULONG *pPutRow;
    ULONG *pGetRow;
    ULONG *pGet;
    ULONG *pPut;
    int row;
    int col;
    int copy_width;
    int copy_height;

    gx_utility_rectangle_define(&display_size, 0, 0, DISPLAY_XRES - 1, DISPLAY_YRES - 1);
    copy_clip = *copy;

    gx_utility_rectangle_shift(&copy_clip, canvas->gx_canvas_display_offset_x, canvas->gx_canvas_display_offset_y);
    gx_utility_rectangle_overlap_detect(&copy_clip, &display_size, &copy_clip);
    copy_width = (copy_clip.gx_rectangle_right - copy_clip.gx_rectangle_left + 1);
    copy_height = copy_clip.gx_rectangle_bottom - copy_clip.gx_rectangle_top + 1;

    if (copy_width <= 0 ||
        copy_height <= 0)
    {
        return;
    }
    
    pGetRow = (ULONG *) canvas -> gx_canvas_memory;
    pGetRow += copy->gx_rectangle_top * canvas->gx_canvas_x_resolution;
    pGetRow += copy->gx_rectangle_left;

    pPutRow = &video_buffer[0];
    pPutRow += (canvas ->gx_canvas_display_offset_y + copy->gx_rectangle_top) * DISPLAY_XRES;
    pPutRow += (canvas ->gx_canvas_display_offset_x + copy->gx_rectangle_left);
    flushaddress = pPutRow;

    for (row = 0; row < copy_height; row++)
    {
        pGet = pGetRow;
        pPut = pPutRow;
        
        for (col = 0; col < copy_width; col++)
        {
            *pPut++ = *pGet++;
        }
        pGetRow += canvas->gx_canvas_x_resolution;
        pPutRow += DISPLAY_XRES;
    }
    R_CACHE_L1DataCleanInvalidAll();
}

void gr_mango_24xrgb_buffer_toggle(GX_CANVAS *canvas, GX_RECTANGLE *dirty)
{
    GX_RECTANGLE Limit;
    GX_RECTANGLE Copy;
        
    gx_utility_rectangle_define(&Limit, 0, 0,
        canvas->gx_canvas_x_resolution -1,
        canvas->gx_canvas_y_resolution -1);
    
    if (gx_utility_rectangle_overlap_detect(&Limit, &canvas->gx_canvas_dirty_area, &Copy))
    {
        /* copy our canvas to the back buffer */
        CopyCanvasToBackBuffer24xrgb(canvas, &Copy);
    }
}

int32_t gr_mango_24xrgb_lcd_init(void)
{
    vdc_error_t error;
    vdc_channel_t vdc_ch = VDC_CHANNEL_0;
    uint32_t table_no;

    /***********************************************************************/
    /* display init (VDC6 output setting) */
    /***********************************************************************/
    error = r_display_init (vdc_ch);

    /***********************************************************************/
    /* Interrupt init */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        uint16_t line_num;

        line_num = LCD_CH0_DISP_VS + R_BCD_LCD_HEIGHT;
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_VLINE, line_num, IntCallbackFunc);
    }

    if (error == VDC_OK)
    {
        error = R_RVAPI_InterruptEnableVDC(vdc_ch, VDC_INT_TYPE_S0_LO_VSYNC, 0, IntCallbackFunc);
    }

    /***********************************************************************/
    /* Graphic Layer 0 VDC6_GR_FORMAT_CLUT8 */
    /***********************************************************************/
    if (error == VDC_OK)
    {
        gr_surface_disp_config_t gr_disp_cnf;

        gr_disp_cnf.layer_id  = VDC_LAYER_ID_0_RD;          /* Layer ID                        */
        gr_disp_cnf.fb_buff   = (void *)&video_buffer[0];   /* Frame buffer address            */
        gr_disp_cnf.fb_stride = VIDEO_BUFFER_STRIDE * sizeof(GX_COLOR);
                                                            /* Frame buffer stride             */

        /* Display Area               */
        gr_disp_cnf.disp_area.hs_rel = LAYER0_DISP_AREA_HS;
        gr_disp_cnf.disp_area.hw_rel = LAYER0_DISP_AREA_HW;
        gr_disp_cnf.disp_area.vs_rel = LAYER0_DISP_AREA_VS;
        gr_disp_cnf.disp_area.vw_rel = LAYER0_DISP_AREA_VW;

        gr_disp_cnf.read_format = VDC_GR_FORMAT_RGB888;     /* Read Format RGB888   */
        gr_disp_cnf.read_swap   = VDC_WR_RD_WRSWA_32BIT;    /* Read Swap 32bit      */
        gr_disp_cnf.disp_mode   = VDC_DISPSEL_CURRENT;      /* Display mode select  */

        error = R_RVAPI_GraphCreateSurfaceVDC(vdc_ch, &gr_disp_cnf);
    }

    /* image and video quality adjustment */
    if (VDC_OK == error)
    {
        r_image_quality_adjustment(vdc_ch);
    }

    /* Display On */
    if (VDC_OK == error)
    {
        R_RVAPI_DispPortSettingVDC(vdc_ch, &VDC_LcdPortSetting);
    }

    return 0;
}

UINT gr_mango_graphics_driver_setup_24xrgb(GX_DISPLAY *display)
{
    _gx_display_driver_24xrgb_setup(display, GX_NULL, gr_mango_24xrgb_buffer_toggle);
    display->gx_display_handle = RZ_SCREEN_HANDLE;
    
    gr_mango_24xrgb_lcd_init();

    return GX_SUCCESS;
}

#endif /* (COLOR_FORMAT == GX_COLOR_FORMAT_24XRGB) */
