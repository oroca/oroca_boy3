/*
 * LibBoy.c
 *
 *  Created on: 2019. 11. 7.
 *      Author: HanCheol Cho
 */

#include "hw.h"

#include "LibBoy.h"
#include "MSX.h"
#include "Console.h"
#include "EMULib.h"
#include "Sound.h"
#include "Record.h"



#define VERBOSE_VIDEO
#define WITH_OVERLAY


static char current_path[1024];




byte* ZBuf;

uint16_t lastBGColor = 0;



#define ILI9341_COLOR(r, g, b)\
  (((uint16_t)b) >> 3) |\
         ((((uint16_t)g) << 3) & 0x07E0) |\
         ((((uint16_t)r) << 8) & 0xf800)


uint8_t* msxFramebuffer;
pixel* cursor;

pixel* cursorBackGround;
int backGroundX = -1;
int backGroundY = -1;
int backGroundHeight = -1;
int backGroundWidth = -1;

QueueHandle_t videoQueue;
uint16_t VideoTaskCommand = 1;
static uint16_t* BPal;
static uint16_t* XPal;
static uint16_t XPal0;
Image overlay;



#include "CommonMux.h"





int InitVideo(void) {
    videoQueue = xQueueCreate(1, sizeof(uint16_t*));

    BPal = memMalloc(256*sizeof(uint16_t));
    XPal = memMalloc(80*sizeof(uint16_t));
    ZBuf = memMalloc(320);


    msxFramebuffer = (uint8_t*)memMalloc(WIDTH*HEIGHT*sizeof(uint8_t));
    memset(msxFramebuffer, 0, WIDTH*HEIGHT);
    if (!msxFramebuffer){ printf("malloc msxFramebuffer failed!\n"); return 0; }



#ifdef WITH_OVERLAY
     /* Menu overlay*/
    overlay.Data = (pixel*)memMalloc(WIDTH_OVERLAY*HEIGHT_OVERLAY*sizeof(pixel));
    if (!overlay.Data){ printf("malloc overlay failed!\n"); return 0; }

    memset(overlay.Data, 0, WIDTH_OVERLAY*HEIGHT_OVERLAY*sizeof(pixel));
    overlay.Cropped = 1;
    overlay.H = HEIGHT_OVERLAY;
    overlay.W = WIDTH_OVERLAY;
    overlay.D = sizeof(pixel)<<3;
    overlay.L = WIDTH_OVERLAY;
    SetVideo(&overlay,0,0,WIDTH_OVERLAY,HEIGHT_OVERLAY);
#endif

    /* Reset the palette */
    for(int J=0;J<16;J++) XPal[J]=255*J;
    XPal0=Black;


    /* Create SCREEN8 palette (GGGRRRBB) */
    for(int J=0;J<256;J++)
    {
      uint16_t r = (0xFF / 7) * (((J&0x1C)>> 2));
      uint16_t g = (0xFF / 7) * (((J&0xE0)>> 5));
      uint16_t b = J & 0x03;
      if (b == 1) b = 73;
      if (b == 2) b = 146;
      if (b == 3) b = 255;

      BPal[J]=ILI9341_COLOR(r,g,b);
      BPal[J]=( BPal[J] << 8) | (( BPal[J] >> 8) & 0xFF);

    }
    XPal[15] = Black;

     // clear screen
    //ili9341_write_frame_msx(0,0,WIDTH_OVERLAY,HEIGHT_OVERLAY, NULL, XPal[BGColor], XPal);


    //xTaskCreatePinnedToCore(&videoTask, "videoTask", 2048, NULL, 5, NULL, 1)
}











char *getcwd(char *__buf, size_t __size )
{

  if (f_getcwd(current_path, __size) == FR_OK)
  {
    printf("getcwd %s\n", current_path);
  }
  else
  {
    printf("getcwd fail\n");
  }

  return current_path;
}


/** SetColor() ***********************************************/
/** Set color N (0..15) to (R,G,B).                         **/
/************************************ TO BE WRITTEN BY USER **/
void SetColor(byte N,byte R,byte G,byte B) {
    register uint16_t col = PIXEL(R,G,B);
    col=( col << 8) | (( col >> 8) & 0xFF);
    if(N) XPal[N]=col; else XPal0=col;
}


