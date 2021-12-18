#ifndef __MOTOR_H__
#define __MOTOR_H__

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


#define 	MOTOR_AMOUNT				2	// ���������

//==== ����ı�š�GPIO�궨�塢��ʱ������
#define		MOTOR_0						0			// ��� �� �� ��� �� 0
#define		MOTOR_0_DRIVE_A_PORT_PIN	GPIO_PORT_P5, GPIO_PIN6	// TA2.1
#define 	MOTOR_0_DRIVE_B_PORT_PIN	GPIO_PORT_P5, GPIO_PIN7	// TA2.2
	//��������Ҫ
	
#define 	MOTOR_0_ENCODER_A_PORT_PIN	GPIO_PORT_P2, GPIO_PIN6
#define 	MOTOR_0_ENCODER_A_PORT		GPIO_PORT_P2
#define		MOTOR_0_ENCODER_A_PIN		GPIO_PIN6	//GPIO �ж��� ʹ��
#define		MOTOR_0_ENCODER_B_PORT_PIN	GPIO_PORT_P2, GPIO_PIN7
#define		MOTOR_0_ENCODER_B_PIN		GPIO_PIN7



#define 	MOTOR_0_PWM_TIMER_BASE		TIMER_A2_BASE	// PWM ��ʱ��, Ҫ�� ��� A �� ��� B Ҫ �ŵ� ͬһ����ʱ���Ĳ�ͬ CCR ��
#define		MOTOR_0_PWM_A_CCRx			1
#define		MOTOR_0_PWM_B_CCRx			2

//===================================
#define		MOTOR_1						1			// ��� �� �� ��� �� 1
#define		MOTOR_1_DRIVE_A_PORT_PIN	GPIO_PORT_P6, GPIO_PIN6	// TA2.3
#define 	MOTOR_1_DRIVE_B_PORT_PIN	GPIO_PORT_P6, GPIO_PIN7	// TA2.4

#define 	MOTOR_1_ENCODER_A_PORT_PIN	GPIO_PORT_P2, GPIO_PIN3
#define 	MOTOR_1_ENCODER_A_PORT		GPIO_PORT_P2
#define		MOTOR_1_ENCODER_A_PIN		GPIO_PIN3	//GPIO �ж��� ʹ��
#define		MOTOR_1_ENCODER_B_PORT_PIN	GPIO_PORT_P2, GPIO_PIN5
#define		MOTOR_1_ENCODER_B_PIN		GPIO_PIN5
// #define		MOTOR_1_ENCODER_IRQHandler	PORT4_IRQHandler	//�жϷ���������
// #define		MOTOR_1_ENCODER_INTERRUPT	INT_PORT4
//=================
#define		MOTOR_ENCODER_IRQHandler	PORT2_IRQHandler	//�жϷ��������ƣ���� 1 �� ��� 2 �� ����
#define		MOTOR_ENCODER_INTERRUPT		INT_PORT2
#define		MOTOR_ENCODER_INT_PORTPIN	MOTOR_0_ENCODER_A_PORT,  MOTOR_0_ENCODER_A_PIN | MOTOR_1_ENCODER_A_PIN	
	// �жϺ��� ʹ�� �����жϵ� IO ��


#define 	MOTOR_1_PWM_TIMER_BASE		MOTOR_0_PWM_TIMER_BASE	// �� MOTOR_0_PWM_TIMER_BASE ��ͬ
#define		MOTOR_1_PWM_A_CCRx			3
#define		MOTOR_1_PWM_B_CCRx			4






//====== PID ģ�� ������ã������������� ��.c�� �е� ˽�к궨����
#define 	MOTOR_PID_TIMER_BASE		TIMER_A1_BASE
#define 	MOTOR_PID_TIMER_INTERRUPT	INT_TA1_0
#define		MOTOR_PID_TIMER_IRQHandler	TA1_0_IRQHandler

#define 	MOTOR_PID_Kp_DEFAULT		1.01f
#define		MOTOR_PID_Ki_DEFAULT		1.10f
#define		MOTOR_PID_Kd_DEFUALT		0.1f

// 	//ѡ��ʹ�� ����ʽ PID ���� λ��ʽ PID
// #define		PID_MODE_DISPLACEMENTAL		0
// #define		PID_MODE_INCREMENTAL		1
// #define 	PID_MODE					PID_MODE_INCREMENTAL	//ʹ������ʽ

//===== ��������Ӧ�Ķ�ʱ��(timer32���������
#define 	MOTOR_ENCODER_TIMER_BASE		TIMER32_1_BASE		//��ʱ����ַ
#define 	MOTOR_ENCODER_TIMER_INTERRUPT	INT_T32_INT2		//�ж�����
#define		MOTOR_ENCODER_TIMER_IRQHandler	T32_INT2_IRQHandler	//�жϷ�����
	
#define		MOTOR_ENCODER_DURATION_BUFFER_AMOUNT	10
#define 	MOTOR_ENCODER_SPEED_BUFFER_AMOUNT		10

//==== ��� ��� �궨������

	// ���� Motor_SetForce(#, #, #) ����ʱ �Ǵӽṹ���ж�ȡforce�������������β�������forceֵ��д��ṹ�壿
#define		MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT		false
#define		MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT		true

	// �������ת����
#define		MOTOR_DIRECTION_FORWARD		true
#define		MOTOR_DIRECTION_BACKWARD	false	//������з���

#define		MOTOR_MAX_FORCE				100		//����forceֵ��������Χ��������ռ�ձ�

#define 	MOTOR_TIMER_CCR0_LOADVALUE	800		//��ʼ CCR0��PWM Ƶ�ʣ�ֵ
#define 	MOTOR_TIMER_PWM_DEFAULT_CCR	0		//��ʼ CCR���Ƚϣ�ֵ


#define		MOTOR_ENCODER_SPEED_QUERING_MAINCYCLE_TIME	// ��ѭ�� ѭ�����ٴ� ��ѯһ�α�������
#define		MOTOR_ENCODER_0_SPEED_DURING_TIME	90	// ��ѭ�� ��ȡ��ʱ����ֵ�����ٴ�û�з����ı䣿������������ٶ����Ϊ 0
#define		MOTOR_ENCODER_SPEED_BUFFER_SIZE		15
#define		MOTOR_ENCODER_MINIMUN_DURING_TIME	35000u		//������ �����ã���ֹ��Ϊ������δ���������

#define		MOTOR_ENCODER_SPEED_COEFFICIENT		215000.0f

/**
 * @brief	PID �ṹ�壬Ƕ���� Motor_Typedef �У�\b ��ֹ ����ʹ��
 * @note	�ں���
 * 				PID ����
 * 				���
 */
typedef struct{
	


	//==== ������������ �� ����ʽPID��ʽ �� ����ϵ������ֹ���и�ֵ����Ҫͨ�� Motor_PID_Set(PID_Typedef*, Kp, Ki, Kd) ���� ��á�
	float A;	// ��ǰ��� ϵ��
	float B;	// �ϴ���� ϵ��
	float C;	// ���ϴ���� ϵ��

		
	float setPoint;			// �趨 Ŀ��ֵ
	
	float outputValue;		// PID ���������ļ�����
	float lastError;		// �ϴ�   ���
	float prevError;		// ���ϴ� ���
	
	bool enableSign; // ���==0��ʹ��PID��ֱ�ӽ������ռ�ձ����ȥ
	
	struct{
		bool  enable; 	// �����޷�
		float max;		
		float min;	
	}limiting;

		
}PID_Typedef; 





/**
 * @brief	Motor �ṹ��
 * @note	�ں���
 * 				��ǰ��⵽���ٶȺ����õ��ٶȣ���������
 * 				���ռ�ձ�
 * 
 * 				���
 */
typedef struct{
	
	bool enable;
	bool isStarted;
	
	// float 	setSpeed;		// with SIGN (direction)
	// bool 	setDirection;	// foreward or backward, synchronized with the sign of SPEED
	
	//float currentSpeed;	// READ ONLY
	
	int16_t force;			// [in/out] duty cycle of Motor PID signal, unsigned, while don't use PID, this value output to the motor(duty cycle)
	
	//bool isStarting;	
	
	uint32_t lastTimer32Value;	// �ϴε� Timer32 ����ֵ�����ڼ�����
	uint32_t durationTime;		// ���� �жϼ� Timer32 �� �������
	
	// uint8_t  durationTimeBufferIndex;
	// uint8_t  durationTimeBufferAmount;	//
	// uint32_t[MOTOR_ENCODER_DURATION_BUFFER_AMOUNT] durationTimeBuffer;
	
	
		
	volatile bool    isInterruptedDuringPIDperiod;// set in interrupt handler and clear in PID funcion, for detecting if the speed refreshed 
	uint8_t dasabi;
	
	//bool isInterrupting;
	
	//==== ���������
	float detectedSpeed;
	bool  detectedDirection;
	/*struct{
		bool  enable;
		float speed;
		bool  direction;
	}encoder;*/
	
	//==== �ǲ���
	volatile bool    isInSteppingMode;	// �Ƿ����ڶ�����ģʽ
	bool 	stepModeDir;		// ������ģʽ ����
	int64_t stepCount;			// ����������ʼ������
	int64_t stepTarget;			// ������ģʽ Ŀ�경�����������꣩���� isInSteppingMode == 1 �� ��Ч
	int16_t steppingModeSpeed;	// ������ģʽʹ�õ��ٶ�
	int16_t speedBeforeStepMode;// �ǲ�ģʽ ����ǰ �� �ٶ�
	// uint8_t  detectedSpeedBufferIndex;
	// uint8_t  detectedSpeedBufferAmount;	//
	// uint32_t[MOTOR_ENCODER_SPEED_BUFFER_AMOUNT] detectedSpeedBuffer;	
	
	
	
	//Ƕ�� PID��encoder �ṹ��
	PID_Typedef 		PID;	// PID structure, embendded here
	
	
}Motor_Typedef;  // while declare, please ASSIGN ={0} to initialize the structure to all 0 !!!!!


//============== ����ȫ�ֱ������� Public Global Variables Defines ===============

extern Motor_Typedef Motor[MOTOR_AMOUNT];
extern uint64_t PID_count;

//================== ���к������� Public Function Defines =======================

extern void PID_Clear(volatile PID_Typedef* p_PID_struct);
extern float PID_Calc(volatile PID_Typedef* p_PID_struct, float currentValue);

extern void Motor_init(bool isStartMotor);
extern void Motor_Start(void);
#define Motor_Halt() Motor_Stop()
extern void Motor_Stop(void);
extern void Motor_Disable(void);
extern void Motor_ClearPID(void);


int16_t Motor_SetForce(uint8_t, bool, int16_t);

extern void PID_SetPoint(PID_Typedef* p_PID_struct, float point);
extern void PID_Set(PID_Typedef* p_PID_struct, float Kp, float Ki, float Kd, bool inverse, bool is_reset);

extern void Motor_GoStep(uint8_t motorSelection, int16_t speed, int64_t step);

#endif
