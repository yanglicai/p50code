/**
  ******************************************************************************
  * @file    wdt.c
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   wdt program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/
	   
#include "main.h"

bit BIT_TMP;	  

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */


/***********************************************************************
	WDT CONFIG enable 
	warning : this macro is only when ICP not enable CONFIG WDT function
	copy this marco code to you code to enable WDT reset.
************************************************************************/
static void Check_WDT_Reset_Config(void)
{
	
}


/**
  * @brief  初始化wdt
  * @param  None
  * @retval None
  */
void Wdt_Init(void)
{

}


/**
  * @brief  清除看门狗计数
  * @param  None
  * @retval None
  */
void Wdt_Free(void)
{

}



