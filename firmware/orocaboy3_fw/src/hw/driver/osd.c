/*
 * osd.c
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */




#include "osd.h"
#include "ltdc.h"
#include "gpio.h"
#include "button.h"
#include "pwm.h"
#include "eeprom.h"

#include "hangul/PHan_Lib.h"


#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif



static int osd_width  = 200;
static int osd_height = 200;


static bool is_init = false;
static bool is_osd_run = false;


extern uint16_t *ltdc_osd_draw_buffer;



//static void osdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex);
static void disHanFont(int x, int y, PHAN_FONT_OBJ *FontPtr, uint16_t textcolor);
static void osdProcess(void);

bool osdInit(void)
{
  is_init = true;



  return true;
}

bool osdIsInit(void)
{
  return is_init;
}

void osdReset(void)
{

}

void osdUpdate(void)
{
  static uint8_t state = 0;
  static uint32_t pre_time = 0;


  switch(state)
  {
    case 0:
      if (buttonGetPressed(_DEF_HW_BTN_START) == true)
      {
        state = 1;
        pre_time = millis();
      }
      break;

    case 1:
      if (buttonGetPressed(_DEF_HW_BTN_START) != true)
      {
        state = 0;
      }
      else if (millis()-pre_time >= 2000)
      {
        state = 2;
        pre_time = millis();
        osdClear(black);
        osdDisplayOn();
        is_osd_run = true;
      }
      break;

    case 2:
      if (millis()-pre_time > 100)
      {
        state = 3;
      }
      break;

    case 3:
      if (buttonGetPressed(_DEF_HW_BTN_START) != true)
      {
        state = 4;
        osdProcess();
      }
      break;

    case 4:
      if (buttonGetPressed(_DEF_HW_BTN_START) == true)
      {
        state = 0;
        osdDisplayOff();
        is_osd_run = false;
      }
      break;
  }
}


uint32_t osdWaitKey(void)
{
  uint32_t ret = 0;
  bool key_pressed = false;
  uint32_t pre_time;

  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 150)
    {
      pre_time = millis();

      for (int i=0; i<BUTTON_MAX_CH; i++)
      {
        if (buttonGetPressed(i) == true)
        {
          ret = i;
          key_pressed = true;
          break;
        }
      }
      if (key_pressed == true)
      {
        break;
      }
    }
  }

  return ret;
}

void osdProcess(void)
{
  uint32_t cursor = 0;
  uint32_t cursor_last = 0;
  uint32_t cursor_max = 2;
  uint32_t key;
  uint32_t volume_level = 0;
  uint32_t bright_level = 0;
  uint32_t draw_bar;

  osdClear(black);
  osdDrawFillRect(0, 0, osd_width, 20, blue);

  osdPrintf((200-64)/2, 2, white, "OSD 설정");


  volume_level = eepromReadByte(_EEP_ADDR_VOLUME+0);
  bright_level = eepromReadByte(_EEP_ADDR_BRIGHT+0);


  while(1)
  {
    draw_bar = map(volume_level, 0, 100, 0, osd_width-80);
    osdDrawFillRect(0, 46 + 20*0, osd_width, 15, black);
    osdDrawFillRect(80, 46 + 20*0, draw_bar , 15, yellow);

    draw_bar = map(bright_level, 0, 100, 0, osd_width-80);
    osdDrawFillRect(0, 46 + 20*1, osd_width , 15, black);
    osdDrawFillRect(80, 46 + 20*1, draw_bar , 15, yellow);

    osdPrintf(0, 46 + 20*0, white, " 볼륨:%03d", volume_level);
    osdPrintf(0, 46 + 20*1, white, " 밝기:%03d", bright_level);

    osdDrawRect(0, 44 + 20*cursor, osd_width-1, 18, blue);
    if (cursor != cursor_last)
    {
      osdDrawRect(0, 44 + 20*cursor_last, osd_width-1, 18, black);
    }

    key = osdWaitKey();

    if (key == _DEF_HW_BTN_UP)
    {
      cursor_last = cursor;

      if (cursor == 0) cursor = cursor_max - 1;
      else             cursor = (cursor - 1) % cursor_max;
    }
    if (key == _DEF_HW_BTN_DOWN)
    {
      cursor_last = cursor;
      cursor = (cursor + 1) % cursor_max;
    }

    if (key == _DEF_HW_BTN_RIGHT)
    {
      if (cursor == 0)
      {
        volume_level = constrain(volume_level + 10, 0, 100);
      }
      if (cursor == 1)
      {
        bright_level = constrain(bright_level + 10, 0, 100);
      }
    }
    if (key == _DEF_HW_BTN_LEFT)
    {
      if (cursor == 0)
      {
        if (volume_level >= 5)
        {
          volume_level = constrain(volume_level - 5, 0, 100);
        }
      }
      if (cursor == 1)
      {
        if (bright_level >= 30)
        {
          bright_level = constrain(bright_level - 5, 0, 100);
        }
      }
    }

    pwmWrite(0, 255 - map(bright_level, 0, 100, 0, 255));


    if (key == _DEF_HW_BTN_START)
    {
      break;
    }
  }


  eepromWriteByte(_EEP_ADDR_BRIGHT+0, (uint8_t)bright_level);
  eepromWriteByte(_EEP_ADDR_BRIGHT+1, (uint8_t)~bright_level);
}




uint32_t osdReadPixel(uint16_t x_pos, uint16_t y_pos)
{
  return ltdc_osd_draw_buffer[y_pos * osdGetWidth() + x_pos];
}

void osdDrawPixel(uint16_t x_pos, uint16_t y_pos, uint32_t rgb_code)
{
  ltdc_osd_draw_buffer[y_pos * osdGetWidth() + x_pos] = rgb_code;
}

void osdClear(uint32_t rgb_code)
{
  osdDrawFillRect(0, 0, osd_width, osd_height, rgb_code);
  //osdFillBuffer((void *)ltdc_osd_draw_buffer, osdGetWidth(), osdGetHeight(), 0, rgb_code);
}

uint16_t *osdGetFrameBuffer(void)
{
  return ltdc_osd_draw_buffer;
}
#if 0
void osdFillBuffer(void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
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
#endif
void osdDisplayOff(void)
{
  ltdcSetAlpha(LTDC_LAYER_2, 0);
}

void osdDisplayOn(void)
{
  ltdcSetAlpha(LTDC_LAYER_2, 230);
}

int32_t osdGetWidth(void)
{
  return ltdcWidth();
}

int32_t osdGetHeight(void)
{
  return ltdcHeight();
}


void osdDrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,uint16_t color)
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
      osdDrawPixel(y0, x0, color);
    } else
    {
      osdDrawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}

void osdDrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  osdDrawLine(x, y, x, y+h-1, color);
}

void osdDrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  osdDrawLine(x, y, x+w-1, y, color);
}

void osdDrawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i=x; i<x+w; i++)
  {
    osdDrawVLine(i, y, h, color);
  }
}

void osdDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  osdDrawHLine(x, y, w, color);
  osdDrawHLine(x, y+h-1, w, color);
  osdDrawVLine(x, y, h, color);
  osdDrawVLine(x+w-1, y, h, color);
}

void osdDrawFillScreen(uint16_t color)
{
  osdDrawFillRect(0, 0, osdGetWidth(), osdGetHeight(), color);
}

void osdPrintf(int x, int y, uint16_t color,  const char *fmt, ...)
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

    if( osdGetWidth() < x )
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
          osdDrawPixel(x + index_x, y + i, textcolor);
        }
        index_x++;
      }
    }
  }
}
