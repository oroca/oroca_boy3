#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void goLeft_pc();
    virtual void goRight_pc();

    virtual void handleTickEvent(void);
    virtual void handleKeyEvent(uint8_t key);

    void processKey(void);
    void updateSlotInfo(void);
protected:

private:

    uint32_t prev_key;
    uint32_t key_repeat;


};

#endif // MAIN_VIEW_HPP
