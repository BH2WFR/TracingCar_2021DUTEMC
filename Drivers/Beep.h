#ifndef __BEEP_H__
#define __BEEP_H__
/**
 * 	板载蜂鸣器代码
 * P2.7   TA0.4
 * */
//================== 引用头文件  Header Files Includes =======================
	//c标准头文件(ANSI C)
#include <stdio.h>  	//用于输出字符串的sprintf函数等
#include <stdint.h>		//里面有 uint8_t 等 一系列方便的宏定义
#include <stdlib.h>		//内有 malloc(@) free(@) rand()等
#include <math.h>  		//数学包
#include <string.h>		//字符串(char数组)操作, 内存填写复制(memcpy($)等)
//#include <ctype.h>		//字符操作
//#include <setjmp.h>		//跳转函数
//#include <stdarg.h>		//变参函数使用
//#include <time.h>			//
//#include <errno.h>		//
//#include <stddef.h>		//
//#include <limits.h>		//里面有各种变量的最大值/最小值定义
//#include <float.h>		//
//#include <locale.h>		//本地化支持

	//c标准头文件(C99)
#include <stdbool.h>		//用于支持bool(布尔)变量
//#include <complex.h>		//复数运算
//#include <tgmath.h>		//智能判断数据类型的数学运算

	//c标准头文件(C11)
//#include <uchar.h> 		//Unicode支持

	//用户模块头文件
#include "main.h"		//必须包含


//====================== 公有宏定义 Public Macros ===============================
#define BEEP_GPIO_PORT			GPIO_PORT_P2
#define BEEP_GPIO_PIN			GPIO_PIN4
//#define BEEP_GPIO_PORT_PIN		BEEP_GPIO_PORT##,##BEEP_GPIO_PIN

#define BEEP_TIMER_A_INDEX		0
#define BEEP_TIMER_A_BASE		TIMER_A0_BASE
#define BEEP_TIMER_A_INTERRPUT	INT_TA0_0
#define BEEP_TIMER32_BASE		TIMER32_0_BASE
#define BEEP_TIMER32_INTERRUPT	INT_T32_INT1


#define BEEP_MODE_USE_DELAY		1
#define BEEP_MODE_NOT_DELAY		0





//	最大/最小  频率 音值序号 指令序号
#define BEEP_FREQ_MAX	16000.0f
#define BEEP_FREQ_MIN	15.5f

#define BEEP_DIVIDER_1_FREQ_MAX		BEEP_FREQ_MAX
#define BEEP_DIVIDER_1_FREQ_MIN		250.00
#define BEEP_DIVIDER_3_FREQ_MAX		250.00
#define BEEP_DIVIDER_3_FREQ_MIN		62.00
#define BEEP_DIVIDER_12_FREQ_MAX	62.00
#define BEEP_DIVIDER_12_FREQ_MIN	BEEP_FREQ_MIN

#define BEEP_PITCH_INDEX_MAX	119
#define BEEP_PITCH_INDEX_MIN	0

#define BEEP_DIVIDER_1_INDEX_MAX	BEEP_PITCH_INDEX_MAX
#define BEEP_DIVIDER_1_INDEX_MIN	48
#define BEEP_DIVIDER_3_INDEX_MAX	47
#define BEEP_DIVIDER_3_INDEX_MIN	24
#define BEEP_DIVIDER_12_INDEX_MAX	23
#define BEEP_DIVIDER_12_INDEX_MIN	BEEP_PITCH_INDEX_MIN

#define BEEP_COMMAND_INDEX_MAX	125
#define BEEP_COMMAND_INDEX_MIN	120


/**
 * 	以下为音符、指令、长度 的 结构体序号
 * 一维数组音符：(序号0-119)
 * 		格式：px_Y, x为所在八度 有效值0-9, Y为该八度内的音符, 有效值 C, sC, bD, D .... sA, bB, B
 * 二维数组音符: (序号0-11)
 * 		格式 p_X, X为音符, 格式为有效值 C, sC, bD, D .... sA, bB, B
 * 一维数组指令: (序号120-?)
 * 		格式: c__, 下划线为指令名称
 * 音符的长度:
 * 		格式: l[d/dd]x[t/p] ,x为音符名称(四分音符/ 八分音符等), d/dd为附点/双附点, t为三连音, p为五连音 
*/
#define		p_C		0
#define		p_sC	1
#define		p_bD	1
#define		p_D		2
#define		p_sD	3
#define		p_bE	3
#define		p_E		4
#define		p_F		5
#define		p_sF	6
#define		p_bG	6
#define		p_G		7
#define		p_sG	8
#define		p_bA	8
#define		p_A		9
#define		p_sA	10
#define		p_bB	10
#define		p_B		11


#define		p0_C	0
#define		p0_sC	1
#define		p0_bD	1
#define		p0_D	2
#define		p0_sD	3
#define		p0_bE	3
#define		p0_E	4
#define		p0_F	5
#define		p0_sF	6
#define		p0_bG	6
#define		p0_G	7
#define		p0_sG	8
#define		p0_bA	8
#define		p0_A	9
#define		p0_sA	10
#define		p0_bB	10
#define		p0_B	11
#define		p1_C	12
#define		p1_sC	13
#define		p1_bD	13
#define		p1_D	14
#define		p1_sD	15
#define		p1_bE	15
#define		p1_E	16
#define		p1_F	17
#define		p1_sF	18
#define		p1_bG	18
#define		p1_G	19
#define		p1_sG	20
#define		p1_bA	20
#define		p1_A	21
#define		p1_sA	22
#define		p1_bB	22
#define		p1_B	23
#define		p2_C	24
#define		p2_sC	25
#define		p2_bD	25
#define		p2_D	26
#define		p2_sD	27
#define		p2_bE	27
#define		p2_E	28
#define		p2_F	29
#define		p2_sF	30
#define		p2_bG	30
#define		p2_G	31
#define		p2_sG	32
#define		p2_bA	32
#define		p2_A	33
#define		p2_sA	34
#define		p2_bB	34
#define		p2_B	35
#define		p3_C	36
#define		p3_sC	37
#define		p3_bD	37
#define		p3_D	38
#define		p3_sD	39
#define		p3_bE	39
#define		p3_E	40
#define		p3_F	41
#define		p3_sF	42
#define		p3_bG	42
#define		p3_G	43
#define		p3_sG	44
#define		p3_bA	44
#define		p3_A	45
#define		p3_sA	46
#define		p3_bB	46
#define		p3_B	47
#define		p4_C	48
#define		p4_sC	49
#define		p4_bD	49
#define		p4_D	50
#define		p4_sD	51
#define		p4_bE	51
#define		p4_E	52
#define		p4_F	53
#define		p4_sF	54
#define		p4_bG	54
#define		p4_G	55
#define		p4_sG	56
#define		p4_bA	56
#define		p4_A	57
#define		p4_sA	58
#define		p4_bB	58
#define		p4_B	59
#define		p5_C	60
#define		p5_sC	61
#define		p5_bD	61
#define		p5_D	62
#define		p5_sD	63
#define		p5_bE	63
#define		p5_E	64
#define		p5_F	65
#define		p5_sF	66
#define		p5_bG	66
#define		p5_G	67
#define		p5_sG	68
#define		p5_bA	68
#define		p5_A	69
#define		p5_sA	70
#define		p5_bB	70
#define		p5_B	71
#define		p6_C	72
#define		p6_sC	73
#define		p6_bD	73
#define		p6_D	74
#define		p6_sD	75
#define		p6_bE	75
#define		p6_E	76
#define		p6_F	77
#define		p6_sF	78
#define		p6_bG	78
#define		p6_G	79
#define		p6_sG	80
#define		p6_bA	80
#define		p6_A	81
#define		p6_sA	82
#define		p6_bB	82
#define		p6_B	83
#define		p7_C	84
#define		p7_sC	85
#define		p7_bD	85
#define		p7_D	86
#define		p7_sD	87
#define		p7_bE	87
#define		p7_E	88
#define		p7_F	89
#define		p7_sF	90
#define		p7_bG	90
#define		p7_G	91
#define		p7_sG	92
#define		p7_bA	92
#define		p7_A	93
#define		p7_sA	94
#define		p7_bB	94
#define		p7_B	95
#define		p8_C	96
#define		p8_sC	97
#define		p8_bD	97
#define		p8_D	98
#define		p8_sD	99
#define		p8_bE	99
#define		p8_E	100
#define		p8_F	101
#define		p8_sF	102
#define		p8_bG	102
#define		p8_G	103
#define		p8_sG	104
#define		p8_bA	104
#define		p8_A	105
#define		p8_sA	106
#define		p8_bB	106
#define		p8_B	107
#define		p9_C	108
#define		p9_sC	109
#define		p9_bD	109
#define		p9_D	110
#define		p9_sD	111
#define		p9_bE	111
#define		p9_E	112
#define		p9_F	113
#define		p9_sF	114
#define		p9_bG	114
#define		p9_G	115
#define		p9_sG	116
#define		p9_bA	116
#define		p9_A	117
#define		p9_sA	118
#define		p9_bB	118
#define		p9_B	119

#define		cPause	120		// 休止符
#define		cSetSpeed		121	// 设置/改变速度
#define		cRepeatStart	122	// 反复记号 开始
#define		cRepeatEnd		123	// 反复记号 结束
#define		cStart	124		// 开始符
#define		cEnd	125		// 终止符

#define		l1		4			//whole note(semibreve)		全音符

#define		l2		2			//half note(minim)				二分音符
#define		ld2		3			//dotted						
#define		ldd2	2.5			//double dotted					
#define		l4t		0.666666	//half triple note				二分音符三连音（三个占一个二分音符）
#define		l4p		0.4			//half pentatonic note			二分音符五连音

#define		l4		1			//quarter note(crotchet)		四分音符
#define		ld4		1.5			//dotted quarter note			附点四分音符
#define		ldd4	1.25		//double dotted quarter note	双附点四分音符
#define		l4t		0.666666	//quarter triple note			四分音符三连音（三个占一个二分音符）
#define		l4p		0.4			//quarter pentatonic note		四分音符五连音

#define		l8		0.5			//eighth note(quaver)			八分音符
#define		ld8		0.75		//dotted eighth note			附点八分音符
#define		ldd8	0.875		//double dotted eighth note		双附点八分音符
#define		l8t		0.333333	//eight triple note				八分音符三连音（三个占一个四分音符）
#define		l8p		0.2			//eight pentatonic note			八分音符五连音

#define 	l16		0.25		//sixteenth note(semiquaver)	十六分音符
#define		ld16	0.375		//dotted sixteenth note			附点十六分音符
#define		l16t	0.166667	//sixteen triple note			十六分音符三连音（三个占一个八分音符）	或 八分音符六连音
#define		l16p	0.1			//sixteen pentatonic note		十六分音符五连音


#define		l32		0.125		//
#define		ld32	0.1875
#define		l64		0.0625
#define		ld64	0.09375

//音符结构体
typedef struct{
	uint32_t 	pitch;		//音调（一维数组）、音调（具体频率，使用float方式读写）、休止符 或 指令
	float 		length;		//音符长度（1为一拍）
	//uint8_t		staccatoRatio;	//断奏时间的占比，范围0-100
}Beep_note_typedef;




//============== 公有全局变量声明 Public Global Variables Defines ===============
//extern const float Beep_Pitch_frequency[];
extern const uint16_t Beep_Pitch_loadValue[];
extern uint32_t Beep_currentTickCountPerBeat;
extern Beep_note_typedef* Beep_p_currentMusic;
extern Beep_note_typedef* Beep_p_currentNote;
extern bool 	Beep_isMute;

//================== 公有函数声明 Public Function Defines =======================
extern void Beep_Init(uint32_t speed);
extern void Beep_SingleNote(uint8_t index, float length, bool delayMode);
extern uint32_t Beep_SetSpeed(uint32_t speed);
extern void Beep_PlayMusic(Beep_note_typedef* p_music);
extern void Beep_StopMusic(void);




#endif





