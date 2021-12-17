#include "Mode\LineTracing.h"
#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"
#include "UART0.h"
#include "Motor.h"
#include "TracingSensor.h"


/*	========================����GPIO˵��==================


*/
//========================== ˽�к궨�� Private Macros ===========================





//===================== ����ȫ�ֱ��� Public Global Variables =====================

bool isTracking = false;



//==================== ˽��ȫ�ֱ��� Private Global Variables =====================





//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

//========================= ���к���  Public Functions ===========================


void LineTracing_Key3_StartTracking(void)
{
	
	if(Sensor.enableTracing == true){ 
		Sensor_StopTracing();
		LED2_switch(LED2_COLOUR_BLACK);
	}else{	// Sensor.enableTracing == false;
		Sensor_StartTracing();
		LED2_switch(LED2_COLOUR_GREEN);
	}
	
	
}



/**
 * @brief  Ĭ��ģʽ_��ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void LineTracing_init(void)
{
	Motor_Start();
	
}

/**
 * @brief  Ĭ��ģʽ_������ѭ������_ÿ \b 200 ���ڵ���һ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void LineTracing_Cycle(void)
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
		
		Sensor_GetStatusString(UART0_string);	
		OLED_PrintString(UART0_string, 12, 4, OLED_TYPE_6X8, false);	
		
		sprintf(UART0_string, "Pos:%+6.2f, Am: %d ", Sensor.averagedLineLocation, Sensor.shelteredSensorsAmount);
		OLED_PrintString(UART0_string, 12, 5, OLED_TYPE_6X8, false);	
		
		sprintf(UART0_string, "PID:%+6.2f", Sensor.PID.outputValue);
		OLED_PrintString(UART0_string, 12, 6, OLED_TYPE_6X8, false);		
		
		if(Sensor.enableTracing == true){
			OLED_PrintString("Tracking...", 54, 7, OLED_TYPE_6X8, true);	
		}else{
			OLED_PrintSpace(11u, 54, 7, OLED_TYPE_6X8, false);
			//OLED_PrintString("           ", 54, 7, OLED_TYPE_6X8, false);
		}
	}
	
	
}

/**
 * @brief  Ĭ��ģʽ_ֹͣ�������ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void LineTracing_Halt(void)
{
	
	Motor_Stop();
	Sensor_StopTracing();
	
}

//======================== �жϺ��� Interrupt Handlers ===========================
