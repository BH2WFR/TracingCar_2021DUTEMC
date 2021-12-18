#include "Motor.h"
#include "LED.h"
#include "TracingSensor.h"
// #include "Mode\Control_Car.h"
// #include "Interface.h"
/*	
	

	
*/


//========================== ˽�к궨�� Private Macros ===========================

// #define 	PID_KP_DEFAULT					0.1f
// #define 	PID_KI_DEFAULT					0.1f
// #define 	PID_KD_DEFAULT					0.01f		// PID�㷨�� ��ʼϵ��

//#define		MOTOR_DUTY_CYCLE_COEFFICIENT	(MOTOR_TIMER_LOADVALUE / MOTOR_MAX_FORCE)
//







//===================== ����ȫ�ֱ��� Public Global Variables =====================

Motor_Typedef Motor[MOTOR_AMOUNT] = {0};	// ����Ҫ��������ýṹ�壬�ں� PID �ṹ�壬ע���Ǹ�����
uint64_t PID_count = 0;


// int16_t Motor_PWM_CCRx_register[MOTOR_AMOUNT][2] = {
// 		[0][0] = TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_A_CCRx],
// 		[0][1] = TIMER_A_CMSIS(MOTOR_0_PWM_TIMER_BASE)->CCR[MOTOR_0_PWM_B_CCRx],
// };




//==================== ˽��ȫ�ֱ��� Private Global Variables =====================

//Attention�� ���½ṹ���ʼ����ʽΪ C99 �﷨
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


//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

/**
 * @brief	˽�к���	���������ȣ�ռ�ձȣ���� CCRx �Ĵ���װ�س�ֵ
 * @param	force[in]	��������ȣ���Χ��(-MOTOR_MAX_FORCE, +MOTOR_MAX_FORCE)
 * @return	CCRx �Ĵ���װ�س�ֵ
 * @note	��˽�к�������ı���״̬��Ҳ�������õ������
 * 			�� \b Motor_SetForce(#,#) ��������*/
static inline uint16_t GetCCRvalue(int16_t force)
{
	//����򿪴����Ż���
	uint16_t output = (uint16_t)( abs(force) * (MOTOR_TIMER_CCR0_LOADVALUE / MOTOR_MAX_FORCE) );
	return output;
}



static void GoStep(uint8_t motorSelection)
{
	//==== ��ֹ�Բ�
	if(Motor[motorSelection].isInSteppingMode == false){
		return;
	}
	
	//==== ��� �ﵽ�� ��������
	if(		Motor[motorSelection].stepModeDir == 1 && (Motor[motorSelection].stepCount > Motor[motorSelection].stepTarget)
		||  Motor[motorSelection].stepModeDir == 0 && (Motor[motorSelection].stepCount < Motor[motorSelection].stepTarget)){
		
		//==== �ر� �ǲ�ģʽ
		Motor[motorSelection].isInSteppingMode = false;
		
		//==== ��� �ָ� �ǲ�ģʽ֮ǰ��ת��
		//Motor[motorSelection].PID.setPoint = Motor[motorSelection].speedBeforeStepMode;
		Motor[motorSelection].PID.setPoint = 0;
		
	}else{
		
		//==== �����ٶ�
		Motor[motorSelection].PID.setPoint = Motor[motorSelection].steppingModeSpeed;
	}
	
}

/**
 * @brief	PID ��� ����ֵ
 * @param	p_PID_struct[in|out] �ṹ���ַ
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
 * @brief		PID ���� \b ��ֹ���� 
 * @param	p_PID_struct[in|out] �ṹ���ַ
 * @param	currentValue[in]	��ǰ��������	
 * @return	pid ����� ���ֵ 
 * @note	�ú��� �� PID ��ʱ���жϺ��� ���ã� \b �Ͻ������ĵ��÷���	
 * 			��ȡ ȫ�ֱ��� 
 * 			 \b �趨Ŀ���� �� \b ���и��� �ṹ�� \b p_PID_struct->setPoint �еĲ��� */
float PID_Calc(volatile PID_Typedef* p_PID_struct, float currentValue)
{
	if(p_PID_struct == NULL){
		return 666;
	}
	
	//float output;
	float error;
	
	//==== ����
	error =  currentValue - p_PID_struct->setPoint;
	
	if(abs(p_PID_struct->outputValue) >= 10000){
		p_PID_struct->outputValue = 0;
	}
	
	p_PID_struct->outputValue +=  (p_PID_struct->A * error )
								 +(p_PID_struct->B * p_PID_struct->lastError)
							 	 +(p_PID_struct->C * p_PID_struct->prevError);
	
	//==== �޷�							  
	if(p_PID_struct->limiting.enable == true){
		if(p_PID_struct->outputValue > p_PID_struct->limiting.max){
			p_PID_struct->outputValue = p_PID_struct->limiting.max;
		}else if(p_PID_struct->outputValue < p_PID_struct->limiting.min){
			p_PID_struct->outputValue = p_PID_struct->limiting.min;
		}
	}
	
		
	p_PID_struct->prevError = p_PID_struct->lastError; //== ���α��ϴ�, �ֻ�
	p_PID_struct->lastError = error;
	
	
	//==== ���
	//p_PID_struct->outputValue = output;
	return p_PID_struct->outputValue;
}

/**
 * @brief	˽�к���	\b ��ֹ���� ���ݱ���������ȡ�����ݼ��� PID���һ�ȡ�ٶ�
 * @param	index[in]	
 * @return	none 
 * @note	�ú��� �� PID ��ʱ���жϺ��� ���ã� \b �Ͻ������ĵ��÷���	
 * 			��ȡ ȫ�ֱ��� */
static void PID_Motor_Control(uint8_t index)
{
	float speed;
	//float force;
	
	//==== ���� �������ж� ��ȡ�� ʱ���� ���� �ٶ�
	
	if(Motor[index].isInterruptedDuringPIDperiod == false){
		// ��� ���� PID �ж��ڼ� û���� �������жϣ����ٶ���Ϊ�� 0�����ͣת��
		speed = 0;	
			
	}else{	//== ����������ת������ PID �ж��ڼ� �������ж� �� ������
		//== ����������ת������������ٶȣ��������ֵ�������������Ƿ���
		speed = MOTOR_ENCODER_SPEED_COEFFICIENT / Motor[index].durationTime;
		
		//== ��ȡ�ṹ���е���ķ��򣬲����ٶȳ��� 1 �� -1
		if(Motor[index].detectedDirection == MOTOR_DIRECTION_BACKWARD){
			speed *= -1;	// speed ^= 0x80000000
		}
		
	}
	
	Motor[index].detectedSpeed = speed;	// д�� ��⵽���ٶ�
	
	
	
	//==== ���� �趨�� Ŀ��ֵ ���� PID
	if(Motor[index].PID.enableSign == 1){	// �Ƿ�ʹ���˵���� PID ���ܣ�
		//=== ���ú��� ���� PID
		
		PID_Calc(&Motor[index].PID, speed);	// ���� PID
		
		volatile int16_t pidOutputVal = (int16_t)Motor[index].PID.outputValue;
		
			//==== force �޷�
		// if(pidOutputVal < -MOTOR_MAX_FORCE){
		// 	pidOutputVal = -MOTOR_MAX_FORCE;
		// }else if (pidOutputVal > MOTOR_MAX_FORCE){
		// 	pidOutputVal = MOTOR_MAX_FORCE;
		// }
		
		
		Motor_SetForce(index, MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT, pidOutputVal);	// ���� PID ���ֵ ���� ���
		
	}else{	// Motor[index].PID.enableSign == 0,  ֱ�����ռ�ձ�
	
		//== ��Ҫ�ڳ��������ط��ֶ���ֵ Motor[index].force
		//Motor_SetForce(index, MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT, 0);	// �� Motor[index].force ��ֵ��Ϊռ�ձ�ֱ�����
		
	}
		
	Motor[index].isInterruptedDuringPIDperiod = false;	// ���� ��־λ���ڱ������ж� �� ��λ
	
}

/**
 * @brief	˽�к���	\b ��ֹ���� ��ȡ ����������ȡʱ������ \b �������ٶȣ� ��
 * @param	index[in]	
 * @return	none 
 * @note	�ú��� �� ������ �жϴ����� ���ã� \b �Ͻ������ĵ��÷���	
 * 			��ȡ ȫ�ֱ��� */
static void Motor_Encoder_Read(uint8_t index)
{
	//==== ץȡ ��ʱ������ֵ
	uint32_t timerValue = Timer32_getValue(MOTOR_ENCODER_TIMER_BASE);
	
	//==== ���� ʱ������д�뵽�ṹ����
	uint32_t duration = Motor[index].lastTimer32Value - timerValue;
	Motor[index].durationTime	  = duration;
	Motor[index].lastTimer32Value = timerValue;	// ���� ��ʱ������ֵ �浵
	
	
	
	
	//==== ��ȡ ������򣬿� B �� �� ��ƽ��Ȼ�� д��
	bool  encoder_B_Status;
	switch(index){
		case 0:
			encoder_B_Status = GPIO_getInputPinValue(MOTOR_0_ENCODER_B_PORT_PIN);
			break;
		case 1:
			encoder_B_Status = GPIO_getInputPinValue(MOTOR_1_ENCODER_B_PORT_PIN);			
			break;
	}
	Motor[index].detectedDirection = ! encoder_B_Status; // д��
	
	//==== �ǲ�����¼���룩
	if(encoder_B_Status == true){
		Motor[index].stepCount--;
	}else{
		Motor[index].stepCount++;
	}
	
	
	
	
	//==== �˲�
	// Motor[index]. durationTimeBuffer[Motor[index]. durationTimeBufferIndex]  = timerValue;
	// RollUpRange(Motor[index]. durationTimeBufferIndex, index, MOTOR_ENCODER_DURATION_BUFFER_AMOUNT);
	

	//==== ��־λ �����жϣ��� PID ���� ���㣬���� ��� ����Ƿ�ͣת
	Motor[index].isInterruptedDuringPIDperiod = true;	// clear in PID funcion, for detecting if the speed refreshed 	
}








//========================= ���к���  Public Functions ===========================



/**
 * @brief	���õ���� ���� �� ����
 * @param	motorSelection[in]	ѡ��������ţ�
 * @param	inputMode[in]		\b ��ȡforceֵ��ģʽ��
 * 								�� inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT \b (false) ʱ
 * 									���������� force ��Ч���������� �ṹ�� Motor[motorSelection].force	�ж�ȡֵ
 * 								�� inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT \b (true) ʱ
 * 									���������� force ��ֵ д�뵽 Motor[motorSelection].force ��
 * @param	force[in]	��������ȣ�
 * 								�� inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT ʱ
 * 									��������Ч���ɸ�ֵ�κ�ֵ
 * 								�� inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT ʱ
 * 									������Ϊ Ҫ ����� force ֵ����Χ��( \b -MOTOR_MAX_FORCE, +MOTOR_MAX_FORCE)�� \b >0_��ת��<0_��ת
 * 								
 * @return	none
 * @note	����˽�к�����GetCCRvalue(#)	
 * 			�л�����ʱ \b ֻ�� CCRx �Ĵ���ֵ������ GPIO ��ؼĴ���
 * 			�ú��� ��ȡ/д�� Motor[x] �ṹ��ȫ�ֱ��� */
int16_t Motor_SetForce(uint8_t motorSelection, bool inputMode, int16_t force)
{
	uint16_t CCRloadValue;
	
	uint16_t CCR_A = 0;
	uint16_t CCR_B = 0;
	
	//==== force �޷�
	if(force < -MOTOR_MAX_FORCE){
		force = -MOTOR_MAX_FORCE;
	}else if (force > MOTOR_MAX_FORCE){
		force = MOTOR_MAX_FORCE;
	}
	
	
	
	//==== ͬ�� ������� �� �ṹ���е� force ����
	if(inputMode == MOTOR_FORCE_INPUTMODE_READ_FORCE_IN_STRUCT){
		force = Motor[motorSelection].force;
	}else{	// inputMode == MOTOR_FORCE_INPUTMODE_WRITE_FORCE_TO_STRUCT
		Motor[motorSelection].force = force;
	}
	
	//==== ���� CCR ֵ
	CCRloadValue = GetCCRvalue(force);
	
	
	//==== ��������ת �� CCRx ֵ
	if(force > 0){	//	��ת
		//Motor[motorSelection].setDirection = MOTOR_DIRECTION_FORWARD;
		
		CCR_A = CCRloadValue;
		CCR_B = 0;
		
	}else{	//force < 0	��ת
		//Motor[motorSelection].setDirection = MOTOR_DIRECTION_BACKWARD;
		
		CCR_A = 0;
		CCR_B = CCRloadValue;			
		
	}
	
	
	
	//===== ѡ�������Ĵ�����ֵ
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
 * @brief	���� �ṹ���ַ �� Ԥ��ֵ���ı� setPoint
 * @param	p_PID_struct[in|out]	Ҫ���в����� PID �ṹ�� ��ַ	
 * @param	point[in]		�趨�� Ԥ��ֵ	
 * @return	none 
 * @note	*/
void PID_SetPoint(PID_Typedef* p_PID_struct, float point)
{
	if(p_PID_struct == NULL){	// ��ֹ�Բ�
		return;
	}
	
	p_PID_struct->setPoint = point;
}


/**
 * @brief	���� �ṹ���ַ �� Kp, Ki, Kd, ���������ʽ pid �㷨���������� A, B, C����������ṹ������
 * @param	p_PID_struct[in|out]	Ҫ���в����� PID �ṹ�� ��ַ	
 * @param	Kp[in]		Ҫ�趨�� PID ����ϵ��
 * @param	Ki[in]		
 * @param	Kd[in]	
 * @param 	inverse		�Ƿ���������ȡ����
 * @param   is_reset	�Ƿ���� 	lastError�� prevError��outputValue
 * @return	none 
 * @note	*/
void PID_Set(PID_Typedef* p_PID_struct, float Kp, float Ki, float Kd, bool inverse, bool is_reset)
{
	if(p_PID_struct == NULL){	// ��ֹ�Բ�
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
 * @brief	\b ��ѭ������ ����ǲ�
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
 * @brief	����ǲ�
 * @param	none 
 * @return	none  
 * @note	Ҫ������ѭ������  Motor_GoStep_circulate(void) ����
 * 			���з���ȡ���� step �� ���ţ��� speed �� ���� �޹�*/
void Motor_GoStep(uint8_t motorSelection, int16_t speed, int64_t step)
{
	if(Motor[motorSelection].isInSteppingMode == true){
		return;
	}
	
	//==== �˶�����ȡ���� step �� ����
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
 * @brief	��� PID ����
 * @param	none
 * @return	none  
 * @note	��*/
void Motor_ClearPID(void)
{
	for(uintptr_t i = 0; i < MOTOR_AMOUNT; i++){
		PID_Clear(&(Motor[i].PID));
	}		
	
}



/**
 * @brief	���ģ�� \b ��ͣ���У� �ر� PWM �����ʱ������Ӱ�� PID ��ʱ���ж�
 * @param	none
 * @return	none  
 * @note	����ģʽ�ļ��� halt() ������ \b ���е��� �������*/
void Motor_Stop(void)
{
	//==== ������� IO �� ȡ������
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_DRIVE_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_DRIVE_B_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_DRIVE_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_DRIVE_B_PORT_PIN);	
	
	
	//==== ֹͣ ��� PWM ��ʱ��
	Timer_A_stopTimer(MOTOR_0_PWM_TIMER_BASE);	// include motor 1
	
	
	//==== ������ȣ�PID Ŀ��ֵ�� ����Ϊ 0
	Motor_ClearPID();		
	
	
	//==== ��־λ �ر�
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].isStarted = false;
	}	
}



/**
 * @brief	���׽��� ���������������� �� PID ��ʱ��
 * @param	none
 * @return	none  
 * @note	�������Ѿ��ڲ����� Motor_Stop(), �����ٴε�����*/
void Motor_Disable(void)
{
	
	//==== ���ͣת��PWM��ʱ���رգ�GPIO�رո���
	Motor_Stop();
	

	
	//==== �ر� PID ��ʱ�� �� �ж�
	Timer_A_stopTimer(MOTOR_PID_TIMER_BASE);
	Timer_A_disableInterrupt(MOTOR_PID_TIMER_BASE);
	
	Timer32_haltTimer(MOTOR_ENCODER_TIMER_BASE);
	Timer32_disableInterrupt(MOTOR_ENCODER_TIMER_BASE);
	
	
	//==== �ر� ������ GPIO �ж�
	GPIO_disableInterrupt(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);		// �� �ص� ���� �ж�
	GPIO_clearInterruptFlag(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);


	
	//==== ��սṹ��
	//memset(&Motor, 0, (MOTOR_AMOUNT * sizeof(Motor_Typedef))  );
		// �Ժ���� malloc/free
	
	
	//==== ��־λ �ر�
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].enable = false;
	}
	
}

/**
 * @brief	���ģ�飬��� GPIO �������ã��ҿ��� PWM ��ʱ������ �����ʼ��ת�������� PID ��ʱ��
 * @param	none
 * @return	none  
 * @note	����ģʽ�ļ��� init() ������ \b ���е��� �������*/
void Motor_Start(void)
{
	//==== �������ǰ�����ʼ����
	if(Motor[0].enable == false){
		return;
	}
	
	//==== ��ʱ
	
	
	//==== ������ȣ�PID Ŀ��ֵ�� ����Ϊ 0
	Motor_ClearPID();		
	
								
	//==== ���� �������PWM �ź� ��ʱ�� 
	Timer_A_startCounter(MOTOR_0_PWM_TIMER_BASE, TIMER_A_UP_MODE);	// together with motor_1

	
	//==== ��� IO �� ��ʼ ����
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_0_DRIVE_A_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_1
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_0_DRIVE_B_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_2
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_1_DRIVE_A_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_3
	GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_1_DRIVE_B_PORT_PIN, 
											GPIO_PRIMARY_MODULE_FUNCTION);	//timerA2_4									
	
	//==== ��ʱ
	
			
	//==== �ź�λ										
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].isStarted = true;
	}											
																					
}

/**
 * @brief	���ģ�� ��ʼ������ ������ �� PID������ PID ��ʱ�� ��ʼ�ж�
 * @param	isStartMotor	�Ƿ���������� \b Motor_Start(void) ,  \b �������_GPIO_���ã�
 * @return	none  
 * @note	  */
void Motor_init(bool isStartMotor)
{
	if(Motor[0].enable == true){
		return;
	}
	
	//==== ��սṹ��
	memset((void*)(&Motor), 0, (MOTOR_AMOUNT * sizeof(Motor_Typedef))  );
		
	
	
	//==== ������� GPIO �� ��ʱ �رո���
	Motor_Stop();
	
	
	//==== ������ GPIO	��ʼ�� ����ģʽ									
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_ENCODER_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_0_ENCODER_B_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_ENCODER_A_PORT_PIN);
	GPIO_setAsInputPinWithPullUpResistor(MOTOR_1_ENCODER_B_PORT_PIN);

		
	//==== ��� PWM ��ʱ�����Ƚ�ģʽ�����������ر��ж�
	Timer_A_configureUpMode(MOTOR_0_PWM_TIMER_BASE, &TimerA2_upConfig);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_1);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_2);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_3);
	Timer_A_initCompare(MOTOR_0_PWM_TIMER_BASE, &compareConfig_PWM2_4);
	
	Timer_A_disableInterrupt(MOTOR_0_PWM_TIMER_BASE);	// �ر� PWM ��ʱ�����жϣ���Ϊ����Ҫ
	
	
	
	//==== ������ GPIO �ж� 
	
	//P6->IE = 0x00;
	GPIO_disableInterrupt(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);		// �� �ص� ���� �ж�
	GPIO_clearInterruptFlag(MOTOR_0_ENCODER_A_PORT, PIN_ALL8);
	// MOTOR_1 �� ������ �� MOTOR_0 �� ��ͬ�� PORT
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
			//===== Ĭ��ʹ��PID
			Motor[i].PID.enableSign = true;
		#endif
	}
	
	//==== ���� PID ��ʱ���ж�
	Timer_A_configureUpMode(MOTOR_PID_TIMER_BASE, &TimerA_PID_upConfig);	
	Timer_A_enableInterrupt(MOTOR_PID_TIMER_BASE);
	Interrupt_enableInterrupt(MOTOR_PID_TIMER_INTERRUPT);
	
	
	//==== �� PID ��ʱ��
	Timer_A_startCounter(MOTOR_PID_TIMER_BASE, TIMER_A_UP_MODE);
	Timer32_startTimer(MOTOR_ENCODER_TIMER_BASE, false);
		
	Timer32_initModule(MOTOR_ENCODER_TIMER_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
	Timer32_setCount(MOTOR_ENCODER_TIMER_BASE, 0xFFFFFFFF);
	Interrupt_enableInterrupt(MOTOR_ENCODER_TIMER_INTERRUPT);
	Timer32_enableInterrupt(MOTOR_ENCODER_TIMER_BASE);
	
	
	//==== �ź�λ
	for(int i = 0; i < MOTOR_AMOUNT; i++){
		Motor[i].enable = true;
	}
	
	
}






//======================== �жϺ��� Interrupt Handlers ===========================
/**
 * @brief  GPIO_PORT_6  �ж�  ��ʱֻ�õ���  P6.2 ������ �ж�
 * 			��Ҫ����:  */
void MOTOR_ENCODER_IRQHandler(void)
{
	uint32_t status = GPIO_getInterruptStatus(MOTOR_ENCODER_INT_PORTPIN);	// == Motor_0_pin | Motor_1_pin
	uint8_t  index;
	
	
	GPIO_clearInterruptFlag(MOTOR_ENCODER_INT_PORTPIN);
	
	
	//==== �ж��� �ĸ������� �ж��� (�п��� ��������� ͬʱ�жϣ������� else if �ṹ)��Ȼ�� ���� ˽�к��� ���� ʱ����
	if(status & MOTOR_0_ENCODER_A_PIN){
		index = 0;
		Motor_Encoder_Read(index);	// ����
	}
	if(status & MOTOR_1_ENCODER_A_PIN){	// ����ʹ�� else if
		index = 1;
		Motor_Encoder_Read(index);	// ����
	}
	
}


/**
 * @brief  PID �ж� 
 * 			��Ҫ����:  */
void MOTOR_PID_TIMER_IRQHandler(void)
{
	Timer_A_clearCaptureCompareInterrupt(MOTOR_PID_TIMER_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);	// ����ж�
	//static uint64_t count = 0;	// �жϴ���������������û�õ�һ������ (��
	
	PID_count++;
	
	//===== ���
	for(uint8_t i = 0; i < MOTOR_AMOUNT; i++){
		PID_Motor_Control(i);	// ��ȡ ����ٶȣ����� pid
	}
	
	
	//==== �������� PID ����
	// ������ PID_Calc(pid_struct, );
	#ifdef __TRACING_SENSOR_H__
		
		if(Sensor.enabled == true){	// ѭ��������
			
			Sensor_ReadStatus();
			
		}
		
	
	#endif
	
	Motor_GoStep_circulate();
	
	
	
}


/**
 * @brief  timer32_1 �ж�
 * 			��Ҫ����:  */
void MOTOR_ENCODER_TIMER_IRQHandler(void)	// Timer32_1 (int2)
{
	Timer32_clearInterruptFlag(MOTOR_ENCODER_TIMER_BASE);
	
	
}





