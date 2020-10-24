#ifndef         __GUI_H__
#define         __GUI_H__

#include "stm32f0xx.h"

#define GUI_MENUITEM_ADD 0x01
#define GUI_MENUITEM_DEC 0x10

#define OLED_MIRROR_IS_EN  1 //纵向反向显示
#define DEBUG_VERSION      1
#define DEBUG_IS_DOING     0 //1 正在调试 显示调试的日期时间 0 正式发布代码 只显示日期，不显示时间

//#define LCD_MIRROR 1  //80x160 132x162 

#ifdef LCD_MIRROR

#define NORMAL_MODE    0x08   
#define INVERTED_MODE  0x68

#define LCD_X_S1 0x00
#define LCD_X_S2 0x02

#define LCD_Y_S1 0x00
#define LCD_Y_S2 0x00

#else

#define NORMAL_MODE    0x30  //0011 0000 
#define INVERTED_MODE  0xA8

#if (OLED_IS_NEW==1)
#define LCD_X_S1 0x00  
#define LCD_Y_S1 0x18  
#else
#define LCD_X_S1 0x01  //1
#define LCD_Y_S1 0x1A  //52 / 2  = 26  
#endif

#endif

//565 
#define COLOR_RED           (uint16_t)(0x001F << 11) //5
#define COLOR_GREEN         (uint16_t)(0x003F << 5 )
#define COLOR_BLUE          (uint16_t)(0x001F)

#define COLOR_LIGHTGREEN      (uint16_t)(0x66FF)

#define COLOR_TEAL            (uint16_t)( COLOR_GREEN | COLOR_BLUE ) // GB 蓝绿色
#define COLOR_LIGHTYELLOW     (uint16_t)( COLOR_RED   | COLOR_GREEN)//浅黄色 
#define COLOR_PURPLE          0x80b4   //紫色 0x8010

#define COLOR_SYS_BLUE        (uint16_t)(0x54 | 0x9b << 5 | 0x9d << 11 )

#define COLOR_WHITE           0xFFFF
#define COLOR_GRAYWHITE       0xEDED
#define COLOR_BLACK           0x0000 //0x0841 


//颜色宏定义
/* LCD color *///16位颜色


//屏幕宽度宏定义
#define LCD_LEN      160  //80
#define LCD_WIDTH    80  //160

//字体宏定义

#define FONT_MSG_TEXT      14   // menu

extern void Display_setDrawMode(uint8_t mode); //1 正显示 0反显示
extern void Display_setNewLineMode(uint8_t enflag);//1 自动换行，遇到空格则换行
extern void Display_setBkColor(uint16_t color);
extern void Display_setFgColor(uint16_t color);
extern void menu_displayString(char *pstr,uint16_t x, uint16_t y,uint8_t font);

extern void Display_setDrawMode(uint8_t mode);

extern void oled_setCoordinateAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
extern void oled_setOriginalAddr(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
extern void oled_init(void);
extern void display_rgb(unsigned int dat);
extern void display_clear(uint8_t x,uint8_t y,uint8_t len,uint8_t high);

extern void draw_picture(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,const unsigned char *ptr,uint16_t BackColor,uint16_t Color);
extern void mYdraw_picture(uint8_t x,uint8_t y,uint8_t len,uint8_t width,const unsigned char *ptr);

typedef struct FONTST
{	
  uint16_t Background;    //背景颜色
	uint16_t Current_Color; //显示的颜色
  uint8_t  Coordinate_x1; //行起始坐标
  uint8_t  Coordinate_x2; //行结束坐标
  uint8_t  Coordinate_y1; //列起始坐标
  uint8_t  Coordinate_y2; //列结束坐标
  uint8_t  Font_high;     //字体高度
  uint8_t  Font_Wide;     //字体宽度 
  uint8_t  Font_Addr;     //显示数据地址 
	const uint8_t *ArrayPr;//
}FONTDAT;

extern FONTDAT fontdat;
extern void Write8High_data(FONTDAT *pr);
extern void Write16High_data(FONTDAT *pr);
extern void Write24High_data(FONTDAT *pr);

extern void display_8highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color);
extern void display_16HighData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color,uint16_t bColor);
extern void display_24highData(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint8_t fh,uint8_t fw,uint8_t fadr,const uint8_t *Pr,uint16_t Color);

extern void draw_point(uint8_t x,uint8_t y,uint16_t colour);
extern void draw_dividingLine(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2,uint16_t colour);	
extern void Show_Voltage(unsigned short v);
extern void Show_Current(uint8_t val);
extern void Display_Battery_icon(unsigned char data,uint16_t Color);
extern void Show_Material(void);
extern void Show_Batter_True_Color(uint8_t lda);
extern void Show_Bat_Voltage(uint16_t val,uint8_t x,uint8_t y,uint16_t colour);
extern void Show_MaxBatter_True_Color(uint8_t lda);
extern void oled_writeFrame(unsigned char DatHbit,unsigned char DatLbit);	
extern void The_atomizer_does_not_match(void);

extern void GUI_WarnMsgReturnHomePage(void);
extern void GUI_PreDisMessage(void);

extern void oled_wakeUp(void);
extern void oled_writeCmd(unsigned char Data);
extern void oled_writeData(unsigned char Data); 

extern void mYdraw_rect(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2,uint8_t disEn,uint32_t backColor,uint32_t fgcolor);
extern void draw_fillRect(uint16_t xl, uint16_t yl, uint16_t xr, uint16_t yr, uint32_t color);

extern uint16_t display_integer(uint32_t pstr, char *len, uint16_t x, uint16_t y, uint8_t font);
extern uint16_t display_float(float pstr, char *len, uint16_t x, uint16_t y, uint8_t font);

#endif



























