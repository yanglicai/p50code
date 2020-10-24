/**
  ******************************************************************************
  * @file    pwm.h 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   pwm program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PWM_H
#define __PWM_H

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
/* IO define ----------------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

#define  PWM_MAX		78
#define  PWM_MIN		8														

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  BUCK_MODE=0,
  BOOST_MODE,
} out_mode_t;


typedef struct
{      
	
  uint16_t  period;				/**< PWW ���� */
	
  uint16_t  boost_pwm_duty;		/**< BOOST PWMռ�ձ� */
  uint16_t  back_pwm_duty;		/**< BACK PWMռ�ձ� */		
	
  uint16_t  back_set_duty0;		/**< BACK �趨ռ�ձ� */	
  uint16_t  back_set_duty1;		/**< BACK �趨ռ�ձ� */
  uint16_t  back_set_duty2;		/**< BACK �趨ռ�ձ� */
  uint16_t  back_set_duty3;		/**< BACK �趨ռ�ձ� */
	uint16_t  back_set_duty4;		/**< BACK �趨ռ�ձ� */
	
//  uint16_t  pwm_duty_Fine;	/**< ΢��PWMռ�ձ� */
//  uint16_t	dead;			/**< PWM���������� */
  uint16_t  pwm_max;			/**< pwm�����ֵ */
  uint16_t  pwm_min;			/**< pwm����Сֵ */

} pwm_data_t;


extern out_mode_t	out_mode;
extern pwm_data_t	pwm_data;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void smoke_outPutOff(void);
void smoke_outPutOn(void);
void Boost_On(void);
void Add_OutVoltage(void);
void Dec_OutVoltage(void);

extern void pwm_init(void);
extern void pwm_disable(void);
extern void pwm_enable(void);
extern void pwm_setDuty(uint32_t dutyNewValue);

#endif 


