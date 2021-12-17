#include "Wireless.h"
#include "Interface.h"


/**
 * @brief  函数功能注释样例 JavaDoc
 * @param  parameter	[in]	按钮的按下/松开事件, 可用值: KEY_EVENT_PRESS, KEY_EVENT_UP
 * @param  parameter2	[in]	按钮编号, 可用值: 0 ~ 8	
 * @return none  */

//======================= 私有宏定义 Private Macros ========================

// 名称： WIRELESS




//================= 公有全局变量 Public Global Variables======================
bool Wireless_isSending = false;
uint8_t Wireless_sendBuf[32] = "ATK-LORA-01 wireless\r\n";
//uint8_t Wireless_receivedBuf[128] = {0};

#if(IS_USE_SETTING_PORT == 1)
	bool isInSettingMode = false;
	
#endif


//================= 私有全局变量 Private Global Variables======================
const eUSCI_UART_ConfigV1 uart_Wireless_Config =
{
	.selectClockSource 	= EUSCI_A_UART_CLOCKSOURCE_SMCLK,	// 时钟源: SMCLK -> LFXT(48 MHz) / 4 = 12MHz
	.clockPrescalar 	= 78,								// BRDIV = 78	 |
	.firstModReg		= 2,								// UCxBRF = 2	 |
	.secondModReg		= 0x00,								// UCxBRS = 0x00 |-> 波特率 9600 bps
	.parity				= EUSCI_A_UART_ODD_PARITY,			// 奇 校验
	.msborLsbFirst		= EUSCI_A_UART_LSB_FIRST,			// 低位优先
	.numberofStopBits	= EUSCI_A_UART_ONE_STOP_BIT,		// 停止位 1
	.uartMode			= EUSCI_A_UART_MODE,				// UART mode
	.overSampling		= EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
	.dataLength			= EUSCI_A_UART_8_BIT_LEN			// 8 bit data length
};

#if(IS_USE_SETTING_PORT == 1)
const eUSCI_UART_ConfigV1 uart_Wireless_Setting_Mode_Config =
{
	.selectClockSource 	= EUSCI_A_UART_CLOCKSOURCE_SMCLK,	// 时钟源: SMCLK -> LFXT(48 MHz) / 4 = 12MHz
	.clockPrescalar 	= 78,								// BRDIV = 78	 |
	.firstModReg		= 2,								// UCxBRF = 2	 |
	.secondModReg		= 0x00,								// UCxBRS = 0x00 |-> 波特率 9600 bps
	.parity				= EUSCI_A_UART_ODD_PARITY,			// 奇 校验
	.msborLsbFirst		= EUSCI_A_UART_LSB_FIRST,			// 低位优先
	.numberofStopBits	= EUSCI_A_UART_ONE_STOP_BIT,		// 停止位 1
	.uartMode			= EUSCI_A_UART_MODE,				// UART mode
	.overSampling		= EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
	.dataLength			= EUSCI_A_UART_8_BIT_LEN			// 8 bit data length
};
#endif

static uint8_t g_sendingIndex = 0;	//当前发送的字符串中字符的序号
static bool	   g_usingDelay = 0;		//是否使用delay?
static bool    g_sendingTo = WIRELESS_SENDING_TO_0;	//发送到'\0'还是指定的长度? 有效值: WIRELESS_SENDING_TO_0(发送到'\0'), WIRELESS_SENDING_TO_T(发送到指定长度)
static uint8_t g_targetSendingIndex = 0;			//要发送的指定长度, 仅在 sendingTo == WIRELESS_SENDING_TO_T 时 有效
static uint8_t *g_p_str = (void*)NULL;	


//========= 私有函数 弱函数 内联函数  Private/Weak/Inline Functions============
/**
 * @brief  私有方法 终止发送, 由 中断函数 和 Wireless_sendString(@) 调用
 * @param  none
 * @return none  */
static void EndSending(void)
{
	g_p_str = (void*)NULL;
	g_sendingIndex = 0u;
	Wireless_isSending = false;	//全部发送完毕后 关闭发送标志位	
}

/**
 * @brief  私有方法 使用delay方法发送一个字节, 由  Wireless_sendString(@) 调用, 参考了库函数的源代码, 减去无用语句减少负担
 * @param  none
 * @return none  */
static void Send_delay(uint8_t **p)
{
	EUSCI_A_CMSIS(WIRELESS_UART_BASE)->TXBUF = **p;
	(*p)++;
	while (!BITBAND_PERI(EUSCI_A_CMSIS(WIRELESS_UART_BASE)->IFG, EUSCI_A_IFG_TXIFG_OFS));
}


static void getContent(uint8_t receivedString, uint8_t length)
{
	
	
}


/**
 * @brief  中断函数调用
 * 			主要功能:  */
static void ReceivedInterrupt(uint8_t buf)
{
	LED2_toggle(LED2_COLOUR_GREEN);
	//main_testSign0 = !main_testSign0;
}


/**
 * @brief  中断函数调用
 * 			主要功能:  */
static void TransmitInterrupt(void)
{
	g_p_str++;	// 先将指针前移一位(!!!) 然后检测, 最后发送, g_sendingIndex--语句在后面, 请勿在这里写
	
	if(g_sendingTo == WIRELESS_SENDING_TO_0){
		
		if(*g_p_str == 0x00){		//如果发送到了 \\0
			
			EndSending();	//清除 Wireless_isSending 标志位	
					
		}else{	//没有发送到 \\0, 继续发送, 触发下一次中断
			
			//UART_transmitData(WIRELESS_UART_BASE, *g_p_str);
			EUSCI_A_CMSIS(WIRELESS_UART_BASE)->TXBUF = *g_p_str;
		}

		
	}else{	//g_sendingTo == WIRELESS_SENDING_TO_T
		g_sendingIndex--;	//指针前移后, 先将计数减一 再检测, 最后发送
		
		if(g_sendingIndex == 0){
			
			EndSending();	//清除 Wireless_isSending 标志位
			
		}else{
			
			//UART_transmitData(WIRELESS_UART_BASE, *g_p_str);
			EUSCI_A_CMSIS(WIRELESS_UART_BASE)->TXBUF = *g_p_str;
		}
	}
}







//========================= 公有函数  Public Functions =========================

/**
 * @brief  发送字符串(要求指定字符串长度)
 * @param  string*[in]		字符串首地址
 * @param  delayMode[in]	是否使用延迟? 可用值: WIRELESS_DELAY_USE, WIRELESS_DELAY_UNUSE
 * @param  length[in]		字符串长度, 当 length==0 的时候发送到 \\0 为止 (推荐 length 输入为 0 ), 勿超过 255
 * @return none  */
void Wireless_sendString(uint8_t *string, bool delayMode, uint8_t length)
{
	if(Wireless_isSending == true || string == (void*)NULL){	//防止重复发送 或 发送空指针
		return;
	}
	Wireless_isSending = true;		//打开发送标志位
	
	g_p_str = string;			//全局变量指针赋值
	g_sendingIndex = length;	//仅在 WIRELESS_SENDING_TO_T 模式 有效
	if(length == 0u){		//设置发送模式 length == 0
		g_sendingTo = WIRELESS_SENDING_TO_0;
	}else{					//length <> 0
		g_sendingTo = WIRELESS_SENDING_TO_T;
	}
	
	
	if(delayMode == true){		//delayMode == WIRELESS_DELAY_USE	//使用延迟, 关闭中断
	
		if(g_usingDelay == WIRELESS_DELAY_UNUSE){		//关闭发送中断
			UART_disableInterrupt(WIRELESS_UART_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
			g_usingDelay = WIRELESS_DELAY_USE;
		}	
		
		if(g_sendingTo == WIRELESS_SENDING_TO_0){
			do{
				Send_delay(&g_p_str);
				// UART_transmitData(WIRELESS_UART_BASE, *g_p_str);	//先发送, 再动指针, 最后检测
				// g_p_str++;
			}while( *g_p_str != 0x00);
			
		}else{	//g_sendingTo == WIRELESS_SENDING_TO_T
			do{
				Send_delay(&g_p_str);
				// UART_transmitData(WIRELESS_UART_BASE, *g_p_str);
				// g_p_str++;
				g_sendingIndex--;
			}while(g_sendingIndex != 0u);
		}
		
		EndSending();	//清除 Wireless_isSending 标志位
		
		
	}else{					//delayMode == WIRELESS_DELAY_UNUSE	//不使用延迟, 打开中断
	
		if(g_usingDelay == WIRELESS_DELAY_USE){		//开启发送中断
			UART_enableInterrupt(WIRELESS_UART_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
		}
		g_usingDelay = WIRELESS_DELAY_UNUSE;
		
		UART_transmitData(WIRELESS_UART_BASE, *g_p_str);	//发送第一个字节, 指針和計數不要动, 在中断函数先改变指针和计数, 再发送字节
		
	}
	
}


#if(IS_USE_SETTING_PORT == 1)
/**
 * @brief  设置为设置模式，引脚拉高
 * @param  none
 * @return none  */
void Wireless_SetToSettingMode(void)
{
	GPIO_setOutputHighOnPin(WIRELESS_SET_PORT, WIRELESS_SET_PIN);
	UART_initModule(uart_Wireless_Setting_Mode_Config);	// UART 配置 改到 设置模式的配置
	
	for(volatile int i = 0; i < 50000; i++){
		__nop();
	}
	
	isInSettingMode = true;
}

/**
 * @brief  设置为发送/接收 模式，引脚拉低
 * @param  none
 * @return none  */
void Wireless_SetToNormalMode(void)
{
	GPIO_setOutputLowOnPin(WIRELESS_SET_PORT, WIRELESS_SET_PIN);
	UART_initModule(uart_Wireless_Config);	// 恢复 UART 配置
	
	for(volatile int i = 0; i < 50000; i++){
		__nop();
	}
	
	isInSettingMode = false;
	
}

/**
 * @brief  设置无线模块，设置完后自动恢复 发送/接收 模式
 * @param  none
 * @return none  */
void Wirelss_Setting(void)
{
	if(isInSettingMode == false){
		Wireless_SetToSettingMode();
	}
	
	
	
	
	
	
	
	
	Wireless_SetToNormalMode();
}


#endif



/**
 * @brief  关闭 无线串口 模块 (eUSCI A2->UART )
 * @param  none
 * @return none  */
void Wireless_TurnOff(void)
{
	//===== TX, RX 恢复 输入模式
	GPIO_setAsInputPin(WIRELESS_TX_GPIO_PORT, WIRELESS_TX_GPIO_PIN);	
	GPIO_setAsInputPin(WIRELESS_RX_GPIO_PORT, WIRELESS_RX_GPIO_PIN);
		
	//===== 设置端口 恢复 输入模式
	#if(IS_USE_SETTING_PORT == 1)
		GPIO_setAsInputPin(WIRELESS_SET_PORT, WIRELESS_SET_PIN);
	#endif		
	
	UART_disableModule(WIRELESS_UART_BASE);							//关闭 UART 2
	
	// 关闭中断
	UART_disableInterrupt(WIRELESS_UART_BASE, 
			EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
	Interrupt_disableInterrupt(WIRELESS_UART_INTERRUPT);
}


/**
 * @brief  初始化 无线串口 模块 (eUSCI A2->UART )
 * @param  none
 * @return none  */
void Wireless_init(void)
{
		//===== TX, RX 初始化
	GPIO_setAsPeripheralModuleFunctionInputPin(
			WIRELESS_TX_GPIO_PORT, WIRELESS_TX_GPIO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);	
	GPIO_setAsPeripheralModuleFunctionInputPin(
			WIRELESS_RX_GPIO_PORT, WIRELESS_RX_GPIO_PIN, GPIO_PRIMARY_MODULE_FUNCTION);	
	
		//===== 设置端口 初始化
	#if(IS_USE_SETTING_PORT == 1)
		GPIO_setAsOutputPin(WIRELESS_SET_PORT, WIRELESS_SET_PIN);
		GPIO_setOutputLowOnPin(WIRELESS_SET_PORT, WIRELESS_SET_PIN);
		
	
	#endif		
	
	UART_initModule(WIRELESS_UART_BASE, &uart_Wireless_Config);		//配置UART
	UART_enableModule(WIRELESS_UART_BASE);							//开启UART 2
	
	/* Enabling interrupts */
	UART_enableInterrupt(WIRELESS_UART_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT | EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
	Interrupt_enableInterrupt(WIRELESS_UART_INTERRUPT);

}



//======================== 中断函数 Interrupt Handlers =========================
/**
 * @brief  EUSCIA2 中断
 * 			主要功能:  */
void EUSCIA2_IRQHandler(void)
{
	uint32_t status = MAP_UART_getEnabledInterruptStatus(WIRELESS_UART_BASE);
	UART_clearInterruptFlag(WIRELESS_UART_BASE, status);	// 清除中断，必须操作
	
	//==== 接收中断
	if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
	{
		ReceivedInterrupt(EUSCI_A_CMSIS(WIRELESS_UART_BASE)->RXBUF);	// 调用内部函数处理接受中断，未来加入DMA等
		
	}
	
	//==== DELAY_UNUSE 模式下 发送完一个字节来的中断
	if(status & EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT)
	{
		
		//LED2_toggle(LED2_COLOUR_YELLOW);
		TransmitInterrupt();	// 调用上面的函数，发送下一个字节 或 停止发送，未来加入缓冲池和 DMA
	}

}

