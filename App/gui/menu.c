/**
  ******************************************************************************
  * @file    menu.c 
  * @author  figo
  * @version V1.0.0
  * @date   04-01-2020
  * @brief     
	
  */	
  
/* Define to prevent recursive inclusion -------------------------------------*/

/* Includes ------------------------------------------------------------------*/


#include "myMath.h"

#include "Fonts.h"
#include "app_task.h"
#include "delay.h"
#include "gui.h"
#include "uiTab.h"
#include "oled.h"

#include <string.h>
#include "menu.h"


#define FONT_LEN (8+2)
#define FONT_WIDTH 16

volatile uint8_t startX,startY,len,width;
static uint8_t itemNum;

static uint8_t menuLevel,modeConfigCntBak;
static uint8_t modeConfigCnt = 0;

uint8_t GUI_MenuItemSelectOrder(uint8_t *menuItem, uint8_t maxNum, uint8_t addDecFlag);

static void menu_show_auto(uint8_t type);
static void menu_show_rba(uint8_t type);

static void menu_show_TitleMode(void);

static void menu_show_HomepageMode(uint8_t type);
static void menu_show_HomepageVersion(uint8_t type);
static void menu_show_HomepageReset(uint8_t type);


void menu_selectHomepageItem(uint8_t type);
void menu_showSlecetMode(uint8_t type);

void menu_deInit(void)
{
	modeConfigCntBak = 0;
	modeConfigCnt = 1;
}

//相关参数读取 方便外部访问处理
//0 RBA 1 AUTO
uint8_t menu_getMode(void)
{
	return modeConfigCnt;
}

void menu_setMode(uint8_t modeType)
{
	modeConfigCnt = modeType;
}

//enter 
void menu_showHomepage(void)
{	
	menuLevel = 0; //层级	
	itemNum = 0;
	
//#if (LOCK_IS_NEW_LOGIC==1)
	if (flagNewLogic==1)
	{	
		menu_show_HomepageMode(1);
		menu_show_HomepageVersion(0);
		menu_show_HomepageReset(0);
  }
	else
//#else
	{
	  menu_show_HomepageMode(0);
	  menu_showSlecetMode(modeConfigCnt);
  }
//#endif
 
	modeConfigCntBak = menu_getMode();
}

//select
//select
void menu_selectHomepageItem(uint8_t type)
{
	if (type==0)
	{
		menu_show_HomepageMode(1);
	  menu_show_HomepageVersion(0);
		menu_show_HomepageReset(0);
	}
	else if (type==1)
	{
		menu_show_HomepageMode(0);
	  menu_show_HomepageVersion(1);
		menu_show_HomepageReset(0);
	}
	else if (type==2)
	{
		menu_show_HomepageMode(0);
	  menu_show_HomepageVersion(0);
		menu_show_HomepageReset(1);
	}
}

void menu_showSlecetMode(uint8_t type)
{
	if (type)
	{
		menu_show_auto(1);	
		menu_show_rba(0);
	}
	else
	{
		menu_show_auto(0);	
		menu_show_rba(1);
	}
}  

extern void System_restoreDefault(void);

void menu_adjustHomepage(uint8_t addDecFlag)
{
//#if (LOCK_IS_NEW_LOGIC==1)
	if (flagNewLogic==1)
	{
		switch (menuLevel) 
		{
			case 0: //第一级菜单
				
				GUI_MenuItemSelectOrder(&itemNum,2,addDecFlag);
				menu_selectHomepageItem(itemNum);
			
				break;
			
			case 1: //第二级菜单
				
				if (itemNum==0) //mode 
				{
					GUI_MenuItemSelectOrder(&modeConfigCnt,1,addDecFlag);
					menu_showSlecetMode(modeConfigCnt);
				}
				
				break;
			
			default:
				break;
		}	
	}
	else
//#else
  {
     GUI_MenuItemSelectOrder(&modeConfigCnt,1,addDecFlag);
	   menu_showSlecetMode(modeConfigCnt);
	}
//#endif
}

//确认当前选择，进入下一级菜单或者退出菜单
void menu_confirmHandle(void) 
{
//#if (LOCK_IS_NEW_LOGIC==1)
  if (flagNewLogic==1)
 {	
		if (menuLevel==0)
		{
			menuLevel = 1;
			
			if (itemNum==0)
			{			
				oled_Show_Clear(0);
				
				menu_show_TitleMode();
				
				menu_showSlecetMode(modeConfigCnt);			
			}
			else
			{
				Sys_Param.active_mode = SYS_RUN_MODE;

				if (itemNum==1) //version
				{
					Sys_Param.active_event = ReadVersion_Event;	
				}
				else if (itemNum==2) //reset 
				{
					Sys_Param.active_event = Factory_Event;

					System_restoreDefault(); 				
				}	    			
			}
		}
		else if (menuLevel==1)//exit
		{
			Sys_Param.active_mode = SYS_RUN_MODE; 

			Sys_Param.active_event = Wakeup_Event; 	 		
		}	
	}
	else
//#else	
	{
		Sys_Param.active_mode = SYS_RUN_MODE; 
		
		if (itemNum==0)
		{
			Sys_Param.active_event = Wakeup_Event; 	
		}
		else if (itemNum==1) //version
		{
			Sys_Param.active_event = ReadVersion_Event;	
		}
		else if (itemNum==2) //reset 
		{
			Sys_Param.active_event = Factory_Event;	
		}	
	}

//#endif	
}

//exit to shift judge
uint8_t menu_modeShiftToAuto(void)
{ 	
	if (modeConfigCntBak!=modeConfigCnt && modeConfigCnt)
	{
		modeConfigCntBak = modeConfigCnt;
		
	  return 1; //相同为0 相异为 1  且当前为AUTO
	}
	return 0;
}

//===========================================
static void menu_show_TitleMode(void)
{
	startX = 60;
	startY = 20;
	
	len = FONT_LEN * 4 + 6;
	width = FONT_WIDTH + 4;
	
	menu_displayString("MODE",startX,startY,1);	
}

static void menu_show_HomepageMode(uint8_t type)
{
	startX = 60;
//#if (LOCK_IS_NEW_LOGIC==1)	
	if (flagNewLogic==1)
	{
	  startY = 20;
  }
	else
//#else
	{
    startY = 20;
		Display_setFgColor(COLOR_TEAL);
	}
//#endif	
	
	len = FONT_LEN * 4 + 6;
	width = FONT_WIDTH + 4;
	
	menu_displayString("MODE",startX,startY,1);
	
	Display_setFgColor(COLOR_WHITE);
	
	mYdraw_rect(startX - 3,startY - 2,len,width,type,COLOR_BLACK,COLOR_TEAL);	//COLOR_TEAL	 COLOR_WHITE
}



static void menu_show_HomepageVersion(uint8_t type)
{
	startX = 20;
	startY = 52;
	
	len = FONT_LEN * 7 + 4;
	width = FONT_WIDTH + 4;
	
	menu_displayString("VERSION",startX,startY,1);
	
	mYdraw_rect(startX - 3,startY - 2,len,width,type,COLOR_BLACK,COLOR_TEAL);	
	
}
	
static void menu_show_HomepageReset(uint8_t type)
{
	startX = 100;
	startY = 52;
	
	len = FONT_LEN * 5 + 5;
	width = FONT_WIDTH + 4;
	
	menu_displayString("RESET",startX,startY,1);
	
	mYdraw_rect(startX - 3,startY - 2,len,width,type,COLOR_BLACK,COLOR_TEAL);	
	
}




//===========================================

void menu_show_rba(uint8_t type)
{
	startX = 20;
	startY = 52;
	len = FONT_LEN * 3 + 6;
	width = FONT_WIDTH + 4;
	
	menu_displayString("RBA",startX,startY,1);
	
	mYdraw_rect(startX - 3,startY - 2,len,width,type,COLOR_BLACK,COLOR_TEAL);
}

void menu_show_auto(uint8_t type)
{
	startX = 100;
	startY = 52;
	
	len = FONT_LEN * 4 + 6;
	width = FONT_WIDTH + 4;
	menu_displayString("AUTO",startX,startY,1);
	
	mYdraw_rect(startX - 3,startY - 2,len,width,type,COLOR_BLACK,COLOR_TEAL);
}


/*******************************************************************************
* Function Name  : uint8_t GUI_MenuItemSelectOrder(uint8_t *menuItem,uint8_t
8_t maxNum, uint8_t addDecFlag))
* Description    : 菜单选项的指针，最大编号
*******************************************************************************/
uint8_t GUI_MenuItemSelectOrder(uint8_t *menuItem, uint8_t maxNum, uint8_t addDecFlag)
{
	if (addDecFlag==0x10)
	{				
		if (*menuItem > 0)
			*menuItem -= 1;
		else
			*menuItem = maxNum;
	}

	if (addDecFlag==0x01)
	{
		//加按键。

		*menuItem += 1;
		if (*menuItem > maxNum)
		{
			*menuItem = 0;
		}		
	}	

	return addDecFlag;
}



