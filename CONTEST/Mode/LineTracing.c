#include "Mode\LineTracing.h"
#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"
#include "UART0.h"
#include "Motor.h"
#include "TracingSensor.h"
#include "Beep.h"
#include "Beep_song.h"

/*	========================����GPIO˵��==================


*/
//========================== ˽�к궨�� Private Macros ===========================

#define ssrln Sensor.features.trackingLine



//===================== ����ȫ�ֱ��� Public Global Variables =====================

//bool isTracking = false;

// bool 		isRushMode = false;
// uint64_t  rushModeStartPIDCount = 0;

//==================== ˽��ȫ�ֱ��� Private Global Variables =====================





//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

//========================= ���к���  Public Functions ===========================

void LideTracing_Key4_StartRush(void)
{
	if(Sensor.enableTracing == true){
		// ֹͣ
		ssrln.fastMode.enable = false;
		//ssrln.fastMode.enable = false;
		
		Beep_StopMusic();
		Sensor_StopTracing();
		LED2_switch(LED2_COLOUR_BLACK);		
		
	}else{
		// ��ʼ
		
		ssrln.fastMode.enable = true;
		ssrln.fastMode.start_PIDCount = PID_count;
		ssrln.fastMode.goSlowAfterCount = true;
		ssrln.fastMode.stop_PIDCount = ssrln.fastMode.start_PIDCount + ssrln.fastMode.slow_PIDDuringCount;
		
		Beep_PlayMusic(&music_ErQuanYingYue);
		Sensor_StartTracing();
		LED2_switch(LED2_COLOUR_GREEN);		
	}
}

void LineTracing_Key3_StartTracking(void)
{
	
	if(Sensor.enableTracing == true){ 
		ssrln.fastMode.enable = false;
		
		Beep_StopMusic();
		Sensor_StopTracing();
		LED2_switch(LED2_COLOUR_BLACK);
		
	}else{	// Sensor.enableTracing == false;
		ssrln.fastMode.enable = false;
		Beep_PlayMusic(&music_ErQuanYingYue);
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
	
	if((ssrln.fastMode.enable == true) && (PID_count >= ssrln.fastMode.stop_PIDCount)){
		ssrln.fastMode.enable = false;
	}
	
	
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
		
		if(ssrln.fastMode.enable == true){
			OLED_PrintString("R", 1, 7, OLED_TYPE_6X8, true);
		}else{
			OLED_PrintString(" ", 1, 7, OLED_TYPE_6X8, false);
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
