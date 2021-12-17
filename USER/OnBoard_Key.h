#ifndef __ONBOARD_KEY_H__
#define __ONBOARD_KEY_H__
/******************************************************************************
���ϰ�����
	S1����: P1.1		S2���ң�: P1.4
	ʹ��TimerA3
	��ѯ������
 *
 *******************************************************************************/
//��Ƭ������ͷ�ļ�
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
//c��׼ͷ�ļ�(ANSI C)
#include <stdio.h>  	//��������ַ�����sprintf����
#include <stdint.h>		
#include <stdlib.h>

#include <stdarg.h>
#include <math.h>  
#include <string.h>
#include <setjmp.h>
//c��׼ͷ�ļ�(C99)
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
