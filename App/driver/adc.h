/**
  ******************************************************************************
  * @file    adc.h 
  * @author  figo 
  * @version V1.0.0
  * @date    24-12-2019
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H_
#define __ADC_H_

/* Includes ------------------------------------------------------------------*/

#include "typedef.h"
#include "main.h"

#define   BATTERY_MAX_LEVEL     20

#define  ADC_VREF               3.00f

#define    ADC_TYPE_IS_ATO        0
#define    ADC_TYPE_IS_COMMON     1

#define    ADC_CHECK_CNT        2
#define    ADC_CHANNEL_MAX      7  

#define   ADC_BAT_NTC_PA2            0 //PA2
#define   ADC_PCB_NTC_PA3            1 //PA3
#define   ADC_ATO_VOL_PA4_AD1        2 //PA4  OUT_AD1

#define   ADC_SHORT_VOL_PA5_AD2      3 //PA5  OUT_AD2
#define   ADC_USB_VOL_PA7            4 //PA7  
#define   ADC_OUT_BAT_V_PB0_AD3      5 //PB0 8 OUT_AD3
#define   ADC_BAT1_VOL_PB1           6 //PB1 9 

#define ADC_BAT_VOL           ADC_BAT1_VOL_PB1        /* 电池电压 */
#define ADC_OUT_VOL           ADC_ATO_VOL_PA4_AD1     /* 输出电压 */
#define ADC_OUT_CURR          ADC_SHORT_VOL_PA5_AD2   /* 输出电流 */



/* Exported define ------------------------------------------------------------*/
#define SHORT_CURR      (uint16_t)(15  * 0.005f * 4095 / ADC_VREF)  // 15 A 110
#define OPEN_CURR        4    // 00A 2 0.5A
#define MIN_CURR        (uint16_t)(1  * 0.005f * 4095 / ADC_VREF) //7

#define TEMPAD_70C       595
#define TEMPAD_65C       701
#define TEMPAD_60C       811
#define TEMPAD_55C       937
#define TEMPAD_50C       1120

#define TEMPAD_45C       1240
#define TEMPAD_42C       1347
#define TEMPAD_40C       1418


#define MAX_TEMP         TEMPAD_65C  
#define MIN_TEMP         TEMPAD_55C  

#define TEST50_TEMP      TEMPAD_50C 
#define TEST40_TEMP      TEMPAD_45C

#define OUT_MAX_AD      (uint16_t)(4.20f * 4095 / (2 * ADC_VREF))
#define BAT_V_SCAL      (float)((ADC_VREF * 2.0f * 1000) / 4096.0f ) 
	
#define OUT_VOL_SCAL    (uint16_t)(4095 / (ADC_VREF * 2))

#define BAT_OUTV_SCAL   (float)((ADC_VREF * 2.0f * 1000.0f) / 4096.0f ) 
	
#define OUT_V_SCAL      (float)((ADC_VREF * 2.0f * 1000.0f) / 4096.0f) 
#define OUT_I_SCAL	    (float)((ADC_VREF * 200.0f * 1000.0f) / 4096.0f) 
#define USB_V_SCAL      (float)((ADC_VREF * 5.0f) / 4096.0f) 


//======定义电池电压对应AD值=========
//Bat_ADC = Ubat*4095/2/Vref 根据实际基准电压设置

#define  BAT_V_REF          (2 * ADC_VREF)

#define  BATTER_V29         (uint16_t)(2.90f * 4095 / BAT_V_REF)
#define  BATTER_V27         (uint16_t)(2.70f * 4095 / BAT_V_REF)


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

extern void adc_init(void);    

uint16_t bat_getLevel(uint16_t bat_v);

uint16_t adc_getBatV_ADC(void); 
uint16_t adc_getBatI_ADC(void);  
uint16_t adc_getBatTemp_ADC(void);  

uint16_t adc_getOutI_ADC(void); 
uint16_t adc_getOutV_ADC(void);  
uint16_t adc_getOutBatV_ADC(void);
uint16_t adc_getOutTemp_ADC(void);  
uint16_t adc_getUsbV_ADC(void);

float adc_getBatV(void); 
float adc_getBatI(void); 
uint8_t adc_getBatTemp(void); 

float adc_getOutI(void); 
float adc_getOutV(void);  
float adc_getUsbV(void);
uint8_t adc_getOutTemp(void);   


#endif /*  */


