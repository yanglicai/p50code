
#ifndef    __FASH_CHARGED_H__
#define    __FASH_CHARGED_H__ 


#include "main.h" 
#include "typedef.h"

typedef unsigned char	byte;
typedef union
{
    byte  Byte;
    struct
    {
        byte  Bit0: 1;
        byte  Bit1: 1;
        byte  Bit2: 1;
        byte  Bit3: 1;
        byte  Bit4: 1;
        byte  Bit5: 1;
        byte  Bit6: 1;
        byte  Bit7: 1;
    }Bits;
    struct
    {
        byte  BIT  :8;
    }MergedBits;
}BxSTR;


#define  TYPE_READ_FACT   0x01
#define  TYPE_WRITE_FACT   0x10

#ifndef CURRENT_RATIO

#define   CURRENT_RATIO           0.0161132f   // 3.3 * 1000 / 4096 / 1 / 50  
#define   CURRENT_DATA           (uint16_t)(1/CURRENT_RATIO + 1)

#define   BATP_RATIO              0.0032226f  //比例 * 3.3 / 4096
#define   BATP_DATA               (uint16_t)(1/BATP_RATIO + 1)  

#define   OUT_V_RADIO           0.002148f   //0.002148f  --30K 18K f  

#define   OUT_V_DATA             (float)(1/OUT_V_RADIO + 2)

 
//#define  MAX(a, b)      (((a) >= (b)) ? (a) : (b))
//#define  MIN(a, b)      (((a) < (b)) ? (a) : (b))


#endif

#define   ADC_CHG_BATC         0
#define   ADC_CHG_BATP         1


#define   ADC_CHG_V            2
#define   ADC_CHG_I            3

#define   ADC_CHG_BAT_NTC1     4
#define   ADC_CHG_BAT_NTC2     5  


#define    CHARGE_MAX_TIME    5400 //90分钟
#define    CHARGE_FULL_DELAY  180 //3分钟
#define    CHARGE_FULL_TIME   2100 //35分钟
#define    CAHRGE_FULL_MIN    1680 //28分钟 
#define    CHARGE_FULL_MAX    1920 //32分钟
#define    ONE_BAT_MAX        4.25f

#define    ADC_MAX_NUM        20    

#define    TX_SHORT_INTERVAL_TIME 50
#define    TX_LONG_INTERVAL_TIME  300
#define    RX_OVERTIEM            100 
#define    HEATBEAT_INTERVAL_TIME 500

#define    CHARGE_TRICKLE_BATV    6.20f
#define    CHARGE_SHIFTIC_BATV    8.27f //确保压差在0.90f也能可靠处理
#define    CHARGE_IC_BATV         8.20f

#define    ZERO_TEMPERATURE         223  //0度 
#define    ZERO_TEMPERATURE1        289  //5度 

#define    BATTERY_TEMPERATURE      994  //37度 
#define    BATTERY_TEMPERATURE1     1120  //40度 
#define    BATTERY_TEMPERATURE2     1152  //42度 
#define    BATTERY_TEMPERATURE3     1218  //44度
#define    BATTERY_TEMPERATURE4     1285  //46度

#define    CHANGE_CURRENT_02A    86
#define    CHANGE_CURRENT_04A    173
#define    CHANGE_CURRENT_06A    260
#define    CHANGE_CURRENT_1A     434
#define    CHANGE_CURRENT_1A5    651
#define    CHANGE_CURRENT_2A     868
#define    CHANGE_CURRENT_2A5    1085
#define    CHANGE_CURRENT_2A8    1215
#define    CHANGE_CURRENT_3A     1303
#define    CHANGE_CURRENT_3A5    1520
#define    CHANGE_CURRENT_3A8    1650
#define    CHANGE_CURRENT_4A     1737 
#define    CHANGE_CURRENT_4A5    1954 
#define    CHANGE_CURRENT_4A8    2085 
#define    CHANGE_CURRENT_5A     2172

#define    USB_ZERO_CURRENT      260 

#define MAX_CHARGE_I       5.5f
#define MAX_CHARGE_V       10.0f

#define MAX_CHARGE_I_MSG  (uint16_t)(MAX_CHARGE_I / 0.05f) 
#define MAX_CHARGE_V_MSG  (uint16_t)(MAX_CHARGE_V / 0.02f)

//BAT 测温
//#define    BAT_NTC_EN_OFF()         (GPIOB->ODR &= (uint16_t)(~GPIO_Pin_2))      
//#define    BAT_NTC_EN_ON()          (GPIOB->ODR |= (uint16_t)GPIO_Pin_2)

//快充回路MOS驱动    PD_EN PC3

//#define    PD_IN_OFF()              (GPIOC->ODR &= (uint16_t)(~GPIO_Pin_3))      
//#define    PD_IN_ON()               (GPIOC->ODR |= (uint16_t)GPIO_Pin_3)  


//集成管理芯片充电驱动 U4_EN PC5
#define    U4_OFF()                 (GPIOC->ODR &= (uint16_t)(~GPIO_Pin_5))      
#define    U4_ON()                  (GPIOC->ODR |= (uint16_t)GPIO_Pin_5)  

extern volatile BxSTR        _Flag20;

#define   Flag20             _Flag20.Byte

#define   B_ChargeOk         _Flag20.Bits.Bit0
#define   USB_BAT_CHARGEOVER _Flag20.Bits.Bit1//2019.08.05 单节电池过压 不充电标志
#define   Bat_ChargeIsOn     _Flag20.Bits.Bit3
#define   USB_QC_IS_ACTIVE   _Flag20.Bits.Bit4   

#define   USB_INSERT_FLAG    _Flag20.Bits.Bit6 //usb插入标志 和 其他USB快充标志完全区分开，不能用同一个变量字节

 

extern volatile BxSTR        _Flag21;

#define   Flag21             _Flag21.Byte

#define   USB_CHARGE_ERROR   _Flag21.Bits.Bit0
#define   USB_HANDSHAKE_OK   _Flag21.Bits.Bit1 //握手成功标志
#define   USB_BAT_OVERHEAT   _Flag21.Bits.Bit2
#define   USB_SEND_OK        _Flag21.Bits.Bit3 //发送成功标志

#define   USB_NEES_ACK       _Flag21.Bits.Bit4
#define   USB_BAT_LOWTEMP    _Flag20.Bits.Bit5
#define   USB_NEW_VALUE      _Flag21.Bits.Bit6
#define   FAST_CHARGE_ON     _Flag21.Bits.Bit7

typedef struct
{
        uint8_t     BATCount;
	//bat	
	
	uint16_t     Bat_Count;
	uint8_t      Change_Count; //充电变化计数
	uint8_t      Change_CountBak;
	uint8_t      singleChange_Count[2]; //单个电池的变化计数
	
	uint16_t    Bat_Val2;
	uint16_t    Bat_Val1;
	
	float       Bat_Voltage;
	float       Bat1_Voltage;
	float       Bat2_Voltage;

	float       Bat_VoltageF;
	float       Bat1_VoltageF;
	float       Bat2_VoltageF;

	float       Bat_Average_Voltage;
	
	uint8_t     BLevel_Val;
	uint8_t     BLevel_ValBak;

	uint8_t     singleBatLevel_Val[2];
	uint8_t     singlePercentage_Val[2];

	uint8_t     BeforeSmoke_Precent;
	uint8_t     Percentage_Val;
	uint8_t     Percentage_ValBak;

	//usb
	float     USB_V;
	float     USB_I;
	float     USB_R;
	float     USB_Vbak;

	uint16_t  BatT1;
	uint16_t  BatT2;

	uint16_t  Bat1_Tadc_Val;
	uint16_t  Bat2_Tadc_Val;

	uint16_t  USB_Vin_Val;
	uint16_t  Batter_Iadc_Val;   //保持和072程序命名一致
	uint16_t  Change_average_current;
	uint16_t  Charge_average_v;

}_CHARGE_DATA;

extern _CHARGE_DATA chargeData;



typedef struct
{
	uint8_t step; //0 涓流 1 恒流 2 恒压 3 充满后的涓流

	uint8_t onNum;
	uint8_t offNum;

	uint16_t chargeMaxITime;
	uint16_t charge4ATime;
	uint16_t charge3ATime;
	uint16_t charge2ATime;

	uint8_t  upCnt;

	uint8_t errCnt;
	uint8_t currentIsZeroCnt;
	
	uint8_t checkCnt;
	uint8_t activeCnt;
	uint8_t overCnt;
	uint8_t normalCnt;
	uint8_t flag;

	uint16_t lineR;

	uint16_t pluseCnt;
	uint16_t measureFactV;
	uint8_t  measureCnt;

	uint8_t  normalHeatCnt;
	uint8_t  temp42Cnt;
	uint8_t  temp44Cnt;
	uint8_t  temp46Cnt;

	uint8_t  flagTempOver42t;
	uint8_t  flagTempOver44t;
	uint8_t  flagTempOver46t;

	uint16_t  chargeFullDelay;

	uint8_t displayDelay;
	uint16_t second;
	uint8_t  NcSecond;
	uint8_t chargeDelay;
	
}CHARGE_CTRL;

extern CHARGE_CTRL chargeCtrl;


typedef struct
{
	volatile uint16_t txDelay;
	volatile uint8_t  txEn;
	volatile uint16_t rxDelay;
	volatile uint8_t rxIsOver;
	volatile uint16_t heatBeatDelay;
	uint16_t heatBeatCnt;
	volatile uint8_t  txHeatBeatEn;
	uint8_t  index;

	uint16_t txMaxDelay;

	uint8_t answerFlag;
	uint8_t repeatCnt;
	uint16_t insertCnt;
	uint16_t uartRevCnt;
	uint16_t uartTxCnt;
	uint8_t  longIntervalFlag;


}UART_CTRL;

extern UART_CTRL uartCtrl;

typedef enum
{
	CHARGE_FAST,
	CHARGE_COMMON,
}USER_SELECT;

extern USER_SELECT userSelcet;

typedef enum
{
	CHARGE_IDLE,
	CHARGE_IC,
	CHARGE_DIRECT,
}CHARGE_SHIFT;

extern CHARGE_SHIFT    chargeShift, chargeShiftBak;

typedef enum
{
	UART_IDLE = 0,
	BUILD_SHAKE_HANDS = 1,
	SHAKE_HANDS_FAILURE = 2,
	SHAKE_HANDS_SUCCESS = 3,
	WAIT_DEV_REPLY = 4,
	SHAKE_SEND_REPLY = 5,
	SHAKE_HANDS_OVER = 6,
}UART_MODEL;

typedef enum
{
	PASS_THROUGH = 0,
	DELAY_WAITING = 1,
}UART_RECEIVE_WAIT;

typedef enum
{
	UART_SEND = 0,
	UART_RECEIVE = 1,
	UART_OVER_TIME = 2,
	RECEIVE_END = 3,
}UART_ENUMERATION;


typedef enum
{
	TX_IDLE,	       // 空闲状态
	TX_SENDDATA,
	TX_END,	       // 发送结束
	TX_START,     // 	启动发送
	TX_DELAY,  	  //  发送延时等待     	
	TX_REPEAT,    //  重发

	WAIT_ANSWER,   // 等待回复
	RX_ANSWER_OK,    //回复ok
	RX_ANSWER_FAIL,//回复失败信息
	WAIT_OVERTIME, // 等待超时

}FAST_CHARGE_COM_STA; //通信状态





typedef enum
{
	OUT1A = 0,
	OUT1A5,
	OUT2A,
	OUT2A5,
	OUT3A,
	OUT3A5,
	OUT4A,
	OUT4A5,
	OUT5A,
}FAST_CHANGE_CURRENT;

typedef enum
{
	CHANGE_IDLE = 0,
	FAST_MODEL,
	NORMAL_MODEL,
}FAST_CHANGE_MODEL;


typedef enum
{
	BATTERY_NORMAL = 0,
	BATTERY_PROTECTION = 1,
}BATTERY_MODEL;

typedef enum
{
	IDLE_TYPE,
	FAST_TYPE,
	NORMAL_TYPE,
	CHARGING_END,
}ADAPTER_TYPE;


typedef enum
{
	MAX_OUTPUT_CURRENT = 0x01,         //输出最大电流
	MAX_OUTPUT_VOLTAGE = 0x02,         //输出最大电压
	OUTPUT_VOLTAGE = 0x03,             //输出电压	
	OUTPUT_CURRENT = 0x04,             //输出电流	
	ADAPTER_REPLY_OK = 0x0F,            //应答
	ADAPTER_NO_ANSWER = 0x7F,          //没应答	
	//=============================================//	
	SHAKE_HANDS_START = 0x80,         //握手开始
	SET_ADAPTER_VOLTAGE = 0x81,       //设置适配器电压。
	CLOSE_ADAPTER_VOLTAGE = 0x82,     //关闭适配器输出
	READ_ADAPTER_VOLTAGE = 0x83,      //读适配器电压
	READ_ADAPTER_CURRENT = 0x84,      //读适配器电流
	SET_ADAPTER_CURRENT = 0x86,       //设置适配器电流。
	REPLY_OK = 0xF0,                  //应答
	NO_ANSWER = 0xFD,                 //没应答
	SHAKE_HANDS_END = 0xFE,           //握手结束	
	//=======================================//
	ENUMERATION_OVER_TIME = 0xFF,
}ENUMERATION_MODEL;



typedef enum
{
	CURRENT_MATCHING = 0,
	FAULT_OUTPUT,
	MATCHING_VOLTAGE,
	TIMEOUT_WAIT,
	REPEAT_MATCHING_VOLTAGE,
	ADAPTER_OUTPUT_VOLTAGE,
	READ_VOLTAGE_VAL,
	READ_VOLTAGE_OVERTIME,
	REPEAT_READ_VOLTAGE,
}CONSTANT_CURRENT;



typedef struct _UART_COM
{
        uint8_t    index;
	uint8_t    Sendbuf[5];                    
	uint8_t    Rebuf[5];    

	uint16_t    crc1_val;
	uint16_t    crc2_val;

	float      Adapter_Max_Voltage;           
	float      Adapter_Max_Current;   
	uint16_t    Adapter_V_data;
	float      Adapter_Voltage;
	float      Set_Adapter_Voltage;
	float      Adapter_Current;

}UART_DATA;


extern  UART_DATA                 Uart_data;
extern  UART_ENUMERATION          Uart_enumeration;
extern  ENUMERATION_MODEL         enumeration_model;
extern  ENUMERATION_MODEL         last_time_enumeration_model;
extern  UART_MODEL                Uart_modle;
extern  UART_RECEIVE_WAIT         Uart_receive_wait;

extern  FAST_CHARGE_COM_STA       fast_charge_sta;

extern  FAST_CHANGE_CURRENT       fast_change_current;
extern  FAST_CHANGE_MODEL         fast_change_model;
extern       ADAPTER_TYPE         adapter_type;
extern  CONSTANT_CURRENT          constant_current;
extern  BATTERY_MODEL             Battery_mode;


extern uint8_t flagUsbIsCom;
extern uint8_t flagUsbIsUpdate;
extern uint8_t t_10msIsOk;


// charge 公共部分
extern void bat_chargeInit(void);
extern void charge_start(void);
extern void charge_stop(void);


//========================================================//
//串口通信部分
extern void UART_ConfigInit(void);
extern void UART_DISABLE(void);

extern uint8_t UART_IsrFunction(void);

extern  void UART_Send(uint8_t *Buffer, uint8_t Length);

//usb和串口的复用
extern void System_USB_Init(void);
extern void System_USB_stop(void);

//快充函数
void fastCharge_1msTaskIsr(void);

extern  void TIM14_init_setms(uint16_t OverswingTime);

extern uint16_t Modbus_CRC16(uint8_t *Buff_addr, uint16_t len);

extern void fastCharge_setAdapterV(float Vo);
extern void fastCharge_setAdapterI(float Ival);
extern void fastCharge_sendCmd(ENUMERATION_MODEL command);
extern uint8_t fastCharge_adjustI(uint16_t adj);

extern void fastCharge_revContinueData(void);
extern uint8_t fastCharge_handshake(void);
extern uint8_t fastCharge_heartBeat(void);
extern void fastCharge_decodeMsg(void);
extern void fastCharge_Init(void);


extern void fastChange_pollFunction(void);
extern void charge_Function(void);
extern void charge_Manage(void);
extern void charge_on(uint8_t onNum);
extern void charge_off(uint8_t offNum);

extern void UART_txDelay(void);
extern void UART_rxDelay(void);
extern void UART_heatBeatDelay(void);
extern uint8_t UART_getTxEnFlag(void);

extern void UART_setRxDelay(uint16_t delayTime);
extern void UART_setTxDelay(uint16_t delayTime);
extern void UART_setHeatBeatDelay(uint16_t delayTime);

extern void fastCharge_1scntFunction(void);
extern void fastCharge_uartComManage(void);

extern void fastCharge_sendReadMsg(uint8_t cmd);
extern void fastCharge_sendWritelMsg(uint8_t cmd, uint16_t dataMsg);
extern void fastCharge_startTx(void);

//文件内部使用
void fastCharge_displayManage(void);

void fastCharge_CC_Charge(void);


void fastCharge_measureVI(void);
void fastCharge_measureBatV(void);

void fastCharge_pollFunction(void);

void fastCharge_currentAdjust(void);

void fastCharge_CV_Charge(void);

void fastCharge_prerCC_Charge(void);

void fastCharge_CC_Charge(void);

void fastCharge_trickleCharge(void);

//对接文件
extern uint8_t charge_getStatus(void);


//调试使用 类似打印函数
extern void charge_debugInteger(uint8_t lineNum,uint16_t debugData,uint8_t isWait);
extern void charge_debugFloat(uint8_t lineNum,float debugData,uint8_t isWait);
extern void debug_displayInteger(uint8_t lineNum,uint16_t integerData);

extern void debug_waitKeyFree(void); //在key.c中使用，用来辅助调试

//外部实现
extern  void Change_Show_Main(uint8_t levda); //充电显示处理

extern void user_mode_charging(void);

extern void charge_atSmoke(void);
extern void charge_atSysOff(void);
extern void charge_atKeyPress(void);

extern void sys_debugInit(void);
extern void sys_debugTimeInit(void);
extern void sys_debugFastChargeMsg(void);
extern void sys_debugFastChargeData(void);




#endif
















