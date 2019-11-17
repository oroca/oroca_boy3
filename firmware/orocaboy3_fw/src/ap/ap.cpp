/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"



void apInit(void)
{
  uartOpen(_DEF_UART1, 57600);
  uartOpen(_DEF_UART2, 57600);
  uartOpen(_DEF_UART3, 115200);
  uartOpen(_DEF_UART4, 115200);
  cmdifOpen(_DEF_UART1, 57600);
}

void apMain(void)
{
  uint32_t pre_time;


  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
      //logPrintf("bat %d %d\n", batteryGetLevel(), batteryGetVoltage());
      //logPrintf("joyt %d %d\n", joypadGetX(), joypadGetY());
    }

    esp32Update();
    batteryUpdate();
    joypadUpdate();
    osdUpdate();
  }
}
