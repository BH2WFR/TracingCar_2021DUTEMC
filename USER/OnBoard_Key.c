#include "OnBoard_Key.h" 
#include "LED.h" 

//public

//不断移位放入按钮状态，周期为定时器中断频率1ms，按下为1（已经在中断函数读取时取反），后面通过这个消抖
uint32_t OnBoard_Key1_buffer = 0;
uint32_t OnBoard_Key2_buffer = 0;

static bool is_key_initialized = false;


bool OnBoard_Key1_pressedFlag = false;
bool OnBoard_Key2_pressedFlag = false;


//========================================

const Timer_A_UpModeConfig TimerA3_upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source == 12MHz(HFXT-48Mhz, /4)
        TIMER_A_CLOCKSOURCE_DIVIDER_12,         // SMCLK/12 = 1MHz
        1000 - 1,                          			// 1M/1000    1kHz, 1ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

__weak void OnBoard_Key1_Press(void)//按下按钮
{
	__NOP();
}

__weak void OnBoard_Key1_KeyUp(void)//松开按钮
{
	__NOP();
}

__weak void OnBoard_Key2_Press(void)
{
	__NOP();
}

__weak void OnBoard_Key2_KeyUp(void)
{
	__NOP();
}

//main()死循环里面要查询一次
void OnBoard_Key_Detect(void)
{
	if(is_key_initialized == false)
	{
		return;
	}else{
		//检验按钮是不是抖动
		if((OnBoard_Key1_pressedFlag == false) && ((OnBoard_Key1_buffer & 0x000000FF) == 0x000000FF)){
			OnBoard_Key1_pressedFlag = true;
			OnBoard_Key1_Press();

		}else if ((OnBoard_Key1_pressedFlag == true) && ((OnBoard_Key1_buffer & 0x000000FF) == 0x00000000)){
			OnBoard_Key1_pressedFlag = false;		
			OnBoard_Key1_KeyUp();	
		}
		
		if((OnBoard_Key2_pressedFlag == false) && ((OnBoard_Key2_buffer & 0x000000FF) == 0x000000FF)){
			OnBoard_Key2_pressedFlag = true;
			OnBoard_Key2_Press();

		}else if ((OnBoard_Key2_pressedFlag == true) && ((OnBoard_Key2_buffer & 0x000000FF) == 0x00000000)){
			OnBoard_Key2_pressedFlag = false;		
			OnBoard_Key2_KeyUp();	
		}
	}
}

void OnBoard_Key_init(void)
{
	MAP_GPIO_setAsInputPinWithPullUpResistor(ONBOARD_KEY1_PORTPIN);
	MAP_GPIO_setAsInputPinWithPullUpResistor(ONBOARD_KEY2_PORTPIN);
	MAP_Timer_A_configureUpMode(TIMER_A3_BASE, &TimerA3_upConfig);
	MAP_Timer_A_enableInterrupt(TIMER_A3_BASE);
	MAP_Interrupt_enableInterrupt(INT_TA3_0);
	MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);
	is_key_initialized = true;
}

void TA3_0_IRQHandler(void)
{
	MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
	//key1
	
	//LED1_toggle();

	OnBoard_Key1_buffer <<= 1; 
	OnBoard_Key1_buffer |= (! MAP_GPIO_getInputPinValue(ONBOARD_KEY1_PORTPIN));

	//key2

	OnBoard_Key2_buffer <<= 1;
	OnBoard_Key2_buffer |= (! MAP_GPIO_getInputPinValue(ONBOARD_KEY2_PORTPIN));

	
    
}
