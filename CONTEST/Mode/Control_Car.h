#ifndef __CONTROL_CAR_H__
#define __CONTROL_CAR_H__

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


//====================== 公有宏定义 Public Macros ===============================


/**
 * 数据帧说明：
 * 	帧数：CONTROL_CAR_DATA_FRAME_AMOUNT -> 3u
 * 第一帧：CONTROL_CAR_DATA_FIRST_FRAME -> 0x7F(127)
 * 第二帧：0x__		:  第一个数据，int8_t, -100 ~ +100
 * 第三帧：0x__		:  第二个数据，int8_t, -100 ~ +100
*/
#define CONTROL_CAR_DATA_FRAME_AMOUNT	3u

#define CONTROL_CAR_DATA_FIRST_FRAME	0x7F	// 127
//#define CONTROL_CAR_DATA_LART_FRAME		0xAA


#define CONTROL_CAR_NO_SIGNAL_TIMER_COUNT_THRESHOLD		1000u


typedef struct{
	bool enable;
	//bool 
	struct{
		bool   sign;
		
		uint16_t timerInterruptCount;
		uint16_t thresholdCount;
	}noSignal;
	
	struct{
		//bool  sign;		// 读取后 清零
		bool  error;
		
		uint8_t command;
		
		int8_t firstParameter;
		int8_t secondParameter;
		
		uint8_t frameCount;		// 接到了第几帧？ （从 0（开头帧） 开始 计数）
		
	}received;
	
}ControlCar_typedef;

//============== 公有全局变量声明 Public Global Variables Defines ===============

//================== 公有函数声明 Public Function Defines =======================
extern void ControlCar_ResetStruct(void);

extern void ControlCar_init(void);
extern void ControlCar_Cycle(void);
extern void ControlCar_Halt(void);

//extern 
extern void ControlCar_SetSpeedAndDir(float speed, float dir);
extern void ControlCar_SetWheelSpeed(float LeftSpeed, float RightSpeed);
extern void ControlCar_AnalysisReceivedDataFrame(uint8_t RxBuf);
extern void ControlCar_TimerInterrupt(uint32_t timerCount);


extern void ControlCar_Key_EnableMotor(void);
extern void ControlCar_StartMotor(void);
extern void ControlCar_StopMotor(void);


#endif
