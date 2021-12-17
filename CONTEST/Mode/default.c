#include "Mode\default.h"
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


//==================== ˽��ȫ�ֱ��� Private Global Variables =====================





//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

//========================= ���к���  Public Functions ===========================

/**
 * @brief  Ĭ��ģʽ_��ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void Default_init(void)
{
	
	
}

/**
 * @brief  Ĭ��ģʽ_������ѭ������_ÿ \b 200 ���ڵ���һ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void Default_Cycle(void)
{
	static uint64_t count = 0;
	//char str[8] = {0};
	
	count++;
	
	if(count % 1000u == 0){
		//��ʾ����
		OLED_PrintString(Interface_modeConfig.name, 0, 0, OLED_TYPE_6X8, true);	
		
			//��ʾ����
		sprintf(UART0_string, "L:%+6.2f, R:%6.2f", Motor[0].detectedSpeed, Motor[1].detectedSpeed);
		OLED_PrintString(UART0_string, 0, 2, OLED_TYPE_6X8, false);
		

		
				
		Sensor_GetStatusString(UART0_string);
		OLED_PrintString(UART0_string, 12, 3, OLED_TYPE_6X8, false);	
		
		sprintf(UART0_string, "Pos:%+6.2f, Am: %d ", Sensor.averagedLineLocation, Sensor.shelteredSensorsAmount);
		OLED_PrintString(UART0_string, 12, 4, OLED_TYPE_6X8, false);
				
		OLED_PrintString(main_versionStr, 48, 6, OLED_TYPE_6X8, false);
				
		OLED_PrintString("Press", 0, 7, OLED_TYPE_6X8, false);
		OLED_PrintString_Append("5 ", OLED_TYPE_6X8, true);
		OLED_PrintString_Append("to switch Mode", OLED_TYPE_6X8, false);	
		

		
	}
	
	
}

/**
 * @brief  Ĭ��ģʽ_ֹͣ�������ʼ��
 * @param  none
 * @return none  
 * @note	����ֱ�ӵ��øú�����Ҫͨ��interface.c ����ṹ���еĺ���ָ�������� */
void Default_Halt(void)
{
	
	
}

//======================== �жϺ��� Interrupt Handlers ===========================
