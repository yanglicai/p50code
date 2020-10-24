
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

//uint8_t debugCnt; //显示调试结果 辅助调试
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
  * @brief  sys_dataConfig 数据处理 采用指针 方便移植
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
  * @brief  允许按键 加减动作的条件判断.
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
  * @brief  点火键短按的模式处理 触摸短按 但是不加减
  * @param  None
  * @retval None
  */
uint8_t sys_shiftChargeSmokeMode(void)
{					
	/* 在休眠充电的情况下按下点火键切换模式 */
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
	/* 系统上锁时点火键按下显示 LOCKED */
	if( Sys_Param.flag.system_locked == 1)  
	{
		Sys_Param.active_event = Lock_Event;
	}
}



/**
  * @brief  切换到菜单模式
  * @param  None
  * @retval None
  */
void sys_shiftMenuMode(void)
{		
	/* 进入到菜单 */	
	Sys_Param.active_mode = MENU_MODE;	
}

extern uint8_t smoke_scanRes(void);
extern uint8_t smoke_startScanRes(void);
extern uint8_t smoke_rungingScanRes(void);


/**
  * @brief  系统模式管理.
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
			
			if ( Sys_Param.prev_mode != INIT_MODE )  /* 按键开机时要初始化 */
			{				
				Sys_Param.prev_mode = INIT_MODE;
				
				Sys_Param.flag.system_on_off = SYS_ON;				
				
				Sys_Param.active_event = Init_Event;	
				
				driver_init(Sys_Param.flag.system_on_off); 
				
				smoke_startScanRes(); //启动阻值检测
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
				
				Firing_Process();  // 点火处理
			}
      else if (bat_getUsbStatusIsr())
			{
				Sys_Param.active_mode = CHARGE_MODE;
				if (Sys_Param.wakeup_type == button_WAKEUP
					|| Sys_Param.wakeup_type == ATO_WAKEUP)  // 按键唤醒
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
				
				Sys_Param.active_event = MenuConfig_Event; //确保不会有显示方面的干扰
				
				oled_Show_Clear(0); //请屏幕 显示进入菜单
				
				menu_showHomepage();	
				
				Sys_Param.menu_timeOut = 10000;
			}
			else if (Sys_Param.menu_timeOut==0)
			{
				 Sys_Param.active_mode = SYS_RUN_MODE;
				
				 Sys_Param.prev_event = Sys_Param.active_event;//确保一定会刷新处理
				
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
				bat_chargeSetDelay(AUTO_IDLE_TIMEOUT); //延时30秒关屏
				
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
					Sys_Param.active_mode = SYS_RUN_MODE; //恢复主界面
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
  * @brief  警告处理
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
  * @brief  点火处理.
  * @param  None
  * @retval None
  */
static void Firing_Process(void)
{
	static uint8_t process_flag = 0;

	/****************** 点火前检测处理 ******************/
	if (Sys_Param.flag.smoke_firing == 0)
	{
		if (keyApp_fireIsPress() && button_fire.short_count==0) 
		{  		
			/* 在无事件和系统无锁定时可以点火 */
			if( (keyApp_addDecIsIdle() || (flagNewLogic==1) || (Sys_Param.flag.system_locked==1)) 
				&& ( Sys_Param.active_event == Null_Event
          || Sys_Param.active_event == Wakeup_Event 			
			    || Sys_Param.active_event == Lock_Event 
				  || Sys_Param.active_event == PowerIsMax_Event)  
			  && (process_flag == 0) )
			{
	
				if( Sys_Param.flag.sys_high_temp == 1 )
				{
					Sys_Param.out_temp_ad = adc_getOutTemp_ADC();  // 获取系统温度AD
				
					if (flagSysIsDefault==1)
					{
						if( Sys_Param.out_temp_ad > MIN_TEMP )//温度低于60℃ 
						{
							Sys_Param.flag.sys_high_temp = 0;
						}	
				  }
					else
					{
						if( Sys_Param.out_temp_ad > TEST40_TEMP )//温度低于45℃ 
						{
							Sys_Param.flag.sys_high_temp = 0;
						}	
					}
				} 

				smoke_calParameter(0x00);      
                
				/* 没发生任何故障事件 */
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
						
						FIRE_TIME_START(); // 启动点火定时器
						
				}
				else 
				{
					bat_setCheckDelay(100);
				}
			}

			process_flag = 1;  // 防止按键不松开连续动作

		}
		else {

			process_flag = 0;
		}

	}
}


/**
  * @brief  显示处理.
  * @param  None
  * @retval None
  */
extern uint8_t  oled_Show_BatV(uint16_t batVData);

static void Show_Process(void)
{
	if( (SYSTEM_ON) && (Sys_Param.active_mode != CHARGE_MODE) )
	{
		if (Sys_Param.active_mode == MENU_MODE) return;
		
		/* 事件切换处理 */
		if( Sys_Param.prev_event != Sys_Param.active_event ) 
		{
			Sys_Param.prev_event = Sys_Param.active_event;
			
			if (flagSysIsDefault==1)
			{
			  Sys_Param.show_timeout = SHOW_TIMEOUT; // 重置显示时间
			}
			else
			{
				Sys_Param.show_timeout = SHOW_TEST_TIMEOUT;
			}
			Sys_Param.flag.show_event = 1;  // 更新
			
			oled_Show_Clear(1); 					
		}

		/* 对应事件显示 */
		switch (Sys_Param.active_event)
		{
			/***************** 无事件 *****************/
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
			    oled_showHomePageBattery(Sys_Param.battery_level);  // 显示电量等级
				}

				oled_Show_Puff(*(sysChannel.puff_count));		 // 显示吸烟口数
				oled_Show_Time(Sys_Param.puff_time);         // 显示吸烟时间
				oled_Show_AtoRes(Sys_Param.load_res);
				oled_Show_Power(*(sysChannel.set_power));    // 显示设置功率 	  
      
			break;
				
				
			/***************** ATO变化事件 *****************/
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

			/***************** 初始化事件 *****************/
			case Init_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					if (flagSysIsDefault==1)
					{
						oled_Show_Snowwolf(); // 显示开机LOGO	
		
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
				
						oled_Show_Snowwolf(); // 显示开机LOGO	
					}	
				}
			break;

			/***************** 开/关机事件 *****************/
			case SystemOff_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_SysOff(); // 关机LOGO
				}
			break;

			/***************** 过温事件 *****************/
			case OverHeat_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_OverHeat(); // 过温LOGO
				}
			break;

			/***************** 电阻高事件 *****************/
			case ResHigh_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_CheckPod(); // 空载LOGO
					
					smoke_startScanRes();
				}
			break;

			/***************** 吸烟超时事件 *****************/
			case OverTime_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_OverTime(); // 超时LOGO
				}
			break;

			/***************** 电压低事件 *****************/
			case VolLow_Event:

				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_BatteryLow(); // 低电量LOGO
				}
			break;

			/***************** 短路事件 *****************/
			case ResLow_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Short(); // 短路LOGO();
					smoke_startScanRes();
					Sys_Param.load_res = SHORT_RES;
				}
				
			break;

			/***************** 上锁事件 *****************/
			case Lock_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Locked(); // 上锁LOGO
				}
			break;

			/***************** 解锁事件 *****************/
			case Unlock_Event:
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_Unlock(); // 解锁LOGO
				}
			break;
				
			/***************** 设置达到最大功率 *****************/
			case PowerIsMax_Event:
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					oled_Show_PowerIsMax(); // 功率达到最大值的提示
				}
			break;
				
			/***************** 读取版本 *****************/
			case ReadVersion_Event: 
				
				if( Sys_Param.flag.show_event == 1 ) 
				{
					Display_setFgColor(COLOR_TEAL);
					
					if (flagNewLogic==0)  // 读取软件版本
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
					oled_Show_RestoreDefault(); // 恢复默认
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

			      Flash_Write_Page(page_buffer,15);  //立即写入
					}
					
					Display_setFgColor(COLOR_WHITE);
				}
			break;				
				
				
				
			default:
				break;
		}
		

		/* 显示完毕和事件重置 */
		if ( Sys_Param.active_event != Null_Event) {
			
			if (Sys_Param.show_timeout > 0)
			{				
				/* 按键按下延时显示(防止按键操作时进入 Null_Event) */
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
				
				/* 在按键释放后才重置事件 */
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
  * @brief  系统休眠处理.
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
		 System_writeConfigData(0); //有变化才写入
	 }

	/* 进入休眠*/
	if (bat_getUsbStatusIsr()==0)
	{	
		sys_sleepMode(Sys_Param.flag.system_on_off);      

		sys_exitSleep(Sys_Param.flag.system_on_off);   

		sys_getWakeUpType();

		/* 休眠时按键唤醒或USB唤醒时 开启9V和oled模块 根据需要来处理*/		
		
		if ( Sys_Param.wakeup_type == USB_WAKEUP )  // USB唤醒进入充电模式
		{
			if (SYSTEM_ON)
			{
				Sys_Param.active_mode = SYS_RUN_MODE; 

        Sys_Param.idle_count = AUTO_IDLE_TIMEOUT - OFF_IDLE_TIMEOUT; //5秒钟马上进入充电模式
      }
      else
			{ 
				Sys_Param.active_mode = CHARGE_MODE; 
			}				   					
		}
		else   // 按键唤醒进入点火模式
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
		
	  Task_Init();  //放在最后，确保唤醒操作不会影响时间片轮初始化
		
		if (flagSysIsDefault==1)
		{
		  Sys_Param.flag.system_locked = 1; //需要锁定显示，做测试
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
//时间片任务=======================================================================

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
	
	  /* 刷屏显示 */
		Show_Process();  

	  if ( SYSTEM_ON == SYS_OFF) return;
		
		if ( Sys_Param.flag.smoke_firing == 1 ) return; 
		  			
			/*初始化时，屏蔽这些操作，确保上电不受任何干扰*/
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
				
				if (keyFirePressDelay >= 300) //持续按压30秒，自动关机
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
					/****************** 吸烟时间更新 ******************/
					Sys_Param.puff_time++;
				
				  if (Sys_Param.puff_time == 3)
					{
						Sys_Param.flag.puff_count_enable = 1;  // 使能吸烟口数计数
					}
			}
			else 
			{        
					
					/********** 电池电量检测 **********/
				  if (SYSTEM_ON)
					{
						  if (Sys_Param.active_mode != MENU_MODE 
								&& Sys_Param.active_event==Null_Event
							  && button_fire.short_count==0)
							{
								if (smoke_scanRes()) 
								{
									sys_shiftChargeSmokeMode(); //100ms 扫描的处理 ATO有变化，对应也回到主界面显示，等同于按键短按的效果
									Sys_Param.idle_count = 0; //有变动 就重新计时
									
									if (Sys_Param.load_res!=OPEN_RES)
									{
										Sys_Param.active_event = CheckRes_Event;
									}
								}
							}
					}
				
					if (Sys_Param.active_mode == CHARGE_MODE)   // 开机状态或系统进入休眠充电显示
					{								
						oled_showChargeBattery(Sys_Param.battery_level);	// 显示电量等级
					}
					
					bat_chargeChangeLevel();

			}
}

void task_1sHandle(void)
{
	  if (Task_TimeOut(Idle_Task)==0) return;   
	
     //充电时关闭屏幕的延时处理 
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
		/* 当系统空闲时进行空闲计时 */
		if( (Sys_Param.active_event == Null_Event) && (Sys_Param.flag.smoke_firing == 0)  
			&& (Sys_Param.active_mode != CHARGE_MODE) 
		  && (Sys_Param.active_mode != MENU_MODE))
		{       
				Sys_Param.idle_count++;

				if( SYSTEM_ON )
				{
					  if (flagSysIsDefault==1) /* 空闲超时 进入休眠模式 */
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
						/* 空闲超时 进入休眠模式 */
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

//====================================================具体任务的实现函数=============
void sys_addDecKeyFunctionHandle(uint8_t keyStatus)
{
	  static uint8_t flagWakeup;
	
	  if (keyStatus) Sys_Param.idle_count = 0; // 清除空闲计数 
	
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
							Sys_Param.menu_timeOut = 10000; //10S限时 重新启动
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
							Sys_Param.menu_timeOut = 10000; //10S限时 重新启动
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
				
				case 0x88: //同时长按
						
					  break;
				
				
				
				default:
						break;
		}					
}

//验证ok后再修改函数名字
void sys_new01_fireKeyFree(void);
void sys_new02_fireKeyPress(void);
void sys_new03_fireKeyContinue3Press(void);
void sys_new05_fireKeyContinue5Press(void);    

void sys_fireKeyFunctionHandle(uint8_t keyStatus)
{
	  uint8_t flagMaskContinue;
	
	  if (keyStatus>=0x02) Sys_Param.idle_count = 0; // 清除空闲计数 
	  
		if ( button_dn.press_count  >=3000
			&& button_up.press_count  >=3000 //
		  && SYSTEM_ON) //避免关机的干扰
		{
			flagMaskContinue = 1;
		}
		else
		{
			flagMaskContinue = 0;
		}
	
	  switch (keyStatus)
		{
				case 0x01:  //释放                    
						sys_new01_fireKeyFree();                  
						break;
				
				case 0x02:
						sys_new02_fireKeyPress();
						break;
				
				case 0x03:
						if (flagMaskContinue==0) sys_new03_fireKeyContinue3Press();
						break;
				
				case 0x04:
					  
					 if (flagMaskContinue==1) //开机才有效
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
		/**** 停止点火处理 正常释放动作 ****/
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
			  if (Sys_Param.active_mode==CHARGE_MODE) Sys_Param.prev_mode = SLEEP_MODE; //激活显示的延时
		}
}

/****************** 功能键检测 在某些事件时按键还能作用******************/
void sys_new03_fireKeyContinue3Press(void)
{
	  if ( SYSTEM_ON )
		{                 
						/****************** 功能键检测 在某些事件时按键还能作用******************/
						
						/**** 系统锁定/解锁处理 ****/
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
	 /****************** 功能键检测 在某些事件时按键还能作用******************/
		
		
		/**** 系统关机/开机处理 ****/
		if ( SYSTEM_ON )
		{
						Sys_Param.active_event = SystemOff_Event;
						Sys_Param.active_mode = IDLE_MODE;  // 切换到IDLE_MODE处理完毕进入关机
		}
		else
		{
						Sys_Param.active_mode = INIT_MODE;  // 切换到开机初始化模式 
						oled_Init(0x00);	
		}	
}


void sys_fireSmokeRun(void) //1ms调用1次
 {
					if ( Sys_Param.flag.smoke_firing == 1 )
					{
							/* 输出功率调节 */         
							smoke_outPowerRegulation(0x00);  

              /**** 口数累计 ****/
							if ( Sys_Param.puff_time >= 3 )  // 点火时间>300ms
							{
								if ( Sys_Param.flag.puff_count_enable == 1 )
								{
										Sys_Param.flag.puff_count_enable = 0;
							
										if( *(sysChannel.puff_count)  < 999)  // 口数超过999后自动清零
										{
														(*(sysChannel.puff_count))++;
										}
										else 
										{
														*(sysChannel.puff_count)  = 0;
										}		
								}
						 }	

             	/****************** 点火时检测处理 ******************/
						
								/* 输出短路 */
								if( Sys_Param.flag.firing_short == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);	
									
									Sys_Param.active_event = ResLow_Event;
								
								}
								/* 输出开路 */
								else if( Sys_Param.flag.firing_open == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);	
									
									Sys_Param.active_event = ResHigh_Event;
									
								}
								
								/* 系统温度过高 */
								else if( Sys_Param.flag.firing_temp_high == 1)
								{
									sys_warnProcess(SYS_CHARGE_OFF);
										
									Sys_Param.active_event = OverHeat_Event;
									Sys_Param.flag.sys_high_temp = 1; 
								}		

								/* 电池电量过低 */
								else if( Sys_Param.flag.firing_bat_low == 1)
								{
									sys_warnProcess(SYS_CHARGE_ON);

									Sys_Param.active_event = VolLow_Event;
									
								}
								/* 点火时间超时 */
								if ( Sys_Param.puff_time >= 100 )  // 点火超时事件
								{
										sys_warnProcess(SYS_CHARGE_ON);
										
										Sys_Param.active_event = OverTime_Event;		
								}							 
					}
					else
					{
							/* 吸烟间隔计时 */
							if( Sys_Param.cease_fire_count < CEASE_FIRING_TIMEOUT )
							{
									Sys_Param.cease_fire_count++;
							}
					}
}



