#ifndef __LED_H__
#define __LED_H__
/******************************************************************************
 官方开发板LED模块
 LED1（左侧—）为红色单色LED
		P1.0
 LED2（右侧）为一个RGB模块	
		
		
暂时决定LED2:
		红灯快速闪烁：	遇到软件/硬件错误
		黄色：			定时器资源重复使用
		白色：			正在初始化
			
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


//====================== 公有宏定义 Public Macros ===============================
#define LED2_COLOUR_BLACK	0
#define LED2_COLOUR_RED		1
#define LED2_COLOUR_GREEN	2
#define LED2_COLOUR_BLUE	3
#define LED2_COLOUR_CYAN	4
#define LED2_COLOUR_MAGENTA	5
#define LED2_COLOUR_YELLOW	6
#define LED2_COLOUR_WHITE	7
#define	LED2_RETVAL_ERROR	8

#define LED2_COLOUR_MAX		7
#define LED2_COLOUR_MIN		0


#define LED_ON	1
#define LED_OFF	0

#define LED1	GPIO_PORT_P1, GPIO_PIN0
#define LED_R	GPIO_PORT_P2, GPIO_PIN0
#define LED_G	GPIO_PORT_P2, GPIO_PIN1
#define LED_B	GPIO_PORT_P2, GPIO_PIN2

#define LED2_ROLLUP		0
#define LED2_ROLLDOWN	1


//============== 公有全局变量声明 Public Global Variables Defines ===============
extern uint8_t LED2_colour;





//================== 公有函数声明 Public Function Defines =======================
extern void LED_Init(void);
extern bool LED1_switch(bool status);
extern bool LED1_toggle(void);
extern uint8_t LED2_switch(uint8_t colour);
extern uint8_t LED2_roll(bool direction);
extern uint8_t LED2_toggle(uint8_t colour);

#endif
