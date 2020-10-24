/**
  ******************************************************************************
  * @file    smoke.h 
  * @author  Figo 
  * @version V1.0.0
  * @date    11-10-2019
  * @brief   battery program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SMOKE_H
#define __SMOKE_H


/* Includes ------------------------------------------------------------------*/

/* IO define ----------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void smoke_portInit(void);
void smoke_init(void);
void smoke_calParameter(uint8_t out_chnNum);
void smoke_outPowerRegulation(uint8_t out_chnNum);

#endif 


