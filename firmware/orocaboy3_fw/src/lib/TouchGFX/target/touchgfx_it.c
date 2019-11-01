
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_dma2d.h"
#include "stm32h7xx_hal_ltdc.h"
#include "hw.h"

#include <stdbool.h>

#include <cmsis_os.h>

extern DMA2D_HandleTypeDef hdma2d;
extern LTDC_HandleTypeDef hltdc;

static volatile int overrunCnt;

extern struct HwJPEG_Context_s HwJPEG_Context;




void DMA2D_IRQHandler(void)
{
    HAL_DMA2D_IRQHandler(&hdma2d);
}

void LTDC_IRQHandler(void)
{
    HAL_LTDC_IRQHandler(&hltdc);
}

void LTDC_ER_IRQHandler(void)
{
  if (LTDC->ISR & 2)
  {
      LTDC->ICR = 2;
      overrunCnt++;
  }
}
