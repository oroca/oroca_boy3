/*
 * orocaboy.c
 *
 *  Created on: 2019. 11. 19.
 *      Author: HanCheol Cho
 */

#include "def.h"
#include "resize.h"
#include "speaker.h"
#include "ltdc.h"
#include "lcd.h"
#include "osd.h"
#include "button.h"
#include "eeprom.h"


#include "../../defs.h"
#include "../../pcm.h"
#include "../../rc.h"
#include "../../fb.h"
#include "../../loader.h"


extern uint32_t micros(void);
extern uint32_t millis(void);
extern void lcdClear(uint32_t rgb_code);
extern uint16_t *lcdGetCurrentFrameBuffer(void);
extern uint16_t *lcdGetFrameBuffer(void);
extern void lcdRequestDraw(void);
extern bool lcdDrawAvailable(void);
extern void resizeImageFast(resize_image_t *src, resize_image_t *dest);
extern void resizeImageFastOffset(resize_image_t *src, resize_image_t *dest);

struct pcm pcm;
struct fb fb;

static int stereo = 0;
static int samplerate = 11025;
static int sound = 1;

static int vid_mode = 0;



uint16_t image_buf[160 * 144];


rcvar_t pcm_exports[] =
{
  RCV_BOOL("sound", &sound),
  RCV_INT("stereo", &stereo),
  RCV_INT("samplerate", &samplerate),
  RCV_END
};

rcvar_t vid_exports[] =
{
  RCV_END
};

rcvar_t joy_exports[] =
{
  RCV_END
};



static uint32_t pre_time;

void *sys_timer()
{
  pre_time = micros();

  return (void *)&pre_time;
}

int sys_elapsed(uint32_t *cl)
{
  uint32_t now;
  int usecs;

  now = micros();
  usecs = now - *cl;
  *cl = now;
  return usecs;
}

void sys_sleep(int us)
{
  uint32_t start;
  if (us <= 0) return;
  start = micros();
  while((micros()-start) < us);
}

void sys_checkdir(char *path, int wr)
{
}

void sys_initpath(char *exe)
{
}

void sys_sanitize(char *s)
{
}

void pcm_init()
{
  int n;


  pcm.stereo = 0;
  n = samplerate;
  pcm.hz = n;
  pcm.len = n / 60;
  pcm.buf = malloc(pcm.len);

  speakerStart(samplerate);
  delay(100);
  speakerEnable();
}

void pcm_close()
{
  if (pcm.buf) free(pcm.buf);
  memset(&pcm, 0, sizeof pcm);
}

int pcm_submit()
{
  if (pcm.buf)
  {
    for (int i=0; i<pcm.pos; i++)
    {
      speakerPutch(pcm.buf[i]);
    }
  }
  pcm.pos = 0;
  return 1;
}





void vid_init()
{
  fb.w        = 160;
  fb.h        = 144;
  fb.pelsize  = 2;
  fb.pitch    = 160 * 2;
  fb.indexed  = 0;

  fb.cc[0].r = 8 - 5;
  fb.cc[1].r = 8 - 6;
  fb.cc[2].r = 8 - 4;
  fb.cc[0].l = 11;
  fb.cc[1].l = 5;
  fb.cc[2].l = 0;


  fb.ptr = (byte *)image_buf;

  fb.dirty    = 0;
  fb.enabled  = 1;

  vid_mode = eepromReadByte(_EEP_ADDR_VID_MODE);

  if (vid_mode >= 5)
  {
    vid_mode = 0;
    eepromWriteByte(_EEP_ADDR_VID_MODE, vid_mode);
  }
}

void vid_close()
{
  fb.enabled = 0;
}

void vid_preinit()
{

}

void vid_settitle(char *title)
{

}

void vid_begin()
{

}

void vid_change_mode(void)
{
  vid_mode = (vid_mode + 1) % 5;

  eepromWriteByte(_EEP_ADDR_VID_MODE, vid_mode);
}

void vid_end()
{
  static uint32_t pre_time;
  static uint32_t fps_count = 0;
  static int last_vid_mode = 0;

  fps_count++;
  if (millis()-pre_time >= 1000 )
  {
    pre_time = millis();
    printf("%d FPS\n", (int)fps_count);
    fps_count = 0;
  }


  while(!lcdDrawAvailable());

  resize_image_t src;
  resize_image_t dest;

  src.p_data = image_buf;
  src.w      = 160;
  src.h      = 144;

  switch(vid_mode)
  {
    case 0:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 160;
      dest.h      = 144;
      dest.stride = 320;
      dest.x = (320-dest.w)/2;
      dest.y = (240-dest.h)/2;

      if (last_vid_mode != 0)
      {
        lcdClear(0);
      }
      resizeImageNearest(&src, &dest);
      break;

    case 1:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 267;
      dest.h      = 240;
      dest.stride = 320;
      dest.x = (320-dest.w)/2;
      dest.y = (240-dest.h)/2;

      resizeImageNearest(&src, &dest);
      break;

    case 2:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 267;
      dest.h      = 240;
      dest.stride = 320;
      dest.x = (320-dest.w)/2;
      dest.y = (240-dest.h)/2;

      resizeImageFastOffset(&src, &dest);
      break;

    case 3:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 320;
      dest.h      = 240;
      dest.stride = 320;
      dest.x = (320-dest.w)/2;
      dest.y = (240-dest.h)/2;

      resizeImageNearest(&src, &dest);
      break;

    case 4:
      dest.p_data = lcdGetFrameBuffer();
      dest.w      = 320;
      dest.h      = 240;
      dest.stride = 320;
      dest.x = (320-dest.w)/2;
      dest.y = (240-dest.h)/2;

      resizeImageFastOffset(&src, &dest);
      break;
  }
  last_vid_mode = vid_mode;
  lcdRequestDraw();
}

void ev_poll()
{
  if (buttonGetPressedTime(_DEF_HW_BTN_Y) > 50 && buttonGetPressedEvent(_DEF_HW_BTN_Y) == true)
  {
    vid_change_mode();
  }
}

void vid_setpal(int i, int r, int g, int b)
{
  printf("setpal %d, %d %d %d\n", i, r, g, b);
}


void joy_init()
{
}

void joy_close()
{
}

void joy_poll()
{
}





bool osd_menu(void)
{
  uint32_t cursor = 0;
  uint32_t cursor_last = 0;
  uint32_t cursor_max = 4;
  uint32_t key;
  bool is_disp_on = false;
  uint16_t bg_color = orange;
  bool ret_exit = false;


  key = osdWaitKey(false);

  if ((key & (1<<_DEF_HW_BTN_X)) == 0)
  {
    return false;
  }

  speakerDisable();
  speakerStop();

  osdClear(bg_color);
  osdDrawFillRect(0, 0, osdGetWidth(), 20, blue);

  osdSetBgColor(blue);
  osdPrintf((osdGetWidth()-lcdGetStrWidth("MENU"))/2, 2, white, "MENU");

  buttonEnable(false);

  while(1)
  {
    osdSetBgColor(bg_color);
    osdPrintf(0, 46 + 20*0, white, " SAVE");
    osdPrintf(0, 46 + 20*1, white, " LOAD");
    osdPrintf(0, 46 + 20*2, white, " EXIT");
    osdPrintf(0, 46 + 20*3, white, " OK");

    osdDrawRect(2, 44 + 20*cursor, osdGetWidth()-4, 19, blue);
    if (cursor != cursor_last)
    {
      osdDrawRect(2, 44 + 20*cursor_last, osdGetWidth()-4, 19, bg_color);
    }

    if (is_disp_on != true)
    {
      is_disp_on = true;
      osdDisplayOn();
    }

    key = osdWaitKey(true);

    if (key & (1<<_DEF_HW_BTN_UP))
    {
      cursor_last = cursor;

      if (cursor == 0) cursor = cursor_max - 1;
      else             cursor = (cursor - 1) % cursor_max;
    }
    if (key & (1<<_DEF_HW_BTN_DOWN))
    {
      cursor_last = cursor;
      cursor = (cursor + 1) % cursor_max;
    }

    if (key & (1<<_DEF_HW_BTN_A))
    {
      if (cursor == 0) // SAVE
      {
        state_save(0);
        osdMessage("Saving...");
        delay(1000);
      }
      if (cursor == 1) // LOAD
      {
        state_load(0);
        osdMessage("Loading...");
        delay(1000);
      }
      if (cursor == 2) // EXIT
      {
        ret_exit = true;
      }
      break;
    }
  }

  osdDisplayOff();
  buttonEnable(true);
  speakerReStart();
  speakerEnable();

  return ret_exit;
}
