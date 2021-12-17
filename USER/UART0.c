#include "UART0.h" 
#include "LED.h"

//=============UART配置结构体==============
const eUSCI_UART_ConfigV1 uart0_Config =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,			// 时钟源: SMCLK -> LFXT(48 MHz) / 4 = 12MHz
	1,										// BRDIV = 3	 |
	10,										// UCxBRF = 4	 |
	0x00,									// UCxBRS = 0x02 |-> 波特率 230400bps (3, 4, 0x02, USB串口 稳定性无所谓)  或者使用更快的 460800bps(1, 10, 0x00)
	EUSCI_A_UART_NO_PARITY,					// 无 校验
	EUSCI_A_UART_LSB_FIRST,					// 低位优先
	EUSCI_A_UART_ONE_STOP_BIT,				// 停止位 1
	EUSCI_A_UART_MODE,						// UART mode
	EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
	EUSCI_A_UART_8_BIT_LEN					 // 8 bit data length
};

//================= 公有全局变量 Public Global Variables=======================
char UART0_string[256] = {0};


//================= 私有全局变量 Private Global Variables======================
static char* p_UART0_str = NULL;		//中断函数中使用，指向当前传输的字符，字符串传输完毕后指向 NULL
static bool is_UART0_initialized = false;
static bool is_UART0_sending = false;




void UART0_readString(char* str, int length)
{
	
}

void UART0_sendString(char* str)//通过UART0发送字符串，参数可声明字符串数组后套用sprintf函数
{
	if( is_UART0_initialized == false || is_UART0_sending == true) return;
	
	is_UART0_sending = true;
	p_UART0_str = str;
	while(*p_UART0_str != '\0'){
		MAP_UART_transmitData(EUSCI_A0_BASE, *p_UART0_str);
		while(MAP_UART_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
			!= EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG );
		p_UART0_str++;
	}
	
	is_UART0_sending = false;
}

void UART0_init(void)
{
	MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
		GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);			
															//GPIO口复用 P1.2和P1.3为第一复用功能（eUSCI_A0之RXD、TXD）
	
	MAP_UART_initModule(EUSCI_A0_BASE, &uart0_Config);		//配置UART
	MAP_UART_enableModule(EUSCI_A0_BASE);					//开启UART0

	is_UART0_initialized = true;
}


