#ifndef __UART0_H__
#define __UART0_H__
/******************************************************************************
 *UART����ģ��
 
 *�����ʣ�9200			У��λ��N			ֹͣλ��1
 *  ͨ��UARt0�����ַ��������ԣ�ʹ�ô��ڵ������ֽ��е���
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
 
 
 
		��ע�⣺�ô�����SMCLK�ҵ�HFXT�ϣ�Ƶ��Ϊ48MHz
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

//�û�ͷ�ļ�
#include "main.h"
//================�궨����====================

#define UART0_printf(pointer, ...)	 sprintf(pointer, __VA_ARGS__);UART0_sendString(pointer);

//=========ȫ�ֱ�����������ע�����������Ƕ��壩============
//static char* p_UART0_str;
//static eUSCI_UART_ConfigV1 uartConfig;
////static struct UART0_baudrate_registers;
//static bool is_UART0_initialized;
//��������ֻ�Ǳ��������������Ǳ����Ķ��壡�����Ķ�����ͬ��.c�ļ���


extern char UART0_string[];


//==================����������========================
//extern struct UART0_baudrate_registers* UART0_getBaudrateResgisters(uint32_t baudrate);
extern void UART0_readString(char* str, int length);
extern void UART0_sendString(char* str);
extern void UART0_init(void);
//��������ԭ��STM32���̵�ϰ�ߣ��жϷ�����������


#endif
