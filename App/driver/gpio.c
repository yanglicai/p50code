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
  * @brief  初始化IO 中断配置
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
函数名称:  void sys_ncPortInit(void) 
悬空端口的配置 一般配置为输出固定电平
GPIO 配置为上拉，输出为低，对应在功耗大约为75uA
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
//函数名称:  void sys_testPortInit(void) 
//悬空端口的配置 一般配置为输出固定电平
//GPIO 配置为上拉，输出为低，对应在功耗大约为75uA
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
  * @brief  休眠设置IO
  * @param  None
  * @retval None
 原来功耗800uA，增加未用端口的配置（设置为输出 上拉  输出高），功耗降低为300uA
 增加了休眠前配置端口--低电平对应 GPIO_PuPd_DOWN 高电平 对应GPIO_PuPd_UP
 同时设置 OUT_AD_GND_OFF LED_OFF之后，功耗降低为 120uA
 MOSI_L;    SCK_L;   设置 导致功耗增加到550uA
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
	  //关闭触摸
	  button_off();  
		button_DeInit(); //

		//关闭充电		  		
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
	
		//中断唤醒 
		gpio_isrConfig();//
		smoke_atoIsrConfig(sysOnFlag);
		
		//关闭ADC
		adc_off(); 
	  adc_DeInit();	
		
		//关闭oled 关闭SPI
	  oled_clearScreen();
		oled_Off(); 
	  oled_DeInit();

		smoke_off(); 
		smoke_DeInit(); //11.02	
		
		//中断唤醒 
		gpio_isrConfig();//
		smoke_atoIsrConfig(sysOnFlag);
		
		//关闭定时器		
		time_100usOff(); 
		time_1msOff();
	  time_DeInit();
		
		sys_ncPortInit();
		button_Init(); //休眠前强制处理
}


void sys_enterSleep(void)
{    	
	  
    HAL_PWR_DeInit();

		HAL_PWR_DisableBkUpAccess();   	

//	  HAL_PWR_EnterSTANDBYMode(); //深度休眠，需要重启才能激活
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); 
}




//2019.12.26 基本处理后，功耗为500uA
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

