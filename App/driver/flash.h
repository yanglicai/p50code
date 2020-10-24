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

#define   FLASH_SAVE_ADDR         0X0801F800 	//�洢��ַ 62

#define   FLASH_DATA_MAXNUM       64          //�����

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1        //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define   STM32_FLASH_BASE    0x08000000 	//STM32 FLASH����ʼ��ַ  

extern uint16_t page_buffer[64];


//FLASH������ֵ
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		  //��������  
void STMFLASH_WriteLenByte(uint32_t WriteAddr, uint32_t DataToWrite, uint16_t  Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
uint32_t  STMFLASH_ReadLenByte(uint32_t ReadAddr, uint16_t  Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(uint32_t WriteAddr, uint16_t  *pBuffer, uint16_t  NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr, uint16_t  *pBuffer, uint16_t  NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

void Flash_Read_Page(uint16_t *pBuff,uint16_t size);
void Flash_Write_Page(uint16_t *pBuff,uint16_t size);


#endif 


