/*
 * joypad.c
 *
 *  Created on: 2019. 11. 14.
 *      Author: Baram
 */




#include "joypad.h"



#include "adc.h"


#define JOYPAD_ADC_MAX_COUNT     5
#define JOYPAD_DEAD_ZONE         50


static uint8_t  adc_ch_x = 0;
static uint8_t  adc_ch_y = 1;

static int32_t x_adc_value = 0;
static int32_t y_adc_value = 0;

static int32_t x_adc_offset = 0;
static int32_t y_adc_offset = 0;


static int32_t x_value = 0;
static int32_t y_value = 0;

static int32_t adc_data_x[JOYPAD_ADC_MAX_COUNT];
static int32_t adc_data_y[JOYPAD_ADC_MAX_COUNT];


bool joypadInit(void)
{
  uint32_t i;

  for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
  {
    adc_data_x[i] = 0;
    adc_data_y[i] = 0;
  }

  return true;
}

void joypadUpdate(void)
{
  static uint32_t index = 0;
  static uint32_t state = 0;
  uint32_t i;
  int32_t sum;
  int32_t value;



  adc_data_x[index] = adcRead(adc_ch_x);
  adc_data_y[index] = adcRead(adc_ch_y);

  if (state == 0 && index == (JOYPAD_ADC_MAX_COUNT - 1))
  {
    state = 1;
  }

  index = (index + 1) % JOYPAD_ADC_MAX_COUNT;


  if (state > 0)
  {
    sum = 0;
    for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
    {
      sum += adc_data_x[i];
    }
    x_adc_value = sum / JOYPAD_ADC_MAX_COUNT;

    sum = 0;
    for (i=0; i<JOYPAD_ADC_MAX_COUNT; i++)
    {
      sum += adc_data_y[i];
    }
    y_adc_value = sum / JOYPAD_ADC_MAX_COUNT;


    if (state == 1)
    {
      x_adc_offset = x_adc_value;
      y_adc_offset = y_adc_value;
    }


    value = constrain(x_adc_value-x_adc_offset, -2000, 2000);
    if (value >  JOYPAD_DEAD_ZONE)      value -= JOYPAD_DEAD_ZONE;
    else if (value < -JOYPAD_DEAD_ZONE) value += JOYPAD_DEAD_ZONE;
    else                                value  = 0;
    x_value = map(value, -2000, 2000, -100, 100);

    value = constrain(y_adc_value-y_adc_offset, -2000, 2000);
    if (value >  JOYPAD_DEAD_ZONE)      value -= JOYPAD_DEAD_ZONE;
    else if (value < -JOYPAD_DEAD_ZONE) value += JOYPAD_DEAD_ZONE;
    else                                value  = 0;
    y_value = map(value, -2000, 2000 , -100, 100);

    state = 2;
  }
}

int32_t joypadGetX(void)
{
  return x_value;
}

int32_t joypadGetY(void)
{
  return y_value;
}

int32_t joypadGetKey(void)
{
  return 0;
}
