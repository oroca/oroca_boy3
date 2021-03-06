/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef BTN_SHOWVIEWBASE_HPP
#define BTN_SHOWVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/btn_show_screen/btn_showPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <touchgfx/widgets/canvas/PainterRGB565.hpp>

class btn_showViewBase : public touchgfx::View<btn_showPresenter>
{
public:
    btn_showViewBase();
    virtual ~btn_showViewBase() {}
    virtual void setupScreen();
    virtual void handleKeyEvent(uint8_t key);

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box box1;
    touchgfx::Circle btn_x;
    touchgfx::PainterRGB565 btn_xPainter;
    touchgfx::Circle btn_b;
    touchgfx::PainterRGB565 btn_bPainter;
    touchgfx::Circle btn_a;
    touchgfx::PainterRGB565 btn_aPainter;
    touchgfx::Circle btn_y;
    touchgfx::PainterRGB565 btn_yPainter;
    touchgfx::Circle btn_up;
    touchgfx::PainterRGB565 btn_upPainter;
    touchgfx::Circle btn_down;
    touchgfx::PainterRGB565 btn_downPainter;
    touchgfx::Circle btn_right;
    touchgfx::PainterRGB565 btn_rightPainter;
    touchgfx::Circle btn_left;
    touchgfx::PainterRGB565 btn_leftPainter;
    touchgfx::Circle btn_select;
    touchgfx::PainterRGB565 btn_selectPainter;
    touchgfx::Circle btn_start;
    touchgfx::PainterRGB565 btn_startPainter;
    touchgfx::Circle btn_joy;
    touchgfx::PainterRGB565 btn_joyPainter;

private:

    /*
     * Canvas Buffer Size
     */
    static const uint16_t CANVAS_BUFFER_SIZE = 4800;
    uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];

};

#endif // BTN_SHOWVIEWBASE_HPP
