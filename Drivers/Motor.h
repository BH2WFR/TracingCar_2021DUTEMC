#ifndef __MOTOR_H__
#define __MOTOR_H__

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


#define 	MOTOR_AMOUNT				2	// 电机的数量

//==== 电机的编号、GPIO宏定义、定时器定义
#define		MOTOR_0						0			// 马达 左 的 序号 是 0
#define		MOTOR_0_DRIVE_A_PORT_PIN	GPIO_PORT_P5, GPIO_PIN6	// TA2.1
#define 	MOTOR_0_DRIVE_B_PORT_PIN	GPIO_PORT_P5, GPIO_PIN7	// TA2.2
	//编码器需要
	
#define 	MOTOR_0_ENCODER_A_PORT_PIN	GPIO_PORT_P2, GPIO_PIN6
#define 	MOTOR_0_ENCODER_A_PORT		GPIO_PORT_P2
#define		MOTOR_0_ENCODER_A_PIN		GPIO_PIN6	//GPIO 中断里 使用
#define		MOTOR_0_ENCODER_B_PORT_PIN	GPIO_PORT_P2, GPIO_PIN7
#define		MOTOR_0_ENCODER_B_PIN		GPIO_PIN7



#define 	MOTOR_0_PWM_TIMER_BASE		TIMER_A2_BASE	// PWM 定时器, 要求 输出 A 与 输出 B 要 放到 同一个定时器的不同 CCR 中
#define		MOTOR_0_PWM_A_CCRx			1
#define		MOTOR_0_PWM_B_CCRx			2

//===================================
#define		MOTOR_1						1			// 马达 右 的 序号 是 1
#define		MOTOR_1_DRIVE_A_PORT_PIN	GPIO_PORT_P6, GPIO_PIN6	// TA2.3
#define 	MOTOR_1_DRIVE_B_PORT_PIN	GPIO_PORT_P6, GPIO_PIN7	// TA2.4

#define 	MOTOR_1_ENCODER_A_PORT_PIN	GPIO_PORT_P2, GPIO_PIN3
#define 	MOTOR_1_ENCODER_A_PORT		GPIO_PORT_P2
#define		MOTOR_1_ENCODER_A_PIN		GPIO_PIN3	//GPIO 中断里 使用
#define		MOTOR_1_ENCODER_B_PORT_PIN	GPIO_PORT_P2, GPIO_PIN5
#define		MOTOR_1_ENCODER_B_PIN		GPIO_PIN5
// #define		MOTOR_1_ENCODER_IRQHandler	PORT4_IRQHandler	//中断服务函数名称
// #define		MOTOR_1_ENCODER_INTERRUPT	INT_PORT4
//=================
#define		MOTOR_ENCODER_IRQHandler	PORT2_IRQHandler	//中断服务函数名称，电机 1 和 电机 2 都 适用
#define		MOTOR_ENCODER_INTERRUPT		INT_PORT2
#define		MOTOR_ENCODER_INT_PORTPIN	MOTOR_0_ENCODER_A_PORT,  MOTOR_0_ENCODER_A_PIN | MOTOR_1_ENCODER_A_PIN	
	// 中断函数 使用 启用中断的 IO 口


#define 	MOTOR_1_PWM_TIMER_BASE		MOTOR_0_PWM_TIMER_BASE	// 与 MOTOR_0_PWM_TIMER_BASE 相同
#define		MOTOR_1_PWM_A_CCRx			3
#define		MOTOR_1_PWM_B_CCRx			4






//====== PID 模块 相关配置，。部分配置在 「.c」 中的 私有宏定义中
#define 	MOTOR_PID_TIMER_BASE		TIMER_A1_BASE
#define 	MOTOR_PID_TIMER_INTERRUPT	INT_TA1_0
#define		MOTOR_PID_TIMER_IRQHandler	TA1_0_IRQHandler

#define 	MOTOR_PID_Kp_DEFAULT		1.01f
#define		MOTOR_PID_Ki_DEFAULT		1.10f
#define		MOTOR_PID_Kd_DEFUALT		0.1f

// 	//选择使用 增量式 PID 还是 位移式 PID
// #define		PID_MODE_DISPLACEMENTAL		0
// #define		PID_MODE_INCREMENTAL		1
// #define 	PID_MODE					PID_MODE_INCREMENTAL	//使用增量式

//===== 编码器对应的定时器(timer32）相关配置
#define 	MOTOR_ENCODER_TIMER_BASE		TIMER32_1_BASE		//定时器地址
#define 	MOTOR_ENCODER_TIMER_INTERRUPT	INT_T32_INT2		//中断名称
#define		MOTOR_ENCODER_TIMER_IRQHandler	T32_INT2_IRQHandler	//中断服务函数
	
#define		MOTOR_ENCODER_DURATION_BUFFER_AMOUNT	10
#define 	MOTOR_ENCODER_SPEED_BUFFER_AMOUNT		10

//==== 电机 相关 宏定义配置

	// 运行 Motor_SetForce(#, #, #) 函数时 是从结构体中读取force变量，还是在形参中输入force值后写入结构体？
#define		MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT		false
#define		MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT		true

	// 电机的旋转方向
#define		MOTOR_DIRECTION_FORWARD		true
#define		MOTOR_DIRECTION_BACKWARD	false	//电机运行方向

#define		MOTOR_MAX_FORCE				100		//最大的force值（正负范围），决定占空比

#define 	MOTOR_TIMER_CCR0_LOADVALUE	800		//初始 CCR0（PWM 频率）值
#define 	MOTOR_TIMER_PWM_DEFAULT_CCR	0		//初始 CCR（比较）值


#define		MOTOR_ENCODER_SPEED_QUERING_MAINCYCLE_TIME	// 主循环 循环多少次 查询一次编码器？
#define		MOTOR_ENCODER_0_SPEED_DURING_TIME	90	// 主循环 读取定时器的值，多少次没有发生改变？如果成立，则速度输出为 0
#define		MOTOR_ENCODER_SPEED_BUFFER_SIZE		15
#define		MOTOR_ENCODER_MINIMUN_DURING_TIME	35000u		//编码器 消抖用，防止因为抖动多次触发编码器

#define		MOTOR_ENCODER_SPEED_COEFFICIENT		215000.0f

/**
 * @brief	PID 结构体，嵌入在 Motor_Typedef 中，\b 禁止 单独使用
 * @note	内含：
 * 				PID 参数
 * 				误差
 */
typedef struct{
	


	//==== 以下三个参数 是 增量式PID公式 的 三个系数，禁止自行赋值，需要通过 Motor_PID_Set(PID_Typedef*, Kp, Ki, Kd) 函数 算得。
	float A;	// 当前误差 系数
	float B;	// 上次误差 系数
	float C;	// 上上次误差 系数

		
	float setPoint;			// 设定 目标值
	
	float outputValue;		// PID 计算后输出的计算结果
	float lastError;		// 上次   误差
	float prevError;		// 上上次 误差
	
	bool enableSign; // 如果==0则不使用PID，直接将输入的占空比输出去
	
	struct{
		bool  enable; 	// 积分限幅
		float max;		
		float min;	
	}limiting;

		
}PID_Typedef; 





/**
 * @brief	Motor 结构体
 * @note	内含：
 * 				当前检测到的速度和设置的速度（有正负）
 * 				电机占空比
 * 
 * 				误差
 */
typedef struct{
	
	bool enable;
	bool isStarted;
	
	// float 	setSpeed;		// with SIGN (direction)
	// bool 	setDirection;	// foreward or backward, synchronized with the sign of SPEED
	
	//float currentSpeed;	// READ ONLY
	
	int16_t force;			// [in/out] duty cycle of Motor PID signal, unsigned, while don't use PID, this value output to the motor(duty cycle)
	
	//bool isStarting;	
	
	uint32_t lastTimer32Value;	// 上次的 Timer32 计数值，用于计算间隔
	uint32_t durationTime;		// 两次 中断间 Timer32 的 计数间隔
	
	// uint8_t  durationTimeBufferIndex;
	// uint8_t  durationTimeBufferAmount;	//
	// uint32_t[MOTOR_ENCODER_DURATION_BUFFER_AMOUNT] durationTimeBuffer;
	
	
		
	volatile bool    isInterruptedDuringPIDperiod;// set in interrupt handler and clear in PID funcion, for detecting if the speed refreshed 
	uint8_t dasabi;
	
	//bool isInterrupting;
	
	//==== 编码器相关
	float detectedSpeed;
	bool  detectedDirection;
	/*struct{
		bool  enable;
		float speed;
		bool  direction;
	}encoder;*/
	
	//==== 记步数
	volatile bool    isInSteppingMode;	// 是否属于定距离模式
	bool 	stepModeDir;		// 定距离模式 方向
	int64_t stepCount;			// 步数计数，始终运行
	int64_t stepTarget;			// 定距离模式 目标步数（绝对座标），仅 isInSteppingMode == 1 下 有效
	int16_t steppingModeSpeed;	// 定距离模式使用的速度
	int16_t speedBeforeStepMode;// 记步模式 触发前 的 速度
	// uint8_t  detectedSpeedBufferIndex;
	// uint8_t  detectedSpeedBufferAmount;	//
	// uint32_t[MOTOR_ENCODER_SPEED_BUFFER_AMOUNT] detectedSpeedBuffer;	
	
	
	
	//嵌入 PID、encoder 结构体
	PID_Typedef 		PID;	// PID structure, embendded here
	
	
}Motor_Typedef;  // while declare, please ASSIGN ={0} to initialize the structure to all 0 !!!!!


//============== 公有全局变量声明 Public Global Variables Defines ===============

extern Motor_Typedef Motor[MOTOR_AMOUNT];
extern uint64_t PID_count;

//================== 公有函数声明 Public Function Defines =======================

extern void PID_Clear(volatile PID_Typedef* p_PID_struct);
extern float PID_Calc(volatile PID_Typedef* p_PID_struct, float currentValue);

extern void Motor_init(bool isStartMotor);
extern void Motor_Start(void);
#define Motor_Halt() Motor_Stop()
extern void Motor_Stop(void);
extern void Motor_Disable(void);
extern void Motor_ClearPID(void);


int16_t Motor_SetForce(uint8_t, bool, int16_t);

extern void PID_SetPoint(PID_Typedef* p_PID_struct, float point);
extern void PID_Set(PID_Typedef* p_PID_struct, float Kp, float Ki, float Kd, bool inverse, bool is_reset);

extern void Motor_GoStep(uint8_t motorSelection, int16_t speed, int64_t step);

#endif
