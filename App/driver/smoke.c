/**
  ******************************************************************************
  * @file    smoke.c 
  * @author  Figo
  * @version V1.0.0
  * @date    11-10-2019
  * @brief   
 
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/

#include "main.h" 

#include "app_task.h"
#include <math.h>

#include "pwm.h"
#include "delay.h"
#include "adc.h"  
#include "menu.h"
#include "myMath.h"

//�͵�ͨ
#define  REN_ON         HAL_GPIO_WritePin(R_EN_GPIO_Port , R_EN_Pin , GPIO_PIN_RESET) 
#define  REN_OFF        HAL_GPIO_WritePin(R_EN_GPIO_Port , R_EN_Pin , GPIO_PIN_SET) 

#define  OUTAD_ON       HAL_GPIO_WritePin(OUTAD_EN_GPIO_Port, OUTAD_EN_Pin , GPIO_PIN_SET) 
#define  OUTAD_OFF      HAL_GPIO_WritePin(OUTAD_EN_GPIO_Port, OUTAD_EN_Pin , GPIO_PIN_RESET) 

#define  P9VEN_ON       HAL_GPIO_WritePin(P9V_EN_GPIO_Port, P9V_EN_Pin , GPIO_PIN_SET) 
#define  P9VEN_OFF      HAL_GPIO_WritePin(P9V_EN_GPIO_Port, P9V_EN_Pin , GPIO_PIN_RESET)

#define  BUCK_ON        HAL_GPIO_WritePin(OUT_PWM_GPIO_Port, OUT_PWM_Pin, GPIO_PIN_SET)      
#define  BUCK_OFF       HAL_GPIO_WritePin(OUT_PWM_GPIO_Port, OUT_PWM_Pin, GPIO_PIN_RESET)   


 extern uint16_t sys_calAverage(uint16_t *array, uint8_t num);
 
 static uint16_t smoke_autoMatchAto(uint16_t res);
 
 uint8_t smoke_getResIsOk(uint16_t load_res);

/*
	 ������صĳ�ʼ��
*/ 

void smoke_portInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin  = OUT_PWM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUT_PWM_GPIO_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(OUT_PWM_GPIO_Port, GPIO_InitStruct.Pin, GPIO_PIN_RESET);
	
	GPIO_InitStruct.Pin  = R_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL; //�ⲿ����������
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(R_EN_GPIO_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(R_EN_GPIO_Port, GPIO_InitStruct.Pin, GPIO_PIN_SET);
}


void smoke_init(void)
{
	smoke_portInit();	
  
  P9VEN_ON;	
	BUCK_OFF;	
}

/**
  * @brief  �ر����
  * @param  None
  * @retval None
  */

void smoke_outPutOff(void)
{      
	 BUCK_OFF;	
}

void smoke_outPutOn(void)
{      
	 BUCK_ON;
}

void smoke_on(void)
{

}

void smoke_off(void)
{
		OUTAD_OFF; 	
		BUCK_OFF;
	  REN_OFF;
	  P9VEN_OFF;
}

void smoke_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Pin  = OUTAD_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; //GPIO_MODE_OUTPUT_OD GPIO_MODE_OUTPUT_PP
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(OUTAD_EN_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin  = P9V_EN_Pin;
  HAL_GPIO_Init(P9V_EN_GPIO_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin  = OUT_PWM_Pin;
  HAL_GPIO_Init(OUT_PWM_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief  ��ȡ���ص���
  * @param  None 
  * @retval mO: 0:��·  500~1500:����   3000:��·
*/ 
#define VBAT_SCAL     (float)(ADC_VREF * 2.0f * 1000 / 4096)
#define VATO_SCAl     (float)(ADC_VREF * 2.0f * 1000 / 4096)
	
static void smoke_startCheck(void)
{
    BUCK_OFF;
         
    REN_ON; 
}

static void smoke_stopCheck(void)
{             
    REN_OFF; 	
}

static void smoke_onAdGnd(void)
{             
    OUTAD_ON; 	
}

static void smoke_offAdGnd(void) //ȷ�������ж�Ӧ����������
{             
    OUTAD_OFF;  	
}



volatile uint16_t adBat[5],adOutV[5];

static void smoke_getResAdValue(void)
{
	  smoke_onAdGnd();

    smoke_startCheck();  //�������
	
	  systick_delay_ms(2); 
    
	  uint8_t  i; 
    for (i=0;i < 4;i++)
    {
       systick_delay_ms(1);                 
      
       adBat[i] = adc_getOutBatV_ADC(); 
      
       adOutV[i] = adc_getOutV_ADC();		

    }
    
    smoke_stopCheck();    
}

#define RES_BASE_MR 5000 //MR 

uint16_t smoke_calResValue(uint16_t adcBat,uint16_t adcResV,uint16_t adcResI)
{   	
	  float batVol,loadVol;
  
    batVol = adcBat * BAT_OUTV_SCAL;       //����Q3֮��ĵ�ѹֵ,��λMV��
	
	  loadVol = (adcResV * OUT_V_SCAL);  //��ʵVoutֵ����λmV 
	 
		Sys_Param.out_vol_ad = adcBat;
	
	  Sys_Param.out_vol_ad_v = Sys_Param.out_vol_ad;
				
	  sys_displayFloatDebug(batVol); //debugFloatData
	
	  sys_displayFloatDebug(loadVol);

    uint16_t temp_res;     
     
    float temp1,temp2;	

    if ((batVol > loadVol) && (loadVol < 2000)) //10R 5R ->1.5V
    {
			
			  temp1 = RES_BASE_MR * loadVol;
      
        temp2 = batVol - loadVol;
      
        temp_res = (uint16_t) (temp1/temp2);

			  sys_displayFloatDebug(temp_res);
			
			  if (temp_res > 5) temp_res = temp_res - 5;
        else temp_res = 0;
    }
    else 
        temp_res =  OPEN_RES;	

    return temp_res;    
}

uint16_t resOffset;

void smoke_setoffset(uint16_t offsetData)
{
	resOffset = offsetData;
}

uint16_t smoke_getoffset(void)
{
	return resOffset;
}
  
uint16_t smoke_Calibration(uint16_t resStandads)
{
	  uint16_t temp_res,res_offset;	
	  uint16_t outVAd;
    uint16_t batAd;
	
    smoke_getResAdValue();
	
    batAd = sys_calAverage((uint16_t*)adBat,4);      
    
    outVAd = sys_calAverage((uint16_t*)adOutV,4);  

	  temp_res = smoke_calResValue(batAd,outVAd,1); 
	
	  res_offset = 0xffff;
	
	  if (temp_res > (resStandads - 120) && temp_res < (resStandads + 10))
		{	
			 res_offset = resStandads - temp_res; //��Ϊ����ƽ��ƫ��ֵ
			
			 resOffset =  res_offset;
		}

    return res_offset;   
}

static uint16_t smoke_getAtoRes(uint8_t out_chnNum)
{
    uint16_t temp_res;	
	  uint16_t outVAd;
    uint16_t batAd;
    
    //set ad mode 1	
	
	  (void)out_chnNum;
	
    smoke_getResAdValue();
	
    batAd = sys_calAverage((uint16_t*)adBat,4);      
    
    outVAd = sys_calAverage((uint16_t*)adOutV,4);  

	  temp_res = smoke_calResValue(batAd,outVAd,1); 
	
	  temp_res = temp_res + resOffset; 

    return temp_res;    
}


//ɨ��ato�Ľ��� ��ֵ���� �ȴ�AD�ж��Ƿ��н���ATO������ATO��AD�ή��
//2M�ķ�ѹ �ź�̫�������ղ����жϸߵ͵�ƽ���ɿ� �����
//����ATO֮��,AD1ͨ��ATO�ӵأ�û�н���ATOʱ��AD1��������ͬ�ڵ�ص�ѹ
//�仯ʱ����Ӧ������ ����1 ��ʾ�б仯
static uint16_t load_res,load_res_bak;
static uint8_t checkOkCnt; 
static uint8_t flagCheckRuning = 0;

uint8_t smoke_rungingScanRes(void) //��ɨ��Ŀ��ؿ��� ��ɨ��ֿ�����
{
	return flagCheckRuning;
}

uint8_t smoke_startScanRes(void) //��ɨ��Ŀ��ؿ��� ��ɨ��ֿ�����
{
	checkOkCnt = 0;
	
	load_res_bak = 0;
	
	Sys_Param.load_res = OPEN_RES;
	
	return 1;
}

uint8_t smoke_getResIsOk(uint16_t load_res)
{
	if ((load_res > MIN_RES) && (load_res < MAX_RES)) 
	{
		return 1;
	}
	
	return 0;	
}

#define MAX_CHECK_MAX    5

uint8_t smoke_scanRes(void) //����ɨ�跽ʽ������
{
	  uint16_t adResV = 0;
	  
    uint8_t ret = 0;	
	
	  smoke_offAdGnd();	
	
	  adResV = adc_getOutV_ADC();		
			
		sys_displayDebug(adResV);	
	
	  if (menu_modeShiftToAuto()) //ֻ�ڱ仯ʱ��ִ��1�Σ���Ҫ�ڵ����Ѿ����������������
		{
			  if (smoke_getResIsOk(load_res)) //˵���Ѿ����Թ� ��ֵ�Ƿ��Ϸ�Χ��
				{						
					*sysChannel.set_power	= smoke_autoMatchAto(Sys_Param.load_res);		
					
          ret = 1;					
				}
		}
	  
	  if (Sys_Param.load_res==OPEN_RES || Sys_Param.load_res==SHORT_RES) 
		{								
			if (adResV < 500) //�ж�Ӧ��ATO���뵼��AD��С
			{				
				flagCheckRuning = 1;
				
				bat_chargePause();  //�رճ��

				load_res = smoke_getAtoRes(0); 

				if (myCal_IntegerDelt(load_res,load_res_bak) < 30) //��������С��30moh
				{
					  if (smoke_getResIsOk(load_res)) //�ڷ�Χ���ٴ���
						{
							checkOkCnt++;
							
							if (checkOkCnt >= MAX_CHECK_MAX)
							{						 
								  smoke_outPutOn();
								
								  systick_delay_ms(20);

                  smoke_outPutOff();								
								
								  load_res = smoke_getAtoRes(0); 
								
								  load_res = smoke_getAtoRes(0); 
								
									Sys_Param.load_res = load_res;
								
								  checkOkCnt = 0;
									
									if (menu_getMode()==1)
									{
										*sysChannel.set_power	= smoke_autoMatchAto(Sys_Param.load_res);		
									}
									flagCheckRuning = 0;
									
									ret = 1;							
								}
							}
						  else checkOkCnt = 0;
				}
				else
				{
					checkOkCnt = 0;
				}
				
				load_res_bak = load_res;
				
				bat_chargeRecover();   //�������
				
				smoke_offAdGnd();
				       			
			}
			else if (adResV > 1000)
			{
				if (load_res_bak!=OPEN_RES)
				{
				  load_res_bak = OPEN_RES;
					ret = 1;	
					flagCheckRuning = 0;
				}
				checkOkCnt = 0;
			}
		}
		else if (adResV > 1000)
		{					
			  if (Sys_Param.load_res != OPEN_RES)
				{
			     Sys_Param.load_res = OPEN_RES;
					 flagCheckRuning = 0;
           ret = 1;					
				}			
			  checkOkCnt = 0;    		
		}	
    
    return ret;
}

//�ж����� ������ǰ����
void smoke_atoIsrConfig(uint8_t sysOnFlag)
{    
  GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 

	GPIO_InitStruct.Pull = GPIO_NOPULL; //  �ⲿ��2M����������
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;	

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);//PB0 OUTAD3
	
	systick_delay_ms(2);

	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET) //�͵�ƽ˵����ATO�ڽ��룬ȷ����ʱAD �����Ѿ��ر�
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	}
	else 
	{
		 GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	}
	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);
		
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}


//�������ռ�ձ�
/**
  * @brief   �������ռ�ձ� 
  * @param  setVolAd:�趨�����ѹ��Ӧ��ADֵ
  * @retval �趨ֵ��Ӧ��ռ�ձ� ������
��Чֵ 
  */
static uint16_t smoke_calOutDuty(uint16_t setVolAd,uint8_t out_chnNum)
{
	(void)out_chnNum;
	
	float temp_buck_duty;
	uint16_t buck_setDuty; 	
	
	temp_buck_duty = (setVolAd * 1.0f) / (Sys_Param.out_vol_ad);

	buck_setDuty =  (uint16_t)(temp_buck_duty * temp_buck_duty * 100);
	
	if (buck_setDuty >= 100)
	{
		buck_setDuty = 100;
	}

	return buck_setDuty;
}

static void buck_calOutDuty(uint8_t out_chnNum)
{
  (void)out_chnNum;
  
	/******** ����׶�0���ռ�ձ� *******/
	
	pwm_data.back_set_duty0 = smoke_calOutDuty(Sys_Param.out_phase0_ad,out_chnNum);            
	
	/******** ����׶�1���ռ�ձ� *******/
	pwm_data.back_set_duty1 = smoke_calOutDuty(Sys_Param.out_phase1_ad,out_chnNum);
	
	/******** ����׶�2���ռ�ձ� *******/
	pwm_data.back_set_duty2 = smoke_calOutDuty(Sys_Param.out_phase2_ad,out_chnNum);
	
	/******** ����׶�3���ռ�ձ� *******/
	pwm_data.back_set_duty3 = smoke_calOutDuty(Sys_Param.out_phase3_ad,out_chnNum);
	
	/******** ����׶�4���ռ�ձ� *******/
	pwm_data.back_set_duty4 = smoke_calOutDuty(Sys_Param.out_phase4_ad,out_chnNum);
}



/*
  ���̲��������� �ʺ϶�ͨ���Ĵ���
*/

void smoke_calParameter(uint8_t out_chnNum)
{
	  uint16_t level; //��ʱȡ�� ��Ӱ��ϵͳ��ʾ
	
    level = bat_getLevel(Sys_Param.batV);  
	
	  bat_chargePause();   //�رճ��
     
    Sys_Param.load_res = smoke_getAtoRes(out_chnNum);                            // ��ȡ���ص���  


    /*********** ������ ***********/
    if( Sys_Param.flag.sys_high_temp == 1) // ���ϵͳ����
    {
        Sys_Param.active_event = OverHeat_Event;  
    }
    else if( level <= 0 ) // ����ص���
    {
        Sys_Param.active_event = VolLow_Event;
    }
    else if (Sys_Param.load_res > MAX_RES)   // ��⸺�ص������
    {
        Sys_Param.active_event = ResHigh_Event;
    }
    else if(Sys_Param.load_res < MIN_RES)   // ��⸺�ص����С(��·)
    {
        Sys_Param.active_event = ResLow_Event;
    }
    else  // ϵͳ����������������� 
    {
		    float temp_res, temp_power, set_voltage;
			
	      uint16_t temp_vol_ad;
    
        temp_res = Sys_Param.load_res;
        temp_res = temp_res /1000.0f;
        temp_power = Sys_Param.set_power / 10.0;
        
        set_voltage = sqrt(temp_res * temp_power);     // �����趨�������ѹ(v)
        temp_vol_ad = (uint16_t)(set_voltage * OUT_VOL_SCAL);  // ����100%���ʱ�������������ADֵ
        //��������Чֵ
        
        /***** SMART ������ʼ��� *****/
        /* ���Ͽڵ�𳬹�3�� */
        if ( Sys_Param.cease_fire_count >= CEASE_FIRING_TIMEOUT )
        {    
					  Sys_Param.out_phase0_ad = temp_vol_ad * 0.9486f;  //90%
					  Sys_Param.out_phase1_ad = temp_vol_ad;            //100%
            Sys_Param.out_phase2_ad = temp_vol_ad * 1.0488f;  //110%(1.1��ƽ����)
            Sys_Param.out_phase3_ad = temp_vol_ad;            //100%
					  Sys_Param.out_phase4_ad = temp_vol_ad * 0.9220f;  //85%
					  
        }
        else /* ���Ͽڵ��С��3�� */
        {
            Sys_Param.out_phase0_ad = temp_vol_ad * 0.9486f;  //90%
					  Sys_Param.out_phase1_ad = temp_vol_ad;            //100%
            Sys_Param.out_phase2_ad = temp_vol_ad * 1.0488f;  //110%(1.1��ƽ����)
            Sys_Param.out_phase3_ad = temp_vol_ad;            //100%
					  Sys_Param.out_phase4_ad = temp_vol_ad * 0.9220f;  //85%
        }
        
        if (Sys_Param.out_phase0_ad > OUT_MAX_AD) //4.2V
        {
            Sys_Param.out_phase0_ad = OUT_MAX_AD;
        }
        if (Sys_Param.out_phase1_ad > OUT_MAX_AD)
        {
            Sys_Param.out_phase1_ad = OUT_MAX_AD;
        }
        if (Sys_Param.out_phase2_ad > OUT_MAX_AD)
        {
            Sys_Param.out_phase2_ad = OUT_MAX_AD;
        }
        if (Sys_Param.out_phase3_ad > OUT_MAX_AD)
        {
            Sys_Param.out_phase3_ad = OUT_MAX_AD;
        }
				if (Sys_Param.out_phase4_ad > OUT_MAX_AD)
        {
            Sys_Param.out_phase4_ad = OUT_MAX_AD;
        }
          

      /* BUCK����������� */                     

      buck_calOutDuty(out_chnNum);

      pwm_data.back_pwm_duty = pwm_data.back_set_duty0;  // ����BUCK ���ռ�ձ�

      Sys_Param.short_ad = SHORT_CURR;  // �趨��·����  

      out_mode = BUCK_MODE;  // BUCKģʽ���    
		
      smoke_on();				
   }
}





//========================================
static uint8_t flagChangeDuty;

void smoke_outManange(uint8_t chnNum) //ִ�в��� ok 0303 
{
    if( (Sys_Param.count_10ms == pwm_data.back_pwm_duty && pwm_data.back_pwm_duty < 100) 
			|| (pwm_data.back_pwm_duty == 0) )
    {      
      BUCK_OFF;
                
      Sys_Param.flag.out_on = 0; // MOS�ص�
      
      flagChangeDuty = 0;   
    }
    else if( Sys_Param.count_10ms >= 100 )
    {    
       BUCK_ON;
			
       Sys_Param.flag.out_on = 1;  // MOSͨ��	

       Sys_Param.checkOpenCnt = 0; 			
    }
    else if( (Sys_Param.count_10ms < pwm_data.back_pwm_duty) 
			&& (Sys_Param.count_10ms > 5)
		  && (Sys_Param.count_10ms % 5 ==0))  //500us֮��ʼ����  
    {
      flagChangeDuty = 1;
    }
}

/**
  * @brief  ������ʵ���
  * @param  None
  * @retval None
  */
void smoke_outPowerRegulation(uint8_t out_chnNum)
{

  (void)out_chnNum;

  if( Sys_Param.firing_timeout > STEP_TIME0 )
  {
    pwm_data.back_pwm_duty = pwm_data.back_set_duty0; // 0-500ms
  }
  else if( Sys_Param.firing_timeout > STEP_TIME1 )
  {
    pwm_data.back_pwm_duty = pwm_data.back_set_duty1; 
  }
  else if( Sys_Param.firing_timeout > STEP_TIME2 )
  {
    pwm_data.back_pwm_duty = pwm_data.back_set_duty2; 
  }
  else if( Sys_Param.firing_timeout > STEP_TIME3 )
  {
    pwm_data.back_pwm_duty = pwm_data.back_set_duty3; 
  }
	else 
  {
    pwm_data.back_pwm_duty = pwm_data.back_set_duty4; 
  }
	
}


void smoke_adjutOut(uint8_t out_chnNum)
{
	(void)out_chnNum;
  float adjutDuty;
	uint16_t newDuty;
	
    if( Sys_Param.firing_timeout > STEP_TIME0 )
    {
			adjutDuty = Sys_Param.out_phase0_ad * 1.0f / Sys_Param.out_vol_ad;
			newDuty = (uint16_t)(adjutDuty * adjutDuty * 100);	
			
			if (newDuty > pwm_data.back_set_duty0)
			{
				pwm_data.back_set_duty0 = newDuty;	
			}			
    }
    else if( Sys_Param.firing_timeout > STEP_TIME1 )
    {
      // 500-3000ms
      adjutDuty = Sys_Param.out_phase1_ad * 1.0f/ Sys_Param.out_vol_ad;
      newDuty = (uint16_t)(adjutDuty * adjutDuty * 100);	
			
			if (newDuty > pwm_data.back_set_duty1)
			{
				pwm_data.back_set_duty1 = newDuty;	
			}  	
    }
    else if( Sys_Param.firing_timeout> STEP_TIME2 )
    {
   
      adjutDuty = Sys_Param.out_phase2_ad * 1.0f/ Sys_Param.out_vol_ad;
      newDuty = (uint16_t)(adjutDuty * adjutDuty * 100);	
			
			if (newDuty > pwm_data.back_set_duty2)
			{
				pwm_data.back_set_duty2 = newDuty;	
			}  	
    }
    else if( Sys_Param.firing_timeout> STEP_TIME3 )
    {
      
      adjutDuty = Sys_Param.out_phase3_ad * 1.0f/ Sys_Param.out_vol_ad;
      newDuty = (uint16_t)(adjutDuty * adjutDuty * 100);	
			
			if (newDuty > pwm_data.back_set_duty3)
			{
				pwm_data.back_set_duty3 = newDuty;	
			}  	
    }
		else 
    {
      adjutDuty = Sys_Param.out_phase4_ad * 1.0f/ Sys_Param.out_vol_ad;
      newDuty = (uint16_t)(adjutDuty * adjutDuty * 100);	
			
			if (newDuty > pwm_data.back_set_duty4)
			{
				pwm_data.back_set_duty4 = newDuty;	
			}  	
    }
}

//��� �жϿ��ƴ���
void TIM100us_IRQHandler(void)  //100us
{   
  
	 if( (Sys_Param.flag.smoke_firing == 0) || (Sys_Param.flag.power_out_enable == 0) ) return;
	
//	   uint16_t batVad; //��ʱȡ�� ȷ�����޸�ϵͳ����
	 
		/* BUCK���ģʽ���� (ģ��PWM���) */
	    
      smoke_outManange(0);
			
	    Sys_Param.count_10ms++;
	
      if (Sys_Param.count_10ms > 100)
      {     
        Sys_Param.count_10ms = 0;
  
        Sys_Param.adc_gather_channel = TEMPERATURE;  
      }        

		/* ����������-�����ѹ-��ص�ѹ-ϵͳ�¶� */
		if( Sys_Param.flag.out_on == 1 ) 
		{	   
			Sys_Param.out_curr_ad = adc_getOutI_ADC(); // ��ȡ�������
			
			if( Sys_Param.out_curr_ad > Sys_Param.short_ad )  
			{
					smoke_outPutOff();  // �ر����

					Sys_Param.flag.firing_short = 1;  // �����·
					Sys_Param.flag.power_out_enable = 0;

					return;
			}
			
			switch( Sys_Param.adc_gather_channel )
			{
				/* ��������ѹ */
				case OUT_VOLTAGE:

					Sys_Param.out_vol_ad = adc_getOutV_ADC();  // ��ȡ�����ѹ
                 
          if (flagChangeDuty)
          {          
              flagChangeDuty = 0;   
              
              smoke_adjutOut(0);
						
						  if (Sys_Param.out_vol_ad > Sys_Param.out_vol_ad_v)
							{
								Sys_Param.checkOpenCnt++; 
						
								if (Sys_Param.checkOpenCnt > 2)
								{
									smoke_outPutOff();  // �ر����

									Sys_Param.flag.firing_open = 1;  // �����·
									Sys_Param.flag.power_out_enable = 0;
								}
							}
							else
							{
								Sys_Param.checkOpenCnt = 0;
							}
          }

					Sys_Param.adc_gather_channel = BAT_VOLTAGE;

				break;
				
				/* ����ص��� */
				case BAT_VOLTAGE: //��ʱȡ�� ���޸�ϵͳ����

//					batVad = adc_getBatV_ADC(); // ��ȡ��ص�ѹ �����صȼ�
					
					Sys_Param.adc_gather_channel = TEMPERATURE;//�����ж϶�·�ᵼ��ֱ����Դ��������

				break;

				/* ���ϵͳ�¶� */
				case TEMPERATURE:

					Sys_Param.out_temp_ad = adc_getOutTemp_ADC();  // ��ȡϵͳ�¶�
				
					if (Sys_Param.out_temp_ad < TEST50_TEMP) // ϵͳ�¶ȹ���
					{
						if (flagSysIsDefault==1)
						{
							if (Sys_Param.out_temp_ad < MAX_TEMP)
							{
								smoke_outPutOff();
								Sys_Param.flag.firing_temp_high = 1; 
								Sys_Param.flag.power_out_enable = 0;
							}
						}
						else 
						{
							smoke_outPutOff();
							Sys_Param.flag.firing_temp_high = 1; 
							Sys_Param.flag.power_out_enable = 0;
						}
					}
				
					Sys_Param.adc_gather_channel = OUT_VOLTAGE;
				
				break;
				
				default:
					Sys_Param.adc_gather_channel = TEMPERATURE;
				break;
				
			}
		}   	
}


/**
  * @brief  ���ʵ��ν���
  * @param  None
  * @retval None
  */

void smoke_powerDataDec(void) //0x02
{
	if (*(sysChannel.set_power) > MIN_POWER) 
	{
			(*(sysChannel.set_power))--;      
	}                         
}

/**
  * @brief  ���ʳ�������
  * @param  None
  * @retval None
  */

void smoke_powerDataContinueDec(void)//0x03
{
  if(*(sysChannel.set_power) > MIN_POWER) 
  {
      (*(sysChannel.set_power))--;      
  }
}



/**
  * @brief  ���ʵ�������
  * @param  None
  * @retval None
  */

uint8_t smoke_powerDataAdd(void) //0x12
{
	if (menu_getMode()==1)
	{
		if (Sys_Param.load_res <= 380) //0.3
		{
			if ((*sysChannel.set_power) < 350)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 350) 
					{
						return 1;
					}
			}
      else return 1;		  
		}
		else if (Sys_Param.load_res <= 520) //0.45
		{
			if ((*sysChannel.set_power) < 250)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 250) 
					{
						return 1;
					}
			}
      else return 1;			  
		}
		else if (Sys_Param.load_res <= 700)//0.6
		{
			if ((*sysChannel.set_power) < 250)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 250) 
					{
						return 1;
					}
			} 
			else return 1;
 
		}
		else if (Sys_Param.load_res <= 900)//0.8
		{
			if ((*sysChannel.set_power) < 180)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 180) 
					{
						return 1;
					}
			} 
			else return 1;
		}
		else if (Sys_Param.load_res <= 1100)//1.0
		{
			if ((*sysChannel.set_power) < 150)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 150) 
					{
						return 1;
					}
			} 
			else return 1;
		}
		else if (Sys_Param.load_res <= 3000)//1.2
		{
			if ((*sysChannel.set_power) < 120)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 120) 
					{
						return 1;
					}
			} 
			else return 1;
 
		}
		else
		{
			if ((*sysChannel.set_power) < 400)		
			{
					(*sysChannel.set_power)++;
				
					if((*sysChannel.set_power) == 400) 
					{
						return 1;
					}
			} 
			else return 1;
 
		}
	}
  else 
	{
		if ((*sysChannel.set_power) < MAX_POWER)		
		{
				(*sysChannel.set_power)++;
				if((*sysChannel.set_power) == MAX_POWER) 
				{
					return 1;
				}
		}
    else return 1;		
	}
  return 0;	
}

/**
  * @brief  ������������
  * @param  None
  * @retval None
  */

uint8_t smoke_powerDataContinueAdd(void) //0x13
{   
	   return smoke_powerDataAdd();
}



#define OUT_V_4D2SQUARE    176.4f     //�Ŵ���10��
#define OUT_V_3D6SQUARE    129.6f     //�Ŵ���10��
#define OUT_V_3D2SQUARE    102.4f     //�Ŵ���10��
#define OUT_V_3D0SQUARE    90.00f 
#define OUT_V_5D0SQUARE    250.0f
#define OUT_V_2D4SQUARE    57.60f
#define OUT_V_1D6SQUARE    25.60f

static uint16_t smoke_autoMatchAto(uint16_t res) //������ֵ�Ŵ�1000����
{

	float currentRes = 0;
	uint16_t remindPower;
	
	currentRes = res / 1000.0f;
	
	if (currentRes <= 0.200)
	{
		remindPower = (uint16_t)(OUT_V_3D2SQUARE / currentRes);		
	}
	else if (currentRes <= 0.380) //0.3 
	{
		remindPower = 300;
	}
	else if (currentRes <= 0.520) //0.45 
	{
		remindPower = 250;
	}
	else if (currentRes <= 0.700) //0.6      
	{
		remindPower = 230;		
	}
	else if (currentRes <= 0.900)      //0.8         
	{
		remindPower = 160;		
	}	
	else if (currentRes <= 1.100)      //1.0     
	{
		remindPower = 130;		
	}
  else if (currentRes <= 3.0)      //1.2   
	{
		remindPower = 100;		
	}
	else 
	{
		remindPower = 50;		
	}
	
	remindPower = remindPower / 10;
	
	remindPower = remindPower * 10;

	//�����ж� ���������ֵ ��Сֵ
	if (remindPower > MAX_POWER)
	{
		remindPower = MAX_POWER;
	}

	if (remindPower < MIN_POWER)
	{
		remindPower = MIN_POWER;
	}
	
	return remindPower;
}

