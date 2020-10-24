/*
******************************************************************************
* @file    driver/driver_init.c
* @author  Figo
* @version V1.0.0
* @date    2019.09.16
* @brief 
*
******************************************************************************

*/

#include <stdint.h> 

#include "driver_init.h"
#include "systick.h"   
#include "myTimer.h"
#include "adc.h"
#include "button.h"
#include "battery.h"
#include "smoke.h"
#include "pwm.h"
#include "oled.h"  
#include "gpio.h"

/*********************************************
��������:uint8_t driver_init(uint8_t sysOnFlag)
���ܣ�adc���� pwm���� ���ܳ�ʼ��     
*********************************************/
uint8_t driver_init(uint8_t sysOnFlag) 
{ 
	  if (sysOnFlag)
		{
			button_Init();
			button_on();	
			keyApp_silderInit();			
		}
		
		adc_init();  //DMA���ô���
		smoke_init();
		pwm_init();
		bat_chargeInit();
	  sys_ncPortInit();
		
	  return 1;
}





 //================================================== END OF FILES==================================//
