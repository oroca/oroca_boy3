/*
 * fmsx_main.c
 *
 *  Created on: 2019. 11. 6.
 *      Author: HanCheol Cho
 */


#include "hw.h"
#include "Console.h"
#include "EMULib.h"
#include "MSX.h"
#include "fmsx.h"


const char *dir_bios = "/msx/bios";
const char *dir_roms = "/msx/roms";


static char cur_path[1024];



extern int StartMSX(int NewMode,int NewRAMPages,int NewVRAMPages);

void fmsxMain(void)
{
  ProgDir = dir_bios;


  f_chdir(ProgDir);


  UPeriod = 50;


  if (InitMachine())
  {
    Mode = (Mode&~MSX_VIDEO)|MSX_PAL;
    Mode = (Mode&~MSX_MODEL)|MSX_MSX2;
    Mode = (Mode&~MSX_JOYSTICKS)|MSX_JOY1|MSX_JOY2;


    RAMPages  = 2;
    VRAMPages = 2;

    if (!StartMSX(Mode,RAMPages,VRAMPages))
    {
      logPrintf("Start fMSX failed.\nMissing bios files?\n");
    }

    logPrintf("The emulation was shutted down\nYou can turn off your device\n");
  }
}


void fmsxChangeHome(void)
{
  f_chdir(dir_roms);
}





char *getcwd(char *__buf, size_t __size )
{
  char *p_buf;

  if (__buf != NULL)
  {
    p_buf = __buf;
  }
  else
  {
    p_buf = cur_path;
  }

  if (f_getcwd(p_buf, __size) == FR_OK)
  {
    printf("getcwd %s\n", p_buf);
  }
  else
  {
    printf("getcwd fail\n");
  }

  return p_buf;
}



