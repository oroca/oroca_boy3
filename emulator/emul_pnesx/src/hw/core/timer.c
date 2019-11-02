/*
 * timer.c
 *
 *  Created on: 2019. 11. 3.
 *      Author: Baram
 */




#include "timer.h"
#include "swtimer.h"


typedef struct
{
  TIM_HandleTypeDef hTIM;
  uint8_t  enable;
  uint32_t freq;
  uint32_t prescaler_value;
  uint32_t prescaler_value_high_res;
  uint32_t prescaler_div;
  uint32_t period;
  voidFuncPtr handler;
} drv_timer_t;



//-- Internal Variables
static drv_timer_t timer_tbl[TIMER_MAX_CH];



bool timerInit(void)
{
  uint8_t tim_ch;
  uint8_t i;


  //-- TIM4
  //
  tim_ch = _DEF_TIMER1;
  timer_tbl[tim_ch].hTIM.Instance               = TIM4;
  timer_tbl[tim_ch].prescaler_value             = (uint32_t)((SystemCoreClock / 2) / 10000  ) - 1; // 0.01Mhz
  timer_tbl[tim_ch].prescaler_value_high_res    = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1; // 1.00Mhz
  timer_tbl[tim_ch].prescaler_div               = 100;
  timer_tbl[tim_ch].hTIM.Init.Period            = 10000 - 1;
  timer_tbl[tim_ch].hTIM.Init.Prescaler         = timer_tbl[tim_ch].prescaler_value;
  timer_tbl[tim_ch].hTIM.Init.ClockDivision     = 0;
  timer_tbl[tim_ch].hTIM.Init.CounterMode       = TIM_COUNTERMODE_UP;
  timer_tbl[tim_ch].hTIM.Init.RepetitionCounter = 0;


  for( i=0; i<TIMER_MAX_CH; i++ )
  {
    timer_tbl[i].handler = NULL;
    timer_tbl[i].enable  = 0;
  }

  return true;
}

void timerStop(uint8_t channel)
{
  if( channel >= TIMER_MAX_CH ) return;

  if (timer_tbl[channel].enable == 0)
  {
    return;
  }
  timer_tbl[channel].enable = 0;
  HAL_TIM_Base_DeInit(&timer_tbl[channel].hTIM);
}

void timerSetPeriod(uint8_t channel, uint32_t period_data)
{
  if( channel >= TIMER_MAX_CH ) return;

  if( period_data > 0xFFFF )
  {
    timer_tbl[channel].hTIM.Init.Prescaler = timer_tbl[channel].prescaler_value;
    timer_tbl[channel].hTIM.Init.Period    = (period_data/timer_tbl[channel].prescaler_div) - 1;
  }
  else
  {
    if( period_data > 0 )
    {
      timer_tbl[channel].hTIM.Init.Prescaler = timer_tbl[channel].prescaler_value_high_res;
      timer_tbl[channel].hTIM.Init.Period    = period_data - 1;
    }
  }
}

void timerAddPeriod(uint8_t channel, int32_t period_data)
{
  int32_t arr;

  arr  = timer_tbl[channel].hTIM.Instance->ARR;
  arr += period_data;

  if (timer_tbl[channel].hTIM.Instance->CNT >= arr)
  {
    timer_tbl[channel].hTIM.Instance->CNT = 0;
  }
  if (arr > 0)
  {
    timer_tbl[channel].hTIM.Instance->ARR = arr;
  }
}

void timerAttachInterrupt(uint8_t channel, voidFuncPtr handler)
{
  if( channel >= TIMER_MAX_CH ) return;

  timerStop(channel);

  timer_tbl[channel].handler = handler;
}


void timerDetachInterrupt(uint8_t channel)
{
  if( channel >= TIMER_MAX_CH ) return;


  timer_tbl[channel].handler = NULL;
}

void timerStart(uint8_t channel)
{
  if( channel >= TIMER_MAX_CH ) return;

  HAL_TIM_Base_Init(&timer_tbl[channel].hTIM);
  HAL_TIM_Base_Start_IT(&timer_tbl[channel].hTIM);

  timer_tbl[channel].enable = 1;
}









void timerCallback(TIM_HandleTypeDef *htim)
{
  uint8_t i;


  for( i=0; i<TIMER_MAX_CH; i++ )
  {
    if( htim->Instance == timer_tbl[i].hTIM.Instance )
    {
      if( timer_tbl[i].handler != NULL )
      {
        (*timer_tbl[i].handler)();
      }
    }
  }
}


void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&timer_tbl[_DEF_TIMER1].hTIM);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM4 )                  // _DEF_TIMER1
  {
    __HAL_RCC_TIM4_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM4_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM4 )
  {
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
    swtimerISR();
  }
  else
  {
    timerCallback(htim);
  }
}
