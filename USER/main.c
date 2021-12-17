
/******************************************************************************
	MSP432P401R LAUNCHPAD专用
	注意：此文件只存放main函数和一些必要的中断服务程序！


	

	
	
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

#include "UART0.h" 
#include "SysTick_delay.h" 
#include "LED.h" 
#include "OLED_12864.h" 
#include "beep.h"
#include "Beep_song.h"
#include "Remocon_Keys.h"
#include "Wireless.h"
#include "interface.h"
#include "Motor.h"
#include "TracingSensor.h"





//======================= 私有宏定义 Private Macros ========================



//================= 公有全局变量 Public Global Variables======================

//================= 私有全局变量 Private Global Variables======================

volatile uint64_t main_circulateCount = 0;	// 主函数 循环计数，一直增加，供其他函数读取，进行 %xxxx 操作后 达到 定时的 效果

bool	main_testSign0 = false;				// 用于其他模块的测试，传递参数
bool	main_testSign1 = false;				// 用于其他模块的测试，传递参数
bool	main_testSign2 = false;				// 用于其他模块的测试，传递参数
bool	main_testSign3 = false;				// 用于其他模块的测试，传递参数
bool	main_testSign4 = false;				// 用于其他模块的测试，传递参数

char   main_versionStr[24] = "Ver:0.3.1 \x03";

//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

void main_init(void)
{
	
	//======系统基础模块初始化代码（请勿改动）====
    WDT_A_holdTimer();									// 看门狗关掉
	
	FPU_enableModule();									// 开启浮点运算单元
	FPU_enableLazyStacking();
	
	PCM_setCoreVoltageLevel(PCM_VCORE1);				// 抬高电压支持高频率
	CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);	// DCO 时钟设置为 24 MHz
	CS_setExternalClockSourceFrequency(32000,48000000);	// 设置外部晶振的频率

	
	//以下几行为将SMCLK时钟挂到HFXT上，频率为48MHz
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
		GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);			
															// GPIO 口复用 PJ2 和 PJ3 为 第一复用功能（HFXT 晶体）
    FlashCtl_setWaitState(FLASH_BANK0, 1);					// 据说操作片内FLASH时需要这样让它运行正常，如果不使用FLASH可删掉
    FlashCtl_setWaitState(FLASH_BANK1, 1);
	
	//SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);
	
    CS_startHFXT(false);													// HFXT起振
	CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);	// SMCLK 挂到 HFXT 上，分频为 4, 12 MHz
	CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);		// MCLK  挂到 HFXT 上，分频为 1，48 MHz
	
	
	//ACLK挂到LFXT上，频率32768Hz
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
		GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
	CS_startLFXT(CS_LFXT_DRIVE3);
	CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);		// ACLK 挂到 LFXT 上，分频为1，3.2kHz
	
}

void user_init(void)
{
	//只能初始化外设模块，不能初始化赛题！
	
	OLED_init();
	
	Key_init();
	
	#ifdef __POTENTIOMETER_H__
		Potentiometer_Init();
	#endif
	
	
	#ifdef __REMOCON_ENCODER
		Encoder_init();
	#endif
	
	#ifdef __TRACING_SENSOR_H__
		Sensor_init(false);
	#endif
	
	#ifdef __BEEP_H__
		Beep_Init(120u);
		
	#endif
	
	#ifdef __WIRELESS_H__
		Wireless_init();
	#endif
	
	#ifndef __MODE_MOTOR_TEST_H__
		Motor_init(false);
		//Motor_Start();
		
	#endif
	
	
}

/**
 * @brief  main函数
 * 			 */
int main(void)	//入口
{
	
	//=====主函数局部变量定义区======
	
	LED_Init();						//开发板板载 LED 初始化代码
	LED2_switch(LED2_COLOUR_BLUE);
	
	//系统初始化
	main_init();					//初始化晶振、时钟、模块等等
	
	//=====用户模块的初始化代码=======
	SysTickDelay_init();			//基于SysTick的延时函数
	UART0_init();					//开启UART0，方便设备与电脑发送实时数据
	
	user_init();	
	
	//开启总中断
	Interrupt_enableMaster();	

	LED2_switch(LED2_COLOUR_BLACK);
	
	//====主函数死循环====
	Interface_init();
	
	Beep_PlayMusic(music_Start);
	
    while(1)
    {
		{
			main_circulateCount++;
			
			LED1_toggle();
			Key_Detect();
			
			
//			#define SHABI	2000
//			if(main_testSign0 == true){
//				if(main_circulateCount % SHABI == 0){
//					sprintf(Wireless_sendBuf, "This is a Wireless test... %08u\r\n", main_circulateCount/SHABI); 
//					Wireless_sendString(Wireless_sendBuf, WIRELESS_DELAY_UNUSE, 0u);
//				}
//			}

			
			Interface_modeConfig.Cycle_func();
			
			
			
		}
	
	
	
	}
	//return 0;
}








//==================== 中断服务函数：==========================

/**
 * @brief  HardFault 中断
 * 			内存等硬件模块发生错误(尤其是程序复杂度太高导致堆栈爆炸) 时 LED2 快速闪红灯 */
void HardFault_Handler(void)
{
#ifdef __OLED_12864_H__
	OLED_ClearAll();
	sprintf(UART0_string, "-> HardFault_ERROR");
	OLED_PrintString(UART0_string, 12, 3, OLED_TYPE_6X8, 0);	
	sprintf(UART0_string, " :(  OOPS! ");
	OLED_PrintString(UART0_string, 6, 1, OLED_TYPE_6X8, 1);	
#endif
	LED1_switch(0);
	Interrupt_disableMaster();
	
	while(1)
	{
		LED1_toggle();
		LED2_toggle(LED2_COLOUR_BLUE);
		SysTick_delay_ms(100);
	}
}




// /**
//  * @brief  MemManage 中断
//  * 		需要使能其中断, 否则默认进HardFault 	 */
// void MemManage_Handler(void)
// {
	
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, "-> MemManage_ERROR");
// 	OLED_Print(12, 36, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// 	sprintf(UART0_string, ":(");
// 	OLED_Print(6, 8, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }

// /**
//  * @brief  BusFault 中断
//  * 		需要使能其中断, 否则默认进HardFault 	 */
// void BusFault_Handler(void)
// {
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, "-> BusFault_ERROR");
// 	OLED_Print(12, 36, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// 	sprintf(UART0_string, ":(");
// 	OLED_Print(6, 8, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }

// /**
//  * @brief  UsageFault 中断
//  * 		需要使能其中断, 否则默认进HardFault 		 */
// void UsageFault_Handler(void)
// {
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, " UsageFault_ERROR ");
// 	OLED_Print(10, 24, UART0_string,OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }



