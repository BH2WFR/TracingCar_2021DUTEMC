#include "Motor.h"
#include "LED.h"
#include "TracingSensor.h"
// #include "Mode\Control_Car.h"
// #include "Interface.h"
/*	
	

	
*/


//========================== 私有宏定义 Private Macros ===========================

// #define 	PID_KP_DEFAULT					0.1f
// #define 	PID_KI_DEFAULT					0.1f
// #define 	PID_KD_DEFAULT					0.01f		// PID算法的 初始系数

//#define		MOTOR_DUTY_CYCLE_COEFFICIENT	(MOTOR_TIMER_LOADVALUE / MOTOR_MAX_FORCE)
//







//===================== 公有全局变量 Public Global Variables =====================

Motor_Typedef Motor[MOTOR_AMOUNT] = {0};	// （重要）电机配置结构体，内含 PID 结构体，注意是个数组
uint64_t PID_count = 0;


// int16_t Motor_PWM_CCRx_register[MOTOR_AMOUNT][2] = {
// 		[0][0] = TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_A_CCRx],
// 		[0][1] = TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_B_CCRx],
// };




//==================== 私有全局变量 Private Global Variables =====================

//Attention： 以下结构体初始化方式为 C99 语法
//===for PID
const Timer_A_UpModeConfig TimerA_PID_upConfig = {
	.clockSource 					= TIMER_A_CLOCKSOURCE_SMCLK,	    // SMCLK Clock Source == 12MHz(HFXT-48Mhz, /4)
	.clockSourceDivider 			= TIMER_A_CLOCKSOURCE_DIVIDER_12,	// SMCLK/ = 
	.timerPeriod 					= 40000 - 1,						// Hz/    kHz, 10ms
	.timerInterruptEnable_TAIE 		= TIMER_A_TAIE_INTERRUPT_DISABLE,	// Disable Timer interrupt
	.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,	// Enable CCR0 interrupt
	.timerClear 					= TIMER_A_DO_CLEAR					// Clear value
};



//===for Motor 0/1 drive
const Timer_A_UpModeConfig TimerA2_upConfig = {
	.clockSource 					= TIMER_A_CLOCKSOURCE_SMCLK,	    // SMCLK Clock Source == 12MHz(HFXT-48Mhz, /4)
	.clockSourceDivider 			= TIMER_A_CLOCKSOURCE_DIVIDER_32,	// SMCLK/64 = 187.5kHz
	.timerPeriod 					= MOTOR_TIMER_CCR0_LOADVALUE,			// 500kHz/100    5kHz, 0.2ms
	.timerInterruptEnable_TAIE 		= TIMER_A_TAIE_INTERRUPT_DISABLE,	// Disable Timer interrupt
	.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,	// Disable CCR0 interrupt
	.timerClear 					= TIMER_A_DO_CLEAR					// Clear value
};

	//Motor0_PWM_A
Timer_A_CompareModeConfig compareConfig_PWM2_1 = {
	.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_1,		// Use CCR1
	.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,	// Disable CCR interrupt
	.compareOutputMode 		= TIMER_A_OUTPUTMODE_RESET_SET,				//reset-set
	.compareValue 			= MOTOR_TIMER_PWM_DEFAULT_CCR,				//default value of CCR1
};
	//Motor0_PWM_B
Timer_A_CompareModeConfig compareConfig_PWM2_2 = {
	.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_2,		// Use CCR2
	.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,	// Disable CCR interrupt
	.compareOutputMode 		= TIMER_A_OUTPUTMODE_RESET_SET,				//reset-set
	.compareValue 			= MOTOR_TIMER_PWM_DEFAULT_CCR,				//default value of CCR2                             
};
	//Motor1_PWM_A
Timer_A_CompareModeConfig compareConfig_PWM2_3 = {
	.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_3,		// Use CCR3
	.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,	// Disable CCR interrupt
	.compareOutputMode 		= TIMER_A_OUTPUTMODE_RESET_SET,				//reset-set
	.compareValue 			= MOTOR_TIMER_PWM_DEFAULT_CCR,				//default value of CCR3
};
	//Motor1_PWM_B
Timer_A_CompareModeConfig compareConfig_PWM2_4 = {
	.compareRegister 		= TIMER_A_CAPTURECOMPARE_REGISTER_4,		// Use CCR4
	.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,	// Disable CCR interrupt
	.compareOutputMode 		= TIMER_A_OUTPUTMODE_RESET_SET,				//reset-set
	.compareValue 			= MOTOR_TIMER_PWM_DEFAULT_CCR,				//default value of CCR4                             
};


//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

/**
 * @brief	私有函数	输入电机力度（占空比）输出 CCRx 寄存器装载初值
 * @param	force[in]	输入的力度，范围：(-MOTOR_MAX_FORCE, +MOTOR_MAX_FORCE)
 * @return	CCRx 寄存器装载初值
 * @note	该私有函数不会改变电机状态，也不会设置电机方向！
 * 			由 \b Motor_SetForce(#,#) 函数调用*/
static inline uint16_t GetCCRvalue(int16_t force)
{
	//建议打开代码优化，
	uint16_t output = (uint16_t)( abs(force) * (MOTOR_TIMER_CCR0_LOADVALUE / MOTOR_MAX_FORCE) );
	return output;
}



static void GoStep(uint8_t motorSelection)
{
	//==== 防止脑残
	if(Motor[motorSelection].isInSteppingMode == false){
		return;
	}
	
	//==== 如果 达到了 计数步数
	if(		Motor[motorSelection].stepModeDir == 1 && (Motor[motorSelection].stepCount > Motor[motorSelection].stepTarget)
		||  Motor[motorSelection].stepModeDir == 0 && (Motor[motorSelection].stepCount < Motor[motorSelection].stepTarget)){
		
		//==== 关闭 记步模式
		Motor[motorSelection].isInSteppingMode = false;
		
		//==== 电机 恢复 记步模式之前的转速
		//Motor[motorSelection].PID.setPoint = Motor[motorSelection].speedBeforeStepMode;
		Motor[motorSelection].PID.setPoint = 0;
		
	}else{
		
		//==== 设置速度
		Motor[motorSelection].PID.setPoint = Motor[motorSelection].steppingModeSpeed;
	}
	
}

/**
 * @brief	PID 清除 过秤值
 * @param	p_PID_struct[in|out] 结构体地址
 * @return	none
 * @note	*/
void PID_Clear(volatile PID_Typedef* p_PID_struct)
{
	p_PID_struct->outputValue = 0;
	p_PID_struct->setPoint = 0;
	p_PID_struct->lastError = 0;
	p_PID_struct->prevError = 0;
	
}




/**
 * @brief		PID 计算 \b 禁止调用 
 * @param	p_PID_struct[in|out] 结构体地址
 * @param	currentValue[in]	当前的输入量	
 * @return	pid 计算后 输出值 
 * @note	该函数 由 PID 定时器中断函数 调用， \b 严禁其他的调用方法	
 * 			读取 全局变量 
 * 			 \b 设定目标量 请 \b 自行更改 结构体 \b p_PID_struct->setPoint 中的参数 */
float PID_Calc(volatile PID_Typedef* p_PID_struct, float currentValue)
{
	if(p_PID_struct == NULL){
		return 666;
	}
	
	//float output;
	float error;
	
	//==== 计算
	error =  currentValue - p_PID_struct->setPoint;
	
	if(abs(p_PID_struct->outputValue) >= 10000){
		p_PID_struct->outputValue = 0;
	}
	
	p_PID_struct->outputValue +=  (p_PID_struct->A * error )
								 +(p_PID_struct->B * p_PID_struct->lastError)
							 	 +(p_PID_struct->C * p_PID_struct->prevError);
	
	//==== 限幅							  
	if(p_PID_struct->limiting.enable == true){
		if(p_PID_struct->outputValue > p_PID_struct->limiting.max){
			p_PID_struct->outputValue = p_PID_struct->limiting.max;
		}else if(p_PID_struct->outputValue < p_PID_struct->limiting.min){
			p_PID_struct->outputValue = p_PID_struct->limiting.min;
		}
	}
	
		
	p_PID_struct->prevError = p_PID_struct->lastError; //== 本次变上次, 轮换
	p_PID_struct->lastError = error;
	
	
	//==== 输出
	//p_PID_struct->outputValue = output;
	return p_PID_struct->outputValue;
}

/**
 * @brief	私有函数	\b 禁止调用 根据编码器所读取的数据计算 PID，且获取速度
 * @param	index[in]	
 * @return	none 
 * @note	该函数 由 PID 定时器中断函数 调用， \b 严禁其他的调用方法	
 * 			读取 全局变量 */
static void PID_Motor_Control(uint8_t index)
{
	float speed;
	//float force;
	
	//==== 根据 编码器中断 获取的 时间间隔 计算 速度
	
	if(Motor[index].isInterruptedDuringPIDperiod == false){
		// 如果 两次 PID 中断期间 没有来 编码器中断，则速度认为是 0（电机停转）
		speed = 0;	
			
	}else{	//== 如果电机在旋转（两次 PID 中断期间 编码器中断 被 触发过
		//== 如果电机在旋转，在这里计算速度（算出来的值是正数，不考虑方向）
		speed = MOTOR_ENCODER_SPEED_COEFFICIENT / Motor[index].durationTime;
		
		//== 读取结构体中电机的方向，并将速度乘以 1 或 -1
		if(Motor[index].detectedDirection == MOTOR_DIRECTION_BACKWARD){
			speed *= -1;	// speed ^= 0x80000000
		}
		
	}
	
	Motor[index].detectedSpeed = speed;	// 写入 检测到的速度
	
	
	
	//==== 根据 设定的 目标值 计算 PID
	if(Motor[index].PID.enableSign == 1){	// 是否使能了电机的 PID 功能？
		//=== 调用函数 计算 PID
		
		PID_Calc(&Motor[index].PID, speed);	// 计算 PID
		
		volatile int16_t pidOutputVal = (int16_t)Motor[index].PID.outputValue;
		
			//==== force 限幅
		// if(pidOutputVal < -MOTOR_MAX_FORCE){
		// 	pidOutputVal = -MOTOR_MAX_FORCE;
		// }else if (pidOutputVal > MOTOR_MAX_FORCE){
		// 	pidOutputVal = MOTOR_MAX_FORCE;
		// }
		
		
		Motor_SetForce(index, MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT, pidOutputVal);	// 根据 PID 输出值 设置 电机
		
	}else{	// Motor[index].PID.enableSign == 0,  直接输出占空比
	
		//== 需要在程序其他地方手动赋值 Motor[index].force
		//Motor_SetForce(index, MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT, 0);	// 将 Motor[index].force 的值作为占空比直接输出
		
	}
		
	Motor[index].isInterruptedDuringPIDperiod = false;	// 清零 标志位，在编码器中断 中 置位
	
}

/**
 * @brief	私有函数	\b 禁止调用 读取 编码器，获取时间间隔（ \b 不计算速度！ ）
 * @param	index[in]	
 * @return	none 
 * @note	该函数 由 编码器 中断处理函数 调用， \b 严禁其他的调用方法	
 * 			读取 全局变量 */
static void Motor_Encoder_Read(uint8_t index)
{
	//==== 抓取 定时器计数值
	uint32_t timerValue = Timer32_getValue(MOTOR_ENCODER_TIMER_BASE);
	
	//==== 计算 时间间隔，写入到结构体中
	uint32_t duration = Motor[index].lastTimer32Value - timerValue;
	Motor[index].durationTime	  = duration;
	Motor[index].lastTimer32Value = timerValue;	// 本次 定时器计数值 存档
	
	
	
	
	//==== 获取 电机方向，看 B 相 的 电平，然后 写入
	bool  encoder_B_Status;
	switch(index){
		case 0:
			encoder_B_Status = GPIO_getInputPinValue(MOTOR_0_ENCODER_B_PORT_PIN);
			break;
		case 1:
			encoder_B_Status = GPIO_getInputPinValue(MOTOR_1_ENCODER_B_PORT_PIN);			
			break;
	}
	Motor[index].detectedDirection = ! encoder_B_Status; // 写入
	
	//==== 记步（记录距离）
	if(encoder_B_Status == true){
		Motor[index].stepCount--;
	}else{
		Motor[index].stepCount++;
	}
	
	
	
	
	//==== 滤波
	// Motor[index]. durationTimeBuffer[Motor[index]. durationTimeBufferIndex]  = timerValue;
	// RollUpRange(Motor[index]. durationTimeBufferIndex, index, MOTOR_ENCODER_DURATION_BUFFER_AMOUNT);
	

	//==== 标志位 来了中断，在 PID 里面 清零，用于 检测 电机是否停转
	Motor[index].isInterruptedDuringPIDperiod = true;	// clear in PID funcion, for detecting if the speed refreshed 	
}








//========================= 公有函数  Public Functions ===========================



/**
 * @brief	设置电机的 力度 和 方向
 * @param	motorSelection[in]	选择电机，编号：
 * @param	inputMode[in]		\b 读取force值的模式，
 * 								当 inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT \b (false) 时
 * 									第三个参数 force 无效，本函数从 结构体 Motor[motorSelection].force	中读取值
 * 								当 inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT \b (true) 时
 * 									第三个参数 force 的值 写入到 Motor[motorSelection].force 中
 * @param	force[in]	输入的力度，
 * 								当 inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT 时
 * 									本参数无效，可赋值任何值
 * 								当 inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT 时
 * 									本参数为 要 输入的 force 值，范围：( \b -MOTOR_MAX_FORCE, +MOTOR_MAX_FORCE)， \b >0_正转，<0_反转
 * 								
 * @return	none
 * @note	调用私有函数：GetCCRvalue(#)	
 * 			切换方向时 \b 只动 CCRx 寄存器值，不动 GPIO 相关寄存器
 * 			该函数 读取/写入 Motor[x] 结构体全局变量 */
int16_t Motor_SetForce(uint8_t motorSelection, bool inputMode, int16_t force)
{
	uint16_t CCRloadValue;
	
	uint16_t CCR_A = 0;
	uint16_t CCR_B = 0;
	
	//==== force 限幅
	if(force < -MOTOR_MAX_FORCE){
		force = -MOTOR_MAX_FORCE;
	}else if (force > MOTOR_MAX_FORCE){
		force = MOTOR_MAX_FORCE;
	}
	
	
	
	//==== 同步 输入参数 与 结构体中的 force 设置
	if(inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT){
		force = Motor[motorSelection].force;
	}else{	// inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT
		Motor[motorSelection].force = force;
	}
	
	//==== 计算 CCR 值
	CCRloadValue = GetCCRvalue(force);
	
	
	//==== 设置正反转 及 CCRx 值
	if(force > 0){	//	正转
		//Motor[motorSelection].setDirection = MOTOR_DIRECTION_FORWARD;
		
		CCR_A = CCRloadValue;
		CCR_B = 0;
		
	}else{	//force < 0	反转
		//Motor[motorSelection].setDirection = MOTOR_DIRECTION_BACKWARD;
		
		CCR_A = 0;
		CCR_B = CCRloadValue;			
		
	}
	
	
	
	//===== 选择电机，寄存器赋值
	switch(motorSelection){
		case 0:
			TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_A_CCRx] = CCR_A;
			TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_B_CCRx] = CCR_B;
			break;
			
		case 1:
			TIMER_A_CMSIS(MOTOR_1_PWM_TIMER_BASE)->CCR[MOTOR_1_PWM_A_CCRx] = CCR_A;
			TIMER_A_CMSIS(MOTOR_1_PWM_TIMER_BASE)->CCR[MOTOR_1_PWM_B_CCRx] = CCR_B;					
			break;
	}		
	
	
	
	
	return force;
}



/**
 * @brief	输入 结构体地址 和 预期值，改变 setPoint
 * @param	p_PID_struct[in|out]	要进行操作的 PID 结构体 地址	
 * @param	point[in]		设定的 预期值	
 * @return	none 
 * @note	*/
void PID_SetPoint(PID_Typedef* p_PID_struct, float point)
{
	if(p_PID_struct == NULL){	// 防止脑残
		return;
	}
	
	p_PID_struct->setPoint = point;
}


/**
 * @brief	输入 结构体地址 和 Kp, Ki, Kd, 计算出增量式 pid 算法的三个参数 A, B, C，并输出到结构体里面
 * @param	p_PID_struct[in|out]	要进行操作的 PID 结构体 地址	
 * @param	Kp[in]		要设定的 PID 三个系数
 * @param	Ki[in]		
 * @param	Kd[in]	
 * @param 	inverse		是否将三个参数取负数
 * @param   is_reset	是否清空 	lastError， prevError，outputValue
 * @return	none 
 * @note	*/
void PID_Set(PID_Typedef* p_PID_struct, float Kp, float Ki, float Kd, bool inverse, bool is_reset)
{
	if(p_PID_struct == NULL){	// 防止脑残
		return;
	}
	
	p_PID_struct->A = (Kp + Ki + Kd);
	p_PID_struct->B = (-Kp - 2*Kd);
	p_PID_struct->C = (Kd);
	
	if(inverse == true){
		p_PID_struct->A = -p_PID_struct->A;
		p_PID_struct->B = -p_PID_struct->B;
		p_PID_struct->C = -p_PID_struct->C;
	}
	
	if(is_reset == true){
		PID_Clear(p_PID_struct);
	}
	
}

/**
 * @brief	\b 主循环调用 电机记步
 * @param	none
 * @return	none  
 * @note	*/
void Motor_GoStep_circulate(void)
{
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		if(Motor[i].isInSteppingMode == true){
			
			GoStep(i);
		}
	}
	
	
}





/**
 * @brief	电机记步
 * @param	none 
 * @return	none  
 * @note	要求在主循环调用  Motor_GoStep_circulate(void) 函数
 * 			运行方向取决于 step 的 符号，与 speed 的 符号 无关*/
void Motor_GoStep(uint8_t motorSelection, int16_t speed, int64_t step)
{
	if(Motor[motorSelection].isInSteppingMode == true){
		return;
	}
	
	//==== 运动方向取决于 step 的 符号
	if((speed > 0 && step < 0) || (speed < 0 && step > 0)){
		speed = -speed;
	}
	
	if(step > 0){
		Motor[motorSelection].stepModeDir = 1;
	}else{
		Motor[motorSelection].stepModeDir = 0;
	}
	
	
	Motor[motorSelection].isInSteppingMode = true;
	Motor[motorSelection].stepTarget = Motor[motorSelection].stepCount + step;
	Motor[motorSelection].steppingModeSpeed = speed;
	Motor[motorSelection].speedBeforeStepMode = Motor[motorSelection].PID.setPoint;
	
	
	
}


/**
 * @brief	电机 PID 清零
 * @param	none
 * @return	none  
 * @note	请*/
void Motor_ClearPID(void)
{
	for(uintptr_t i = 0; i < MOTOR_AMOUNT; i++){
		PID_Clear(&(Motor[i].PID));
	}		
	
}



/**
 * @brief	电机模块 \b 暂停运行， 关闭 PWM 输出定时器，不影响 PID 定时器中断
 * @param	none
 * @return	none  
 * @note	请在模式文件的 halt() 函数中 \b 自行调用 这个函数*/
void Motor_Stop(void)
{
	//==== 电机驱动 IO 口 取消复用
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_DRIVE_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_DRIVE_B_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_DRIVE_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_DRIVE_B_PORT_PIN);	
	
	
	//==== 停止 电机 PWM 定时器
	Timer_A_stopTimer(MOTOR_0_PWM_TIMER_BASE);	// include motor 1
	
	
	//==== 电机力度（PID 目标值） 设置为 0
	Motor_ClearPID();		
	
	
	//==== 标志位 关闭
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].isStarted = false;
	}	
}



/**
 * @brief	彻底禁用 电机、电机编码器、 和 PID 定时器
 * @param	none
 * @return	none  
 * @note	本函数已经内部调用 Motor_Stop(), 无需再次调用它*/
void Motor_Disable(void)
{
	
	//==== 电机停转，PWM定时器关闭，GPIO关闭复用
	Motor_Stop();
	

	
	//==== 关闭 PID 定时器 和 中断
	Timer_A_stopTimer(MOTOR_PID_TIMER_BASE);
	Timer_A_disableInterrupt(MOTOR_PID_TIMER_BASE);
	
	Timer32_haltTimer(MOTOR_ENCODER_TIMER_BASE);
	Timer32_disableInterrupt(MOTOR_ENCODER_TIMER_BASE);
	
	
	//==== 关闭 编码器 GPIO 中断
	GPIO_disableInterrupt(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);		// 先 关掉 其他 中断
	GPIO_clearInterruptFlag(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);


	
	//==== 清空结构体
	//memset(&Motor, 0, (MOTOR_AMOUNT * sizeof(Motor_Typedef))  );
		// 以后改用 malloc/free
	
	
	//==== 标志位 关闭
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].enable = false;
	}
	
}

/**
 * @brief	电机模块，电机 GPIO 开启复用，且开启 PWM 定时器，即 电机开始运转，不开启 PID 定时器
 * @param	none
 * @return	none  
 * @note	请在模式文件的 init() 函数中 \b 自行调用 这个函数*/
void Motor_Start(void)
{
	//==== 调用这个前必须初始化！
	if(Motor[0].enable == false){
		return;
	}
	
	//==== 延时
	
	
	//==== 电机力度（PID 目标值） 设置为 0
	Motor_ClearPID();		
	
								
	//==== 启动 电机控制PWM 信号 定时器 
	Timer_A_startCounter(MOTOR_0_PWM_TIMER_BASE, TIMER_A_UP_MODE);	// together with motor_1

	
	//==== 电机 IO 口 开始 复用
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_0_DRIVE_A_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_1
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_0_DRIVE_B_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_2
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_1_DRIVE_A_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_3
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_1_DRIVE_B_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_4									
	
	//==== 延时
	
			
	//==== 信号位										
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].isStarted = true;
	}											
																					
}

/**
 * @brief	电机模块 初始化，含 编码器 和 PID，并让 PID 定时器 开始中断
 * @param	isStartMotor	是否启动电机， \b Motor_Start(void) ,  \b 启动电机_GPIO_复用！
 * @return	none  
 * @note	  */
void Motor_init(bool isStartMotor)
{
	if(Motor[0].enable == true){
		return;
	}
	
	//==== 清空结构体
	memset((void*)(&Motor), 0, (MOTOR_AMOUNT * sizeof(Motor_Typedef))  );
		
	
	
	//==== 电机驱动 GPIO 先 暂时 关闭复用
	Motor_Stop();
	
	
	//==== 编码器 GPIO	初始化 输入模式									
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_ENCODER_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_ENCODER_B_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_ENCODER_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_ENCODER_B_PORT_PIN);

		
	//==== 电机 PWM 定时器，比较模式，不启动，关闭中断
	Timer_A_configureUpMode(MOTOR_0_PWM_TIMER_BASE, &TimerA2_upConfig);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_1);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_2);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_3);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_4);
	
	Timer_A_disableInterrupt(MOTOR_0_PWM_TIMER_BASE);	// 关闭 PWM 定时器的中断，因为不需要
	
	
	
	//==== 编码器 GPIO 中断 
	
	//P6->IE = 0x00;
	GPIO_disableInterrupt(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);		// 先 关掉 其他 中断
	GPIO_clearInterruptFlag(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);
	// MOTOR_1 的 编码器 与 MOTOR_0 是 相同的 PORT
	GPIO_interruptEdgeSelect(MOTOR_0_ENCODER_A_PORT_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
	GPIO_interruptEdgeSelect(MOTOR_1_ENCODER_A_PORT_PIN, GPIO_LOW_TO_HIGH_TRANSITION);
	
	GPIO_enableInterrupt(MOTOR_0_ENCODER_A_PORT_PIN);		//P6->IE
	GPIO_enableInterrupt(MOTOR_1_ENCODER_A_PORT_PIN);	
	
	
	Interrupt_enableInterrupt(MOTOR_ENCODER_INTERRUPT);
	//P6->IFG = 0x00;
	
	
	//==== PID initialation
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		PID_Set(&Motor[i].PID, MOTOR_PID_Kp_DEFAULT, MOTOR_PID_Ki_DEFAULT, MOTOR_PID_Kd_DEFUALT, true, true);
		
		#ifndef __MODE_MOTOR_TEST_H__
			//===== 默认使能PID
			Motor[i].PID.enableSign = true;
		#endif
	}
	
	//==== 配置 PID 定时器中断
	Timer_A_configureUpMode(MOTOR_PID_TIMER_BASE, &TimerA_PID_upConfig);	
	Timer_A_enableInterrupt(MOTOR_PID_TIMER_BASE);
	Interrupt_enableInterrupt(MOTOR_PID_TIMER_INTERRUPT);
	
	
	//==== 打开 PID 定时器
	Timer_A_startCounter(MOTOR_PID_TIMER_BASE, TIMER_A_UP_MODE);
	Timer32_startTimer(MOTOR_ENCODER_TIMER_BASE, false);
		
	Timer32_initModule(MOTOR_ENCODER_TIMER_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
	Timer32_setCount(MOTOR_ENCODER_TIMER_BASE, 0xFFFFFFFF);
	Interrupt_enableInterrupt(MOTOR_ENCODER_TIMER_INTERRUPT);
	Timer32_enableInterrupt(MOTOR_ENCODER_TIMER_BASE);
	
	
	//==== 信号位
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].enable = true;
	}
	
	
}






//======================== 中断函数 Interrupt Handlers ===========================
/**
 * @brief  GPIO_PORT_6  中断  暂时只用单相  P6.2 上升沿 中断
 * 			主要功能:  */
void MOTOR_ENCODER_IRQHandler(void)
{
	uint32_t status = GPIO_getInterruptStatus(MOTOR_ENCODER_INT_PORTPIN);	// == Motor_0_pin | Motor_1_pin
	uint8_t  index;
	
	
	GPIO_clearInterruptFlag(MOTOR_ENCODER_INT_PORTPIN);
	
	
	//==== 判断是 哪个编码器 中断了 (有可能 多个编码器 同时中断！不能用 else if 结构)，然后 调用 私有函数 计算 时间间隔
	if(status & MOTOR_0_ENCODER_A_PIN){
		index = 0;
		Motor_Encoder_Read(index);	// 计算
	}
	if(status & MOTOR_1_ENCODER_A_PIN){	// 不能使用 else if
		index = 1;
		Motor_Encoder_Read(index);	// 计算
	}
	
}


/**
 * @brief  PID 中断 
 * 			主要功能:  */
void MOTOR_PID_TIMER_IRQHandler(void)
{
	Timer_A_clearCaptureCompareInterrupt(MOTOR_PID_TIMER_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);	// 清除中断
	//static uint64_t count = 0;	// 中断次数计数，可能是没用的一个变量 (逃
	
	PID_count++;
	
	//===== 电机
	for(uint8_t i = 0; i < MOTOR_AMOUNT; i++){
		PID_Motor_Control(i);	// 读取 电机速度，计算 pid
	}
	
	
	//==== 除电机外的 PID 计算
	// 样例： PID_Calc(pid_struct, );
	#ifdef __TRACING_SENSOR_H__
		
		if(Sensor.enabled == true){	// 循迹传感器
			
			Sensor_ReadStatus();
			
		}
		
	
	#endif
	
	Motor_GoStep_circulate();
	
	
	
}


/**
 * @brief  timer32_1 中断
 * 			主要功能:  */
void MOTOR_ENCODER_TIMER_IRQHandler(void)	// Timer32_1 (int2)
{
	Timer32_clearInterruptFlag(MOTOR_ENCODER_TIMER_BASE);
	
	
}





