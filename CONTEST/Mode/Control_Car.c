#include "Mode\Control_Car.h"
#include "Interface.h"

#include "LED.h"
#include "OLED_12864.h"
#include "Remocon_Keys.h"	// 借用其中的定时器，判断是否长时间没有来串口信号
#include "UART0.h"
#include "Motor.h"
#include "TracingSensor.h"

/*	========================按键GPIO说明==================


	
	
*/
//========================== 私有宏定义 Private Macros ===========================





//===================== 公有全局变量 Public Global Variables =====================
ControlCar_typedef  ControlCar = {0};

//==================== 私有全局变量 Private Global Variables =====================

//bool g_enable = false;



//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============






//========================= 公有函数  Public Functions ===========================



void ControlCar_Key_EnableMotor(void)
{
	if(ControlCar.enable == true){ 
		ControlCar_StopMotor();	// 这里已经 false 掉 标志位了 
		
	}else{	// ControlCar.enable == false;
		ControlCar_StartMotor();
		
	}	
}

/**
 * @brief  由 \b 定时器中断 调用！， 读取 定时器
 * @param  none  
 * @return none  
 * @note	*/
void ControlCar_TimerInterrupt(uint32_t timerCount)
{
	
	//ControlCar.noSignal.timerInterruptCount = timerCount;
	
	
	if(ControlCar.noSignal.sign == false){
		
		ControlCar.noSignal.timerInterruptCount++;	// 计数 自增
		
		//==== 如果 长时间没有读取到 串口
		if(ControlCar.noSignal.timerInterruptCount == ControlCar.noSignal.thresholdCount){
			ControlCar.noSignal.sign = true;	// 
			ControlCar_StopMotor();				// 直接关闭电机
			
		}
		
	}else{	// ControlCar.noSignal.sign == true
		
		
		
	}
	
	
	
}


/**
 * @brief  由 \b 串口接受中断 调用！
 * @param  none
 * @return none  
 * @note	*/
void ControlCar_AnalysisReceivedDataFrame(uint8_t RxBuf)
{
	if(ControlCar.enable == false){
		return;		// 防呆设计
	}
	
	ControlCar.noSignal.sign = false;				// 读取到了串口！
	ControlCar.noSignal.timerInterruptCount = 0;	// 清空 计数
	
	
	if(RxBuf == CONTROL_CAR_DATA_FIRST_FRAME){
		ControlCar.received.frameCount = 0;
		
	}else{	// 接到的 不是 头帧
	
		ControlCar.received.frameCount++;	// 自增
		
		// 接到了 第一个参数
		if(ControlCar.received.frameCount == 1){
			ControlCar.received.firstParameter = RxBuf;
			
		// 接到了 第二个参数
		}else if (ControlCar.received.frameCount == 2){
			ControlCar.received.secondParameter = RxBuf;
		
		// 接到了 后面
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
	//==== 重置结构体
	ControlCar_ResetStruct();
	
	//==== 使能标志位
	ControlCar.enable = true;
	ControlCar.noSignal.sign = false;
	
	//==== 启动 电机 GPIO 复用 和 PWM 定时器
	Motor_Start();
	LED2_switch(LED2_COLOUR_YELLOW);
}




//===============================


void ControlCar_ResetStruct(void)
{
	//==== 清空结构体
	memset(&ControlCar, 0, sizeof(ControlCar));
	
	//==== 初始化设置参数（不能赋值使能位）
	ControlCar.noSignal.thresholdCount = CONTROL_CAR_NO_SIGNAL_TIMER_COUNT_THRESHOLD;	
	
	
}


/**
 * @brief  默认模式_初始化
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void ControlCar_init(void)
{
	
	// //==== 清空结构体
	 ControlCar_ResetStruct();
	
	//==== 防呆设计
	ControlCar_StopMotor();
	
}

/**
 * @brief  默认模式_主函数循环调用_每 \b 200 周期调用一次
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void ControlCar_Cycle(void)
{
	static uint64_t count = 0;
	
	//char str[8] = {0};
	
	count++;
	
	if(count % 1000u == 0){
		//显示标题
		OLED_PrintString(Interface_modeConfig.name, 0, 0, OLED_TYPE_6X8, true);	
		
			//显示内容
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
 * @brief  默认模式_停止，逆向初始化
 * @param  none
 * @return none  
 * @note	不可直接调用该函数，要通过interface.c 里面结构体中的函数指针来调用 */
void ControlCar_Halt(void)
{
	
	ControlCar_StopMotor();
	
	
}

//======================== 中断函数 Interrupt Handlers ===========================




