/**
  ******************************************************************************
  * @file    delay.h 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   Main program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DELAY_H
#define __DELAY_H

/* Includes ------------------------------------------------------------------*/

#include "typedef.h"


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/



/* Exported functions ------------------------------------------------------- */

void delay_us_soft(uint16_t us);
void delay_ms_soft(uint16_t ms);


#endif 


