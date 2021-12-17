#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"
#include "Beep.h"
#include "Beep_song.h"

/**====================================================================================
 * 			添加/删除 模块时需要重点改动的地方 1 	！！！！！！！
 * ==============在这里写入各个模块的头文件，要求用「Mode\」 开头==============*/
#include "Mode\default.h"
#include "Mode\LineTracing.h"
#include "Mode\Control_Car.h"


/*	========================按键GPIO说明==================
												│    Debugger   │
				 ┌──────┬──────┬──────┐    Key0┌┤  ───────────  ├┐Key2
	┌───────┐    │ Key6 │ Key7 │ Key8 │    P1.1└┤ ┌─┐       ┌─┐ ├┘P1.4
	│  Key0 │    │ P4.4 │ P4.5 │ P4.6 │         │ │ │ ┌───┐ │ │ │
	│Encoder│    ├──────┼──────┼──────┤         │ │ │ │MCU│ │ │ │
	│  P4.0 │    │ Key3 │ Key4 │ Key5 │         │ │ │ └───┘ │ │ │
	└───────┘    │ P4.1 │ P4.2 │ P4.3 │         │ └─┘       └─┘ │
			 	 └──────┴──────┴──────┘         └───────────────┘
				  
	特殊按键功能说明:  
	
		切换模式按键 (KEY 6)：	依次向上切换模式！
		屏幕刷新按键 (KEY 8)：	重启屏幕（参见 Remocon_Keys.c 和 OLED_12664.c）
	
	
	
================================  使  用  方  法  ===============================

	1. 主函数循环中必须调用:	 	Interface_modeConfig.Cycle_func();
	
	2. 主函数循环前必须调用初始化： Interface_SwitchMode(0);
	
	3. 手动切换模式的方法:			Interface_SwitchMode(mode);
	
	4. 对应功能文件中的 Interface_modeConfig.Cycle_func 指向的函数里面 
		需要有：	OLED_PrintString(Interface_modeConfig.name, 0, 0, OLED_TYPE_6X8, true);	
		这个语句，像屏幕的第一行反白显示当前运行的模式
	
	5. "Remocon_Keys.c" -> Key_EventFunctionVector_ResetFunctionalKeys() 函数中 将 
		Key_SwitchMode_Press() 赋值给 按钮事件服务函数指针数组中
	
	6. 如需增删功能模块文件，需要更改一下内容：
		1) 包含\删除 模块的.c文件，将.h文件放入对应目录
		1) 本文件上面的 #include, 要求格式用 "mode\____" 开头
		2) 本文件 下面的 MODE_AMOUNT 宏定义中 填入 总模块数量, 在它的下面修改具体的模块名宏定义
		3) 本文件 Interface_SwitchMode() 函数 的 switch() 结构中 增加/删除 对应内容
*/



//========================== 私有宏定义 Private Macros ===========================

#define  g_config	Interface_modeConfig

/**====================================================================================
 * 			添加/删除 模块时需要重点改动的地方 2 	！！！！！！！
 * ==============在这里写入各个模块的序号及名称 即其数目==============*/





//===================== 公有全局变量 Public Global Variables =====================

uint8_t 		Interface_currentMode = 0;		// (只允许读取，严禁外部更改) 当前工作的模式

Mode_typedef 	Interface_modeConfig = 
								{0/*.Halt_func =*/ };		//当前工作的模式的配置结构体，内含标题与各种函数指针


//==================== 私有全局变量 Private Global Variables =====================





//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

static void SetModeString(char* p_str)
{
	//=====以下代码为生成 屏幕第一行的标题字符串，格式是 "x ====ModuleName======", 长度为OLED_X_6x8_AMOUNT，x 为 模式编号，模式名居中，两边等号填充；只是生成文本，不做显示
	
	
	uint8_t restOfLength = 0;			//字符显示在屏幕后该行还剩多少个字符空间？
	uint8_t leftRemainingSize = 0;		// 字符显示 居中后 左侧 剩余空间
	//uint8_t rightRemainingSize = 0;	
	uint8_t rigntSpaceStartIndex = 0;
		
	uint8_t length_str = strlen(p_str);
	
	
	restOfLength = OLED_X_6x8_AMOUNT - length_str;
	leftRemainingSize = (restOfLength / 2) + 1;						// 左边的「=」符号数量（让模式名居中）
	
	for(uintptr_t i = 0; i < leftRemainingSize; i++){			//打印左边的「=」
		g_config.name[i] = '=';
	}
	
	g_config.name[leftRemainingSize] = '\0';	// 为下一句strcat识别准备（相当于字符串「=」后面清零）
	strcat(g_config.name, p_str);			// 拼接模式名
	
	rigntSpaceStartIndex = leftRemainingSize + length_str;	// 打印右边的「=」
	for(uintptr_t j = rigntSpaceStartIndex; j < OLED_X_6x8_AMOUNT; j++){
		g_config.name[j] = '=';
	}
	g_config.name[OLED_X_6x8_AMOUNT] = '\0';	// 字符串结尾
	
	char modeIndexStr[3];
	sprintf(modeIndexStr, "%-2u", Interface_currentMode);	// 左边另行打印当前的模式编号（覆盖「=」符号）
	for(uintptr_t i = 0; i < 2; i++){
		g_config.name[i] = modeIndexStr[i];
	}
}


static void defaultVoidFunction()
{
	volatile int a;
	__NOP();
}

//========================= 公有函数  Public Functions ===========================

/**
 * @brief  ( \b 严禁外部调用！) 切换模式按键触发的事件处理函数，
 * 			该函数 需要在 按键初始化时 被 函数指针指上	
 * @note	如果需要修改，请上 "Remocon_Keys.c" -> Key_EventFunctionVector_ResetFunctionalKeys() 函数 更改*/
void Key_SwitchMode_Press()
{

	Beep_PlayMusic(music_ChangeMode);

	
	RollUpRange( Interface_currentMode, 0, MODE_AMOUNT);
	Interface_SwitchMode(Interface_currentMode);
	
}




/**
 * @brief  切换模式
 * @param  mode[in]	需要更换的模式
 * @return none  
 * @note	该函数 改动 Interface_currentMode 全局变量 但是！全局变量要在函数的末尾赋值！*/
void Interface_SwitchMode(uint8_t mode)
{
	char mode_name[24] = {0};	// 临时存放模式名称
	

	
	Key_EventFunctionVector_ResetAll();	//清除所有按键映射
	OLED_ClearAll();					//清除屏幕
	
	g_config.Halt_func();	//停止上一个模式

	
	uint8_t shabi = LED2_colour;	// 记录 LED 状态，然后亮红色，函数运行结束后恢复原状
	LED2_switch(LED2_COLOUR_RED);
	
	
	/**====================================================================================
	 * 		添加/删除 模块时需要重点改动的地方 3 ！！！！！！！*/
	switch(mode){
		case 0:
			//======== 1.   模 式 名 赋 值 （不要赋值给 g_config.name） =================
			strcpy(mode_name, "Hardware Test");	//这里只赋值给这个局部变量，后面有代码给字符串前后加上装饰符号赋值给 g_config.name
			
			//======== 2. 模式对应的 开始/循环/结束 函数赋值给函数指针 =================
			g_config.Start_func = Default_init;
			g_config.Halt_func = Default_Halt;
			g_config.Cycle_func = Default_Cycle;
			
			//======== 3. 模式对应的 按键事件服务 函数赋值给函数指针 =================
			// Key_eventFunctionVector[KEY_EVENT_PRESS][1] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][2] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][3] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][4] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][5] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][6] = NULL;

			
			break;
		case 1:
			//======== 1.   模 式 名 赋 值 （不要赋值给 g_config.name） =================
			strcpy(mode_name, "Line Tracking");	//这里只赋值给这个局部变量，后面有代码给字符串前后加上装饰符号赋值给 g_config.name
			
			//======== 2. 模式对应的 开始/循环/结束 函数赋值给函数指针 =================
			g_config.Start_func = LineTracing_init;
			g_config.Halt_func = LineTracing_Halt;
			g_config.Cycle_func = LineTracing_Cycle;
			
			//======== 3. 模式对应的 按键事件服务 函数赋值给函数指针 =================
			// Key_eventFunctionVector[KEY_EVENT_PRESS][1] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][2] = NULL;
			Key_eventFunctionVector[KEY_EVENT_PRESS][3] = LineTracing_Key3_StartTracking;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][4] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][5] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][6] = NULL;
		
			
			
			
			
			
			break;
		case 2:
			//======== 1.   模 式 名 赋 值 （不要赋值给 g_config.name） =================
			strcpy(mode_name, "Remote Ctrl");	//这里只赋值给这个局部变量，后面有代码给字符串前后加上装饰符号赋值给 g_config.name
			
			//======== 2. 模式对应的 开始/循环/结束 函数赋值给函数指针 =================
			g_config.Start_func = ControlCar_init;
			g_config.Halt_func = ControlCar_Halt;
			g_config.Cycle_func = ControlCar_Cycle;
			
			//======== 3. 模式对应的 按键事件服务 函数赋值给函数指针 =================
			// Key_eventFunctionVector[KEY_EVENT_PRESS][0] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][1] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][2] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][3] = NULL;
			 Key_eventFunctionVector[KEY_EVENT_PRESS][4] = ControlCar_Key_EnableMotor;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][5] = NULL;
			// Key_eventFunctionVector[KEY_EVENT_PRESS][6] = NULL;
		
		default:
			break;
			
	}
	

	
	SetModeString(mode_name);
	
	
	Interface_currentMode = mode;		//更改全局变量
	g_config.Start_func();				//启动切换后的模式
	
	
	//====不显示到OLED，请在各模式的文件中自行打印
	
	//Interface_DisplayFirstLine();
	//OLED_PrintString(g_config.name, 0, 0, OLED_TYPE_6X8, true);	//液晶屏显示，反白显示
	
	
	
	LED2_switch(shabi);
}


void Interface_init(void)
{
	g_config.Start_func = defaultVoidFunction;
	g_config.Cycle_func = defaultVoidFunction;
	g_config.Halt_func = defaultVoidFunction;
	
	Interface_SwitchMode(0);
}
//======================== 中断函数 Interrupt Handlers ===========================







