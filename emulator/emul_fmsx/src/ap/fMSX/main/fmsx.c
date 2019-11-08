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


const char *dir_program = FMSX_ROOT_GAMESDIR;


extern int StartMSX(int NewMode,int NewRAMPages,int NewVRAMPages);

void fmsxMain(void)
{
  /*
  fmsxGUI_initMenu();


  StartMSX(0, 0, 0);

  fmsxGUI_showMenu();
  */



 uint8_t initOkay;
 uint8_t failure = 0;



  // Display


 /// keyboard


 UPeriod = 50;

 ProgDir = dir_program;

 f_chdir(ProgDir);


 initOkay = InitMachine();

 _printHeapInfo();

 /*
 if (initOkay)
 {
      if (! dirExist(FMSX_ROOT_GAMESDIR)) mkdir(FMSX_ROOT_GAMESDIR, 666);
      if (! dirExist(FMSX_ROOT_GAMESDIR"/bios")) mkdir(FMSX_ROOT_GAMESDIR"/bios", 666);
      if (! dirExist(FMSX_ROOT_DATADIR)) mkdir(FMSX_ROOT_DATADIR, 666);
 }
  */

 if (initOkay)
 {
    Mode=(Mode&~MSX_VIDEO)|MSX_PAL;
    Mode=(Mode&~MSX_MODEL)|MSX_MSX2;
    Mode=(Mode&~MSX_JOYSTICKS)|MSX_JOY1|MSX_JOY2;

    RAMPages = 2;
    VRAMPages = 2;

    if (!StartMSX(Mode,RAMPages,VRAMPages))
    {
#if 0
      switch (failure)
      {
         case 1:
              //odroidFmsxGUI_msgBox("Error","Cannot mount SDCard", 1);
              break;
         default:
              //odroidFmsxGUI_msgBox("Error","Start fMSX failed.\nMissing bios files?", 1);
      }
#endif
    }

    //odroidFmsxGUI_msgBox("End","The emulation was shutted down\nYou can turn off your device", 1);
  }

}
