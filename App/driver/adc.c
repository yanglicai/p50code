/**
  ******************************************************************************
  * @file    adc.c 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   
  * @NOTE: 
  当前的电源电源是3.0V ；之前一般是3.3V来计算的
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/

#include "main.h" 
#include "adc.h"
#include <string.h>

extern ADC_HandleTypeDef hadc;
extern DMA_HandleTypeDef hdma_adc;

#define BAT_NTC_ON           HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_SET) 
#define BAT_NTC_OFF          HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_RESET)

#define PCB_NTC_ON           HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_SET) 
#define PCB_NTC_OFF          HAL_GPIO_WritePin(OUT_EN_GPIO_Port, OUT_EN_Pin, GPIO_PIN_RESET) 

#define   NTC_SERIES_RESISTOR  100 //K  
//tempRes = (float)((NTC_SERIES_RESISTOR) * (4096 - tempAD) / tempAD); 

volatile uint16_t  adc_DMAbuff[ADC_CHECK_CNT][ADC_CHANNEL_MAX]; //测量次数 通道编号  

/* Private define ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

 extern uint16_t sys_calAverage(uint16_t *array, uint8_t num);
 
 typedef struct
 {
	 uint16_t adBatV;
	 uint16_t adBatOutV;
	 uint16_t adBatT;
	 
	 uint16_t adOutV;
	 uint16_t adOutI;
	 uint16_t adOutT; 
	 
	 uint16_t adUsbV;
	 
 }_ADC_DATA;
 
 _ADC_DATA adcData;

/**
  * @brief  ADC 数据计算
  * @param  ADC_Channel 
  * @retval None
*/
static uint16_t adc_getChannellAdc(uint8_t ADC_Channel)
{
    uint8_t    i;
    uint16_t  temp;
    uint8_t   sel;
  
    uint16_t  adc_calBuff[ADC_CHECK_CNT];

    sel = ADC_Channel;      
      
    for (i=0;i < ADC_CHECK_CNT;i++)  //读取缓冲池中的数据
    {
      adc_calBuff[i] = adc_DMAbuff[i][sel];
    }	    

     temp = sys_calAverage(adc_calBuff,ADC_CHECK_CNT);  
  
     return temp;
}


/* Exported functions ------------------------------------------------------- */

/**
  * @brief  ADC 数据计算
  * @param  ADC_Channel 
  * @retval None
*/ 

/*******************************************
*函数名称：adc_init
配置DMA通道处理
HAL_ADC_Stop
********************************************/
void adc_init(void)
{	 
	memset((uint16_t*)adc_DMAbuff, 0, sizeof(adc_DMAbuff));
	
  if (HAL_ADC_Start_DMA(&hadc, (uint32_t*)&adc_DMAbuff[0][0], ADC_CHANNEL_MAX * ADC_CHECK_CNT) != HAL_OK)
	{
		/* Start Conversation Error */
		Error_Handler();
	}
	
	BAT_NTC_ON;
	PCB_NTC_ON;
} 

/*******************************************
函数名称：获取对应的ADC值
********************************************/
uint16_t adc_getBatV_ADC(void)
{	 
   adcData.adBatV = adc_getChannellAdc(ADC_BAT_VOL);  
   return adcData.adBatV;
}

uint16_t adc_getBatI_ADC(void) //NO USE
{	   
   return 1;
}

uint16_t adc_getBatTemp_ADC(void)
{	   
	 adcData.adBatT = adc_getChannellAdc(ADC_BAT_NTC_PA2); 
   return adcData.adBatT;
}

uint16_t adc_getOutV_ADC(void)
{	   
	 adcData.adOutV = adc_getChannellAdc(ADC_OUT_VOL); 	
   return adcData.adOutV;
}

uint16_t adc_getOutBatV_ADC(void)
{	   
	 adcData.adBatOutV = adc_getChannellAdc(ADC_OUT_BAT_V_PB0_AD3);
   return adcData.adBatOutV;
}


uint16_t adc_getOutI_ADC(void)
{	   
	 adcData.adOutI = adc_getChannellAdc(ADC_OUT_CURR); 
   return adcData.adOutI;
} 

uint16_t adc_getOutTemp_ADC(void)
{	  
   adcData.adOutT = adc_getChannellAdc(ADC_PCB_NTC_PA3); 	
   return adcData.adOutT;
}

uint16_t adc_getUsbV_ADC(void)
{	  
   adcData.adUsbV = adc_getChannellAdc(ADC_USB_VOL_PA7); 	
   return adcData.adUsbV;
}  

/*******************************************
*函数名称：获取对应的实际值
********************************************/

float adc_getBatV(void)
{	   
   return (float)(adcData.adBatV * BAT_V_SCAL); 
}  

float adc_getBatI(void) //NO USE
{	   
   return 9.90f;
}  

uint8_t adc_getBatTemp(void)
{	   
   return 25;
}

float adc_getOutV(void)
{	   
   return (float)(adcData.adOutV * OUT_V_SCAL); 
}

float adc_getOutI(void)
{	   
   return (float)(adcData.adOutI * OUT_I_SCAL); 
} 

float adc_getUsbV(void) 
{	   
   return (float)(adcData.adUsbV * USB_V_SCAL); 
}

uint8_t adc_getOutTemp(void)
{	   
   return 25;
}

extern void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);

void adc_off(void)
{
	BAT_NTC_OFF;
	PCB_NTC_OFF;
	
	HAL_ADC_Stop(&hadc);
	HAL_ADC_Stop_DMA(&hadc);
}


void adc_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin  = OUT_EN_Pin; //使用了同一个端口
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //GPIO_MODE_OUTPUT_OD GPIO_MODE_OUTPUT_PP
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUT_EN_GPIO_Port, &GPIO_InitStruct);

}


