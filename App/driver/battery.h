/**
  ******************************************************************************
  * @file    battery.h 
  * @author  Figo 
  * @version V1.0.0
  * @date    11-10-2019
  * @brief   battery program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BATTERY_H
#define __BATTERY_H

/* Includes ------------------------------------------------------------------*/

/* IO define ----------------------------------------------------------------*/
#define BAT_CHARGE_CONNECT_GPIO_Pin GPIO_PIN_10
#define BAT_CHARGE_CONNECT_GPIO_Port GPIOA
#define USB_IN_Pin         GPIO_PIN_11
#define USB_IN_GPIO_Port   GPIOA
/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
#define CHARGE_IS_FAST  1
#define CHARGE_IS_SLOW  2

extern void bat_chargeInit(void);	
extern void bat_chargeDeInit(void);
extern void bat_chargeChangeLevel(void);
extern void bat_chargeSetDelay(unsigned char delayS);
extern unsigned char bat_chargeGetDelay(void);

extern unsigned char bat_getChargeStatus(void);
extern void bat_chargeRecover(void);
extern void bat_chargeStart(void);
extern void bat_chargeOff(void);
extern void bat_chargePause(void);
extern void bat_chargeTaskHandle(void);
extern unsigned char bat_getUsbStatusIsr(void);
extern void bat_getBatFactLevel(void);
extern unsigned short bat_getBatVol(void);

extern void bat_setChargeType(unsigned char type);
unsigned char bat_getChargeType(void);	 

#endif 


