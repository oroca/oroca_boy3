#include <gui/btn_show_screen/btn_showView.hpp>
#ifndef SIMULATOR
#include "hw.h"
#endif


btn_showView::btn_showView()
{

}

void btn_showView::setupScreen()
{
    btn_showViewBase::setupScreen();
}

void btn_showView::tearDownScreen()
{
    btn_showViewBase::tearDownScreen();
}

void btn_showView::handleTickEvent(void)
{
#ifndef SIMULATOR
  if (buttonGetPressed(_DEF_HW_BTN_UP)) btn_up.setLineWidth(0);
  else                                  btn_up.setLineWidth(1);
  btn_up.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_DOWN)) btn_down.setLineWidth(0);
  else                                    btn_down.setLineWidth(1);
  btn_down.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_LEFT)) btn_left.setLineWidth(0);
  else                                    btn_left.setLineWidth(1);
  btn_left.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_RIGHT)) btn_right.setLineWidth(0);
  else                                     btn_right.setLineWidth(1);
  btn_right.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_A)) btn_a.setLineWidth(0);
  else                                 btn_a.setLineWidth(1);
  btn_a.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_B)) btn_b.setLineWidth(0);
  else                                 btn_b.setLineWidth(1);
  btn_b.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_X)) btn_x.setLineWidth(0);
  else                                 btn_x.setLineWidth(1);
  btn_x.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_Y)) btn_y.setLineWidth(0);
  else                                 btn_y.setLineWidth(1);
  btn_y.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_SELECT)) btn_select.setLineWidth(0);
  else                                      btn_select.setLineWidth(1);
  btn_select.invalidate();

  if (buttonGetPressed(_DEF_HW_BTN_START)) btn_start.setLineWidth(0);
  else                                     btn_start.setLineWidth(1);
  btn_start.invalidate();



  btn_joy.moveTo(44  + map(adcRead(0), 0, 4095, -20, 20),
                 168 - map(adcRead(1), 0, 4095, -20, 20));
  btn_joy.invalidate();
#endif
}

void btn_showView::handleKeyEvent(uint8_t key)
{
  btn_showViewBase::handleKeyEvent(key);

#ifndef SIMULATOR
  static uint32_t pre_time;

  if (millis()-pre_time >= 100)
  {
    pre_time = millis();
  }

#endif

}
