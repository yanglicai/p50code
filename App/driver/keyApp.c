/**
  ******************************************************************************
  * @file    keyApp.c
  * @author  
  * @version V1.0.0
  * @date    2019.12.16
  * @brief   
   按键检测 返回对应状态 放在10ms定时或轮询中
  */

/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "button.h"	 

#define BUTTON_SILDER_IS_EN  1 // 1 使能滑动 0 屏蔽滑动

typedef enum
{
	Key_Idle  = 0,	
	Key_Slide = 0x20,
}KEY_STATE;

KEY_STATE     sys_keyState;

uint8_t sliderTouchCnt;

void keyApp_silderInit(void)
{
	sys_keyState = Key_Idle;
	sliderTouchCnt = 0;
}

uint8_t key_combinationFuction(void)
{
	#if (BUTTON_SILDER_IS_EN==1)
	 
	  uint8_t ret = 0;	 
	 
	 if (sliderTouchCnt)
	 {		
     if (button_Up_Press() && button_Dn_Press())		 
			 ret = 0x00;
		 else
		   ret = 0x0F;
	 }
	
   keyApp_silderClear();
	
	 return ret;
	 
	#else
	 return 0;
	#endif
}

void keyApp_silderClear(void)
{
	if (sliderTouchCnt) sliderTouchCnt = 0;
}

void keyApp_silderCheckAdd(void)
{
	if ((button_cfm.loosen_count < button_dn.loosen_count)
		&& (button_dn.loosen_count < 500) 
//	  && (button_dn.state == button_idle)
	  && (button_dn.loosen_count - button_cfm.loosen_count > 50 ))	
	{
		sliderTouchCnt = 0x03; //011 左滑动
	}  	
}

void keyApp_silderCheckCfm(void)
{
//	if (button_dn.loosen_count < 300) 	
//	{
//		sliderTouchCnt = 0x03; //011 左滑动
//	}	
//  else 	if (button_up.loosen_count < 300) 
//	{
//		sliderTouchCnt = 0x06; //110 右滑动
//	}			
}

void keyApp_silderCheckDec(void)
{
	if ((button_cfm.loosen_count < button_up.loosen_count) 	
		&& (button_up.loosen_count < 500) 
//	  && (button_up.state == button_idle)
	  && (button_up.loosen_count - button_cfm.loosen_count > 50 ))	  
	{
		sliderTouchCnt = 0x06; //110 右滑动
	}	
}

/**
  * @brief  外部调用 函数 返回按键按下 空闲等的状态的参数
  * @param  None
  * @retval None
  */
uint8_t keyApp_fireIsPress(void)  //点火键按下 不是短按 长按
{
	 if (button_fire.state == button_press)
		 return 1;
	 else 
		 return 0;
}	

uint8_t keyApp_addIsPress(void)  
{
	 if (button_up.state == button_press)
		 return 1;
	 else 
		 return 0;
}	

uint8_t keyApp_decIsPress(void)  
{
	 if (button_dn.state == button_press)
		 return 1;
	 else 
		 return 0;
}	

uint8_t keyApp_cfmIsPress(void)  
{
	 if (button_cfm.state == button_press)
		 return 1;
	 else 
		 return 0;
}

uint8_t keyApp_fireIsIdle(void)    //点火键空闲
{
	 if (button_fire.state == button_idle)
		 return 1;
	 else 
		 return 0;
}	

uint8_t keyApp_addDecIsIdle(void)  //非吸烟按键
{
	if (button_up.state + button_dn.state == button_idle)
		return 1;
	else
		return 0;
}


uint8_t keyApp_allIsIdle(void)   //所有按键空闲
{
	if (button_up.state + button_dn.state + button_fire.state== button_idle)
		return 1;
	else
		return 0;
}


/**
  * @brief  按键和外部的交互处理：非点火键 功能的处理，比如加减功率 比如显示处理等；
  * @param  None
  * @retval None

  */
static uint16_t  fire_keep_count = 0;  
static uint16_t  add_keep_count = 0;  
static uint16_t  dec_keep_count = 0;

uint8_t keyApp_getAddDecStatus(void)
{
         if ( button_dn.press_count  >=3000
					&& button_up.press_count  >=3000
				    ) 
				{
					return 0xF0;  
				}
				
				if ((button_cfm.state != button_idle) 
					&& (button_dn.state != button_idle) 
				  && (button_up.state != button_idle)) return 0xFF;
			
        /****************** DN按键操作处理 ******************/
				if (button_dn.state == button_idle)  // 按键空闲处理
				{
					dec_keep_count = 0;
				}
				else if ( button_dn.state == button_press)  // 按键按下处理 
				{ 					
					return 0x01;
				}
        else if (button_dn.state == button_short_press)  // 按键短按处理 
				{					
					/**** 功率单点减少 ****/
					if (button_up.state == button_idle) 
					{   						
            return 0x02;              
					} 
					else
					{
					  return 0xF0;  
					}
				}
        else if (button_dn.state == button_long_press)  
				{  						
					/**** 功率持续减少 ****/
					if (button_up.state == button_idle && button_fire.state == button_idle)
					{
             dec_keep_count++;

						if (dec_keep_count>=1) { 
                            
              dec_keep_count = 0;
						
						  return 0x03;
						}
					}
					else if ( button_fire.state == button_press )  //组合按键显示
					{   						
							/* 长按超过3S */	
							if( fire_keep_count < 100)
							{
								fire_keep_count++;
								
								if( fire_keep_count == 100)
								{ 
									if (button_fire.press_count < button_dn.press_count) //判断先后顺序 通过按下的时间长短
									  return 0x04;                
								}							
							} 
						  							
					}
					else if ( button_up.state == button_long_press && button_dn.state == button_idle) //组合按键显示
					{   						
							/* 长按超过3S */
							if( add_keep_count < 100)
							{
								add_keep_count++;
								
								if( add_keep_count == 100)
								{ 
									return 0x88;                
								}							
							} 												
					}
					
					return 0xF0; 
				}
				
				/****************** UP按键操作处理 ******************/  
				if (button_up.state == button_idle)  // 按键空闲处理
				{
					add_keep_count = 0;			
				}
				else if ( button_up.state == button_press )  // 按键按下处理 
				{  					
					return 0x11;  
				}
				else if (button_up.state == button_short_press)  // 按键短按处理 
				{  					
					/**** 功率单点增加 ****/
					if (button_dn.state == button_idle)
					{						
            return 0x12;
					}
					else
					{
					  return 0xF0;  
					}
				}
				else if (button_up.state == button_long_press)  // 按键长按处理 
				{					
					/**** 功率连续减少 ****/
					if ( button_dn.state == button_idle  && button_fire.state == button_idle)
					{
						add_keep_count++;

						if (add_keep_count>=1) {  // 长按功率自加间隔

							add_keep_count = 0;  

							return 0x13;
						}						
					}
					else if ( button_fire.state == button_press) //组合按键显示
					{   						
						/* 长按超过3S*/
						 //判断先后顺序 通过按下的时间长短
						
							if( fire_keep_count < 100)
							{
								fire_keep_count++;
								
								if( fire_keep_count == 100)
								{ 
									 if (button_fire.press_count < button_up.press_count) //判断先后顺序 通过按下的时间长短
									   return 0x14;                
								}							
							} 
																		
					}
					else if ( button_dn.state == button_long_press && button_up.state == button_idle ) //组合按键显示
					{   						
							/* 长按超过3S */
							if( dec_keep_count < 150)
							{
								dec_keep_count++;
								
								if( dec_keep_count == 150)
								{ 
									return 0x88;                
								}							
							} 												
					}
					
					return 0xF0;
				} 
		
		    return 0;
}

/**
  * @brief  按键和外部的交互处理：点火按键对应的功能处理：启动点火 口数累计
  * @param  None
  * @retval None
放在 button_Fire_Detect 下一步使用

提供按键状态，在这种状态下，按键是否起效，由系统根据当前条件来判断
  */
uint8_t keyApp_getFireStatus(void)
{
	 /****************** FIRE按键操作处理 ******************/
		
		if ( button_fire.state != button_press )  // 按键松开
		{  
				if (button_fire.state == button_3_press)  // 按键连按3次处理 
				{    
						return 0x03;
				} 
        else if(button_fire.state == button_4_press)  // 按键连按4次处理 
				{    
						return 0x04;
				} 			
				else if( button_fire.state == button_5_press )  // 按键连按5次处理 
				{                 
						return 0x05;  					  
				}
				
				fire_keep_count = 0;
			
				return 0x01;  									  
		}
		else if ( button_fire.state == button_press )  // 按键按下处理 
		{			
				return 0x02;  
		}
		
		return 0x00;
}

//===========================END OF FILES====================================

