/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"

extern "C"
{
void D_DoomMain (void);
}

static void threadEmul(void const *argument);


void apInit(void)
{
  uartOpen(_DEF_UART1, 57600);
  uartOpen(_DEF_UART2, 57600);
  cmdifOpen(_DEF_UART1, 57600);


  osThreadDef(threadEmul, threadEmul, _HW_DEF_RTOS_THREAD_PRI_EMUL, 0, _HW_DEF_RTOS_THREAD_MEM_EMUL);
  if (osThreadCreate(osThread(threadEmul), NULL) != NULL)
  {
    logPrintf("threadEmul \t\t: OK\r\n");
  }
  else
  {
    logPrintf("threadEmul \t\t: Fail\r\n");
    while(1);
  }

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
    }
    osThreadYield();
  }
}



static void threadEmul(void const *argument)
{
  UNUSED(argument);


  D_DoomMain();


  while(1)
  {
    delay(100);
  }
}
