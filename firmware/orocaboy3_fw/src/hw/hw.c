/*
 * hw.c
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "hw.h"




/*
typedef struct
{
  uint8_t  version_str[32];
  uint8_t  board_str[32];
  uint8_t  reserved_str[32];

  uint32_t magic_number;
} flash_ver_t;
*/

__attribute__((section(".version"))) flash_ver_t fw_ver =
    {"V191017R1",
     "OROCABOY3",
     "Firmware",
     0x5555AAAA,          // magic_number
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
  logPrintf("Booting..Name \t\t: %s\r\n", fw_ver.board_str);
  logPrintf("Booting..Ver  \t\t: %s\r\n", fw_ver.version_str);

  resetLog();
  rtcInit();
  pwmInit();
  ledInit();


  sdramInit();
  qspiInit();
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


  logPrintf("Start...\r\n");
}

