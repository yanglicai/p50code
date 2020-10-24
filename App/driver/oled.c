/**
  ******************************************************************************
  * @file    oled.c 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   
	
注意：屏幕是从中间截取的居中的部分；
所以Y坐标的offset：(132 - 80) /2
	
被谁调用：
gui.c 实现具体的字 图标等显示

实现的功能：
物理层：
1.端口操作宏定义
2.基本通信：iic或者spi根据芯片来确定

数据层：
1.基本写操作：写命令， 写数据
*/   
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/

#include "main.h" 

#include "delay.h"  
#include "gui.h"        

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/ 

#define oled_SDA_Pin GPIO_PIN_5
#define oled_SDA_GPIO_Port GPIOB
#define oled_SCL_Pin GPIO_PIN_3
#define oled_SCL_GPIO_Port GPIOB    

     
#define  SCK_H        HAL_GPIO_WritePin(oled_SCL_GPIO_Port, oled_SCL_Pin, GPIO_PIN_SET) 
#define  SCK_L        HAL_GPIO_WritePin(oled_SCL_GPIO_Port, oled_SCL_Pin, GPIO_PIN_RESET) 

#define  MOSI_H       HAL_GPIO_WritePin(oled_SDA_GPIO_Port, oled_SDA_Pin, GPIO_PIN_SET)      
#define  MOSI_L       HAL_GPIO_WritePin(oled_SDA_GPIO_Port, oled_SDA_Pin, GPIO_PIN_RESET) 

#define  RES_H        HAL_GPIO_WritePin(oled_RES_GPIO_Port, oled_RES_Pin, GPIO_PIN_SET)    
#define  RES_L        HAL_GPIO_WritePin(oled_RES_GPIO_Port, oled_RES_Pin, GPIO_PIN_RESET) 

#define  LED_ON       HAL_GPIO_WritePin(P3V_EN_GPIO_Port, P3V_EN_Pin, GPIO_PIN_RESET) 
#define  LED_OFF      HAL_GPIO_WritePin(P3V_EN_GPIO_Port, P3V_EN_Pin, GPIO_PIN_SET)   


#define         SDIN_0()           MOSI_L
#define         SDIN_1()           MOSI_H

#define         SCLK_0()           SCK_L
#define         SCLK_1()           SCK_H    

#define         RES_0()            RES_L
#define         RES_1()            RES_H

#define         DC_0()             HAL_GPIO_WritePin(oled_DC_GPIO_Port, oled_DC_Pin, GPIO_PIN_RESET) 
#define         DC_1()             HAL_GPIO_WritePin(oled_DC_GPIO_Port, oled_DC_Pin, GPIO_PIN_SET) 

#define         CS_0()             HAL_GPIO_WritePin(oled_CS_GPIO_Port, oled_CS_Pin, GPIO_PIN_RESET) 
#define         CS_1()             HAL_GPIO_WritePin(oled_CS_GPIO_Port, oled_CS_Pin, GPIO_PIN_SET) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#define SPI_IS_SOFT   0
/* Exported functions ------------------------------------------------------- */

#if (SPI_IS_SOFT==0)
extern SPI_HandleTypeDef hspi1;
#endif

/**
  * @brief  oled spi 发送数据
  * @param  pTxData 
  * @retval None
*/

/*********************************************
函数名称:oled_writeCmd
函数说明:写命令字节.
*********************************************/
void oled_writeCmd(unsigned char Data)
{
	CS_0();	
	DC_0();
	
#if (SPI_IS_SOFT==1)
	
	unsigned char i;
	for (i=0; i<8; i++)
	{
		SCLK_0();
		if((Data & 0x80)==0x80)
		{
			 SDIN_1();  
		}
		else
		{
			 SDIN_0();  
		}	
		
		Data <<= 1;		
		SCLK_1();
		
	}
	
#else
	
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&Data,1,0xFFFFFF); 
	
#endif
	
	DC_1();
	CS_1();
	
}

/*********************************************
函数名称:oled_writeData
函数说明:写数据字节.
*********************************************/
void oled_writeData(unsigned char Data)
{
	CS_0();	
	DC_1();

#if (SPI_IS_SOFT==1)
	 unsigned char i;
	
	for (i=0; i<8; i++)
	{
		SCLK_0();	

		if((Data & 0x80)==0x80)
		{
			 SDIN_1();  
		}
		else
		{
			 SDIN_0();  
		}			
		
		Data <<= 1;		
		SCLK_1();
	}
	
#else
	
	HAL_SPI_Transmit(&hspi1,(uint8_t *)&Data,1,0xFFFFFF); 

#endif	
	
	DC_1();
	CS_1();
}

void oled_setWriteData(void)
{
	 CS_0();	
	 DC_1(); 
}

void oled_clrWriteData(void)
{
	 DC_1();
	 CS_1();
}

/*********************************************
函数名称:oled_writeContinueData
函数说明:写数据字节.连续写数据
*********************************************/
void oled_writeContinueData(unsigned char *buf,uint16_t size)
{
	HAL_SPI_Transmit(&hspi1,(uint8_t *)buf,size,0xFFFFFF); 	
} 

/*********************************************
函数名称:oled_writeFrame
函数说明:写一帧数据两个字节.
*********************************************/
void oled_writeFrame(unsigned char DatHbit,unsigned char DatLbit)		
{
	oled_writeData(DatHbit);
	oled_writeData(DatLbit);	
}
/*********************************************
函数名称:oled_setOriginalAddr
函数说明:设置原始坐标地址.
*********************************************/  
void oled_setOriginalAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	oled_writeCmd(0x2A);//设置列地址命令.
	
	oled_writeData(0x00);
	oled_writeData(x1);//前面两行开始列地址.
	
	oled_writeData(0x00);
	oled_writeData(x2);//后面两行结束列地址.
  //====================================//
	oled_writeCmd(0x2B);//设置行地址命令.
	
	oled_writeData(0x00);
	oled_writeData(y1);//前面两行开始行地址
	
	oled_writeData(0x00);
	oled_writeData(y2);//后面两行结束行地址
  //====================================//
	oled_writeCmd(0x2C);//写缓存. 
}

/*********************************************
函数名称:oled_setCoordinateAddr
函数说明:设置坐标地址. 添加了地址偏移的处理 
*********************************************/
void oled_setCoordinateAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{    
	oled_setOriginalAddr(LCD_X_S1+x1,LCD_X_S1+x2,LCD_Y_S1+y1,LCD_Y_S1+y2);  	
} 

/**
  * @brief  oled 复位 发送硬件复位信号
  * @param  data 
  * @param  None
  * @retval None
**/
void oled_clearScreen(void)
{
	 display_rgb(COLOR_BLACK);
}

/**
  * @brief  oled 复位 发送硬件复位信号
  * @param  data 
  * @param  None
  * @retval None
**/

void oled_reset(void)
{
	LED_ON;
	delay_ms_soft(10);
	
	RES_H;
	delay_ms_soft(10);
	RES_L;
	delay_ms_soft(10);
	RES_H;
	delay_ms_soft(10);
}

void oled_Off(void)
{
   oled_writeCmd(0x28);
   oled_writeCmd(0x10); 
	
	 HAL_SPI_DeInit(&hspi1);
}

void oled_On(void)
{
	oled_writeCmd(0x11); 
	oled_writeCmd(0x29);	
	LED_ON;
}

void oled_displayOff(void)
{
	oled_writeCmd(0x28);	
	LED_OFF;
}

void oled_displayOn(void)
{
	oled_writeCmd(0x29);	
	LED_ON;
}

void oled_DeInit(void)
{	 
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
//	DC_1();
//	CS_1();
//	RES_H;
//	SCK_H;
//	MOSI_H;//??????

	LED_OFF;	//---40mA
	
	GPIO_InitStruct.Pin = P3V_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //GPIO_MODE_OUTPUT_PP 电平一致，不用使用开漏来处理
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(P3V_EN_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = oled_SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(oled_SCL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = oled_SDA_Pin;
	HAL_GPIO_Init(oled_SDA_GPIO_Port, &GPIO_InitStruct);
}


