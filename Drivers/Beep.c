#include "beep.h"
#include "Beep_song.h"
//========================== 私有宏定义 Private Macros ===========================

//==================== 私有全局变量 Private Global Variables =====================
static bool g_delayMode = 0;				//是否使用delay模式
static bool g_isPlayingMusic = false;		//是否正在播放整个音乐
static bool g_isPlayingNote = false;		//是否正在播放单个音符

//===================== 公有全局变量 Public Global Variables =====================

bool 	 Beep_isMute = false;				// true 时 静音，无法播放音乐

uint32_t Beep_currentTickCountPerBeat = 0;	//每拍的timer32计数值
/*	以下两个变量可能指向 Beep_note_typedef*, 也可能指向const Beep_note_typedef*, 如果编译器弹出警告, 无视即可, 
	但是这种代码在正常的电脑中是不会编译通过的, 但是在cortex M(STM32 / MSP432E/P )中 因 flash在内存中有映射 故 此代码可编译 	*/
Beep_note_typedef* Beep_p_currentMusic = (void*)NULL;			//当前音乐结构体数组的首地址, 不播放时指向NULL
Beep_note_typedef* Beep_p_currentNote  = (void*)NULL;			//当前音乐结构体数组中 正在播放的音符结构体的地址




//以下为定时器重载值：
const uint16_t Beep_Pitch_loadValue[] = {		//0-119

	// C   | #C bD  |  D   |  sD bE |   E  |   F   |  sF bG |   G  | sG bA |   A    | sA bB |   B  |
	//以下1MHz定时器频率		12分频	0-23
	 61155,  57723,  54484,  51427,  48539,  45815,  43243,  40816,  38525,  36364,  34323,  32396, //0	
	 30578,  28862,  27242,  25713,  24270,  22907,  21622,  20409,  19263,  18182,  17161,  16198,	//1
	//以下按照4MHz定时器频率	3分频	24-47
	 61156,  57723,  54484,  51426,  48540,  45815,  43244,  40817,  38525,  36364,  34323,  32397,	//2
	 30579,  28862,  27242,  25714,  24270,  22908,  21622,  20408,  19263,  18182,  17161,  16198,	//3
	//以下使用12MHz定时器频率	1分频	48-119
	 45866,  43293,  40864,  38569,  36404,  34361,  32433,  30612,  28895,  27273,  25742,  24297,	//4
	 22934,  21646,  20431,  19285,  18202,  17181,  16216,  15306,  14447,  13636,  12871,  12149,	//5
	 11467,  10823,  10215,   9642,   9101,   8590,   8108,   7653,   7224,   6818,   6435,   6074,	//6
	  5733,   5412,   5108,   4821,   4551,   4295,   4054,   3827,   3612,   3409,   3218,   3037,	//7
	  2867,   2706,   2554,   2411,   2275,   2148,   2027,   1913,   1806,   1705,   1609,   1519,	//8
	  1433,   1353,   1277,   1205,   1138,   1074,   1014,    957,    903,    852,    804,    759	//9
	  //120: pause      121: repeat start	122: repeat end 	123: song end
};
//上述数组的二维化   dd = double dimensional
const uint16_t (*Beep_Pitch_loadValue_dd)[12] = (void*)Beep_Pitch_loadValue;





// //以下为各音节对应的频率：
// const float Beep_Pitch_frequency[] = //15.5-16000
// {
// 	// C   | sC bD  |  D   |  sD bE |   E  |   F   |  sF bG |   G  | sG bA |   A    | sA bB |   B  |
// 	//以下1MHz定时器频率		0-23	[15.50, 62.00)
// 	16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.500, 25.957, 27.500, 29.135, 30.868, //0
// 	32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, //1
// 	//以下按照4MHz定时器频率	24-47	[62.00, 250.00)
// 	65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.83, 110.00, 116.54, 123.47, //2
// 	130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,	//3
// 	//以下使用12MHz定时器频率	48-119	[250.00, 16000(20000)]
// 	261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,	//4
// 	523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,	//5
// 	1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5,	//6
// 	2093.0, 221705, 2349.3, 2489.0, 2637.0, 2793.8, 2960.0, 3136.0, 3322.4, 3520.0, 3729.3, 3951.1,	//7
// 	4186.0, 4434.9, 4698.6, 4978.0, 5274.0, 5587.7, 5919.9, 6271.9, 6644.9, 7040.0, 7458.6, 7902.1,	//8
// 	8372.0, 8869.8, 9397.3, 9956.1, 10548 , 11175 , 11840 , 12544 , 13290,  14080 , 14917,  15804 	//9
// };
// //上述数组的二维化   dd = double dimensional
// const uint16_t (*Beep_Pitch_frequency_dd)[12] = (void*)Beep_Pitch_frequency;

//TimerA0用于控制音频的频率
static const Timer_A_UpModeConfig TimerA0_upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source == 12MHz(HFXT-48Mhz, /4)
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 1MHz
        500 - 1,                          		// 1M/1000    1kHz, 1ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};
static Timer_A_CompareModeConfig compareConfig_PWM0_1 =	//P2.4
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,				//CCR < CCR1:et,   CCR > CCR1: reset
        250                                 
};


//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

/**
 * @brief  私有方法 设置定时器的装载值和分频 
 * 			|  被 SetFrequency(float frequency) 和 SetFrequencyByIndex(uint8_t index) 调用
 * @param[in]  value 	设置的初始装载值
 * @param[in]  divider 	定时器的分频系数 | 必须是：1u, 3u, 12u
 * @return none  */
static void SetTimerLoadValue(uint16_t value, uint8_t divider)
{
	//范围检查  divider  必须是 1, 2 或 12
	if(  !(divider == 1u || divider == 3u || divider == 12u)){
		return;
	}
	
	// //检测定时器是否在运作(Up模式)   默认打注释  如果需要对程序魔改, 为防止莫名其妙的错误, 请取消这段注释
	// bool is_timer_worked = false;
	// if( (TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL & TIMER_A_CTL_MC_3) == TIMER_A_CTL_MC_3 ){
	// 	is_timer_worked = true;
	// }
	// if(is_timer_worked == true){	//停止定时器   CTL寄存器4、5位清零
	// 	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;
	// }
	
	//设置分频
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_ID__8;	//清零
    TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 &= ~TIMER_A_EX0_IDEX_MASK;
	switch(divider){
		case 1u:	//1分频（12MHz）
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__1;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x01 - 1;
			break;
		case 3u:	//3分频（4MHz）
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__1;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x03 - 1;
			break;
		case 12u:	//12分频（12MHz）
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__2;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x06 - 1;
			break;
	}
	
	//设置装载值
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CCR[0] = value;
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CCR[4] = value / 2;
	
	// //若之前在运作，恢复定时器Up模式 否则 保持停止状态
	// if(is_timer_worked == true){
	// 	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;
	// }
	
}

/**
 * @brief  私有方法 根据输入的频率设置蜂鸣器频率
 * 			调用 Beep_SetLoadValue(uint16_t value, uint8_t divider)
 * @param  none
 * @return none  
 * 
 */
// static void SetFrequency(float frequency)
// {
// 	//范围检查
// 	if(frequency > BEEP_FREQ_MAX || frequency < BEEP_FREQ_MIN){
// 		return;
// 	}
	
// 	uint8_t divider = 0;
// 	uint16_t loadValue = 0;
	
	
// 	if		(frequency >= BEEP_DIVIDER_1_INDEX_MIN && frequency < BEEP_DIVIDER_1_INDEX_MAX){	//frequency ∈ [250.00, 16000.00]
// 		divider = 1u;
// 		loadValue = (uint16_t)(12e6f / (frequency));	// 12 / 1  = 12MHz
// 	}else if(frequency >= BEEP_DIVIDER_3_INDEX_MIN && frequency < BEEP_DIVIDER_3_INDEX_MAX){	//frequency ∈ [62.00, 250.00)
// 		divider = 3u;
// 		loadValue = (uint16_t)(4e6f / (frequency));		// 12 / 3  = 4MHz
// 	}else if(frequency >= BEEP_DIVIDER_12_INDEX_MIN && frequency <= BEEP_DIVIDER_12_INDEX_MAX){	//frequency ∈ [15.50, 62.00)
// 		divider = 12u;
// 		loadValue = (uint16_t)(1e6f / (frequency));		// 12 / 12 = 1MHz
// 	}	
	
// 	SetTimerLoadValue(loadValue, divider);	
	
// }

/**
 * @brief  私有方法 根据输入的编号（音符序号）选择蜂鸣器频率 
 * 			调用 Beep_SetLoadValue(uint16_t value, uint8_t divider)
 * @param  none
 * @return none  */
static void SetFrequencyByIndex(uint8_t index)
{
	//范围检查
	if(index > BEEP_PITCH_INDEX_MAX){
		return;
	}
	
	uint8_t divider = 0;
	
	if		(index >= BEEP_DIVIDER_1_INDEX_MIN && index <= BEEP_DIVIDER_1_INDEX_MAX){	//index ∈ [48, 119]
		divider = 1u;
	}else if(index >= BEEP_DIVIDER_3_INDEX_MIN && index <= BEEP_DIVIDER_3_INDEX_MAX){	//index ∈ [24, 47]
		divider = 3u;
	}else if(index <= BEEP_DIVIDER_12_INDEX_MAX){	//index ∈ [0, 23]
		divider = 12u;
	}
	
	SetTimerLoadValue(Beep_Pitch_loadValue[index], divider);
	
}


/**
 * @brief  私有方法 解析音符, 判断是音符还是指令
 * @param  p_note**[in|out]	音符指针的地址 (二阶指针! ), 播放音符时不会改动*p_note, 但是有些指令会让其自增 例: &p_note
 * @return none  */
static void ReadNote(Beep_note_typedef** p_note)
{
	switch((*p_note)->pitch){
	case cEnd:		//终止符
		Beep_StopMusic();
		break;
	case cRepeatStart:	//反复记号 开始
		//根据反复次数, 将当前的 ++指针地址 压栈
		break;
	case cRepeatEnd:
		//检测是不是反复完了(次数)
		//将存入的指针地址 出栈, 并跳转到那里
		break;
	case cSetSpeed:		//设置速度
		Beep_SetSpeed((*p_note)->length);
		(*p_note)++;
		ReadNote(p_note);	//递归调用
		break;
	case cStart:
		//不可能发生
		break;
	default:	//音符
		if((*p_note)->pitch > BEEP_COMMAND_INDEX_MAX){
			Beep_StopMusic();
		}
		
		Beep_SingleNote((*p_note)->pitch, (*p_note)->length, BEEP_MODE_NOT_DELAY);
		break;
	}
}


//========================= 公有函数  Public Functions ===========================
/**
 * @brief  蜂鸣器响一个音符
 * @param  index[in]		音符序号, 可用值: 参见宏定义
 * @param  length[in]		音符的长度, 单位: 拍
 * @param  delayMode[in]	延迟模式 / 中断模式, 可用值: BEEP_MODE_USE_DELAY, BEEP_MODE_NOT_DELAY
 * @return none  */
void Beep_SingleNote(uint8_t index, float length, bool delayMode)
{
	g_delayMode = delayMode;
	if(g_isPlayingNote == true || index > cPause) {			//如果正在运行则退出
		return;
	}
	g_isPlayingNote = true;	//打开标志位
	
	TIMER32_CMSIS(BEEP_TIMER32_BASE)->LOAD = (uint32_t)(length * (double)Beep_currentTickCountPerBeat);		//配置Timer32装载值


	//使用delay延迟
	if(delayMode == BEEP_MODE_USE_DELAY){	
		
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL &= ~TIMER32_CONTROL_IE;	//关闭Timer32中断
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_ENABLE;	//启动Timer32
		
		//音符
		if(index != cPause){		
				
			SetFrequencyByIndex(index);	//设置TimerA0装载和分频
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;	//启动 timerA
			while(TIMER32_CMSIS(BEEP_TIMER32_BASE)->VALUE);				//延迟卡死等待, timer32 处于 单次模式
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//关闭 TimerA			
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA 计数值 清零，以使比较模式GPIO口输出低电平，保护蜂鸣器
				
				
		//休止符		
		}else if (index == cPause){		
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//关闭TimerA
			while(TIMER32_CMSIS(BEEP_TIMER32_BASE)->VALUE);				//延迟卡死等待
		}
		
		g_isPlayingNote = false;
		
	//不使用delay延迟，不占用MCU资源	
	}else{		// delayMode == BEEP_MODE_NOT_DELAY							
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_IE;	//打开Timer32中断
		//BITBAND_PERI(TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL, TIMER32_CONTROL_ONESHOT_OFS)= 1;		//配置Timer32单次模式

		if(index != cPause){		
			SetFrequencyByIndex(index);	//设置TimerA0装载和分频
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;	//启动timerA, TimerA在 中断函数 关闭
			
			
		//休止符		
		}else if (index == cPause){	
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//关闭TimerA
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA 计数值 清零，以使比较模式GPIO口输出低电平，保护蜂鸣器
		}
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_ENABLE;	//启动Timer32, 进入中断前本函数退出, 程序正常运行
		
		//g_isPlayingNote 由  中断函数  清零, 在 中断函数 再次调用本函数
		
	}
	
}

/**
 * @brief  设置速度参数
 * @param  speed - 单位：每分钟多少拍
 * @return 每拍的定时器装载值  */
uint32_t Beep_SetSpeed(uint32_t speed)
{
	//Timer32频率 3MHz
	Beep_currentTickCountPerBeat = (3000000 * 60) / speed;
	return Beep_currentTickCountPerBeat;	
}

// uint32_t Beep_GetSpeed()
// {
// 	return Beep_currentTickCountPerBeat;
// }

/**
 * @brief  停止播放音乐, 可外部调用, 也可以被中断函数调用
 * @param  none
 * @return none  */
void Beep_StopMusic(void)
{
	TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL &= ~TIMER32_CONTROL_ENABLE;	//关闭Timer32
	
	if(g_isPlayingNote == true){
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//关闭 TimerA			
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA 计数值 清零，以使比较模式GPIO口输出低电平，保护蜂鸣器		
	}
	
	Beep_p_currentMusic = Beep_p_currentNote = (void*)NULL;
	g_isPlayingMusic = g_isPlayingNote = false;


}


/**
 * @brief  开始播放音乐
 * @param  p_music*[in]  乐谱结构体数组的首地址
 * @return none  */
void Beep_PlayMusic(Beep_note_typedef* p_music)
{
	if(g_isPlayingMusic == true || p_music == (void*)NULL || p_music->pitch != cStart || Beep_isMute == true){
		return;
	}
	
	g_isPlayingMusic = true;	//打开标志位, 中断函数检测到指针指到休止符后清零
	Beep_p_currentMusic = Beep_p_currentNote = p_music;	//给指针赋值
	Beep_p_currentMusic = p_music;
	Beep_p_currentNote = (p_music + 1);
	
		//播放第一个音符(或速度设置指令), 开启timer32中断
	ReadNote(&Beep_p_currentNote);
}




/**
 * @brief  初始化蜂鸣器GPIO及相关定时器 使用本文件前面的两个const结构体 234234
 * @param  speed 	初始速度， \b ==0_时_不初始化， 调用 SetSpeed()
 * @return none  */
void Beep_Init(uint32_t speed)
{
	//	P2.4 TA0.1
	GPIO_setAsPeripheralModuleFunctionOutputPin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
	//GPIO_setAsOutputPin(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
	Timer_A_configureUpMode(BEEP_TIMER_A_BASE, &TimerA0_upConfig);
	Timer_A_initCompare(BEEP_TIMER_A_BASE, &compareConfig_PWM0_1);
	
	//timer32_0频率： 48 MHz / 16 = 3MHz
	Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
	BITBAND_PERI(TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL, TIMER32_CONTROL_ONESHOT_OFS)= 1;		//配置Timer32单次模式
	Timer32_setCount(TIMER32_0_BASE, 10000u);
	Timer32_haltTimer(TIMER32_0_BASE);
	Interrupt_enableInterrupt(INT_T32_INT1);
	Timer32_disableInterrupt(TIMER32_BASE);		//关闭中断
	
	//TimerA中断关掉
	Timer_A_disableInterrupt(BEEP_TIMER_A_BASE);			//
	Interrupt_disableInterrupt(BEEP_TIMER_A_INTERRPUT);		//
	Timer_A_stopTimer(BEEP_TIMER_A_BASE);
	
	//初始化速度
	if(speed != 0){
		Beep_SetSpeed(speed);
	}
	
	
}



//======================== 中断函数 Interrupt Handlers ===========================

/**
 * @brief  Timer32_0 中断函数
 * 主要功能：非delay模式下将乐谱指针一个一个向后推，播放音乐，直到终止符指令；delay模式下中断关闭
 */
void T32_INT1_IRQHandler(void)
{
	Timer32_clearInterruptFlag(BEEP_TIMER32_BASE);	//清除中断标志位
	
	if(Beep_p_currentNote->pitch <= BEEP_PITCH_INDEX_MAX){		//停止 timerA
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//关闭TimerA
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA 计数值 清零，以使比较模式GPIO口输出低电平，保护蜂鸣器	
	}
	//Timer32 处于 单次模式 无需 停止
	Beep_p_currentNote++;
	g_isPlayingNote = false;
	
	ReadNote(&Beep_p_currentNote);

}





