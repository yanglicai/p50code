/*
******************************************************************************
* @file    fast_changed.c
* @author  Figo
* @version V1.0.0
* @date    2019.07.06
* @brief
1.快充的初始化；
2.快充管理；普通充电和快充充电的处理；

******************************************************************************

*/

#include "fast_changed.h"

#include "math.h"  

void fastCharge_displayManage(void);
uint8_t fastCharge_batVolManange(void);

void fastCharge_startCharge(void);
uint8_t fastCharge_checkBatOverHeat(void);
void fastCharge_handShakeJudge(void);

uint8_t charge_checkDisconnect(void);
uint8_t charge_errHandle(void);

/*********************************************
函数说明:快充初始化   NORMAL_TYPE
*********************************************/
void fastCharge_Init(void)
{
	Uart_modle = UART_IDLE;
	fast_charge_sta = TX_END;
	adapter_type = IDLE_TYPE;

	userSelcet = CHARGE_COMMON;
	chargeShift = CHARGE_IDLE;
	chargeShiftBak = CHARGE_IDLE;

	uartCtrl.insertCnt = 0;
	uartCtrl.heatBeatCnt = 0;
	uartCtrl.uartRevCnt = 0;
	uartCtrl.uartTxCnt = 0;

	chargeCtrl.step = 0;
	chargeCtrl.chargeMaxITime = 0;
	chargeCtrl.charge2ATime = 0;
	chargeCtrl.errCnt = 0;
	chargeCtrl.currentIsZeroCnt = 0;
	chargeCtrl.overCnt = 0;
	chargeCtrl.normalCnt = 0;
	
	chargeCtrl.chargeFullDelay = 0;

	UART_setTxDelay(0);
	UART_setRxDelay(0);

	Flag21= 0;
	
	USB_BAT_CHARGEOVER = 0; //插入就清零 启动充电
	
	chargeCtrl.chargeDelay = 0;
  chargeCtrl.upCnt = 0;

}

/*********************************************
函数名称:charge_Manage
函数说明:充电管理 包括快充和普通充电
*********************************************/
volatile uint16_t shiftDelay;
extern float    adc_getUsbV(void);
extern uint16_t adc_getUsbV_ADC(void);

void charge_Manage(void)
{	
	switch (adapter_type)
	{
	case IDLE_TYPE: //空闲状态  		

		fastCharge_startCharge();

		if (chargeCtrl.step == 0x00)
		{
			chargeCtrl.step = 0x01;			
		}

		break;

	case NORMAL_TYPE:

		if (chargeShiftBak != chargeShift)
		{
			chargeShiftBak = chargeShift;					
		}
		
		if (USB_HANDSHAKE_OK==0)
	  {
			fastCharge_handShakeJudge();
	  } 

		fastCharge_checkBatOverHeat();

		fastCharge_batVolManange();

		break;

	case FAST_TYPE:   		

	  if (UART_getTxEnFlag()==0) return; 
	
    if (USB_SEND_OK==0) return;	
		
		USB_SEND_OK = 0; 
	
	  adc_getUsbV_ADC();
	
		chargeData.USB_V = adc_getUsbV();
	
	  if (chargeData.USB_V < 9.0f)
		{
			if (chargeData.USB_V < 8.0f)
			{
			  Uart_data.Set_Adapter_Voltage += 0.50f;
			}
			else
			{
				Uart_data.Set_Adapter_Voltage += 0.20f;
			}
			
			fastCharge_setAdapterV(Uart_data.Set_Adapter_Voltage);
		}

		break;

	case CHARGING_END:

		adapter_type = IDLE_TYPE;

		uartCtrl.insertCnt = 0;

		break;

	default:

		break;
	}
}

/********************************************************************************
函数名称:fastCharge_startCharge
函数说明:充电电压管理
*********************************************************************************/
void fastCharge_handShakeJudge(void)
{
	if (UART_getTxEnFlag() && USB_SEND_OK)
	{
		USB_SEND_OK = 0;

		fastCharge_setAdapterI(MAX_CHARGE_I);

		adapter_type = FAST_TYPE;
	}
}	

void fastCharge_startCharge(void)
{
	uartCtrl.insertCnt++;

	chargeShift = CHARGE_IC;

	if (uartCtrl.insertCnt > 10)  //100ms	
	{
		if (uartCtrl.insertCnt == 15) //150
		{						
			UART_ConfigInit();		
			
			Uart_modle = BUILD_SHAKE_HANDS; 	

			USB_HANDSHAKE_OK = 0;
		}

		if (USB_HANDSHAKE_OK)
		{
			 fastCharge_handShakeJudge();			 
		}
		else if (uartCtrl.insertCnt > 300)
		{
			adapter_type = NORMAL_TYPE;
			
			chargeShiftBak = CHARGE_IDLE;

			Uart_data.Adapter_V_data = 5.0f;			
			
			Uart_modle = BUILD_SHAKE_HANDS;  //进入普通模式后 再来1次握手处理
		}
		
		Uart_data.Set_Adapter_Voltage = 5.2f;
	}
}

/*********************************************
函数名称:fastCharge_pollFunction
函数说明:快充主函数  10ms
*********************************************/
void fastCharge_pollFunction(void)
{
	
}


/*********************************************
函数名称:
函数说明:快充直冲管理
*********************************************/
//0 充电电流管理

void fastCharge_currentAdjust(void)
{	 
	

}
//1   涓流充电
void fastCharge_trickleCharge(void)
{

	
}

//2 CC  恒流充电
void fastCharge_prerCC_Charge(void)
{
	
}

void fastCharge_CC_Charge(void)
{
	fastCharge_currentAdjust();

}

//3  CV  //恒压充电
void fastCharge_CV_Charge(void)
{
	
}

/*********************************************
函数说明:快充usb断开判断  在直充模式才需要判断
*********************************************/
extern uint8_t t_10msCnt;
extern uint8_t t_1secCnt;
extern uint8_t t_1MinCnt;

uint8_t charge_checkDisconnect(void)
{
	
	return 0;
}

/*********************************************
函数名称:charge_errHandle
函数说明:预留该函数 方便后续增加错误处理
*********************************************/

uint8_t charge_errHandle(void)
{

	return 0;
}

/*********************************************
函数名称:fastCharge_batVolManange
函数说明:充电电压管理   30秒1个周期
*********************************************/

uint8_t fastCharge_batVolManange(void)
{	
	return (uint8_t)USB_BAT_CHARGEOVER;
}


/*********************************************
函数名称:fastCharge_checkBatOverHeat
函数说明:充电电池温度检测
*********************************************/
uint8_t fastCharge_checkBatOverHeat(void)
{
   return 1;
}




//=======================和充电相关的显示 温度 电压等等管理

/******************************************************************
函数名称: void fastCharge_displayManage(void)
函数说明: 显示管理 ：电池电压低于6.2V时，充电时关闭显示 只显示触摸led，省电
显示主界面5秒后，切换到充电界面
***************************************************************/
void fastCharge_displayManage(void)
{

	
}

//=====================================涉及的算法处理


uint16_t sysCalRootMeanSquare(uint16_t *pData, uint8_t nNum)
{
 
    // 平方和
    uint32_t fSum = 0;
  
    for (uint8_t i=0; i<nNum; ++i)
    {
        fSum += pData[i] * pData[i];
    }
 
    // 平均, 开方
    return (uint16_t)(sqrt(fSum/nNum)); 
}




void fastCharge_measureVI(void)
{
	
}

void fastCharge_testBat(void)
{
  
}

void fastCharge_measureBatV(void)
{
  
}

//==============================END OF FILES=======================================================





