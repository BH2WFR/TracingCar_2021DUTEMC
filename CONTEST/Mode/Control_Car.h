#ifndef __CONTROL_CAR_H__
#define __CONTROL_CAR_H__

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


/**
 * ����֡˵����
 * 	֡����CONTROL_CAR_DATA_FRAME_AMOUNT -> 3u
 * ��һ֡��CONTROL_CAR_DATA_FIRST_FRAME -> 0x7F(127)
 * �ڶ�֡��0x__		:  ��һ�����ݣ�int8_t, -100 ~ +100
 * ����֡��0x__		:  �ڶ������ݣ�int8_t, -100 ~ +100
*/
#define CONTROL_CAR_DATA_FRAME_AMOUNT	3u

#define CONTROL_CAR_DATA_FIRST_FRAME	0x7F	// 127
//#define CONTROL_CAR_DATA_LART_FRAME		0xAA


#define CONTROL_CAR_NO_SIGNAL_TIMER_COUNT_THRESHOLD		1000u


typedef struct{
	bool enable;
	//bool 
	struct{
		bool   sign;
		
		uint16_t timerInterruptCount;
		uint16_t thresholdCount;
	}noSignal;
	
	struct{
		//bool  sign;		// ��ȡ�� ����
		bool  error;
		
		uint8_t command;
		
		int8_t firstParameter;
		int8_t secondParameter;
		
		uint8_t frameCount;		// �ӵ��˵ڼ�֡�� ���� 0����ͷ֡�� ��ʼ ������
		
	}received;
	
}ControlCar_typedef;

//============== ����ȫ�ֱ������� Public Global Variables Defines ===============

//================== ���к������� Public Function Defines =======================
extern void ControlCar_ResetStruct(void);

extern void ControlCar_init(void);
extern void ControlCar_Cycle(void);
extern void ControlCar_Halt(void);

//extern 
extern void ControlCar_SetSpeedAndDir(float speed, float dir);
extern void ControlCar_SetWheelSpeed(float LeftSpeed, float RightSpeed);
extern void ControlCar_AnalysisReceivedDataFrame(uint8_t RxBuf);
extern void ControlCar_TimerInterrupt(uint32_t timerCount);


extern void ControlCar_Key_EnableMotor(void);
extern void ControlCar_StartMotor(void);
extern void ControlCar_StopMotor(void);


#endif
