/**
  ******************************************************************************
  * @file    myMath.c 
  * @author  figo
  * @version V1.0.0
  * @date    04/01/2020
  * @brief     

常规算法 集成 脱离硬件，纯软件算法
	
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/


#include "myMath.h"

uint16_t myCal_IntegerDelt(uint16_t data1,uint16_t data2)
{
	uint16_t maxData,minData;
	
	maxData = MAX(data1,data2);
	minData = MIN(data1,data2);
	
	return (maxData - minData);
}

float myCal_FloatDelt(float data1,float data2)
{
	float maxData,minData;
	
	maxData = MAX(data1,data2);
	minData = MIN(data1,data2);
	
	return (maxData - minData);
}

/*********************************************
函数名称: uint16_t sys_calAverage(uint16_t *array,uint8_t num)
函数说明: 计算平均值
*********************/
uint16_t sys_calAverage(uint16_t *array, uint8_t num)
{
	uint8_t j, k,adcNum;

	uint16_t Average_Val, rtemp;

	for (j = 0; j<(num - 1); j++)
	{
		for (k = 0; k<(num - 1 - j); k++)
		{
			if (array[k] > array[k + 1])
			{
				rtemp = array[k];
				array[k] = array[k + 1];
				array[k + 1] = rtemp;
			}
		}
	}

	adcNum = num >> 1;
	
	Average_Val = array[adcNum];

	return Average_Val;
}




