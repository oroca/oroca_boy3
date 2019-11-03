#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#ifndef SIMULATOR
#include "hw.h"
#endif


uint8_t current_page = 0;


MainView::MainView()
{
}

void MainView::setupScreen()
{
  swipeContainer_emulator.setSelectedPage(current_page);
}

void MainView::tearDownScreen()
{
  current_page = swipeContainer_emulator.currentPage;
}


void MainView::goLeft_pc()
{
  touchgfx::GestureEvent evt(GestureEvent::SWIPE_HORIZONTAL, 100, 0, 0);
  swipeContainer_emulator.handleGestureEvent(evt);
}

void MainView::goRight_pc()
{
  touchgfx::GestureEvent evt(GestureEvent::SWIPE_HORIZONTAL, -100, 0, 0);
  swipeContainer_emulator.handleGestureEvent(evt);
}

void MainView::handleTickEvent(void)
{
#ifndef SIMULATOR
  //if (buttonGetPressed(_DEF_HW_BTN_UP)) circle1.setLineWidth(0);
  //else                                  circle1.setLineWidth(1);
  //circle1.invalidate();

#endif
}

void MainView::handleKeyEvent(uint8_t key)
{
  MainViewBase::handleKeyEvent(key);

#ifndef SIMULATOR
  static uint32_t pre_time;

  if (millis()-pre_time >= 100)
  {
    pre_time = millis();

    if(key == _DEF_HW_BTN_LEFT)
    {
      goLeft_pc();
    }
    if(key == _DEF_HW_BTN_RIGHT)
    {
      goRight_pc();
    }
    if(key == _DEF_HW_BTN_A)
    {
      switch(swipeContainer_emulator.currentPage)
      {
        case 3:
          MainViewBase::handleKeyEvent(49);
          break;

        default:
          MainViewBase::handleKeyEvent(51);
          hwRunFw(0);
          break;
      }
    }
  }

#endif

}
