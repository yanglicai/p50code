

#ifndef _DRIVER_INIT_H
#define _DRIVER_INIT_H

#include "stm32f0xx.h"



void key_portSleepInit(void);

void oled_portSleepInit(void);

void pwm_portSleepInit(void);

void smoke_portSleepInit(void);

void bat_chargePortSleepInit(void);
void iic_portSleepInit(void);



void sys_ncPortInit(void);



#endif


