#include "Mode\default.h"
#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"
#include "UART0.h"
#include "Motor.h"
#include "TracingSensor.h"

/*	========================按键GPIO说明==================


*/


//========================== 私有宏定义 Private Macros ===========================





//===================== 公有全局变量 Public Global Variables =====================


//==================== 私有全局变量 Private Global Variables =====================





//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

//========================= 公有函数  Public Functions ===========================

/**
 * @brief  默认模式_初始化
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void Default_init(void)
{
	
	
}

/**
 * @brief  默认模式_主函数循环调用_每 \b 200 周期调用一次
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void Default_Cycle(void)
{
	static uint64_t count = 0;
	//char str[8] = {0};
	
	count++;
	
	if(count % 1000u == 0){
		//显示标题
		OLED_PrintString(Interface_modeConfig.name, 0, 0, OLED_TYPE_6X8, true);	
		
			//显示内容
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
 * @brief  默认模式_停止，逆向初始化
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void Default_Halt(void)
{
	
	
}

//======================== 中断函数 Interrupt Handlers ===========================
