//============================================
// filename: mytasks.h
// chip    : 
// author  : figo
//����FreeRTOS ��������ļ���ϵ���� ���ļ�����Ϊͨ���ļ�
//����Ƶ���޸� ����My�Ա��������rtos�е�tasks����
// date    : 2020-01-05
//============================================

#ifndef  _MY_TASKS_H
#define  _MY_TASKS_H

#include "app_task.h"


typedef struct
{
  uint16_t TimeOut[Task_Num];
  uint16_t TimeCount[Task_Num];
  uint16_t t1msCnt;
} Task_param_t;

extern Task_param_t  Task;


#endif
