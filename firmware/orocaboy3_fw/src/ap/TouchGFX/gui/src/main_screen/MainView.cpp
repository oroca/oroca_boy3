#include <gui/main_screen/MainView.hpp>
#include "BitmapDatabase.hpp"
#ifndef SIMULATOR
#include "hw.h"
#endif


uint8_t current_page = 0;
bool    slot_run = false;
static uint32_t pre_time;

typedef struct
{
  bool is_empty;
  Unicode::UnicodeChar name_str[32];
  Unicode::UnicodeChar ver_str[32];
} slot_info_t;

slot_info_t slot_info[16];


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

  textArea_slot.setWidth(320);
  textArea_slot_title.setWidth(320);
  textArea_slot_title_1.setWidth(320);

  flash_tag_t tag;


  for (int i=0; i<16; i++)
  {
    if (slotGetTag(i, &tag) == true)
    {
      slot_info[i].is_empty = false;
      Unicode::fromUTF8((const uint8_t*)tag.name_str, slot_info[i].name_str, 32);
      Unicode::fromUTF8((const uint8_t*)tag.version_str, slot_info[i].ver_str, 32);
    }
    else
    {
      slot_info[i].is_empty = true;
    }
  }
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

    if (batteryGetLevel() > 70)
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


    Unicode::snprintf(MainView::textArea_volumeBuffer, TEXTAREA_VOLUME_SIZE, "%d", speakerGetVolume());
    textArea_volume.invalidate();

    int slot_index = swipeContainer_emulator.currentPage;

    if (slot_info[slot_index].is_empty != true)
    {
      Unicode::snprintf(MainView::textArea_slotBuffer, TEXTAREA_SLOT_SIZE, "%d %s %s", slot_index,
                        slot_info[slot_index].name_str, slot_info[slot_index].ver_str);
      textArea_slot.invalidate();

      Unicode::snprintf(MainView::textArea_slot_titleBuffer, TEXTAREA_SLOT_SIZE, "%s", slot_info[slot_index].name_str);
      textArea_slot_title.invalidate();
      Unicode::snprintf(MainView::textArea_slot_title_1Buffer, TEXTAREA_SLOT_SIZE, "%s",slot_info[slot_index].name_str);
      textArea_slot_title_1.invalidate();
    }
    else
    {
      Unicode::snprintf(MainView::textArea_slotBuffer, TEXTAREA_SLOT_SIZE, "%d Empty", slot_index);
      textArea_slot.invalidate();

      Unicode::snprintf(MainView::textArea_slot_titleBuffer, TEXTAREA_SLOT_SIZE, "Empty");
      textArea_slot_title.invalidate();
      Unicode::snprintf(MainView::textArea_slot_title_1Buffer, TEXTAREA_SLOT_SIZE, "Empty");
      textArea_slot_title_1.invalidate();
    }

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
            application().gotomsg_boxScreenNoTransition();
          }
          break;
      }
    }
  }

#endif

}

