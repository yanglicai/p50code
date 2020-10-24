/*
******************************************************************************
* @file    fastCharge_protocol.c
* @author  Figo
* @version V1.0.0
* @date    2019.07.06
* @brief
1.����Э���ʵ�֣�
2.ͨ�ŷ��͵Ŀ���ʵ�֣�
3.ͨ�Ž��յĽ��봦��
******************************************************************************

*/   

#include "fast_changed.h"

#include "math.h" 

/*********************************************
��������:fastCharge_handshake
����˵��:����ʵ�ֹ��� ��Ϊһ�������ĺ���������
*********************************************/
uint8_t fastCharge_handshake(void)
{
	uint8_t ret = 0xFF;

	switch (Uart_modle)
	{
	case UART_IDLE:

		ret = 0xF1;

		break;

	case BUILD_SHAKE_HANDS:

		//����������Ϣ ����ʱ������ȴ���ʱ �ȴ�����300ms����Ϊ��ʱ ���·��� 	
		fastCharge_sendCmd(SHAKE_HANDS_START);

		Uart_modle = WAIT_DEV_REPLY;

		break;

	case SHAKE_SEND_REPLY:

		fastCharge_sendCmd(REPLY_OK);

		Uart_modle = UART_IDLE;

		break;

	case SHAKE_HANDS_FAILURE:

		USB_HANDSHAKE_OK = 0;

		Uart_modle = UART_IDLE;

		ret = 0x00;

		break;

	case SHAKE_HANDS_SUCCESS:
		//������� 	
		fastCharge_sendCmd(SHAKE_HANDS_END);

		Uart_modle = SHAKE_HANDS_OVER;

		break;

	case WAIT_DEV_REPLY:

		ret = 0xF2;

		break;

	case SHAKE_HANDS_OVER:

		USB_HANDSHAKE_OK = 1;

		ret = 1;

		Uart_modle = UART_IDLE;

		break;

	default:

		break;
	}

	return ret;
}

/*********************************************
��������:fastCharge_startTx
����˵��:��������
*********************************************/
void fastCharge_startTx(void)
{
	uartCtrl.repeatCnt = 0;

	uartCtrl.index = 0;
  
  Uart_data.index = 0;

	switch (Uart_data.Sendbuf[0])
	{
	case SHAKE_HANDS_START:

		USB_NEES_ACK = 1;

		uartCtrl.txMaxDelay = TX_LONG_INTERVAL_TIME;

		uartCtrl.repeatCnt = 5;

		break;

	case SET_ADAPTER_VOLTAGE:

		USB_NEES_ACK = 1;

		if (uartCtrl.longIntervalFlag)
		{
			uartCtrl.txMaxDelay = TX_LONG_INTERVAL_TIME;
		}
		else
		{
			uartCtrl.txMaxDelay = TX_SHORT_INTERVAL_TIME;
		}

		uartCtrl.repeatCnt = 3;

		break;

	case SET_ADAPTER_CURRENT:
	case CLOSE_ADAPTER_VOLTAGE:

		USB_NEES_ACK = 1;

		uartCtrl.txMaxDelay = TX_SHORT_INTERVAL_TIME;

		uartCtrl.repeatCnt = 3;

		break;


	case READ_ADAPTER_CURRENT:
	case READ_ADAPTER_VOLTAGE:

		USB_NEES_ACK = 1;

		uartCtrl.txMaxDelay = TX_SHORT_INTERVAL_TIME;

		uartCtrl.repeatCnt = 3;

		break;

	default:

		USB_NEES_ACK = 0;

		UART_setRxDelay(0);

		break;
	}

	if (UART_getTxEnFlag())
	{
		fast_charge_sta = TX_START;
	}
	else
	{
		fast_charge_sta = TX_DELAY;
	}

	if (USB_HANDSHAKE_OK)
	{
		UART_setHeatBeatDelay(HEATBEAT_INTERVAL_TIME);   //1s֮��Ҫ���������ź�
	}
}


/*********************************************
��������:fastCharge_uartComManage
����˵��:���ͨ�Ź���
 fastCharge_uartComManage(); //10msִ��1��        
 fastCharge_handshake();//��������   
*********************************************/
void fastCharge_uartComManage(void)
{
	switch (fast_charge_sta)
	{
	case TX_START:

		UART_Send(Uart_data.Sendbuf, 5);

		if (USB_NEES_ACK)
		{
			if (uartCtrl.repeatCnt) uartCtrl.repeatCnt--;

			UART_setTxDelay(uartCtrl.txMaxDelay);

			fast_charge_sta = WAIT_ANSWER;

			USB_SEND_OK = 0;

			UART_setRxDelay(uartCtrl.txMaxDelay);
		}
		else
		{
			fast_charge_sta = TX_END;

			USB_SEND_OK = 1;

			UART_setRxDelay(0);
		}

		break;

	case TX_DELAY:

		if (UART_getTxEnFlag())
		{
			fast_charge_sta = TX_START;
		}

		break;

	case TX_REPEAT:

		if (uartCtrl.repeatCnt)
		{
			fast_charge_sta = TX_DELAY;
		}
		else
		{
			fast_charge_sta = TX_END;
		}

		break;

	case WAIT_ANSWER:


		break;


	case WAIT_OVERTIME:

		if (uartCtrl.repeatCnt == 0)
		{
			fast_charge_sta = TX_END;

			if (USB_NEES_ACK)
			{
				Uart_modle = SHAKE_HANDS_FAILURE;
			}
		}
		else
		{
			fast_charge_sta = TX_REPEAT;
		}

		break;

	case TX_END:

		USB_NEES_ACK = 0;

		break;

	default:

		break;

	}
}


/*********************************************
��������:fastCharge_revContinueData
//��������    81 90 01 8C 28     80 00 00 70 28
//���ڴ����ж��д���
*********************************************/
void fastCharge_revContinueData(void)
{
	if (UART_IsrFunction()) return;

	fastCharge_decodeMsg();
}

/*********************************************
��������:fastCharge_decodeMsg
����˵��:�����봦��  ��ȡ��Ļظ� ͨ�Żظ� ����������Ļظ�
*********************************************/
void fastCharge_decodeMsg(void)
{
	ENUMERATION_MODEL cmdMode;

	cmdMode = (ENUMERATION_MODEL)Uart_data.Rebuf[0];

	uint16_t dataTemp;

	switch (cmdMode)
	{
	case MAX_OUTPUT_CURRENT:    //�������߽�������	

		UART_setTxDelay(0);

		Uart_modle = SHAKE_SEND_REPLY;

		dataTemp = Uart_data.Rebuf[2] << 8;
		dataTemp |= Uart_data.Rebuf[1];

		Uart_data.Adapter_Max_Current = (double)(dataTemp * 0.05f);
	
	  if (uartCtrl.insertCnt < 100) uartCtrl.insertCnt = 100;

		break;

	case MAX_OUTPUT_VOLTAGE:

		UART_setTxDelay(0);

		Uart_modle = SHAKE_HANDS_SUCCESS;

		dataTemp = Uart_data.Rebuf[2] << 8;
		dataTemp |= Uart_data.Rebuf[1];

		Uart_data.Adapter_Max_Voltage = (double)(dataTemp * 0.02f);

		Uart_data.Adapter_Voltage = 5.0f;
	
	  if (uartCtrl.insertCnt < 100) uartCtrl.insertCnt = 100;

		break;

	case OUTPUT_CURRENT:    //���������Ƿ���ǰ�˶�·

		UART_setTxDelay(0);

		fastCharge_sendCmd(REPLY_OK);

		dataTemp = Uart_data.Rebuf[2] << 8;
		dataTemp |= Uart_data.Rebuf[1];

		Uart_data.Adapter_Current = (double)(dataTemp * 0.05f);

		uartCtrl.heatBeatCnt++;

		break;

	case OUTPUT_VOLTAGE:     //�����ο���������ֵ

		UART_setTxDelay(0);

		fastCharge_sendCmd(REPLY_OK);

		dataTemp = Uart_data.Rebuf[2] << 8;
		dataTemp |= Uart_data.Rebuf[1];

		Uart_data.Adapter_V_data = dataTemp;

		Uart_data.Adapter_Voltage = (double)(dataTemp * 0.02f);

		uartCtrl.heatBeatCnt++;

		break;

	case ADAPTER_REPLY_OK:

		USB_SEND_OK = 1;

		USB_NEES_ACK = 0;

		uartCtrl.repeatCnt = 0;

		fast_charge_sta = TX_END;	//�յ���ȷ�ظ� ����Ϊ���ͳɹ�		

		if (WAIT_DEV_REPLY == Uart_modle)
		{
			Uart_modle = UART_IDLE;
		}

		break;

	case ADAPTER_NO_ANSWER: //����ΪREPLY_FAIL����

		Uart_modle = BUILD_SHAKE_HANDS; //ʧ��������������������

		USB_HANDSHAKE_OK = 0;

		break;

	default:

		break;
	}
}

/*********************************************
��������: uint8_t fastCharge_heartBeat
����˵��: ���ֳɹ��� ��ʱ1S����500MS����1������
*********************************************/
uint8_t fastCharge_heartBeat(void)
{
	
	fastCharge_sendCmd(READ_ADAPTER_CURRENT);

	return 0;
}

/*********************************************
��������: fastCharge_adjustI
�������ڣ�ʵ����ͨ�����ڵ�ѹ��ʵ�ֵ�������
���յ������20MR������
*********************************************/

uint8_t fastCharge_adjustI(uint16_t adj)
{	

	return 0;
}





//===========================END OF FILES============================================
