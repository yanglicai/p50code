/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#define LOCK_IS_NEW_LOGIC   0

#define CHARGE_IC_IS_59121  1
#define OLED_IS_NEW         CHARGE_IC_IS_59121 //1 京东方 0 翰彩  

/* USER CODE BEGIN EFP */
extern  uint8_t flagNewLogic;
extern  uint8_t flagSysIsDefault;;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define R_EN_Pin       GPIO_PIN_0
#define R_EN_GPIO_Port GPIOF

#define BAT_NTC_AD_Pin GPIO_PIN_2
#define BAT_NTC_AD_GPIO_Port GPIOA
#define PCB_NTC_AD_Pin GPIO_PIN_3
#define PCB_NTC_AD_GPIO_Port GPIOA
#define OUT_AD1_Pin GPIO_PIN_4
#define OUT_AD1_GPIO_Port GPIOA
#define OUT_AD2_Pin GPIO_PIN_5
#define OUT_AD2_GPIO_Port GPIOA
#define OUT_PWM_Pin GPIO_PIN_6
#define OUT_PWM_GPIO_Port GPIOA
#define VIN_AD_Pin GPIO_PIN_7
#define VIN_AD_GPIO_Port GPIOA
#define OUT_AD3_Pin GPIO_PIN_0
#define OUT_AD3_GPIO_Port GPIOB
#define BAT_AD_Pin GPIO_PIN_1
#define BAT_AD_GPIO_Port GPIOB
#define P3V_EN_Pin GPIO_PIN_2
#define P3V_EN_GPIO_Port GPIOB


#define SC8913_SCL_Pin GPIO_PIN_10
#define SC8913_SCL_GPIO_Port GPIOB
#define SC8913_SDA_Pin GPIO_PIN_11
#define SC8913_SDA_GPIO_Port GPIOB

#define USB_EN_Pin           SC8913_SCL_Pin
#define USB_EN_GPIO_Port     SC8913_SCL_GPIO_Port
#define USB_DET_Pin          SC8913_SDA_Pin
#define USB_DET_GPIO_Port    SC8913_SDA_GPIO_Port


#define SC8913_EN_Pin GPIO_PIN_15
#define SC8913_EN_GPIO_Port GPIOB

#define OUTAD_EN_Pin GPIO_PIN_9
#define OUTAD_EN_GPIO_Port GPIOA

#define P9V_EN_Pin GPIO_PIN_12
#define P9V_EN_GPIO_Port GPIOA
#define oled_DC_Pin GPIO_PIN_15
#define oled_DC_GPIO_Port GPIOA
#define oled_RES_Pin GPIO_PIN_4
#define oled_RES_GPIO_Port GPIOB
#define oled_CS_Pin GPIO_PIN_8
#define oled_CS_GPIO_Port GPIOB
#define OUT_EN_Pin GPIO_PIN_9
#define OUT_EN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */


#define CHARGE_STAT_IS_TRUE      0  //PC15 0 表示充满电

extern void sys_displayDebug(uint16_t debug);
extern void sys_displayFloatDebug(float debug);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
