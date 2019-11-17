/*
 * lcd.c
 *
 *  Created on: 2019. 11. 16.
 *      Author: Baram
 */




#include "lcd.h"
#include "pwm.h"
#include "eeprom.h"


static uint8_t backlight_value = 100;




bool lcdInit(void)
{
  uint8_t eep_data[2];


  eep_data[0] = eepromReadByte(_EEP_ADDR_BRIGHT+0);
  eep_data[1] = eepromReadByte(_EEP_ADDR_BRIGHT+1);

  if ((uint8_t)eep_data[0] == (uint8_t)(~eep_data[1]))
  {
    backlight_value = eep_data[0];
  }
  else
  {
    backlight_value = 100;
    eepromWriteByte(_EEP_ADDR_BRIGHT+0, backlight_value);
    eepromWriteByte(_EEP_ADDR_BRIGHT+1, ~backlight_value);
  }


  lcdSetBackLight(backlight_value);

  return true;
}

uint8_t lcdGetBackLight(void)
{
  return backlight_value;
}

void lcdSetBackLight(uint8_t value)
{
  pwmWrite(0, map(value, 0, 100, 255, 0));

  if (value != backlight_value)
  {
    eepromWriteByte(_EEP_ADDR_BRIGHT+0, value);
    eepromWriteByte(_EEP_ADDR_BRIGHT+1, ~value);
  }
}
