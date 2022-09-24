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
/**************************************************************************//**
* @file         draw.c
* @version      0.01
* @brief        graphics draw sample
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "draw.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

/******************************************************************************
Private global variables and functions
******************************************************************************/
static draw_layer_t DrawLayerID( vdc_layer_id_t layer_id );
static draw_buffer_info buffer_info[DRAW_LAYER_NUM];

/**************************************************************************//**
 * @fn          DrawInit
 * @brief       DrawInit
 * @param[in]   layer_id          : layer id
 * @param[in]   info              : buffer info
 * @retval      None
******************************************************************************/
void DrawInit( vdc_layer_id_t layer_id, draw_buffer_info * info ) 
{
    draw_layer_t draw_layer;
    draw_layer = DrawLayerID(layer_id);

    buffer_info[draw_layer].buff_addr1        = info->buff_addr1;
    buffer_info[draw_layer].buff_addr2        = info->buff_addr2;
    buffer_info[draw_layer].buff_select       = 0;
    buffer_info[draw_layer].buff_stride       = info->buff_stride;
    buffer_info[draw_layer].buff_bit_per_byte = info->buff_bit_per_byte;
    buffer_info[draw_layer].window_width      = info->window_width;
    buffer_info[draw_layer].window_height     = info->window_height;
}   /* End of function DrawInit() */

/**************************************************************************//**
 * @fn          DrawSwapBuffer
 * @brief       DrawSwapBuffer
 * @param[in]   layer_id          : layer id
 * @retval      None
******************************************************************************/
void DrawSwapBuffer( vdc_layer_id_t layer_id )
{
    draw_layer_t draw_layer;
    draw_layer = DrawLayerID(layer_id);

    if (buffer_info[draw_layer].buff_select == 0u)
    {
        /* Cast to an appropriate type */
        buffer_info[draw_layer].buff_select= (uint32_t)1u;
    }
    else
    {
        /* Cast to an appropriate type */
        buffer_info[draw_layer].buff_select = (uint32_t)0u;
    }
}   /* End of function DrawSwapBuffer() */

/**************************************************************************//**
 * @fn          DrawGetBackBuffer
 * @brief       DrawGetBackBuffer
 * @param[in]   layer_id          : layer id
 * @retval      None
******************************************************************************/
void * DrawGetBackBuffer ( vdc_layer_id_t layer_id ) 
{
    draw_layer_t draw_layer;
    draw_layer = DrawLayerID(layer_id);

    return ((buffer_info[draw_layer].buff_select == 0u) ? buffer_info[draw_layer].buff_addr1
            : buffer_info[draw_layer].buff_addr2 );
}   /* End of function DrawGetBackBuffer() */

/**************************************************************************//**
 * @fn          DrawPoint
 * @brief       DrawPoint
 * @param[in]   layer_id          : layer id
 * @param[in]   x                 : position x
 * @param[in]   y                 : position y
 * @param[in]   color             : 4bit CLUT No
 * @retval      None
******************************************************************************/
void DrawPoint( vdc_layer_id_t layer_id, int32_t x, int32_t y , uint8_t color )
{
    uint8_t * vram;
    uint8_t   data;
    uint32_t  width;
    uint32_t  height;
    uint32_t  stride;
    draw_layer_t draw_layer;

    draw_layer = DrawLayerID(layer_id);

    /* buffer get */
    vram = ((buffer_info[draw_layer].buff_select == 0u) ? buffer_info[draw_layer].buff_addr1
            : buffer_info[draw_layer].buff_addr2 );
    width  = buffer_info[draw_layer].window_width;
    height = buffer_info[draw_layer].window_height;
    stride = buffer_info[draw_layer].buff_stride;

    /* area check */
    if( (x < 0) || (int)(width <= x) || (y < 0) || (int)(height <= y) )
    {
        return;
    }

    /* draw */
    data = vram[ (x/2) + (stride * y)];

    if( (x & 1) == 0 )
    {
        data &= 0x0F;
        data |= (color << 4);
    }
    else
    {
        data &= 0xF0;
        data |= (color & 0x0F);
    }
    vram[ (x/2) + (stride * y)] = data;
}   /* End of function DrawPoint() */

/**************************************************************************//**
 * @fn          DrawLine
 * @brief       DrawLine
 * @param[in]   layer_id          : layer id
 * @param[in]   x1                : position x1
 * @param[in]   y1                : position y1
 * @param[in]   x2                : position x2
 * @param[in]   y2                : position y2
 * @param[in]   color             : 4bit CLUT No
 * @retval      None
******************************************************************************/
void DrawLine
    ( vdc_layer_id_t layer_id, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint8_t color )
{
    int32_t dx=x2-x1;
    int32_t dy=y2-y1;
    int32_t sx=1;
    int32_t sy=1;
    int32_t i;
    int32_t de;


    if( dx<0 )
    {
        dx *= (-1);
        sx *= (-1);
    }

    if( dy<0 )
    {
        dy *= (-1);    sy *= (-1);
    }

    DrawPoint( layer_id, x1, y1, color );

    if( dx>dy )
    {
        for( i=dx, de=i/2; i; i-- )
        {
            x1 += sx;
            de += dy;
            if( de>dx )
            {
                de -= dx;
                y1 += sy;
            }
            DrawPoint( layer_id, x1, y1, color );
        }
    }
    else
    {
        for( i=dy, de=i/2; i; i-- )
        {
            y1 += sy;
            de += dx;
            if( de>dy )
            {
                de -= dy;
                x1 += sx;
            }
            DrawPoint( layer_id, x1, y1, color );
        }
    }
}   /* End of function DrawLine() */

/**************************************************************************//**
 * @fn          DrawRect
 * @brief       DrawRect
 * @param[in]   layer_id          : layer id
 * @param[in]   x                 : position x
 * @param[in]   y                 : position y
 * @param[in]   width             : rect width
 * @param[in]   height            : rect height
 * @param[in]   color             : 4bit CLUT No
 * @retval      None
******************************************************************************/
void DrawRect
    ( vdc_layer_id_t layer_id, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color)
{
    DrawLine( layer_id, x,               y,                ((x + width)-1), y,                color );
    DrawLine( layer_id, ((x + width)-1), y,                ((x + width)-1), ((y + height)-1), color );
    DrawLine( layer_id, ((x + width)-1), ((y + height)-1), x,               ((y + height)-1), color );
    DrawLine( layer_id, x,               ((y + height)-1), x,               y,                color );
}   /* End of function DrawRect() */

/**************************************************************************//**
 * @fn          DrawRectFill
 * @brief       DrawRectFill
 * @param[in]   layer_id          : layer id
 * @param[in]   x                 : position x
 * @param[in]   y                 : position y
 * @param[in]   width             : rect width
 * @param[in]   height            : rect height
 * @param[in]   color             : 4bit CLUT No
 * @retval      None
******************************************************************************/
void DrawRectFill
    ( vdc_layer_id_t layer_id, int32_t x, int32_t y, int32_t width, int32_t height, uint8_t color)
{
    int32_t i;

    for( i = 0 ; i < height; i++ )
    {
        DrawLine( layer_id, x, y+i,  x + width, y+i, color );
    }
}   /* End of function DrawRectFill() */

/**************************************************************************//**
 * @fn          DrawFont1bpp
 * @brief       DrawFont1bpp
 * @param[in]   layer_id          : layer id
 * @param[in]   x                 : position x
 * @param[in]   y                 : position y
 * @param[in]   fwidth            : font width
 * @param[in]   fheight           : font height
 * @param[in]   fcolor            : front color 4bit CLUT No
 * @param[in]   bcolor            : back color  4bit CLUT No
 * @param[in]   font_data         : font data pointer
 * @retval      None
******************************************************************************/
void DrawFont1bpp( vdc_layer_id_t layer_id, 
                    int32_t x, int32_t y,
                    int32_t fwidth, int32_t fheight,
                    uint8_t fcolor, uint8_t bcolor,
                    uint8_t *font_data )
{

    uint8_t   data;
    uint8_t  *pfont;
    uint32_t  pixel_count;
    uint32_t  read_byte_count;
    uint8_t   mask;
    uint32_t  i,j,k;
    uint32_t  pixel_total;

    pfont = font_data;

    read_byte_count = ((fwidth + 7)/ 8);

    for( k = 0 ; k < fheight; k ++ )
    {
        pixel_total = 0;

        for( i = 0 ; i < read_byte_count ; i ++ )
        {
            data = *pfont++;
            if( (fwidth/(8*read_byte_count)) == 1 )
            {
                pixel_count = 8;
            }
            else
            {
                pixel_count = (fwidth % 8);
            }

            mask = 0x80;
            for( j = 0; j  < pixel_count ; j++ )
            {
                if( data & mask )
                {
                    DrawPoint( layer_id, x+pixel_total, y+k, fcolor );
                }
                else
                {
                    DrawPoint( layer_id, x+pixel_total, y+k, bcolor );
                }
                mask = mask >> 1;
                pixel_total++;
            }
        }
    }
}   /* End of function DrawFont1bpp() */

/**************************************************************************//**
 * @fn          DrawLayerID
 * @brief       DrawLayerID
 * @param[in]   layer_id          : layer id
 * @retval      draw_layer_t
******************************************************************************/
static draw_layer_t DrawLayerID( vdc_layer_id_t layer_id )
{
    draw_layer_t draw_layer = DRAW_LAYER0;

    if(    VDC_LAYER_ID_0_RD == layer_id )
    {
        draw_layer = DRAW_LAYER0;
    }
    else if( VDC_LAYER_ID_2_RD == layer_id )
    {
        draw_layer = DRAW_LAYER2;
    }
    else if( VDC_LAYER_ID_3_RD == layer_id )
    {
        draw_layer = DRAW_LAYER3;
    }
    return draw_layer;
}   /* End of function DrawLayerID() */
