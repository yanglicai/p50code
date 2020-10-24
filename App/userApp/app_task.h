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
#define  button_Task                0       /**< 按键处理任务1ms */
#define  Status_Task                1       /**< 系统状态任务100ms */
#define  Idle_Task                  2       /**< 空闲处理任务1000ms */
#define  Show_Task                  3       /**< 显示处理任务10ms */ 

#define  MAX_POWER                  500      /**< 输出最大功率 */
#define  MIN_POWER                  50       /**< 输出最小功率 */
#define  DEFAULT_POWER              MAX_POWER      /**< 默认功率 */

#define  SHOW_TIMEOUT               1500    /**< 显示超时 1500ms*/
#define  SHOW_TEST_TIMEOUT          750    /**< 测试状态显示超时 750ms*/

#define  FIRING_TIMEOUT             10000   /**< 点火超时 10000ms*/
#define  AUTO_IDLE_TIMEOUT          30      /**< 自动休眠空闲时间 30s*/
#define  OFF_IDLE_TIMEOUT           5       /**< 关机模式下休眠空闲时间 5s*/

#define  CEASE_FIRING_TIMEOUT       3000    /**< 停火超时 3s*/

#define  CHARGE_FULL_TIMEOUT        1500     /**< 充电时电池电池高于4.08V超过设定时间显示满电 1500 * 0.2S秒 */
#define  CHARGE_MAX_TIMEOUT         4500   //15分钟 * 0.2S 两次计数

#define  MIN_RES                    200     /**< 最小阻值*/
#define  MAX_RES                    3000    /**< 最大阻值*/
#define  OPEN_RES                   MAX_RES + 200
#define  SHORT_RES                  0

#define SYS_OFF                     0
#define SYS_ON                      1

#define SYSTEM_ON                   ( (Sys_Param.flag.system_on_off == SYS_ON) ? (1):(0) )  // 1:开机  0:关机  


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
  uint16_t*   set_power;         /**< 设置功率 */    
  uint16_t*  puff_count;        /**< 吸烟口数 */  
  uint16_t*  load_res;          /**< 负载电阻值*/

} _SYSCHANNEL_;


extern _SYSCHANNEL_  sysChannel; 


typedef  enum
{
  NULL_WAKEUP = 0,
  button_WAKEUP,  // 按键唤醒
  USB_WAKEUP,     // USB唤醒
	ATO_WAKEUP,     //接入ATO唤醒
} wakeup_type_t;



typedef  enum
{
  OUT_VOLTAGE=0,  // 输出电压
  BAT_VOLTAGE,    // 电池电压
  TEMPERATURE,    // 系统温度
} adc_channel_t;


/**
 * @brief  系统工作模式
 */
typedef enum
{
  IDLE_MODE,     /**< 空闲模式 */
  INIT_MODE,     /**< 初始化模式 */
  SYS_RUN_MODE,    /**< 运行模式 */

	MENU_MODE,     /**< 菜单模式 */
	CHARGE_MODE,   /**< 充电模式 */
	
  SLEEP_MODE,    /**< 休眠模式 */
 
} m_mode_t;


/**
 * @brief  系统状态标志
 */
typedef struct {

  uint8_t system_on_off     :1;     /**< 系统开机关机标志 */
  uint8_t smoke_firing      :1;     /**< 点火标志 */
  uint8_t smoke_short       :1;     /**< 输出短路标志 */
  uint8_t system_locked     :1;     /**< 系统锁定标志 */
  uint8_t show_event        :1;     /**< 事件显示标志 */
  uint8_t out_on            :1;     /**< MOS导通标志 */
  uint8_t firing_short      :1;     /**< 点火中短路 */
  uint8_t firing_temp_high  :1;     /**< 点火中高温 */
  uint8_t firing_bat_low    :1;     /**< 点火中电池电量过低 */
  uint8_t firing_open       :1;     /**< 点火中输出开路 */
  uint8_t power_out_enable  :1;     /**< 功率输出标志 */
  uint8_t puff_count_enable :1;     /**< 口数计数使能标志 */
	uint8_t power_is_max      :1;    /**< 功率调节达到最大 */
	
  uint8_t sys_high_temp     :1;     /**< 系统高温标志 */
	
} m_flag_t;




/**
 * @brief  系统事件
 */
typedef enum
{
  Null_Event,             /**< 无事件 */
  Init_Event,             /**< 初始化事件 */
	Wakeup_Event,           /**< 唤醒事件 */
	CheckRes_Event,         /**< 阻值变更事件 */
  VolLow_Event,           /**< 电压低事件 */
  ResLow_Event,           /**< 电阻低事件 */
  ResHigh_Event,          /**< 电阻高事件 */  
  OverTime_Event,         /**< 点火超时事件 */
  OverHeat_Event,         /**< 过热事件 */
  SystemOff_Event,        /**< 开机/关机事件 */
  Lock_Event,             /**< 上锁事件 */
  Unlock_Event,           /**< 解锁事件 */
	PowerIsMax_Event,       /**< 功率最大事件 */ 
	ReadVersion_Event,      /**< 读取版本事件 */  
  Factory_Event,           /**< 恢复出厂默认事件 */ 
	MenuConfig_Event,        /**< 菜单配置事件 */
	ModeConfig_Event,         /**< 模式配置事件 */
	ResAdjust_Event,         /**< ATO校准事件 */
} m_event_t;


typedef struct
{
  m_mode_t	active_mode;          /**< 新工作模式 */
  m_mode_t	prev_mode;         /**< 历史工作模式 */
	
  m_event_t active_event;         /**< 新发生的事件 */
  m_event_t prev_event;        /**< 历史事件 */
  m_flag_t  flag;              /**< 系统标志 */
	
	uint8_t lockDelay;      //吸烟3次，锁按键
	
  adc_channel_t	adc_gather_channel;  /**< 采集ADC通道设置 */
  wakeup_type_t  wakeup_type;  /**< 系统唤醒类型 */ 
    
  uint8_t   out_chNum;  

  uint16_t	set_power;         /**< 设置功率 */
  uint8_t	  puff_time;         /**< 吸烟时间 */
  uint16_t	puff_count;        /**< 吸烟口数 */

  uint16_t	battery_level;     /**< 电池电量等级 */

  uint16_t	load_res;          /**< 负载电阻值 */

  uint16_t  show_timeout;      /**< 显示超时 */
	uint16_t  menu_timeOut;     /**< 显示超时 */
  uint16_t  firing_timeout;    /**< 点火超时 */
  uint16_t  idle_count;        /**< 系统空闲计时 */
	
  uint16_t  out_phase0_ad;     /**< 输出阶段0AD */
  uint16_t  out_phase1_ad;     /**< 输出阶段1AD */
  uint16_t  out_phase2_ad;     /**< 输出阶段2AD */
  uint16_t  out_phase3_ad;     /**< 输出阶段3AD */
	uint16_t  out_phase4_ad;     /**< 输出阶段4AD */
	
  uint16_t  bat_vol_ad;        /**< 电池电压AD值 */
  uint16_t  bat_vol_adNew; 
	uint16_t  bat_vol_adOld; 
	uint16_t  bat_vol_ad_idle; 
	
	uint16_t  batV;

  uint16_t  out_vol_ad;        /**< 输出电压AD值 */
  uint16_t  out_vol_ad_v;        /**< 输出电压AD值 */
	
  uint16_t  out_curr_ad;       /**< 实际电流AD值 */ 
  uint16_t  out_temp_ad;       /**< 系统温度AD值 */
  uint16_t  max_limit_i;       /**< 输出最大限制电量 */
  uint16_t  short_ad;          /**< 短路电流输出AD值 */  
  uint8_t   count_10ms;        /**< 10ms计时(频率10KHz) */
  uint8_t   bat_low_count;     /**< 点火时电池低电量计数 */
  uint16_t  cease_fire_count;  /**< 停止点火时间计数ms */

  uint16_t  charge_full_count; /**< 电池电压超过4.12v计数 unit:100ms */
	uint16_t  chargeTime; //unit：100ms
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

