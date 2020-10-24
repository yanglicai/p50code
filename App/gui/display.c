//======================================
// File Name:  dev/display.c 
// brief: 显示实现 显示字体 显示图片 绘图处理
// Date: 2019.12.21 
//======================================

#include "stm32f0xx.h"
#include "GUI.h"  
#include "delay.h"
#include "systick.h"
#include <string.h>
#include <stdio.h>


FONTDAT  fontdat;  

static uint32_t backColor=COLOR_BLACK;//默认背景色（文字）	
static uint32_t foregroundColor=COLOR_WHITE;  //默认前景色（画图，文字）
static uint8_t  drawMode = 0;	  //0正显示 


/****************************************************************************
* 名    称：void Display_setDrawMode(uint8_t mode)
* 功    能：设置图片的反白显示，即背景色和前景色调换 
****************************************************************************/
void Display_setDrawMode(uint8_t mode)
{
	drawMode = mode;
}   

/****************************************************************************
* 名    称：void Display_setBkColor(uint16_t color)
* 功    能：设置图片的背景色 
****************************************************************************/
void Display_setBkColor(uint16_t color)
{
	backColor = color;
}

/****************************************************************************
* 名    称：void Display_setFgColor(uint16_t color)
* 功    能：设置图片的前景色 
****************************************************************************/
void Display_setFgColor(uint16_t color)
{
	foregroundColor = color;
} 

extern void oled_writeContinueData(unsigned char *buf,uint16_t size);
extern void oled_setWriteData(void);  

extern void oled_clrWriteData(void);

uint8_t colorData[330];

#define LEN_BYTE_TOTAL  (uint16_t) (0xA2 * 2)

/*********************************************
函数名称:display_rgb
函数说明:全屏显示    0xA0     0x50
*********************************************/ 
void display_rgb(unsigned int dat)
{	
	uint16_t len,high,cnt;
	uint8_t highByte,lowByte;
	
	len = 0xA2;
	high = 0x52; 	

	oled_setCoordinateAddr(0x00,len,0x00,high);
	
	unsigned int i,j;
	
	highByte = (uint8_t)(dat>>8);
	lowByte = (uint8_t)(dat&0xFF); 
	
	for (j=0; j < len;j++)
	{ 
		cnt = j * 2;
		colorData[cnt] = highByte;
	  colorData[cnt + 1] = lowByte; 
	}
	
	oled_setWriteData();
	
	for(i=0;i<high;i++)		
	{		
		oled_writeContinueData(colorData,LEN_BYTE_TOTAL); 
	}	
	
	oled_clrWriteData();
} 

/*********************************************
函数名称:void display_clear(uint8_t x,uint8_t y,uint8_t len,uint8_t high)
函数说明:清屏显示处理
*********************************************/ 
void display_clear(uint8_t x,uint8_t y,uint8_t len,uint8_t high)
{
  uint16_t cnt;
	uint8_t highByte,lowByte;
	uint16_t dat = COLOR_BLACK;

	oled_setCoordinateAddr(x,x + len - 1,y,y + high - 1);
	
	unsigned int i,j;
	
	highByte = (uint8_t)(dat>>8);
	lowByte = (uint8_t)(dat&0xFF); 
	
	for (j=0; j < len;j++)
	{ 
		cnt = j * 2;
		colorData[cnt] = highByte;
	  colorData[cnt + 1] = lowByte; 
	}
	
	oled_setWriteData();
	
	cnt = len * 2;
	
	for(i=0;i<high;i++)		
	{		
		oled_writeContinueData(colorData,cnt); 
	}	
	
	oled_clrWriteData();
} 

/*********************************************
函数名称:static display_writeFontData
函数说明:显示字体数据
*********************************************/
static void display_writeFontData(FONTDAT *pr)
{	
  unsigned char   i, j, k,width;
	unsigned char   compare;
	unsigned char   val = 0;
	unsigned short  tem = 0;
	unsigned short  num = 0;


	//开始结束坐标地址
	oled_setCoordinateAddr(pr->Coordinate_x1, pr->Coordinate_x2, pr->Coordinate_y1, pr->Coordinate_y2);

	if (pr->Font_high % 8 == 0)
	{
		num = (pr->Font_high / 8)*(pr->Font_Wide);
	}
	else
	{
		num = (pr->Font_high / 8 + 1)*(pr->Font_Wide);
	}

	tem = num*(pr->Font_Addr - 1);
	
	val = 0x01;
	
	width = pr->Font_high / 8;
	
	uint8_t fgColorData[2],bkColorData[2];
	
	fgColorData[0] = (uint8_t)((pr->Current_Color & 0xFF00) >> 8);
	fgColorData[1]   = (uint8_t)(pr->Current_Color & 0x00FF);
	
	bkColorData[0] = (uint8_t)((pr->Background & 0xFF00) >> 8);
	bkColorData[1]   = (uint8_t)(pr->Background & 0x00FF);
	
	for (j = 0; j< width; j++)
	{
		for (i = 0; i<8; i++)
		{
			compare = val << i;

			for (k = 0; k<pr->Font_Wide; k++)
			{
				if ((pr->ArrayPr[tem + k + j*pr->Font_Wide] & compare) == compare)
				{
					//显示字体
					oled_writeFrame(fgColorData[0], fgColorData[1]);
				}
				else
				{
					//显示字体背景颜色.
					oled_writeFrame(bkColorData[0], bkColorData[1]);
				}
			}

		}
	}
	if (pr->Font_high % 8>0)
	{

		for (i = 0; i<(pr->Font_high % 8); i++)
		{
			compare = val << i;
			for (k = 0; k<pr->Font_Wide; k++)
			{
				if ((pr->ArrayPr[tem + k + j*pr->Font_Wide] & compare) == compare)
				{
					//显示字体
					oled_writeFrame(fgColorData[0], fgColorData[1]);
				}
				else
				{
					//显示字体背景颜色.
					oled_writeFrame(bkColorData[0], bkColorData[1]);
				}
			}
		}

	}
}  

/*********************************************
函数名称:display_8highData
函数说明:8
*********************************************/
void display_8highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color)
{
	fontdat.Background=0x0000;
	
	fontdat.Current_Color=Color;
	fontdat.Coordinate_x1=x1;
	fontdat.Coordinate_x2=x2;
	fontdat.Coordinate_y1=y1;
	fontdat.Coordinate_y2=y2;
	fontdat.ArrayPr=Pr;
	fontdat.Font_Addr=fadr;
	fontdat.Font_Wide=fw;
	fontdat.Font_high=fh;	
	
	display_writeFontData(&fontdat);
}	


/*********************************************
函数名称:display_16HighData
函数说明:16
*********************************************/
void display_16HighData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color,uint16_t bColor)
{
	fontdat.Background=bColor;
	fontdat.Current_Color=Color;//0xFFFF;
	fontdat.Coordinate_x1=x1;
	fontdat.Coordinate_x2=x2;
	fontdat.Coordinate_y1=y1;
	fontdat.Coordinate_y2=y2;
	fontdat.ArrayPr=Pr;
	fontdat.Font_Addr=fadr;
	fontdat.Font_Wide=fw;
	fontdat.Font_high=fh;	
	display_writeFontData(&fontdat);
}	 

/*********************************************
函数名称:display_24highData
函数说明:16    96
*********************************************/
void display_24highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t colour)
{
	fontdat.Background=0x0000;
	fontdat.Current_Color=colour;
	fontdat.Coordinate_x1=x1;
	fontdat.Coordinate_x2=x2;
	fontdat.Coordinate_y1=y1;
	fontdat.Coordinate_y2=y2;
	fontdat.ArrayPr=Pr;
	fontdat.Font_Addr=fadr;
	fontdat.Font_Wide=fw;
	fontdat.Font_high=fh;	
	display_writeFontData(&fontdat);
}

/*********************************************
函数名称:display_32highData
函数说明:32
*********************************************/
void display_32highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color,uint16_t bColor)
{
	fontdat.Background=bColor;
	fontdat.Current_Color=Color;//0xFFFF;
	fontdat.Coordinate_x1=x1;
	fontdat.Coordinate_x2=x2;
	fontdat.Coordinate_y1=y1;
	fontdat.Coordinate_y2=y2;
	fontdat.ArrayPr=Pr;
	fontdat.Font_Addr=fadr;
	fontdat.Font_Wide=fw;
	fontdat.Font_high=fh;	
	
	display_writeFontData(&fontdat);
}	

#define TAB_INDEX_MAX  72
#define TAB_BYTE_TOTAL 72 * 16

const char tab_char_ascii_index[TAB_INDEX_MAX][1]=
{	
		
	"A","B","C","D","E","F","G","H","I","J", //10
	
	"K","L","M","N","O","P","Q","R","S","T", //10
	
	"U","V","W","X","Y","Z",                 //6
	
	"a","b","c","d","e","f","g","h","i","j", //10
	
	"k","l","m","n","o","p","q","r","s","t", //10
	
	"u","v","w","x","y","z",                  //6	
	
	"!","0","1","2","3","4",                  //6
	
	"5","6","7","8","9", " ",                 //6
	
	"=","_",".",
	
};

//宋体 16*16 阴码 列行式 逆向 十六进制;
//每行：点阵16，索引8，输出C51

const unsigned char Ascii168_tabe[TAB_BYTE_TOTAL] =
{
0x00,0x00,0x00,0xE0,0x1C,0xE0,0x00,0x00,0x00,0x38,0x07,0x04,0x04,0x04,0x07,0x38,/*"A",0*/
0x00,0xFC,0x84,0x84,0x84,0x84,0x48,0x30,0x00,0x3F,0x20,0x20,0x20,0x20,0x11,0x0E,/*"B",1*/
0x00,0xF0,0x08,0x04,0x04,0x04,0x08,0x30,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0C,/*"C",2*/
0x00,0xFC,0x04,0x04,0x04,0x04,0x18,0xE0,0x00,0x3F,0x20,0x20,0x20,0x20,0x18,0x07,/*"D",3*/
0x00,0xFC,0x84,0x84,0x84,0x84,0x84,0x04,0x00,0x3F,0x20,0x20,0x20,0x20,0x20,0x20,/*"E",4*/
0x00,0xFC,0x84,0x84,0x84,0x84,0x84,0x04,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x00,/*"F",5*/
0x00,0xF0,0x08,0x04,0x04,0x04,0x08,0x10,0x00,0x0F,0x10,0x20,0x20,0x21,0x11,0x3F,/*"G",6*/
0x00,0xFC,0x80,0x80,0x80,0x80,0x80,0xFC,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,0x3F,/*"H",7*/
0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,/*"I",8*/
0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x18,0x20,0x20,0x20,0x1F,0x00,0x00,/*"J",9*/
0x00,0xFC,0x80,0x40,0x20,0x10,0x08,0x04,0x00,0x3F,0x01,0x02,0x04,0x08,0x10,0x20,/*"K",10*/
0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x20,0x20,0x20,0x20,0x20,0x00,/*"L",11*/
0x00,0xFC,0x30,0xC0,0x00,0xC0,0x30,0xFC,0x00,0x3F,0x00,0x00,0x07,0x00,0x00,0x3F,/*"M",12*/
0x00,0xFC,0x18,0x60,0x80,0x00,0x00,0xFC,0x00,0x3F,0x00,0x00,0x01,0x06,0x18,0x3F,/*"N",13*/
0x00,0xF0,0x08,0x04,0x04,0x04,0x08,0xF0,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,/*"O",14*/
0x00,0xFC,0x04,0x04,0x04,0x04,0x88,0x70,0x00,0x3F,0x01,0x01,0x01,0x01,0x00,0x00,/*"P",15*/
0x00,0xF0,0x08,0x04,0x04,0x04,0x08,0xF0,0x00,0x0F,0x10,0x20,0x20,0x28,0x10,0x2F,/*"Q",16*/
0x00,0xFC,0x04,0x04,0x04,0x04,0x88,0x70,0x00,0x3F,0x01,0x03,0x05,0x09,0x10,0x20,/*"R",17*/
0x00,0x38,0x44,0x84,0x84,0x04,0x04,0x18,0x00,0x18,0x20,0x20,0x20,0x21,0x22,0x1C,/*"S",18*/
0x00,0x04,0x04,0x04,0xFC,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,/*"T",19*/
0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x1F,0x20,0x20,0x20,0x20,0x20,0x1F,/*"U",20*/
0x00,0x1C,0xE0,0x00,0x00,0x00,0xE0,0x1C,0x00,0x00,0x00,0x07,0x38,0x07,0x00,0x00,/*"V",21*/
0x00,0xFC,0x00,0x80,0x7C,0x80,0x00,0xFC,0x00,0x03,0x3C,0x03,0x00,0x03,0x3C,0x03,/*"W",22*/
0x00,0x0C,0x30,0x40,0x80,0x40,0x30,0x0C,0x00,0x30,0x0C,0x03,0x00,0x03,0x0C,0x30,/*"X",23*/
0x00,0x0C,0x30,0xC0,0x00,0xC0,0x30,0x0C,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,/*"Y",24*/
0x00,0x04,0x04,0x04,0xC4,0x34,0x0C,0x00,0x00,0x30,0x2C,0x23,0x20,0x20,0x20,0x00,/*"Z",25*/
0x00,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x00,0x1C,0x22,0x22,0x22,0x22,0x1F,0x20,/*"a",26*/
0x00,0xFC,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x3F,0x10,0x20,0x20,0x20,0x10,0x0F,/*"b",27*/
0x00,0x00,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x0F,0x10,0x20,0x20,0x20,0x20,0x10,/*"c",28*/
0x00,0x00,0x80,0x40,0x40,0x40,0x80,0xFC,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x3F,/*"d",29*/
0x00,0x00,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x0F,0x12,0x22,0x22,0x22,0x22,0x13,/*"e",30*/
0x00,0x40,0x40,0xF8,0x44,0x44,0x44,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,/*"f",31*/
0x00,0x00,0x80,0x40,0x40,0x80,0xC0,0x00,0x00,0x27,0x48,0x50,0x50,0x48,0x3F,0x00,/*"g",32*/
0x00,0xFC,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x3F,0x00,/*"h",33*/
0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,/*"i",34*/
0x00,0x00,0x00,0x00,0xCC,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x3F,0x00,0x00,0x00,/*"j",35*/
0x00,0xFC,0x00,0x00,0x80,0x40,0x20,0x00,0x00,0x3F,0x02,0x05,0x08,0x10,0x20,0x00,/*"k",36*/
0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,/*"l",37*/
0x00,0xC0,0x40,0x40,0x80,0x40,0x40,0x80,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00,0x3F,/*"m",38*/
0x00,0xC0,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x3F,0x00,/*"n",39*/
0x00,0x00,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x0F,0x10,0x20,0x20,0x20,0x10,0x0F,/*"o",40*/
0x00,0xC0,0x80,0x40,0x40,0x40,0x80,0x00,0x00,0x7F,0x08,0x10,0x10,0x10,0x08,0x07,/*"p",41*/
0x00,0x00,0x80,0x40,0x40,0x40,0x80,0xC0,0x00,0x07,0x08,0x10,0x10,0x10,0x08,0x7F,/*"q",42*/
0x00,0x00,0xC0,0x80,0x40,0x40,0x40,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,0x00,/*"r",43*/
0x00,0x80,0x40,0x40,0x40,0x40,0x80,0x00,0x00,0x11,0x22,0x22,0x24,0x24,0x18,0x00,/*"s",44*/
0x00,0x40,0x40,0xFC,0x40,0x40,0x40,0x00,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x00,/*"t",45*/
0x00,0xC0,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x3F,0x00,/*"u",46*/
0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x00,0x03,0x0C,0x30,0x0C,0x03,0x00,/*"v",47*/
0x00,0xC0,0x00,0x00,0x80,0x00,0x00,0xC0,0x00,0x07,0x38,0x0C,0x03,0x0C,0x38,0x07,/*"w",48*/
0x00,0x40,0x80,0x00,0x00,0x00,0x80,0x40,0x00,0x20,0x10,0x09,0x06,0x09,0x10,0x20,/*"x",49*/
0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0x40,0x43,0x4C,0x30,0x0C,0x03,0x00,/*"y",50*/
0x00,0x40,0x40,0x40,0x40,0x40,0xC0,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x20,0x00,/*"z",51*/
0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x00,0x00,0x00,/*"!",52*/
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,/*"0",29*/
0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,/*"1",30*/
0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,/*"2",31*/
0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,/*"3",32*/
0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,/*"4",33*/
0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,/*"5",34*/
0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,/*"6",35*/
0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,/*"7",36*/
0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,/*"8",37*/
0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,/*"9",38*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*" ",41*/
0x00,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x02,/*"=",42*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,/*"_",39*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,/*".",40*/
};

/****************************************************************************
* static void menu_displayChar(char *ASCii, uint16_t x, uint16_t y,uint8_t font);

****************************************************************************/
static void menu_displayChar(char *ASCii, uint16_t x, uint16_t y,uint8_t font)
{
	
	uint16_t j=0;
	uint8_t font_len,font_width;  
  

	
	 for(j=0;j<TAB_INDEX_MAX;j++)
	 {
		 if (tab_char_ascii_index[j][0]==ASCii[0])	
		break;
	 }
		
		font_len = 8; 
		font_width = 16;      	  
    
    if (j<TAB_INDEX_MAX)
    {
			  if (drawMode)
				{
					 display_16HighData(x,x+font_len-1,y,y+font_width-1,font_width,font_len,j+1,Ascii168_tabe,backColor,foregroundColor);
				}
				else
				{
					 display_16HighData(x,x+font_len-1,y,y+font_width-1,font_width,font_len,j+1,Ascii168_tabe,foregroundColor,backColor);
				}
    }		 
}

/****************************************************************************
* function:void menu_displayString
           (int8_t *pstr,uint16_t x, uint16_t y,
            uint8_t font,
            //uint32_t bkColr,uint32_t fgColor)
****************************************************************************/
void menu_displayString(char *pstr,uint16_t x, uint16_t y,uint8_t font)
{	 	   
			while(1)
	    {
	       if (*pstr == 0)
	       {
	            return;
	       }	
	        
				 menu_displayChar(pstr, x, y,font);	 				 
				 
				 x += 8;	
				 
				 if (font!=0) x += 1;
				 
				 pstr += 1;   
	    } 
}

/****************************************************************************
* 名    称：void display_float(float pstr,char *len,uint16_t x, uint16_t y,uint8_t font)
* 功    能：在指定位置显示浮点数字
* 入口参数：pstr浮点数，len浮点数输出参数。x,y位置 font 字体
* 出口参数：无
* 说    明：Figo
* 调用方法：无
****************************************************************************/
uint16_t display_float(float pstr, char *len, uint16_t x, uint16_t y, uint8_t font)
{
	char str[20];

	sprintf(str, len, pstr);
	
	menu_displayString(str, x, y, font);

	return 1;
}

/****************************************************************************
* 名    称：uint16_t display_integer(uint32_t pstr,char *len,uint16_t x, uint16_t y,uint8_t font)
* 功    能：在指定位置显示整形
* 入口参数：pstr 整形数据，len 输出参数。x,y位置 font 字体
* 出口参数：无
* 说    明：Figo
* 调用方法：无
****************************************************************************/
uint16_t display_integer(uint32_t pstr, char *len, uint16_t x, uint16_t y, uint8_t font)
{
	char str[20];

	sprintf(str, len, pstr);

	menu_displayString(str, x, y, font);

	return 1;

}



//===========绘图处理========================
//-----------------------------修改LED 色块选择时，抖动的问题 声明放在struct.h中

#define  oled_SetArea oled_setCoordinateAddr	

extern SPI_HandleTypeDef hspi1;

  
/****************************************************************************
* 名    称：static  void draw_writeContinueSameColor(uint32_t dataLen,uint32_t color)
* 功    能：连续写相同颜色
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法： 666 高6bit
****************************************************************************/
static void draw_writeContinueSameColor(uint32_t dataLen,uint32_t color)
{
  volatile uint32_t len;
	uint8_t dataByte[2];
  
  len = dataLen;	
	
	oled_setWriteData();

  dataByte[0] = (uint8_t)((color&0xFF00)>>8);	
	dataByte[1] = (uint8_t)(color&0x00FF);		
  
  do
	{				 
		 HAL_SPI_Transmit(&hspi1,(uint8_t *)dataByte,2,0xFFFFFF); 
	}while(len--);	
	
	oled_clrWriteData();
}

/****************************************************************************
* 名    称：void draw_hLine(uint16_t x,uint16_t y,uint8_t len,uint8_t width,uint32_t color)
* 功    能：画水平线
* 入口参数：x,y起始点，len长度(像素)，width宽度(像素)
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：
****************************************************************************/
void draw_hLine(uint16_t x, uint16_t y, uint8_t len, uint8_t width,uint32_t color)
{
	
	if (x+len>LCD_LEN) len = LCD_LEN-x;
	
	oled_SetArea(x,x+len,y,y+width);  
	 
	draw_writeContinueSameColor((uint32_t)(len*width),color);	
}

/****************************************************************************
* 名    称：void draw_vLine(uint16_t x,uint16_t y,uint16_t len,uint8_t width,uint32_t color)
* 功    能：画垂直线
* 入口参数：x,y起始点，len长度(像素)，width宽度(像素)
* 出口参数：当前座标颜色值
* 说    明：2017.02.09 figo
* 调用方法：
****************************************************************************/
void draw_vLine(uint16_t x, uint16_t y, uint8_t len, uint8_t width,uint32_t color)
{
	
	if(y + len > LCD_WIDTH) len = LCD_WIDTH - y;	
		
	oled_SetArea(x,x+width-1,y,y+len); 
	 
	draw_writeContinueSameColor((uint32_t)(len*width),color);
}

/****************************************************************************
* 名    称：void draw_fillRect(uint16_t xl, uint16_t yl, uint16_t xr, uint16_t yr,uint32_t color)
* 功    能：指定区域清屏，填充背景色。
* 入口参数：Start xl,yl; End xr,yr
* 出口参数：无
* 说    明： 正反显参数无效
* 调用方法：无 可以用来清除局部显示，局部刷屏用
****************************************************************************/
void draw_fillRect(uint16_t xl, uint16_t yl, uint16_t xr, uint16_t yr, uint32_t color)
{
	uint16_t w, h;

	if ((xl>xr) || (yl>yr)) return;

	w = xr - xl + 1;
	h = yr - yl + 1;

	oled_SetArea(xl,xr,yl,yr);

	draw_writeContinueSameColor((uint32_t)(w*h), color);
}

/****************************************************************************
* 名    称：void mYdraw_rect(uint16_t x1,uint16_t yl,uint16_t len,uint16_t width,uint8_t disEn,uint32_t backColor,uint32_t fgcolor)
* 功    能：画方框
* 入口参数：x,y起始点，len长度(像素)，width宽度(像素)
* 出口参数：当前座标颜色值
* 说    明：figo 2020.01.03
* 调用方法：用于菜单选择处理
****************************************************************************/
void mYdraw_rect(uint16_t x1,uint16_t yl,uint16_t len,uint16_t width,uint8_t disEn,uint32_t backColor,uint32_t fgcolor)
{
  uint16_t xLeft,xRight,yUp,yDown;
  
  xLeft = x1;
  xRight = x1 + len - 1;
  yUp = yl;
  yDown = yl + width - 1;
  
  if (disEn)
  {
    draw_hLine(xLeft,yUp,    xRight-xLeft,1,fgcolor);
		draw_hLine(xLeft,yDown,  xRight-xLeft,1,fgcolor);
    
		draw_vLine(xLeft, yUp,   yDown-yUp,1,fgcolor);
		draw_vLine(xRight,yUp,   yDown-yUp,1,fgcolor);   
  }
  else
  {
    draw_hLine(xLeft,yUp,    xRight-xLeft,1,backColor);
		draw_hLine(xLeft,yDown,  xRight-xLeft,1,backColor);
    
		draw_vLine(xLeft, yUp,   yDown-yUp,1,backColor);
		draw_vLine(xRight,yUp,   yDown-yUp,1,backColor);   
  }
}

/*********************************************
函数名称:draw_point
函数说明:绘制一个点
*********************************************/ 
void draw_point(uint8_t x,uint8_t y,uint16_t colour)		
{
	 oled_setCoordinateAddr(x,x,y,y);
	 oled_writeFrame(((colour&0xFF00)>>8),(colour&0x00FF));
}

/*********************************************
函数名称:draw_dividingLine
函数说明:显示分格线.
*********************************************/
void draw_dividingLine(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint16_t colour)	
{
	int c,s;
	
  uint8_t x;
  uint8_t y;
	x = x2-x1;
	y = y2-y1;
	
  oled_setCoordinateAddr(x1,x2,y1,y2);
	
	for(c=0;c<y;c++)
	{
		for(s=0;s<=x;s++)
		{ oled_writeFrame(((colour&0xFF00)>>8),colour&0x00FF);}	
	}	
}

/*********************************************
函数名称:mYdraw_picture my interface
*********************************************/
void mYdraw_picture(uint8_t x,uint8_t y,uint8_t len,uint8_t width,const unsigned char *ptr)	
{		
	volatile uint8_t y1,y2;
	volatile uint8_t x1,x2;
	
	x1 = x; 
	x2 = x + len - 1;
	y1 = y;
	y2 = y + width - 1;
	
	draw_picture(x1,x2,y1,y2,ptr,0x0000,0x051D);	
}

#define LCD_setPointColor   draw_point

/* 像素--单位显示*/
void draw_hollowCirclePixel(uint8_t disbit, uint16_t x, uint16_t y, int16_t a, int16_t b, uint32_t color)
{
	if (disbit & 0x01)
		LCD_setPointColor(x - a, y - b, color); 	// 0~7   1   

	if (disbit & 0x02)
		LCD_setPointColor(x - b, y - a, color); 	// 6~7   2

	if (disbit & 0x04)
		LCD_setPointColor(x - b, y + a, color); 	// 6~5	 3 

	if (disbit & 0x08)
		LCD_setPointColor(x - a, y + b, color); 	// 4~5   4    

	if (disbit & 0x10)
		LCD_setPointColor(x + a, y + b, color); 	// 4~3	 5   

	if (disbit & 0x20)
		LCD_setPointColor(x + b, y + a, color); 	// 2~3	 6

	if (disbit & 0x40)
		LCD_setPointColor(x + b, y - a, color); 	// 2~1	 7

	if (disbit & 0x80)
		LCD_setPointColor(x + a, y - b, color); 	// 0~1	 8 
}

/* 画圆函数，
disbit 代表画完整的圆还是一部分的圆，圆弧
x，y为圆心坐标，r为半径，colour为颜色 空心圆
*/
void draw_hollowCircle(uint8_t disbit, uint8_t x, uint8_t y, uint8_t r, uint32_t color)
{
	volatile int16_t a = 0, b;
	volatile int16_t di;


	//画圆2  双圆重叠   
	a = 0; b = r;
	
	di = 3 - (r << 1);             //判断下个点位置的标志
	
	while (a <= b)
	{
		draw_hollowCirclePixel(disbit, x, y, a, b, color);
		
		//使用Bresenham算法画圆     
		if (di<0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b--;
		}
		
		a++;
	}
}


/*********************************************
函数名称:draw_picture
*********************************************/
void draw_picture(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,const unsigned char *ptr,uint16_t BackColor,uint16_t Color)	
{	
	volatile uint16_t len,width;
	volatile uint16_t dataLen;
	volatile uint16_t i;
	volatile uint8_t *pData;
	
	len = (uint16_t)(x2 - x1 + 1);
	width = (uint16_t)(y2 - y1 + 1);
	dataLen = (uint16_t)(len * 2);
	pData = (uint8_t *)ptr;
	
  oled_setCoordinateAddr(x1,x2,y1,y2); 
	
	oled_setWriteData();
	
	for (i=0;i <width;i++)
	{
	  oled_writeContinueData((uint8_t*)pData,dataLen); 
    pData += dataLen;		
	}		
	
	oled_clrWriteData();
	
}

void display_clearScreen(void)
{
	 display_rgb(COLOR_BLACK);
}

void display_colorTest(void)
{
	 display_rgb(COLOR_RED);
	 systick_delay_ms(500);
	 display_rgb(COLOR_GREEN);
	 systick_delay_ms(500); 	
	 display_rgb(COLOR_BLUE);
	 systick_delay_ms(500);
	  
   display_rgb(COLOR_WHITE);
	 systick_delay_ms(500); 	
	 display_rgb(COLOR_BLACK);
	 systick_delay_ms(500);
}

extern void oled_Show_Power(uint8_t power);
extern void oled_Show_AtoRes(uint16_t res);

void display_test(void)
{
	 static uint8_t powerV,resV;
	 static uint8_t cnt;
	   	
//	 display_rgb(COLOR_BLACK);
	
	 powerV = 50;
	 resV = 20;
	
	 for (cnt=0;cnt < 200;cnt++)
	{
		 oled_Show_Power(powerV++);
		 systick_delay_ms(300); 
		
		 resV += 10;
		 oled_Show_AtoRes(resV);
	}	
}


