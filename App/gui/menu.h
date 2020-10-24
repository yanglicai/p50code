/**
  ******************************************************************************
  * @file    menu.h 
  * @author  figo
  * @version V1.0.0
  * @date    04/01/2020
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MENU_H_
#define _MENU_H_

#include <math.h>
#include "typedef.h"

extern void menu_deInit(void);
extern void menu_showHomepage(void);
extern void menu_confirmHandle(void);
extern void menu_adjustHomepage(uint8_t addDecFlag);

extern uint8_t menu_getMode(void);
extern void menu_setMode(uint8_t modeType);
extern uint8_t menu_getColor(void);
extern uint8_t menu_modeShiftToAuto(void);

#endif


