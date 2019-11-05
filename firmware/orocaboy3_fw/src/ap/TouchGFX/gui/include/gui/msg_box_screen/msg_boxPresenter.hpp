#ifndef MSG_BOXPRESENTER_HPP
#define MSG_BOXPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class msg_boxView;

class msg_boxPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    msg_boxPresenter(msg_boxView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~msg_boxPresenter() {};

private:
    msg_boxPresenter();

    msg_boxView& view;
};


#endif // MSG_BOXPRESENTER_HPP
