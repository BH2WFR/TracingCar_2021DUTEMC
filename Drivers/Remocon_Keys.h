#ifndef __REMOCON_KEYS_H__
#define __REMOCON_KEYS_H__
//#define __REMOCON_ENCODER
/*====警告: 本文件不能与  OnBoard_Key.h/.c  共存!========*/

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
#include "OLED_12864.h"
#include "Wireless.h"
#include "beep.h"
#include "Beep_song.h"
#include "Interface.h"

//====================== 公有宏定义 Public Macros ===============================

	//按钮缓冲数组判断按钮按下(消抖)时使用的 mask
#define KEY_DETECT_MASK				0x000000FF
#define KEY_DETECT_DOWN				0x000000FF
#define KEY_DETECT_UP				0x00000000

	//消抖缓存定时器相关懒人包
#define KEY_DETECT_TIMER_BASE			TIMER_A3_BASE
#define KEY_DETECT_TIMER_INDEX			3
#define KEY_DETECT_TIMER_INTERRUPT		INT_TA3_0
#define KEY_DETECT_TIMER_IRQHANDLER		

	//按键检测模式 推荐从主循环查询
#define KEY_DETECT_IN_MAIN_LOOP				0		//在 main循环 里面手动判断, 进入对应的事件函数
#define KEY_DETECT_IN_INTERRUPT_HANDLER		1		//在 定时器中断函数 里面自动判断, 进入对应的事件函数
#define KEY_DETECT_MODE						KEY_DETECT_IN_MAIN_LOOP	//推荐从主循环查询


/*	========================按键GPIO说明==================
 	 	 │    Debugger   │
    Key0┌┤  ───────────  ├┐Key2
    P1.1└┤ ┌─┐       ┌─┐ ├┘P1.4
         │ │ │ ┌───┐ │ │ │
         │ │ │ │MCU│ │ │ │
         │ │ │ └───┘ │ │ │			
         │ └─┘       └─┘ │
         └───────────────┘
	特殊按键功能说明:  
*/
#define KEY_AMOUNT		7	// key0 - key6
	//如需更改GPIO端口,请同时修改下面的全部宏定义 与 .c文件 里面的 Key_GPIOport 和 Key_GPIOpin 数组的初始化内容
// #define KEY_1_2_PORT			GPIO_PORT_P1
// #define KEY_0_PORT				GPIO_PORT_P3
// #define KEY_3_4_5_6_PORT		GPIO_PORT_P5
	//KEY0	显示器刷新键
#define KEY0_PORT	GPIO_PORT_P3
#define KEY0_PIN	GPIO_PIN0
	//KEY1	板上按钮	Key1
#define KEY1_PORT	GPIO_PORT_P1
#define KEY1_PIN	GPIO_PIN1
	//KEY2	板上按钮	Key2
#define KEY2_PORT	GPIO_PORT_P1
#define KEY2_PIN	GPIO_PIN4
	//KEY3	左侧位置开关
#define KEY3_PORT	GPIO_PORT_P5
#define KEY3_PIN	GPIO_PIN0
	//KEY4	右侧位置开关
#define KEY4_PORT	GPIO_PORT_P5
#define KEY4_PIN	GPIO_PIN1
	//KEY5	左侧
#define KEY5_PORT	GPIO_PORT_P5
#define KEY5_PIN	GPIO_PIN2
	//KEY6	右侧
#define KEY6_PORT	GPIO_PORT_P5
#define KEY6_PIN	GPIO_PIN4


#define SWITCHMODE_KEY_INDEX	5	// 切换模式是哪个键？
#define OLED_RESTART_KEY_INDEX	0	// 重启OLED是哪个键？

#define KEY_EVENT_AMOUNT	2u
#define KEY_EVENT_PRESS		0u
#define KEY_EVENT_UP		1u
//#define KEY_EVENT_LONG_PRESS	2u 
//#define KEY_EVENT_DOUBLE_PRESS	3u







//============== 公有全局变量声明 Public Global Variables Defines ===============
extern void (*Key_eventFunctionVector[KEY_EVENT_AMOUNT][KEY_AMOUNT])(void);

extern uint8_t Key_lastKey;
extern uint8_t Key_lastEvent;

extern volatile bool	   Key_detectFlag[KEY_AMOUNT];

extern uint8_t Key_lastKeyEvent_event;			// 当前/曾经 执行的按钮的 事件类型
extern uint8_t Key_lastKeyEvent_index;			// 当前/曾经 执行的按钮的 编号
//extern bool    Encoder_isRunningEventFunction;	// 是否正在执行 按钮事件函数

//================== 公有函数声明 Public Function Defines =======================

extern void Key_EventFunctionVector_ResetAll(void);
extern void Key_EventFunctionVector_Reset(uint8_t event, uint8_t index);
extern void Key_Detect(void);
extern void Key_init(void);



#endif

