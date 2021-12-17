#ifndef __ONBOARD_KEY_H__
#define __ONBOARD_KEY_H__
/******************************************************************************
板上按键：
	S1（左）: P1.1		S2（右）: P1.4
	使用TimerA3
	查询方法：
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




#define ONBOARD_KEY1_PORT		GPIO_PORT_P1
#define ONBOARD_KEY2_PORT		GPIO_PORT_P1
#define ONBOARD_KEY1_PIN		GPIO_PIN1
#define ONBOARD_KEY2_PIN		GPIO_PIN4
#define ONBOARD_KEY1_PORTPIN	GPIO_PORT_P1, GPIO_PIN1
#define ONBOARD_KEY2_PORTPIN	GPIO_PORT_P1, GPIO_PIN4




extern uint32_t key1_buf;
extern uint32_t key2_buf;
extern bool OnBoard_Key1_pressedFlag;
extern bool OnBoard_Key2_pressedFlag;


__weak void OnBoard_Key1_Press(void);
__weak void OnBoard_Key1_KeyUp(void);
__weak void OnBoard_Key2_Press(void);
__weak void OnBoard_Key2_KeyUp(void);
extern void OnBoard_Key_Detect(void);
extern void OnBoard_Key_init(void);


#endif
