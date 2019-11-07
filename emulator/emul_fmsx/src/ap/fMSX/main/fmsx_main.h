/*
 * fmsx_main.h
 *
 *  Created on: 2019. 11. 6.
 *      Author: HanCheol Cho
 */

#ifndef SRC_AP_FMSX_MAIN_FMSX_MAIN_H_
#define SRC_AP_FMSX_MAIN_FMSX_MAIN_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"



#define FMSX_CONFIG_FILE    "/sd/odroid/data/msx/config.ini"
#define FMSX_ROOT_GAMESDIR  "/sd/roms/msx"
#define FMSX_ROOT_DATADIR   "/sd/odroid/data/msx"
#define FMSX_MP_ROM_DATA    "/sd/odroid/data/msx/mprom.rom"




#define WIDTH   256
#define HEIGHT  216 // normally the msx has 212 lines in PAL, 4 more to send the bytes faster over SPI to the display

#define WIDTH_OVERLAY   320
#define HEIGHT_OVERLAY  240





void fmsxMain(void);



#ifdef __cplusplus
 }
#endif



#endif /* SRC_AP_FMSX_MAIN_FMSX_MAIN_H_ */
