/*
 * hw.c
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "hw.h"
#include "util.h"


void bootCmdif(void);


extern uint32_t _flash_tag_addr;
extern uint32_t _flash_fw_addr;



__attribute__((section(".tag"))) flash_tag_t fw_tag =
    {
     // fw info
     //
     0xAAAA5555,        // magic_number
     "V191201R1",       // version_str
     "OROCABOY3",       // board_str
     "Launcher",        // name
     __DATE__,
     __TIME__,
     (uint32_t)&_flash_tag_addr,
     (uint32_t)&_flash_fw_addr,


     // tag info
     //
    };




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


  sdramInit();
  qspiInit();
  qspiEnableMemoryMappedMode();

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
  ltdcInit();
  slotInit();


  if (sdInit() == true)
  {
    fatfsInit();
  }

  if (buttonGetPressed(_DEF_HW_BTN_SELECT) == true)
  {
    logPrintf("usb mode   \t\t: USB_MSC\r\n");
    usbBegin(USB_MSC_MODE);
  }
  else
  {
    logPrintf("usb mode   \t\t: USB_CDC\r\n");
    usbBegin(USB_CDC_MODE);
    vcpInit();
  }

  //dacInit();
  speakerInit();
  lcdInit();

  esp32Init();
  batteryInit();
  joypadInit();
  osdInit();









  logPrintf("volume    \t\t: %d\n", speakerGetVolume());
  logPrintf("bright    \t\t: %d\n", lcdGetBackLight());



  logPrintf("Start...\r\n");

  cmdifAdd("boot", bootCmdif);
}

void hwJumpToBoot(void)
{
  rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, (1<<7));
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

    if(cmdifHasString("reset", 0) == true)
    {
      cmdifPrintf( "reset\n");
      delay(100);
      rtcWriteBackupData(_HW_DEF_RTC_BOOT_MODE, 0);
      resetRunSoftReset();
    }

  }
}
