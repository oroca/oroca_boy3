
#include "STM32H750TouchController.hpp"

extern "C"
{
//#include "stm32h750b_discovery_ts.h"
#include "ts.h"

    uint32_t LCD_GetXSize();
    uint32_t LCD_GetYSize();
}

using namespace touchgfx;

void STM32H750TouchController::init()
{

}

bool STM32H750TouchController::sampleTouch(int32_t& x, int32_t& y)
{

  return false;
}
