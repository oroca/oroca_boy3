#include <gui/msg_box_screen/msg_boxView.hpp>
#ifndef SIMULATOR
#include "hw.h"
#endif

msg_boxView::msg_boxView()
{

}

void msg_boxView::setupScreen()
{
    msg_boxViewBase::setupScreen();
}

void msg_boxView::tearDownScreen()
{
    msg_boxViewBase::tearDownScreen();
}

void msg_boxView::handleKeyEvent(uint8_t key)
{
  msg_boxViewBase::handleKeyEvent(key);

#ifndef SIMULATOR
  static uint32_t pre_time;

  if (millis()-pre_time >= 100)
  {
    pre_time = millis();

    if(key == _DEF_HW_BTN_B)
    {
      msg_boxViewBase::handleKeyEvent(51);
    }
  }
#endif

}
