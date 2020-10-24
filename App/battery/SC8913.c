/**
  ******************************************************************************
  * @file    sc8913.c 
  * @author  figo
  * @version V1.0.0
  * @date    20-12-2019
  * @brief   
	
依赖的文件：
sc8913.c 根据器件地址做具体的操作

实现的功能：
1.开 关充电；
2.QC通信的处理；
*/

#include "SC8913.h"
#include "main.h" 
#include "delay.h"
#include "systick.h"
#include "battery.h"

//调试用
#include "gui.h"

#define SC_WRITE_ADD 0xE8
#define SC_READ_ADD 0xE9

#define I2C_SLAW SC_WRITE_ADD	/*器件地址选择及写标志*/
#define I2C_SLAR SC_READ_ADD	/*器件地址选择及读标志*/  

uint8_t sc_i2c_buffer[28]={0x01,0x31,0xc0,0x7c,0xc0,0xff,0xff,0x2c,0x38,0x04,0x01,0x01,0x02,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static uint8_t SC8913_readByte(uint16_t u16Addr);   
static void SC8913_writeByte(uint16_t u16Addr,uint8_t pDat);  
void SC_Control_GPO(uint8_t On_Off);
static void SC_Control_ADC(uint8_t select);

static uint16_t SC_Read_BatteryVoltage(void);
static uint16_t SC_Read_BatteryCurrent(void);
static uint16_t SC_Read_VbusVoltage(void);


void SC_Charge_SDP_OR_CDP(void);


volatile uint8_t readByte;
volatile uint16_t chargeHalfWord;
static volatile uint8_t flagQcIsOk;
_QC_STATUS qcStatus;

#define SC_I2C_Write SC8913_writeByte
#define I2C_ReadReg  SC8913_readByte

void SC8913_init(void);

/*
协议过程
DP --> D-
DM --> D+

1.将D+和D-短接,重置1.25秒定时器.
2.检测D+上电压,
如果此电压超过0.325V小于2.0V超过1.25S,进入下一步,
否则重置1.25秒定时器.
3.断开D+,D-连接,由于D+和D-断开,所以D-上的电压不再跟随D+上的电压0.325V变动,此时开始下降;
4.检测D+,D-电压以修改输出电压值.
DM     DP
D+     D-      Output
0.6V    0.6V    12V
3.3V    0.6V    9V
3.3V    3.3V    20V
0.6     GND     5V(default)
注: 所有0.6V代表0.325-2.000V, 所有3.3V代表大于2.000V, 在此范围内即可正确申请QC 2.0握手协议.
*/

void SC8913_set_DP_DM_QC_12V(void)
{	
	SC_I2C_Write(SC_DMCTRL,0x46);
	readByte = 0x00;
	readByte = SC8913_readByte(SC_DMREAD);
	if (readByte==0x07)
	{
		systick_delay_ms(100);
		readByte = 0x00;
		readByte = SC8913_readByte(SC_DMREAD);
		if (readByte==0x07)
		{
			systick_delay_ms(100);
		}
	}
}

uint8_t SC8913_set_DP_DM_QC_9V(void)
{
	SC_I2C_Write(SC_DMCTRL,0x64);
	
	systick_delay_ms(100);
	
	chargeHalfWord = SC_Read_VbusVoltage();
	
	if (chargeHalfWord < 8300)
	{
		systick_delay_ms(100);
		
		chargeHalfWord = SC_Read_VbusVoltage();
		
		if (chargeHalfWord>8300)
		{
			return 1;			
		}
		else
		{
			return 0;
		}
	}
	
	return 1;
}

void SC8913_setInput9V(void)
{		
		readByte = SC8913_readByte(SC_VINREG_SET);
	
	  if (readByte != 85) //set 9V
		{
		  SC_I2C_Write(SC_VINREG_SET,85); //9V
      SC_I2C_Write(SC_DMCTRL,0x00);			
		}
		
		readByte = SC8913_readByte(SC_DMREAD);		
		
	  //预期：DM 是TX 设置为高 DP是RX 设置为低
		
		delay_ms_soft(10);
}



//10ms调用1次
uint8_t SC8913_QCswitch(void)
{
	static uint16_t delayCnt;
	uint8_t ret = 1;
	
  switch((uint8_t)qcStatus)
	{		
		case QC_NULL:
			
			SC_I2C_Write(SC_DMCTRL,0x48);
		  qcStatus = QC_START;
		  bat_chargePause();
		  delayCnt = 20;
			break;
		
		case QC_START:			
		  
		  if (delayCnt)
			{
				delayCnt--;					   			
			}
			else
			{
				readByte = 0x00;
				readByte = SC8913_readByte(SC_DMREAD);
				if (readByte==0x05)
				{
					SC_I2C_Write(SC_DMCTRL,0x84);
					qcStatus = QC_CHECK_DISCONNECT;
					delayCnt = 10;
				}	
				else
				{
					qcStatus = QC_FAIL;
				}
			}					
		  
			break;
		
		case QC_CHECK_DISCONNECT:
			
		  if (delayCnt)
			{
				delayCnt--;					 			
			}
			else
			{
				readByte = 0x00;
				readByte = SC8913_readByte(SC_DMREAD);
				
				if (readByte==0x05)
				{
					SC_I2C_Write(SC_DMCTRL,0x48);
					qcStatus = QC_CHECK_04ISOK;
					delayCnt = 200;
				}	
				else
				{
					qcStatus = QC_FAIL;
				}	
			}
			
			break;
		
		case QC_CHECK_04ISOK:
			
		   if (delayCnt)
			 {
				delayCnt--;					 			
			 }
			 
			 if (delayCnt%10==0)
			 {
				readByte = 0x00;
				readByte = SC8913_readByte(SC_DMREAD);
				
				if (readByte==0x04)
				{
					SC_I2C_Write(SC_DMCTRL,0x48);
					qcStatus = QC_OK;
					delayCnt = 100;
				}	
			}
			 
			if (delayCnt==0)
			{
				qcStatus = QC_FAIL;
			}	
			
			break;
		
		case QC_OK:
			
		   if (delayCnt)
			 {
				 delayCnt--;					 			
			 }
			 else
			 {
					SC_I2C_Write(SC_VINREG_SET,80); //9V
					systick_delay_ms(100);
				 
					if (SC8913_set_DP_DM_QC_9V()==1)
					{						
						bat_chargeRecover();
						flagQcIsOk = 1;
						bat_setChargeType(CHARGE_IS_FAST);
						qcStatus = QC_OVER; 
					}
					else
					{
						qcStatus = QC_FAIL;
					}
			 }
		
			break;
		
		case QC_FAIL:
			
			bat_chargeRecover();
		  SC_Charge_SDP_OR_CDP();		
			flagQcIsOk = 2;				  
		  ret = 0;
		
			break;
		
		case QC_OVER:
			
			ret = 1;
			break;
	}
	
	return ret;	
}

uint8_t SC8913_checkOffIsEn(void)
{
	chargeHalfWord = SC_Read_BatteryCurrent();
	
	if (chargeHalfWord < 200)
	{
		return 1;
	}
	
	return 0;
		
}

uint16_t SC8913_getChargeBatV(void)
{
	chargeHalfWord = SC_Read_BatteryVoltage();
	
	return chargeHalfWord;		
}

uint8_t SC8913_chargeManage(uint8_t type)
{		
	if (type==0)
	{			
		SC8913_init();
		
		bat_chargeStart();		
		
		SC_Control_ADC(1);
		
		bat_setChargeType(CHARGE_IS_SLOW);
		
		flagQcIsOk = 0;		
		
		return 0;
	}
	else if (type==1)
	{		
		if (flagQcIsOk==2) return 0;
		
		if (flagQcIsOk==1) return 1;		
		
		return SC8913_QCswitch();
	}		

	
	return 1;
}
	

void SC_Charge_SDP_OR_CDP(void) //普通充电配置
{ 
	SC_Control_GPO(0);
	SC_I2C_Write(SC_DMCTRL,0x00);//写寄存器1AH=00H,将DP、DM浮空
	SC_I2C_Write(SC_VINREG_SET,42);//设置Vinreg=4.5V，否则无法充电
}

//对外调用 
void SC8913_init(void)
{   
    SC8913_writeByte(SC_VBAT_SET,0x81); //set battery to 1CELL + 20moh + 4.20Vresistor 0x41--20moh 0x81 40 moh
	  SC8913_writeByte(SC_CTRL0_SET,0x24); 	
    
    readByte = sc_i2c_buffer[SC_CTRL3_SET];
    SC8913_writeByte(SC_CTRL3_SET,readByte | 0x20 | 0x08 );  //bit5 = 1  Enable ADC,   bit 3 =1 enable loop//0C

    SC8913_writeByte(SC_IBAT_LIM_SET,0x54); //limit max battery current to 4.0A//电池端最大电流设置为4.0A//06
    SC8913_writeByte(SC_RATIO,0x2A);  //default 5V output. IBATT RATIO = 3X,  VBUS 12.5X   //  
	
	  readByte = 0x00;
	  readByte = SC8913_readByte(SC_RATIO);

    SC8913_writeByte(SC_IBUS_LIM_SET,170);  //USB端的最大电流设置为0xFF--->3A 170--0xAA--2.0A;//05
	  readByte = 0x00;
	  readByte = SC8913_readByte(SC_IBUS_LIM_SET);

    SC_Charge_SDP_OR_CDP();	 
		
		readByte = 0x00;
	  readByte = SC8913_readByte(SC_STATUS);	

	  readByte = SC8913_readByte(SC_STATUS);	
		
		qcStatus = QC_NULL;
}

void SC8913_DeInit(void)
{   
	SC_Control_GPO(1); 
	
	SC_Control_ADC(0);
	
	SC_I2C_Write(SC_DMCTRL,0xFF); //0xFF 设置为0xFF和0x00 功耗没有变化
}


/*
 控制GPIO端口
*/

void SC_Control_GPO(uint8_t On_Off)
{
	uint8_t read_temp;

	read_temp = SC8913_readByte(SC_CTRL3_SET);

	if (On_Off)
	{
		SC8913_writeByte(SC_CTRL3_SET,read_temp & ~0x40);  //bit 6 = 0;
	}
	else
	{
		SC8913_writeByte(SC_CTRL3_SET,read_temp | 0x40);  //bit6 = 1
	}
}
/*
 控制ADC
*/
static void SC_Control_ADC(uint8_t select)
{    
	
    readByte = SC8913_readByte(SC_CTRL3_SET);
    
    if (select)
    {
        SC8913_writeByte(SC_CTRL3_SET,readByte | 0x20);  //bit5 = 1  Enable ADC
    }
    else
    {
        SC8913_writeByte(SC_CTRL3_SET,readByte & ~0x20);  //bit5 = 0  Disable ADC
    }
    
    readByte = SC8913_readByte(SC_CTRL3_SET);      
}

/*
 读取电池电压
*/
static uint16_t SC_Read_BatteryVoltage(void)
{
    uint8_t read_temp1;
    uint8_t read_temp2;
    uint16_t voltage;
    
    read_temp1 = SC8913_readByte(SC_VBAT_FB_VALUE);
    read_temp2 = SC8913_readByte(SC_VBAT_FB_VALUE2);
   
    voltage = (read_temp1 * 4 + (read_temp2 >> 6) + 1) * 10;
    
    return(voltage);
}

/*
 读取电池电流
*/
static uint16_t SC_Read_BatteryCurrent(void)
{
    uint8_t read_temp1;
    uint8_t read_temp2;
    uint16_t current;
    
    read_temp1 = SC8913_readByte(SC_IBAT_VALUE);
    read_temp2 = SC8913_readByte(SC_IBAT_VALUE2);
   
    current = read_temp1 << 2;
    current = current + (read_temp2 >> 6) + 1;
    current = current * 20;
      
    return(current);
}

/*
 读取总线电压
*/
static uint16_t SC_Read_VbusVoltage(void)
{
    uint8_t read_temp1;
    uint8_t read_temp2;
    uint16_t voltage;
    
    read_temp1 = SC8913_readByte(SC_VBUS_FB_VALUE);
    read_temp2 = SC8913_readByte(SC_VBUS_FB_VALUE2);
   
    voltage = (read_temp1 * 4 + (read_temp2 >> 6) + 1) * 25;
    
    return(voltage);
}
  
//完成，但没有进行测试

static void SC8913_writeByte(uint16_t u16Addr,uint8_t pDat)
{   	
    IIC_Start();  
    IIC_Send_Byte(I2C_SLAW);       
    IIC_Wait_Ack();   		
  
    IIC_Send_Byte(u16Addr);
    IIC_Wait_Ack(); 	 										  		   

    IIC_Send_Byte( pDat ); 
    IIC_Wait_Ack();  		    	   

    IIC_Stop();
    delay_ms_soft(10);	  
}  

static uint8_t SC8913_readByte(uint16_t u16Addr)
{
	  uint8_t tempData = 0xFF;
   		  	    																 
    IIC_Start();  

    IIC_Send_Byte(I2C_SLAW); 
    IIC_Wait_Ack(); 

    IIC_Send_Byte(u16Addr); 
    IIC_Wait_Ack();	    

    IIC_Start();  	 	   
    IIC_Send_Byte(I2C_SLAR); 
    IIC_Wait_Ack();	 

    tempData = IIC_Read_Byte(0);		
    
    IIC_Stop();
		
	  return tempData;
}

//=======================END OF FILES============================



