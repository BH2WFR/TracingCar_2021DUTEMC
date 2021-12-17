#include "Remocon_Keys.h"

/*====警告: 本文件不能与  OnBoard_Key.h/.c  共存!========*/


//========================== 私有宏定义 Private Macros ===========================


//===================== 公有全局变量 Public Global Variables =====================
uint8_t Key_lastKey = 0;			//
uint8_t Key_lastEvent = 0;			// 


volatile bool	Key_detectFlag[KEY_AMOUNT] = {false};	//上一次的按键状态


/**
 * @brief	以下为 函数向量表, 根据每个功能模块的不同, 自定义按键事件函数, 请在各个赛题模块中自行对其赋值
 * 		=赋值方法:	 Key_eventFunctionVector[event][index] = FunctionName;
 * 				或  Key_eventFunctionVector[event][index] = &FunctionName;
 * 		=调用方法:	Key_eventFunctionVector[event][index]();
 * */
void (*Key_eventFunctionVector[KEY_EVENT_AMOUNT][KEY_AMOUNT])(void) = {0};	//按钮 事件函数向量表

//==================== 私有全局变量 Private Global Variables =====================

	//	按键事件函数是否正在运行, 需要在按键检测程序中
bool    Key_isRunningEventFunction[KEY_EVENT_AMOUNT][KEY_AMOUNT] = {0};

uint8_t Key_lastKeyEvent_event	= 0;			// 当前/曾经 执行的按钮的 事件类型
uint8_t Key_lastKeyEvent_index  = 0;			// 当前/曾经 执行的按钮的 编号

	//按键检测缓冲, 在中断服务函数中对其不断移位, 检测时看它是不是全1或全0
uint32_t Key_detectBuffer[KEY_AMOUNT] = {0};	// {0} 代表初始化全部元素


const Timer_A_UpModeConfig TimerA3_upConfig =
{
	TIMER_A_CLOCKSOURCE_SMCLK,				// SMCLK Clock Source == 12MHz(HFXT-48Mhz, /4)
	TIMER_A_CLOCKSOURCE_DIVIDER_12,			// SMCLK/12 = 1MHz
	1500 - 1,								// 1M/1500   666.67kHz, 1.5ms
	TIMER_A_TAIE_INTERRUPT_DISABLE,			// Disable Timer interrupt
	TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,		// Enable CCR0 interrupt
	TIMER_A_DO_CLEAR						// Clear value
};

/**
 * @brief  GPIO端口的映射表, 方便中断函数中的循环对按钮进行检测   */
const uint8_t Key_GPIOport[KEY_AMOUNT] = {
	[0] = KEY0_PORT,
	[1] = KEY1_PORT,
	[2] = KEY2_PORT,
	[3] = KEY3_PORT,
	[4] = KEY4_PORT,
	[5] = KEY5_PORT,
	[6] = KEY6_PORT,
};
	
const uint8_t Key_GPIOpin[KEY_AMOUNT] = {
	[0] = KEY0_PIN, 
	[1] = KEY1_PIN, 
	[2] = KEY2_PIN, 
	[3] = KEY3_PIN, 
	[4] = KEY4_PIN, 
	[5] = KEY5_PIN, 
	[6] = KEY6_PIN,
};

//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

/**
 * @brief  按钮事件向量表默认指向的函数	暂时里面不放东西
 * @param  none
 * @return none  */
__weak void Key_UndefinedPressEvent(void)
{
	LED2_roll(LED2_ROLLUP);
	

	Beep_PlayMusic(music_UndefinedButton);

		
}
__weak void Key_UndefinedUpEvent(void)
{
	
}



/**
 * @brief  弱函数  保留备用  按键事件函数, 推荐写到模块文件里面, 命名为 Mode0_Key0_Press
 * @param  none
 * @return none  */
__weak void Key1_Press(void)	{}
__weak void Key1_Up(void)		{}
//__weak void Key1_LongPress()	{}
__weak void Key2_Press(void)	{}
__weak void Key2_Up(void)		{}
//__weak void Key2_LongPress()	{}


/**
 * @brief  私有方法 按键事件服务函数  检测上一个事件是否处理完, 调整标志位, 调用按钮事件向量表里面指向的按钮事件函数
 * 			本函数 由 Key_Detect() 调用
 * @param  event	[in]	按钮的按下/松开事件, 可用值: KEY_EVENT_PRESS, KEY_EVENT_UP
 * @param  index	[in]	按钮编号, 可用值: 0 ~ 8	
 * @return none  */
static void Key_Event(uint8_t event, uint8_t index)
{
	if(Key_isRunningEventFunction[event][index] == true) {
		return;
	}
	
	Key_isRunningEventFunction[event][index] = true;	//标志位改为: 正在运行
	Key_eventFunctionVector[event][index]();			//触发向量表中的函数
	Key_isRunningEventFunction[event][index] = false;
	
}


/**
 * @brief  （ \b 严禁外部调用！ ）重置按钮事件向量表中的 该遥控器版固有功能按键 对应服务函数
 * @param  none
 * @return none 
 * @note	本函数 由 Key_EventFunctionVector_ResetAll() 函数 调用*/
static void Key_EventFunctionVector_ResetFunctionalKeys()
{
	
//KEY8(上排右) 指向 OLED初始化函数 因为 OLED容易被干扰, 有可能需要重启它	
#ifdef __OLED_12864_H__
	Key_eventFunctionVector[KEY_EVENT_PRESS][OLED_RESTART_KEY_INDEX] = &OLED_Restart;
#endif

//切换功能键 触发的函数
#ifdef __INTERFACE_H__
	Key_eventFunctionVector[KEY_EVENT_PRESS][SWITCHMODE_KEY_INDEX] = &Key_SwitchMode_Press;
#endif
	
}

//========================= 公有函数  Public Functions ===========================



/**
 * @brief  重置按钮事件向量表中的 所有 元素, 默认指到 &Key_UndefinedEvent
 * @param  none
 * @return none 
 * @note   禁止使用memset函数  Key_init()中要调用它*/
void Key_EventFunctionVector_ResetAll(void)
{
	//向量表中的函数指针全部改为 &Key_UndefinedEvent
	
	// for(uint8_t i = 0; i < KEY_EVENT_AMOUNT; i++){
	// 	for(uint8_t j = 0; j < KEY_AMOUNT; j++){
	// 		Key_eventFunctionVector[i][j] = &Key_UndefinedPressEvent;
	// 	}	
	// }
	
	for(uint8_t j = 0; j < KEY_AMOUNT; j++){
		Key_eventFunctionVector[KEY_EVENT_PRESS][j] = &Key_UndefinedPressEvent;
		Key_eventFunctionVector[  KEY_EVENT_UP ][j] = &Key_UndefinedUpEvent;
	}
	
		//重置按钮事件向量表中的 该遥控器版固有功能按键 对应服务函数
	Key_EventFunctionVector_ResetFunctionalKeys();

}

/**
 * @brief  重置按钮事件向量表中的 指定 元素, 默认指到 &Key_UndefinedEvent
 * @param  event	[in]	按钮的按下/松开, 可用值: KEY_EVENT_PRESS, KEY_EVENT_UP
 * @param  index	[in]	按钮编号, 可用值: 0 ~ 8	
 * @return none  */
void Key_EventFunctionVector_Reset(uint8_t event, uint8_t index)
{
	switch(event){
		case KEY_EVENT_PRESS:
				Key_eventFunctionVector[event][index] = &Key_UndefinedPressEvent;
			break;
		case KEY_EVENT_UP:
				Key_eventFunctionVector[event][index] = &Key_UndefinedUpEvent;
			break;
	}
}


/**
 * @brief  根据中断函数放入的按键状态缓冲数组 比较 来判断按钮是否真实按下 (进行按钮消抖), 
 * 			如果检测到, 调用 Key_Event(#,#) 函数, 从而触发发向量表里面的函数
 * 			如果启用 KEY_DETECT_IN_INTERRUPT_HANDLER, 此函数由 中断服务函数 触发
 * 			如果处于 KEY_DETECT_IN_MAIN_LOOP, 此函数 需要在 main()循环里面 手动调用(推荐)
 * @param  none
 * @return none  */
void Key_Detect(void)
{
	for(uint8_t i = 0; i < KEY_AMOUNT; i++){
		//按键松开时按下
		if((Key_detectFlag[i] == false) && ((Key_detectBuffer[i] & KEY_DETECT_MASK) == KEY_DETECT_DOWN)){
			Key_detectFlag[i] = true;
			Key_Event(KEY_EVENT_PRESS, i);
		//按键按下后松开
		}else if ((Key_detectFlag[i] == true) && ((Key_detectBuffer[i] & KEY_DETECT_MASK) == KEY_DETECT_UP)){
			Key_detectFlag[i] = false;		
			Key_Event(KEY_EVENT_UP, i);
		}
		
	}
}



/**
 * @brief  初始化按钮
 * @param  none
 * @return none  */
void Key_init(void)
{

	for(int i = 0; i < KEY_AMOUNT; i++){
		GPIO_setAsInputPinWithPullUpResistor(Key_GPIOport[i], Key_GPIOpin[i]);
		GPIO_clearInterruptFlag(Key_GPIOport[i], Key_GPIOpin[i]);
		GPIO_disableInterrupt(Key_GPIOport[i], Key_GPIOpin[i]);
		
	}
	
	Key_EventFunctionVector_ResetAll();
	
	Timer_A_configureUpMode(TIMER_A3_BASE, &TimerA3_upConfig);
	Timer_A_enableInterrupt(TIMER_A3_BASE);
	Interrupt_enableInterrupt(INT_TA3_0);
	Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);
	
}




//======================== 中断函数 Interrupt Handlers ===========================
/**
 * @brief  TimerA3_0中断
 * 			主要功能: 将八个按钮的当前状态放入按键缓冲全局数组中, 并对其进行移位操作, 
 * 			如果打开 KEY_DETECT_IN_INTERRUPT_HANDLER(不推荐) 则顺便检测按钮*/
void TA3_0_IRQHandler(void)
{
	//清除中断标志
	BITBAND_PERI(TIMER_A_CMSIS(KEY_DETECT_TIMER_BASE)->CCTL[0], TIMER_A_CCTLN_CCIFG_OFS) = 0;
	//BITBAND_PERI(TIMER_A_CMSIS(KEY_DETECT_TIMER_BASE)->CTL,TIMER_A_CTL_IFG_OFS) = 0;
	//LED1_toggle();

	for(uint8_t key = 0; key < KEY_AMOUNT; key++)
	{
		Key_detectBuffer[key] <<= 1;	//按钮缓冲移位
		Key_detectBuffer[key] |= ( ! GPIO_getInputPinValue(Key_GPIOport[key], (uint16_t)(Key_GPIOpin[key])));
		
	}
	
	
	//检测按钮进入事件
	#if (KEY_DETECT_MODE == KEY_DETECT_IN_INTERRUPT_HANDLER)	
		Key_Detect();
	#endif
    
}


