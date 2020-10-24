/**
  ******************************************************************************
  * @file    myTimer.h 
  * @author  figo
  * @version V1.0.0
  * @date    13-12-2019
  * @brief   timer program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYTIMER_H
#define __MYTIMER_H

/* Includes ------------------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void time_100usInit(void);
void time_100usOn(void);
void time_100usOff(void);

void time_msInit(void);  
void time_1msOn(void);
void time_1msOff(void);

#endif 


