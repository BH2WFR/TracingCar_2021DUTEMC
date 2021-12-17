#ifndef __SYSTICK_DELAY_H__
#define __SYSTICK_DELAY_H__
/*
ʹ��SysTick��ʱ�����жϺ�����

Ĭ��ʹ��MCLK�����ص�DCO�ϣ�֧�ֵ�DCOƵ��Ϊ1.5  3  6  12  24  48MHz


*/

//==============ͷ�ļ�������==================
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

//================�궨����====================



//============================================

//=========ȫ�ֱ�����������ע�����������Ƕ��壩============


//========================================================

//==================����������========================
extern void SysTickDelay_init(void);
extern void SysTick_delay_us(uint32_t time_us);
extern void SysTick_delay_ms(uint32_t time_ms);


//================================================


#endif
