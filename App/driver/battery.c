/**
  ******************************************************************************
  * @file    battery.c 
  * @author  Figo
  * @version V1.0.0
  * @date    12-19-2019
  * @brief 
	
	����㣺  ����adc.c
	1.��ص��� ����ѹ ������ ����¶ȣ�
	2.����ͨ�ţ�
	3.���оƬ��ʹ�ܣ��ر�
	
	���ݲ㣺 �ڲ�ʵ��
	1.��صȼ�  �ٷֱ�
	2.���ʱ��
	3.�͵����ż�
	
	Ӧ�ò㣺  ����oled.c 
	1.��ص�����ʾ��
	2.��綯̬��ʾ��
	3.�͵����жϣ�
	4.�͵���������	
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/

#include "main.h" 

#include "app_task.h"
#include "myMath.h"

#include "adc.h"
#include "battery.h"
#include "delay.h"
#include "fast_changed.h"

#define BAT_CONNET_IS_NOT   0 //����ע����԰���1 ��ʵ�ʰ����Ƿ��� 0

#if (CHARGE_IC_IS_59121==1)

  #define ENABLE_CHARGE         HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_RESET) 
	#define DISABLE_CHARGE        HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_SET) 
	
#else
	#define ENABLE_CHARGE         HAL_GPIO_WritePin(SC8913_EN_GPIO_Port, SC8913_EN_Pin, GPIO_PIN_RESET) //LOW IS ENABLE
	#define DISABLE_CHARGE        HAL_GPIO_WritePin(SC8913_EN_GPIO_Port, SC8913_EN_Pin, GPIO_PIN_SET)
#endif

#if (BAT_CONNET_IS_NOT==1)

#define DISCONNECT_CHARGE     HAL_GPIO_WritePin(BAT_CHARGE_CONNECT_GPIO_Port, BAT_CHARGE_CONNECT_GPIO_Pin, GPIO_PIN_SET) 
#define CONNECT_CHARGE        HAL_GPIO_WritePin(BAT_CHARGE_CONNECT_GPIO_Port, BAT_CHARGE_CONNECT_GPIO_Pin, GPIO_PIN_RESET)

#else 
#define  CONNECT_CHARGE       HAL_GPIO_WritePin(BAT_CHARGE_CONNECT_GPIO_Port, BAT_CHARGE_CONNECT_GPIO_Pin, GPIO_PIN_SET) 
#define  DISCONNECT_CHARGE    HAL_GPIO_WritePin(BAT_CHARGE_CONNECT_GPIO_Port, BAT_CHARGE_CONNECT_GPIO_Pin, GPIO_PIN_RESET)

#endif

#define SYS_IS_CHARGING       (HAL_GPIO_ReadPin(USB_IN_GPIO_Port,USB_IN_Pin)==GPIO_PIN_RESET) 
#define SYS_NO_CHARGING	      (HAL_GPIO_ReadPin(USB_IN_GPIO_Port,USB_IN_Pin)==GPIO_PIN_SET)

#define IC_IS_CHARGING        (HAL_GPIO_ReadPin(USB_DET_GPIO_Port,USB_DET_Pin)==GPIO_PIN_RESET) 
#define IC_NO_CHARGING	      (HAL_GPIO_ReadPin(USB_DET_GPIO_Port,USB_DET_Pin)==GPIO_PIN_SET)

#define UART_TX_Pin              GPIO_PIN_6
#define UART_TX_Pin_GPIO_Port    GPIOB
#define UART_RX_Pin              GPIO_PIN_7
#define UART_RX_Pin_GPIO_Port    GPIOB


extern UART_HandleTypeDef huart1;

static uint8_t chargeType;
static uint8_t checkBatVdelay;

void bat_setCheckDelay(uint8_t delay) //��λ10ms
{
	checkBatVdelay = delay;  //��������ʱ ����
}

uint8_t bat_getCheckDelay(void) //��λ10ms
{
	if (checkBatVdelay) 
	{
		checkBatVdelay--;
		if (checkBatVdelay==0) return 2; 
		return 1;
	}
	
	return 0;
		
}

void bat_getBatFactLevel(void)
{
		Sys_Param.bat_vol_ad = adc_getBatV_ADC(); // ��ȡ���AD 
	  Sys_Param.batV = bat_getBatVol();		
    Sys_Param.battery_level = bat_getLevel(Sys_Param.batV); 
}

void bat_setChargeType(uint8_t type)
{
		chargeType = type;
}

uint8_t bat_getChargeType(void)
{
		return chargeType;
}



void bat_uartDeinit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */ 
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : UART_TX_Pin UART_RX_Pin */
  GPIO_InitStruct.Pin = UART_TX_Pin|UART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
  HAL_GPIO_Init(UART_TX_Pin_GPIO_Port, &GPIO_InitStruct);	
}

static uint8_t flagUsbInsert;
extern uint8_t SC8913_chargeManage(uint8_t type);
extern void SC8913_init(void);

/**
  * @}
  */


 /**
  * @brief  ����������
  * @param  
  * @retval 
1.��ѭ���� 10ms����1��
2.��Ҫ���ã�
1����ʼ���˿�
2������ ��ͣ �ָ� ֹͣ ���
3�������ʾ�Ĵ���
4�������� �������ļ���
*/
extern void fastCharge_10mscntFunction(void);
uint8_t bat_getUsbStatus(void);

extern void SC8913_setInput9V(void);
extern void SC8913_DeInit(void);
static uint8_t chargeIsOff;

void bat_chargeTaskHandle(void)
{
	bat_getUsbStatus();
	
	if (bat_getUsbStatusIsr()==0) return;
	
	#if (CHARGE_IC_IS_59121==0)
		
	uint8_t ret = 0;
	
	ret = SC8913_chargeManage(1);
	
//	if (ret==0)
//	{
//		FAST_CHARGE_ON = 1;
//		
//		SC8913_setInput9V();
//		
//		charge_Manage();  
//		
//		fastCharge_uartComManage(); //10msִ��1��  
//		
//		fastCharge_handshake();//�������� 

//		fastCharge_10mscntFunction();			
//	}

	#endif
}

void bat_chargeInit(void)
{
#if (CHARGE_IC_IS_59121==0)	
	
	 HAL_UART_DeInit(&huart1);
	
   bat_uartDeinit();
	
	 ENABLE_CHARGE;	
	
	 SC8913_init();
	
	 fastCharge_Init();
	
#else
	
	 DISABLE_CHARGE;	
	
#endif
	
	 flagUsbInsert = 0;
}

extern void SC_Control_GPO(uint8_t On_Off);

uint8_t bat_getChargeStatus(void)
{
	if (IC_IS_CHARGING && chargeIsOff==0)
		return 1;
	return 0;
}

void bat_chargeRecover(void)
{
	
	#if (CHARGE_IC_IS_59121==0)
		CONNECT_CHARGE;
		SC_Control_GPO(0); 
	#endif
	
  if (bat_getUsbStatusIsr())
	{
	  ENABLE_CHARGE;	
	}
}

void bat_chargePause(void) //��ͣ ���̻��߳�������ͣ
{
#if (CHARGE_IC_IS_59121==0)
	DISCONNECT_CHARGE;
	SC_Control_GPO(1);
#endif
	DISABLE_CHARGE;
	
	Sys_Param.chargeTime = 0;
}

void bat_chargeOff(void)
{
#if (CHARGE_IC_IS_59121==0)	
	DISCONNECT_CHARGE;	
#endif
	DISABLE_CHARGE;

	chargeIsOff = 1;
}


void bat_chargeStart(void)
{	
#if (CHARGE_IC_IS_59121==0)	
	CONNECT_CHARGE; 
#endif
	ENABLE_CHARGE;	

  Sys_Param.charge_full_count	= 0;	

  Sys_Param.chargeTime = 0;	
	
	chargeIsOff = 0;
}


uint8_t bat_getUsbStatus(void)
{	 
#if (CHARGE_IC_IS_59121==0)		
	 static uint8_t checkCnt;
#endif
	
	 if (HAL_GPIO_ReadPin(USB_IN_GPIO_Port,USB_IN_Pin)==GPIO_PIN_RESET) //HAL_GPIO_ReadPin(USB_IN_GPIO_Port,USB_IN_Pin)==GPIO_PIN_RESET  SYS_IS_CHARGING
	 {
		 if (flagUsbInsert==0)
		 {
			 flagUsbInsert = 1;
			 
#if (CHARGE_IC_IS_59121==0)					 
			 HAL_UART_DeInit(&huart1);	
			 bat_uartDeinit();		 
			 SC8913_chargeManage(0);	 
			 fastCharge_Init();
			 checkCnt = 0;
#else
			 bat_chargeStart();	
#endif				 	 
		 }
		 else
		 {
#if (CHARGE_IC_IS_59121==0)	
			 adc_getUsbV_ADC();
	
		   chargeData.USB_V = adc_getUsbV();		
			 
			 if (chargeData.USB_V < 4.0f)
			 {
				 checkCnt++;
				 
				 if (checkCnt > 50)
				 {
					 
				   SC8913_DeInit();
					 
					 bat_chargeOff();	
				 }
			 }
			 else if (chargeData.USB_V > 4.5f)
			 {
				 checkCnt = 0;
			 }
#endif
		 }	 
     return 1; 
	 }
	 else if (flagUsbInsert)
	 {	 
		 flagUsbInsert = 0;
		 
		 bat_setCheckDelay(5);
		 
#if (CHARGE_IC_IS_59121==0)	
		 HAL_UART_DeInit(&huart1);
		 
		 bat_uartDeinit();	
		 
		 fastCharge_Init();	
		 
#else
		 
		 bat_chargeOff();	
		 
#endif
	 		 	  
	 }
	
	 return 0;
	
}

uint8_t bat_getUsbStatusIsr(void)
{	  
   if (SYS_IS_CHARGING)
     return 1; 
	 else
		 return 0;
}
  
/****************��ȡʵ�ʵ�ص�ѹ�ͳ�絽����ϵĵ���*****************************
ʵ�ʲ��� ����ok
*********************************************************************************/

//�����ļ������ʾ

/**
  * @brief  ��ȡ��ص�ѹ�ȼ�
  * @param  bat_ad:���AD 
  * @retval bat level 0~16
*/
/* ��ص����ȼ��б� */
const uint16_t bat_list[BATTERY_MAX_LEVEL] = {3350,3400,3500,3550,3600,
	                                            3650,3700,3720,3750,3800,
	                                            3820,3850,3880,3900,3930,
                                              3950,3980,4000,4020,4080};

uint16_t bat_getBatVol(void)
{
	  volatile uint16_t batV; 
	  
	  if (bat_getChargeStatus())
		   batV = (uint16_t)(Sys_Param.bat_vol_ad * BAT_V_SCAL) + 80; //-0.05
		else
			 batV = (uint16_t)(Sys_Param.bat_vol_ad * BAT_V_SCAL) + 130;
		
	  return batV;
}

uint16_t bat_getLevel(uint16_t bat_v)
{
  uint8_t i =0;
	
	uint16_t temp_val = bat_v; 
	uint16_t temp_level = 0;
	
	for (i=0; i<BATTERY_MAX_LEVEL;i++)
	{
		if (temp_val >= bat_list[i])
		{
			temp_level = i+1;
		}
	}
  
  return temp_level;
}  

/**
  * @brief  bat_chargeChangeLevel ��ʾ��س������״̬
  * @param  
  * @retval 
 ��100ms�������
*/
extern uint8_t SC8913_checkOffIsEn(void);
extern uint8_t smoke_rungingScanRes(void);

static volatile uint8_t chargeDelayS;

void bat_chargeSetDelay(uint8_t delayS)
{
	chargeDelayS = delayS;
}

uint8_t bat_chargeGetDelay(void)
{
	if (chargeDelayS)
	{
		chargeDelayS--;
		if (chargeDelayS==0)
		{
			return 1;
		}
	}
	return 0;
}


void bat_chargeChangeLevel(void)
{
  /* ��ȡ��ص��� (��ֹbutton_fire����ʱ�պö�ȡ��ص������� oled��ʾռ��ʱ��) */    
	  static uint8_t t100msCnt;
	  
		t100msCnt++;

		if (t100msCnt < 2) return ;

		t100msCnt = 0;

	
    /* ���ڳ���� */
    if (bat_getUsbStatusIsr())
    {
			Sys_Param.charge_changeFlag = 1;
			
      if( Sys_Param.battery_level < BATTERY_MAX_LEVEL )
      {
        Sys_Param.battery_level++;
      }
      else
      {
				Sys_Param.batV = bat_getBatVol();	
				
        Sys_Param.battery_level = bat_getLevel(Sys_Param.batV);
		
        if ( Sys_Param.battery_level >= BATTERY_MAX_LEVEL )
        {
          /* (��ص�ѹ>4.12V��ʱ5����)��(���оƬ�������)��ʾ���� */		
					if (Sys_Param.batV > 4150) 
					{
						if (Sys_Param.chargeTime < CHARGE_MAX_TIMEOUT)
						{
							Sys_Param.chargeTime +=10; //4.15V֮��1.5����ǿ�ƹر�
							
							if (Sys_Param.chargeTime>=CHARGE_MAX_TIMEOUT)
							{
									bat_chargeOff(); 
							}
						}	
						Sys_Param.charge_full_count = CHARGE_FULL_TIMEOUT;			
					}
          else if (Sys_Param.charge_full_count < CHARGE_FULL_TIMEOUT)
          {
						if (Sys_Param.batV >= 4100) //4.10V 
						{
							Sys_Param.charge_full_count += 10; //4.10V���Ͽ���ֹͣ 50��
						}
						else
						{
							Sys_Param.charge_full_count++;		
						}
						
						Sys_Param.battery_level = BATTERY_MAX_LEVEL - 1;
						
						bat_chargeRecover(); 
          }
//#if (CHARGE_IC_IS_59121==1) //����4.08V֮��5���� ��ʾ���� 
//					else 
//					{
//						bat_chargeOff(); 
//					}
//#else
//          else if (SC8913_checkOffIsEn())
//					{
//						bat_chargeOff(); //����4.12V֮��5���ӣ���ʾ����
//					}
//#endif					
        }			
        else if (Sys_Param.batV > 4000) //4V�Ժ�15���� ǿ�ƹر�
        {
					if (Sys_Param.chargeTime < CHARGE_MAX_TIMEOUT)
					{
						Sys_Param.chargeTime++;
						
						if (Sys_Param.chargeTime==CHARGE_MAX_TIMEOUT)
						{
						    bat_chargeOff(); 
						}
					}	
					
					if (Sys_Param.batV < 4020) //����4.02V�������
					{
					  if ((Sys_Param.charge_full_count >= CHARGE_FULL_TIMEOUT) || (Sys_Param.chargeTime >= CHARGE_MAX_TIMEOUT))
						{
							bat_chargeStart();	
						}
					}
        }
				else if (Sys_Param.batV < 3950) 
				{
					Sys_Param.chargeTime = 0;
					
					Sys_Param.charge_full_count = 0;
				}				
      }
    }

}



//�˴�Ӱ�칦�����
void bat_chargeDeInit(void)
{
#if (CHARGE_IC_IS_59121==0)		
	
	//�������״̬������
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = SC8913_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; 
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SC8913_EN_GPIO_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(SC8913_EN_GPIO_Port, SC8913_EN_Pin, GPIO_PIN_RESET);

#if (BAT_CONNET_IS_NOT==1)	
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
#else
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
#endif	

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  
	GPIO_InitStruct.Pin = BAT_CHARGE_CONNECT_GPIO_Pin;
  HAL_GPIO_Init(BAT_CHARGE_CONNECT_GPIO_Port, &GPIO_InitStruct);
	
#else
  	//�������״̬������
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  GPIO_InitStruct.Pin =  USB_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; //GPIO_MODE_OUTPUT_PP GPIO_MODE_INPUT
  GPIO_InitStruct.Pull = GPIO_PULLDOWN; //GPIO_PULLUP GPIO_NOPULL GPIO_PULLDOWN
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(USB_DET_GPIO_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_DET_Pin, GPIO_PIN_RESET); 
	
	GPIO_InitStruct.Pin =  USB_EN_Pin;

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; 
  GPIO_InitStruct.Pull = GPIO_PULLDOWN; //GPIO_PULLDOWN   GPIO_PULLUP
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(USB_EN_GPIO_Port, &GPIO_InitStruct);

#endif
}
  



//==================================END OF FILES===========================================

