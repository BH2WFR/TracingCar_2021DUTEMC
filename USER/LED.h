#ifndef __LED_H__
#define __LED_H__
/******************************************************************************
 �ٷ�������LEDģ��
 LED1����ࡪ��Ϊ��ɫ��ɫLED
		P1.0
 LED2���ҲࣩΪһ��RGBģ��	
		
		
��ʱ����LED2:
		��ƿ�����˸��	�������/Ӳ������
		��ɫ��			��ʱ����Դ�ظ�ʹ��
		��ɫ��			���ڳ�ʼ��
			
 *******************************************************************************/
 
 
 
//================== ����ͷ�ļ�  Header Files Includes =======================
	//��Ƭ���⺯��ͷ�ļ�
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

	//c��׼ͷ�ļ�(ANSI C)
#include <stdio.h>  	//��������ַ�����sprintf������
#include <stdint.h>		//������ uint8_t �� һϵ�з���ĺ궨��
#include <stdlib.h>		//���� malloc(@) free(@) rand()��
#include <math.h>  		//��ѧ��
#include <string.h>		//�ַ���(char����)����, �ڴ���д����(memcpy($)��)
//#include <ctype.h>		//�ַ�����
//#include <setjmp.h>		//��ת����
//#include <stdarg.h>		//��κ���ʹ��
//#include <time.h>			//
//#include <errno.h>		//
//#include <stddef.h>		//
//#include <limits.h>		//�����и��ֱ��������ֵ/��Сֵ����
//#include <float.h>		//
//#include <locale.h>		//���ػ�֧��

	//c��׼ͷ�ļ�(C99)
#include <stdbool.h>		//����֧��bool(����)����
//#include <complex.h>		//��������
//#include <tgmath.h>		//�����ж��������͵���ѧ����

	//c��׼ͷ�ļ�(C11)
//#include <uchar.h> 		//Unicode֧��

	//�û�ģ��ͷ�ļ�
#include "main.h"		//�������


//====================== ���к궨�� Public Macros ===============================
#define LED2_COLOUR_BLACK	0
#define LED2_COLOUR_RED		1
#define LED2_COLOUR_GREEN	2
#define LED2_COLOUR_BLUE	3
#define LED2_COLOUR_CYAN	4
#define LED2_COLOUR_MAGENTA	5
#define LED2_COLOUR_YELLOW	6
#define LED2_COLOUR_WHITE	7
#define	LED2_RETVAL_ERROR	8

#define LED2_COLOUR_MAX		7
#define LED2_COLOUR_MIN		0


#define LED_ON	1
#define LED_OFF	0

#define LED1	GPIO_PORT_P1, GPIO_PIN0
#define LED_R	GPIO_PORT_P2, GPIO_PIN0
#define LED_G	GPIO_PORT_P2, GPIO_PIN1
#define LED_B	GPIO_PORT_P2, GPIO_PIN2

#define LED2_ROLLUP		0
#define LED2_ROLLDOWN	1


//============== ����ȫ�ֱ������� Public Global Variables Defines ===============
extern uint8_t LED2_colour;





//================== ���к������� Public Function Defines =======================
extern void LED_Init(void);
extern bool LED1_switch(bool status);
extern bool LED1_toggle(void);
extern uint8_t LED2_switch(uint8_t colour);
extern uint8_t LED2_roll(bool direction);
extern uint8_t LED2_toggle(uint8_t colour);

#endif
