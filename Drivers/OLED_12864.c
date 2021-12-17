#include "OLED_12864.h"
#include "FONT.h" 
#include "SysTick_delay.h" 
//#include "interface.h"

//========================== ˽�к궨�� Private Macros ===========================



  

//===================== ����ȫ�ֱ��� Public Global Variables =====================


uint16_t OLED_operatingX    = 0;	// �ϴ� ��λ/д�� �� X ����
uint16_t OLED_operatingPage = 0;	// �ϴ� ��λ/д�� �� ҳ ���


//==================== ˽��ȫ�ֱ��� Private Global Variables =====================
const eUSCI_SPI_MasterConfig SPI_B2_MasterConfig =
{
	.selectClockSource 		= EUSCI_B_SPI_CLOCKSOURCE_SMCLK,				// SMCLK Clock Source
	.clockSourceFrequency 	= 12000000,										// SMCLK = HFXT/4 = 12MHz
	.desiredSpiClock 		= 12000000,										// SPICLK = 12MHz
	.msbFirst 				= EUSCI_B_SPI_MSB_FIRST,						// MSB First
	.clockPhase				= EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,	// Phase ��
	.clockPolarity			= EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,		// Low polarity      ��ģʽ0
	.spiMode				= EUSCI_B_SPI_3PIN								// 3Wire SPI Mode
};





//============ ˽�к��� ������ ��������  Private/Weak/Inline Functions=============

#if (OLED_MODE == OLED_MODE_SPI)
/** 
 * @breif	ʹ�� SPI ����һ���ֽڵ����ݻ�ָ��
 * @param	sendByte[in]		Ҫ���͵��ֽ�
 * @param	isDataOrCommand[in]	���ݻ���ָ���Чֵ��OLED_COMMAND��OLED_DATA
 * @return	none	*/
static void OLED_SPI_WriteByte(uint8_t sendByte, bool isDataOrCommand)
{
	if(isDataOrCommand == OLED_COMMAND){
		OLED_DC_CLR();		//���ָ��
	}else{	// isDataOrCommand == OLED_DATA
		OLED_DC_SET();		//�������
	}
	
	
	#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//���SPI
	
		volatile uint8_t i=8;
		
		OLED_SCL_CLR();		
		//asm("nop");   
		while(i--)
		{
			if(sendByte & 0x80){
				OLED_MOSI_SET();
			}else{
				OLED_MOSI_CLR();
			
			}
			OLED_SCL_SET();
			//asm("nop");
			//asm("nop");             
			OLED_SCL_CLR();    
			sendByte <<= 1;;   
		} 	
		//OLED_CS=1;		//Ƭѡ�ź� ���ﲻʹ��
			
	#else	//OLED_SEND_MODE == OLED_SEND_HARDWARE	Ӳ��SPI
	
		SPI_transmitData(EUSCI_B2_BASE, sendByte);
		while(  !(SPI_getInterruptStatus(EUSCI_B2_BASE, EUSCI_SPI_TRANSMIT_INTERRUPT))  );
	
	#endif
}

#else	// OLED_MODE == OLED_MODE_IIC

static void OLED_IIC_WriteByte(uint8_t sendByte, bool isDataOrCommand)
{
	
}
	//IIC��ش���
	
	
#endif

static inline void OLED_WriteByte(uint8_t sendByte, bool isDataOrCommand)
{
	#if (OLED_MODE == OLED_MODE_SPI)
		OLED_SPI_WriteByte(sendByte, isDataOrCommand);
	#else	// OLED_MODE == OLED_MODE_IIC
		
	#endif
}

static inline void OLED_WriteCommand(uint8_t cmd)
{
	OLED_WriteByte(cmd, OLED_COMMAND);
}

static inline void OLED_WriteData(uint8_t data)
{
	OLED_WriteByte(data, OLED_DATA);
}
//========================= ���к���  Public Functions ===========================

/**
 * @brief 	�����ӳ�
 * @param  time_ms
 * @return none  */
void OLED_DelayMs(uint32_t time_ms)
{
	volatile uint32_t i;
	for(	; time_ms > 0; time_ms--){
		for(i = 0; i < OLED_DELAY_1MS_CYCLES; i++){
			
		}
	}
}



/**
 * @brief 	��������ֵ, y����ʹ�á�ҳ��
 * @param  x[in]		0u ~ 127u
 * @param  page[in]		0u ~ 8u
 * @return none  */
void OLED_SetPosition(uint8_t x, uint8_t page)
{ 
	OLED_WriteCommand(0xb0 + page);					// ҳ ���
	
	OLED_WriteCommand(((x&0xf0)>>4)|0x10);	// x �� 8 λ
	OLED_WriteCommand((x&0x0f)|0x01); 		// x �� 8 λ
	
	OLED_operatingX = x;
	OLED_operatingPage = page;
} 



/**
 * @brief  ����ҳ���, ��䷶Χ��page_index ҳ�� 0 ~ 127 ��Χ
 * @param  bmp_data[in]		�������
 * @param  page_index[in]	Ҫ����ҳ���, 0u ~ 7u
 * @return none  
 * @note   �����Щ��Щ��Щ��Щ��������������� uint8_t bmp_data ���������
           �� �� �� �� ��
   in     �����Щ��Щ��Щ��Щ��Щ��Щ��������Щ���
  Page x  ��7��7��7��7��7�� ��    ��7��
          ��6��6��6��6��6�� ��    ��6��
          ��5��5��5��5��5�� ��    ��5��
          ��4��4��4��4��4�� ��    ��4��
          ��3��3��3��3��3�� ��... ��3��
          ��2��2��2��2��2�� ��    ��2��
          ��1��1��1��1��1�� ��    ��1��
          ��0��0��0��0��0�� ��    ��0��
          �����ة��ة��ة��ة��ة��ة��������ة���
          0  ���������������������������� 127			*/
void OLED_FillWithByte_singlePage(uint8_t bmp_data, uint8_t page_index)
{
	OLED_SetPosition(0, page_index);
	
	for(volatile uint8_t x = 0; x < OLED_X_WIDTH; x++){
		OLED_WriteData(bmp_data);
	}
}

/**
 * @brief  ����ҳ������䣬��䷶Χ��page_index ҳ�� x_start ~ x_end ��Χ
 * @param  bmp_data[in]		�������
 * @param  page_index[in]	Ҫ����ҳ���, 0u ~ 7u
 * @param  x_start[in]		��俪ʼ x ���꣬��Χ 0u ~ 127u
 * @param  x_end[in]		������ x ���꣬��Χ 0u ~ 127u����Ҫ���� x_start
 * @return none  */
void OLED_FillWithByte_partialPage(uint8_t bmp_data, uint8_t page_index, uint8_t x_start, uint8_t x_end)
{
	OLED_SetPosition(x_start, page_index);
	
	for(volatile uint8_t x = x_start; x <= x_end; x++){
		OLED_WriteData(bmp_data);
	}	
}


// /**
//  * @brief  ȫ��Ļ��䣬���� OLED_FillWithByte_singlePage(@, @)
//  * @param  bmp_data[in]	�������
//  * @return none  */
// void OLED_FillWithByte_all(uint8_t bmp_data)
// {
// 		// 0 - 8 ҳһ��һ�������������
// 	for(volatile uint8_t page = 0; page < OLED_PAGE_AMPOUNT; page++)
// 	{
// 		OLED_FillWithByte_singlePage(bmp_data, page);
// 	}
// }

/**
 * @brief  ȫ��Ļ���㣬���� OLED_FillWithByte_singlePage(0x00, page)
 * @param  none
 * @return none  */
void OLED_ClearAll(void)
{
		// 0 - 8 ҳһ��һ�������������
	for(volatile uint8_t page = 0; page < OLED_PAGE_AMPOUNT; page++)
	{
		OLED_FillWithByte_singlePage(0x00, page);
	}
}




/**
 * @brief  ������Ļ��ʾ
 * @param  none
 * @return none  */
void OLED_TurnOnDisplay(void)
{
	OLED_WriteCommand(0x8D);	//--��ɱ�ʹ��
	OLED_WriteCommand(0x14);	//	������ɱã�0x10 Ϊ�رյ�ɱ�	
	OLED_WriteCommand(0xAF);	//������Ļ
}

/**
 * @brief  �ر���Ļ��ʾ
 * @param  none
 * @return none  */
void OLED_TurnOffDisplay(void)
{
	OLED_WriteCommand(0x8D);	//--��ɱ�ʹ��
	OLED_WriteCommand(0x10);	//	�رյ�ɱ�
	OLED_WriteCommand(0xAE);	//�ر���Ļ	
}


/**
 * @brief  ������Ļ�����ͳ�ʼ�����ã��� OLED_init() ����
 * @param  none
 * @return none 
 * @note	Remocon_Keys.c �� ��ʼ������ʱ ��һ�������Ķ�Ӧ�¼�������ָ�� ָ�������������
 * 			�� ����Ļ��������ʱ�����¸ð�ť��������������������Ļ 
 * �����Ҫ�޸ģ����� "Remocon_Keys.c" -> Key_EventFunctionVector_ResetFunctionalKeys() ���� ����*/
void OLED_Restart(void)
{
	uint8_t shabi = LED2_colour;
	
	LED2_switch(LED2_COLOUR_CYAN);
	
		//���������źţ�RST ��ƽ���� 50 ms ������
	OLED_RST_CLR();		
	OLED_DelayMs(50);
	OLED_RST_SET();
	
	{
		OLED_WriteCommand(0xAE);	//	�ر���ʾ
		
		#define OLED_WrCmd OLED_WriteCommand
		OLED_WriteCommand(0xAE);//--turn off oled panel
		OLED_WriteCommand(0x00);//---set low column address
		OLED_WriteCommand(0x10);//---set high column address
		OLED_WriteCommand(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
		OLED_WriteCommand(0x81);//--set contrast control register
		OLED_WriteCommand(0xA7);// Set SEG Output Current Brightness
		
		OLED_WriteCommand(0xA1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
		OLED_WriteCommand(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
		OLED_WriteCommand(0xA6);//--set normal display
		OLED_WriteCommand(0xA8);//--set multiplex ratio(1 to 64)
		OLED_WriteCommand(0x3F);//--1/64 duty
		OLED_WriteCommand(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
		OLED_WriteCommand(0x00);//-not offset
		OLED_WriteCommand(0xD5);//--set display clock divide ratio/oscillator frequency
		OLED_WriteCommand(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
		OLED_WriteCommand(0xD9);//--set pre-charge period
		OLED_WriteCommand(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
		OLED_WriteCommand(0xDA);//--set com pins hardware configuration
		OLED_WriteCommand(0x12);
		OLED_WriteCommand(0xDB);//--set vcomh
		OLED_WriteCommand(0x40);//Set VCOM Deselect Level
		OLED_WriteCommand(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
		OLED_WriteCommand(0x02);//
		OLED_WriteCommand(0x8D);//--set Charge Pump enable/disable
		OLED_WriteCommand(0x14);//--set(0x10) disable
		OLED_WriteCommand(0xA4);// Disable Entire Display On (0xa4/0xa5)
		OLED_WriteCommand(0xA6);// Disable Inverse Display On (0xa6/a7) 
			
		OLED_WriteCommand(0xAF);	//	������ʾ
	}
	
	OLED_ClearAll();  			//����
	OLED_SetPosition(0,0);  	//����ŵ� ԭ��
	
#ifdef __INTERFACE_H__
	Interface_DisplayFirstLine();
#endif
	
	
	
	LED2_switch(shabi);
	
}




/**
 * @brief  ��ʼ�� OLED ģ��
 * @param  none
 * @return none  */
void OLED_init(void)
{
	GPIO_setAsOutputPin(OLED_DC_PORTPIN);
	GPIO_setAsOutputPin(OLED_RST_PORTPIN);
	OLED_RST_CLR();	
	
	
	#if (OLED_MODE == OLED_MODE_SPI)	//SPI ģʽ
	
		#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//���SPI
			GPIO_setAsOutputPin(OLED_SCL_PIN);
			GPIO_setAsOutputPin(OLED_MOSI_PIN);
		
			OLED_SCL_CLR();
			//OLED_CS=1;	//Ԥ��SLK��SSΪ�ߵ�ƽ   	

		#else  // OLED_SEND_MODE == OLED_SEND_HARDWARE	//Ӳ��SPI
			GPIO_setAsPeripheralModuleFunctionOutputPin(OLED_MOSI_PORTPIN, GPIO_PRIMARY_MODULE_FUNCTION);	//����
			GPIO_setAsPeripheralModuleFunctionOutputPin(OLED_SCL_PORTPIN, GPIO_PRIMARY_MODULE_FUNCTION);
			
			SPI_initMaster(EUSCI_B2_BASE, &SPI_B2_MasterConfig);	//�� SPI ģ��
			SPI_enableModule(EUSCI_B2_BASE);
			SPI_disableInterrupt(EUSCI_B2_BASE, 
						EUSCI_SPI_RECEIVE_INTERRUPT | EUSCI_SPI_TRANSMIT_INTERRUPT);//�ر��ж�

		#endif
		
	#else	//(OLED_MODE == OLED_MODE_IIC)	//IIC ģʽ
	
		#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//���IIC
			GPIO_setAsOutputPin(OLED_SCL_PIN);
			GPIO_setAsOutputPin(OLED_MOSI_PIN);
		
			OLED_SCL_CLR();  	

		#else  // OLED_SEND_MODE == OLED_SEND_HARDWARE	//Ӳ��IIC


		#endif	
		
		
	#endif
	
		//����Ҫ�Ĳ��裺
	OLED_Restart();		//��ʼ���ͳ�ʼ��ָ������������

} 


//========����Ϊ�ַ���ʾ��������Ҫ���� font.h ͷ�ļ�����ʹ��=======
#ifdef __FONT_H__

/**
 * @brief  ����Ļ�ϴ�ӡ�ַ���
 * @param  str*[in]	�ַ����׵�ַ
 * @param  start_x	�ַ����ĸ� x ���꿪ʼ��ʾ��
 * @param  page		�ַ���ʾ����һҳ
 * @param  FontType	�����С����Чֵ�� \b OLED_TYPE_6X8
 * @param  inverse	�Ƿ�ȡ����ʾ���׵׺��֣�1 Ϊȡ��
 * @return none  
 * @note	����ʾ����OLED_PrintString(strBuf, 12, 3, OLED_TYPE_6X8, 0);*/
void OLED_PrintString(char* str, uint8_t start_x, uint8_t page, uint8_t FontType, bool inverse)
{
	char *p_str = str;
	uint8_t index = 0;				//��ǰ��ʾ���ַ������ַ���ƫ����
	uint8_t current_x = start_x;	//��ǰ����x���꣬ÿ������һ�� �������ַ� �� ������Ӧ�Ŀ��
	uint8_t writeBuf		= 0x00;
	//uint8_t current_page = page;
	OLED_SetPosition(current_x, page);	//�ı�����

	switch(FontType){
		case OLED_TYPE_6X8:
			while(p_str[index] != '\0'){
				
				
				for(uint8_t i = 0; i < 6; i++){		// ��������x��дһ���������ַ�
					
					writeBuf = Font_ASCII_6x8[ 6 * p_str[index]  +  i];
					
					if(inverse == 1){	//ȡ����ʾ
						writeBuf = ~writeBuf;
					}
					
					OLED_WriteData(writeBuf);	//д������
					OLED_operatingX++;

					current_x++;
					if(current_x == 128){
						current_x = 0;
					}
				}
				
				index++;	//ƫ��������
			}
			
			break;
		case OLED_TYPE_8X16:
			
			break;
		// case OLED_TYPE_16X16:	//������ʾ
			
		// 	break;
		default:
			break;
	}
}

/**
 * @brief  �� �ϴδ�ӡ�ַ���λ�� \b �Ҳ� \b ������ ��ӡ�ַ� 
 * @param  str*[in]	�ַ����׵�ַ
 * @param  FontType	�����С����Чֵ�� \b OLED_TYPE_6X8
 * @param  inverse	�Ƿ�ȡ����ʾ���׵׺��֣�1 Ϊȡ��
 * @return none  
 * @note	����ʾ����OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintString_Append(char* str, uint8_t FontType, bool inverse)
{
	OLED_PrintString(str, (OLED_operatingX + 1), OLED_operatingPage, FontType, inverse);
	
}

/**
 * @brief  ��ӡ�ո� 
 * @param  length	�ո񳤶�, ��λΪ �ַ����
 * @param  start_x	�ַ����ĸ� x ���꿪ʼ��ʾ��
 * @param  page		�ַ���ʾ����һҳ
 * @param  FontType	�����С����Чֵ�� \b OLED_TYPE_6X8
 * @param  inverse	�Ƿ�ȡ����ʾ���׵׺��֣�1 Ϊȡ��
 * @return none  
 * @note	����ʾ����OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintSpace(uint8_t length, uint8_t start_X, uint8_t page, uint8_t FontType, bool inverse)
{
	uint8_t writeBuf = 0x00;
	
	OLED_SetPosition(start_X, page);	//�ı�����
	
	if(inverse == true){
		writeBuf = 0xFF;
	}else{
		writeBuf = 0x00;
	}
	
	
	
	switch(FontType){
		case OLED_TYPE_6X8:
		
			length *= 6u;	// �ַ���� ����
			for(uint8_t i = 0; i < length; i++){
				OLED_WriteData(writeBuf);	//д������
				OLED_operatingX++;
			}
			
			break;
		case OLED_TYPE_8X16:
			
			break;
		// case OLED_TYPE_16X16:	//������ʾ
			
		// 	break;
		default:
			break;
	}	
}

/**
 * @brief  �� �ϴδ�ӡ�ַ���λ�� \b �Ҳ� \b ������ ��ӡ \b �ո� 
 * @param  length	�ո񳤶�, ��λΪ �ַ����
 * @param  FontType	�����С����Чֵ�� \b OLED_TYPE_6X8
 * @param  inverse	�Ƿ�ȡ����ʾ���׵׺��֣�1 Ϊȡ��
 * @return none  
 * @note	����ʾ����OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintSpace_Append(uint8_t length, uint8_t FontType, bool inverse)
{
	OLED_PrintSpace(length, (OLED_operatingX + 1), OLED_operatingPage, FontType, inverse);
}



#endif

#ifdef __OLED_GRAPHICS_H__




#endif

/**
 * @brief  ���㷨��λ ��ʾλͼ����ʽ��uint8_t(һҳ) * ����
 * @param  BMP *[in]	λͼ����ָ��
 * @param  start_x		��ʼ�� x ����
 * @param  start_page	��ʼ�� ҳ���
 * @param  end_x		ĩβ�� x ���꣨����x!) ����16*8 bmp from (0,0): end_x == 15
 * @param  end_page		ĩβ�� ҳ��ţ�������ҳ��������16*16 bmp from(0,0): end_page == 1
 * @return none  
 * @note	λͼ����˳��
 *        0   1   2     uint8_t[]
        ���������Щ������Щ�������
  page 0�� 1 �� 2 �� 3 ��
        �� �� �� �� �� �� ��
        ���������੤�����੤������
  page 1�� 4 �� 5 �� 6 ��
        �� �� �� �� �� �� ��
        ���������ة������ة�������		*/
void OLED_DrawBMP_twoPoints(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t end_x, uint8_t end_page)
{
	volatile uint16_t bmpIndex = 0;
	
	for(uint32_t i = start_page; i <= end_page; i++){	// y (page)
		OLED_SetPosition(start_x, i);	// �������� (x, page) 
		
		for(uint32_t x = start_x; x <= end_x; x++){			// x
			OLED_WriteData(BMP[bmpIndex]);
			bmpIndex++;
		}
		
	}
	
	
}

/**
 * @brief  һ��+�ߴ綨λ ��ʾλͼ����ʽ��uint8_t(һҳ) * ����
 * @param  BMP *[in]	λͼ����ָ��
 * @param  start_x		��ʼ�� x ����
 * @param  start_page	��ʼ�� ҳ���
 * @param  x_size		λͼ x ����ĳߴ�
 * @param  page_size	λͼ ҳ �ߴ磨��Сֵ 1 ��
 * @return none  */
void OLED_DrawBMP_size(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t x_size, uint8_t page_size)
{
	volatile uint16_t bmpIndex = 0;
	
	for(uint32_t i = 0; i < page_size; i++){	// y (page)
		OLED_SetPosition(start_x, i);	// �������� (x, page) 
		
		for(uint32_t x = start_x; x < (start_x + x_size); x++){			// x
			OLED_WriteData(BMP[bmpIndex]);
			bmpIndex++;
		}
		
	}	
}

//======================== �жϺ��� Interrupt Handlers ===========================
