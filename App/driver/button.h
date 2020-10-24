/**
  ******************************************************************************
  * @file    button.h 
  * @author  Leo Ou
  * @version V1.1.0
  * @date    06-11-2018
  * @brief  
1. 2019 10 30 figo �����жϺ����������ṹ������ƣ��������� 
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __button_H
#define __button_H

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
#include "stm32f0xx_hal.h"
/* IO define ----------------------------------------------------------------*/
#define TOUCH_VDD_Pin          GPIO_PIN_1
#define TOUCH_VDD_GPIO_Port    GPIOF
#define TOUCH_K2DEC_Pin        GPIO_PIN_12
#define TOUCH_K2DEC_GPIO_Port  GPIOB
#define TOUCH_K3ADD_Pin        GPIO_PIN_13
#define TOUCH_K3ADD_GPIO_Port  GPIOB
#define TOUCH_K4_CFM_Pin GPIO_PIN_8
#define TOUCH_K4_CFM_GPIO_Port GPIOA

#define KEY_FIRE_Pin           GPIO_PIN_14
#define KEY_FIRE_GPIO_Port     GPIOB

#define T_LED_EN_Pin           GPIO_PIN_13
#define T_LED_EN_GPIO_Port     GPIOC

/* Exported types ------------------------------------------------------------*/
typedef enum {

  button_idle = 0,					/**< �������� */
  button_press,						/**< �������� */
  button_short_press,				/**< �̰� */
  button_long_press,				/**< ���� */
  button_3_press,					/**< ����3�� */
	button_4_press,					/**< ����3�� */
  button_5_press,					/**< ����5�� */

} button_state_t;



typedef struct {

  button_state_t state;				/**< ����״̬ */
  uint16_t press_count;				/**< ���¼�ʱ */
  uint16_t loosen_count;			/**< �ɿ���ʱ */
  uint16_t short_count;				/**< �̰����� */
	uint8_t  (*pressFun)(void);         /**< �̰����� */
	void  (*silderFun)(void);         /**< �������� */

} button_para_t;



/* Exported constants --------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Exported macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern button_para_t button_fire;
extern button_para_t button_up;
extern button_para_t button_dn;
extern button_para_t button_cfm;

/* Exported functions ------------------------------------------------------- */


void button_Init(void);
void button_Scan(void);
void button_Up_Detect(void);   
void button_Dn_Detect(void);
void button_Cfm_Detect(void);
void button_Fire_Detect(void);
extern uint8_t key_combinationFuction(void);
extern void keyApp_silderInit(void);

void button_on(void);
void button_off(void);
void button_DeInit(void);
extern void button_ledOn(void);
extern void button_ledOff(void);

uint8_t button_Fire_Press(void);
uint8_t button_Up_Press(void);
uint8_t button_Dn_Press(void);  
uint8_t button_Cfm_Press(void);	

void keyApp_silderCheckAdd(void);
void keyApp_silderCheckCfm(void);
void keyApp_silderCheckDec(void);
void keyApp_silderClear(void);

uint8_t keyApp_getAddDecStatus(void);
uint8_t keyApp_getFireStatus(void);


#endif


