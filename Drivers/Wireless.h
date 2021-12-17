#ifndef __WIRELESS_H__
#define __WIRELESS_H__

/******************************************************************************
 *	GT-38 无线串口模块
 * 接口: 
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
#include "LED.h"

//===================== 公有宏定义 Public Macros ==============================

#define WIRELESS_TX_GPIO_PORT	GPIO_PORT_P3	//这个是 WIRELESS 的 RX, 对应单片机的 TX
#define WIRELESS_TX_GPIO_PIN	GPIO_PIN3	//单片机 UCA2 TXD

#define WIRELESS_RX_GPIO_PORT	GPIO_PORT_P3	//这个是 WIRELESS 的 TX, 对应单片机的 RX
#define WIRELESS_RX_GPIO_PIN	GPIO_PIN2	//单片机 UCA2 RXD

#define WIRELESS_UART_BASE			EUSCI_A2_BASE	
#define WIRELESS_UART_RX_INTERRUPT	EUSCI_A_UART_RECEIVE_INTERRUPT
#define WIRELESS_UART_TX_INTERRUPT	EUSCI_A_UART_TRANSNMIT_INTERRUPT
#define WIRELESS_UART_INTERRUPT		INT_EUSCIA2

	//使用延时还是中断?
#define WIRELESS_DELAY_USE		1
#define WIRELESS_DELAY_UNUSE	0

	//WIRELESS_sendingMode, 代表当前使用的发送模式, 是发送到'\0'还是发送到指定的长度
#define WIRELESS_SENDING_TO_T	1
#define WIRELESS_SENDING_TO_0	0


//============配置IO口，对应正点原子无线串口的 MDO，上拉有效

#define IS_USE_SETTING_PORT		0

#if(IS_USE_SETTING_PORT == 1)		// 接到了 P6.1
	#define WIRELESS_SET_PORT	GPIO_PORT_P6
	#define WIRELESS_SET_PIN	GPIO_PIN1
	
	typedef struct{
		bool isInSettingMode;
		
		struct {
			uint32_t 	speed;
			uint8_t 	parity;
			uint8_t 	stopBits;
			
		}UART;
		
		uint8_t channel;
		uint8_t power;
		uint8_t airSpeed;
		
		
	}Wireless_Setting_typedef
	
#endif

//============= 公有全局变量定义 Public Global Variables Defines ==============
extern bool     Wireless_isSending;
extern uint8_t  Wireless_sendBuf[];
//extern uint8_t Wireless_receivedBuf[128];

#if(IS_USE_SETTING_PORT == 1)

#endif


//================= 公有函数定义 Public Function Defines ======================

extern void Wireless_init(void);
extern void Wireless_TurnOff(void);
extern void Wireless_sendString(uint8_t *string, bool delayMode, uint8_t length);


#endif

