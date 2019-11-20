/*
 * hw.h
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "hw_def.h"


#include "micros.h"
#include "millis.h"
#include "delay.h"
#include "led.h"
#include "uart.h"
#include "cmd.h"
#include "cmdif.h"
#include "i2c.h"
#include "reset.h"
#include "rtc.h"
#include "qspi.h"
#include "swtimer.h"
#include "button.h"
#include "pwm.h"
#include "sdram.h"
#include "eeprom.h"
#include "flash.h"
#include "gpio.h"
#include "usb.h"
#include "vcp.h"
#include "adc.h"
#include "ltdc.h"
#include "mem.h"
#include "lcd.h"
#include "dac.h"
#include "timer.h"
#include "speaker.h"
#include "mpu.h"
#include "resize.h"
#include "battery.h"
#include "joypad.h"
#include "osd.h"
#include "files.h"

#include "sd.h"
#include "fatfs/fatfs.h"


void hwInit(void);


uint8_t hwGetResetCount(void);
void hwJumpToBoot(void);
void hwJumpToFw(void);



#ifdef __cplusplus
 }
#endif

#endif /* SRC_HW_HW_H_ */
