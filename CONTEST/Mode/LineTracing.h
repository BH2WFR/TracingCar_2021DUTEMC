#ifndef __MODE_LINETRACING_H__
#define __MODE_LINETRACING_H__

/******************************************************************************
 * 	@brief	
 * 
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

#define LINETRACING_RUSH_PID_COUNT	100u

//============== ����ȫ�ֱ������� Public Global Variables Defines ===============
extern bool isTracking;




//================== ���к������� Public Function Defines =======================
extern void LineTracing_Key3_StartTracking(void);
extern void LideTracing_Key4_StartRush(void);

extern void LineTracing_init(void);
extern void LineTracing_Cycle(void);
extern void LineTracing_Halt(void);




#endif
