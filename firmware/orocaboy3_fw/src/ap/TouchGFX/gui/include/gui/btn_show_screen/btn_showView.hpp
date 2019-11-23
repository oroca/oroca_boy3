#ifndef BTN_SHOWVIEW_HPP
#define BTN_SHOWVIEW_HPP

#include <gui_generated/btn_show_screen/btn_showViewBase.hpp>
#include <gui/btn_show_screen/btn_showPresenter.hpp>

class btn_showView : public btn_showViewBase
{
public:
    btn_showView();
    virtual ~btn_showView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();


    void handleTickEvent(void);
    void handleKeyEvent(uint8_t key);

    void processKey(void);

protected:

    uint32_t prev_key;
    uint32_t key_repeat;

};

#endif // BTN_SHOWVIEW_HPP
