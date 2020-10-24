/**
  ******************************************************************************
  * @file    myMath.h 
  * @author  figo
  * @version V1.0.0
  * @date    04/01/2020
  * @brief   
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MYMATH_H
#define MYMATH_H

#include <math.h>
#include "typedef.h"

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

uint16_t myCal_IntegerDelt(uint16_t data1,uint16_t data2);

float myCal_FloatDelt(float data1,float data2);

uint16_t sys_calAverage(uint16_t *array, uint8_t num);

#endif


