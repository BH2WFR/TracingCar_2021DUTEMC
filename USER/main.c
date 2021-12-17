
/******************************************************************************
	MSP432P401R LAUNCHPADר��
	ע�⣺���ļ�ֻ���main������һЩ��Ҫ���жϷ������


	

	
	
 *******************************************************************************/
 
 
//================== ����ͷ�ļ�  Header Files Includes =======================
	//��Ƭ���⺯��ͷ�ļ�
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

	//c��׼ͷ�ļ�(ANSI C)
#include <stdio.h>  	//��������ַ�����sprintf������
#include <stdint.h>		//������ uint8_t �� һϵ�з���ĺ궨��
#include <stdlib.h>		//���� malloc(@) free(@) rand()��
#include <math.h>  		//��ѧ��
#include <string.h>		//�ַ���(char����)����, �ڴ���д����(memcpy($)��)
//#include <ctype.h>		//�ַ�����
//#include <setjmp.h>		//��ת����
//#include <stdarg.h>		//��κ���ʹ��
//#include <time.h>			//
//#include <errno.h>		//
//#include <stddef.h>		//
//#include <limits.h>		//�����и��ֱ��������ֵ/��Сֵ����
//#include <float.h>		//
//#include <locale.h>		//���ػ�֧��

	//c��׼ͷ�ļ�(C99)
#include <stdbool.h>		//����֧��bool(����)����
//#include <complex.h>		//��������
//#include <tgmath.h>		//�����ж��������͵���ѧ����

	//c��׼ͷ�ļ�(C11)
//#include <uchar.h> 		//Unicode֧��

	//�û�ģ��ͷ�ļ�
#include "main.h"		//�������

#include "UART0.h" 
#include "SysTick_delay.h" 
#include "LED.h" 
#include "OLED_12864.h" 
#include "beep.h"
#include "Beep_song.h"
#include "Remocon_Keys.h"
#include "Wireless.h"
#include "interface.h"
#include "Motor.h"
#include "TracingSensor.h"





//======================= ˽�к궨�� Private Macros ========================



//================= ����ȫ�ֱ��� Public Global Variables======================

//================= ˽��ȫ�ֱ��� Private Global Variables======================

volatile uint64_t main_circulateCount = 0;	// ������ ѭ��������һֱ���ӣ�������������ȡ������ %xxxx ������ �ﵽ ��ʱ�� Ч��

bool	main_testSign0 = false;				// ��������ģ��Ĳ��ԣ����ݲ���
bool	main_testSign1 = false;				// ��������ģ��Ĳ��ԣ����ݲ���
bool	main_testSign2 = false;				// ��������ģ��Ĳ��ԣ����ݲ���
bool	main_testSign3 = false;				// ��������ģ��Ĳ��ԣ����ݲ���
bool	main_testSign4 = false;				// ��������ģ��Ĳ��ԣ����ݲ���

char   main_versionStr[24] = "Ver:0.3.1 \x03";

//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

void main_init(void)
{
	
	//======ϵͳ����ģ���ʼ�����루����Ķ���====
    WDT_A_holdTimer();									// ���Ź��ص�
	
	FPU_enableModule();									// �����������㵥Ԫ
	FPU_enableLazyStacking();
	
	PCM_setCoreVoltageLevel(PCM_VCORE1);				// ̧�ߵ�ѹ֧�ָ�Ƶ��
	CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);	// DCO ʱ������Ϊ 24 MHz
	CS_setExternalClockSourceFrequency(32000,48000000);	// �����ⲿ�����Ƶ��

	
	//���¼���Ϊ��SMCLKʱ�ӹҵ�HFXT�ϣ�Ƶ��Ϊ48MHz
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
		GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);			
															// GPIO �ڸ��� PJ2 �� PJ3 Ϊ ��һ���ù��ܣ�HFXT ���壩
    FlashCtl_setWaitState(FLASH_BANK0, 1);					// ��˵����Ƭ��FLASHʱ��Ҫ�����������������������ʹ��FLASH��ɾ��
    FlashCtl_setWaitState(FLASH_BANK1, 1);
	
	//SysCtl_enableSRAMBankRetention(SYSCTL_SRAM_BANK1);
	
    CS_startHFXT(false);													// HFXT����
	CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_4);	// SMCLK �ҵ� HFXT �ϣ���ƵΪ 4, 12 MHz
	CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);		// MCLK  �ҵ� HFXT �ϣ���ƵΪ 1��48 MHz
	
	
	//ACLK�ҵ�LFXT�ϣ�Ƶ��32768Hz
	GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
		GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
	CS_startLFXT(CS_LFXT_DRIVE3);
	CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);		// ACLK �ҵ� LFXT �ϣ���ƵΪ1��3.2kHz
	
}

void user_init(void)
{
	//ֻ�ܳ�ʼ������ģ�飬���ܳ�ʼ�����⣡
	
	OLED_init();
	
	Key_init();
	
	#ifdef __POTENTIOMETER_H__
		Potentiometer_Init();
	#endif
	
	
	#ifdef __REMOCON_ENCODER
		Encoder_init();
	#endif
	
	#ifdef __TRACING_SENSOR_H__
		Sensor_init(false);
	#endif
	
	#ifdef __BEEP_H__
		Beep_Init(120u);
		
	#endif
	
	#ifdef __WIRELESS_H__
		Wireless_init();
	#endif
	
	#ifndef __MODE_MOTOR_TEST_H__
		Motor_init(false);
		//Motor_Start();
		
	#endif
	
	
}

/**
 * @brief  main����
 * 			 */
int main(void)	//���
{
	
	//=====�������ֲ�����������======
	
	LED_Init();						//��������� LED ��ʼ������
	LED2_switch(LED2_COLOUR_BLUE);
	
	//ϵͳ��ʼ��
	main_init();					//��ʼ������ʱ�ӡ�ģ��ȵ�
	
	//=====�û�ģ��ĳ�ʼ������=======
	SysTickDelay_init();			//����SysTick����ʱ����
	UART0_init();					//����UART0�������豸����Է���ʵʱ����
	
	user_init();	
	
	//�������ж�
	Interrupt_enableMaster();	

	LED2_switch(LED2_COLOUR_BLACK);
	
	//====��������ѭ��====
	Interface_init();
	
	Beep_PlayMusic(music_Start);
	
    while(1)
    {
		{
			main_circulateCount++;
			
			LED1_toggle();
			Key_Detect();
			
			
//			#define SHABI	2000
//			if(main_testSign0 == true){
//				if(main_circulateCount % SHABI == 0){
//					sprintf(Wireless_sendBuf, "This is a Wireless test... %08u\r\n", main_circulateCount/SHABI); 
//					Wireless_sendString(Wireless_sendBuf, WIRELESS_DELAY_UNUSE, 0u);
//				}
//			}

			
			Interface_modeConfig.Cycle_func();
			
			
			
		}
	
	
	
	}
	//return 0;
}








//==================== �жϷ�������==========================

/**
 * @brief  HardFault �ж�
 * 			�ڴ��Ӳ��ģ�鷢������(�����ǳ����Ӷ�̫�ߵ��¶�ջ��ը) ʱ LED2 ��������� */
void HardFault_Handler(void)
{
#ifdef __OLED_12864_H__
	OLED_ClearAll();
	sprintf(UART0_string, "-> HardFault_ERROR");
	OLED_PrintString(UART0_string, 12, 3, OLED_TYPE_6X8, 0);	
	sprintf(UART0_string, " :(  OOPS! ");
	OLED_PrintString(UART0_string, 6, 1, OLED_TYPE_6X8, 1);	
#endif
	LED1_switch(0);
	Interrupt_disableMaster();
	
	while(1)
	{
		LED1_toggle();
		LED2_toggle(LED2_COLOUR_BLUE);
		SysTick_delay_ms(100);
	}
}




// /**
//  * @brief  MemManage �ж�
//  * 		��Ҫʹ�����ж�, ����Ĭ�Ͻ�HardFault 	 */
// void MemManage_Handler(void)
// {
	
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, "-> MemManage_ERROR");
// 	OLED_Print(12, 36, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// 	sprintf(UART0_string, ":(");
// 	OLED_Print(6, 8, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }

// /**
//  * @brief  BusFault �ж�
//  * 		��Ҫʹ�����ж�, ����Ĭ�Ͻ�HardFault 	 */
// void BusFault_Handler(void)
// {
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, "-> BusFault_ERROR");
// 	OLED_Print(12, 36, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// 	sprintf(UART0_string, ":(");
// 	OLED_Print(6, 8, UART0_string, OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }

// /**
//  * @brief  UsageFault �ж�
//  * 		��Ҫʹ�����ж�, ����Ĭ�Ͻ�HardFault 		 */
// void UsageFault_Handler(void)
// {
// #ifdef __OLED_12864_H__
// 	OLED_CLS();
// 	sprintf(UART0_string, " UsageFault_ERROR ");
// 	OLED_Print(10, 24, UART0_string,OLED_TYPE16X16,OLED_TYPE6X8);	
// #endif
// 	while(1)
// 	{
// 		LED2_toggle(LED2_COLOUR_RED);
// 		SysTick_delay_ms(150);
// 	}
// }



