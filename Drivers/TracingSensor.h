#ifndef __TRACING_SENSOR_H__
#define __TRACING_SENSOR_H__

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
#include "main.h"		// 必须包含
#include "Motor.h" 		// 借用 PID 运算用

//====================== 公有宏定义 Public Macros ===============================

#define SENSOR_AMOUNT				8u

#define SENSOR_GPIO_SHARED_PORT 	GPIO_PORT_P4

#define SENSOR_USE_TIMER		0
#define SENSOR_USE_PID			1

#define SENSOR_PID_KP		-1.00000f
#define SENSOR_PID_KI		-0.00000f
#define SENSOR_PID_KD		-0.50000f

#define SENSOR_COLOUR_INVERSE	1		// Sensor.colourInverse,	 == true 时 为 黑底白线，== false 时 为 白底黑线

#define SENSOR_ENABLE_0_AND_7	0		// Sensor.enable_sensor_0_and_7, 是否使能编码器最两侧的 0 和 7

#define TRACKING_BASIC_SPEED	50.0f		// Sensor.trackingLine.basicSpeed 循迹 基本直线速度
#define TRACKING_SPEED_COORD	0.020f	// Sensor.trackingLine.speedCoord 循迹 转向系数，即 传感器位置和 电机差速的关系
#define TRACKING_DIR_INVERSE	1			// 	是否翻转方向

#define SENSOR_CROSS_DETECT_ENABLE			false	// 十字路口检测 使能
#define SENSOR_CROSS_SHELTERED_THRESHOLD	4u	// 十字路口 传感器遮挡个数 阈值
#define SENSOR_CROSS_ACTION_CYCLE			60u

#define SENSOR_90_DEG_DETECT_ENABLE			false	// 直角弯检测 使能

typedef struct{
	
	bool 	enabled;				// 使能信号
	bool 	enableTracing;
	
	bool 	colourInverse;			// == true 时 为 白底黑线，==false 时 为 黑底白线
	
	bool	enable_sensor_0_and_7;	// 是否使能 最两侧的 0 和 7 编码器，如果赛道上有并排的两根线间距小于 15cm, 关闭
	
	bool 	isReading_flag;			// 正在通过中断函数的调用 读取编码器状态, 
	
	//uint8_t status_bit;						// 读到的状态，用位表示
	bool	status_array[SENSOR_AMOUNT];	// 读到的状态，用布尔数组表示，1 表示被遮挡
	
	
	uint8_t shelteredSensorsAmount;	// 被遮挡的 灰度/红外 传感器数量
	
	float 	averagedLineLocation;	// 取平均后检测到的 线 的 位置, 范围： +40 ~ -40
	
	struct{
		struct{
			//bool 	enable;		//参见 Sensors.PID.enableSign
			float 	basicSpeed;
			float 	speedCoord;
			bool	inverseDir;		// 寻迹方向：==1 时 相反
			void 	(*Action)(float);
		}trackingLine;
		
		struct{
			bool 	enable;
			bool 	isDetected_flag;
			bool 	isDoingCrossAction;
			uint32_t length;
			void 	(*Action)(bool);	// 
		}crossRecognition;
		
		struct{
			bool enable;
			
			bool isDetectedLeft90deg;
			bool isDoingTurnLeft90Action;
			
			bool isDetectedRight90deg;
			bool isDoingTurnRight90Action;
			
			void (*ActionLeft)(bool);
			void (*ActionRight)(bool);
			
		}_90degRecognition;

	}features;
	
	PID_Typedef PID;			// PID 运算，借用 motor.h 
	
}Sensors_typedef;




//============== 公有全局变量声明 Public Global Variables Defines ===============
extern Sensors_typedef Sensor;



//================== 公有函数声明 Public Function Defines =======================
extern void Sensor_init(bool);
extern void Sensor_Disable(void);
extern void Sensor_StartTracing(void);
extern void Sensor_StopTracing(void);

extern void Sensor_ReadStatus(void);

extern void Sensor_GetStatusString(char* p_str);

#endif


