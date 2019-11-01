
#include "BUTTON.hpp"
#include "hw.h"

using namespace touchgfx;


void ButtonSampler::init()
{
  //Initialise gpio for buttons here
}
bool ButtonSampler::sample(uint8_t& key)
{
  uint8_t i;

  for (i=1; i<BUTTON_MAX_CH; i++)
  {
    if (buttonGetReleasedEvent(i) == true && buttonGetReleasedTime(i) < 100)
    {
      key = i-1;
      return true;
    }
  }

  return false;
}
