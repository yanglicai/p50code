
//============================================
// filename: myTasks.c
// chip    : 
// author  : figo
// version : v1.0.0	
// date    : 2020-01-05
//============================================

/* Includes */
#include "myTasks.h"
#include "app_task.h"

/* Private variables ---------------------------------------------------------*/
Task_param_t  Task;

/* Extern function  ---------------------------------------------------------*/
extern void Mode_Management(void);
extern void task_1msHandle(void);  	
extern void task_10msHandle(void); 	
extern void task_100msHandle(void);	
extern void task_1sHandle(void);  

/**
  * @brief  Task����.
  * @param  None
  * @retval None
  */  

void Task_Handle(void)
{						
	
	Mode_Management();
	
	if (Sys_Param.active_mode!=INIT_MODE) 
	{
     bat_chargeTaskHandle();		
	}
	
	task_1msHandle();
	
	task_10msHandle();
	
	task_100msHandle();
	
	task_1sHandle();  
	
}

/**
  * @brief  Task ������ʼ��.
  * @param  None 
  * @retval None
  */
void Task_Init(void)
{				
	/* ����Task���� */
	Task.TimeOut[button_Task]   = button_Task_Timer;
	Task.TimeOut[Status_Task]   = Status_Task_Timer;
	Task.TimeOut[Idle_Task]     = Idle_Task_Timer;
	Task.TimeOut[Show_Task]     = Show_Task_Timer;
	
	Task.TimeCount[button_Task] = Task.TimeOut[button_Task];
	Task.TimeCount[Status_Task] = Task.TimeOut[Status_Task];
	Task.TimeCount[Idle_Task]   = Task.TimeOut[Idle_Task];
	Task.TimeCount[Show_Task]   = Task.TimeOut[Show_Task]; 
  Task.t1msCnt = 0;
	
	TASK_TIME_START(); //  ���� Task timer		
}

/**
  * @brief  Task�������1ms����һ��.
  * @param  None
  * @retval None  
ʹ��˵��������1ms�ж���
  */
void Task_Time_Manage(void)
{
	/* ����ʱ����� */
	if(Task.TimeCount[button_Task]>0)	Task.TimeCount[button_Task]--;
	if(Task.TimeCount[Status_Task]>0)	Task.TimeCount[Status_Task]--;
	if(Task.TimeCount[Idle_Task]>0)		Task.TimeCount[Idle_Task]--;
	if(Task.TimeCount[Show_Task]>0)		Task.TimeCount[Show_Task]--;
	
	//������������
  if (Task.t1msCnt)   Task.t1msCnt--;

	/* ���ʱ����� */
	if(Sys_Param.firing_timeout>0)	Sys_Param.firing_timeout--;
	
	/*��ʾ����*/
	if (Sys_Param.show_timeout>0)		Sys_Param.show_timeout--;
	
	if (Sys_Param.menu_timeOut>0)		Sys_Param.menu_timeOut--;
	
	
	
} 


/**
  * @brief  Task��ʱ����.
  * @param  task_number: ������
  * @retval 1:����ʱ���ѵ�  0:����ʱ��δ��
  */
uint8_t Task_TimeOut(uint8_t task_number)
{
	/* �������ʱ�䳬ʱ */
	if (Task.TimeCount[task_number]==0)
	{
		Task.TimeCount[task_number] = Task.TimeOut[task_number];    
		return 1;
	}
	else
	{
		return 0;
	}
}



