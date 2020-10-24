/**
  ******************************************************************************
  * @file    button.c
  * @author  Leo Ou
  * @version V1.0.0
  * @date    06-11-2018
  * @brief   button program body
对模块外使用的函数： 
button_Init  
button_Scan

button_Up_Detect
button_Dn_Detect
button_Fire_Detect

其他内部使用

  */

/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "button.h"	  

#define BUTTON_NEW_MODE      1  //1 用指针来实现 0 直接实现

/* Exported functions ------------------------------------------------------- */

#define BT_DN_PRESS     (HAL_GPIO_ReadPin(TOUCH_K2DEC_GPIO_Port,TOUCH_K2DEC_Pin)==GPIO_PIN_RESET)  
#define BT_DN_FREE       (HAL_GPIO_ReadPin(TOUCH_K2DEC_GPIO_Port,TOUCH_K2DEC_Pin)==GPIO_PIN_SET) 

#define BT_UP_PRESS     (HAL_GPIO_ReadPin(TOUCH_K3ADD_GPIO_Port,TOUCH_K3ADD_Pin)==GPIO_PIN_RESET)  
#define BT_UP_FREE      (HAL_GPIO_ReadPin(TOUCH_K3ADD_GPIO_Port,TOUCH_K3ADD_Pin)==GPIO_PIN_SET)

#define BT_FIRE_PRESS    (HAL_GPIO_ReadPin(KEY_FIRE_GPIO_Port,KEY_FIRE_Pin)==GPIO_PIN_RESET)  
#define BT_FIRE_FREE     (HAL_GPIO_ReadPin(KEY_FIRE_GPIO_Port,KEY_FIRE_Pin)==GPIO_PIN_SET) 

#define BT_CFM_PRESS    (HAL_GPIO_ReadPin(TOUCH_K4_CFM_GPIO_Port,TOUCH_K4_CFM_Pin)==GPIO_PIN_RESET)  
#define BT_CFM_FREE     (HAL_GPIO_ReadPin(TOUCH_K4_CFM_GPIO_Port,TOUCH_K4_CFM_Pin)==GPIO_PIN_SET) 

#define BT_LED_ON        HAL_GPIO_WritePin(T_LED_EN_GPIO_Port, T_LED_EN_Pin, GPIO_PIN_RESET)
#define BT_LED_OFF       HAL_GPIO_WritePin(T_LED_EN_GPIO_Port, T_LED_EN_Pin, GPIO_PIN_SET)

#define BT_VDD_ON        HAL_GPIO_WritePin(TOUCH_VDD_GPIO_Port, TOUCH_VDD_Pin, GPIO_PIN_SET)
#define BT_VDD_OFF       HAL_GPIO_WritePin(TOUCH_VDD_GPIO_Port, TOUCH_VDD_Pin, GPIO_PIN_RESET)


//====================================下面部分和硬件无直接关联，保持不变=================================
       
button_para_t button_up;    
button_para_t button_dn;  
button_para_t button_cfm; 
button_para_t button_fire;

/**
  * @brief  初始化button
  * @param  None
  * @retval None
	使用说明：放在端口初始化之后，
  */
void button_Init(void)
{
	button_up.state = button_idle;  
	button_up.press_count = 0;
	button_up.loosen_count = 0;  
  button_up.pressFun = button_Up_Press;		
	button_up.silderFun = keyApp_silderCheckAdd;		
    
  button_dn.state = button_idle;
	button_dn.press_count = 0;
	button_dn.loosen_count = 0;
	button_dn.pressFun = button_Dn_Press;	
	button_dn.silderFun = keyApp_silderCheckDec;	
	
	button_cfm.state = button_idle;
	button_cfm.press_count = 0;
	button_cfm.loosen_count = 0;	
	button_cfm.pressFun = button_Cfm_Press;
  button_cfm.silderFun = keyApp_silderCheckCfm;		
	
	button_fire.state = button_idle;
	button_fire.press_count = 0;
	button_fire.loosen_count = 0;
	button_fire.short_count = 0;	
	button_fire.pressFun = button_Fire_Press;	
	
}

void button_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	HAL_GPIO_WritePin(T_LED_EN_GPIO_Port, T_LED_EN_Pin, GPIO_PIN_RESET);
	
  /*Configure GPIO pin : T_LED_EN_Pin */
  GPIO_InitStruct.Pin = T_LED_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //GPIO_MODE_INPUT
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(T_LED_EN_GPIO_Port, &GPIO_InitStruct);
	
  /*Configure GPIO pins : TOUCH_VDD_Pin */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pin = TOUCH_VDD_Pin;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(TOUCH_VDD_GPIO_Port, &GPIO_InitStruct); 

  /*Configure GPIO pins : TOUCH_K2DEC_Pin TOUCH_K3ADD_Pin */
  GPIO_InitStruct.Pin = TOUCH_K2DEC_Pin|TOUCH_K3ADD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);  
	
	//2020.01.06 这里修改为浮空输入后，功耗从90uA降低到22uA，就是说因为触摸没有供电，导致这里输入是低电平，导致功耗偏大
	GPIO_InitStruct.Pin = TOUCH_K4_CFM_Pin; 	
  HAL_GPIO_Init(TOUCH_K4_CFM_GPIO_Port, &GPIO_InitStruct);  
}

/**
  * @brief  button_on
  * @param  None
  * @retval None
打开和button相关的电源 led等
  */
void button_on(void)
{
	BT_VDD_ON;
	BT_LED_ON;
}

/**
  * @brief  button_off
  * @param  None
  * @retval None
use: 休眠配置中使用
  */
void button_off(void)
{
	BT_VDD_OFF;
	BT_LED_OFF;	
}	

void button_ledOn(void)
{
	BT_LED_ON;
}

void button_ledOff(void)
{
	BT_LED_OFF;
}


/**
  * @brief  button扫描 (1ms)
  * @param  None
  * @retval None
  */
void button_Scan(void)
{
  
  if (BT_UP_PRESS)  // UP_button按下
	{
		if (button_up.press_count <= 5000)
		{
			button_up.press_count++;
		}
	}
	else {  // 松开计时
		
		if(button_up.loosen_count < 1000) {
			button_up.loosen_count++;
		}
	}
	
	
	if (BT_CFM_PRESS)  // CFM_button按下
	{
		if (button_cfm.press_count <= 5000)
		{
			button_cfm.press_count++;
		}
	}
	else {  // 松开计时
		
		if(button_cfm.loosen_count < 1000) {
			button_cfm.loosen_count++;
		}
	}
    
  if(BT_DN_PRESS)  // DN_button按下
	{
		if (button_dn.press_count <= 5000)
		{
			button_dn.press_count++;
		}
	}
	else {  // 松开计时
		
		if(button_dn.loosen_count < 1000) {
			button_dn.loosen_count++;
		}
	}

	if (BT_FIRE_PRESS)  // FIRE_button按下
	{
		if(button_fire.press_count <= 5000)
		{
			button_fire.press_count++;
		}
	}
	else {  // 松开计时
		
		if(button_fire.loosen_count < 1000) {
			button_fire.loosen_count++;
		}
	}
}

/**
  * @brief  处理dn_button短按_长按
  * @param  None
  * @retval None
  */
static void button_Detect(button_para_t *buttonType)
{
  //可以用指针来实现
  button_para_t *pbutton;
    
  pbutton = buttonType;
    
	if( pbutton->state == button_short_press ) {  // 释放按键
		pbutton->state = button_idle;
	}

	if (pbutton->pressFun())  // 按键按下
	{
		if( (pbutton->press_count>1) && (pbutton->state == button_idle) )
		{
			pbutton->silderFun();	
			pbutton->state = button_press;
			pbutton->loosen_count = 0;			
		}
		else if( (pbutton->press_count>500) && (pbutton->state == button_press) )
		{
			pbutton->state = button_long_press;
			
			if (pbutton->press_count>=800)  keyApp_silderClear();
		}		
	}
	else if (pbutton->loosen_count > 50)
	{  
		if ( pbutton->state == button_press )
		{
			pbutton->state = button_short_press;
		}
		else if ( pbutton->state == button_long_press )
		{
			pbutton->state = button_idle;
		}
		if (pbutton->loosen_count>500)  keyApp_silderClear();
		
		pbutton->press_count = 0;
	}
}


/**
  * @brief  加按键检测 放在轮询中处理
  * @param  None
  * @retval None
  */
void button_Up_Detect(void)
{
#if (BUTTON_NEW_MODE==1)  
  
  button_Detect(&button_up);
  
#else
  
  if( button_up.state == button_short_press ) {  // 释放按键
		button_up.state = button_idle;
	}

	if(BT_UP_PRESS)  // 按键按下
	{
		if( (button_up.press_count>1)&&(button_up.state == button_idle) )
		{
			button_up.state = button_press;
			button_up.loosen_count = 0;  
		}
		else if( (button_up.press_count>500)&&(button_up.state == button_press) )
		{
			button_up.state = button_long_press;
		}
		
		keyApp_silderCheckAdd();
		
	}
	else if (button_up.loosen_count > 50)  // 按键松开
	{  
		if( button_up.state == button_press )
		{
			button_up.state = button_short_press;			 
		}
		else if( button_up.state == button_long_press )
		{
			button_up.state = button_idle;
		}
		
		button_up.press_count = 0;
	}
#endif  
}



/**
  * @brief  减按键 检测 放在轮询中处理
  * @param  None
  * @retval None
  */
void button_Dn_Detect(void)
{     
  button_Detect(&button_dn);
}

/**
  * @brief  减按键 检测 放在轮询中处理
  * @param  None
  * @retval None
  */
void button_Cfm_Detect(void)
{     
  button_Detect(&button_cfm);
}


/**
  * @brief  点火键检测 放在轮询中处理
  * @param  None
  * @retval None
  */
void button_Fire_Detect(void)
{

	/* 释放按键 */
	if( (button_fire.state == button_short_press)
		|| (button_fire.state == button_5_press) 
	  ||(button_fire.state == button_4_press)
	  ||(button_fire.state == button_3_press))
	{  
		button_fire.state = button_idle;
	}

	if (BT_FIRE_PRESS)  // 按键按下
	{
		if( (button_fire.press_count>1)  && (button_fire.state == button_idle) )
		{
			button_fire.state = button_press;			
		}
		
		button_fire.loosen_count = 0; 	
	}
	else 
	{

		 if ( button_fire.state == button_press )
		 {
			  button_fire.state = button_short_press;		
		 }
		
		 if ((button_fire.press_count >= 20) && (button_fire.press_count <= 300))
		 {				
				if ( button_fire.short_count < 5 ) // 连按自加 
				{
					 button_fire.short_count++;
				}
			}			

		 if (button_fire.loosen_count >= 200)
		 {
			 if (button_fire.short_count >= 5)  		// 连按5次进行关机/开机
			 {
				 button_fire.state = button_5_press;
			 }
			 else if(button_fire.short_count == 4)  // 连按4次
			 {
				button_fire.state = button_4_press;
			 }
			 else if	(button_fire.short_count == 3)  // 连按3次进行lock/unlock
			 {
				button_fire.state = button_3_press;
			 }
			
			 button_fire.short_count = 0;
		}
		
		button_fire.press_count = 0;
		
	}
} 

/**
  * @brief  减 按键状态判断
  * @param  None
  * @retval None
  */
uint8_t button_Fire_Press(void)
{
    if (BT_FIRE_PRESS) 
     return 1;
   return 0;
}

/**
  * @brief  减 按键状态判断
  * @param  None
  * @retval None
  */
uint8_t button_Dn_Press(void)
{
    if (BT_DN_PRESS) 
     return 1;
   return 0;
}

/**
  * @brief  CFM 按键状态判断
  * @param  None
  * @retval None
  */
uint8_t button_Cfm_Press(void)
{
    if (BT_CFM_PRESS) 
     return 1;
   return 0;
}

/**
  * @brief  加 按键状态判断
  * @param  None
  * @retval None
  */
uint8_t button_Up_Press(void)
{
   if (BT_UP_PRESS) 
     return 1;
   return 0;
} 


