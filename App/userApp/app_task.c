
//============================================
// filename: app_task.c
// chip    : 
// author  : Leo Ou
// version : v1.0.0	
// date    : 2019-06-25
//============================================

/* Includes */

#include "main.h"
#include "math.h"

#include "app_task.h"
#include "menu.h"
#include "gui.h"
#include "myMath.h"
#include "flash.h"

/* Privae Define */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
m_param_t     Sys_Param;
_SYSCHANNEL_  sysChannel; 

uint8_t flagNewLogic;

//uint8_t debugCnt; //��ʾ���Խ�� ��������
// display_integer(debugCnt,"%02d",10,30,1);

/* Private function prototypes -----------------------------------------------*/

static void Show_Process(void);
static void Firing_Process(void);
static void Sleep_Process(void);

extern void oled_displayOff(void);
extern void oled_displayOn(void);


/* Public function prototypes -----------------------------------------------*/

extern void System_restoreDefault(void);
extern void sys_enterSleep(void); 
extern void sys_exitSleep(uint8_t type); 

extern  uint8_t keyApp_fireIsPress(void);
extern  uint8_t keyApp_fireIsIdle(void);
extern  uint8_t keyApp_addDecIsIdle(void);
extern  uint8_t keyApp_allIsIdle(void);

extern  uint16_t smoke_Calibration(uint16_t resStandads);
extern  uint16_t smoke_getoffset(void);

extern  void System_copyConfigData(void);
extern  void    bat_setCheckDelay(uint8_t delay);
extern  uint8_t bat_getCheckDelay(void);


/* Private functions ---------------------------------------------------------*/

/*----------------------------------------------------------
* void sys_softReset(void)
-----------------------------------------------------------*/
void sys_softReset(void)
{
	SCB->AIRCR = 0X05FA0000 | (uint32_t)0x04;
}


volatile uint16_t debugData;

void sys_displayDebug(uint16_t debug)
{
	 debugData = debug;
}

volatile float debugFloatData;

void sys_displayFloatDebug(float debug)
{
	 debugFloatData = debug;
}


/**
  * @brief  sys_dataConfig ���ݴ��� ����ָ�� ������ֲ
  * @param  None
  * @retval None
  */
void sys_dataConfig(uint8_t out_chnNum)
{
 (void)out_chnNum;
  
	sysChannel.set_power  = &Sys_Param.set_power;
  sysChannel.puff_count = &Sys_Param.puff_count;
  sysChannel.load_res   = &Sys_Param.load_res; 	
}


/**
  * @brief  ������ �Ӽ������������ж�.
  * @param  None
  * @retval None
  */
uint8_t sys_keyAddDecEn(void)
{
   if (keyApp_fireIsIdle()==0) return 0;  

   if (Sys_Param.flag.system_locked == 1) return 0;  
   
   return 1;   
}

/**
  * @brief  �����̰���ģʽ���� �����̰� ���ǲ��Ӽ�
  * @param  None
  * @retval None
  */
uint8_t sys_shiftChargeSmokeMode(void)
{					
	/* �����߳�������°��µ����л�ģʽ */
	if( Sys_Param.active_mode == CHARGE_MODE )
	{
		  Sys_Param.prev_mode = Sys_Param.active_mode; //CHARGE_MODE
			Sys_Param.active_mode = SYS_RUN_MODE;
		  Sys_Param.active_event = Wakeup_Event; 
		  return 1;
	}
	return 0;
}

void sys_shiftSmokeMode(void)
{	
	/* ϵͳ����ʱ����������ʾ LOCKED */
	if( Sys_Param.flag.system_locked == 1)  
	{
		Sys_Param.active_event = Lock_Event;
	}
}



/**
  * @brief  �л����˵�ģʽ
  * @param  None
  * @retval None
  */
void sys_shiftMenuMode(void)
{		
	/* ���뵽�˵� */	
	Sys_Param.active_mode = MENU_MODE;	
}

extern uint8_t smoke_scanRes(void);
extern uint8_t smoke_startScanRes(void);
extern uint8_t smoke_rungingScanRes(void);


/**
  * @brief  ϵͳģʽ����.
  * @param  None
  * @retval None
  */
extern void driver_init(uint8_t sysOnFlag);


void Mode_Management(void)
{

	switch( Sys_Param.active_mode )
	{

		case  IDLE_MODE:

			if( Sys_Param.prev_mode != IDLE_MODE ) 
			{
				Sys_Param.prev_mode = IDLE_MODE;
			}
			
			if ( (Sys_Param.active_event == Null_Event) && (Sys_Param.flag.system_on_off == SYS_ON) )
			{
				Sys_Param.flag.system_on_off = SYS_OFF; 
				Sys_Param.active_mode = SLEEP_MODE;
			}
			
		break;
		
		case  INIT_MODE:
			
			if ( Sys_Param.prev_mode != INIT_MODE )  /* ��������ʱҪ��ʼ�� */
			{				
				Sys_Param.prev_mode = INIT_MODE;
				
				Sys_Param.flag.system_on_off = SYS_ON;				
				
				Sys_Param.active_event = Init_Event;	
				
				driver_init(Sys_Param.flag.system_on_off); 
				
				smoke_startScanRes(); //������ֵ���
			}
			
			if (Sys_Param.active_event == Null_Event)
			{
				Sys_Param.active_mode = SYS_RUN_MODE;
	
		    bat_getBatFactLevel();
				
				bat_setCheckDelay(0);
			}	
			
		break; 
			
		case  SYS_RUN_MODE:

			if ( SYSTEM_ON )
			{
				if (Sys_Param.prev_mode != SYS_RUN_MODE)
				{
					oled_displayOn();
					
					button_ledOn();
				}
				
				Sys_Param.prev_mode = SYS_RUN_MODE;
				
				Firing_Process();  // �����
			}
      else if (bat_getUsbStatusIsr())
			{
				Sys_Param.active_mode = CHARGE_MODE;
				if (Sys_Param.wakeup_type == button_WAKEUP
					|| Sys_Param.wakeup_type == ATO_WAKEUP)  // ��������
				{
					Sys_Param.prev_mode = SLEEP_MODE;
					
					oled_Init(0x00);
				}
				else Sys_Param.prev_mode = SYS_RUN_MODE;
			}				
			
		break;
			
		case MENU_MODE:
			
		  if ( Sys_Param.prev_mode != MENU_MODE )
			{
				Sys_Param.prev_mode = MENU_MODE;			
				
				Sys_Param.active_event = MenuConfig_Event; //ȷ����������ʾ����ĸ���
				
				oled_Show_Clear(0); //����Ļ ��ʾ����˵�
				
				menu_showHomepage();	
				
				Sys_Param.menu_timeOut = 10000;
			}
			else if (Sys_Param.menu_timeOut==0)
			{
				 Sys_Param.active_mode = SYS_RUN_MODE;
				
				 Sys_Param.prev_event = Sys_Param.active_event;//ȷ��һ����ˢ�´���
				
				 Sys_Param.active_event = Null_Event;
	
			}
			
		 break;

		case  SLEEP_MODE:      
    
			Sleep_Process();
    
			Sys_Param.prev_mode = SLEEP_MODE;

		break;

		case  CHARGE_MODE:

			if ( Sys_Param.prev_mode == SLEEP_MODE )
			{					
				bat_chargeSetDelay(AUTO_IDLE_TIMEOUT); //��ʱ30�����
				
				show_charge_battery_flag = 1;
				
				oled_displayOn();
				
				if (SYSTEM_ON)
				{
					button_ledOn();
				}
				else
				{
					button_ledOff();
				}
				show_ativer_flag = 1;
			}		
		
			Sys_Param.prev_mode = CHARGE_MODE;
			
			
			if (bat_getUsbStatusIsr()==0)
			{
				if (SYSTEM_ON)
				{
					Sys_Param.active_mode = SYS_RUN_MODE; //�ָ�������
					Sys_Param.idle_count = AUTO_IDLE_TIMEOUT - OFF_IDLE_TIMEOUT;
				}
				else
				{
					Sys_Param.active_mode = SLEEP_MODE; 
				}
				Sys_Param.prev_event = Null_Event;
				Sys_Param.active_event = Wakeup_Event;
			}

		break;

		default:

			Sys_Param.active_mode = INIT_MODE;
		
		break;

	}
}

/**
  * @brief  ���洦��
  * @param  None
  * @retval None
  */

#define SYS_CHARGE_ON    1
#define SYS_CHARGE_OFF   0

static void sys_warnProcess(uint8_t chargeEn)
{
	(void)(chargeEn);
	
  Sys_Param.flag.smoke_firing = 0;

  FIRE_TIME_STOP();

  smoke_outPutOff();  	
	
	bat_chargeRecover();  
}

/**
  * @brief  �����.
  * @param  None
  * @retval None
  */
static void Firing_Process(void)
{
	static uint8_t process_flag = 0;

	/****************** ���ǰ��⴦�� ******************/
	if (Sys_Param.flag.smoke_firing == 0)
	{
		if (keyApp_fireIsPress() && button_fire.short_count==0) 
		{  		
			/* �����¼���ϵͳ������ʱ���Ե�� */
			if( (keyApp_addDecIsIdle() || (flagNewLogic==1) || (Sys_Param.flag.system_locked==1)) 
				&& ( Sys_Param.active_event == Null_Event
          || Sys_Param.active_event == Wakeup_Event 			
			    || Sys_Param.active_event == Lock_Event 
				  || Sys_Param.active_event == PowerIsMax_Event)  
			  && (process_flag == 0) )
			{
	
				if( Sys_Param.flag.sys_high_temp == 1 )
				{
					Sys_Param.out_temp_ad = adc_getOutTemp_ADC();  // ��ȡϵͳ�¶�AD
				
					if (flagSysIsDefault==1)
					{
						if( Sys_Param.out_temp_ad > MIN_TEMP )//�¶ȵ���60�� 
						{
							Sys_Param.flag.sys_high_temp = 0;
						}	
				  }
					else
					{
						if( Sys_Param.out_temp_ad > TEST40_TEMP )//�¶ȵ���45�� 
						{
							Sys_Param.flag.sys_high_temp = 0;
						}	
					}
				} 

				smoke_calParameter(0x00);      
                
				/* û�����κι����¼� */
				if( Sys_Param.active_event == Null_Event
					|| Sys_Param.active_event == Wakeup_Event 
			    || Sys_Param.active_event == Lock_Event 
				  || Sys_Param.active_event == PowerIsMax_Event)
				{		
						Sys_Param.puff_time = 0;
						Sys_Param.count_10ms = 100;
					
						Sys_Param.firing_timeout = FIRING_TIMEOUT;
					  Sys_Param.flag.smoke_firing = 1;
						Sys_Param.flag.out_on = 1;  
						Sys_Param.flag.power_out_enable = 1;
					
					  bat_setCheckDelay(250); 
					
						Sys_Param.flag.firing_short = 0;
						Sys_Param.flag.firing_temp_high = 0;
						Sys_Param.flag.firing_bat_low = 0;
						Sys_Param.flag.firing_open = 0;
						Sys_Param.checkOpenCnt = 0;			
						Sys_Param.bat_low_count = 0;
						Sys_Param.cease_fire_count = 0;
						
						Sys_Param.adc_gather_channel = OUT_VOLTAGE;
						
						FIRE_TIME_START(); // �������ʱ��
						
				}
				else 
				{
					bat_setCheckDelay(100);
				}
			}

			process_flag = 1;  // ��ֹ�������ɿ���������

		}
		else {

			process_flag = 0;
		}

	}
}


/**
  * @brief  ��ʾ����.
  * @param  None
  * @retval None
  */
extern uint8_t  oled_Show_BatV(uint16_t batVData);

static void Show_Process(void)
{
	if( (SYSTEM_ON) && (Sys_Param.active_mode != CHARGE_MODE) )
	{
		if (Sys_Param.active_mode == MENU_MODE) return;
		
		/* �¼��л����� */
		if( Sys_Param.prev_event != Sys_Param.active_event ) 
		{
			Sys_Param.prev_event = Sys_Param.active_event;
			
			if (flagSysIsDefault==1)
			{
			  Sys_Param.show_timeout = SHOW_TIMEOUT; // ������ʾʱ��
			}
			else
			{
				Sys_Param.show_timeout = SHOW_TEST_TIMEOUT;
			}
			Sys_Param.flag.show_event = 1;  // ����
			
			oled_Show_Clear(1); 					
		}

		/* ��Ӧ�¼���ʾ */
		switch (Sys_Param.active_event)
		{
			/***************** ���¼� *****************/
			case Wakeup_Event:
				
				Sys_Param.active_event = Null_Event;			  
			  Sys_Param.prev_event = Sys_Param.active_event;
			
			case Null_Event: 

        if (Sys_Param.flag.show_event==1)		oled_Show_BackUI();

			  if ( Sys_Param.flag.smoke_firing == 0 && bat_getUsbStatusIsr()==0) 
				{
					Sys_Param.batV = bat_getBatVol();		
          Sys_Param.battery_level = bat_getLevel(Sys_Param.batV); 					
				}

				oled_Show_BatV(Sys_Param.batV/10);

			  if (Sys_Param.charge_changeFlag)
				{
					Sys_Param.charge_changeFlag = 0;
			    oled_showHomePageBattery(Sys_Param.battery_level);  // ��ʾ�����ȼ�
				}

				oled_Show_Puff(*(sysChannel.puff_count));		 // ��ʾ���̿���
				oled_Show_Time(Sys_Param.puff_time);         // ��ʾ����ʱ��
				oled_Show_AtoRes(Sys_Param.load_res);
				oled_Show_Power(*(sysChannel.set_power));    // ��ʾ���ù��� 	  
      
			break;
				
				
			/***************** ATO�仯�¼� *****************/
			case CheckRes_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					menu_displayString("ATO RES",45,10,1);
					oled_showMessaggeAtoRes(Sys_Param.load_res);				
				}
			
				break;
			
			case MenuConfig_Event:
			
				break;
			
			case ModeConfig_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					menu_displayString("MODE IS",15,35,1);
					
					Display_setFgColor(COLOR_TEAL);

					if (flagNewLogic==0)
					{
					   menu_displayString("OLD",110,35,1);
					}
					else
					{
						 menu_displayString("NEW",110,35,1);
					}
					
					Display_setFgColor(COLOR_WHITE);
				}
				break;

			/***************** ��ʼ���¼� *****************/
			case Init_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					if (flagSysIsDefault==1)
					{
						oled_Show_Snowwolf(); // ��ʾ����LOGO	
		
						systick_delay_ms(1000);	
						
						Display_setFgColor(COLOR_TEAL);
						
						if (flagNewLogic==0)
						{
							oled_Show_Version(35,55);
						}
						else
						{
							oled_Show_NewVersion(35,55);
						}
						
						Display_setFgColor(COLOR_WHITE);
						
						systick_delay_ms(1000);	
					}
					else
					{
						Display_setFgColor(COLOR_TEAL);
						
						if (flagNewLogic==0)
						{
							oled_Show_Version(35,55);
						}
						else
						{
							oled_Show_NewVersion(35,55);
						}
						
						Display_setFgColor(COLOR_WHITE);
				
						oled_Show_Snowwolf(); // ��ʾ����LOGO	
					}	
				}
			break;

			/***************** ��/�ػ��¼� *****************/
			case SystemOff_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_SysOff(); // �ػ�LOGO
				}
			break;

			/***************** �����¼� *****************/
			case OverHeat_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_OverHeat(); // ����LOGO
				}
			break;

			/***************** ������¼� *****************/
			case ResHigh_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_CheckPod(); // ����LOGO
					
					smoke_startScanRes();
				}
			break;

			/***************** ���̳�ʱ�¼� *****************/
			case OverTime_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_OverTime(); // ��ʱLOGO
				}
			break;

			/***************** ��ѹ���¼� *****************/
			case VolLow_Event:

				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_BatteryLow(); // �͵���LOGO
				}
			break;

			/***************** ��·�¼� *****************/
			case ResLow_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Short(); // ��·LOGO();
					smoke_startScanRes();
					Sys_Param.load_res = SHORT_RES;
				}
				
			break;

			/***************** �����¼� *****************/
			case Lock_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Locked(); // ����LOGO
				}
			break;

			/***************** �����¼� *****************/
			case Unlock_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Unlock(); // ����LOGO
				}
			break;
				
			/***************** ���ôﵽ����� *****************/
			case PowerIsMax_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_PowerIsMax(); // ���ʴﵽ���ֵ����ʾ
				}
			break;
				
			/***************** ��ȡ�汾 *****************/
			case ReadVersion_Event: 
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					Display_setFgColor(COLOR_TEAL);
					
					if (flagNewLogic==0)  // ��ȡ����汾
					{
            oled_Show_Version(35,35);
					}
					else
					{
						oled_Show_NewVersion(35,35);
					}
					
					Display_setFgColor(COLOR_WHITE);

				}
			break;	
      /***************** Factory *****************/
			case Factory_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_RestoreDefault(); // �ָ�Ĭ��
				}
			break;

      /***************** Res Adjust *****************/
			case ResAdjust_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					Display_setFgColor(COLOR_TEAL);
					
					if (flagNewLogic==0)
					{
            oled_Show_Version(35,5);
					}
					else
					{
						oled_Show_NewVersion(35,5);
					}
					
					Display_setFgColor(COLOR_WHITE);
					
					menu_displayString("Calibration",30,25,1);
					
					if (smoke_Calibration(310)==0xffff)
					{
					  Display_setFgColor(COLOR_RED);
						menu_displayString("Fail",70,55,1);
					}
          else
					{
						Display_setFgColor(COLOR_GREEN);
					  menu_displayString("Sucess",60,55,1);
						
					  page_buffer[12]  = 0x05;
						page_buffer[13] = smoke_getoffset(); //default
						page_buffer[14] = smoke_getoffset();

			      Flash_Write_Page(page_buffer,15);  //����д��
					}
					
					Display_setFgColor(COLOR_WHITE);
				}
			break;				
				
				
				
			default:
				break;
		}
		

		/* ��ʾ��Ϻ��¼����� */
		if ( Sys_Param.active_event != Null_Event) {
			
			if (Sys_Param.show_timeout > 0)
			{				
				/* ����������ʱ��ʾ(��ֹ��������ʱ���� Null_Event) */
				if( (keyApp_fireIsPress()) && (Sys_Param.active_event != Init_Event) )
				{
					if (Sys_Param.show_timeout < 50)
					{
						Sys_Param.show_timeout = 50;
					}
				}
	
				if ((Sys_Param.active_event  == Lock_Event)
					||(Sys_Param.active_event  == Unlock_Event)
				  || (Sys_Param.active_event == PowerIsMax_Event))
				{
					if (keyApp_fireIsPress())
					{
						Sys_Param.active_event = Null_Event;
					}
				}
				
			} 
			else {
				
				/* �ڰ����ͷź�������¼� */
				if ((keyApp_fireIsIdle()) || (Sys_Param.active_event == Init_Event) )
				{
					 if ((Sys_Param.active_event == ReadVersion_Event) || (Sys_Param.active_event == Factory_Event))
					 {
						 if (keyApp_addDecIsIdle()) Sys_Param.active_event = Null_Event;		
					 }
					 else
						 Sys_Param.active_event = Null_Event;	
					 				
				}
	
			}
		}
		
		Sys_Param.flag.show_event = 0;
	}
}  

/**
  * @brief  ϵͳ���ߴ���.
  * @param  None
  * @retval None
  */
extern void sys_sleepMode(uint8_t type);

static void sys_getWakeUpType(void)
{
	if (button_Fire_Press()) Sys_Param.wakeup_type = button_WAKEUP;
	else if (bat_getUsbStatusIsr()) Sys_Param.wakeup_type = USB_WAKEUP;	
	else Sys_Param.wakeup_type = ATO_WAKEUP;	 
}


extern void System_writeConfigData(uint8_t writeType);

static void Sleep_Process(void)
{
	Sys_Param.active_event = Null_Event; 
	Sys_Param.wakeup_type = NULL_WAKEUP;
	
	
	 if (flagSysIsDefault)
	 {
		 System_writeConfigData(0); //�б仯��д��
	 }

	/* ��������*/
	if (bat_getUsbStatusIsr()==0)
	{	
		sys_sleepMode(Sys_Param.flag.system_on_off);      

		sys_exitSleep(Sys_Param.flag.system_on_off);   

		sys_getWakeUpType();

		/* ����ʱ�������ѻ�USB����ʱ ����9V��oledģ�� ������Ҫ������*/		
		
		if ( Sys_Param.wakeup_type == USB_WAKEUP )  // USB���ѽ�����ģʽ
		{
			if (SYSTEM_ON)
			{
				Sys_Param.active_mode = SYS_RUN_MODE; 

        Sys_Param.idle_count = AUTO_IDLE_TIMEOUT - OFF_IDLE_TIMEOUT; //5�������Ͻ�����ģʽ
      }
      else
			{ 
				Sys_Param.active_mode = CHARGE_MODE; 
			}				   					
		}
		else   // �������ѽ�����ģʽ
		{
			Sys_Param.active_mode = SYS_RUN_MODE;	
			
			if (SYSTEM_ON)
			{
			  Sys_Param.flag.show_event = 1;
				
				if ( Sys_Param.wakeup_type == ATO_WAKEUP ) 
				{
					smoke_startScanRes();
				}
			}	
		}	
			
		if (SYSTEM_ON || (Sys_Param.wakeup_type == USB_WAKEUP))
		{					
			oled_Init(0x00);	

      Sys_Param.active_event = Wakeup_Event; 			
		}
		
	  Task_Init();  //�������ȷ�����Ѳ�������Ӱ��ʱ��Ƭ�ֳ�ʼ��
		
		if (flagSysIsDefault==1)
		{
		  Sys_Param.flag.system_locked = 1; //��Ҫ������ʾ��������
		}
		else if (SYSTEM_ON==SYS_OFF && menu_getMode()==0)
		{
			Sys_Param.set_power = DEFAULT_POWER; 
		}
	}
	else{
    
		delay_ms_soft(100);	    	
    
		Sys_Param.active_mode = CHARGE_MODE;		
	}

}   
//ʱ��Ƭ����=======================================================================

/**
  * @}
  */
void sys_addDecKeyFunctionHandle(uint8_t keyStatus);

void sys_fireKeyFunctionHandle(uint8_t keyStatus);

void sys_fireSmokeRun(void);

void task_1msHandle(void)
{
	if (Task_TimeOut(button_Task)==0) return;
	
	 static uint8_t fireKeyStatus,fireKeyStatusBak;
	
	 button_Fire_Detect();
		
	 fireKeyStatus = keyApp_getFireStatus();
	
	 if (MENU_MODE==Sys_Param.active_mode) 
	 {
		 if (fireKeyStatus==0x02 && fireKeyStatusBak!=fireKeyStatus)
		 {
		   menu_confirmHandle();
		 }
	 }
	 else
	 {
		 sys_fireKeyFunctionHandle(fireKeyStatus);
		
		 sys_fireSmokeRun();	
	 }
	 
	 fireKeyStatusBak = fireKeyStatus;
} 


void task_10msHandle(void)
{
	  if (Task_TimeOut(Show_Task)==0)  return;  

	  if (smoke_rungingScanRes()) bat_setCheckDelay(100);
	
	  uint8_t ret;
	  ret = bat_getCheckDelay();
	
    if (ret==0)
		{
			Sys_Param.bat_vol_ad = adc_getBatV_ADC();
		}	
		else if (ret==2)
		{
			bat_chargeRecover(); 
			
			show_batv_flag = 1;
		}
	
	  /* ˢ����ʾ */
		Show_Process();  

	  if ( SYSTEM_ON == SYS_OFF) return;
		
		if ( Sys_Param.flag.smoke_firing == 1 ) return; 
		  			
			/*��ʼ��ʱ��������Щ������ȷ���ϵ粻���κθ���*/
			if (Sys_Param.active_mode!=INIT_MODE) 
			{
				button_Up_Detect();
				
				button_Dn_Detect();   
			
				button_Cfm_Detect();			
				
				if ( Sys_Param.active_event == Null_Event || Sys_Param.active_event == MenuConfig_Event) 
				{  			
					uint8_t keyFun = 0;
					
					if (Sys_Param.active_event == Null_Event)
					{
				    keyFun = key_combinationFuction();
					}
					
					if (keyFun==0)
					{					
						keyFun = keyApp_getAddDecStatus();

						sys_addDecKeyFunctionHandle(keyFun);
					}
					else if (keyFun==0x0F)
					{
						Sys_Param.flag.system_locked = 1;
						Sys_Param.active_event = Lock_Event;
					}
				}
			}	
}

void task_100msHandle(void)
{
	    if (Task_TimeOut(Status_Task)==0)  return; 
	
	    static uint16_t keyFirePressDelay;
	
			if (button_Fire_Press())
			{
				keyFirePressDelay++;
				
				if (keyFirePressDelay >= 300) //������ѹ30�룬�Զ��ػ�
				{
					Sys_Param.idle_count = 0;
					keyFirePressDelay = 0;
					Sys_Param.active_mode = SLEEP_MODE;
				}
			}
			else
			{
				keyFirePressDelay = 0;
			}

		  if ( Sys_Param.flag.smoke_firing == 1 )
			{  
					/****************** ����ʱ����� ******************/
					Sys_Param.puff_time++;
				
				  if (Sys_Param.puff_time == 3)
					{
						Sys_Param.flag.puff_count_enable = 1;  // ʹ�����̿�������
					}
			}
			else 
			{        
					
					/********** ��ص������ **********/
				  if (SYSTEM_ON)
					{
						  if (Sys_Param.active_mode != MENU_MODE 
								&& Sys_Param.active_event==Null_Event
							  && button_fire.short_count==0)
							{
								if (smoke_scanRes()) 
								{
									sys_shiftChargeSmokeMode(); //100ms ɨ��Ĵ��� ATO�б仯����ӦҲ�ص���������ʾ����ͬ�ڰ����̰���Ч��
									Sys_Param.idle_count = 0; //�б䶯 �����¼�ʱ
									
									if (Sys_Param.load_res!=OPEN_RES)
									{
										Sys_Param.active_event = CheckRes_Event;
									}
								}
							}
					}
				
					if (Sys_Param.active_mode == CHARGE_MODE)   // ����״̬��ϵͳ�������߳����ʾ
					{								
						oled_showChargeBattery(Sys_Param.battery_level);	// ��ʾ�����ȼ�
					}
					
					bat_chargeChangeLevel();

			}
}

void task_1sHandle(void)
{
	  if (Task_TimeOut(Idle_Task)==0) return;   
	
     //���ʱ�ر���Ļ����ʱ���� 
    if (Sys_Param.active_mode == CHARGE_MODE) 
	  {		 
      if (bat_chargeGetDelay())
			{
				oled_displayOff();
				show_ativer_flag = 0;
				if (SYSTEM_ON)
				{
					button_ledOff();
				}
			}
	  }
		/* ��ϵͳ����ʱ���п��м�ʱ */
		if( (Sys_Param.active_event == Null_Event) && (Sys_Param.flag.smoke_firing == 0)  
			&& (Sys_Param.active_mode != CHARGE_MODE) 
		  && (Sys_Param.active_mode != MENU_MODE))
		{       
				Sys_Param.idle_count++;

				if( SYSTEM_ON )
				{
					  if (flagSysIsDefault==1) /* ���г�ʱ ��������ģʽ */
						{
							 if (Sys_Param.idle_count > AUTO_IDLE_TIMEOUT) {
							
									Sys_Param.idle_count = 0;
									Sys_Param.active_mode = SLEEP_MODE;
								}
								else if (Sys_Param.idle_count >= 10 && Sys_Param.flag.system_locked==0)
								{
											Sys_Param.flag.system_locked = 1;
										
											Sys_Param.active_event = Lock_Event;
								}
						}
						else
						{
								if (Sys_Param.idle_count > 3) {					
									Sys_Param.idle_count = 0;
									Sys_Param.active_mode = SLEEP_MODE;
							 }	
						}
				}
				else
				{
						/* ���г�ʱ ��������ģʽ */
						if( (Sys_Param.idle_count > OFF_IDLE_TIMEOUT) ) {
						
								Sys_Param.idle_count = 0;
								Sys_Param.active_mode = SLEEP_MODE;
						}
				}
		}
		else {

				Sys_Param.idle_count = 0;
				
		}    
}

//====================================================���������ʵ�ֺ���=============
void sys_addDecKeyFunctionHandle(uint8_t keyStatus)
{
	  static uint8_t flagWakeup;
	
	  if (keyStatus) Sys_Param.idle_count = 0; // ������м��� 
	
	  uint8_t ret = 0; 
	
		switch(keyStatus)
		{
				case 0x01:   
					
						if (MENU_MODE!=Sys_Param.active_mode && button_fire.state == button_idle) 
						{
							sys_shiftSmokeMode();
              							
							if (sys_shiftChargeSmokeMode())
							{
								flagWakeup = 1;
							}
						}
						
						break;
				
				case 0x02: //single dec
					
				    if (MENU_MODE==Sys_Param.active_mode) 
						{
							menu_adjustHomepage(0x10);	
							Sys_Param.menu_timeOut = 10000; //10S��ʱ ��������
						}
            else if (sys_keyAddDecEn()) 
						{
							if (flagWakeup) flagWakeup = 0;
						  else smoke_powerDataDec();		 
						}							
				
						break;
				
				case 0x03: //continue dec
					
				    if (MENU_MODE==Sys_Param.active_mode) 
						{
							
						}
				    else if (sys_keyAddDecEn()) 
						{
							smoke_powerDataContinueDec();
						}
	
						break;
				
				case 0x04:					
	
				    if (flagSysIsDefault==1)
						{
							if (flagNewLogic==0)
							{
								if( Sys_Param.flag.system_locked == 0)
								{							
									Sys_Param.active_event = ReadVersion_Event;
								}
							}
						}
						else
						{
							if( Sys_Param.flag.system_locked == 0)
							{							
								Sys_Param.active_event = ResAdjust_Event;
							}
						}
				   
						break;						
				
				case 0x11:
					
						if (MENU_MODE!=Sys_Param.active_mode && button_fire.state == button_idle)
						{
							sys_shiftSmokeMode(); 
							
							if (sys_shiftChargeSmokeMode())
							{
								flagWakeup = 1;
							}
						}
				
						break;
				
				case 0x12: //single add
					
					  if (MENU_MODE==Sys_Param.active_mode) 
						{
							menu_adjustHomepage(0x01);
							Sys_Param.menu_timeOut = 10000; //10S��ʱ ��������
						}
				    else if (sys_keyAddDecEn()) 
						{
							if (flagWakeup) flagWakeup = 0;
						  else ret = smoke_powerDataAdd();
						}
						
				    if (ret) Sys_Param.active_event = PowerIsMax_Event;
						
						break;
				
				case 0x13: //continue add
					
				    if (MENU_MODE==Sys_Param.active_mode) 
						{
							
						}
				    else if (sys_keyAddDecEn()) 
						{
							ret = smoke_powerDataContinueAdd();
						}
				    
				    if (ret) Sys_Param.active_event = PowerIsMax_Event;
		
						break;
				
				case 0x14:
//#if (LOCK_IS_NEW_LOGIC==0)	
            if (flagNewLogic==0)
						{							
							if( Sys_Param.flag.system_locked == 0)
							{							
								Sys_Param.active_event = Factory_Event;		
						
								System_restoreDefault(); 
							}
						}
//#endif
				
						break;
				
				case 0x88: //ͬʱ����
						
					  break;
				
				
				
				default:
						break;
		}					
}

//��֤ok�����޸ĺ�������
void sys_new01_fireKeyFree(void);
void sys_new02_fireKeyPress(void);
void sys_new03_fireKeyContinue3Press(void);
void sys_new05_fireKeyContinue5Press(void);    

void sys_fireKeyFunctionHandle(uint8_t keyStatus)
{
	  uint8_t flagMaskContinue;
	
	  if (keyStatus>=0x02) Sys_Param.idle_count = 0; // ������м��� 
	  
		if ( button_dn.press_count  >=3000
			&& button_up.press_count  >=3000 //
		  && SYSTEM_ON) //����ػ��ĸ���
		{
			flagMaskContinue = 1;
		}
		else
		{
			flagMaskContinue = 0;
		}
	
	  switch (keyStatus)
		{
				case 0x01:  //�ͷ�                    
						sys_new01_fireKeyFree();                  
						break;
				
				case 0x02:
						sys_new02_fireKeyPress();
						break;
				
				case 0x03:
						if (flagMaskContinue==0) sys_new03_fireKeyContinue3Press();
						break;
				
				case 0x04:
					  
					 if (flagMaskContinue==1) //��������Ч
				   {
							Sys_Param.active_event = ModeConfig_Event;
					
							if (flagNewLogic==0) flagNewLogic = 1;
							else flagNewLogic = 0;				
				    }
					  break;
				
				case 0x05:
						if (flagMaskContinue==0) sys_new05_fireKeyContinue5Press();
						break;
				
				default:
						break;
		}					
}

void sys_new01_fireKeyFree(void)
{
		/**** ֹͣ����� �����ͷŶ��� ****/
		if( Sys_Param.flag.smoke_firing == 1 )
		{
			      sys_warnProcess(SYS_CHARGE_ON);	
            show_batv_flag = 1;	  			
		}
}

void sys_new02_fireKeyPress(void)
{
	  if ( SYSTEM_ON )
		{     
        sys_shiftChargeSmokeMode();
			
			  if (Sys_Param.active_event == CheckRes_Event)
				{
					Sys_Param.active_event = Null_Event;
				}
		}
		else if (show_ativer_flag==0)
		{
			  if (Sys_Param.active_mode==CHARGE_MODE) Sys_Param.prev_mode = SLEEP_MODE; //������ʾ����ʱ
		}
}

/****************** ���ܼ���� ��ĳЩ�¼�ʱ������������******************/
void sys_new03_fireKeyContinue3Press(void)
{
	  if ( SYSTEM_ON )
		{                 
						/****************** ���ܼ���� ��ĳЩ�¼�ʱ������������******************/
						
						/**** ϵͳ����/�������� ****/
						if( Sys_Param.flag.system_locked == 1 ) 
						{
										Sys_Param.flag.system_locked = 0;
										Sys_Param.active_event = Unlock_Event;
						}
						else
						{
								    sys_shiftMenuMode();														
						}
						
		}
}

void sys_new05_fireKeyContinue5Press(void)
{
	 /****************** ���ܼ���� ��ĳЩ�¼�ʱ������������******************/
		
		
		/**** ϵͳ�ػ�/�������� ****/
		if ( SYSTEM_ON )
		{
						Sys_Param.active_event = SystemOff_Event;
						Sys_Param.active_mode = IDLE_MODE;  // �л���IDLE_MODE������Ͻ���ػ�
		}
		else
		{
						Sys_Param.active_mode = INIT_MODE;  // �л���������ʼ��ģʽ 
						oled_Init(0x00);	
		}	
}


void sys_fireSmokeRun(void) //1ms����1��
 {
					if ( Sys_Param.flag.smoke_firing == 1 )
					{
							/* ������ʵ��� */         
							smoke_outPowerRegulation(0x00);  

              /**** �����ۼ� ****/
							if ( Sys_Param.puff_time >= 3 )  // ���ʱ��>300ms
							{
								if ( Sys_Param.flag.puff_count_enable == 1 )
								{
										Sys_Param.flag.puff_count_enable = 0;
							
										if( *(sysChannel.puff_count)  < 999)  // ��������999���Զ�����
										{
														(*(sysChannel.puff_count))++;
										}
										else 
										{
														*(sysChannel.puff_count)  = 0;
										}		
								}
						 }	

             	/****************** ���ʱ��⴦�� ******************/
						
								/* �����· */
								if( Sys_Param.flag.firing_short == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);	
									
									Sys_Param.active_event = ResLow_Event;
								
								}
								/* �����· */
								else if( Sys_Param.flag.firing_open == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);	
									
									Sys_Param.active_event = ResHigh_Event;
									
								}
								
								/* ϵͳ�¶ȹ��� */
								else if( Sys_Param.flag.firing_temp_high == 1)
								{
									sys_warnProcess(SYS_CHARGE_OFF);
										
									Sys_Param.active_event = OverHeat_Event;
									Sys_Param.flag.sys_high_temp = 1; 
								}		

								/* ��ص������� */
								else if( Sys_Param.flag.firing_bat_low == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);

									Sys_Param.active_event = VolLow_Event;
									
								}
								/* ���ʱ�䳬ʱ */
								if ( Sys_Param.puff_time >= 100 )  // ���ʱ�¼�
								{
										sys_warnProcess(SYS_CHARGE_ON);
										
										Sys_Param.active_event = OverTime_Event;		
								}							 
					}
					else
					{
							/* ���̼����ʱ */
							if( Sys_Param.cease_fire_count < CEASE_FIRING_TIMEOUT )
							{
									Sys_Param.cease_fire_count++;
							}
					}
}



