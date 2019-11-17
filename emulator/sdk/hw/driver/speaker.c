/*
 * speaker.c
 *
 *  Created on: 2019. 11. 3.
 *      Author: Baram
 */




#include "speaker.h"
#include "dac.h"
#include "gpio.h"
#include "eeprom.h"


static uint8_t volume = 0;


bool speakerInit(void)
{
  uint8_t eep_data[2];

  speakerDisable();

  eep_data[0] = eepromReadByte(_EEP_ADDR_VOLUME+0);
  eep_data[1] = eepromReadByte(_EEP_ADDR_VOLUME+1);

  if ((uint8_t)eep_data[0] == (uint8_t)~eep_data[1])
  {
    volume = eep_data[0];
  }
  else
  {
    volume = 0;
    eepromWriteByte(_EEP_ADDR_VOLUME+0, volume);
    eepromWriteByte(_EEP_ADDR_VOLUME+1, ~volume);
  }

  return true;
}

void speakerEnable(void)
{
  gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_LOW);
}

void speakerDisable(void)
{
  gpioPinWrite(_PIN_GPIO_SPK_EN, _DEF_HIGH);
}

void speakerSetVolume(uint8_t volume_data)
{
  if (volume != volume_data)
  {
    volume = volume_data;
    eepromWriteByte(_EEP_ADDR_VOLUME+0, volume_data);
    eepromWriteByte(_EEP_ADDR_VOLUME+1, ~volume_data);
  }
}

uint8_t speakerGetVolume(void)
{


  return volume;
}


void speakerStart(uint32_t hz)
{
  dacSetup(hz);
  dacStart();
}

void speakerStop(void)
{
  dacStop();
}

uint32_t speakerAvailable(void)
{
  return dacAvailable();
}

void speakerPutch(uint8_t data)
{
  dacPutch(map(data, 0, 255, 0, (volume>>1)));
}

void speakerWrite(uint8_t *p_data, uint32_t length)
{
  uint32_t i;


  for (i=0; i<length; i++)
  {
    speakerPutch(p_data[i]);
  }
}
