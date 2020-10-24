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
void oled_Show_Snowwolf(void);  /* L1 SNOWWOLF ����*/
void oled_Show_Version(uint8_t x,uint8_t y); /* ����汾*/
void oled_Show_NewVersion(uint8_t x,uint8_t y);
void oled_Show_SysOff(void);  /* L2 BYE �ػ�*/
void oled_Show_OverHeat(void);  /* L3 OVER HEAT ����*/
void oled_Show_CheckPod(void);  /* L4 CHECK POD ���� */
void oled_Show_OverTime(void);  /* L5 OVER FIRE ��ʱ*/
void oled_Show_BatteryLow(void);  /* L6 BATTERY LOW �͵���*/
void oled_Show_Short(void);         /* L7 SHORT ��·*/
void oled_Show_Locked(void);        /* L8 LOCKED ����*/
void oled_Show_Unlock(void);        /* L9 UNLOCKED ����*/
void oled_Show_RestoreDefault(void); /* L10 FACTORY ��������*/
void oled_Show_PowerIsMax(void); /* L11 MAX POWER �Ѿ��ﵽ�����*/



#endif /*  */


