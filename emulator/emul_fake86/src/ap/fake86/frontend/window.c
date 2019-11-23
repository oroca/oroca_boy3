/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2013 Mike Chambers
               2019      Aidan Dodds

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
  USA.
*/

#include "frontend.h"
#include "lcd.h"


#define PIXEL(R,G,B)    (uint16_t)(((31*(R)/255)<<11)|((63*(G)/255)<<5)|(31*(B)/255))
#define RMASK           0xF800
#define GMASK           0x07E0
#define BMASK           0x001F



// params
bool do_fullscreen;
uint32_t frame_skip;

//static SDL_Surface *_surface;
static uint32_t frame_index;


uint32_t *frame_buf;


void win_fs_toggle(void) {
//  assert(_surface);
//  const int flags = _surface->flags ^ SDL_FULLSCREEN;
//  _surface = SDL_SetVideoMode(_surface->w, _surface->h, 32, flags);
//  if (!_surface)
//  {
//    log_printf(LOG_CHAN_VIDEO, "SDL_SetVideoMode failed");
//  }
//  SDL_WM_SetCaption(BUILD_STRING, NULL);
}

bool win_init(void) {
#if 0
  const int flags =
    (do_fullscreen ? SDL_FULLSCREEN : 0);

  _surface = SDL_SetVideoMode(640, 480, 32, flags);
  if (!_surface) {
    log_printf(LOG_CHAN_VIDEO, "SDL_SetVideoMode failed");
    return false;
  }
  SDL_WM_SetCaption(BUILD_STRING, NULL);
#endif

  frame_buf = (uint32_t *)malloc(640*480*4);

  return true;
}

void win_render(void) {

  static uint32_t pre_time_skip;


  if (millis()-pre_time_skip < 1000)
  {
    return;
  }

  pre_time_skip = millis();


  ++frame_index;
  if (frame_index >= frame_skip) {
    frame_index = 0;
  }

  if (frame_index != 0) {
    return;
  }
#if 0
  SDL_FillRect(_surface, NULL, 0x050505);

  struct render_target_t target = {
    (uint32_t*)_surface->pixels,
    _surface->w,
    _surface->h,
    _surface->pitch / sizeof(uint32_t)
  };

  neo_render_tick(&target);
  osd_render(&target);

  SDL_Flip(_surface);
#endif

  struct render_target_t target =
  {
    (uint32_t*)frame_buf,
    640,
    480,
    640
  };

  if (lcdDrawAvailable() != true)
  {
    return;
  }

  uint32_t pre_time;

  pre_time = millis();
  neo_render_tick(&target);



  uint16_t *p_buf = lcdGetFrameBuffer();
  uint32_t pixel;
  uint8_t r;
  uint8_t g;
  uint8_t b;


  for (int y=0; y<240; y++)
  {
    for (int x=0; x<320; x++)
    {
      pixel = frame_buf[(y<<1)*640 + (x<<1)];
      r = (pixel>>16)&0xFF;
      g = (pixel>> 8)&0xFF;
      b = (pixel>> 0)&0xFF;
      p_buf[y*320 + x] = PIXEL(r, g, b);
    }
  }
  lcdRequestDraw();

  printf("win_render %dms\n", millis()-pre_time);
}

void win_size(uint32_t *w, uint32_t *h)
{
  assert(w && h);
  //*w = _surface->w;
  //*h = _surface->h;
}
