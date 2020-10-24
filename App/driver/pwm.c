/**
  ******************************************************************************
  * @file    pwm.c
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   pwm program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "pwm.h" 

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

out_mode_t	out_mode;
pwm_data_t	pwm_data;
extern TIM_HandleTypeDef htim3;


/*********************************************
函数名称:pwm_init 
*********************************************/  
void pwm_init(void)  
{	 
	

	
}
/*********************************************
函数名称:pwm_enable   
*********************************************/
void pwm_enable(void)
{
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);

	HAL_TIM_Base_Start(&htim3);
}

/*********************************************
函数名称:pwm_disable    
*********************************************/
void pwm_disable(void) 
{
  HAL_TIM_OC_Stop(&htim3, TIM_CHANNEL_1);

	HAL_TIM_Base_Stop(&htim3);
}

/*********************************************
函数名称:void pwm_setDuty(uint32_t dutyNewValue
*********************************************/
void pwm_setDuty(uint32_t dutyNewValue)
{
	if (dutyNewValue < 300)
	{
	  htim3.Instance->CCR1 = dutyNewValue; //设置占空比
	}
}





