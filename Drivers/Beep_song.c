#include "Beep_song.h"
#include "beep.h"
//========================== 私有宏定义 Private Macros ===========================


//===================== 公有全局变量 Public Global Variables =====================
const Beep_note_typedef music_Start[] = 
{
	{cStart, 0},
	{cSetSpeed, 180},
	
	// {p4_C, 0.5},
	// {p4_E, 0.5},
	// {p4_G, 0.5},
	// {p5_C, 1},

	
	{p4_C, l16},
	{p4_E, l16},
	{p4_G, l16},
	{p5_C, l8},

	
	{cEnd, 0},
};


const Beep_note_typedef music_UndefinedButton[] = 
{
	{cStart, 0},
	{cSetSpeed, 150},
	
	{p4_E, l16},
	
	{cEnd, 0},
};

const Beep_note_typedef music_Button[] = 
{
	{cStart, 0},
	{cSetSpeed, 150},
	
	// {p4_C, 0.5},
	// {p4_E, 0.5},
	// {p4_G, 0.5},
	// {p5_C, 1},
	{p6_sC, l32},
	{p6_sG, l32},
	
	{cEnd, 0},
};

const Beep_note_typedef music_ChangeMode[] = 
{
	{cStart, 0},
	{cSetSpeed, 180},
	
	// {p5_C, l4},
	// {p4_A, l8},
	// {p5_C, l8},
	// {p5_F, l8},
	// {p5_G, l16},
	// {p5_A, l16},
	// {p5_D, l8},
	// {p5_C, l8},
	
	// {p4_A, ld4},
	// {p5_C, l16},
	// {p4_A, l16},
	// {p4_G, ld4},
	// {p4_A, l8},
	
	// {p4_F, ld8},
	// {p4_A, l16},
	// {p4_G, l16},
	// {p4_F, l16},
	// {p4_D, l16},
	// {p4_F, l16},
	// {p4_C, l4},
	
	{p4_C, l16},
	{p4_G, l16},

	
	{cEnd, 0},
};
//==================== 私有全局变量 Private Global Variables =====================

//============ 私有函数 弱函数 内联函数  Private/Weak/Inline Functions=============

//========================= 公有函数  Public Functions ===========================

//======================== 中断函数 Interrupt Handlers ===========================


