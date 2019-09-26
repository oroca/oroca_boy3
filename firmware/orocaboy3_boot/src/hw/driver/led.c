/*
 * led.c
 *
 *  Created on: 2019. 3. 7.
 *      Author: HanCheol Cho
 */



#include "led.h"
#include "pwm.h"





bool ledInit(void)
{
  if (pwmIsInit() == false)
  {
    pwmInit();
  }

  pwmWrite(0, 0);

  return true;
}

void ledOn(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    pwmWrite(0, 255);
  }
}

void ledOff(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    pwmWrite(0, 0);
  }
}

void ledToggle(uint8_t ch)
{
  if (ch < LED_MAX_CH)
  {
    if (pwmRead(0) > 0)
    {
      pwmWrite(0, 0);
    }
    else
    {
      pwmWrite(0, 255);
    }
  }
}

void ledPwm(uint8_t ch, uint8_t pwm)
{
  if (ch < LED_MAX_CH)
  {
    pwm = constrain(pwm, 0, 100);
    pwmWrite(0, map(pwm, 0, 100, 0, 255));
  }
}
