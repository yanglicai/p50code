/**
  ******************************************************************************
  * @file    delay.c
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   delay program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/



/* Exported functions ------------------------------------------------------- */

void delay_us_soft(uint16_t us)
{
	uint16_t j,k;
	
  for (j=0;j<us;j++)
	{
      for (k=0;k<8;k++);
	}
}

void delay_ms_soft(uint16_t ms)
{  	
	for(; ms!=0; ms--)
	{
		delay_us_soft(1000);
	}
}




