#include "OLED_12864.h"
#include "FONT.h" 
#include "SysTick_delay.h" 
//#include "interface.h"

//========================== 私有宏定义 Private Macros ===========================



  

//===================== 公有全局变量 Public Global Variables =====================


uint16_t OLED_operatingX    = 0;	// 上次 定位/写入 的 X 座标
uint16_t OLED_operatingPage = 0;	// 上次 定位/写入 的 页 序号


//==================== 私有全局变量 Private Global Variables =====================
const eUSCI_SPI_MasterConfig SPI_B2_MasterConfig =
{
	.selectClockSource 		= EUSCI_B_SPI_CLOCKSOURCE_SMCLK,				// SMCLK Clock Source
	.clockSourceFrequency 	= 12000000,										// SMCLK = HFXT/4 = 12MHz
	.desiredSpiClock 		= 12000000,										// SPICLK = 12MHz
	.msbFirst 				= EUSCI_B_SPI_MSB_FIRST,						// MSB First
	.clockPhase				= EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,	// Phase ┐
	.clockPolarity			= EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,		// Low polarity      ┘模式0
	.spiMode				= EUSCI_B_SPI_3PIN								// 3Wire SPI Mode
};





//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

#if (OLED_MODE == OLED_MODE_SPI)
/** 
 * @breif	使用 SPI 发送一个字节的数据或指令
 * @param	sendByte[in]		要发送的字节
 * @param	isDataOrCommand[in]	数据还是指令，有效值：OLED_COMMAND，OLED_DATA
 * @return	none	*/
static void OLED_SPI_WriteByte(uint8_t sendByte, bool isDataOrCommand)
{
	if(isDataOrCommand == OLED_COMMAND){
		OLED_DC_CLR();		//输出指令
	}else{	// isDataOrCommand == OLED_DATA
		OLED_DC_SET();		//输出数据
	}
	
	
	#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//软件SPI
	
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
		//OLED_CS=1;		//片选信号 这里不使用
			
	#else	//OLED_SEND_MODE == OLED_SEND_HARDWARE	硬件SPI
	
		SPI_transmitData(EUSCI_B2_BASE, sendByte);
		while(  !(SPI_getInterruptStatus(EUSCI_B2_BASE, EUSCI_SPI_TRANSMIT_INTERRUPT))  );
	
	#endif
}

#else	// OLED_MODE == OLED_MODE_IIC

static void OLED_IIC_WriteByte(uint8_t sendByte, bool isDataOrCommand)
{
	
}
	//IIC相关代码
	
	
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
//========================= 公有函数  Public Functions ===========================

/**
 * @brief 	卡死延迟
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
 * @brief 	设置座标值, y座标使用「页」
 * @param  x[in]		0u ~ 127u
 * @param  page[in]		0u ~ 8u
 * @return none  */
void OLED_SetPosition(uint8_t x, uint8_t page)
{ 
	OLED_WriteCommand(0xb0 + page);					// 页 序号
	
	OLED_WriteCommand(((x&0xf0)>>4)|0x10);	// x 高 8 位
	OLED_WriteCommand((x&0x0f)|0x01); 		// x 低 8 位
	
	OLED_operatingX = x;
	OLED_operatingPage = page;
} 



/**
 * @brief  单个页填充, 填充范围：page_index 页的 0 ~ 127 范围
 * @param  bmp_data[in]		填充数据
 * @param  page_index[in]	要填充的页序号, 0u ~ 7u
 * @return none  
 * @note   ┌─┬─┬─┬─┬──────── uint8_t bmp_data 往这里填充
           ▼ ▼ ▼ ▼ ▼
   in     ┌─┬─┬─┬─┬─┬─┬────┬─┐
  Page x  │7│7│7│7│7│ │    │7│
          │6│6│6│6│6│ │    │6│
          │5│5│5│5│5│ │    │5│
          │4│4│4│4│4│ │    │4│
          │3│3│3│3│3│ │... │3│
          │2│2│2│2│2│ │    │2│
          │1│1│1│1│1│ │    │1│
          │0│0│0│0│0│ │    │0│
          └─┴─┴─┴─┴─┴─┴────┴─┘
          0  ─────────────→ 127			*/
void OLED_FillWithByte_singlePage(uint8_t bmp_data, uint8_t page_index)
{
	OLED_SetPosition(0, page_index);
	
	for(volatile uint8_t x = 0; x < OLED_X_WIDTH; x++){
		OLED_WriteData(bmp_data);
	}
}

/**
 * @brief  单个页部分填充，填充范围：page_index 页的 x_start ~ x_end 范围
 * @param  bmp_data[in]		填充数据
 * @param  page_index[in]	要填充的页序号, 0u ~ 7u
 * @param  x_start[in]		填充开始 x 座标，范围 0u ~ 127u
 * @param  x_end[in]		填充结束 x 座标，范围 0u ~ 127u，且要大于 x_start
 * @return none  */
void OLED_FillWithByte_partialPage(uint8_t bmp_data, uint8_t page_index, uint8_t x_start, uint8_t x_end)
{
	OLED_SetPosition(x_start, page_index);
	
	for(volatile uint8_t x = x_start; x <= x_end; x++){
		OLED_WriteData(bmp_data);
	}	
}


// /**
//  * @brief  全屏幕填充，调用 OLED_FillWithByte_singlePage(@, @)
//  * @param  bmp_data[in]	填充数据
//  * @return none  */
// void OLED_FillWithByte_all(uint8_t bmp_data)
// {
// 		// 0 - 8 页一个一个从左往右填充
// 	for(volatile uint8_t page = 0; page < OLED_PAGE_AMPOUNT; page++)
// 	{
// 		OLED_FillWithByte_singlePage(bmp_data, page);
// 	}
// }

/**
 * @brief  全屏幕清零，调用 OLED_FillWithByte_singlePage(0x00, page)
 * @param  none
 * @return none  */
void OLED_ClearAll(void)
{
		// 0 - 8 页一个一个从左往右填充
	for(volatile uint8_t page = 0; page < OLED_PAGE_AMPOUNT; page++)
	{
		OLED_FillWithByte_singlePage(0x00, page);
	}
}




/**
 * @brief  开启屏幕显示
 * @param  none
 * @return none  */
void OLED_TurnOnDisplay(void)
{
	OLED_WriteCommand(0x8D);	//--电荷泵使能
	OLED_WriteCommand(0x14);	//	开启电荷泵，0x10 为关闭电荷泵	
	OLED_WriteCommand(0xAF);	//点亮屏幕
}

/**
 * @brief  关闭屏幕显示
 * @param  none
 * @return none  */
void OLED_TurnOffDisplay(void)
{
	OLED_WriteCommand(0x8D);	//--电荷泵使能
	OLED_WriteCommand(0x10);	//	关闭电荷泵
	OLED_WriteCommand(0xAE);	//关闭屏幕	
}


/**
 * @brief  重启屏幕，发送初始化设置，由 OLED_init() 调用
 * @param  none
 * @return none 
 * @note	Remocon_Keys.c 中 初始化按键时 有一个按键的对应事件处理函数指针 指向了这个函数，
 * 			当 在屏幕发生错乱时，按下该按钮，本函数被触发重启屏幕 
 * 如果需要修改，请上 "Remocon_Keys.c" -> Key_EventFunctionVector_ResetFunctionalKeys() 函数 更改*/
void OLED_Restart(void)
{
	uint8_t shabi = LED2_colour;
	
	LED2_switch(LED2_COLOUR_CYAN);
	
		//发送重启信号，RST 电平拉低 50 ms 后拉高
	OLED_RST_CLR();		
	OLED_DelayMs(50);
	OLED_RST_SET();
	
	{
		OLED_WriteCommand(0xAE);	//	关闭显示
		
		#define OLED_WrCmd OLED_WriteCommand
		OLED_WriteCommand(0xAE);//--turn off oled panel
		OLED_WriteCommand(0x00);//---set low column address
		OLED_WriteCommand(0x10);//---set high column address
		OLED_WriteCommand(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
		OLED_WriteCommand(0x81);//--set contrast control register
		OLED_WriteCommand(0xA7);// Set SEG Output Current Brightness
		
		OLED_WriteCommand(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
		OLED_WriteCommand(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
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
			
		OLED_WriteCommand(0xAF);	//	开启显示
	}
	
	OLED_ClearAll();  			//清屏
	OLED_SetPosition(0,0);  	//座标放到 原点
	
#ifdef __INTERFACE_H__
	Interface_DisplayFirstLine();
#endif
	
	
	
	LED2_switch(shabi);
	
}




/**
 * @brief  初始化 OLED 模块
 * @param  none
 * @return none  */
void OLED_init(void)
{
	GPIO_setAsOutputPin(OLED_DC_PORTPIN);
	GPIO_setAsOutputPin(OLED_RST_PORTPIN);
	OLED_RST_CLR();	
	
	
	#if (OLED_MODE == OLED_MODE_SPI)	//SPI 模式
	
		#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//软件SPI
			GPIO_setAsOutputPin(OLED_SCL_PIN);
			GPIO_setAsOutputPin(OLED_MOSI_PIN);
		
			OLED_SCL_CLR();
			//OLED_CS=1;	//预制SLK和SS为高电平   	

		#else  // OLED_SEND_MODE == OLED_SEND_HARDWARE	//硬件SPI
			GPIO_setAsPeripheralModuleFunctionOutputPin(OLED_MOSI_PORTPIN, GPIO_PRIMARY_MODULE_FUNCTION);	//复用
			GPIO_setAsPeripheralModuleFunctionOutputPin(OLED_SCL_PORTPIN, GPIO_PRIMARY_MODULE_FUNCTION);
			
			SPI_initMaster(EUSCI_B2_BASE, &SPI_B2_MasterConfig);	//打开 SPI 模块
			SPI_enableModule(EUSCI_B2_BASE);
			SPI_disableInterrupt(EUSCI_B2_BASE, 
						EUSCI_SPI_RECEIVE_INTERRUPT | EUSCI_SPI_TRANSMIT_INTERRUPT);//关闭中断

		#endif
		
	#else	//(OLED_MODE == OLED_MODE_IIC)	//IIC 模式
	
		#if (OLED_SEND_MODE == OLED_SEND_SOFTWARE)	//软件IIC
			GPIO_setAsOutputPin(OLED_SCL_PIN);
			GPIO_setAsOutputPin(OLED_MOSI_PIN);
		
			OLED_SCL_CLR();  	

		#else  // OLED_SEND_MODE == OLED_SEND_HARDWARE	//硬件IIC


		#endif	
		
		
	#endif
	
		//最重要的步骤：
	OLED_Restart();		//开始发送初始化指令，完成重启动作

} 


//========以下为字符显示函数，需要引用 font.h 头文件才能使用=======
#ifdef __FONT_H__

/**
 * @brief  在屏幕上打印字符串
 * @param  str*[in]	字符串首地址
 * @param  start_x	字符从哪个 x 座标开始显示？
 * @param  page		字符显示在哪一页
 * @param  FontType	字体大小，有效值： \b OLED_TYPE_6X8
 * @param  inverse	是否取反显示（白底黑字）1 为取反
 * @return none  
 * @note	调用示例：OLED_PrintString(strBuf, 12, 3, OLED_TYPE_6X8, 0);*/
void OLED_PrintString(char* str, uint8_t start_x, uint8_t page, uint8_t FontType, bool inverse)
{
	char *p_str = str;
	uint8_t index = 0;				//当前显示的字符串中字符的偏移量
	uint8_t current_x = start_x;	//当前画的x座标，每发送完一个 完整的字符 后 自增相应的宽度
	uint8_t writeBuf		= 0x00;
	//uint8_t current_page = page;
	OLED_SetPosition(current_x, page);	//改变座标

	switch(FontType){
		case OLED_TYPE_6X8:
			while(p_str[index] != '\0'){
				
				
				for(uint8_t i = 0; i < 6; i++){		// 连续六个x，写一个完整的字符
					
					writeBuf = Font_ASCII_6x8[ 6 * p_str[index]  +  i];
					
					if(inverse == 1){	//取反显示
						writeBuf = ~writeBuf;
					}
					
					OLED_WriteData(writeBuf);	//写入数据
					OLED_operatingX++;

					current_x++;
					if(current_x == 128){
						current_x = 0;
					}
				}
				
				index++;	//偏移量自增
			}
			
			break;
		case OLED_TYPE_8X16:
			
			break;
		// case OLED_TYPE_16X16:	//汉字显示
			
		// 	break;
		default:
			break;
	}
}

/**
 * @brief  在 上次打印字符的位置 \b 右侧 \b 紧跟着 打印字符 
 * @param  str*[in]	字符串首地址
 * @param  FontType	字体大小，有效值： \b OLED_TYPE_6X8
 * @param  inverse	是否取反显示（白底黑字）1 为取反
 * @return none  
 * @note	调用示例：OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintString_Append(char* str, uint8_t FontType, bool inverse)
{
	OLED_PrintString(str, (OLED_operatingX + 1), OLED_operatingPage, FontType, inverse);
	
}

/**
 * @brief  打印空格 
 * @param  length	空格长度, 单位为 字符宽度
 * @param  start_x	字符从哪个 x 座标开始显示？
 * @param  page		字符显示在哪一页
 * @param  FontType	字体大小，有效值： \b OLED_TYPE_6X8
 * @param  inverse	是否取反显示（白底黑字）1 为取反
 * @return none  
 * @note	调用示例：OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintSpace(uint8_t length, uint8_t start_X, uint8_t page, uint8_t FontType, bool inverse)
{
	uint8_t writeBuf = 0x00;
	
	OLED_SetPosition(start_X, page);	//改变座标
	
	if(inverse == true){
		writeBuf = 0xFF;
	}else{
		writeBuf = 0x00;
	}
	
	
	
	switch(FontType){
		case OLED_TYPE_6X8:
		
			length *= 6u;	// 字符宽度 乘上
			for(uint8_t i = 0; i < length; i++){
				OLED_WriteData(writeBuf);	//写入数据
				OLED_operatingX++;
			}
			
			break;
		case OLED_TYPE_8X16:
			
			break;
		// case OLED_TYPE_16X16:	//汉字显示
			
		// 	break;
		default:
			break;
	}	
}

/**
 * @brief  在 上次打印字符的位置 \b 右侧 \b 紧跟着 打印 \b 空格 
 * @param  length	空格长度, 单位为 字符宽度
 * @param  FontType	字体大小，有效值： \b OLED_TYPE_6X8
 * @param  inverse	是否取反显示（白底黑字）1 为取反
 * @return none  
 * @note	调用示例：OLED_PrintString(strBuf, OLED_TYPE_6X8, 0);*/
void OLED_PrintSpace_Append(uint8_t length, uint8_t FontType, bool inverse)
{
	OLED_PrintSpace(length, (OLED_operatingX + 1), OLED_operatingPage, FontType, inverse);
}



#endif

#ifdef __OLED_GRAPHICS_H__




#endif

/**
 * @brief  两点法定位 显示位图，格式：uint8_t(一页) * 长度
 * @param  BMP *[in]	位图数组指针
 * @param  start_x		开始的 x 座标
 * @param  start_page	开始的 页序号
 * @param  end_x		末尾的 x 座标（包含x!) 例：16*8 bmp from (0,0): end_x == 15
 * @param  end_page		末尾的 页序号（包含该页！）例：16*16 bmp from(0,0): end_page == 1
 * @return none  
 * @note	位图排列顺序：
 *        0   1   2     uint8_t[]
        ┌───┬───┬───┐
  page 0│ 1 │ 2 │ 3 │
        │ ↓ │ ↓ │ ↓ │
        ├───┼───┼───┤
  page 1│ 4 │ 5 │ 6 │
        │ ↓ │ ↓ │ ↓ │
        └───┴───┴───┘		*/
void OLED_DrawBMP_twoPoints(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t end_x, uint8_t end_page)
{
	volatile uint16_t bmpIndex = 0;
	
	for(uint32_t i = start_page; i <= end_page; i++){	// y (page)
		OLED_SetPosition(start_x, i);	// 设置座标 (x, page) 
		
		for(uint32_t x = start_x; x <= end_x; x++){			// x
			OLED_WriteData(BMP[bmpIndex]);
			bmpIndex++;
		}
		
	}
	
	
}

/**
 * @brief  一点+尺寸定位 显示位图，格式：uint8_t(一页) * 长度
 * @param  BMP *[in]	位图数组指针
 * @param  start_x		开始的 x 座标
 * @param  start_page	开始的 页序号
 * @param  x_size		位图 x 方向的尺寸
 * @param  page_size	位图 页 尺寸（最小值 1 ）
 * @return none  */
void OLED_DrawBMP_size(uint8_t *BMP, uint8_t start_x, uint8_t start_page, uint8_t x_size, uint8_t page_size)
{
	volatile uint16_t bmpIndex = 0;
	
	for(uint32_t i = 0; i < page_size; i++){	// y (page)
		OLED_SetPosition(start_x, i);	// 设置座标 (x, page) 
		
		for(uint32_t x = start_x; x < (start_x + x_size); x++){			// x
			OLED_WriteData(BMP[bmpIndex]);
			bmpIndex++;
		}
		
	}	
}

//======================== 中断函数 Interrupt Handlers ===========================
