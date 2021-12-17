#ifndef __OLED_12864_H__
#define __OLED_12864_H__

/******************************************************************************
 * 	@brief	
 * 
 * 注：字体在 font.c 和 font.h 文件里面，已在 OLED_12864.c 文件里面声明
   
     Y                  OLED_12864
      │                                                      (127, 63)
    63├────────────────────────────────────────────────────────┐
      │                  Page 7                                │
    55├────────────────────────────────────────────────────────┤
      │                  Page 6                                │
    47├────────────────────────────────────────────────────────┤
      │                  Page 5                                │
    39├────────────────────────────────────────────────────────┤
      │                  Page 4                                │
    31├────────────────────────────────────────────────────────┤
      │                  Page 3                                │
    23├────────────────────────────────────────────────────────┤
      │                  Page 2                                │
    15├────────────────────────────────────────────────────────┤
      │                  Page 1                                │
    7 ├────────────────────────────────────────────────────────┤
      │                  Page 0                                │
    0 └────────────────────────────────────────────────────────┴─X
 (0,0)0                                                        127
                                                            21*8 6*8


 *******************************************************************************/



//================== 引用头文件  Header Files Includes =======================
	//单片机库函数头文件
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

	//c标准头文件(ANSI C)
#include <stdio.h>  	//用于输出字符串的sprintf函数等
#include <stdint.h>		//里面有 uint8_t 等 一系列方便的宏定义
#include <stdlib.h>		//内有 malloc(@) free(@) rand()等
#include <math.h>  		//数学包
#include <string.h>		//字符串(char数组)操作, 内存填写复制(memcpy($)等)
//#include <ctype.h>		//字符操作
//#include <setjmp.h>		//跳转函数
//#include <stdarg.h>		//变参函数使用
//#include <time.h>			//
//#include <errno.h>		//
//#include <stddef.h>		//
//#include <limits.h>		//里面有各种变量的最大值/最小值定义
//#include <float.h>		//
//#include <locale.h>		//本地化支持

	//c标准头文件(C99)
#include <stdbool.h>		//用于支持bool(布尔)变量
//#include <complex.h>		//复数运算
//#include <tgmath.h>		//智能判断数据类型的数学运算

	//c标准头文件(C11)
//#include <uchar.h> 		//Unicode支持

	//用户模块头文件
#include "main.h"		//必须包含
#include "LED.h"

//====================== 公有宏定义 Public Macros ===============================
//-----------------OLED端口定义----------------  	
/**	准备使用eUSCI_B2  SPI三线     其中
 * 	P6.0	原来是片选，改成RESET					初始化为GPIO
 * 	P3.5	用作CLK信号（SCL）
 * 	P3.6	SDA(SIMO) MOS
 * 	P3.7	DC(不用MISO了)（低电平指令-高电平内容）   初始化为GPIO	*/
#define OLED_SCL_PORTPIN	GPIO_PORT_P3, GPIO_PIN5
#define OLED_MOSI_PORTPIN	GPIO_PORT_P3, GPIO_PIN6
#define OLED_RST_PORTPIN	GPIO_PORT_P6, GPIO_PIN0
#define OLED_DC_PORTPIN		GPIO_PORT_P3, GPIO_PIN7
	//以下为OLED端口电平切换宏定义
#define OLED_SCL_CLR()	GPIO_setOutputLowOnPin(OLED_SCL_PORTPIN)
#define OLED_SCL_SET()	GPIO_setOutputHighOnPin(OLED_SCL_PORTPIN)

#define OLED_MOSI_CLR()	GPIO_setOutputLowOnPin(OLED_MOSI_PORTPIN)
#define OLED_MOSI_SET()	GPIO_setOutputHighOnPin(OLED_MOSI_PORTPIN)

#define OLED_RST_CLR()	GPIO_setOutputLowOnPin(OLED_RST_PORTPIN)
#define OLED_RST_SET()	GPIO_setOutputHighOnPin(OLED_RST_PORTPIN)

#define OLED_DC_CLR()	GPIO_setOutputLowOnPin(OLED_DC_PORTPIN)
#define OLED_DC_SET()	GPIO_setOutputHighOnPin(OLED_DC_PORTPIN)
//   ────────────────────────────────┘






#define OLED_DELAY_1MS_CYCLES	4000u		// 延时 1ms 使用的处理器循环数

	// OLED 屏幕 相关参数
#define OLED_X_WIDTH 		128		//   x  范围 0 ~ 127
#define OLED_Y_WIDTH 		64		//   y  范围 0 ~ 63
#define OLED_PAGE_AMPOUNT	8		// page 范围 0 ~ 8
#define OLED_X_6x8_AMOUNT	21		// 每行能放多少个 6*8 字符？ 此宏定义不用于本文件

	// 汉字大小，英文数字大小
#define OLED_TYPE_6X8		0	//  6 * 8   大小 ASCII 字符，可显示字符数：21.* 8
#define OLED_TYPE_8X16		1	//  8 * 16  大小 ASCII 字符，可显示字符数：16 * 4
#define OLED_TYPE_16X16		2	// 16 * 16  大小    汉字，   可显示字符数：16 * 4	

	//发送的是数据还是指令
#define OLED_COMMAND		0		
#define OLED_DATA 			1

	//当这个是1的时候使用硬件SPI，为0的时候使用软件SPI
#define OLED_SEND_HARDWARE	1
#define OLED_SEND_SOFTWARE	0
#define OLED_SEND_MODE		OLED_SEND_HARDWARE

	//使用 SPI 还是 I2C
#define OLED_MODE_SPI		0		
#define OLED_MODE_IIC		1
#define OLED_MODE			OLED_MODE_SPI


// #define OLED_XLevelL	0x00
// #define OLED_XLevelH	0x10
// #define OLED_XLevel		((XLevelH & 0x0F) * 16 + XLevelL)
// #define	OLED_Brightness	0xCF 





//============== 公有全局变量声明 Public Global Variables Defines ===============


//================== 公有函数声明 Public Function Defines =======================
extern void OLED_DelayMs(uint32_t);

extern void OLED_SetPosition(uint8_t x, uint8_t page);

extern void OLED_FillWithByte_singlePage(uint8_t bmp_data, uint8_t page_index);
extern void OLED_FillWithByte_partialPage(uint8_t bmp_data, uint8_t page_index, uint8_t x_start, uint8_t x_end);
extern void OLED_ClearAll(void);

extern void OLED_TurnOnDisplay(void);
extern void OLED_TurnOffDisplay(void);

extern void OLED_Restart(void);
extern void OLED_init(void);

extern void OLED_PrintString(char* str, uint8_t start_x, uint8_t page, uint8_t FontType, bool inverse);
extern void OLED_PrintString_Append(char* str, uint8_t FontType, bool inverse);
extern void OLED_PrintSpace(uint8_t length, uint8_t start_X, uint8_t page, uint8_t FontType, bool inverse);
extern void OLED_PrintSpace_Append(uint8_t length, uint8_t FontType, bool inverse);
extern void OLED_DrawBMP_twoPoints(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t end_x, uint8_t end_page);
extern void OLED_DrawBMP_size(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t x_size, uint8_t page_size);



#endif
