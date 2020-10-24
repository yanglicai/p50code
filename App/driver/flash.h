/**
  ******************************************************************************
  * @file    flash.h 
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   flash program body
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f0xx.h"

#define   FLASH_SAVE_ADDR         0X0801F800 	//存储地址 62

#define   FLASH_DATA_MAXNUM       64          //最大编号

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1        //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define   STM32_FLASH_BASE    0x08000000 	//STM32 FLASH的起始地址  

extern uint16_t page_buffer[64];


//FLASH解锁键值
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  //读出半字  
void STMFLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t  Len);	//指定地址开始写入指定长度的数据
uint32_t  STMFLASH_ReadLenByte(uint32_t ReadAddr, uint16_t  Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(uint32_t WriteAddr, uint16_t  *pBuffer, uint16_t  NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr, uint16_t  *pBuffer, uint16_t  NumToRead);   		//从指定地址开始读出指定长度的数据

void Flash_Read_Page(uint16_t *pBuff,uint16_t size);
void Flash_Write_Page(uint16_t *pBuff,uint16_t size);


#endif 


