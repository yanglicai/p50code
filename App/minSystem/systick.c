/*
******************************************************************************
* @file    core/systick.c
* @author  Figo
* @version V1.0.0
* @date    2019.12.27
* @brief 
*
******************************************************************************

*/

#include "stm32f0xx.h"
#include "systick.h"

volatile uint32_t sysMsCount; 
volatile uint32_t sysRunTime[32]; //��32���Ĵ���������    

static __IO uint32_t TimingDelay;

/*******************************************************************************
* Function Name  : void TimingDelay_Decrement(void)
* Description    : dec   ��Ҫ����1ms�ж���
*******************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/*******************************************************************************
* Function Name  : void systick_delay_ms(__IO uint32_t nTime)
* Description    : delay ms
*******************************************************************************/
void systick_delay_ms(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

//=============================================END OF FILES========================================
