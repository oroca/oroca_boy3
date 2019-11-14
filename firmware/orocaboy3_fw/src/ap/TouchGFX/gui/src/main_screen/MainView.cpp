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
  image_sd.setVisible(false);
  image_drive.setVisible(false);

  image_bat1.setVisible(true);
  image_bat2.setVisible(false);
  image_bat3.setVisible(false);
  image_bat4.setVisible(false);
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

    if (slotRun(swipeContainer_emulator.currentPage) == false)
    {
      //MainViewBase::handleKeyEvent(51);
      button_load.setVisible(false);
      button_load.invalidate();
    }
  }

  if (millis()-pre_time_bat >= 100)
  {
    pre_time_bat = millis();


    image_bat1.setVisible(false);
    image_bat2.setVisible(false);
    image_bat3.setVisible(false);
    image_bat4.setVisible(false);

    if (batteryGetLevel() > 80)
    {
      image_bat1.setVisible(true);
    }
    else if (batteryGetLevel() > 50)
    {
      image_bat2.setVisible(true);
    }
    else if (batteryGetLevel() > 20)
    {
      image_bat3.setVisible(true);
    }
    else
    {
      image_bat4.setVisible(true);
    }
    image_bat1.invalidate();
    image_bat2.invalidate();
    image_bat3.invalidate();
    image_bat4.invalidate();


    if (gpioPinRead(_PIN_GPIO_BAT_CHG) == _DEF_LOW)
    {
      image_bat.setVisible(true);
    }
    else
    {
      image_bat.setVisible(false);
    }
    image_bat.invalidate();

    if (sdIsDetected() == true)
    {
      image_sd.setVisible(true);
    }
    else
    {
      image_sd.setVisible(false);
    }
    image_sd.invalidate();

    if (usbGetMode() == USB_MSC_MODE && sdIsDetected() == true)
    {
      image_drive.setVisible(true);
    }
    else
    {
      image_drive.setVisible(false);
    }
    image_drive.invalidate();

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
          application().gotobtn_showScreenNoTransition();
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

