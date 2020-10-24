/*
******************************************************************************
* @file    fast_changed.c
* @author  Figo
* @version V1.0.0
* @date    2019.07.06
* @brief
1.���ĳ�ʼ����
2.��������ͨ���Ϳ����Ĵ���

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
����˵��:����ʼ��   NORMAL_TYPE
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
	
	USB_BAT_CHARGEOVER = 0; //��������� �������
	
	chargeCtrl.chargeDelay = 0;
  chargeCtrl.upCnt = 0;

}

/*********************************************
��������:charge_Manage
����˵��:������ ����������ͨ���
*********************************************/
volatile uint16_t shiftDelay;
extern float    adc_getUsbV(void);
extern uint16_t adc_getUsbV_ADC(void);

void charge_Manage(void)
{	
	switch (adapter_type)
	{
	case IDLE_TYPE: //����״̬  		

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
��������:fastCharge_startCharge
����˵��:����ѹ����
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
			
			Uart_modle = BUILD_SHAKE_HANDS;  //������ͨģʽ�� ����1�����ִ���
		}
		
		Uart_data.Set_Adapter_Voltage = 5.2f;
	}
}

/*********************************************
��������:fastCharge_pollFunction
����˵��:���������  10ms
*********************************************/
void fastCharge_pollFunction(void)
{
	
}


/*********************************************
��������:
����˵��:���ֱ�����
*********************************************/
//0 ����������

void fastCharge_currentAdjust(void)
{	 
	

}
//1   ������
void fastCharge_trickleCharge(void)
{

	
}

//2 CC  �������
void fastCharge_prerCC_Charge(void)
{
	
}

void fastCharge_CC_Charge(void)
{
	fastCharge_currentAdjust();

}

//3  CV  //��ѹ���
void fastCharge_CV_Charge(void)
{
	
}

/*********************************************
����˵��:���usb�Ͽ��ж�  ��ֱ��ģʽ����Ҫ�ж�
*********************************************/
extern uint8_t t_10msCnt;
extern uint8_t t_1secCnt;
extern uint8_t t_1MinCnt;

uint8_t charge_checkDisconnect(void)
{
	
	return 0;
}

/*********************************************
��������:charge_errHandle
����˵��:Ԥ���ú��� ����������Ӵ�����
*********************************************/

uint8_t charge_errHandle(void)
{

	return 0;
}

/*********************************************
��������:fastCharge_batVolManange
����˵��:����ѹ����   30��1������
*********************************************/

uint8_t fastCharge_batVolManange(void)
{	
	return (uint8_t)USB_BAT_CHARGEOVER;
}


/*********************************************
��������:fastCharge_checkBatOverHeat
����˵��:������¶ȼ��
*********************************************/
uint8_t fastCharge_checkBatOverHeat(void)
{
   return 1;
}




//=======================�ͳ����ص���ʾ �¶� ��ѹ�ȵȹ���

/******************************************************************
��������: void fastCharge_displayManage(void)
����˵��: ��ʾ���� ����ص�ѹ����6.2Vʱ�����ʱ�ر���ʾ ֻ��ʾ����led��ʡ��
��ʾ������5����л���������
***************************************************************/
void fastCharge_displayManage(void)
{

	
}

//=====================================�漰���㷨����


uint16_t sysCalRootMeanSquare(uint16_t *pData, uint8_t nNum)
{
 
    // ƽ����
    uint32_t fSum = 0;
  
    for (uint8_t i=0; i<nNum; ++i)
    {
        fSum += pData[i] * pData[i];
    }
 
    // ƽ��, ����
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





