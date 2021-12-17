#ifndef __TRACING_SENSOR_H__
#define __TRACING_SENSOR_H__

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
#include "main.h"		// �������
#include "Motor.h" 		// ���� PID ������

//====================== ���к궨�� Public Macros ===============================

#define SENSOR_AMOUNT				8u

#define SENSOR_GPIO_SHARED_PORT 	GPIO_PORT_P4

#define SENSOR_USE_TIMER		0
#define SENSOR_USE_PID			1

#define SENSOR_PID_KP		-1.00000f
#define SENSOR_PID_KI		-0.00000f
#define SENSOR_PID_KD		-0.50000f

#define SENSOR_COLOUR_INVERSE	1		// Sensor.colourInverse,	 == true ʱ Ϊ �ڵװ��ߣ�== false ʱ Ϊ �׵׺���

#define SENSOR_ENABLE_0_AND_7	0		// Sensor.enable_sensor_0_and_7, �Ƿ�ʹ�ܱ������������ 0 �� 7

#define TRACKING_BASIC_SPEED	50.0f		// Sensor.trackingLine.basicSpeed ѭ�� ����ֱ���ٶ�
#define TRACKING_SPEED_COORD	0.020f	// Sensor.trackingLine.speedCoord ѭ�� ת��ϵ������ ������λ�ú� ������ٵĹ�ϵ
#define TRACKING_DIR_INVERSE	1			// 	�Ƿ�ת����

#define SENSOR_CROSS_DETECT_ENABLE			false	// ʮ��·�ڼ�� ʹ��
#define SENSOR_CROSS_SHELTERED_THRESHOLD	4u	// ʮ��·�� �������ڵ����� ��ֵ
#define SENSOR_CROSS_ACTION_CYCLE			60u

#define SENSOR_90_DEG_DETECT_ENABLE			false	// ֱ������ ʹ��

typedef struct{
	
	bool 	enabled;				// ʹ���ź�
	bool 	enableTracing;
	
	bool 	colourInverse;			// == true ʱ Ϊ �׵׺��ߣ�==false ʱ Ϊ �ڵװ���
	
	bool	enable_sensor_0_and_7;	// �Ƿ�ʹ�� ������� 0 �� 7 ������������������в��ŵ������߼��С�� 15cm, �ر�
	
	bool 	isReading_flag;			// ����ͨ���жϺ����ĵ��� ��ȡ������״̬, 
	
	//uint8_t status_bit;						// ������״̬����λ��ʾ
	bool	status_array[SENSOR_AMOUNT];	// ������״̬���ò��������ʾ��1 ��ʾ���ڵ�
	
	
	uint8_t shelteredSensorsAmount;	// ���ڵ��� �Ҷ�/���� ����������
	
	float 	averagedLineLocation;	// ȡƽ�����⵽�� �� �� λ��, ��Χ�� +40 ~ -40
	
	struct{
		struct{
			//bool 	enable;		//�μ� Sensors.PID.enableSign
			float 	basicSpeed;
			float 	speedCoord;
			bool	inverseDir;		// Ѱ������==1 ʱ �෴
			void 	(*Action)(float);
		}trackingLine;
		
		struct{
			bool 	enable;
			bool 	isDetected_flag;
			bool 	isDoingCrossAction;
			uint32_t length;
			void 	(*Action)(bool);	// 
		}crossRecognition;
		
		struct{
			bool enable;
			
			bool isDetectedLeft90deg;
			bool isDoingTurnLeft90Action;
			
			bool isDetectedRight90deg;
			bool isDoingTurnRight90Action;
			
			void (*ActionLeft)(bool);
			void (*ActionRight)(bool);
			
		}_90degRecognition;

	}features;
	
	PID_Typedef PID;			// PID ���㣬���� motor.h 
	
}Sensors_typedef;




//============== ����ȫ�ֱ������� Public Global Variables Defines ===============
extern Sensors_typedef Sensor;



//================== ���к������� Public Function Defines =======================
extern void Sensor_init(bool);
extern void Sensor_Disable(void);
extern void Sensor_StartTracing(void);
extern void Sensor_StopTracing(void);

extern void Sensor_ReadStatus(void);

extern void Sensor_GetStatusString(char* p_str);

#endif


