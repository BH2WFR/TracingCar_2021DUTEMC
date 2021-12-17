#ifndef __UART0_H__
#define __UART0_H__
/******************************************************************************
 *UART调试模块
 
 *波特率：9200			校验位：N			停止位：1
 *  通过UARt0发送字符串到电脑，使用串口调试助手进行调试
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----> PC (echo)
 *            |                 |
 *            |                 |
 *            |     P1.2/UCA0RXD|<---- PC
 *            |                 |
 
 
 
		请注意：该代码会把SMCLK挂到HFXT上，频率为48MHz
 *
 *******************************************************************************/
 
 
 
//单片机独有头文件
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//c标准头文件(ANSI C)
#include <stdio.h>  	//用于输出字符串的sprintf函数
#include <stdint.h>		
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>  
#include <string.h>
#include <setjmp.h>
//c标准头文件(C99)
#include <stdbool.h>

//用户头文件
#include "main.h"
//================宏定义区====================

#define UART0_printf(pointer, ...)	 sprintf(pointer, __VA_ARGS__);UART0_sendString(pointer);

//=========全局变量声明区（注意是声明不是定义）============
//static char* p_UART0_str;
//static eUSCI_UART_ConfigV1 uartConfig;
////static struct UART0_baudrate_registers;
//static bool is_UART0_initialized;
//以上两行只是变量的声明，不是变量的定义！变量的定义在同名.c文件中


extern char UART0_string[];


//==================函数声明区========================
//extern struct UART0_baudrate_registers* UART0_getBaudrateResgisters(uint32_t baudrate);
extern void UART0_readString(char* str, int length);
extern void UART0_sendString(char* str);
extern void UART0_init(void);
//按照正点原子STM32例程的习惯，中断服务函数不声明


#endif
