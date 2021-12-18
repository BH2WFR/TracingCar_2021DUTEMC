#include "TracingSensor.h"
#include "Motor.h"

#include "Interface.h"


//========================== 私有宏定义 Private Macros ===========================


//===================== 公有全局变量 Public Global Variables =====================


Sensors_typedef Sensor = {0};





/*
	编码器：	  0		  1		  2		  3	  |	  4		  5		  6		  7
	位置(mm)：
	端口		P4.1	P4.2	P4.3	P4.4	P4.5	P4.6	P4.7	P4.0		


*/
const uint8_t Sensor_GPIO_port[SENSOR_AMOUNT] = {	// 各传感器探头的 GPIO 引脚
	//== left
	[0] = GPIO_PORT_P4,
	[1] = GPIO_PORT_P4,
	[2] = GPIO_PORT_P4,
	[3] = GPIO_PORT_P4,
	//== middle
	[4] = GPIO_PORT_P4,
	[5] = GPIO_PORT_P4,
	[6] = GPIO_PORT_P4,
	[7] = GPIO_PORT_P4,
	//== right
};

const uint8_t Sensor_GPIO_pin[SENSOR_AMOUNT] = {	// 各传感器探头的 GPIO 引脚
	//== left
	[0] = GPIO_PIN0,
	[1] = GPIO_PIN7,
	[2] = GPIO_PIN6,
	[3] = GPIO_PIN5,
	//== middle
	[4] = GPIO_PIN4,
	[5] = GPIO_PIN3,
	[6] = GPIO_PIN2,
	[7] = GPIO_PIN1,
	//== right
};

const float Sensor_location[SENSOR_AMOUNT] = {	// 各 灰度/红外 传感器 探头 的 物理位置，单位：mm
	//== left
	[0] = +67.0f,
	[1] = +44.0f,
	[2] = +24.0f,
	[3] = +7.5f,
	//== middle
	[4] = -7.5f,
	[5] = -24.0f,
	[6] = -44.0f,
	[7] = -67.0f,
	//== right
};

//==================== 私有全局变量 Private Global Variables =====================



//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============


/**
 * @brief	
 * @param	offset[in]	传感器检测到的 小车位置 偏移量， >0: 右拐
 * @return	none  
 * @note	*/
__weak void TrackingLine(float offset)
{
	if(Sensor.enableTracing == false){
		return;
	}
	
	float dif_raw = 0;	// < 0 : turn left,   > 0 : turn right
	float dif_final = 0;
	float basicSpeed = 0;
	
	//==== 偏差：+44 mm（右） -> 速度：30 -> 系数：0.0113636f ---> 差速：+15（右）
	if(Sensor.features.trackingLine.fastMode.enable == false){
		// 慢速模式
		basicSpeed = Sensor.features.trackingLine.basicSpeed;
		
	}else{
		// 快速模式
		basicSpeed = Sensor.features.trackingLine.fastMode.basicSpeed;
	}
	
	//==== 获取 速度差值
	dif_raw =  offset * basicSpeed * Sensor.features.trackingLine.speedCoord;
	
	
	//==== PID 巡线
	if(Sensor.PID.enableSign == true){
		//==== 将 PID 运算值 放入 电机中
		PID_Calc(&(Sensor.PID), dif_raw); // 输入数据，计算 PID
		dif_final = Sensor.PID.outputValue;	// 根据 PID 输出 计算 两轮速度变化值
		
	}else{	// Sensor.PID.enableSign == false 未开启 PID 功能
		dif_final = dif_raw;	// 根据 输入值 直接 不使用 PID  计算 两轮速度变化值
		
	}	
	
	
	//==== 输出到 电机 PID 中
	#ifdef __MOTOR_H__
		if(Sensor.features.trackingLine.inverseDir == true){
			if(Motor[0].enable == true){
				Motor[0].PID.setPoint = basicSpeed - dif_final;
			}
			
			if(Motor[1].enable == true){
				Motor[1].PID.setPoint = basicSpeed + dif_final;	
			}		
			
		}else{ // Sensor.features.trackingLine.inverseDir == false
			if(Motor[0].enable == true){
				Motor[0].PID.setPoint = basicSpeed + dif_final;
			}
			
			if(Motor[1].enable == true){
				Motor[1].PID.setPoint = basicSpeed - dif_final;	
			}		
		}
	#endif
}

/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
__weak void GoCross(bool isFirst)
{
	static uint32_t count = 0;
	
	if(Sensor.features.crossRecognition.enable == false){
		return;
	}	
	
	//==== 如果是刚刚检测到十字路口，第一次执行这个函数
	if(isFirst == true){
		count = 0;
		Sensor.features.crossRecognition.isDoingCrossAction = true;
		
	}else{
		
	}
	
	
	//==== 盲走十字路口，并计时，检测是否到达指定时间
	if(count != Sensor.features.crossRecognition.length){	// 正在计时，盲走十字路口
		count++;
		
	}else{				// 计时结束，已经越过十字路口
		count = 0;	// 计数清零
		Sensor.features.crossRecognition.isDoingCrossAction = false; // 标志位清零
		
	}
	
	
	
	
	//Sensor.features.crossRecognition.isDetected_flag = false;
}

/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
__weak void GoLeft90Deg(bool isFirst)
{
	if(Sensor.features._90degRecognition.enable == false){
		return;
	}
	
	
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
__weak void GoRight90Deg(bool isFirst)
{
	if(Sensor.features._90degRecognition.enable == false){
		return;
	}
	
		
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
bool DetectCross(void)
{
	if(Sensor.features.crossRecognition.enable == false){
		return false;
	}
		
	if(Sensor.shelteredSensorsAmount >= SENSOR_CROSS_SHELTERED_THRESHOLD){	// 检查传感器被遮挡几个？
		//cross
		Sensor.features.crossRecognition.isDetected_flag = true;		// 开启标志位, 后面巡线使用
	
		if(Sensor.features.crossRecognition.Action != (void*)NULL ){
	 		Sensor.features.crossRecognition.Action(true);		// 进入处理函数，重新开始
		}
	
	}else{		// ==== 如果传感器遮挡个数没有一半，不认为到了十字路口
		Sensor.features.crossRecognition.isDetected_flag = false;		// 清零标志位, 后面巡线使用
		
	}
	

	
	
	return Sensor.features.crossRecognition.isDetected_flag;
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
bool Detect90Deg(void)
{
	if(Sensor.features._90degRecognition.enable == false){
		return false;
	}	
	
	//==== 未来再写
	
	return false;
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
bool TreatSensorStatus(void)
{
	bool val = false;
	
	// ==== 是否 正在处理十字路口 事件函数？
	if(Sensor.features.crossRecognition.isDoingCrossAction == true){	// 保证此时一定已经 enable == true 了
	 	if(Sensor.features.crossRecognition.Action != (void*)NULL ){
	 		Sensor.features.crossRecognition.Action(false);		// 进入处理函数
	 	}
		val = true;		
		 
		// 提前退出
		
		// --- 检测到正在执行 左转 90 度 事件函数
	}/*else if (Sensor.features._90degRecognition.isDoingTurnLeft90Action == true){
	 	if(Sensor.features._90degRecognition.ActionLeft != (void*)NULL ){
	 		Sensor.features._90degRecognition.ActionLeft(false);		// 进入处理函数
	 	}		
		val = true;	
		
		// --- 检测到正在执行 右转 90 度 事件函数
	}else if (Sensor.features._90degRecognition.isDoingTurnLeft90Action == true){
	 	if(Sensor.features._90degRecognition.ActionRight != (void*)NULL ){
	 		Sensor.features._90degRecognition.ActionRight(false);		// 进入处理函数
	 	}			
		val = true;
		
	}*/else{	// 不在正在执行那些事件函数，则检测是否遇到十字路口和直角转弯，如果没遇到那就巡线
		
		//==== 检查是不是十字路口，置位标志位
		if(Sensor.features.crossRecognition.enable == true){
			DetectCross();	// 赋值到标志位
		}		
		
		// //==== 检查是否遇到 90 度直角弯
		// if(Sensor.features._90degRecognition.enable == true){
		// 	Detect90Deg();	
		// }
		
		
		//==== 检查完毕，如果 不在执行事件函数 时 遇到了十字路口
		if(Sensor.features.crossRecognition.enable == true){		// 先确认是否使能 十字路口检测
			if(Sensor.features.crossRecognition.isDetected_flag == true){	// 再确认是否检测到了十字路口
				if(Sensor.features.crossRecognition.Action != (void*)NULL ){
					Sensor.features.crossRecognition.Action(true);	// 进入事件处理，然后不循迹
				}
				val = true;
			}

		}
		
		// if( Sensor.features._90degRecognition.enable == true){
		// 	if(Sensor.features._90degRecognition.isDetectedLeft90deg == true){
		// 		if(Sensor.features._90degRecognition.ActionLeft != (void*)NULL){
		// 			Sensor.features._90degRecognition.ActionLeft(true);
		// 		}
		//		val = true;
		// 	}else if (Sensor.features._90degRecognition.isDetectedRight90deg == true){
		// 		if(Sensor.features._90degRecognition.ActionRight != (void*)NULL){
		// 			Sensor.features._90degRecognition.ActionRight(true);
		// 		}
		//		val = true;
		// 	}	
			
		// }

	
	}	
	
	return val;	// val == true 时 不循迹
}

//========================= 公有函数  Public Functions ===========================


/**
 * @brief	读取灰度/红外传感器，并作出指定行为，包括：循线、过十字路口、过直角弯， \b 由_电机_PID_中断函数_定时调用
 * @param	none
 * @return	none  
 * @note	全自动完成*/
void Sensor_ReadStatus(void)
{
	if (Sensor.enabled == false){
		return;
	}
	
	Sensor.isReading_flag = true;
	
	
	//======= 开始读取，求遮挡传感器个数，求平均值

	bool val[SENSOR_AMOUNT] = {0};	// 临时变量，稍后写入结构体中
	//uint8_t val_bit = 0;
	uint8_t sheltered = 0;	// initialized to 0 !!!
	float loc_sum = 0.0f;	// 
	
	//Sensor.status_bit = 0x00;	// 记录数据（位），清零
	
		// 循环
	for(int i = 0; i < SENSOR_AMOUNT; i++){
			//GPIO_INPUT_PIN_HIGH == 0x01,  GPIO_INPUT_PIN_LOW == 0x00
			
		if( (Sensor.enable_sensor_0_and_7 == false)  &&  ( (i == 0) || (i == 7) )  ){	
			// i == 0 or 7, and disabled sensor 0 and 7
			val[i] = 0;		// obviously, not detected
			Sensor.status_array[i] = 0;
			//continue;
			
		}else{	// i == 1 to 6; or i == 0 or 7 and  sensor 0 and 7 are enabled
			
			val[i] = GPIO_getInputPinValue(Sensor_GPIO_port[i], Sensor_GPIO_pin[i]); // 读取 GPIO
			
			if(Sensor.colourInverse == true){	// 如果 颜色相反 则 取反
				val[i] =  ! val[i];
			}
			
			Sensor.status_array[i] = val[i];// 记录数据（布尔数组）
			
			if(val[i] == true){		// 如果第 i 个 传感器 被 遮挡了
				//val_bit |= ( 0x01 << i ) ;	// 记录数据（位），此变量要求 进入循环前 必须清零，循环结束后 写入结构体中
				sheltered++;	// 遮挡计数 +1
				loc_sum += Sensor_location[i];	// 取平均用
			}
		}
		
	}		
	
	Sensor.shelteredSensorsAmount = sheltered;	// 记录数据（位），写入 结构体
	

	if(sheltered == 0){
		Sensor.averagedLineLocation = 0.0f;
	}else{
		Sensor.averagedLineLocation = loc_sum / (float)sheltered;
	}		
	

	//Sensor.status_bit = var_bit;  
	
	//==== 处理刚才得到的数据
	
	volatile bool ifNotNeedTracking = TreatSensorStatus();	// 检测有没有直角弯和十字路口，或正在进行事件处理函数，如果有则调用
	
	//== 如果没有检测到 十字路口 和 直角弯
	if(ifNotNeedTracking == false){	
		if(Sensor.features.trackingLine.Action != (void*)NULL ){
			
			if(Sensor.enableTracing == true){
				Sensor.features.trackingLine.Action(Sensor.averagedLineLocation);	// 巡线
			}else{
				Sensor.features.trackingLine.Action(0.0f);						// 直走
			}
			
		}
		
	}else{
		return;
	}	
	
	
	Sensor.isReading_flag = false;
	return;
}

static void initStruct_Sensor(Sensors_typedef* p_sensor)
{
	//==== 初始化设置
	memset(p_sensor, 0, sizeof(Sensors_typedef));	// 清空结构体
	
	p_sensor->colourInverse 	= SENSOR_COLOUR_INVERSE;
	p_sensor->enable_sensor_0_and_7 = SENSOR_ENABLE_0_AND_7;
	p_sensor->isReading_flag 	= false;
	
	//==== PID initialation
	#if(SENSOR_USE_PID == 1)
		p_sensor->PID.enableSign = true;
		PID_Set(&(p_sensor->PID), SENSOR_PID_KP, SENSOR_PID_KI, SENSOR_PID_KD, true, true);
	#endif
	
	
	//p_sensor->features.trackingLine.enable 			= true;
	p_sensor->features.trackingLine.basicSpeed		= TRACKING_BASIC_SPEED;
	p_sensor->features.trackingLine.speedCoord		= TRACKING_SPEED_COORD;
	p_sensor->features.trackingLine.inverseDir 		= TRACKING_DIR_INVERSE;
	p_sensor->features.trackingLine.fastMode.enable = false;
	p_sensor->features.trackingLine.fastMode.basicSpeed = TRACKING_FAST_SPEED;
	p_sensor->features.trackingLine.fastMode.goSlowAfterCount = TRACKING_FAST_SLOW_ENABLE;
	p_sensor->features.trackingLine.fastMode.slow_PIDDuringCount = TRACKING_FAST_DURATION;
	p_sensor->features.trackingLine.Action			= TrackingLine;
	
	p_sensor->features.crossRecognition.enable 		= SENSOR_CROSS_DETECT_ENABLE;
	p_sensor->features.crossRecognition.length		= SENSOR_CROSS_ACTION_CYCLE;
	p_sensor->features.crossRecognition.Action		= GoCross;
	
	p_sensor->features._90degRecognition.enable 	 = SENSOR_90_DEG_DETECT_ENABLE;
	p_sensor->features._90degRecognition.ActionLeft  = GoLeft90Deg;
	p_sensor->features._90degRecognition.ActionRight = GoRight90Deg;	
	
}


#ifdef __INTERFACE_H__
/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
void Sensor_GetStatusString(char* p_str)
{
	// 确保 p_str 指向的空间长度大于 15，否则 后果自负！
	char str[8] = {0};
	p_str[0] = '\0';	// 清零字符串
	
	for(int i = 0; i < SENSOR_AMOUNT; i++){
		if((i == 0 || i == 7) && Sensor.enable_sensor_0_and_7 == false){
			strcat(p_str, "*");
			
		}else{	// i == 1 to 6, or i == 0 or 7 and enabled them
			
			if(Sensor.status_array[i] == true){
				sprintf(str, "%d", i);
				strcat(p_str, str);
				
			}else{	// Sensor.status_array[i] == false
				strcat(p_str, "_");
			}
		}
		
		if(i == 3){
			strcat(p_str, ".");
		}else if (i == 7){
			
		}else{
			strcat(p_str, " ");
		}
	}
}
#endif



/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
void Sensor_StopTracing(void)
{
	
	Sensor.enableTracing = false;
	PID_Clear(&(Sensor.PID));
	
	//==== 如果 有 电机，则 电机 PID 清零
	#ifdef __MOTOR_H__
		Motor_ClearPID();	
	#endif
	
	
}



/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
void Sensor_Disable(void)
{
	
	Sensor_StopTracing();
	
	for(int i = 0; i < SENSOR_AMOUNT; i++){
		GPIO_setAsInputPin(Sensor_GPIO_port[i], Sensor_GPIO_pin[i]);
	}	

	
	memset(&Sensor, 0, sizeof(Sensors_typedef));	// 清空结构体
	
	Sensor.enabled = false;
	
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
void Sensor_StartTracing(void)
{
	Sensor.enableTracing = true;
	PID_Clear(&(Sensor.PID));
	PID_Clear(&(Motor[0].PID));
	PID_Clear(&(Motor[1].PID));	
}


/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
void Sensor_init(bool isDoStarting)
{
	if(Sensor.enabled == true){
		return;
	}
	
	
	
	//==== 初始化 GPIO 口
	for(int i = 0; i < SENSOR_AMOUNT; i++){
		GPIO_setAsInputPin(Sensor_GPIO_port[i], Sensor_GPIO_pin[i]);
	}
	
	
	initStruct_Sensor(&Sensor);	// 初始化 结构体数组
		
		
	
	if(isDoStarting == true){
		Sensor_StartTracing();
	}
	
	Sensor.enabled = true;
	
}

//======================== 中断函数 Interrupt Handlers ===========================

/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
//====== 暂时挂到 电机pid中断上
/*

void Interrupt(void)
{
	
	ClearInterruptFlag();
	
	#ifdef __TRACING_SENSOR_H__
		
		if(Sensor.enabled == true){	// 循迹传感器
			
			Sensor_ReadStatus();
			
			if(Sensor.PID.enableSign == true){
				PID_Calc(&Sensor.PID, Sensor.averagedLineLocation);
			}
		}
		
	
	#endif
	
}





*/


