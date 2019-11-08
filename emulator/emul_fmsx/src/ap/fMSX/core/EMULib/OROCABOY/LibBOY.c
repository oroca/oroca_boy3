/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                        LibUnix.c                        **/
/**                                                         **/
/** This file contains Unix-dependent implementation        **/
/** parts of the emulation library.                         **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2018                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "EMULib.h"
#include "Sound.h"
#include "Console.h"

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#ifdef MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include "FatFs/src/ff.h"


#define FPS_COLOR PIXEL(255,0,255)

extern int MasterSwitch; /* Switches to turn channels on/off */
extern int MasterVolume; /* Master volume                    */

static volatile int TimerReady = 0;   /* 1: Sync timer ready */
static volatile unsigned int JoyState = 0; /* Joystick state */
static volatile unsigned int LastKey  = 0; /* Last key prsd  */
static volatile unsigned int KeyModes = 0; /* SHIFT/CTRL/ALT */

static int Effects    = EFF_SCALE|EFF_SAVECPU; /* EFF_* bits */
static int TimerON    = 0; /* 1: sync timer is running       */
static Image OutImg;       /* Scaled output image buffer     */
static const char *AppTitle; /* Current window title         */
static int XSize,YSize;    /* Current window dimensions      */

static int FrameCount;      /* Frame counter for EFF_SHOWFPS */
static int FrameRate;       /* Last frame rate value         */
static struct timeval TimeStamp; /* Last timestamp           */

/** TimerHandler() *******************************************/
/** The main timer handler used by SetSyncTimer().          **/
/*************************************************************/
static void TimerHandler(int Arg)
{
  /* Mark sync timer as "ready" */
  TimerReady=1;
  /* Repeat signal next time */
  signal(Arg,TimerHandler);
}

/** InitUnix() ***********************************************/
/** Initialize Unix/X11 resources and set initial window    **/
/** title and dimensions.                                   **/
/*************************************************************/
int InitUnix(const char *Title,int Width,int Height)
{
  /* Initialize variables */
  AppTitle    = Title;
  XSize       = Width;
  YSize       = Height;
  TimerON     = 0;
  TimerReady  = 0;
  JoyState    = 0;
  LastKey     = 0;
  KeyModes    = 0;
  FrameCount  = 0;
  FrameRate   = 0;

  /* Get initial timestamp */
  gettimeofday(&TimeStamp,0);

  /* No output image yet */



  /* Done */
  return(1);
}

/** TrashUnix() **********************************************/
/** Free resources allocated in InitUnix()                  **/
/*************************************************************/
void TrashUnix(void)
{
  /* Remove sync timer */
  SetSyncTimer(0);
  /* Shut down audio */
  TrashAudio();
  /* Free output image buffer */
  FreeImage(&OutImg);
}

/** ShowVideo() **********************************************/
/** Show "active" image at the actual screen or window.     **/
/*************************************************************/
int ShowVideo(void)
{
  Image *Output;
  int SX,SY,SW,SH,J;

  /* Must have active video image, X11 display */
  if(!VideoImg||!VideoImg->Data) return(0);

  /* If no window yet... */

  /* Allocate image buffer if none */
  if(!OutImg.Data&&!NewImage(&OutImg,Effects&EFF_4X3? 4*YSize/3:XSize,YSize)) return(0);

  /* Wait for all X11 requests to complete, to avoid flicker */
  //XSync(Dsp,False);

  /* If not scaling or post-processing image, avoid extra work */
  if(!(Effects&(EFF_RASTER_ALL|EFF_MASK_ALL|EFF_SOFTEN_ALL|EFF_VIGNETTE|EFF_SCALE|EFF_4X3)))
  {
    //XPutImage(Dsp,Wnd,DefaultGCOfScreen(Scr),VideoImg->XImg,VideoX,VideoY,(XSize-VideoW)>>1,(YSize-VideoH)>>1,VideoW,VideoH);
    return(1);
  }

  /* By default, we will be showing OutImg */
  Output = &OutImg;
  SX     = 0;
  SY     = 0;
  SW     = OutImg.W;
  SH     = OutImg.H;

  /* Interpolate image if required */
  J = Effects&EFF_SOFTEN_ALL;
  if((J==EFF_2XSAI) || (J==EFF_HQ4X))
    SoftenImage(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  else if(J==EFF_EPX)
    SoftenEPX(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  else if(J==EFF_EAGLE)
    SoftenEAGLE(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  else if(J==EFF_SCALE2X)
    SoftenSCALE2X(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  else if(J || (J==EFF_NEAREST))
    ScaleImage(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  else if((OutImg.W==VideoW)&&(OutImg.H==VideoH))
  {
    if(Effects&(EFF_RASTER_ALL|EFF_MASK_ALL))
      IMGCopy(&OutImg,0,0,VideoImg,VideoX,VideoY,VideoW,VideoH,-1);
    else
    {
      /* Use VideoImg directly */
      Output = VideoImg;
      SX     = VideoX;
      SY     = VideoY;
    }
  }
  else if(Effects&(EFF_SCALE|EFF_4X3))
  {
    /* Scale VideoImg to OutImg */
    ScaleImage(&OutImg,VideoImg,VideoX,VideoY,VideoW,VideoH);
  }
  else if((OutImg.W<=VideoW)&&(OutImg.H<=VideoH))
  {
    if(Effects&(EFF_RASTER_ALL|EFF_MASK_ALL))
      IMGCopy(&OutImg,0,0,VideoImg,VideoX+((VideoW-OutImg.W)>>1),VideoY+((VideoH-OutImg.H)>>1),OutImg.W,OutImg.H,-1);
    else
    {
      /* Use VideoImg directly */
      Output = VideoImg;
      SX     = VideoX+((VideoW-OutImg.W)>>1);
      SY     = VideoY+((VideoH-OutImg.H)>>1);
    }
  }
  else
  {
    /* Use rectangle at the center of OutImg */
    SX = (OutImg.W-VideoW)>>1;
    SY = (OutImg.H-VideoH)>>1;
    SW = VideoW;
    SH = VideoH;
    /* Center VideoImg in OutImg */
    IMGCopy(&OutImg,SX,SY,VideoImg,VideoX,VideoY,VideoW,VideoH,-1);
  }

  /* Apply color mask to the pixels */
  switch(Effects&EFF_MASK_ALL)
  {
    case EFF_CMYMASK: CMYizeImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_RGBMASK: RGBizeImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_MONO:    MonoImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_GREEN:   GreenImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_AMBER:   AmberImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_SEPIA:   SepiaImage(&OutImg,SX,SY,SW,SH);break;
  }

  /* Apply scanlines or raster */
  switch(Effects&EFF_RASTER_ALL)
  {
    case EFF_TVLINES:  TelevizeImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_LCDLINES: LcdizeImage(&OutImg,SX,SY,SW,SH);break;
    case EFF_RASTER:   RasterizeImage(&OutImg,SX,SY,SW,SH);break;
  }

  /* Show framerate if requested */
  if((Effects&EFF_SHOWFPS)&&(FrameRate>0))
  {
    char S[8];
    sprintf(S,"%dfps",FrameRate);
    PrintXY(
      &OutImg,S,
      ((OutImg.W-VideoW)>>1)+8,((OutImg.H-VideoH)>>1)+8,
      FPS_COLOR,-1
    );
  }

  /* Wait for sync timer if requested */
  if(Effects&EFF_SYNC) WaitSyncTimer();

  /* Copy image to the window, either using SHM or not */
  //XPutImage(Dsp,Wnd,DefaultGCOfScreen(Scr),Output->XImg,SX,SY,0,0,OutImg.W,OutImg.H);

  /* Done */
  return(1);
}

/** GetJoystick() ********************************************/
/** Get the state of joypad buttons (1="pressed"). Refer to **/
/** the BTN_* #defines for the button mappings.             **/
/*************************************************************/
unsigned int GetJoystick(void)
{
  /* Count framerate */
  if((Effects&EFF_SHOWFPS)&&(++FrameCount>=300))
  {
    struct timeval NewTS;
    int Time;

    gettimeofday(&NewTS,0);
    Time       = (NewTS.tv_sec-TimeStamp.tv_sec)*1000
               + (NewTS.tv_usec-TimeStamp.tv_usec)/1000;
    FrameRate  = 1000*FrameCount/(Time>0? Time:1); 
    TimeStamp  = NewTS;
    FrameCount = 0;
    FrameRate  = FrameRate>999? 999:FrameRate;
  }

  /* Process any pending events */
  ProcessEvents(0);

  /* Return current joystick state */
  return(JoyState|KeyModes);
}

/** GetMouse() ***********************************************/
/** Get mouse position and button states in the following   **/
/** format: RMB.LMB.Y[29-16].X[15-0].                       **/
/*************************************************************/
unsigned int GetMouse(void)
{
  /* Return mouse position and buttons */
  return 0;
}

/** GetKey() *************************************************/
/** Get currently pressed key or 0 if none pressed. Returns **/
/** CON_* definitions for arrows and special keys.          **/
/*************************************************************/
unsigned int GetKey(void)
{
  unsigned int J;

  ProcessEvents(0);
  J=LastKey;
  LastKey=0;
  return(J);
}

/** WaitKey() ************************************************/
/** Wait for a key to be pressed. Returns CON_* definitions **/
/** for arrows and special keys.                            **/
/*************************************************************/
unsigned int WaitKey(void)
{
  unsigned int J;

  /* Swallow current keypress */
  GetKey();
  /* Wait in 100ms increments for a new keypress */
  while(!(J=GetKey())&&VideoImg) usleep(100000);
  /* Return key code */
  return(J);
}

/** WaitKeyOrMouse() *****************************************/
/** Wait for a key or a mouse button to be pressed. Returns **/
/** the same result as GetMouse(). If no mouse buttons      **/
/** reported to be pressed, do GetKey() to fetch a key.     **/
/*************************************************************/
unsigned int WaitKeyOrMouse(void)
{
  unsigned int J;

  /* Swallow current keypress */
  GetKey();
  /* Make sure mouse keys are not pressed */
  while(GetMouse()&MSE_BUTTONS) usleep(100000);
  /* Wait in 100ms increments for a key or mouse click */
  while(!(J=GetKey())&&!(GetMouse()&MSE_BUTTONS)&&VideoImg) usleep(100000);
  /* Place key back into the buffer and return mouse state */
  LastKey=J;
  return(GetMouse());
}

/** WaitSyncTimer() ******************************************/
/** Wait for the timer to become ready. Returns number of   **/
/** times timer has been triggered after the last call to   **/
/** WaitSyncTimer().                                        **/
/*************************************************************/
int WaitSyncTimer(void)
{
  int J;

  /* Wait in 1ms increments until timer becomes ready */
  while(!TimerReady&&TimerON&&VideoImg)
  {
    delay(1);
  }
  /* Warn of missed timer events */
  if((TimerReady>1)&&(Effects&EFF_VERBOSE))
    printf("WaitSyncTimer(): Missed %d timer events.\n",TimerReady-1);
  /* Reset timer */
  J=TimerReady;
  TimerReady=0;
  return(J);
}

int SyncTimerReady(void)
{
  /* Return whether timer is ready or not */
  return(TimerReady||!TimerON||!VideoImg);
}

int SetSyncTimer(int Hz)
{
  return(1);
}

int ChangeDir(const char *Name)
{
  return(chdir(Name));
}

void MicroSleep(unsigned int uS)
{
  //usleep(uS);
}

unsigned int X11GetColor(unsigned char R,unsigned char G,unsigned char B)
{
  int J;

  /* If using constant BPP, just return a pixel */
  //if(!Dsp||!(Effects&EFF_VARBPP)) return(PIXEL(R,G,B));

  /* If variable BPP, compute pixel based on the screen depth */
  J=16;
  return(
    J<=8?  (((7*(R)/255)<<5)|((7*(G)/255)<<2)|(3*(B)/255))
  : J<=16? (((31*(R)/255)<<11)|((63*(G)/255)<<5)|(31*(B)/255))
  : J<=32? (((R)<<16)|((G)<<8)|B)
  : 0
  );
}

static char current_path[1024];



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

void SetEffects(unsigned int NewEffects)
{
  /* Set new effects */
  Effects=NewEffects;
}

int ProcessEvents(int Wait)
{
  return 0;
}


