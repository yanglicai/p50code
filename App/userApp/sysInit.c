/**
  ******************************************************************************
  * @file    sysInit.c 
  * @author  Figo
  * @version V1.0.0
  * @date    11-10-2019
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/



#include "typedef.h"

#include "app_task.h"
#include "menu.h"
#include "flash.h"

uint8_t flagSysIsDefault; //1 系统已经恢复默认 0 系统没有恢复默认

void System_Init(void);
void System_InitParameter(void);
extern void smoke_setoffset(uint16_t offsetData);
extern uint16_t smoke_getoffset(void);
extern void Flash_SetData(uint16_t *pBuff,uint16_t size,uint16_t setData);

void System_readConfigData(void);

/**
  * @brief  系统初始化 
  * @param  None
  * @retval None				 
  */
void System_Init(void)
{    
  /* 系统参数初始化 */
	System_InitParameter();
  
  sys_dataConfig(Sys_Param.out_chNum);
	
	smoke_setoffset(120);
	
	System_readConfigData();
}
/**
  * @brief  系统参数初始化 
  * @param  None
  * @retval None				 
  */

void System_InitParameter(void)
{	    
	Sys_Param.active_mode   = INIT_MODE;
	Sys_Param.prev_mode  = IDLE_MODE;
	Sys_Param.active_event  = Init_Event;
	Sys_Param.prev_event = Null_Event;
	Sys_Param.adc_gather_channel = OUT_VOLTAGE;
	Sys_Param.wakeup_type = NULL_WAKEUP;
	
	Sys_Param.flag.system_on_off     = SYS_ON;
	Sys_Param.flag.smoke_firing      = 0;
	Sys_Param.flag.smoke_short       = 0;
	Sys_Param.flag.system_locked     = 0;
	Sys_Param.flag.show_event        = 0;
	Sys_Param.flag.out_on            = 0;
	Sys_Param.flag.firing_short      = 0;
	Sys_Param.flag.firing_temp_high  = 0;
	Sys_Param.flag.firing_bat_low    = 0;
	Sys_Param.flag.firing_open       = 0;
	Sys_Param.checkOpenCnt           = 0;
	Sys_Param.flag.power_out_enable  = 0;
	Sys_Param.flag.puff_count_enable = 0;
	Sys_Param.flag.sys_high_temp     = 1;
  
 
	Sys_Param.set_power       = DEFAULT_POWER; 
	Sys_Param.load_res        = OPEN_RES;
 
	Sys_Param.out_phase0_ad   = 0;
	Sys_Param.out_phase1_ad   = 0;
	Sys_Param.out_phase2_ad   = 0;
	Sys_Param.out_phase3_ad   = 0;     
	Sys_Param.out_vol_ad      = 0;
  
  Sys_Param.out_chNum        = 0; //通道编号     
	Sys_Param.puff_time        = 0; 
  Sys_Param.puff_count       = 0;    
	Sys_Param.battery_level    = 0; 
	Sys_Param.show_timeout     = SHOW_TIMEOUT;
	Sys_Param.idle_count       = 1; //上电更快休眠 方便测试

	Sys_Param.bat_vol_ad       = 0;
	Sys_Param.out_curr_ad      = 0;
	Sys_Param.out_temp_ad      = 0;
	Sys_Param.max_limit_i      = 0;
	Sys_Param.short_ad         = SHORT_CURR; // 15A
	Sys_Param.count_10ms       = 0;
	Sys_Param.bat_low_count    = 0;
	Sys_Param.cease_fire_count = CEASE_FIRING_TIMEOUT;
	Sys_Param.charge_full_count= 0;
	Sys_Param.chargeTime = 0;
	
	flagNewLogic = 0;
	flagSysIsDefault= 0;
}

void System_copyConfigData(void)
{
	page_buffer[2] = menu_getMode(); //mode 互相比较来判断是否需要备份
	
	page_buffer[5] = Sys_Param.set_power; //power

	page_buffer[8] = Sys_Param.puff_count; //puffer count
	
	page_buffer[11] = flagSysIsDefault; //default
	
	page_buffer[14] = smoke_getoffset(); //offset 
	
}

uint8_t System_compareConfigData(void)
{
	System_copyConfigData();
	
	uint8_t m,n;
	
	for (m=0;m<5;m++)
	{
		n = m * 3;
		if (page_buffer[n+1] != page_buffer[n+2]) return 1; //有变化就存储 原始值是0xFF
	}
  
	return 0;
	
}

void System_writeConfigData(uint8_t writeType) //1强制写入 0 变化才写入
{
	if (writeType==0)
	{
	  if (System_compareConfigData()==0) return;
	}
	
	page_buffer[0] = 0x01;
	page_buffer[1] = menu_getMode(); //mode 互相比较来判断是否需要备份
	page_buffer[2] = page_buffer[1];
	
	page_buffer[3] = 0x02;
	page_buffer[4] = Sys_Param.set_power; //power
  page_buffer[5] = page_buffer[4];
	
	page_buffer[6] = 0x03;
	page_buffer[7] = Sys_Param.puff_count; //puffer count
	page_buffer[8] = page_buffer[7];
	
	page_buffer[9]  = 0x04;
	page_buffer[10] = flagSysIsDefault; //default
	page_buffer[11] = page_buffer[10];
	
	page_buffer[12]  = 0x05;
	page_buffer[13] = smoke_getoffset(); //offset
  page_buffer[14] = page_buffer[13];
	
	Flash_Write_Page(page_buffer,15);  
}

void System_readConfigData(void)
{
	Flash_SetData(page_buffer,15,0xFFFF);
	
	Flash_Read_Page(page_buffer,15); //3个数据为一个组
	
	if (page_buffer[0] == 0x01)
	{
		 menu_setMode((uint8_t)(page_buffer[1]));
	}
	
	if (page_buffer[3] == 0x02)
	{
		 Sys_Param.set_power = page_buffer[4];
	}
	
	if (page_buffer[6] == 0x03)
	{
		 Sys_Param.puff_count = page_buffer[7];
	}
	
	if (page_buffer[9] == 0x04)
	{
		 flagSysIsDefault = page_buffer[10];
	}
	
	if (page_buffer[12] == 0x05)
	{
		 smoke_setoffset(page_buffer[13]);
	}
}


void System_restoreDefault(void)
{				
	Sys_Param.set_power = MIN_POWER;

	Sys_Param.puff_count      = 0;  
  Sys_Param.puff_time       = 0;  
	
	menu_deInit();
	
	flagSysIsDefault= 1;
	
	System_writeConfigData(1); //强制写入
}

