/**
  ******************************************************************************
  * @file    keyApp.c
  * @author  
  * @version V1.0.0
  * @date    2019.12.16
  * @brief   
   ������� ���ض�Ӧ״̬ ����10ms��ʱ����ѯ��
  */

/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "button.h"	 

#define BUTTON_SILDER_IS_EN  1 // 1 ʹ�ܻ��� 0 ���λ���

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
		sliderTouchCnt = 0x03; //011 �󻬶�
	}  	
}

void keyApp_silderCheckCfm(void)
{
//	if (button_dn.loosen_count < 300) 	
//	{
//		sliderTouchCnt = 0x03; //011 �󻬶�
//	}	
//  else 	if (button_up.loosen_count < 300) 
//	{
//		sliderTouchCnt = 0x06; //110 �һ���
//	}			
}

void keyApp_silderCheckDec(void)
{
	if ((button_cfm.loosen_count < button_up.loosen_count) 	
		&& (button_up.loosen_count < 500) 
//	  && (button_up.state == button_idle)
	  && (button_up.loosen_count - button_cfm.loosen_count > 50 ))	  
	{
		sliderTouchCnt = 0x06; //110 �һ���
	}	
}

/**
  * @brief  �ⲿ���� ���� ���ذ������� ���еȵ�״̬�Ĳ���
  * @param  None
  * @retval None
  */
uint8_t keyApp_fireIsPress(void)  //�������� ���Ƕ̰� ����
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

uint8_t keyApp_fireIsIdle(void)    //��������
{
	 if (button_fire.state == button_idle)
		 return 1;
	 else 
		 return 0;
}	

uint8_t keyApp_addDecIsIdle(void)  //�����̰���
{
	if (button_up.state + button_dn.state == button_idle)
		return 1;
	else
		return 0;
}


uint8_t keyApp_allIsIdle(void)   //���а�������
{
	if (button_up.state + button_dn.state + button_fire.state== button_idle)
		return 1;
	else
		return 0;
}


/**
  * @brief  �������ⲿ�Ľ��������ǵ��� ���ܵĴ�������Ӽ����� ������ʾ����ȣ�
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
			
        /****************** DN������������ ******************/
				if (button_dn.state == button_idle)  // �������д���
				{
					dec_keep_count = 0;
				}
				else if ( button_dn.state == button_press)  // �������´��� 
				{ 					
					return 0x01;
				}
        else if (button_dn.state == button_short_press)  // �����̰����� 
				{					
					/**** ���ʵ������ ****/
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
					/**** ���ʳ������� ****/
					if (button_up.state == button_idle && button_fire.state == button_idle)
					{
             dec_keep_count++;

						if (dec_keep_count>=1) { 
                            
              dec_keep_count = 0;
						
						  return 0x03;
						}
					}
					else if ( button_fire.state == button_press )  //��ϰ�����ʾ
					{   						
							/* ��������3S */	
							if( fire_keep_count < 100)
							{
								fire_keep_count++;
								
								if( fire_keep_count == 100)
								{ 
									if (button_fire.press_count < button_dn.press_count) //�ж��Ⱥ�˳�� ͨ�����µ�ʱ�䳤��
									  return 0x04;                
								}							
							} 
						  							
					}
					else if ( button_up.state == button_long_press && button_dn.state == button_idle) //��ϰ�����ʾ
					{   						
							/* ��������3S */
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
				
				/****************** UP������������ ******************/  
				if (button_up.state == button_idle)  // �������д���
				{
					add_keep_count = 0;			
				}
				else if ( button_up.state == button_press )  // �������´��� 
				{  					
					return 0x11;  
				}
				else if (button_up.state == button_short_press)  // �����̰����� 
				{  					
					/**** ���ʵ������� ****/
					if (button_dn.state == button_idle)
					{						
            return 0x12;
					}
					else
					{
					  return 0xF0;  
					}
				}
				else if (button_up.state == button_long_press)  // ������������ 
				{					
					/**** ������������ ****/
					if ( button_dn.state == button_idle  && button_fire.state == button_idle)
					{
						add_keep_count++;

						if (add_keep_count>=1) {  // ���������ԼӼ��

							add_keep_count = 0;  

							return 0x13;
						}						
					}
					else if ( button_fire.state == button_press) //��ϰ�����ʾ
					{   						
						/* ��������3S*/
						 //�ж��Ⱥ�˳�� ͨ�����µ�ʱ�䳤��
						
							if( fire_keep_count < 100)
							{
								fire_keep_count++;
								
								if( fire_keep_count == 100)
								{ 
									 if (button_fire.press_count < button_up.press_count) //�ж��Ⱥ�˳�� ͨ�����µ�ʱ�䳤��
									   return 0x14;                
								}							
							} 
																		
					}
					else if ( button_dn.state == button_long_press && button_up.state == button_idle ) //��ϰ�����ʾ
					{   						
							/* ��������3S */
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
  * @brief  �������ⲿ�Ľ���������𰴼���Ӧ�Ĺ��ܴ���������� �����ۼ�
  * @param  None
  * @retval None
���� button_Fire_Detect ��һ��ʹ��

�ṩ����״̬��������״̬�£������Ƿ���Ч����ϵͳ���ݵ�ǰ�������ж�
  */
uint8_t keyApp_getFireStatus(void)
{
	 /****************** FIRE������������ ******************/
		
		if ( button_fire.state != button_press )  // �����ɿ�
		{  
				if (button_fire.state == button_3_press)  // ��������3�δ��� 
				{    
						return 0x03;
				} 
        else if(button_fire.state == button_4_press)  // ��������4�δ��� 
				{    
						return 0x04;
				} 			
				else if( button_fire.state == button_5_press )  // ��������5�δ��� 
				{                 
						return 0x05;  					  
				}
				
				fire_keep_count = 0;
			
				return 0x01;  									  
		}
		else if ( button_fire.state == button_press )  // �������´��� 
		{			
				return 0x02;  
		}
		
		return 0x00;
}

//===========================END OF FILES====================================

