#include "UART0.h" 
#include "LED.h"

//=============UART���ýṹ��==============
const eUSCI_UART_ConfigV1 uart0_Config =
{
	EUSCI_A_UART_CLOCKSOURCE_SMCLK,			// ʱ��Դ: SMCLK -> LFXT(48 MHz) / 4 = 12MHz
	1,										// BRDIV = 3	 |
	10,										// UCxBRF = 4	 |
	0x00,									// UCxBRS = 0x02 |-> ������ 230400bps (3, 4, 0x02, USB���� �ȶ�������ν)  ����ʹ�ø���� 460800bps(1, 10, 0x00)
	EUSCI_A_UART_NO_PARITY,					// �� У��
	EUSCI_A_UART_LSB_FIRST,					// ��λ����
	EUSCI_A_UART_ONE_STOP_BIT,				// ֹͣλ 1
	EUSCI_A_UART_MODE,						// UART mode
	EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
	EUSCI_A_UART_8_BIT_LEN					 // 8 bit data length
};

//================= ����ȫ�ֱ��� Public Global Variables=======================
char UART0_string[256] = {0};


//================= ˽��ȫ�ֱ��� Private Global Variables======================
static char* p_UART0_str = NULL;		//�жϺ�����ʹ�ã�ָ��ǰ������ַ����ַ���������Ϻ�ָ�� NULL
static bool is_UART0_initialized = false;
static bool is_UART0_sending = false;




void UART0_readString(char* str, int length)
{
	
}

void UART0_sendString(char* str)//ͨ��UART0�����ַ����������������ַ������������sprintf����
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
															//GPIO�ڸ��� P1.2��P1.3Ϊ��һ���ù��ܣ�eUSCI_A0֮RXD��TXD��
	
	MAP_UART_initModule(EUSCI_A0_BASE, &uart0_Config);		//����UART
	MAP_UART_enableModule(EUSCI_A0_BASE);					//����UART0

	is_UART0_initialized = true;
}


