/** @file
 *
 * @brief	 Redefine typedef
 *
 * @note	 This file typedef is for the Little Endian CPU, if the Big Endian CPU, please change the bytexx define sequence.
 *
 * History
 * ------------
 * @author  Leo Ou
 * @version V1.0.0
 * @date    2018-11-06
 *
 * Copyright Notice
 * ------------
 *
 */

#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

#ifndef  MCU_IS_ARM

typedef char                  INT8;
typedef int                   INT16;
typedef long                  INT32;

typedef char                  int8_t;
typedef int                   int16_t;
typedef long                  int32_t;

typedef unsigned char         UINT8;
typedef unsigned int          UINT16;
typedef unsigned long         UINT32;

typedef unsigned char         uint8_t;
typedef unsigned int          uint16_t;
typedef unsigned long         uint32_t;



#else

#include <stdint.h>

#define code const

#endif





#define BIT0	1		        //bit0 set mask
#define BIT1	2		        //bit1 set mask
#define BIT2	4		        //bit2 set mask
#define BIT3	8		        //bit3 set mask
#define BIT4	0x10		    //bit4 set mask
#define BIT5	0x20		    //bit5 set mask
#define BIT6	0x40		    //bit6 set mask
#define BIT7	0x80		    //bit7 set mask

#endif /* _LO_TYPEDEF_H_ */

