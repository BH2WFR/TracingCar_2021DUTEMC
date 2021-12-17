#if defined __MSP432P401R__
#include "SysTick_delay.h"
#include "LED.h" 

//============ȫ�ֱ������ṹ�嶨��============

static uint32_t MCLK_Frequency = 3000000;//��SysTickDelay_init()������ʹ��AP_CS_getMCLK()�⺯������MCLK��Ƶ�ʣ��������������
static bool SysTickDelay_isInitialized = false;
bool flag_Systick_in_use = false;



//===============����===================


void SysTickDelay_init(void)
{
	MCLK_Frequency = MAP_CS_getMCLK();	//��ȡMCLKƵ�ʷ���MCLK_Frequency������
//	MAP_SysTick_disableModule();
//	MAP_SysTick_disableInterrupt();
	MAP_Interrupt_enableMaster();
	SysTickDelay_isInitialized = true;
}

void SysTick_delay_us(uint32_t time_us)//��Ҫ����60000us   �����ʱ����׼������������PWM��
{

	if(SysTickDelay_isInitialized == false || time_us > 60000 ){
		return;
	}else if (flag_Systick_in_use == true){	//���Systick�Ѿ�����ʹ��״̬����ʹ�ÿ�ѭ����ʱ
		LED2_switch(LED2_COLOUR_YELLOW);
		switch(MAP_CS_getMCLK()){
			case 12000000:
				for(volatile uint64_t aa = 2 * time_us - 2; aa > 0; aa--);
				break;
			case 24000000:
				for(volatile uint64_t aa = 4 * time_us - 2; aa > 0; aa--);
				break;
			case 48000000:
				for(volatile uint64_t aa = 8 * time_us - 2; aa > 0; aa--);
				break;
		}
		
		return;
	}else{
		uint32_t period;		//����ֵ���㣬ʹ��ȫ�ֱ���MCLK_Frequency���㣬��ֵ�ڳ�ʼ��ʱ�Ѿ��ӿ⺯�����룬����CPU��Դ�˷�
		uint16_t cycle = 0;
		flag_Systick_in_use = true;
		period = (uint32_t)((MCLK_Frequency / 1000000) * time_us);
		cycle = period / 16777215;
		period = period - (cycle * 16777215);//or period = period % 16777215
		while(cycle != 0)						//
		{
			SysTick_setPeriod (16777215);
			SysTick->CTRL |= 1<<0;
			while(!(SysTick->CTRL & 1<<16));
			SysTick->CTRL &= ~(1<<0);
			cycle--;
		}
		SysTick_setPeriod (period);
		SysTick->CTRL |= 1<<0;
		while(!(SysTick->CTRL & 1<<16));
		SysTick->CTRL &= ~(1<<0);
		flag_Systick_in_use = false;
		
	}
}

void SysTick_delay_ms(uint32_t time_ms)//��Ҫ����60000ms   �����ʱ����׼������������PWM��
{

	if(SysTickDelay_isInitialized == false || time_ms > 60000 ){
		return;
	}else if (flag_Systick_in_use == true){	//���Systick�Ѿ�����ʹ��״̬����ʹ�ÿ�ѭ����ʱ
		LED2_switch(LED2_COLOUR_YELLOW);
		switch(MAP_CS_getMCLK()){
			case 12000000:
				for(volatile uint64_t aa = 2000 * time_ms - 2; aa > 0; aa--);
				break;
			case 24000000:
				for(volatile uint64_t aa = 4000 * time_ms - 2; aa > 0; aa--);
				break;
			case 48000000:
				for(volatile uint64_t aa = 8000 * time_ms - 2; aa > 0; aa--);
				break;
		}
		
		return;
	}else{
		uint32_t period;		//����ֵ���㣬ʹ��ȫ�ֱ���MCLK_Frequency���㣬��ֵ�ڳ�ʼ��ʱ�Ѿ��ӿ⺯�����룬����CPU��Դ�˷�
		uint16_t cycle = 0;
		flag_Systick_in_use = true;
		period = (uint32_t)((MCLK_Frequency / 1000) * time_ms);
		cycle = period / 16777215;
		period = period - (cycle * 16777215);//or period = period % 16777215
		while(cycle != 0)						//
		{
			MAP_SysTick_setPeriod (16777215);
			SysTick->CTRL |= 1<<0;
			while(!(SysTick->CTRL & 1<<16));
			SysTick->CTRL &= !(1<<0);
			cycle--;
		}
		MAP_SysTick_setPeriod (period);
		SysTick->CTRL |= 1<<0;
		while(!(SysTick->CTRL & 1<<16));
		SysTick->CTRL &= !(1<<0);
		flag_Systick_in_use = false;
		
	}
}
#endif
