/*
******************************************************************************
* @file    fastCharge_protocol.c
* @author  Figo
* @version V1.0.0
* @date    2019.07.06
* @brief
1.握手协议的实现；
2.通信发送的控制实现；
3.通信接收的解码处理；
******************************************************************************

*/   

#include "fast_changed.h"

#include "math.h" 

/*********************************************
函数名称:fastCharge_handshake
函数说明:握手实现过程 作为一个单独的函数来处理
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

		//发送握手信息 发送时，清零等待计时 等待超过300ms则视为超时 重新发送 	
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
		//完成握手 	
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
函数名称:fastCharge_startTx
函数说明:启动发送
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
		UART_setHeatBeatDelay(HEATBEAT_INTERVAL_TIME);   //1s之内要发送心跳信号
	}
}


/*********************************************
函数名称:fastCharge_uartComManage
函数说明:快充通信管理
 fastCharge_uartComManage(); //10ms执行1次        
 fastCharge_handshake();//握手任务   
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
函数名称:fastCharge_revContinueData
//测试数据    81 90 01 8C 28     80 00 00 70 28
//放在串口中断中处理
*********************************************/
void fastCharge_revContinueData(void)
{
	if (UART_IsrFunction()) return;

	fastCharge_decodeMsg();
}

/*********************************************
函数名称:fastCharge_decodeMsg
函数说明:快充解码处理  读取类的回复 通信回复 适配器自身的回复
*********************************************/
void fastCharge_decodeMsg(void)
{
	ENUMERATION_MODEL cmdMode;

	cmdMode = (ENUMERATION_MODEL)Uart_data.Rebuf[0];

	uint16_t dataTemp;

	switch (cmdMode)
	{
	case MAX_OUTPUT_CURRENT:    //用来做边界限制用	

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

	case OUTPUT_CURRENT:    //用来处理是否有前端短路

		UART_setTxDelay(0);

		fastCharge_sendCmd(REPLY_OK);

		dataTemp = Uart_data.Rebuf[2] << 8;
		dataTemp |= Uart_data.Rebuf[1];

		Uart_data.Adapter_Current = (double)(dataTemp * 0.05f);

		uartCtrl.heatBeatCnt++;

		break;

	case OUTPUT_VOLTAGE:     //用来参考计算线阻值

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

		fast_charge_sta = TX_END;	//收到正确回复 则认为发送成功		

		if (WAIT_DEV_REPLY == Uart_modle)
		{
			Uart_modle = UART_IDLE;
		}

		break;

	case ADAPTER_NO_ANSWER: //命名为REPLY_FAIL更好

		Uart_modle = BUILD_SHAKE_HANDS; //失败则重新握手启动处理

		USB_HANDSHAKE_OK = 0;

		break;

	default:

		break;
	}
}

/*********************************************
函数名称: uint8_t fastCharge_heartBeat
函数说明: 握手成功后 定时1S或者500MS发送1次数据
*********************************************/
uint8_t fastCharge_heartBeat(void)
{
	
	fastCharge_sendCmd(READ_ADAPTER_CURRENT);

	return 0;
}

/*********************************************
函数名称: fastCharge_adjustI
电流调节：实际是通过调节电压来实现电流调节
按照电池内阻20MR来处理
*********************************************/

uint8_t fastCharge_adjustI(uint16_t adj)
{	

	return 0;
}





//===========================END OF FILES============================================
