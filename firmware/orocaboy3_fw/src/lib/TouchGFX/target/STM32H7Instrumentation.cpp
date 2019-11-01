
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"

#include <touchgfx/hal/HAL.hpp>
#include <STM32H7Instrumentation.hpp>
#include "bsp.h"


namespace touchgfx
{
static TIM_HandleTypeDef tim;

void STM32H7Instrumentation::init()
{
  __HAL_RCC_TIM5_CLK_ENABLE();

  tim.Instance = TIM5;
  tim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim.Init.Period = 0xFFFFFFFF;
  tim.Init.Prescaler = 0;
  tim.Init.RepetitionCounter = 1;


  HAL_TIM_Base_Init(&tim);
  HAL_TIM_Base_Start(&tim);
}

unsigned int STM32H7Instrumentation::getElapsedUS(unsigned int start, unsigned int now, unsigned int clockfrequency)
{
  unsigned int ret;

  ret = ((now - start) + (clockfrequency / 2)) / clockfrequency;

  return ret;
}

unsigned int STM32H7Instrumentation::getCPUCycles()
{
  unsigned int ret;

  ret = __HAL_TIM_GET_COUNTER(&tim);

  return ret;
}

void STM32H7Instrumentation::setMCUActive(bool active)
{
  if (active) //idle task sched out
  {
      cc_consumed += getCPUCycles() - cc_in;
  }
  else //idle task sched in
  {
      cc_in = getCPUCycles();
  }
}

}
