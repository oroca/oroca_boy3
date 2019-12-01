/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/main_screen/MainViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

MainViewBase::MainViewBase()
{
    box_bg.setPosition(0, 0, 320, 240);
    box_bg.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    image_speaker.setXY(4, 3);
    image_speaker.setBitmap(touchgfx::Bitmap(BITMAP_A4_ID));

    textArea_title.setXY(124, 4);
    textArea_title.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 8, 245));
    textArea_title.setLinespacing(0);
    textArea_title.setTypedText(touchgfx::TypedText(T_SINGLEUSEID1));

    animatedImage_title.setXY(96, 5);
    animatedImage_title.setBitmaps(BITMAP_OROCABOY1_ID, BITMAP_OROCABOY4_ID);
    animatedImage_title.setUpdateTicksInterval(6);
    animatedImage_title.startAnimation(false, true, true);

    swipeContainer_emulator.setXY(0, 37);

    page_nes.setWidth(320);
    page_nes.setHeight(150);

    image_nes.setXY(60, 41);
    image_nes.setBitmap(touchgfx::Bitmap(BITMAP_A1_ID));
    page_nes.add(image_nes);
    swipeContainer_emulator.add(page_nes);

    page_gnuboy.setWidth(320);
    page_gnuboy.setHeight(150);

    image_gameboy.setXY(60, 56);
    image_gameboy.setBitmap(touchgfx::Bitmap(BITMAP_GAMEBOY_ID));
    page_gnuboy.add(image_gameboy);
    swipeContainer_emulator.add(page_gnuboy);

    page_msx.setWidth(320);
    page_msx.setHeight(150);

    image_msx.setXY(60, 39);
    image_msx.setBitmap(touchgfx::Bitmap(BITMAP_MSX_ID));
    page_msx.add(image_msx);
    swipeContainer_emulator.add(page_msx);

    page_doom.setWidth(320);
    page_doom.setHeight(150);

    image_doom.setXY(60, 39);
    image_doom.setBitmap(touchgfx::Bitmap(BITMAP_DOOM_ID));
    page_doom.add(image_doom);
    swipeContainer_emulator.add(page_doom);

    page_user1.setWidth(320);
    page_user1.setHeight(150);

    textArea_slot_title.setXY(1, 66);
    textArea_slot_title.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea_slot_title.setLinespacing(0);
    textArea_slot_titleBuffer[0] = 0;
    textArea_slot_title.setWildcard(textArea_slot_titleBuffer);
    textArea_slot_title.resizeToCurrentText();
    textArea_slot_title.setTypedText(touchgfx::TypedText(T_SINGLEUSEID8));
    page_user1.add(textArea_slot_title);
    swipeContainer_emulator.add(page_user1);

    page_user2.setWidth(320);
    page_user2.setHeight(150);

    textArea_slot_title_1.setXY(1, 66);
    textArea_slot_title_1.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea_slot_title_1.setLinespacing(0);
    textArea_slot_title_1Buffer[0] = 0;
    textArea_slot_title_1.setWildcard(textArea_slot_title_1Buffer);
    textArea_slot_title_1.resizeToCurrentText();
    textArea_slot_title_1.setTypedText(touchgfx::TypedText(T_SINGLEUSEID9));
    page_user2.add(textArea_slot_title_1);
    swipeContainer_emulator.add(page_user2);

    swipeContainer_emulator.setPageIndicatorBitmaps(touchgfx::Bitmap(BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_NORMAL_ID), touchgfx::Bitmap(BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_HIGHLIGHT_ID));
    swipeContainer_emulator.setPageIndicatorXY(115, 0);
    swipeContainer_emulator.setSwipeCutoff(50);
    swipeContainer_emulator.setEndSwipeElasticWidth(50);
    swipeContainer_emulator.setSelectedPage(4);

    button_load.setXY(75, 263);
    button_load.setBitmaps(touchgfx::Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_ID), touchgfx::Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));
    button_load.setLabelText(touchgfx::TypedText(T_SINGLEUSEID4));
    button_load.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    button_load.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    image_bat3.setXY(279, 2);
    image_bat3.setBitmap(touchgfx::Bitmap(BITMAP_BAT3_ID));

    image_bat4.setXY(279, 2);
    image_bat4.setBitmap(touchgfx::Bitmap(BITMAP_BAT4_ID));

    image_bat2.setXY(279, 2);
    image_bat2.setBitmap(touchgfx::Bitmap(BITMAP_BAT2_ID));

    image_bat1.setXY(279, 2);
    image_bat1.setBitmap(touchgfx::Bitmap(BITMAP_A2_ID));

    image_bat.setBitmap(touchgfx::Bitmap(BITMAP_B1_ID));
    image_bat.setPosition(287, 3, 18, 22);
    image_bat.setScalingAlgorithm(touchgfx::ScalableImage::BILINEAR_INTERPOLATION);

    image_sd.setXY(261, 3);
    image_sd.setBitmap(touchgfx::Bitmap(BITMAP_SD_CARD_ID));

    textArea_volume.setXY(23, 0);
    textArea_volume.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea_volume.setLinespacing(0);
    Unicode::snprintf(textArea_volumeBuffer, TEXTAREA_VOLUME_SIZE, "%s", touchgfx::TypedText(T_SINGLEUSEID6).getText());
    textArea_volume.setWildcard(textArea_volumeBuffer);
    textArea_volume.resizeToCurrentText();
    textArea_volume.setTypedText(touchgfx::TypedText(T_SINGLEUSEID5));

    image_drive.setXY(242, 4);
    image_drive.setBitmap(touchgfx::Bitmap(BITMAP_USB1_ID));

    textArea_slot.setXY(3, 227);
    textArea_slot.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));
    textArea_slot.setLinespacing(0);
    textArea_slotBuffer[0] = 0;
    textArea_slot.setWildcard(textArea_slotBuffer);
    textArea_slot.resizeToCurrentText();
    textArea_slot.setTypedText(touchgfx::TypedText(T_SINGLEUSEID7));

    add(box_bg);
    add(image_speaker);
    add(textArea_title);
    add(animatedImage_title);
    add(swipeContainer_emulator);
    add(button_load);
    add(image_bat3);
    add(image_bat4);
    add(image_bat2);
    add(image_bat1);
    add(image_bat);
    add(image_sd);
    add(textArea_volume);
    add(image_drive);
    add(textArea_slot);
}

void MainViewBase::setupScreen()
{

}

//Handles when a key is pressed
void MainViewBase::handleKeyEvent(uint8_t key)
{
    if(0 == key)
    {
        //goLeft_pc
        //When hardware button 0 clicked call virtual function
        //Call goLeft_pc
        goLeft_pc();

        //goRight_pc
        //When hardware button 0 clicked call virtual function
        //Call goRight_pc
        goRight_pc();

        //show_empty
        //When hardware button 0 clicked change screen to msg_box
        //Go to msg_box with no screen transition
        application().gotomsg_boxScreenNoTransition();

        //go_home
        //When hardware button 0 clicked change screen to btn_show
        //Go to btn_show with no screen transition
        application().gotobtn_showScreenNoTransition();

        //go_btn_test
        //When hardware button 0 clicked change screen to btn_show
        //Go to btn_show with no screen transition
        application().gotobtn_showScreenNoTransition();
    }
}
