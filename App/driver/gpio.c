/**
  ******************************************************************************
  * @file    gpio.c 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/


#include "button.h"
#include "battery.h"
#include "gpio.h"


#define POWER_SAVEING  0
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  ��ʼ��IO �ж�����
  * @param  None
  * @retval None #define USB_IN_Pin GPIO_PIN_11
#define USB_IN_GPIO_Port GPIOA
  */
void gpio_isrConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = USB_IN_Pin;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(USB_IN_GPIO_Port, &GPIO_InitStruct); //PA11 USB
	
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Pin = KEY_FIRE_Pin;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(KEY_FIRE_GPIO_Port, &GPIO_InitStruct);//PB14 KEY FIRE

	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
} 

/*********************************************
��������:  void sys_ncPortInit(void) 
���ն˿ڵ����� һ������Ϊ����̶���ƽ
GPIO ����Ϊ���������Ϊ�ͣ���Ӧ�ڹ��Ĵ�ԼΪ75uA
*********************************************/  
void sys_ncPortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
 	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //    GPIO_MODE_INPUT GPIO_MODE_OUTPUT_PP
  GPIO_InitStruct.Pull = GPIO_PULLDOWN; //GPIO_PULLDOWN GPIO_NOPULL GPIO_PULLUP
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  
#if (CHARGE_IC_IS_59121==1)
	
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_10;
	HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOB, GPIO_InitStruct.Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_WritePin(GPIOC, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);		
	
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	HAL_GPIO_WritePin(GPIOF, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	 
#else	
	
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOA, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
 
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);		
	HAL_GPIO_WritePin(GPIOC, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	 
  HAL_GPIO_WritePin(GPIOF, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	
#endif
} 

///*********************************************
//��������:  void sys_testPortInit(void) 
//���ն˿ڵ����� һ������Ϊ����̶���ƽ
//GPIO ����Ϊ���������Ϊ�ͣ���Ӧ�ڹ��Ĵ�ԼΪ75uA
//*********************************************/  
//void sys_testPortInit(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//  
//  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //    
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  
//	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
//	|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
//	
//  GPIO_InitStruct.Pin = GPIO_PIN_All;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	
//	
//  GPIO_InitStruct.Pin = GPIO_PIN_All;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);		
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_All;
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	
//	
//	GPIO_InitStruct.Pin = GPIO_PIN_All;
//	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);	 

//} 


/**
  * @brief  ��������IO
  * @param  None
  * @retval None
 ԭ������800uA������δ�ö˿ڵ����ã�����Ϊ��� ����  ����ߣ������Ľ���Ϊ300uA
 ����������ǰ���ö˿�--�͵�ƽ��Ӧ GPIO_PuPd_DOWN �ߵ�ƽ ��ӦGPIO_PuPd_UP
 ͬʱ���� OUT_AD_GND_OFF LED_OFF֮�󣬹��Ľ���Ϊ 120uA
 MOSI_L;    SCK_L;   ���� ���¹������ӵ�550uA
  */

extern void button_off(void);
extern void button_DeInit(void);

extern void bat_chargeRecover(void);
extern void bat_chargeOff(void);
extern void bat_chargeDeInit(void);
extern void bat_uartDeinit(void);
extern void SC8913_DeInit(void);
extern void IIC_DeInit(void);

extern void oled_Off(void);
extern void oled_DeInit(void);

extern void time_100usOff(void);
extern void time_1msOff(void);
extern void time_DeInit(void);

extern void adc_off(void);
extern void adc_DeInit(void);

extern void smoke_off(void);
extern void smoke_DeInit(void);

extern UART_HandleTypeDef huart1;
extern void oled_clearScreen(void);

extern void smoke_atoIsrConfig(uint8_t sysOnFlag);

extern uint8_t button_Fire_Press(void);
extern void spi_deInit(void);
extern void button_Init(void);

void driver_deInit(uint8_t sysOnFlag)
{
	  //�رմ���
	  button_off();  
		button_DeInit(); //

		//�رճ��		  		
#if (CHARGE_IC_IS_59121==0)	
    HAL_UART_DeInit(&huart1);
	  bat_chargeDeInit();
	  bat_uartDeinit();
	  SC8913_DeInit();
	  IIC_DeInit(); 	
#else
	  bat_chargeDeInit(); 
#endif
	
	  smoke_off(); 
		smoke_DeInit(); //11.02	
	
		//�жϻ��� 
		gpio_isrConfig();//
		smoke_atoIsrConfig(sysOnFlag);
		
		//�ر�ADC
		adc_off(); 
	  adc_DeInit();	
		
		//�ر�oled �ر�SPI
	  oled_clearScreen();
		oled_Off(); 
	  oled_DeInit();

		smoke_off(); 
		smoke_DeInit(); //11.02	
		
		//�жϻ��� 
		gpio_isrConfig();//
		smoke_atoIsrConfig(sysOnFlag);
		
		//�رն�ʱ��		
		time_100usOff(); 
		time_1msOff();
	  time_DeInit();
		
		sys_ncPortInit();
		button_Init(); //����ǰǿ�ƴ���
}


void sys_enterSleep(void)
{    	
	  
    HAL_PWR_DeInit();

		HAL_PWR_DisableBkUpAccess();   	

//	  HAL_PWR_EnterSTANDBYMode(); //������ߣ���Ҫ�������ܼ���
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); 
}




//2019.12.26 ��������󣬹���Ϊ500uA
void sys_sleepMode(uint8_t type)
{	
	  driver_deInit(type);
	
	  sys_enterSleep();
}

extern void sys_cubeInit(void);
extern void driver_init(uint8_t sysOnFlag);  

void sys_exitSleep(uint8_t type)
{      
	  sys_cubeInit();
	
	  driver_init(type);    	
}

