
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
    if (buttonGetPressed(i) == true)
    {
      key = i;
      return true;
    }
  }

  return false;
}
