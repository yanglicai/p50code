//============================================
// filename: app_task.h
// chip    : 
// author  : Leo Ou	
// date    : 2019-06-25
//============================================
#ifndef  _APP_TASK_H
#define  _APP_TASK_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f0xx_hal.h"
#include "typedef.h"
#include "type.h"

#include "button.h"
#include "adc.h"
#include "oled.h"
#include "myTimer.h"
#include "pwm.h"
#include "smoke.h"

#include "delay.h"
#include "battery.h"
#include "systick.h"


/* Private define ------------------------------------------------------------*/

#define TASK_TIME_START  time_1msOn
#define TASK_TIME_STOP   time_1msOff

#define FIRE_TIME_START  time_100usOn 
#define FIRE_TIME_STOP   time_100usOff

#define  button_Task_Timer          1
#define  Status_Task_Timer          100
#define  Idle_Task_Timer            1000
#define  Show_Task_Timer            10


#define  Task_Num                   4
#define  button_Task                0       /**< ������������1ms */
#define  Status_Task                1       /**< ϵͳ״̬����100ms */
#define  Idle_Task                  2       /**< ���д�������1000ms */
#define  Show_Task                  3       /**< ��ʾ��������10ms */ 

#define  MAX_POWER                  500      /**< �������� */
#define  MIN_POWER                  50       /**< �����С���� */
#define  DEFAULT_POWER              MAX_POWER      /**< Ĭ�Ϲ��� */

#define  SHOW_TIMEOUT               1500    /**< ��ʾ��ʱ 1500ms*/
#define  SHOW_TEST_TIMEOUT          750    /**< ����״̬��ʾ��ʱ 750ms*/

#define  FIRING_TIMEOUT             10000   /**< ���ʱ 10000ms*/
#define  AUTO_IDLE_TIMEOUT          30      /**< �Զ����߿���ʱ�� 30s*/
#define  OFF_IDLE_TIMEOUT           5       /**< �ػ�ģʽ�����߿���ʱ�� 5s*/

#define  CEASE_FIRING_TIMEOUT       3000    /**< ͣ��ʱ 3s*/

#define  CHARGE_FULL_TIMEOUT        1500     /**< ���ʱ��ص�ظ���4.08V�����趨ʱ����ʾ���� 1500 * 0.2S�� */
#define  CHARGE_MAX_TIMEOUT         4500   //15���� * 0.2S ���μ���

#define  MIN_RES                    200     /**< ��С��ֵ*/
#define  MAX_RES                    3000    /**< �����ֵ*/
#define  OPEN_RES                   MAX_RES + 200
#define  SHORT_RES                  0

#define SYS_OFF                     0
#define SYS_ON                      1

#define SYSTEM_ON                   ( (Sys_Param.flag.system_on_off == SYS_ON) ? (1):(0) )  // 1:����  0:�ػ�  


#define STTP_MAX    FIRING_TIMEOUT
#define STEP_TIME0  (STTP_MAX - 100)
#define STEP_TIME1  (STTP_MAX - 200)
#define STEP_TIME2  (STTP_MAX - 1200)
#define STEP_TIME3  (STTP_MAX - 2900)


typedef union
{
  uint8_t bytes;

  struct
  {
		uint8_t b0:1;
		uint8_t b1:1;
		uint8_t b2:1;
		uint8_t b3:1;

		uint8_t b4:1;
		uint8_t b5:1;
		uint8_t b6:1;
		uint8_t b7:1;
  }bits;

}byteBit;

extern byteBit show;

#define SHOW_BYTES show.bytes
#define SHOW_BITS  show.bits 

#define  show_puff_flag           SHOW_BITS.b0
#define  show_time_flag           SHOW_BITS.b1
#define  show_power_flag          SHOW_BITS.b2
#define  show_battery_flag        SHOW_BITS.b3
#define  show_charge_battery_flag SHOW_BITS.b4
#define  show_res_flag            SHOW_BITS.b5
#define  show_batv_flag           SHOW_BITS.b6
#define  show_ativer_flag         SHOW_BITS.b7 


   


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/



typedef struct
{
  uint16_t*   set_power;         /**< ���ù��� */    
  uint16_t*  puff_count;        /**< ���̿��� */  
  uint16_t*  load_res;          /**< ���ص���ֵ*/

} _SYSCHANNEL_;


extern _SYSCHANNEL_  sysChannel; 


typedef  enum
{
  NULL_WAKEUP = 0,
  button_WAKEUP,  // ��������
  USB_WAKEUP,     // USB����
	ATO_WAKEUP,     //����ATO����
} wakeup_type_t;



typedef  enum
{
  OUT_VOLTAGE=0,  // �����ѹ
  BAT_VOLTAGE,    // ��ص�ѹ
  TEMPERATURE,    // ϵͳ�¶�
} adc_channel_t;


/**
 * @brief  ϵͳ����ģʽ
 */
typedef enum
{
  IDLE_MODE,     /**< ����ģʽ */
  INIT_MODE,     /**< ��ʼ��ģʽ */
  SYS_RUN_MODE,    /**< ����ģʽ */

	MENU_MODE,     /**< �˵�ģʽ */
	CHARGE_MODE,   /**< ���ģʽ */
	
  SLEEP_MODE,    /**< ����ģʽ */
 
} m_mode_t;


/**
 * @brief  ϵͳ״̬��־
 */
typedef struct {

  uint8_t system_on_off     :1;     /**< ϵͳ�����ػ���־ */
  uint8_t smoke_firing      :1;     /**< ����־ */
  uint8_t smoke_short       :1;     /**< �����·��־ */
  uint8_t system_locked     :1;     /**< ϵͳ������־ */
  uint8_t show_event        :1;     /**< �¼���ʾ��־ */
  uint8_t out_on            :1;     /**< MOS��ͨ��־ */
  uint8_t firing_short      :1;     /**< ����ж�· */
  uint8_t firing_temp_high  :1;     /**< ����и��� */
  uint8_t firing_bat_low    :1;     /**< ����е�ص������� */
  uint8_t firing_open       :1;     /**< ����������· */
  uint8_t power_out_enable  :1;     /**< ���������־ */
  uint8_t puff_count_enable :1;     /**< ��������ʹ�ܱ�־ */
	uint8_t power_is_max      :1;    /**< ���ʵ��ڴﵽ��� */
	
  uint8_t sys_high_temp     :1;     /**< ϵͳ���±�־ */
	
} m_flag_t;




/**
 * @brief  ϵͳ�¼�
 */
typedef enum
{
  Null_Event,             /**< ���¼� */
  Init_Event,             /**< ��ʼ���¼� */
	Wakeup_Event,           /**< �����¼� */
	CheckRes_Event,         /**< ��ֵ����¼� */
  VolLow_Event,           /**< ��ѹ���¼� */
  ResLow_Event,           /**< ������¼� */
  ResHigh_Event,          /**< ������¼� */  
  OverTime_Event,         /**< ���ʱ�¼� */
  OverHeat_Event,         /**< �����¼� */
  SystemOff_Event,        /**< ����/�ػ��¼� */
  Lock_Event,             /**< �����¼� */
  Unlock_Event,           /**< �����¼� */
	PowerIsMax_Event,       /**< ��������¼� */ 
	ReadVersion_Event,      /**< ��ȡ�汾�¼� */  
  Factory_Event,           /**< �ָ�����Ĭ���¼� */ 
	MenuConfig_Event,        /**< �˵������¼� */
	ModeConfig_Event,         /**< ģʽ�����¼� */
	ResAdjust_Event,         /**< ATOУ׼�¼� */
} m_event_t;


typedef struct
{
  m_mode_t	active_mode;          /**< �¹���ģʽ */
  m_mode_t	prev_mode;         /**< ��ʷ����ģʽ */
	
  m_event_t active_event;         /**< �·������¼� */
  m_event_t prev_event;        /**< ��ʷ�¼� */
  m_flag_t  flag;              /**< ϵͳ��־ */
	
	uint8_t lockDelay;      //����3�Σ�������
	
  adc_channel_t	adc_gather_channel;  /**< �ɼ�ADCͨ������ */
  wakeup_type_t  wakeup_type;  /**< ϵͳ�������� */ 
    
  uint8_t   out_chNum;  

  uint16_t	set_power;         /**< ���ù��� */
  uint8_t	  puff_time;         /**< ����ʱ�� */
  uint16_t	puff_count;        /**< ���̿��� */

  uint16_t	battery_level;     /**< ��ص����ȼ� */

  uint16_t	load_res;          /**< ���ص���ֵ */

  uint16_t  show_timeout;      /**< ��ʾ��ʱ */
	uint16_t  menu_timeOut;     /**< ��ʾ��ʱ */
  uint16_t  firing_timeout;    /**< ���ʱ */
  uint16_t  idle_count;        /**< ϵͳ���м�ʱ */
	
  uint16_t  out_phase0_ad;     /**< ����׶�0AD */
  uint16_t  out_phase1_ad;     /**< ����׶�1AD */
  uint16_t  out_phase2_ad;     /**< ����׶�2AD */
  uint16_t  out_phase3_ad;     /**< ����׶�3AD */
	uint16_t  out_phase4_ad;     /**< ����׶�4AD */
	
  uint16_t  bat_vol_ad;        /**< ��ص�ѹADֵ */
  uint16_t  bat_vol_adNew; 
	uint16_t  bat_vol_adOld; 
	uint16_t  bat_vol_ad_idle; 
	
	uint16_t  batV;

  uint16_t  out_vol_ad;        /**< �����ѹADֵ */
  uint16_t  out_vol_ad_v;        /**< �����ѹADֵ */
	
  uint16_t  out_curr_ad;       /**< ʵ�ʵ���ADֵ */ 
  uint16_t  out_temp_ad;       /**< ϵͳ�¶�ADֵ */
  uint16_t  max_limit_i;       /**< ���������Ƶ��� */
  uint16_t  short_ad;          /**< ��·�������ADֵ */  
  uint8_t   count_10ms;        /**< 10ms��ʱ(Ƶ��10KHz) */
  uint8_t   bat_low_count;     /**< ���ʱ��ص͵������� */
  uint16_t  cease_fire_count;  /**< ֹͣ���ʱ�����ms */

  uint16_t  charge_full_count; /**< ��ص�ѹ����4.12v���� unit:100ms */
	uint16_t  chargeTime; //unit��100ms
	uint16_t  checkOpenCnt;
	uint8_t   charge_changeFlag;

} m_param_t;


extern m_param_t  Sys_Param;


/* Private function prototypes -----------------------------------------------*/
void sys_dataConfig(uint8_t out_chnNum);
void Task_Time_Manage(void);
uint8_t Task_TimeOut(uint8_t task_number);
void Task_Init(void);
void Task_Handle(void);

uint8_t smoke_powerDataAdd(void);
uint8_t smoke_powerDataContinueAdd(void);
void smoke_powerDataDec(void);
void smoke_powerDataContinueDec(void);
void sys_softReset(void);




#endif /* _APP_TASK_H */

