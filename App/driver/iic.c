/**
  ******************************************************************************
  * @file    iic.c 
  * @author  figo
  * @version V1.0.0
  * @date    20-12-2019
  * @brief   
	
被谁调用：
sc8913.c 根据器件地址做具体的操作

实现的功能：
物理层：
1.端口操作宏定义
2.基本通信：iic或者spi根据芯片来确定

数据层：
1.基本写操作：写命令， 写数据  	

*/

#include "sc8913.h"  //根据需求来设置

#include "main.h" 
#include "delay.h"

void IIC_SDA_0(void);
void IIC_SDA_1(void);

void IIC_SCL_0(void);
void IIC_SCL_1(void);

//SC8913 端口配置
//其他不变， GPIO_MODE_INPUT不做处理，功耗为300uA
//原本功耗为300uA左右，设置这里为OD SDA SCL设置为0后功耗降低为90uA
void IIC_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin = SC8913_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //GPIO_MODE_OUTPUT_OD GPIO_MODE_INPUT
  GPIO_InitStruct.Pull = GPIO_NOPULL;         //外部是上拉电阻
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SC8913_SDA_GPIO_Port, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin = SC8913_SCL_Pin;
  HAL_GPIO_Init(SC8913_SCL_GPIO_Port, &GPIO_InitStruct);
	
	IIC_SDA_0();
	IIC_SCL_0();
}
 
/**
  * @brief  Set SDA Pin as Output Mode
  * @retval None
  */
void SDA_OUT()  
{  
  GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin = SC8913_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SC8913_SDA_GPIO_Port, &GPIO_InitStruct);
}  
 
/**
  * @brief  Set SDA Pin as Input Mode
  * @retval None
  */
void SDA_IN()  
{  
  GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin = SC8913_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SC8913_SDA_GPIO_Port, &GPIO_InitStruct);
} 
 
/**
  * @brief  read input voltage from SDA pin
  * @retval None
  */
unsigned char SDA_READ()
{	
  return HAL_GPIO_ReadPin(SC8913_SDA_GPIO_Port,SC8913_SDA_Pin);
}
 
/**
  * @brief  output high form SDA pin
  * @retval None
  */
void IIC_SDA_1()
{
  HAL_GPIO_WritePin(SC8913_SDA_GPIO_Port, SC8913_SDA_Pin, GPIO_PIN_SET);
}
 
/**
  * @brief  output low form SDA pin
  * @retval None
  */
void IIC_SDA_0()
{
  HAL_GPIO_WritePin(SC8913_SDA_GPIO_Port, SC8913_SDA_Pin, GPIO_PIN_RESET);
}
 
/**
  * @brief  output high form SCL pin
  * @retval None
  */
void IIC_SCL_1()
{
  HAL_GPIO_WritePin(SC8913_SCL_GPIO_Port, SC8913_SCL_Pin, GPIO_PIN_SET); 
}
 
/**
  * @brief  output LOW form SCL pin
  * @retval None
  */
void IIC_SCL_0()
{
   HAL_GPIO_WritePin(SC8913_SCL_GPIO_Port, SC8913_SCL_Pin, GPIO_PIN_RESET); 
}

//==========================================================
 
 
/**
* @brief  Simulate IIC conmunication :Create Start signal
  * @retval None
  */
void IIC_Start(void)
{
	SDA_OUT();     //sda output
	IIC_SDA_1();	  	  
	IIC_SCL_1();
	delay_us_soft(4);
 	IIC_SDA_0();   //START:when CLK is high,DATA change form high to low 
	delay_us_soft(4);
	IIC_SCL_0();   //hold scl line, prepare to transmit data
}	  
 
/**
  * @brief  Simulate IIC conmunication : Create Stop signal
  * @retval None
  */
void IIC_Stop(void)
{
	SDA_OUT();    //sda output mode 
	IIC_SCL_0();
	IIC_SDA_0();  //STOP:when CLK is high DATA change form low to high
 	delay_us_soft(4);
	IIC_SCL_1(); 
	IIC_SDA_1();  //indicate transmit over
	delay_us_soft(4);							   	
}
 
/**
* @brief  Simulate IIC conmunication : wait for target device's ACK
* @retval ACK (0) : receive success
* @retval NACK(1) : receive unsuccess
  */
unsigned char IIC_Wait_Ack(void)
{
    unsigned char ucErrTime = 0;
  
    SDA_IN();      //set as input mode
    IIC_SDA_1();
    delay_us_soft(1);	   
    IIC_SCL_1();
    delay_us_soft(1);	 
    while(SDA_READ())
    {
      ucErrTime++;
      if(ucErrTime > 250)
      {
        IIC_Stop();
        return 1;
      }
    }
    IIC_SCL_0(); //release scl line
		
    return 0;  
} 
 
/**
  * @brief  Simulate IIC conmunication : make an ACK
  * @retval None
  */
void IIC_Ack(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_0();
	delay_us_soft(2);
	IIC_SCL_1();
	delay_us_soft(2);
	IIC_SCL_0();
}
 
/**
  * @brief  Simulate IIC conmunication : don't make an ACK
  * @retval None
  */
void IIC_NAck(void)
{
	IIC_SCL_0();
	SDA_OUT();
	IIC_SDA_1();
	delay_us_soft(2);
	IIC_SCL_1();
	delay_us_soft(2);
	IIC_SCL_0();
}					 				     
 
 
/**
  * @brief  Simulate IIC conmunication : Transmit one byte Data
  * @param  txd: data to be transmit
  * @retval None
  */
void IIC_Send_Byte(unsigned char txd)
{                        
  unsigned char i;   
  SDA_OUT(); 	    
  IIC_SCL_0();//push down scl  to start transmit data
  for(i = 0; i < 8; ++i)
  {              
    if(txd & 0x80)
    {
      IIC_SDA_1();
    }
    else
    {
      IIC_SDA_0();
    }
    txd <<= 1; 	  
    delay_us_soft(2);   
    IIC_SCL_1();
    delay_us_soft(2); 
    IIC_SCL_0();	
    delay_us_soft(2);
  }	 
} 	  
 

/**
  * @brief  Simulate IIC conmunication : Receive one byte Data
  * @param  ack: Whether transmit ACK
  * @retval the data have been receive
  */
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, res = 0;

    SDA_IN();               //SDA input mode

    for(i = 0; i < 8; ++i )
    {
        IIC_SCL_0(); 
        delay_us_soft(2);
        IIC_SCL_1();
        res <<= 1;
        if(SDA_READ())
        {
          res++; 
        }      
        delay_us_soft(1); 
    }
    
    if (!ack)
    {
        IIC_NAck();//make NACK
    }
    else
    {
        IIC_Ack(); //make ACK
    }
    return res;
}  



