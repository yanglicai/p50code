//type.h

#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>    //¿âÎÄ¼þ

typedef struct {
	uint8_t b0 : 1;
	uint8_t b1 : 1;
	uint8_t b2 : 1;
	uint8_t b3 : 1;
	uint8_t b4 : 1;
	uint8_t b5 : 1;
	uint8_t b6 : 1;
	uint8_t b7 : 1;
}b8;

typedef union {
	b8    BIT;      
	uint8_t	BYTE;    
}ub8;

typedef union {
	struct {
		uint8_t BYTEL;
		uint8_t BYTEH;
	}BYTES;
	uint16_t  WORD;
}uw16;

typedef union {
	struct {
		uint8_t BYTE0; //LSB is First
		uint8_t BYTE1;
		uint8_t BYTE2;
		uint8_t BYTE3;
	}BYTES;
	uint32_t DWORD;
}uw32;


typedef struct
{
	uint8_t modeNum;
	void(*modeFun)(void);
	uint8_t keyMode;
} _MODE_FUNTYPE;


#endif

 //================================================== END OF FILES==================================//

