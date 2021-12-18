#include "TracingSensor.h"
#include "Motor.h"

#include "Interface.h"


//========================== ˽�к궨�� Private Macros ===========================


//===================== ����ȫ�ֱ��� Public Global Variables =====================


Sensors_typedef Sensor = {0};





/*
	��������	  0		  1		  2		  3	  |	  4		  5		  6		  7
	λ��(mm)��
	�˿�		P4.1	P4.2	P4.3	P4.4	P4.5	P4.6	P4.7	P4.0		


*/
const uint8_t Sensor_GPIO_port[SENSOR_AMOUNT] = {	// ��������̽ͷ�� GPIO ����
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

const uint8_t Sensor_GPIO_pin[SENSOR_AMOUNT] = {	// ��������̽ͷ�� GPIO ����
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

const float Sensor_location[SENSOR_AMOUNT] = {	// �� �Ҷ�/���� ������ ̽ͷ �� ����λ�ã���λ��mm
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

//==================== ˽��ȫ�ֱ��� Private Global Variables =====================



//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============


/**
 * @brief	
 * @param	offset[in]	��������⵽�� С��λ�� ƫ������ >0: �ҹ�
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
	
	//==== ƫ�+44 mm���ң� -> �ٶȣ�30 -> ϵ����0.0113636f ---> ���٣�+15���ң�
	if(Sensor.features.trackingLine.fastMode.enable == false){
		// ����ģʽ
		basicSpeed = Sensor.features.trackingLine.basicSpeed;
		
	}else{
		// ����ģʽ
		basicSpeed = Sensor.features.trackingLine.fastMode.basicSpeed;
	}
	
	//==== ��ȡ �ٶȲ�ֵ
	dif_raw =  offset * basicSpeed * Sensor.features.trackingLine.speedCoord;
	
	
	//==== PID Ѳ��
	if(Sensor.PID.enableSign == true){
		//==== �� PID ����ֵ ���� �����
		PID_Calc(&(Sensor.PID), dif_raw); // �������ݣ����� PID
		dif_final = Sensor.PID.outputValue;	// ���� PID ��� ���� �����ٶȱ仯ֵ
		
	}else{	// Sensor.PID.enableSign == false δ���� PID ����
		dif_final = dif_raw;	// ���� ����ֵ ֱ�� ��ʹ�� PID  ���� �����ٶȱ仯ֵ
		
	}	
	
	
	//==== ����� ��� PID ��
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
	
	//==== ����Ǹոռ�⵽ʮ��·�ڣ���һ��ִ���������
	if(isFirst == true){
		count = 0;
		Sensor.features.crossRecognition.isDoingCrossAction = true;
		
	}else{
		
	}
	
	
	//==== ä��ʮ��·�ڣ�����ʱ������Ƿ񵽴�ָ��ʱ��
	if(count != Sensor.features.crossRecognition.length){	// ���ڼ�ʱ��ä��ʮ��·��
		count++;
		
	}else{				// ��ʱ�������Ѿ�Խ��ʮ��·��
		count = 0;	// ��������
		Sensor.features.crossRecognition.isDoingCrossAction = false; // ��־λ����
		
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
		
	if(Sensor.shelteredSensorsAmount >= SENSOR_CROSS_SHELTERED_THRESHOLD){	// ��鴫�������ڵ�������
		//cross
		Sensor.features.crossRecognition.isDetected_flag = true;		// ������־λ, ����Ѳ��ʹ��
	
		if(Sensor.features.crossRecognition.Action != (void*)NULL ){
	 		Sensor.features.crossRecognition.Action(true);		// ���봦���������¿�ʼ
		}
	
	}else{		// ==== ����������ڵ�����û��һ�룬����Ϊ����ʮ��·��
		Sensor.features.crossRecognition.isDetected_flag = false;		// �����־λ, ����Ѳ��ʹ��
		
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
	
	//==== δ����д
	
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
	
	// ==== �Ƿ� ���ڴ���ʮ��·�� �¼�������
	if(Sensor.features.crossRecognition.isDoingCrossAction == true){	// ��֤��ʱһ���Ѿ� enable == true ��
	 	if(Sensor.features.crossRecognition.Action != (void*)NULL ){
	 		Sensor.features.crossRecognition.Action(false);		// ���봦����
	 	}
		val = true;		
		 
		// ��ǰ�˳�
		
		// --- ��⵽����ִ�� ��ת 90 �� �¼�����
	}/*else if (Sensor.features._90degRecognition.isDoingTurnLeft90Action == true){
	 	if(Sensor.features._90degRecognition.ActionLeft != (void*)NULL ){
	 		Sensor.features._90degRecognition.ActionLeft(false);		// ���봦����
	 	}		
		val = true;	
		
		// --- ��⵽����ִ�� ��ת 90 �� �¼�����
	}else if (Sensor.features._90degRecognition.isDoingTurnLeft90Action == true){
	 	if(Sensor.features._90degRecognition.ActionRight != (void*)NULL ){
	 		Sensor.features._90degRecognition.ActionRight(false);		// ���봦����
	 	}			
		val = true;
		
	}*/else{	// ��������ִ����Щ�¼������������Ƿ�����ʮ��·�ں�ֱ��ת�䣬���û�����Ǿ�Ѳ��
		
		//==== ����ǲ���ʮ��·�ڣ���λ��־λ
		if(Sensor.features.crossRecognition.enable == true){
			DetectCross();	// ��ֵ����־λ
		}		
		
		// //==== ����Ƿ����� 90 ��ֱ����
		// if(Sensor.features._90degRecognition.enable == true){
		// 	Detect90Deg();	
		// }
		
		
		//==== �����ϣ���� ����ִ���¼����� ʱ ������ʮ��·��
		if(Sensor.features.crossRecognition.enable == true){		// ��ȷ���Ƿ�ʹ�� ʮ��·�ڼ��
			if(Sensor.features.crossRecognition.isDetected_flag == true){	// ��ȷ���Ƿ��⵽��ʮ��·��
				if(Sensor.features.crossRecognition.Action != (void*)NULL ){
					Sensor.features.crossRecognition.Action(true);	// �����¼�����Ȼ��ѭ��
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
	
	return val;	// val == true ʱ ��ѭ��
}

//========================= ���к���  Public Functions ===========================


/**
 * @brief	��ȡ�Ҷ�/���⴫������������ָ����Ϊ��������ѭ�ߡ���ʮ��·�ڡ���ֱ���䣬 \b ��_���_PID_�жϺ���_��ʱ����
 * @param	none
 * @return	none  
 * @note	ȫ�Զ����*/
void Sensor_ReadStatus(void)
{
	if (Sensor.enabled == false){
		return;
	}
	
	Sensor.isReading_flag = true;
	
	
	//======= ��ʼ��ȡ�����ڵ���������������ƽ��ֵ

	bool val[SENSOR_AMOUNT] = {0};	// ��ʱ�������Ժ�д��ṹ����
	//uint8_t val_bit = 0;
	uint8_t sheltered = 0;	// initialized to 0 !!!
	float loc_sum = 0.0f;	// 
	
	//Sensor.status_bit = 0x00;	// ��¼���ݣ�λ��������
	
		// ѭ��
	for(int i = 0; i < SENSOR_AMOUNT; i++){
			//GPIO_INPUT_PIN_HIGH == 0x01,  GPIO_INPUT_PIN_LOW == 0x00
			
		if( (Sensor.enable_sensor_0_and_7 == false)  &&  ( (i == 0) || (i == 7) )  ){	
			// i == 0 or 7, and disabled sensor 0 and 7
			val[i] = 0;		// obviously, not detected
			Sensor.status_array[i] = 0;
			//continue;
			
		}else{	// i == 1 to 6; or i == 0 or 7 and  sensor 0 and 7 are enabled
			
			val[i] = GPIO_getInputPinValue(Sensor_GPIO_port[i], Sensor_GPIO_pin[i]); // ��ȡ GPIO
			
			if(Sensor.colourInverse == true){	// ��� ��ɫ�෴ �� ȡ��
				val[i] =  ! val[i];
			}
			
			Sensor.status_array[i] = val[i];// ��¼���ݣ��������飩
			
			if(val[i] == true){		// ����� i �� ������ �� �ڵ���
				//val_bit |= ( 0x01 << i ) ;	// ��¼���ݣ�λ�����˱���Ҫ�� ����ѭ��ǰ �������㣬ѭ�������� д��ṹ����
				sheltered++;	// �ڵ����� +1
				loc_sum += Sensor_location[i];	// ȡƽ����
			}
		}
		
	}		
	
	Sensor.shelteredSensorsAmount = sheltered;	// ��¼���ݣ�λ����д�� �ṹ��
	

	if(sheltered == 0){
		Sensor.averagedLineLocation = 0.0f;
	}else{
		Sensor.averagedLineLocation = loc_sum / (float)sheltered;
	}		
	

	//Sensor.status_bit = var_bit;  
	
	//==== ����ղŵõ�������
	
	volatile bool ifNotNeedTracking = TreatSensorStatus();	// �����û��ֱ�����ʮ��·�ڣ������ڽ����¼�������������������
	
	//== ���û�м�⵽ ʮ��·�� �� ֱ����
	if(ifNotNeedTracking == false){	
		if(Sensor.features.trackingLine.Action != (void*)NULL ){
			
			if(Sensor.enableTracing == true){
				Sensor.features.trackingLine.Action(Sensor.averagedLineLocation);	// Ѳ��
			}else{
				Sensor.features.trackingLine.Action(0.0f);						// ֱ��
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
	//==== ��ʼ������
	memset(p_sensor, 0, sizeof(Sensors_typedef));	// ��սṹ��
	
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
	// ȷ�� p_str ָ��Ŀռ䳤�ȴ��� 15������ ����Ը���
	char str[8] = {0};
	p_str[0] = '\0';	// �����ַ���
	
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
	
	//==== ��� �� ������� ��� PID ����
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

	
	memset(&Sensor, 0, sizeof(Sensors_typedef));	// ��սṹ��
	
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
	
	
	
	//==== ��ʼ�� GPIO ��
	for(int i = 0; i < SENSOR_AMOUNT; i++){
		GPIO_setAsInputPin(Sensor_GPIO_port[i], Sensor_GPIO_pin[i]);
	}
	
	
	initStruct_Sensor(&Sensor);	// ��ʼ�� �ṹ������
		
		
	
	if(isDoStarting == true){
		Sensor_StartTracing();
	}
	
	Sensor.enabled = true;
	
}

//======================== �жϺ��� Interrupt Handlers ===========================

/**
 * @brief	
 * @param	none
 * @return	none  
 * @note	*/
//====== ��ʱ�ҵ� ���pid�ж���
/*

void Interrupt(void)
{
	
	ClearInterruptFlag();
	
	#ifdef __TRACING_SENSOR_H__
		
		if(Sensor.enabled == true){	// ѭ��������
			
			Sensor_ReadStatus();
			
			if(Sensor.PID.enableSign == true){
				PID_Calc(&Sensor.PID, Sensor.averagedLineLocation);
			}
		}
		
	
	#endif
	
}





*/


