/**
  ******************************************************************************
  * @file    user/systick.h
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   系统准确定时ms
  * 
  ******************************************************************************
  * @attention
  
  *
  ******************************************************************************
  */

#ifndef _SYSTICK_H
#define _SYSTICK_H

#include "stm32f0xx.h"
#include "type.h" 

extern volatile  ub8 timeFlag;   

extern volatile uint32_t sysMsCount; 
extern volatile uint32_t sysRunTime[32]; 

#define B_TIME_1MS     timeFlag.BIT.b0
#define B_TIME_10MS    timeFlag.BIT.b1
#define B_TIME_100MS   timeFlag.BIT.b2
#define B_TIME_1S      timeFlag.BIT.b3
#define B_TIME_CHARGE  timeFlag.BIT.b4
#define B_RTC_1S       timeFlag.BIT.b5

//延时程序

extern  void systick_delay_ms(__IO uint32_t nTime);
extern  void Systick_Init(void);
extern  void TimingDelay_Decrement(void);
extern  void HundredUs_Decrement(void);
extern  void delay_us(uint32_t us);
extern  void delay_ms(uint16_t ms);

#endif

