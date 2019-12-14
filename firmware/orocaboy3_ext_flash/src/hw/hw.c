/*
 * hw.c
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "hw.h"





void hwInit(void)
{
  bspInit();



  microsInit();
  millisInit();
  delayInit();
  cmdifInit();
  swtimerInit();

  uartInit();
  uartOpen(_DEF_UART1, 57600);

  logPrintf("\n\n[ ExtFlash Begin... ]\r\n");

  ledInit();

  sdramInit();

  qspiInit();
  flashInit();
  buttonInit();

  logPrintf("Start...\r\n");
}

