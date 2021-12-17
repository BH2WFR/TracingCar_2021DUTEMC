#ifndef __OLED_12864_H__
#define __OLED_12864_H__

/******************************************************************************
 * 	@brief	
 * 
 * ע�������� font.c �� font.h �ļ����棬���� OLED_12864.c �ļ���������
   
     Y                  OLED_12864
      ��                                                      (127, 63)
    63��������������������������������������������������������������������������������������������������������������������
      ��                  Page 7                                ��
    55��������������������������������������������������������������������������������������������������������������������
      ��                  Page 6                                ��
    47��������������������������������������������������������������������������������������������������������������������
      ��                  Page 5                                ��
    39��������������������������������������������������������������������������������������������������������������������
      ��                  Page 4                                ��
    31��������������������������������������������������������������������������������������������������������������������
      ��                  Page 3                                ��
    23��������������������������������������������������������������������������������������������������������������������
      ��                  Page 2                                ��
    15��������������������������������������������������������������������������������������������������������������������
      ��                  Page 1                                ��
    7 ��������������������������������������������������������������������������������������������������������������������
      ��                  Page 0                                ��
    0 �������������������������������������������������������������������������������������������������������������������ة�X
 (0,0)0                                                        127
                                                            21*8 6*8


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
#include "LED.h"

//====================== ���к궨�� Public Macros ===============================
//-----------------OLED�˿ڶ���----------------  	
/**	׼��ʹ��eUSCI_B2  SPI����     ����
 * 	P6.0	ԭ����Ƭѡ���ĳ�RESET					��ʼ��ΪGPIO
 * 	P3.5	����CLK�źţ�SCL��
 * 	P3.6	SDA(SIMO) MOS
 * 	P3.7	DC(����MISO��)���͵�ƽָ��-�ߵ�ƽ���ݣ�   ��ʼ��ΪGPIO	*/
#define OLED_SCL_PORTPIN	GPIO_PORT_P3, GPIO_PIN5
#define OLED_MOSI_PORTPIN	GPIO_PORT_P3, GPIO_PIN6
#define OLED_RST_PORTPIN	GPIO_PORT_P6, GPIO_PIN0
#define OLED_DC_PORTPIN		GPIO_PORT_P3, GPIO_PIN7
	//����ΪOLED�˿ڵ�ƽ�л��궨��
#define OLED_SCL_CLR()	GPIO_setOutputLowOnPin(OLED_SCL_PORTPIN)
#define OLED_SCL_SET()	GPIO_setOutputHighOnPin(OLED_SCL_PORTPIN)

#define OLED_MOSI_CLR()	GPIO_setOutputLowOnPin(OLED_MOSI_PORTPIN)
#define OLED_MOSI_SET()	GPIO_setOutputHighOnPin(OLED_MOSI_PORTPIN)

#define OLED_RST_CLR()	GPIO_setOutputLowOnPin(OLED_RST_PORTPIN)
#define OLED_RST_SET()	GPIO_setOutputHighOnPin(OLED_RST_PORTPIN)

#define OLED_DC_CLR()	GPIO_setOutputLowOnPin(OLED_DC_PORTPIN)
#define OLED_DC_SET()	GPIO_setOutputHighOnPin(OLED_DC_PORTPIN)
//   ������������������������������������������������������������������






#define OLED_DELAY_1MS_CYCLES	4000u		// ��ʱ 1ms ʹ�õĴ�����ѭ����

	// OLED ��Ļ ��ز���
#define OLED_X_WIDTH 		128		//   x  ��Χ 0 ~ 127
#define OLED_Y_WIDTH 		64		//   y  ��Χ 0 ~ 63
#define OLED_PAGE_AMPOUNT	8		// page ��Χ 0 ~ 8
#define OLED_X_6x8_AMOUNT	21		// ÿ���ܷŶ��ٸ� 6*8 �ַ��� �˺궨�岻���ڱ��ļ�

	// ���ִ�С��Ӣ�����ִ�С
#define OLED_TYPE_6X8		0	//  6 * 8   ��С ASCII �ַ�������ʾ�ַ�����21.* 8
#define OLED_TYPE_8X16		1	//  8 * 16  ��С ASCII �ַ�������ʾ�ַ�����16 * 4
#define OLED_TYPE_16X16		2	// 16 * 16  ��С    ���֣�   ����ʾ�ַ�����16 * 4	

	//���͵������ݻ���ָ��
#define OLED_COMMAND		0		
#define OLED_DATA 			1

	//�������1��ʱ��ʹ��Ӳ��SPI��Ϊ0��ʱ��ʹ�����SPI
#define OLED_SEND_HARDWARE	1
#define OLED_SEND_SOFTWARE	0
#define OLED_SEND_MODE		OLED_SEND_HARDWARE

	//ʹ�� SPI ���� I2C
#define OLED_MODE_SPI		0		
#define OLED_MODE_IIC		1
#define OLED_MODE			OLED_MODE_SPI


// #define OLED_XLevelL	0x00
// #define OLED_XLevelH	0x10
// #define OLED_XLevel		((XLevelH & 0x0F) * 16 + XLevelL)
// #define	OLED_Brightness	0xCF 





//============== ����ȫ�ֱ������� Public Global Variables Defines ===============


//================== ���к������� Public Function Defines =======================
extern void OLED_DelayMs(uint32_t);

extern void OLED_SetPosition(uint8_t x, uint8_t page);

extern void OLED_FillWithByte_singlePage(uint8_t bmp_data, uint8_t page_index);
extern void OLED_FillWithByte_partialPage(uint8_t bmp_data, uint8_t page_index, uint8_t x_start, uint8_t x_end);
extern void OLED_ClearAll(void);

extern void OLED_TurnOnDisplay(void);
extern void OLED_TurnOffDisplay(void);

extern void OLED_Restart(void);
extern void OLED_init(void);

extern void OLED_PrintString(char* str, uint8_t start_x, uint8_t page, uint8_t FontType, bool inverse);
extern void OLED_PrintString_Append(char* str, uint8_t FontType, bool inverse);
extern void OLED_PrintSpace(uint8_t length, uint8_t start_X, uint8_t page, uint8_t FontType, bool inverse);
extern void OLED_PrintSpace_Append(uint8_t length, uint8_t FontType, bool inverse);
extern void OLED_DrawBMP_twoPoints(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t end_x, uint8_t end_page);
extern void OLED_DrawBMP_size(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t x_size, uint8_t page_size);



#endif
