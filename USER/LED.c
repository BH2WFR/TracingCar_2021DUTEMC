#ifdef __MSP432P401R__
#include "LED.h"


//========================== 私有宏定义 Private Macros ===========================
#define TURNON(a) 	MAP_GPIO_setOutputHighOnPin(a)
#define TURNOFF(a) 	MAP_GPIO_setOutputLowOnPin(a)
#define TOGGLE(a) 	MAP_GPIO_toggleOutputOnPin(a)


//===================== 公有全局变量 Public Global Variables =====================
uint8_t LED2_colour = 0;	//当前的 LED2(RGB) 颜色值
bool LED1_status = LED_OFF;


//==================== 私有全局变量 Private Global Variables =====================




//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============


//========================= 公有函数  Public Functions ===========================

/**
 * @brief  初始化板上LED
 * @param  none
 * @return none  */
void LED_Init(void)
{
	GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
	GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
	GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
	GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
	
}



/**
 * @brief  LED1(RED) 取反闪烁
 * @param  none
 * @return 改变后的状态, 有效值: LED_ON, LED_OFF  */
bool LED1_toggle(void)
{
	GPIO_toggleOutputOnPin(LED1);
	LED1_status = ! LED1_status;
	return LED1_status;
}


/**
 * @brief  打开/关闭LED1
 * @param  status[in]	可用值: LED_ON, LED_OFF
 * @return 改变后的状态, 有效值: LED_ON, LED_OFF    */
bool LED1_switch(bool status)
{
	if(status == LED_ON){
		TURNON(LED1);
	}else{
		TURNOFF(LED1);
	}
	
	LED1_status = status;
	
	return status;
}


/**
 * @brief   LED2(RGB) 按照指定颜色 改变 LED状态
 * @param  colour[in]	想要改变的颜色序号, 
 * 						有效值: LED2_COLOUR_BLACK, LED2_COLOUR_WHITE, LED2_COLOUR_RED, LED2_COLOUR_BLUE, 
 * 								LED2_COLOUR_GREEN, LED2_COLOUR_YELLOW, LED2_COLOUR_CYAN, LED2_COLOUR_MAGENTA
 * @return 改变后的颜色序号, 有效值同输入参数 */
uint8_t LED2_switch(uint8_t colour)
{
	if(colour > LED2_COLOUR_MAX) return LED2_RETVAL_ERROR;
	
	LED2_colour = colour;
	
	switch(colour){
		case LED2_COLOUR_BLACK:
			TURNOFF(LED_R);TURNOFF(LED_G);TURNOFF(LED_B);
			LED2_colour = colour;
			//MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
			break;
		case LED2_COLOUR_RED:
			TURNON(LED_R);TURNOFF(LED_G);TURNOFF(LED_B);
			break;
		case LED2_COLOUR_BLUE:
			TURNOFF(LED_R);TURNOFF(LED_G);TURNON(LED_B);
			break;
		case LED2_COLOUR_GREEN:
			TURNOFF(LED_R);TURNON(LED_G);TURNOFF(LED_B);
			break;
		case LED2_COLOUR_WHITE:
			TURNON(LED_R);TURNON(LED_G);TURNON(LED_B);
			//MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
			break;
		case LED2_COLOUR_YELLOW:
			TURNON(LED_R);TURNON(LED_G);TURNOFF(LED_B);
			break;
		case LED2_COLOUR_CYAN:
			TURNOFF(LED_R);TURNON(LED_G);TURNON(LED_B);
			break;
		case LED2_COLOUR_MAGENTA:
			TURNON(LED_R);TURNOFF(LED_G);TURNON(LED_B);
			break;	
	}
	
	return colour;
	
}

/**
 * @brief  LED2(RGB) 按照颜色序号 顺次上/下改变颜色
 * @param  direction[in]	顺次改变颜色的方向, 可用值: LED2_ROLLUP, LED2_ROLLDOWN
 * @return 改变后的颜色序号, 有效值同输入参数  */
uint8_t LED2_roll(bool direction)
{
	if(direction == LED2_ROLLUP){
		RollUpMax(LED2_colour, LED2_COLOUR_MIN, LED2_COLOUR_MAX);
	}else{
		RollDownMax(LED2_colour, LED2_COLOUR_MIN, LED2_COLOUR_MAX);
	}
	
	LED2_switch(LED2_colour);
	
	return LED2_colour;
}


/**
 * @brief   LED2(RGB) 按照指定颜色 取反 闪烁
 * @param  colour[in]	想要闪烁的颜色序号, 
 * 						有效值: LED2_COLOUR_BLACK, LED2_COLOUR_WHITE, LED2_COLOUR_RED, LED2_COLOUR_BLUE, 
 * 								LED2_COLOUR_GREEN, LED2_COLOUR_YELLOW, LED2_COLOUR_CYAN, LED2_COLOUR_MAGENTA
 * @return 改变后的颜色序号, 有效值同输入参数  */
uint8_t LED2_toggle(uint8_t colour)
{
	if(LED2_colour == LED2_COLOUR_BLACK){	//如果LED2是黑色(关闭)
		LED2_switch(colour);				//打开为指定颜色, 全局变量 LED2_colour 在 LED2_switch() 函数中已经改变了
	}else{	//LED2 已经开启
		LED2_switch(LED2_COLOUR_BLACK);		//关掉
	}

	return colour;
}


#endif
