/**
  ******************************************************************************
  * @file    oled.h 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OLED_H
#define __OLED_H

/* Includes ------------------------------------------------------------------*/


/* IO define ----------------------------------------------------------------*/

#include "typedef.h"

/* Private define ------------------------------------------------------------*/


//#define oled_VDD_ON     P11 = 1
//#define oled_VDD_OFF    P11 = 0


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */




void oled_Init(const uint8_t displayType);

void oled_Show_Clear(uint8_t type);
void oled_Show_Puff(uint16_t puff);
void oled_Show_Time(uint8_t time);
void oled_Show_Power(uint16_t power);
void oled_showHomePageBattery(uint8_t level);
void oled_showChargeBattery(uint8_t level);
void oled_Show_AtoRes(uint16_t res);
void oled_showMessaggeAtoRes(uint16_t res);


void oled_Show_BackUI(void);
void oled_Show_Snowwolf(void);  /* L1 SNOWWOLF 开机*/
void oled_Show_Version(uint8_t x,uint8_t y); /* 软件版本*/
void oled_Show_NewVersion(uint8_t x,uint8_t y);
void oled_Show_SysOff(void);  /* L2 BYE 关机*/
void oled_Show_OverHeat(void);  /* L3 OVER HEAT 过热*/
void oled_Show_CheckPod(void);  /* L4 CHECK POD 空载 */
void oled_Show_OverTime(void);  /* L5 OVER FIRE 超时*/
void oled_Show_BatteryLow(void);  /* L6 BATTERY LOW 低电量*/
void oled_Show_Short(void);         /* L7 SHORT 短路*/
void oled_Show_Locked(void);        /* L8 LOCKED 上锁*/
void oled_Show_Unlock(void);        /* L9 UNLOCKED 解锁*/
void oled_Show_RestoreDefault(void); /* L10 FACTORY 出厂设置*/
void oled_Show_PowerIsMax(void); /* L11 MAX POWER 已经达到最大功率*/



#endif /*  */


