/**
  ******************************************************************************
  * @file    timer.c
  * @author  figo
  * @version V1.0.0
  * @date    29-02-2019
  * @brief   timer program body
	timer3 100us
	time6 base 1ms  ע�������ʱ�����ܻ�Ӱ�칦��
  time1 time14 PWM	
	����Ϊֻ���ļ�������Ŀ�в���Ҫ�޸�
  */ 
	
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim6;

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */




/*********************************************
��������:time_100usInit   
*********************************************/   
void time_100usInit(void)  
{	 	
   
	
} 


void time_100usOn(void)
{
	HAL_TIM_Base_Start_IT(&htim1); 
}

void time_100usOff(void)
{
	HAL_TIM_Base_Stop_IT(&htim1); 
}

/*********************************************
��������:time_msInit  
*********************************************/
void time_msInit(void) 
{	 	
    
}  

void time_1msOn(void)
{
	HAL_TIM_Base_Start_IT(&htim6); 
}

void time_1msOff(void)
{
	HAL_TIM_Base_Stop_IT(&htim6);	
} 

void time_DeInit(void)
{
	HAL_TIM_Base_DeInit(&htim1); 
	
	HAL_TIM_Base_DeInit(&htim6); 
}


/*********************************************
��������:void smoke_pwmSetDuty(uint32_t dutyNewValue
*********************************************/
void smoke_pwmSetDuty(uint32_t dutyNewValue)
{
//����ռ�ձ�
}









