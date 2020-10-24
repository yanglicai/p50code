//============================================
// filename: mytasks.h
// chip    : 
// author  : figo
//参照FreeRTOS 初步完成文件体系建构 此文件后续为通用文件
//无需频繁修改 加上My以便和真正的rtos中的tasks区分
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
