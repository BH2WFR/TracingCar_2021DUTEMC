#ifndef __SYSTICK_DELAY_H__
#define __SYSTICK_DELAY_H__
/*
使用SysTick定时器的中断函数：

默认使用MCLK，挂载到DCO上，支持的DCO频率为1.5  3  6  12  24  48MHz


*/

//==============头文件引用区==================
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>  
#include <stdarg.h>
#include <math.h>  
#include <string.h>
#include <setjmp.h> 

//============================================

//================宏定义区====================



//============================================

//=========全局变量声明区（注意是声明不是定义）============


//========================================================

//==================函数声明区========================
extern void SysTickDelay_init(void);
extern void SysTick_delay_us(uint32_t time_us);
extern void SysTick_delay_ms(uint32_t time_ms);


//================================================


#endif
