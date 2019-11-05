#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#ifndef SIMULATOR
#include "hw.h"
#endif


uint8_t current_page = 0;
bool    slot_run = false;
static uint32_t pre_time;

MainView::MainView()
{
}

void MainView::setupScreen()
{
  swipeContainer_emulator.setSelectedPage(current_page);
  image_bat.setVisible(false);
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
  static uint32_t pre_time_bat;


  if (slot_run == true && millis()-pre_time >= 100)
  {
    slot_run = false;

    if (slotRunFromFlash(swipeContainer_emulator.currentPage) == false)
    {
      //MainViewBase::handleKeyEvent(51);
      button_load.setVisible(false);
      button_load.invalidate();
    }
  }

  if (millis()-pre_time_bat >= 100)
  {
    pre_time_bat = millis();
    if (gpioPinRead(_PIN_GPIO_BAT_CHG) == _DEF_LOW)
    {
      image_bat.setVisible(true);
    }
    else
    {
      image_bat.setVisible(false);
    }
    image_bat.invalidate();
  }
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
          if (slotIsAvailable(swipeContainer_emulator.currentPage) == true)
          {
            slot_run = true;
            button_load.moveTo(75,93);
            button_load.setVisible(true);
            button_load.invalidate();
            pre_time = millis();
          }
          else
          {
            MainViewBase::handleKeyEvent(46);
          }
          break;
      }
    }
  }

#endif

}

