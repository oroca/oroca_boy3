/*
 * hw.c
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "hw.h"



extern flash_tag_t fw_tag;



void bootCmdif(void);




void hwInit(void)
{
  bspInit();

  resetInit();
  microsInit();
  millisInit();
  delayInit();
  cmdifInit();
  swtimerInit();

  uartInit();
  uartOpen(_DEF_UART1, 57600);

  logPrintf("\n\n[ Firmware Begin... ]\r\n");
  logPrintf("Booting..Board\t\t: %s\r\n", fw_tag.board_str);
  logPrintf("Booting..Name \t\t: %s\r\n", fw_tag.name_str);
  logPrintf("Booting..Ver  \t\t: %s\r\n", fw_tag.version_str);


  rtcInit();
  logPrintf("ResetBits \t\t: 0x%X\n", (int)rtcReadBackupData(_HW_DEF_RTC_RESET_SRC));

  resetLog();

  pwmInit();
  ledInit();
  gpioInit();
  adcInit();

  //sdramInit();
  //qspiInit();
  //qspiEnableMemoryMappedMode();

  flashInit();
  buttonInit();
  i2cInit();
  eepromInit();
  if (eepromValid(0) == true)
  {
    logPrintf("eeprom %dKB \t\t: OK\r\n", (int)eepromGetLength()/1024);
  }
  else
  {
    logPrintf("eeprom %dKB \t\t: Fail\r\n", (int)eepromGetLength()/1024);
  }

  usbInit();
  vcpInit();
  ltdcInit();
  lcdInit();
  dacInit();
  timerInit();
  speakerInit();
  batteryInit();
  joypadInit();
  osdInit();

  if (sdInit() == true)
  {
    fatfsInit();
  }

  logPrintf("volume    \t\t: %d\n", speakerGetVolume());
  logPrintf("bright    \t\t: %d\n", lcdGetBackLight());

  logPrintf("Start...\r\n");

  cmdifAdd("boot", bootCmdif);


  lcdDisplayOn();
}

void hwJumpToBoot(void)
{
  rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, (1<<7));
  resetRunSoftReset();
}


void hwJumpToFw(void)
{
  rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, (0<<7));
  resetRunSoftReset();
}


void bootCmdif(void)
{

  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("0x5555AAAA", 0) == true)
    {
      cmdifPrintf( "jump to boot\n");
      delay(100);
      hwJumpToBoot();
    }
    else if(cmdifHasString("reset", 0) == true)
    {
      cmdifPrintf( "reset\n");
      delay(100);
      rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, 0);
      resetRunSoftReset();
    }
    else
    {
      cmdifPrintf( "boot 0x5555AAAA\n");
      cmdifPrintf( "boot reset\n");
    }
  }
}
