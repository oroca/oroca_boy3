/*
 * menu.h
 *
 *  Created on: 2019. 11. 6.
 *      Author: HanCheol Cho
 */

#ifndef SRC_AP_FMSX_MAIN_MENU_H_
#define SRC_AP_FMSX_MAIN_MENU_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"



typedef enum  {
   MENU_ACTION_NONE,
   MENU_ACTION_CHANGED_AUDIO_TYPE
} MENU_ACTION;


void fmsxGUI_initMenu();
MENU_ACTION fmsxGUI_showMenu();


#ifdef __cplusplus
 }
#endif


#endif /* SRC_AP_FMSX_MAIN_MENU_H_ */
