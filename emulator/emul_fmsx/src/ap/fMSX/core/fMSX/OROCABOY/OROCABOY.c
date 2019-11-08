/** fMSX: portable MSX emulator ******************************/
/**                                                         **/
/**                         Unix.c                          **/
/**                                                         **/
/** This file contains Unix-dependent subroutines and       **/
/** drivers. It includes screen drivers via CommonMux.h.    **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2018                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
#include "MSX.h"
#include "Console.h"
#include "EMULib.h"
#include "NetPlay.h"
#include "Sound.h"
#include "Record.h"

#include <string.h>
#include <stdio.h>

#define WIDTH       272                   /* Buffer width    */
#define HEIGHT      228                   /* Buffer height   */


#define BOY_COLOR(r, g, b)\
  (((uint16_t)b) >> 3) |\
         ((((uint16_t)g) << 3) & 0x07E0) |\
         ((((uint16_t)r) << 8) & 0xf800)


/* Press/Release keys in the background KeyState */
#define XKBD_SET(K) XKeyState[Keys[K][0]]&=~Keys[K][1]
#define XKBD_RES(K) XKeyState[Keys[K][0]]|=Keys[K][1]

/* Combination of EFF_* bits */
int UseEffects  = EFF_SCALE|EFF_SAVECPU|EFF_SYNC;

int InMenu;                /* 1: In MenuMSX(), ignore keys   */
int UseZoom     = 1;       /* Zoom factor (1=no zoom)        */
int UseSound    = 22050;   /* Audio sampling frequency (Hz)  */
int SyncFreq    = 60;      /* Sync frequency (0=sync off)    */
int FastForward;           /* Fast-forwarded UPeriod backup  */
int SndSwitch;             /* Mask of enabled sound channels */
int SndVolume;             /* Master volume for audio        */
int OldScrMode;            /* fMSX "ScrMode" variable storage*/

const char *Title     = "fMSX 5.4";       /* Program version */

Image NormScreen;          /* Main screen image              */
Image WideScreen;          /* Wide screen image              */
static pixel *WBuf;        /* From Wide.h                    */
static pixel *XBuf;        /* From Common.h                  */
static unsigned int XPal[80];
static unsigned int BPal[256];
static unsigned int XPal0;

const char *Disks[2][MAXDISKS+1];         /* Disk names      */
volatile byte XKeyState[20]; /* Temporary KeyState array     */

void HandleKeys(unsigned int Key);
void PutImage(void);

/** CommonMux.h **********************************************/
/** Display drivers for all possible screen depths.         **/
/*************************************************************/
#include "CommonMux.h"

/** InitMachine() ********************************************/
/** Allocate resources needed by machine-dependent code.    **/
/*************************************************************/
int InitMachine(void)
{
  int J;

  /* Initialize variables */
  UseZoom         = UseZoom<1? 1:UseZoom>5? 5:UseZoom;
  InMenu          = 0;
  FastForward     = 0;
  OldScrMode      = 0;
  NormScreen.Data = 0;
  WideScreen.Data = 0;



  /* Initialize system resources */
  InitUnix(Title,UseZoom*WIDTH,UseZoom*HEIGHT);


  /* Set visual effects */
  SetEffects(UseEffects);

  /* Create main image buffer */
  if(!NewImage(&NormScreen,WIDTH,HEIGHT)) { TrashUnix();return(0); }
  XBuf = NormScreen.Data;

#ifndef NARROW
  /* Create wide image buffer */
  if(!NewImage(&WideScreen,WIDTH*2,HEIGHT)) { TrashUnix();return(0); }
  WBuf = WideScreen.Data;
#endif

  /* Set correct screen drivers */
  if(!SetScreenDepth(NormScreen.D)) { TrashUnix();return(0); }

  /* Initialize video to main image */
  SetVideo(&NormScreen,0,0,WIDTH,HEIGHT);

  /* Set all colors to black */
  for(J=0;J<80;J++) SetColor(J,0,0,0);

  /* Create SCREEN8 palette (GGGRRRBB) */
  for(J=0;J<256;J++)
  {
    //BPal[J]=X11GetColor(((J>>2)&0x07)*255/7,((J>>5)&0x07)*255/7,(J&0x03)*255/3);
    uint16_t r = (0xFF / 7) * (((J&0x1C)>> 2));
    uint16_t g = (0xFF / 7) * (((J&0xE0)>> 5));
    uint16_t b = J & 0x03;
    if (b == 1) b = 73;
    if (b == 2) b = 146;
    if (b == 3) b = 255;

    BPal[J]= BOY_COLOR(r, g, b);
  }

  /* Initialize temporary keyboard array */
  memset((void *)XKeyState,0xFF,sizeof(XKeyState));

  /* Attach keyboard handler */
  SetKeyHandler(HandleKeys);

  /* Initialize sound */
  InitSound(UseSound,150);
  SndSwitch=(1<<MAXCHANNELS)-1;
  SndVolume=64;
  SetChannels(SndVolume,SndSwitch);

  /* Initialize sync timer if needed */
  if((SyncFreq>0)&&!SetSyncTimer(SyncFreq*UPeriod/100)) SyncFreq=0;

  /* Initialize record/replay */
  RPLInit(SaveState,LoadState,MAX_STASIZE);
  RPLRecord(RPL_RESET);

  /* Done */
  return(1);
}

/** TrashMachine() *******************************************/
/** Deallocate all resources taken by InitMachine().        **/
/*************************************************************/
void TrashMachine(void)
{
  /* Flush and free recording buffers */
  RPLTrash();

#ifndef NARROW
  FreeImage(&WideScreen);
#endif
  FreeImage(&NormScreen);
  TrashSound();
  TrashUnix();
}

/** PutImage() ***********************************************/
/** Put an image on the screen.                             **/
/*************************************************************/
void PutImage(void)
{
#ifndef NARROW
  /* If screen mode changed... */
  if(ScrMode!=OldScrMode)
  {
    /* Switch to the new screen mode */
    OldScrMode=ScrMode;
    /* Depending on the new screen width... */
    if((ScrMode==6)||((ScrMode==7)&&!ModeYJK)||(ScrMode==MAXSCREEN+1))
      SetVideo(&WideScreen,0,0,WIDTH*2,HEIGHT);
    else
      SetVideo(&NormScreen,0,0,WIDTH,HEIGHT);
  }
#endif

  /* Show replay icon */
  if(RPLPlay(RPL_QUERY)) RPLShow(VideoImg,VideoX+10,VideoY+10);

  /* Show display buffer */
  ShowVideo();
}

/** PlayAllSound() *******************************************/
/** Render and play given number of microseconds of sound.  **/
/*************************************************************/
void PlayAllSound(int uSec)
{
  /* @@@ Twice the argument to avoid skipping */
  RenderAndPlayAudio(2*uSec*UseSound/1000000);
}

/** Joystick() ***********************************************/
/** Query positions of two joystick connected to ports 0/1. **/
/** Returns 0.0.B2.A2.R2.L2.D2.U2.0.0.B1.A1.R1.L1.D1.U1.    **/
/*************************************************************/
unsigned int Joystick(void)
{
  byte RemoteKeyState[20];
  unsigned int J,I;

  /* Get joystick state */
  J = GetJoystick();

  /* Make SHIFT/CONTROL act as fire buttons */
  if(J&BTN_SHIFT)   J|=(J&BTN_ALT? (BTN_FIREB<<16):BTN_FIREB);
  if(J&BTN_CONTROL) J|=(J&BTN_ALT? (BTN_FIREA<<16):BTN_FIREA);

  /* Store joystick state for NetPlay transmission */
  *(unsigned int *)&XKeyState[sizeof(XKeyState)-sizeof(int)]=J;

  /* If failed exchanging KeyStates with remote... */
  if(!NETExchange((void *)RemoteKeyState,(const void *)XKeyState,sizeof(XKeyState)))
    /* Copy temporary keyboard map into permanent one */
    memcpy((void *)KeyState,(const void *)XKeyState,sizeof(KeyState));
  else
  {
    /* Merge local and remote KeyStates */
    for(I=0;I<sizeof(KeyState);++I) KeyState[I]=XKeyState[I]&RemoteKeyState[I];
    /* Merge joysticks, server is player #1, client is player #2 */
    I = *(unsigned int *)&RemoteKeyState[sizeof(XKeyState)-sizeof(int)];
    J = NETConnected()==NET_SERVER?
        ((J&(BTN_ALL|BTN_MODES))|((I&BTN_ALL)<<16))
      : ((I&(BTN_ALL|BTN_MODES))|((J&BTN_ALL)<<16));
  }

  /* Run replay user interface */
  RPLControls(J);

  /* Replay recorded joystick and keyboard states */
  I = RPLPlayKeys(RPL_NEXT,(byte *)KeyState,sizeof(KeyState));
  I = I!=RPL_ENDED? I:0;

  /* Parse joystick */
  if(J&BTN_LEFT)                    I|=JST_LEFT;
  if(J&BTN_RIGHT)                   I|=JST_RIGHT;
  if(J&BTN_UP)                      I|=JST_UP;
  if(J&BTN_DOWN)                    I|=JST_DOWN;
  if(J&(BTN_FIREA|BTN_FIRER))       I|=JST_FIREA;
  if(J&(BTN_FIREB|BTN_FIREL))       I|=JST_FIREB;
  if(J&(BTN_LEFT<<16))              I|=JST_LEFT<<8;
  if(J&(BTN_RIGHT<<16))             I|=JST_RIGHT<<8;
  if(J&(BTN_UP<<16))                I|=JST_UP<<8;
  if(J&(BTN_DOWN<<16))              I|=JST_DOWN<<8;
  if(J&((BTN_FIREA|BTN_FIRER)<<16)) I|=JST_FIREA<<8;
  if(J&((BTN_FIREB|BTN_FIREL)<<16)) I|=JST_FIREB<<8;

  /* Record joystick and keyboard states */
  RPLRecordKeys(I,(byte *)KeyState,sizeof(KeyState));

  /* Done */
  return(I);
}

/** Keyboard() ***********************************************/
/** Modify keyboard matrix.                                 **/
/*************************************************************/
void Keyboard(void)
{
  /* Everything is done in Joystick() */
}

/** Mouse() **************************************************/
/** Query coordinates of a mouse connected to port N.       **/
/** Returns F2.F1.Y.Y.Y.Y.Y.Y.Y.Y.X.X.X.X.X.X.X.X.          **/
/*************************************************************/
unsigned int Mouse(byte N)
{
  unsigned int J;
  int X,Y;

  J = GetMouse();
  X = ScanLines212? 212:192;
  Y = ((J&MSE_YPOS)>>16)-(HEIGHT-X)/2-VAdjust;
  Y = Y<0? 0:Y>=X? X-1:Y;
  X = J&MSE_XPOS;
  X = (ScrMode==6)||((ScrMode==7)&&!ModeYJK)||(ScrMode==MAXSCREEN+1)? (X>>1):X;
  X = X-(WIDTH-256)/2-HAdjust;
  X = X<0? 0:X>=256? 255:X;

  return(((J&MSE_BUTTONS)>>14)|X|(Y<<8));
}

/** SetColor() ***********************************************/
/** Set color N to (R,G,B).                                 **/
/*************************************************************/
void SetColor(byte N,byte R,byte G,byte B)
{
  if(N) XPal[N]=X11GetColor(R,G,B); else XPal0=X11GetColor(R,G,B);
}

/** HandleKeys() *********************************************/
/** Keyboard handler.                                       **/
/*************************************************************/
void HandleKeys(unsigned int Key)
{
}
