#include "Mode\Control_Car.h"
#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"	// �������еĶ�ʱ�����ж��Ƿ�ʱ��û���������ź�
#include "UART0.h"
#include "Motor.h"
#include "TracingSensor.h"

/*	========================����GPIO˵��==================


	
	
*/
//========================== ˽�к궨�� Private Macros ===========================





//===================== ����ȫ�ֱ��� Public Global Variables =====================
ControlCar_typedef  ControlCar = {0};

//==================== ˽��ȫ�ֱ��� Private Global Variables =====================

//bool g_enable = false;



//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============






//========================= ���к���  Public Functions ===========================



void ControlCar_Key_EnableMotor(void)
{
	if(ControlCar.enable == true){ 
		ControlCar_StopMotor();	// �����Ѿ� false �� ��־λ�� 
		
	}else{	// ControlCar.enable == false;
		ControlCar_StartMotor();
		
	}	
}

/**
 * @brief  �� \b ��ʱ���ж� ���ã��� ��ȡ ��ʱ��
 * @param  none  
 * @return none  
 * @note	*/
void ControlCar_TimerInterrupt(uint32_t timerCount)
{
	
	//ControlCar.noSignal.timerInterruptCount = timerCount;
	
	
	if(ControlCar.noSignal.sign == false){
		
		ControlCar.noSignal.timerInterruptCount++;	// ���� ����
		
		//==== ��� ��ʱ��û�ж�ȡ�� ����
		if(ControlCar.noSignal.timerInterruptCount == ControlCar.noSignal.thresholdCount){
			ControlCar.noSignal.sign = true;	// 
			ControlCar_StopMotor();				// ֱ�ӹرյ��
			
		}
		
	}else{	// ControlCar.noSignal.sign == true
		
		
		
	}
	
	
	
}


/**
 * @brief  �� \b ���ڽ����ж� ���ã�
 * @param  none
 * @return none  
 * @note	*/
void ControlCar_AnalysisReceivedDataFrame(uint8_t RxBuf)
{
	if(ControlCar.enable == false){
		return;		// �������
	}
	
	ControlCar.noSignal.sign = false;				// ��ȡ���˴��ڣ�
	ControlCar.noSignal.timerInterruptCount = 0;	// ��� ����
	
	
	if(RxBuf == CONTROL_CAR_DATA_FIRST_FRAME){
		ControlCar.received.frameCount = 0;
		
	}else{	// �ӵ��� ���� ͷ֡
	
		ControlCar.received.frameCount++;	// ����
		
		// �ӵ��� ��һ������
		if(ControlCar.received.frameCount == 1){
			ControlCar.received.firstParameter = RxBuf;
			
		// �ӵ��� �ڶ�������
		}else if (ControlCar.received.frameCount == 2){
			ControlCar.received.secondParameter = RxBuf;
		
		// �ӵ��� ����
		}else{
			
			
		}
		
		
	}
	
	
}



void ControlCar_SetSpeedAndDir(float speed, float direction)
{
	
}

void ControlCar_SetWheelSpeed(float LeftSpeed, float RightSpeed)
{
	
	
}



void ControlCar_StopMotor(void)
{
	ControlCar.enable = false;
	ControlCar.noSignal.sign = false;
	
	Motor_Stop();
	LED2_switch(LED2_COLOUR_BLACK);
}


void ControlCar_StartMotor(void)
{
	//==== ���ýṹ��
	ControlCar_ResetStruct();
	
	//==== ʹ�ܱ�־λ
	ControlCar.enable = true;
	ControlCar.noSignal.sign = false;
	
	//==== ���� ��� GPIO ���� �� PWM ��ʱ��
	Motor_Start();
	LED2_switch(LED2_COLOUR_YELLOW);
}




//===============================


void ControlCar_ResetStruct(void)
{
	//==== ��սṹ��
	memset(&ControlCar, 0, sizeof(ControlCar));
	
	//==== ��ʼ�����ò��������ܸ�ֵʹ��λ��
	ControlCar.noSignal.thresholdCount = CONTROL_CAR_NO_SIGNAL_TIMER_COUNT_THRESHOLD;	
	
	
}


/**
 * @brief  Ĭ��ģʽ_��ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void ControlCar_init(void)
{
	
	// //==== ��սṹ��
	 ControlCar_ResetStruct();
	
	//==== �������
	ControlCar_StopMotor();
	
}

/**
 * @brief  Ĭ��ģʽ_������ѭ������_ÿ \b 200 ���ڵ���һ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void ControlCar_Cycle(void)
{
	static uint64_t count = 0;
	
	//char str[8] = {0};
	
	count++;
	
	if(count % 1000u == 0){
		//��ʾ����
		OLED_PrintString(Interface_modeConfig.name, 0, 0, OLED_TYPE_6X8, true);	
		
			//��ʾ����
		sprintf(UART0_string, "L :%+6.2f, %+6.2f", Motor[0].detectedSpeed, Motor[0].PID.setPoint);
		OLED_PrintString(UART0_string, 6, 2, OLED_TYPE_6X8, false);
		
		sprintf(UART0_string, "R :%+6.2f, %+6.2f", Motor[1].detectedSpeed, Motor[1].PID.setPoint);
		OLED_PrintString(UART0_string, 6, 3, OLED_TYPE_6X8, false);	
		
			
		
		if(ControlCar.enable == true){
			OLED_PrintString("RmtCtr ON...", 48, 7, OLED_TYPE_6X8, true);	
		}else{
			OLED_PrintSpace(12u, 48, 7, OLED_TYPE_6X8, false);
			//OLED_PrintString("           ", 54, 7, OLED_TYPE_6X8, false);
		}
	}
	
	
}

/**
 * @brief  Ĭ��ģʽ_ֹͣ�������ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void ControlCar_Halt(void)
{
	
	ControlCar_StopMotor();
	
	
}

//======================== �жϺ��� Interrupt Handlers ===========================




