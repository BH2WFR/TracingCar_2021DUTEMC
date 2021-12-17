#include "beep.h"
#include "Beep_song.h"
//========================== ˽�к궨�� Private Macros ===========================

//==================== ˽��ȫ�ֱ��� Private Global Variables =====================
static bool g_delayMode = 0;				//�Ƿ�ʹ��delayģʽ
static bool g_isPlayingMusic = false;		//�Ƿ����ڲ�����������
static bool g_isPlayingNote = false;		//�Ƿ����ڲ��ŵ�������

//===================== ����ȫ�ֱ��� Public Global Variables =====================

bool 	 Beep_isMute = false;				// true ʱ �������޷���������

uint32_t Beep_currentTickCountPerBeat = 0;	//ÿ�ĵ�timer32����ֵ
/*	����������������ָ�� Beep_note_typedef*, Ҳ����ָ��const Beep_note_typedef*, �����������������, ���Ӽ���, 
	�������ִ����������ĵ������ǲ������ͨ����, ������cortex M(STM32 / MSP432E/P )�� �� flash���ڴ�����ӳ�� �� �˴���ɱ��� 	*/
Beep_note_typedef* Beep_p_currentMusic = (void*)NULL;			//��ǰ���ֽṹ��������׵�ַ, ������ʱָ��NULL
Beep_note_typedef* Beep_p_currentNote  = (void*)NULL;			//��ǰ���ֽṹ�������� ���ڲ��ŵ������ṹ��ĵ�ַ




//����Ϊ��ʱ������ֵ��
const uint16_t Beep_Pitch_loadValue[] = {		//0-119

	// C   | #C bD  |  D   |  sD bE |   E  |   F   |  sF bG |   G  | sG bA |   A    | sA bB |   B  |
	//����1MHz��ʱ��Ƶ��		12��Ƶ	0-23
	 61155,  57723,  54484,  51427,  48539,  45815,  43243,  40816,  38525,  36364,  34323,  32396, //0	
	 30578,  28862,  27242,  25713,  24270,  22907,  21622,  20409,  19263,  18182,  17161,  16198,	//1
	//���°���4MHz��ʱ��Ƶ��	3��Ƶ	24-47
	 61156,  57723,  54484,  51426,  48540,  45815,  43244,  40817,  38525,  36364,  34323,  32397,	//2
	 30579,  28862,  27242,  25714,  24270,  22908,  21622,  20408,  19263,  18182,  17161,  16198,	//3
	//����ʹ��12MHz��ʱ��Ƶ��	1��Ƶ	48-119
	 45866,  43293,  40864,  38569,  36404,  34361,  32433,  30612,  28895,  27273,  25742,  24297,	//4
	 22934,  21646,  20431,  19285,  18202,  17181,  16216,  15306,  14447,  13636,  12871,  12149,	//5
	 11467,  10823,  10215,   9642,   9101,   8590,   8108,   7653,   7224,   6818,   6435,   6074,	//6
	  5733,   5412,   5108,   4821,   4551,   4295,   4054,   3827,   3612,   3409,   3218,   3037,	//7
	  2867,   2706,   2554,   2411,   2275,   2148,   2027,   1913,   1806,   1705,   1609,   1519,	//8
	  1433,   1353,   1277,   1205,   1138,   1074,   1014,    957,    903,    852,    804,    759	//9
	  //120: pause      121: repeat start	122: repeat end 	123: song end
};
//��������Ķ�ά��   dd = double dimensional
const uint16_t (*Beep_Pitch_loadValue_dd)[12] = (void*)Beep_Pitch_loadValue;





// //����Ϊ�����ڶ�Ӧ��Ƶ�ʣ�
// const float Beep_Pitch_frequency[] = //15.5-16000
// {
// 	// C   | sC bD  |  D   |  sD bE |   E  |   F   |  sF bG |   G  | sG bA |   A    | sA bB |   B  |
// 	//����1MHz��ʱ��Ƶ��		0-23	[15.50, 62.00)
// 	16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.500, 25.957, 27.500, 29.135, 30.868, //0
// 	32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, //1
// 	//���°���4MHz��ʱ��Ƶ��	24-47	[62.00, 250.00)
// 	65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.83, 110.00, 116.54, 123.47, //2
// 	130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185.00, 196.00, 207.65, 220.00, 233.08, 246.94,	//3
// 	//����ʹ��12MHz��ʱ��Ƶ��	48-119	[250.00, 16000(20000)]
// 	261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88,	//4
// 	523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.33, 987.77,	//5
// 	1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5,	//6
// 	2093.0, 221705, 2349.3, 2489.0, 2637.0, 2793.8, 2960.0, 3136.0, 3322.4, 3520.0, 3729.3, 3951.1,	//7
// 	4186.0, 4434.9, 4698.6, 4978.0, 5274.0, 5587.7, 5919.9, 6271.9, 6644.9, 7040.0, 7458.6, 7902.1,	//8
// 	8372.0, 8869.8, 9397.3, 9956.1, 10548 , 11175 , 11840 , 12544 , 13290,  14080 , 14917,  15804 	//9
// };
// //��������Ķ�ά��   dd = double dimensional
// const uint16_t (*Beep_Pitch_frequency_dd)[12] = (void*)Beep_Pitch_frequency;

//TimerA0���ڿ�����Ƶ��Ƶ��
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


//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

/**
 * @brief  ˽�з��� ���ö�ʱ����װ��ֵ�ͷ�Ƶ 
 * 			|  �� SetFrequency(float frequency) �� SetFrequencyByIndex(uint8_t index) ����
 * @param[in]  value 	���õĳ�ʼװ��ֵ
 * @param[in]  divider 	��ʱ���ķ�Ƶϵ�� | �����ǣ�1u, 3u, 12u
 * @return none  */
static void SetTimerLoadValue(uint16_t value, uint8_t divider)
{
	//��Χ���  divider  ������ 1, 2 �� 12
	if(  !(divider == 1u || divider == 3u || divider == 12u)){
		return;
	}
	
	// //��ⶨʱ���Ƿ�������(Upģʽ)   Ĭ�ϴ�ע��  �����Ҫ�Գ���ħ��, Ϊ��ֹĪ������Ĵ���, ��ȡ�����ע��
	// bool is_timer_worked = false;
	// if( (TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL & TIMER_A_CTL_MC_3) == TIMER_A_CTL_MC_3 ){
	// 	is_timer_worked = true;
	// }
	// if(is_timer_worked == true){	//ֹͣ��ʱ��   CTL�Ĵ���4��5λ����
	// 	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;
	// }
	
	//���÷�Ƶ
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_ID__8;	//����
    TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 &= ~TIMER_A_EX0_IDEX_MASK;
	switch(divider){
		case 1u:	//1��Ƶ��12MHz��
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__1;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x01 - 1;
			break;
		case 3u:	//3��Ƶ��4MHz��
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__1;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x03 - 1;
			break;
		case 12u:	//12��Ƶ��12MHz��
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_CTL_ID__2;
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->EX0 = 0x06 - 1;
			break;
	}
	
	//����װ��ֵ
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CCR[0] = value;
	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CCR[4] = value / 2;
	
	// //��֮ǰ���������ָ���ʱ��Upģʽ ���� ����ֹͣ״̬
	// if(is_timer_worked == true){
	// 	TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;
	// }
	
}

/**
 * @brief  ˽�з��� ���������Ƶ�����÷�����Ƶ��
 * 			���� Beep_SetLoadValue(uint16_t value, uint8_t divider)
 * @param  none
 * @return none  
 * 
 */
// static void SetFrequency(float frequency)
// {
// 	//��Χ���
// 	if(frequency > BEEP_FREQ_MAX || frequency < BEEP_FREQ_MIN){
// 		return;
// 	}
	
// 	uint8_t divider = 0;
// 	uint16_t loadValue = 0;
	
	
// 	if		(frequency >= BEEP_DIVIDER_1_INDEX_MIN && frequency < BEEP_DIVIDER_1_INDEX_MAX){	//frequency �� [250.00, 16000.00]
// 		divider = 1u;
// 		loadValue = (uint16_t)(12e6f / (frequency));	// 12 / 1  = 12MHz
// 	}else if(frequency >= BEEP_DIVIDER_3_INDEX_MIN && frequency < BEEP_DIVIDER_3_INDEX_MAX){	//frequency �� [62.00, 250.00)
// 		divider = 3u;
// 		loadValue = (uint16_t)(4e6f / (frequency));		// 12 / 3  = 4MHz
// 	}else if(frequency >= BEEP_DIVIDER_12_INDEX_MIN && frequency <= BEEP_DIVIDER_12_INDEX_MAX){	//frequency �� [15.50, 62.00)
// 		divider = 12u;
// 		loadValue = (uint16_t)(1e6f / (frequency));		// 12 / 12 = 1MHz
// 	}	
	
// 	SetTimerLoadValue(loadValue, divider);	
	
// }

/**
 * @brief  ˽�з��� ��������ı�ţ�������ţ�ѡ�������Ƶ�� 
 * 			���� Beep_SetLoadValue(uint16_t value, uint8_t divider)
 * @param  none
 * @return none  */
static void SetFrequencyByIndex(uint8_t index)
{
	//��Χ���
	if(index > BEEP_PITCH_INDEX_MAX){
		return;
	}
	
	uint8_t divider = 0;
	
	if		(index >= BEEP_DIVIDER_1_INDEX_MIN && index <= BEEP_DIVIDER_1_INDEX_MAX){	//index �� [48, 119]
		divider = 1u;
	}else if(index >= BEEP_DIVIDER_3_INDEX_MIN && index <= BEEP_DIVIDER_3_INDEX_MAX){	//index �� [24, 47]
		divider = 3u;
	}else if(index <= BEEP_DIVIDER_12_INDEX_MAX){	//index �� [0, 23]
		divider = 12u;
	}
	
	SetTimerLoadValue(Beep_Pitch_loadValue[index], divider);
	
}


/**
 * @brief  ˽�з��� ��������, �ж�����������ָ��
 * @param  p_note**[in|out]	����ָ��ĵ�ַ (����ָ��! ), ��������ʱ����Ķ�*p_note, ������Щָ����������� ��: &p_note
 * @return none  */
static void ReadNote(Beep_note_typedef** p_note)
{
	switch((*p_note)->pitch){
	case cEnd:		//��ֹ��
		Beep_StopMusic();
		break;
	case cRepeatStart:	//�����Ǻ� ��ʼ
		//���ݷ�������, ����ǰ�� ++ָ���ַ ѹջ
		break;
	case cRepeatEnd:
		//����ǲ��Ƿ�������(����)
		//�������ָ���ַ ��ջ, ����ת������
		break;
	case cSetSpeed:		//�����ٶ�
		Beep_SetSpeed((*p_note)->length);
		(*p_note)++;
		ReadNote(p_note);	//�ݹ����
		break;
	case cStart:
		//�����ܷ���
		break;
	default:	//����
		if((*p_note)->pitch > BEEP_COMMAND_INDEX_MAX){
			Beep_StopMusic();
		}
		
		Beep_SingleNote((*p_note)->pitch, (*p_note)->length, BEEP_MODE_NOT_DELAY);
		break;
	}
}


//========================= ���к���  Public Functions ===========================
/**
 * @brief  ��������һ������
 * @param  index[in]		�������, ����ֵ: �μ��궨��
 * @param  length[in]		�����ĳ���, ��λ: ��
 * @param  delayMode[in]	�ӳ�ģʽ / �ж�ģʽ, ����ֵ: BEEP_MODE_USE_DELAY, BEEP_MODE_NOT_DELAY
 * @return none  */
void Beep_SingleNote(uint8_t index, float length, bool delayMode)
{
	g_delayMode = delayMode;
	if(g_isPlayingNote == true || index > cPause) {			//��������������˳�
		return;
	}
	g_isPlayingNote = true;	//�򿪱�־λ
	
	TIMER32_CMSIS(BEEP_TIMER32_BASE)->LOAD = (uint32_t)(length * (double)Beep_currentTickCountPerBeat);		//����Timer32װ��ֵ


	//ʹ��delay�ӳ�
	if(delayMode == BEEP_MODE_USE_DELAY){	
		
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL &= ~TIMER32_CONTROL_IE;	//�ر�Timer32�ж�
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_ENABLE;	//����Timer32
		
		//����
		if(index != cPause){		
				
			SetFrequencyByIndex(index);	//����TimerA0װ�غͷ�Ƶ
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;	//���� timerA
			while(TIMER32_CMSIS(BEEP_TIMER32_BASE)->VALUE);				//�ӳٿ����ȴ�, timer32 ���� ����ģʽ
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//�ر� TimerA			
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA ����ֵ ���㣬��ʹ�Ƚ�ģʽGPIO������͵�ƽ������������
				
				
		//��ֹ��		
		}else if (index == cPause){		
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//�ر�TimerA
			while(TIMER32_CMSIS(BEEP_TIMER32_BASE)->VALUE);				//�ӳٿ����ȴ�
		}
		
		g_isPlayingNote = false;
		
	//��ʹ��delay�ӳ٣���ռ��MCU��Դ	
	}else{		// delayMode == BEEP_MODE_NOT_DELAY							
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_IE;	//��Timer32�ж�
		//BITBAND_PERI(TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL, TIMER32_CONTROL_ONESHOT_OFS)= 1;		//����Timer32����ģʽ

		if(index != cPause){		
			SetFrequencyByIndex(index);	//����TimerA0װ�غͷ�Ƶ
			TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL |= TIMER_A_UP_MODE;	//����timerA, TimerA�� �жϺ��� �ر�
			
			
		//��ֹ��		
		}else if (index == cPause){	
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//�ر�TimerA
			//TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA ����ֵ ���㣬��ʹ�Ƚ�ģʽGPIO������͵�ƽ������������
		}
		
		TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL |= TIMER32_CONTROL_ENABLE;	//����Timer32, �����ж�ǰ�������˳�, ������������
		
		//g_isPlayingNote ��  �жϺ���  ����, �� �жϺ��� �ٴε��ñ�����
		
	}
	
}

/**
 * @brief  �����ٶȲ���
 * @param  speed - ��λ��ÿ���Ӷ�����
 * @return ÿ�ĵĶ�ʱ��װ��ֵ  */
uint32_t Beep_SetSpeed(uint32_t speed)
{
	//Timer32Ƶ�� 3MHz
	Beep_currentTickCountPerBeat = (3000000 * 60) / speed;
	return Beep_currentTickCountPerBeat;	
}

// uint32_t Beep_GetSpeed()
// {
// 	return Beep_currentTickCountPerBeat;
// }

/**
 * @brief  ֹͣ��������, ���ⲿ����, Ҳ���Ա��жϺ�������
 * @param  none
 * @return none  */
void Beep_StopMusic(void)
{
	TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL &= ~TIMER32_CONTROL_ENABLE;	//�ر�Timer32
	
	if(g_isPlayingNote == true){
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//�ر� TimerA			
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA ����ֵ ���㣬��ʹ�Ƚ�ģʽGPIO������͵�ƽ������������		
	}
	
	Beep_p_currentMusic = Beep_p_currentNote = (void*)NULL;
	g_isPlayingMusic = g_isPlayingNote = false;


}


/**
 * @brief  ��ʼ��������
 * @param  p_music*[in]  ���׽ṹ��������׵�ַ
 * @return none  */
void Beep_PlayMusic(Beep_note_typedef* p_music)
{
	if(g_isPlayingMusic == true || p_music == (void*)NULL || p_music->pitch != cStart || Beep_isMute == true){
		return;
	}
	
	g_isPlayingMusic = true;	//�򿪱�־λ, �жϺ�����⵽ָ��ָ����ֹ��������
	Beep_p_currentMusic = Beep_p_currentNote = p_music;	//��ָ�븳ֵ
	Beep_p_currentMusic = p_music;
	Beep_p_currentNote = (p_music + 1);
	
		//���ŵ�һ������(���ٶ�����ָ��), ����timer32�ж�
	ReadNote(&Beep_p_currentNote);
}




/**
 * @brief  ��ʼ��������GPIO����ض�ʱ�� ʹ�ñ��ļ�ǰ�������const�ṹ�� 234234
 * @param  speed 	��ʼ�ٶȣ� \b ==0_ʱ_����ʼ���� ���� SetSpeed()
 * @return none  */
void Beep_Init(uint32_t speed)
{
	//	P2.4 TA0.1
	GPIO_setAsPeripheralModuleFunctionOutputPin(BEEP_GPIO_PORT, BEEP_GPIO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
	//GPIO_setAsOutputPin(BEEP_GPIO_PORT, BEEP_GPIO_PIN);
	Timer_A_configureUpMode(BEEP_TIMER_A_BASE, &TimerA0_upConfig);
	Timer_A_initCompare(BEEP_TIMER_A_BASE, &compareConfig_PWM0_1);
	
	//timer32_0Ƶ�ʣ� 48 MHz / 16 = 3MHz
	Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
	BITBAND_PERI(TIMER32_CMSIS(BEEP_TIMER32_BASE)->CONTROL, TIMER32_CONTROL_ONESHOT_OFS)= 1;		//����Timer32����ģʽ
	Timer32_setCount(TIMER32_0_BASE, 10000u);
	Timer32_haltTimer(TIMER32_0_BASE);
	Interrupt_enableInterrupt(INT_T32_INT1);
	Timer32_disableInterrupt(TIMER32_BASE);		//�ر��ж�
	
	//TimerA�жϹص�
	Timer_A_disableInterrupt(BEEP_TIMER_A_BASE);			//
	Interrupt_disableInterrupt(BEEP_TIMER_A_INTERRPUT);		//
	Timer_A_stopTimer(BEEP_TIMER_A_BASE);
	
	//��ʼ���ٶ�
	if(speed != 0){
		Beep_SetSpeed(speed);
	}
	
	
}



//======================== �жϺ��� Interrupt Handlers ===========================

/**
 * @brief  Timer32_0 �жϺ���
 * ��Ҫ���ܣ���delayģʽ�½�����ָ��һ��һ������ƣ��������֣�ֱ����ֹ��ָ�delayģʽ���жϹر�
 */
void T32_INT1_IRQHandler(void)
{
	Timer32_clearInterruptFlag(BEEP_TIMER32_BASE);	//����жϱ�־λ
	
	if(Beep_p_currentNote->pitch <= BEEP_PITCH_INDEX_MAX){		//ֹͣ timerA
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->CTL &= ~TIMER_A_CTL_MC_3;	//�ر�TimerA
		TIMER_A_CMSIS(BEEP_TIMER_A_BASE)->R = 0x0000;	// TimerA ����ֵ ���㣬��ʹ�Ƚ�ģʽGPIO������͵�ƽ������������	
	}
	//Timer32 ���� ����ģʽ ���� ֹͣ
	Beep_p_currentNote++;
	g_isPlayingNote = false;
	
	ReadNote(&Beep_p_currentNote);

}





