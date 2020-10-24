/*
******************************************************************************
* @file    fastCharge_driver
* @author  Figo
* @version V1.0.0
* @date    2019.07.06
* @brief
//IP2326 停止充电：pdf设置为8.4V为恒压充电状态 实际只有8.2V就进入小电流充电
1.uart通信的实现和控制处理；
2.心跳通信的实现；
3.通信中所有时间的控制；

******************************************************************************

*/  

#include "myMath.h"
#include "fast_changed.h"   

#define USB_CHARGE_STOP_DELAY  30

uint8_t adcReadProtect; 


volatile BxSTR        _Flag20,_Flag21;

_CHARGE_DATA chargeData;    

UART_CTRL uartCtrl;

UART_DATA Uart_data;

CHARGE_CTRL chargeCtrl;

FAST_CHARGE_COM_STA  fast_charge_sta;

UART_ENUMERATION          Uart_enumeration;
ENUMERATION_MODEL         enumeration_model;
ENUMERATION_MODEL         last_time_enumeration_model;
UART_MODEL                Uart_modle;
UART_RECEIVE_WAIT         Uart_receive_wait;

FAST_CHANGE_CURRENT       fast_change_current;
FAST_CHANGE_MODEL         fast_change_model;
ADAPTER_TYPE              adapter_type;
CONSTANT_CURRENT          constant_current;
BATTERY_MODEL             Battery_mode;

USER_SELECT               userSelcet;
CHARGE_SHIFT              chargeShift, chargeShiftBak;


//物理层控制处理
/*********************************************
函数说明:充电关闭  //中断保护使用
*********************************************/
void charge_off(uint8_t offNum)
{

}

/*********************************************
函数说明:充电开启  //中断保护使用
*********************************************/
void charge_on(uint8_t onNum)
{

}


//=========================================================通用通信控制参数的设置================================

/********************************************
函数名称:     fastCharge_1msTaskIsr
中断调用函数：快充1ms计数处理 放在定时器中处理 处理uart通信
*********************************************/
void fastCharge_1msTaskIsr(void)
{
	if (FAST_CHARGE_ON==0) return;
	
	UART_txDelay();
	UART_rxDelay();
	UART_heatBeatDelay(); 
}

/********************************************
函数名称:     fastCharge_10mscntFunction
中断调用函数：
快充10ms计数处理 放在中断中处理
控制电压测量
*********************************************/    
void fastCharge_10mscntFunction(void)
{
	
}

/********************************************
函数名称:     fastCharge_1scntFunction
中断调用函数：快充1s计数处理 主要做时间区分管理 放在定时器中断中处理
*********************************************/
void fastCharge_1scntFunction(void)
{
	
}

/*********************************************
函数名称: void UART_setTxDelay(uint8_t delayTime)
函数说明: 设置延时，用来控制和判断通信状态
*********************************************/
void UART_setTxDelay(uint16_t delayTime)
{
	uartCtrl.txDelay = delayTime;

	if (uartCtrl.txDelay)
		uartCtrl.txEn = 0;
	else
		uartCtrl.txEn = 1;
}

/*********************************************
函数名称: void UART_setRxDelay(uint8_t delayTime)
函数说明: 设置延时，用来控制和判断通信状态
*********************************************/
void UART_setRxDelay(uint16_t delayTime)
{
	uartCtrl.rxDelay = delayTime;
}

/*********************************************
函数名称: UART_txDelay
函数说明: 在1ms中断中调用
确保实现8（10ms） 140ms（150ms）
*********************************************/
void UART_txDelay(void)
{
	if (uartCtrl.txDelay == 0) return;

	uartCtrl.txDelay--;

	if (uartCtrl.txDelay == 0)
	{
		uartCtrl.txEn = 1;
	}
	else
	{
		uartCtrl.txEn = 0;
	}
}

/*********************************************
函数名称: void UART_setHeatBeatDelay(uint16_t delayTime)
函数说明: 设置心跳计时
*********************************************/
void UART_setHeatBeatDelay(uint16_t delayTime)
{
	uartCtrl.heatBeatDelay = delayTime;

	uartCtrl.txHeatBeatEn = 0;
}

/*********************************************
函数名称: void UART_heatBeatDelay(void)
函数说明: 设置心跳计时
*********************************************/
void UART_heatBeatDelay(void)
{
	if (USB_HANDSHAKE_OK == 0)  return;

	if (adapter_type == IDLE_TYPE) return;

	if (Bat_ChargeIsOn == 0) return;

	if (uartCtrl.heatBeatDelay == 0) return;

	uartCtrl.heatBeatDelay--;

	if (uartCtrl.heatBeatDelay == 0)
	{
		uartCtrl.heatBeatDelay = HEATBEAT_INTERVAL_TIME;

		uartCtrl.txHeatBeatEn = 1;

		fastCharge_heartBeat();
	}
}

/*********************************************
函数名称: void UART_setRxDelay(uint8_t delayTime)
函数说明: 设置延时，用来控制和判断通信状态
*********************************************/
void UART_rxDelay(void)
{
	if (uartCtrl.rxDelay == 0) return;

	uartCtrl.rxDelay--;

	if (uartCtrl.rxDelay == 0)
	{
		if (USB_NEES_ACK) fast_charge_sta = WAIT_OVERTIME;
	}
}

/*********************************************
函数名称: uint8_t UART_getTxEnFlag(void)
函数说明: 设置延时，用来控制和判断通信状态
*********************************************/
uint8_t UART_getTxEnFlag(void)
{
	return uartCtrl.txEn;
}


static const uint8_t  auchCRCLo[] = 
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};  


static const uint8_t auchCRCHi[] = 
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};

/*********************************************
函数名称:Modbus_CRC16
函数说明:CRC校验
*********************************************/
uint16_t Modbus_CRC16(uint8_t *Buff_addr,uint16_t len)
{
	uint8_t uchCRCHi = 0xFF;
	uint8_t uchCRCLo = 0xFF;
	uint16_t uIndex;
	while (len--)
	{
		uIndex = uchCRCLo ^ *Buff_addr++;
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return(uchCRCHi <<8 | uchCRCLo);
}

/*********************************************
函数名称:fastCharge_sendCmd
函数说明:发送适配器控制命令
*********************************************/
void fastCharge_sendCmd(ENUMERATION_MODEL command)
{
	uint16_t  Reply_CRCVal = 0;

	Uart_data.Sendbuf[0] = (uint8_t)command;
	Uart_data.Sendbuf[1] = 0x00;
	Uart_data.Sendbuf[2] = 0x00;
	Reply_CRCVal = Modbus_CRC16(Uart_data.Sendbuf, 3);
	Uart_data.Sendbuf[3] = (uint8_t)(Reply_CRCVal & 0x00FF);
	Uart_data.Sendbuf[4] = (uint8_t)((Reply_CRCVal & 0xFF00) >> 8);

	fastCharge_startTx();
}

/*********************************************
函数名称:fastCharge_setAdapterV
函数说明:设置适配器输出电压
*********************************************/
void fastCharge_setAdapterV(float Vo)
{
	uint16_t  Reply_CRCVal = 0;
	uint16_t  outval = 0;
	uint8_t   tem = 0;

	outval = (uint16_t)(Vo / 0.02f);

	Uart_data.Sendbuf[0] = SET_ADAPTER_VOLTAGE;
	tem = (uint8_t)(outval & 0x00FF);
	Uart_data.Sendbuf[1] = tem;
	tem = (uint8_t)((outval & 0xFF00) >> 8);
	Uart_data.Sendbuf[2] = tem;
	Reply_CRCVal = Modbus_CRC16(Uart_data.Sendbuf, 3);
	Uart_data.Sendbuf[3] = (uint8_t)(Reply_CRCVal & 0x00FF);
	Uart_data.Sendbuf[4] = (uint8_t)((Reply_CRCVal & 0xFF00) >> 8);

	uint16_t maxData, minData;

	maxData = MAX(outval, Uart_data.Adapter_V_data);
	minData = MIN(outval, Uart_data.Adapter_V_data);

	if (maxData - minData == 1)
	{
		uartCtrl.longIntervalFlag = 0;
	}
	else
	{
		uartCtrl.longIntervalFlag = 1;
	}

	Uart_data.Adapter_V_data = outval;

	fastCharge_startTx();
}

/*********************************************
函数名称:fastCharge_setAdapterI
函数说明:设置适配器输出电流
*********************************************/
void fastCharge_setAdapterI(float Ival)
{
	uint16_t  Reply_CRCVal = 0;
	uint16_t  outval = 0;
	uint8_t   tem = 0;

	Uart_data.Sendbuf[0] = SET_ADAPTER_CURRENT;

	outval = (uint16_t)(Ival / 0.05f);
	tem = (uint8_t)(outval & 0x00FF);
	Uart_data.Sendbuf[1] = tem;
	tem = (uint8_t)((outval & 0xFF00) >> 8);
	Uart_data.Sendbuf[2] = tem;
	Reply_CRCVal = Modbus_CRC16(Uart_data.Sendbuf, 3);
	Uart_data.Sendbuf[3] = (uint8_t)(Reply_CRCVal & 0x00FF);
	Uart_data.Sendbuf[4] = (uint8_t)((Reply_CRCVal & 0xFF00) >> 8);

	fastCharge_startTx();
}



//=======================硬件相关======================================

extern UART_HandleTypeDef huart1;

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
	huart1.Init.BaudRate = 19200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	huart1.gState = HAL_UART_STATE_RESET; //确保每次都能可靠初始化 2019.08.05 V2.25修改
	
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/*********************************************
函数名称: void UART_ConfigInit(void)
函数说明: 串口初始化函数
函数输入: 19200;//设置串口波特率
*********************************************/
void UART_ConfigInit(void)
{
  MX_USART1_UART_Init();
	
  Uart_data.index = 0;
}


void UART_Send(uint8_t *buf, uint8_t size)
{
  if (HAL_UART_Transmit_IT(&huart1, (uint8_t*)buf, size) != HAL_OK)
	{
		Error_Handler();
	}

	huart1.RxXferCount = size;

	uartCtrl.uartTxCnt++;
}

#define UART_TX_Pin              GPIO_PIN_6
#define UART_TX_Pin_GPIO_Port    GPIOB
#define UART_RX_Pin              GPIO_PIN_7
#define UART_RX_Pin_GPIO_Port    GPIOB


/*********************************************
函数名称: 关闭串口
*********************************************/
void UART_DISABLE(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin = UART_TX_Pin | UART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL; //这里要检查外围是否有上拉或下拉，否则容易导致功耗增加和波动
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(UART_TX_Pin_GPIO_Port, &GPIO_InitStruct);
}

/********************************************
函数名称:     UART_IsrFunction
中断调用函数：根据发送的命令和回复的命令，确定当前的运行状态
回复的命令按照协议进行分类处理
81 90 01 8C 28
*********************************************/
uint8_t UART_IsrFunction(void)
{
	  uint16_t      tem = 0;  
    
    if (HAL_UART_Receive_IT(&huart1, (uint8_t*)Uart_data.Rebuf, 5) != HAL_OK)  return 1;
    
    uartCtrl.uartRevCnt++;

		//接收完成1帧数据			
		Uart_data.crc1_val = Modbus_CRC16(Uart_data.Rebuf, 3);

		Uart_data.crc2_val = 0;

		tem = Uart_data.Rebuf[4];
		Uart_data.crc2_val = ((tem & 0x00FF) << 8);
		Uart_data.crc2_val |= Uart_data.Rebuf[3];

		if (Uart_data.crc1_val != Uart_data.crc2_val)  return 2;

		return 0;
}



//====================================END OF FILES==============================================================




