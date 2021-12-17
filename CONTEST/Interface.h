#ifndef __INTERFACE_H__
#define __INTERFACE_H__

/******************************************************************************
 * 	@brief	
 * 
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
#include "Remocon_Keys.h"
#include "OLED_12864.h"
//====================== 公有宏定义 Public Macros ===============================

#define MODE_AMOUNT				3	// 总数值 \b 必须更改！从 1 开始计数，该值为 最高序号 - 1

#define MODE_DEFAULT			0
#define MODE_LINE_TRACING		1
#define MODE_CONTROL_CAR		2


typedef struct{
	//uint8_t index;
	char name[22];					//该模式在屏幕中显示的名称  小心溢出！
	//bool isDisplayingModeName;		//是否在屏幕第一行显示模式名称
	void (*Start_func)(void);		//该模式的 初始化与开始函数
	void (*Cycle_func)(void);		//该模式的 主函数循环调用函数 (含 显示屏刷新 Display)
	void (*Halt_func)(void);		//该模式的 停止与复位函数
	
	// void (*Key_func[KEY_AMOUNT])(void);	//按钮服务函数向量
}Mode_typedef;


//============== 公有全局变量声明 Public Global Variables Defines ===============
extern Mode_typedef Interface_modeConfig;
extern uint8_t 		Interface_currentMode;


//================== 公有函数声明 Public Function Defines =======================
extern void Interface_SwitchMode(uint8_t mode);
extern void Key_SwitchMode_Press(void);
extern void Interface_init(void);


#endif
