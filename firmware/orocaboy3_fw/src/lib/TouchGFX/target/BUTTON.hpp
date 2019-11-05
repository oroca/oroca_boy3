/*
 * BUTTON.hpp
 *
 *  Created on: 2019. 9. 1.
 *      Author: Baram
 */

#ifndef SRC_LIB_TOUCHGFX_TARGET_BUTTON_HPP_
#define SRC_LIB_TOUCHGFX_TARGET_BUTTON_HPP_

#include <touchgfx/hal/HAL.hpp>
#include <platform/driver/button/ButtonController.hpp>


class ButtonSampler : public touchgfx::ButtonController
{
public:
  ButtonSampler() {
        init();
    }
    virtual ~ButtonSampler() {}
    virtual void init();
    virtual bool sample(uint8_t& key);
};


#endif /* SRC_LIB_TOUCHGFX_TARGET_BUTTON_HPP_ */
