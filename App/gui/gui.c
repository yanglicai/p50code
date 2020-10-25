/**
  ******************************************************************************
  * @file    gui.c 
  * @author  figo
  * @version V1.0.0
  * @date    19-12-2019
  * @brief     
gui：font,icon,bmp,function
依赖文件
oled.c 
fonts.c 对应字体 图标在里面定义

被谁调用：
app_task.c 中交互处理

数据层：
1.设置地址操作范围；

应用层：
1.清屏；
2.芯片初始化；

实现的功能：
1. 显示的开 关
2. 显示的清屏
3. 显示各类的图标，显示主ui，显示充电图；

显示分类：
1.不同字体；
2.带小数点，不带小数点的；
3.单纯图标，比如SNOWWOLF这个logo图标；
	
*/
	
	
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "gui.h"

#include "myMath.h"

#include "Fonts.h"
#include "app_task.h"
#include "delay.h"

#include "uiTab.h"
#include "oled.h"

#include "menu.h"

#include <string.h>

char *buildTime = __TIME__;
char *buildDate = __DATE__;

char *strVersion = "P50 V2.0A";
char *NewstrVersion = "P50 V2.0B";

char  time[5];

#define oled_CMD	0  //写命令
#define oled_DATA	1  //写数据  
byteBit show;

extern void oled_writeCmd(unsigned char Data);
extern void oled_writeData(unsigned char Data);

extern void oled_setPos(uint8_t column,uint8_t page);
extern void oled_clearScreen(void);  
extern void oled_writeCmdData(uint8_t dat, uint8_t cmd);

extern void display_32highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,
	uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color,uint16_t bColor);

/**
  * @brief  oled 显示标志的设置
  * @param  None
  * @retval None
**/ 
void oled_setAllFlag(void)
{
  show_puff_flag = 1;
	show_time_flag = 1;
  show_res_flag = 1;
	show_power_flag = 1;
	show_battery_flag = 1;	
	show_batv_flag = 1;
}


/**
  * @brief  oled清屏
  * @param  None  96 * 16
  * @retval None
**/  
void oled_Show_Clear(uint8_t type)
{
	
	oled_clearScreen();

	if (type==1)
	{
	  oled_setAllFlag();
	}
}


/**
  * @brief  显示电池电压
  * @param  puff
  * @retval None
**/ 
static void display_batV(uint16_t batV)
{
		unsigned char tem=0;
		unsigned char num1=0;
		unsigned char num2=0;
		unsigned char num3=0;
	
	 #define BATV_Y 68
	 #define BATV_Y2 BATV_Y + 8 

		num1=batV/100;
		tem=batV%100; 
		num2=tem/10;
		num3=tem%10; 	
	 
		display_16HighData(125, 129, BATV_Y,BATV_Y2,9,5,  (num1+1),font5x16tabe,0xFFFF,0x0000);//显示电阻的阻值.
		display_16HighData(130, 132, BATV_Y,BATV_Y2,9,3,  1,       font3x16tabe,0xFFFF,0x0000);  //显示小数点	
		display_16HighData(133, 137, BATV_Y,BATV_Y2,9,5,  (num2+1),font5x16tabe,0xFFFF,0x0000);	
		display_16HighData(141, 145, BATV_Y,BATV_Y2,9,5,  (num3+1),font5x16tabe,0xFFFF,0x0000);	
	
		mYdraw_picture(148 , BATV_Y + 1, 6, 8 ,gImage_V_UNIT);
}

uint8_t  oled_Show_BatV(uint16_t batVData)
{
	static uint16_t past_batV = 0xffff;	
	static uint8_t batOkCnt;
	
	uint16_t delt;
	
	delt = myCal_IntegerDelt(past_batV,batVData);

	if ((delt > 5 && batOkCnt++ > 10) || (show_batv_flag == 1))
	{				
		show_batv_flag = 0;
		
		batOkCnt = 0;
			
		display_batV(batVData);	

    past_batV = batVData;	

		Sys_Param.charge_changeFlag = 1;		

    return 1;		
	}	
	
	return 0;
}

/**
  * @brief  显示吸烟口数
  * @param  puff 左上角
  * @retval None
**/ 

static void display_puffCnt(uint16_t puff)
{
		unsigned char tem=0;
		unsigned char num1=0;
		unsigned char num2=0;
		unsigned char num3=0;

	  volatile uint8_t puffCntX;  
    #define PUFF_CNT_Y  2
	  
		num1 = puff/100;
		tem = puff%100; 
		num2 = tem/10;
		num3 = tem%10; 	

	  puffCntX = 2;
		display_16HighData(puffCntX ,puffCntX + 4,PUFF_CNT_Y,PUFF_CNT_Y + 8,9,5,(num1+1),font5x16tabe,0xFFFF,0x0000); 
    puffCntX += 7;	
		display_16HighData(puffCntX ,puffCntX + 4,PUFF_CNT_Y,PUFF_CNT_Y + 8,9,5,(num2+1),font5x16tabe,0xFFFF,0x0000); 
    puffCntX += 7;		
		display_16HighData(puffCntX ,puffCntX + 4,PUFF_CNT_Y,PUFF_CNT_Y + 8,9,5,(num3+1),font5x16tabe,0xFFFF,0x0000);   
	  puffCntX += 7;	
	  mYdraw_picture(puffCntX + 2 ,  PUFF_CNT_Y + 1,  6, 8 ,gImage_P_UNIT);
}

void oled_Show_Puff(uint16_t puff)
{
	static uint16_t past_puff = 0xffff;	

	if((past_puff != puff) || (show_puff_flag == 1))
	{
		show_puff_flag = 0;
		
		display_puffCnt(puff);
	}

	past_puff = puff;
}

#define PUFFTIME_Y   2

static void display_puffTime(uint8_t time)
{
	unsigned char tem=0;
	unsigned char num1=0;
	unsigned char num2=0;
	
  if (time > 100) return;

	if (time < 100)
	{
		num1=time/10;
		tem=time%10; 
		num2=tem;		

		display_16HighData(128,132,PUFFTIME_Y,PUFFTIME_Y + 8,9,5,(num1+1),font5x16tabe,0xFFFF,0x0000);
		display_16HighData(135,137,PUFFTIME_Y,PUFFTIME_Y + 8,9,3,1,font3x16tabe,0xFFFF,0x0000);  //显示小数点    
		display_16HighData(140,144,PUFFTIME_Y,PUFFTIME_Y + 8,9,5,(num2+1),font5x16tabe,0xFFFF,0x0000);  
	}
	else
	{
		display_16HighData(128,132,PUFFTIME_Y,PUFFTIME_Y + 8,9,5,(1+1),font5x16tabe,0xFFFF,0x0000);  
		display_16HighData(136,140,PUFFTIME_Y,PUFFTIME_Y + 8,9,5,(0+1),font5x16tabe,0xFFFF,0x0000);  
	}
	
	mYdraw_picture(148 , PUFFTIME_Y + 1 , 6, 8 ,gImage_S_UNIT);
}


/**
  * @brief  显示吸烟时间
  * @param  time: 100ms    
  * @retval None    
**/ 
void oled_Show_Time(uint8_t time)
{
	static uint8_t past_time = 0xff;

	if( (past_time != time) || (show_time_flag == 1) )
	{
		show_time_flag = 0;	

    if (past_time >= 99)		
		{
			display_clear(128,PUFFTIME_Y,16,9);
		}
		
    display_puffTime(time);
	}
	past_time = time;

}

/**
  * @brief  显示ATO阻值
  * @param  time: 100ms    
  * @retval None    
**/ 
/*********************************************
函数名称:oled_Show_Resistance
函数说明:显示电阻阻值函数 
*********************************************/
void oled_Show_CheckResistance(unsigned short r)
{		
	 if (r > 999) return;
	 
	 unsigned short tem2; 
	unsigned short tem1; 

  unsigned char  num2;
  unsigned char  num3;
	unsigned char  num4;
	unsigned short colourval=0;

	volatile uint8_t startX = 44,startX2 = 37,startY = 40,startY2 = 71,len = 13;
 
	colourval = COLOR_WHITE;
	

	tem1=(r%1000);
	num2=(tem1/100);
	tem2=(tem1%100);
	num3=(tem2/10);	
  num4=(tem2%10);			
	
	startX2 = startX + len;	
	display_32highData(startX,startX2,startY,startY2,32,14,num2+1,num_14X32,colourval,0x0000);	
	
	startX = startX2 + 5;	
	startX2 = startX + 4;	
	display_32highData(startX,startX2,startY,startY2,32,5,1,font5x32tabe,colourval,0x0000);
	
	startX = startX2 + 5;	
	startX2 = startX + len;		
	display_32highData(startX,startX2,startY,startY2,32,14,num3+1,num_14X32,colourval,0x0000);
	
	startX = startX2 + 5;	
	startX2 = startX + len;	
	display_32highData(startX,startX2,startY,startY2,32,14,num4+1,num_14X32,colourval,0x0000);	
}

void oled_showMessaggeAtoRes(uint16_t res)
{    
	uint16_t show_res;
	
	if (res > MAX_RES) res = 9990; 
    
  show_res = (uint16_t)(res / 10);
	
  oled_Show_CheckResistance(show_res);		
}

//主界面显示处理
void oled_Show_Resistance(unsigned short r)
{		
	 if (r > 999) return;
	 {	
			unsigned char tem=0;
			unsigned char num1=0;
			unsigned char num2=0;
			unsigned char num3=0;

      #define ATO_R_Y    68
		  #define ATO_R_Y2   ATO_R_Y + 8
		 
			num1=r/100;
			tem=r%100; 
			num2=tem/10;
			num3=tem%10; 	
		 
			display_16HighData(2, 6, ATO_R_Y,ATO_R_Y2,9,5,(num1+1),font5x16tabe,0xFFFF,0x0000);//显示电阻的阻值.
			display_16HighData(7, 9, ATO_R_Y,ATO_R_Y2,9,3,1,font3x16tabe,0xFFFF,0x0000);  //显示小数点	
			display_16HighData(10,14,ATO_R_Y,ATO_R_Y2,9,5,(num2+1),font5x16tabe,0xFFFF,0x0000);	
			display_16HighData(18,22,ATO_R_Y,ATO_R_Y2,9,5,(num3+1),font5x16tabe,0xFFFF,0x0000);	
		 
			mYdraw_picture(26 , ATO_R_Y + 1, 8, 8 ,gImage_R_UNIT);
   }	
}

void oled_Show_AtoRes(uint16_t res)
{   
	  static uint16_t past_res = 0xffff;	  
    
    uint16_t show_res;
    
    if (res > MAX_RES) res = 9990; 
    
    show_res = (uint16_t)(res / 10); 

		if( (past_res != res) || (show_res_flag == 1) )
		{
				show_res_flag = 0;

				oled_Show_Resistance(show_res);		
			
			  past_res = res;
		}
}


/**
  * @brief  显示功率
  * @param  power 0~40    
  * @retval None
**/ 


static void display_power(unsigned short Pra)
{
	unsigned short tem2; 
	unsigned short tem1; 

  unsigned char  num2;
  unsigned char  num3;
	unsigned char  num4;
	unsigned short colourval=0;

	volatile uint8_t startX = 44,startX2 = 37,startY = 20,startY2 = 51,len = 13;

	 
	colourval = COLOR_WHITE;
		
	tem1=(Pra%1000);
	num2=(tem1/100);
	tem2=(tem1%100);
	num3=(tem2/10);	
  num4=(tem2%10);			
	
  //显示100W以下带小数点。
	startX2 = startX + len;	
	display_32highData(startX,startX2,startY,startY2,32,14,num2+1,num_14X32,colourval,0x0000);	
	startX = startX2 + 5;	
	startX2 = startX + len;		
	display_32highData(startX,startX2,startY,startY2,32,14,num3+1,num_14X32,colourval,0x0000);
	
	startX = startX2 + 5;	
	startX2 = startX + 4;	
	display_32highData(startX,startX2,startY,startY2,32,5,1,font5x32tabe,colourval,0x0000);
	
	startX = startX2 + 5;	
	startX2 = startX + len;	
	display_32highData(startX,startX2,startY,startY2,32,14,num4+1,num_14X32,colourval,0x0000);		   		 
  
  mYdraw_picture(106 , 40 , 10, 14 ,gImage_W);
}

void oled_Show_Power(uint16_t power)
{
	static uint8_t past_power = 0xff;

	if( (past_power != power) || (show_power_flag == 1) )
	{
		show_power_flag = 0;
		
		display_power(power);	
	}
	
	past_power = power;
}

/**
  * @brief  显示通道
  * @param  power 1~3   
  * @retval None
**/ 


/**
  * @brief  显示电池电量  主界面的显示
  * @param  
  * @retval None
**/ 
#define MAX_BATTERY_LEN     26 //主界面电池条最大长度
#define HOME_BATTER_START_X (66 + 2) //2 66

void oled_showHomePageBattery(uint8_t level)
{
	static uint8_t past_level = 0xff;
	
	uint8_t temp_level = level;

	if( (past_level != level) || (show_battery_flag == 1) )
	{
		if (show_battery_flag)
		{
		  show_battery_flag = 0;
			past_level = 0xff;
		}

		if (temp_level > BATTERY_MAX_LEVEL)
		{
			temp_level = BATTERY_MAX_LEVEL;
		}	
		volatile uint16_t batLevelLen;
		
		if (temp_level < BATTERY_MAX_LEVEL)
		{
		  batLevelLen = temp_level * MAX_BATTERY_LEN / BATTERY_MAX_LEVEL;
		}
		else
		{
			batLevelLen = MAX_BATTERY_LEN;
		}
	 
		//user code
		batLevelLen = HOME_BATTER_START_X + batLevelLen - 1;
		
		if (past_level > level && past_level!=0xff)
		{
			draw_fillRect(batLevelLen,2,HOME_BATTER_START_X + MAX_BATTERY_LEN - 1,9,COLOR_BLACK);	
		}
		else
		{
		  draw_fillRect(HOME_BATTER_START_X,2,batLevelLen,9,COLOR_WHITE);	
		}
	}
	
	past_level = level;

}

//==========充电电池 大电池 和主界面UI无关======================================
/**
  * @brief  显示充电电池电量
  * @param  level 0~32
  * @retval None
**/ 
extern const unsigned char gImage_BAT_B[656];
extern const unsigned char gImage_BAT_T[656]; 
extern const unsigned char gImage_BAT_L[384];
extern const unsigned char gImage_BAT_R[576];

#define FLASH_CHARGE_BOX_START_X  40
#define FLASH_CHARGE_BAT_START_X  FLASH_CHARGE_BOX_START_X + 6
#define FLASH_CHARGE_FLASH_TECH_START_X FLASH_CHARGE_BOX_START_X - 8


void display_chargeBatBox(void)//充电模式电池框架图标
{
	mYdraw_picture(FLASH_CHARGE_BOX_START_X + 1,20,6,32,gImage_BAT_L);
	mYdraw_picture(FLASH_CHARGE_BOX_START_X + 76,20,9,32,gImage_BAT_R);
	mYdraw_picture(FLASH_CHARGE_BOX_START_X,20,82,4,gImage_BAT_T);
	mYdraw_picture(FLASH_CHARGE_BOX_START_X,50,82,4,gImage_BAT_B);
}


#define MAX_CHARGE_LEN 72    //最大电池条长度

void oled_showChargeBattery(uint8_t level)
{
	uint8_t temp_level = level;
	
	static uint8_t past_level = 0xff;   
	static uint8_t pastStartX;

	if( (past_level != level) || (show_charge_battery_flag == 1) )
	{
		if (show_charge_battery_flag)
		{
			show_charge_battery_flag = 0;  
			oled_Show_Clear(0);	
			display_chargeBatBox();
			
			past_level = 0xff;	
		  
      pastStartX = FLASH_CHARGE_BAT_START_X;	      		
		}		

		if (temp_level > BATTERY_MAX_LEVEL) 
		{
			temp_level = BATTERY_MAX_LEVEL;
		}
		
		volatile uint16_t batLevelLen;
		
		if (temp_level < BATTERY_MAX_LEVEL) 
		{		
			batLevelLen = temp_level * MAX_CHARGE_LEN /BATTERY_MAX_LEVEL ;		
		}
		else
		{
			batLevelLen = MAX_CHARGE_LEN;
		}
		//user code
		batLevelLen = FLASH_CHARGE_BAT_START_X + batLevelLen - 1;
		
		if (past_level > level && past_level!=0xff)
		{
			draw_fillRect(batLevelLen,26,FLASH_CHARGE_BAT_START_X + MAX_CHARGE_LEN - 1,50,COLOR_BLACK);	
			
			pastStartX = batLevelLen;
		}
		else
		{
		  draw_fillRect(pastStartX,26,batLevelLen,50,COLOR_WHITE);	
			
			pastStartX = batLevelLen;
		}				
	}
	
	past_level = level;
}

//==========logo 保留动态显示和常规显示两种方式======================================


/**
  * @brief  oled_Show_BackUI 显示主界面的背景以及公共元素
  * @param  None   
  * @retval None
**/ 
extern void draw_hollowCircle(uint8_t disbit, uint8_t x, uint8_t y, uint8_t r, uint32_t color);

#define OFFSET_X  6
#define OFFSET_Y  0

void oled_Show_BackUI(void)
{
	  mYdraw_picture(66 ,  0 , 31, 12  ,gImage_batUI_0_0_31x12);	
    mYdraw_picture(30 ,  3 , 21, 75  ,gImage_circle1);		
	  mYdraw_picture(110 ,  3 , 21, 75  ,gImage_circle2);	
	
	  mYdraw_picture(2 ,    39 , 28, 4  ,gImage_lineBlue);	
	  mYdraw_picture(132 ,  39 , 28, 4  ,gImage_lineBlue);	
	
		mYdraw_picture(8  ,  22,  16, 10 ,gImage_P_ICON);
		mYdraw_picture(12  , 47,  8,  14 ,gImage_R_ICON);	
	  mYdraw_picture(138 , 20,  14, 14 ,gImage_T_ICON);
	  mYdraw_picture(140 , 46,  8,  14 ,gImage_U_ICON);	
	
	  Display_setFgColor(COLOR_TEAL);
	
		if (menu_getMode()==0) //RBA
		{
			menu_displayString("RBA",68,62,1); 
		}
		else
		{			
			menu_displayString("AUTO",64,62,1); 
		}	  

		Display_setFgColor(COLOR_WHITE);
				
		if (Sys_Param.flag.system_locked==1)
		{
			 mYdraw_picture(104 ,  22 , 16, 14 ,gImage_green_Lock);
		}
		else
		{
			 mYdraw_picture(104 ,  22 , 16, 14 ,gImage_green_Unlock);
		}	
}

void oled_Show_Version(uint8_t x,uint8_t y)
{
	menu_displayString(strVersion,x,y,1);
	
	#if (DEBUG_IS_DOING==1)
	strncpy(time, buildTime, 5);	
  menu_displayString(time,x + 100,y,1);	
	#else
//	strncpy(time, buildDate, 5);	
//  menu_displayString(time,x + 100,y,1);	
	#endif
}

void oled_Show_NewVersion(uint8_t x,uint8_t y)
{
	menu_displayString(NewstrVersion,x,y,1);
	
	#if (DEBUG_IS_DOING==1)
	strncpy(time, buildTime, 5);	
  menu_displayString(time,x + 100,y,1);	
	#else
//	strncpy(time, buildDate, 5);	
//  menu_displayString(time,x + 100,y,1);	
	#endif
}



/**
  * @brief  显示logo1(SNOWWOLF)
  * @param  None   
  * @retval None
**/ 
void oled_Show_Snowwolf(void)
{
	uint8_t byeStartX;
	
	byeStartX = 20 ;
	
	uint8_t  lineNum,num;
	uint16_t lenBytes,bytesNum,bytesNum2,bytesSum;

	lenBytes = 116 * 2;
	bytesSum = lenBytes * 24;
	lineNum = 11;
	
	for (num=0;num < 12;num++)
	{
		bytesNum = lineNum * lenBytes;
		mYdraw_picture(byeStartX , 20 + lineNum,     116, 1 ,(uint8_t*)&gImage_snowwolf_116x24[bytesNum]);
		bytesNum2 = bytesSum - bytesNum - lenBytes; 
		mYdraw_picture(byeStartX , 20 + 23 - lineNum ,116, 1 ,(uint8_t*)&gImage_snowwolf_116x24[bytesNum2]);
		systick_delay_ms(65);
		
		if (lineNum) lineNum--;		
	}
	
	 mYdraw_picture(20,20,116,24,gImage_snowwolf_116x24);
}

/**
  * @brief  显示logo2(BYE)
  * @param  None   
  * @retval None
**/ 
void oled_Show_SysOff(void)
{	
	uint8_t byeStartX;
	
	byeStartX = (160 - 51) / 2 ;
	
	mYdraw_picture(byeStartX , 30 , 51, 20 ,gImage_BYE);
	
	systick_delay_ms(500);
	
	uint8_t  width;
	
	for (width=0;width < 10;width++)
	{
		mYdraw_picture(byeStartX , 30 + width , 51, 1 ,commonBlack);
		mYdraw_picture(byeStartX , 30 + 19 - width , 51, 1 ,commonBlack);
		systick_delay_ms(75);
	}
}

//以下部分为报警，共同点是背景图
/**
  * @brief  显示logo3(OVER HEAT)
  * @param  None   
  * @retval None
**/ 
void oled_Show_OverHeat(void)
{
	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	
	menu_displayString("OVER HEAT",40,55,1);
}


/**
  * @brief  显示logo4(CHECK POD)
  * @param  None   
  * @retval None (160 - 132) / 2
**/ 
void oled_Show_CheckPod(void)
{
	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	
//	mYdraw_picture((160 - 106) / 2 ,55 ,106,16,gImage_CHECK_POD_106x16);
	menu_displayString("CHECK POD",40,55,1);
}


/**
  * @brief  显示logo5(OVER FIRE)
  * @param  None   
  * @retval None
**/ 
void oled_Show_OverTime(void)
{
	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	menu_displayString("OVER FIRE",40,55,1);
}


/**
  * @brief  显示logo6(BATTERY LOW)
  * @param  None   
  * @retval None
**/ 
void oled_Show_BatteryLow(void)
{
	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	menu_displayString("LOW BATTERY ",30,55,1);
}


/**
  * @brief  显示logo7(SHORT)
  * @param  None   
  * @retval None
**/ 
void oled_Show_Short(void)
{
	mYdraw_picture((160 - 45) / 2 , 2  ,45, 40 ,gImage_WARN_BACK1_45x40);
//	mYdraw_picture((160 - 68) / 2 , 55 ,68, 16 ,gImage_SHORT_68x16);
	menu_displayString("SHORT",60,55,1);
}


/**
  * @brief  显示logo8(LOCKED)
  * @param  None   
  * @retval None
**/ 
void oled_Show_Locked(void)
{
//	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	menu_displayString("LOCKED",50,35,1);
	mYdraw_picture(120 ,  35 , 16, 14 ,gImage_green_Lock);
}


/**
  * @brief  显示logo9(UNLOCKED)
  * @param  None   
  * @retval None
**/ 
void oled_Show_Unlock(void)
{
//	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);

	menu_displayString("UNLOCK",50,35,1);
	mYdraw_picture(120 ,  35 , 16, 14 ,gImage_green_Unlock);
}

/**
  * @brief  显示logo10(FACTORY)
  * @param  None   
  * @retval None
**/ 
void oled_Show_RestoreDefault(void)
{
//	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	menu_displayString("RESTORE DEFAULT",20,20,1);
	
	Display_setFgColor(COLOR_TEAL);
	
	if (flagNewLogic==0)  // 读取软件版本
	{
		oled_Show_Version(35,52);
	}
	else
	{
		oled_Show_NewVersion(35,52);
	}
	
	Display_setFgColor(COLOR_WHITE);
}  

/**
  * @brief  显示logo11(MAX POWER)
  * @param  None   
  * @retval None
**/ 
void oled_Show_PowerIsMax(void)
{
	mYdraw_picture((160 - 45) / 2 , 2 , 45, 40 ,gImage_WARN_BACK1_45x40);
	menu_displayString("MAX POWER",40,55,1);
} 

//=======================对外的调用==========================


/**
  * @brief  oled 参数配置初始化
  * @param  None 
  * @retval None
0x36 对应的参数
//0xA0和A是竖向镜像 横向显示   		
//0xE0和A一样 底部显示 横向显示 字横向镜像  
//0xC0竖向显示

**/
void oled_configInit(uint8_t modeType)
{
	oled_writeCmd(0x11);//sleep out
	delay_ms_soft(2);
	
	oled_writeCmd(0x36);//
	oled_writeData(modeType);//RGB 16-bit//
	
	//=====set frame rate=====
	oled_writeCmd(0xB1);//frame rate control	
	oled_writeData(0x05);
	oled_writeData(0x3A);
	oled_writeData(0x3A);  
	
	oled_writeCmd(0xB2);
	oled_writeData(0x05);
	oled_writeData(0x3A);
	oled_writeData(0x3A);
	
	oled_writeCmd(0xB3);
	oled_writeData(0x05);
	oled_writeData(0x3A);
	oled_writeData(0x3A);
	oled_writeData(0x05);
	oled_writeData(0x3A);
	oled_writeData(0x3A);
	
	//=====================================/
	oled_writeCmd(0xB4);//inversion setting
	oled_writeData(0x00);//0x01=dot inversion
	//=====set power sequence=====
	oled_writeCmd(0xC0);//Power Control 1
	oled_writeData(0xAA);//AVDD=5V;GVDD=4.2V
	oled_writeData(0x0A);//GVCL=-4.2V
	oled_writeData(0x84);//Mode=AUTO;VRHP=VRHN=0
	
	oled_writeCmd(0xC1);//Power Control 2
	oled_writeData(0xC0);//V25=2.4V;VGH=2AVDD+VGH25-0.5=11.9V;
									 //         VGL=-7.5V
	
	oled_writeCmd(0xC2);//Power Control 3
	oled_writeData(0x0D);//
	oled_writeData(0x00);//
	
	oled_writeCmd(0xC3);//Power Control 4
	oled_writeData(0x8D);//
	oled_writeData(0x2A);//
	
	oled_writeCmd(0xC4);//Power Control 5
	oled_writeData(0x8D);
	oled_writeData(0xEE);
	//=========================//
	oled_writeCmd(0xC5);//VCOM setting
	oled_writeData(0x06);//
	//==========Gamma=========//
	oled_writeCmd(0xE0);
	oled_writeData(0x06);
	oled_writeData(0x1C);
	oled_writeData(0x10);	
	oled_writeData(0x14);
	oled_writeData(0x30);	
	oled_writeData(0x2B);
	oled_writeData(0x24);
	oled_writeData(0x29);
	oled_writeData(0x28);
	oled_writeData(0x26);
	oled_writeData(0x2E);
	oled_writeData(0x39);
	oled_writeData(0x00);
	oled_writeData(0x02);
	oled_writeData(0x01);
	oled_writeData(0x10);
	
	oled_writeCmd(0xE1);
	oled_writeData(0x06);
	oled_writeData(0x1C);
	oled_writeData(0x10);	
	oled_writeData(0x14);
	oled_writeData(0x30);	
	oled_writeData(0x2B);
	oled_writeData(0x24);
	oled_writeData(0x29);
	oled_writeData(0x28);
	oled_writeData(0x26);
	oled_writeData(0x2E);
	oled_writeData(0x39);
	oled_writeData(0x00);
	oled_writeData(0x02);
	oled_writeData(0x01);
	oled_writeData(0x10);	
#if (OLED_IS_NEW==1)
  oled_writeCmd(0x20);  //inversion on 0x21 0x20 inversion off 
#else
  oled_writeCmd(0x21); 
#endif
	oled_writeCmd(0x3A);//
	oled_writeData(0x05);//RGB 16-bit	 565 
}


/**
  * @brief  oled 初始化
  * @param  None 
  * @retval None
**/
extern void oled_reset(void);  
extern void oled_On(void);

#define MIRROR_Y         0xA8
#define NORMAL_DISPLAY   0x68


void oled_Init(const uint8_t displayType)
{    
	if (displayType==0x00)
	{
		oled_reset();
		
#if (OLED_MIRROR_IS_EN==1)
    oled_configInit(0xA8); 
#else		
		oled_configInit(0x68); //正常显示 0x68 倒影显示 0xA8
#endif
	}
	
	oled_On();
	
	if (displayType==0x00)
	{
	  oled_Show_Clear(1);	
	}	
	
	
	Display_setDrawMode(0); //正常显示
}



//==========END OF FILES=====================================



