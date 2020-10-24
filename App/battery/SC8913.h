#ifndef __SC8913_H__
#define __SC8913_H__

#include "stm32F0xx.h" //∫Õmcuœ‡πÿ 
#include "typedef.h"   

#define SC_VBAT_SET                 (0x00)//
#define SC_VBUSREF_I_SET            (0x01)
#define SC_VBUSREF_I_SET2           (0x02)
#define SC_VBUSREF_E_SET            (0x03)
#define SC_VBUSREF_E_SET2           (0x04)
#define SC_IBUS_LIM_SET             (0x05)
#define SC_IBAT_LIM_SET             (0x06)
#define SC_VINREG_SET               (0x07)
#define SC_RATIO                    (0x08)
#define SC_CTRL0_SET                (0x09)
#define SC_CTRL1_SET                (0x0A)
#define SC_CTRL2_SET                (0x0B)
#define SC_CTRL3_SET                (0x0C)
#define SC_VBUS_FB_VALUE            (0x0D)
#define SC_VBUS_FB_VALUE2           (0x0E)
#define SC_VBAT_FB_VALUE            (0x0F)
#define SC_VBAT_FB_VALUE2           (0x10)
#define SC_IBUS_VALUE               (0x11)
#define SC_IBUS_VALUE2              (0x12)
#define SC_IBAT_VALUE               (0x13)
#define SC_IBAT_VALUE2              (0x14)
#define SC_ADIN_VALUE               (0x15)
#define SC_ADIN_VALUE_2             (0x16)
#define SC_STATUS                   (0x17)
#define SC_RESERVED1                (0x18)
#define SC_MASK                     (0x19)
#define SC_DMCTRL                     (0x1A)
#define SC_DMREAD                     (0x1B)

typedef enum 
{
	QC_NULL,
	QC_START,
	QC_CHECK_DISCONNECT,
	QC_CHECK_04ISOK,
	QC_OK,
	QC_FAIL,
	QC_OVER,	
}_QC_STATUS;

extern _QC_STATUS qcStatus;



void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t  IIC_Wait_Ack(void);

uint8_t IIC_Read_Byte(unsigned char ack);
void IIC_Send_Byte(unsigned char txd);


#endif	//__24CXX_H__
