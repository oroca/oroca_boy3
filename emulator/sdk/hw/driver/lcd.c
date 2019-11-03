/*
 * lcd.c
 *
 *  Created on: 2019. 11. 2.
 *      Author: Baram
 */




#include "lcd.h"
#include "ltdc.h"
#include "gpio.h"
#include "hangul/PHan_Lib.h"


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif



static bool is_init = false;


extern uint16_t *ltdc_draw_buffer;



void lcdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr, uint16_t textcolor);


bool lcdInit(void)
{
  is_init = true;

  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_LOW);


  return true;
}

bool lcdIsInit(void)
{
  return is_init;
}

void lcdReset(void)
{

}

uint32_t lcdReadPixel(uint16_t x_pos, uint16_t y_pos)
{
  return ltdc_draw_buffer[y_pos * HW_LCD_WIDTH + x_pos];
}

void lcdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code)
{
  ltdc_draw_buffer[y_pos * HW_LCD_WIDTH + x_pos] = rgb_code;
}

void lcdClear(uint32_t rgb_code)
{
  lcdFillBuffer((void *)ltdc_draw_buffer, lcdGetWidth(), lcdGetHeight(), 0, rgb_code);
  lcdFillBuffer((void *)ltdcGetCurrentFrameBuffer(), lcdGetWidth(), lcdGetHeight(), 0, rgb_code);
}

bool lcdDrawAvailable(void)
{
  return ltdcDrawAvailable();
}

void lcdRequestDraw(void)
{
  ltdcRequestDraw();
}

void lcdUpdateDraw(void)
{
  lcdRequestDraw();
  while(lcdDrawAvailable() != true);
}

uint16_t *lcdGetFrameBuffer(void)
{
  return ltdcGetFrameBuffer();
}

uint16_t *lcdGetCurrentFrameBuffer(void)
{
  return ltdcGetCurrentFrameBuffer();
}

void lcdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);
  DMA2D->OCOLR   = ColorIndex;

  /* Set up pointers */
  DMA2D->OMAR    = (uint32_t)pDst;

  /* Set up offsets */
  DMA2D->OOR     = OffLine;

  /* Set up pixel format */
  DMA2D->OPFCCR  = LTDC_PIXEL_FORMAT_RGB565;

  /*  Set up size */
  DMA2D->NLR     = (uint32_t)(xSize << 16) | (uint32_t)ySize;

  DMA2D->CR     |= DMA2D_CR_START;

  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START)
  {
  }
}

void lcdDisplayOff(void)
{
  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_LOW);
}

void lcdDisplayOn(void)
{
  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_HIGH);
}

int32_t lcdGetWidth(void)
{
  return ltdcWidth();
}

int32_t lcdGetHeight(void)
{
  return ltdcHeight();
}

void lcdSetDoubleBuffer(bool enable)
{
  ltdcSetDoubleBuffer(enable);
}

bool lcdGetDoubleBuffer(void)
{
  return ltdcGetDoubleBuffer();
}



void lcdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);

  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++)
  {
    if (steep)
    {
      lcdDrawPixel(y0, x0, color);
    } else
    {
      lcdDrawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void lcdDrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  lcdDrawLine(x, y, x, y+h-1, color);
}

void lcdDrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  lcdDrawLine(x, y, x+w-1, y, color);
}

void lcdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i=x; i<x+w; i++)
  {
    lcdDrawVLine(i, y, h, color);
  }
}

void lcdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  lcdDrawHLine(x, y, w, color);
  lcdDrawHLine(x, y+h-1, w, color);
  lcdDrawVLine(x, y, h, color);
  lcdDrawVLine(x+w-1, y, h, color);
}

void lcdDrawFillScreen(uint16_t color)
{
  lcdDrawFillRect(0, 0, HW_LCD_WIDTH, HW_LCD_HEIGHT, color);
}

void lcdPrintf(int x, int y, uint16_t color,  const char *fmt, ...)
{
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[256];
  int Size_Char;
  int i, x_Pre = x;
  PHAN_FONT_OBJ FontBuf;


  len = vsnprintf(print_buffer, 255, fmt, arg);
  va_end (arg);

  for( i=0; i<len; i+=Size_Char )
  {
    PHan_FontLoad( &print_buffer[i], &FontBuf );


    disHanFont( x, y, &FontBuf, color);

    Size_Char = FontBuf.Size_Char;
    if (Size_Char >= 2)
    {
        x += 2*8;
    }
    else
    {
        x += 1*8;
    }

    if( HW_LCD_WIDTH < x )
    {
        x  = x_Pre;
        y += 16;
    }

    if( FontBuf.Code_Type == PHAN_END_CODE ) break;
  }
}

void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr, uint16_t textcolor)
{
  uint16_t    i, j, Loop;
  uint16_t  FontSize = FontPtr->Size_Char;
  uint16_t index_x;

  if (FontSize > 2)
  {
    FontSize = 2;
  }

  for ( i = 0 ; i < 16 ; i++ )        // 16 Lines per Font/Char
  {
    index_x = 0;
    for ( j = 0 ; j < FontSize ; j++ )      // 16 x 16 (2 Bytes)
    {
      for( Loop=0; Loop<8; Loop++ )
      {
        if( FontPtr->FontBuffer[i*FontSize +j] & (0x80>>Loop))
        {
          lcdDrawPixel(x + index_x, y + i, textcolor);
        }
        index_x++;
      }
    }
  }
}
