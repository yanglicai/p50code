/**
  ******************************************************************************
  * @file    oled.c 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   
	
ע�⣺��Ļ�Ǵ��м��ȡ�ľ��еĲ��֣�
����Y�����offset��(132 - 80) /2
	
��˭���ã�
gui.c ʵ�־������ ͼ�����ʾ

ʵ�ֵĹ��ܣ�
����㣺
1.�˿ڲ����궨��
2.����ͨ�ţ�iic����spi����оƬ��ȷ��

���ݲ㣺
1.����д������д��� д����
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
  * @brief  oled spi ��������
  * @param  pTxData 
  * @retval None
*/

/*********************************************
��������:oled_writeCmd
����˵��:д�����ֽ�.
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
��������:oled_writeData
����˵��:д�����ֽ�.
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
��������:oled_writeContinueData
����˵��:д�����ֽ�.����д����
*********************************************/
void oled_writeContinueData(unsigned char *buf,uint16_t size)
{
	HAL_SPI_Transmit(&hspi1,(uint8_t *)buf,size,0xFFFFFF); 	
} 

/*********************************************
��������:oled_writeFrame
����˵��:дһ֡���������ֽ�.
*********************************************/
void oled_writeFrame(unsigned char DatHbit,unsigned char DatLbit)		
{
	oled_writeData(DatHbit);
	oled_writeData(DatLbit);	
}
/*********************************************
��������:oled_setOriginalAddr
����˵��:����ԭʼ�����ַ.
*********************************************/  
void oled_setOriginalAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
	oled_writeCmd(0x2A);//�����е�ַ����.
	
	oled_writeData(0x00);
	oled_writeData(x1);//ǰ�����п�ʼ�е�ַ.
	
	oled_writeData(0x00);
	oled_writeData(x2);//�������н����е�ַ.
  //====================================//
	oled_writeCmd(0x2B);//�����е�ַ����.
	
	oled_writeData(0x00);
	oled_writeData(y1);//ǰ�����п�ʼ�е�ַ
	
	oled_writeData(0x00);
	oled_writeData(y2);//�������н����е�ַ
  //====================================//
	oled_writeCmd(0x2C);//д����. 
}

/*********************************************
��������:oled_setCoordinateAddr
����˵��:���������ַ. ����˵�ַƫ�ƵĴ��� 
*********************************************/
void oled_setCoordinateAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{    
	oled_setOriginalAddr(LCD_X_S1+x1,LCD_X_S1+x2,LCD_Y_S1+y1,LCD_Y_S1+y2);  	
} 

/**
  * @brief  oled ��λ ����Ӳ����λ�ź�
  * @param  data 
  * @param  None
  * @retval None
**/
void oled_clearScreen(void)
{
	 display_rgb(COLOR_BLACK);
}

/**
  * @brief  oled ��λ ����Ӳ����λ�ź�
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
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //GPIO_MODE_OUTPUT_PP ��ƽһ�£�����ʹ�ÿ�©������
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


