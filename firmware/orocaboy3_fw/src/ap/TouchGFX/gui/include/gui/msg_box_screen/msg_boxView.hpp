#ifndef MSG_BOXVIEW_HPP
#define MSG_BOXVIEW_HPP

#include <gui_generated/msg_box_screen/msg_boxViewBase.hpp>
#include <gui/msg_box_screen/msg_boxPresenter.hpp>

class msg_boxView : public msg_boxViewBase
{
public:
    msg_boxView();
    virtual ~msg_boxView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();


    void handleKeyEvent(uint8_t key);
protected:
};

#endif // MSG_BOXVIEW_HPP
