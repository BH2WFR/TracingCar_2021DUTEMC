#ifndef __MAIN_H__
#define __MAIN_H__

//====================== 公有宏定义 Public Macros ===============================


#define PI_F	3.141592f
#define PI		3.14159265358979323846264338327950288
#define E_F		2.718281f
#define E		2.71828182845904523536028747135266249
#define A_GRAVITY	9.8f

#define ABSORB()	{char ch; while((ch = getchar()) != '\n') continue;}

#define GetRange(min_index, max_index) 		((max_index) - (min_index) + 1)
#define Swap(x, y)			{typeof(x) swap; swap = (y); (y) = (x); (x) = swap;}	//需要 GNU C 支持
#define Swap_giveType(type, x, y)	 {type swap; swap = (y); (y) = (x); (x) = swap;}
//stdlib.h 有 类似宏 名称为 max， min
#define GetMax(x, y) 		( ((x) > (y)) ? (x) : (y) )
#define GetMin(x, y) 		( ((x) < (y)) ? (x) : (y) )
#define	GetSign(x)			( ((x) >  0 ) ?  1  :  0 )
#define IsSignDifferent(x, y) (  (  ((x) > 0 && (y) < 0) || ((x) < 0 && (y) > 0)  )? 1 : 0  )

#define RollUpMax(var, min, max)	{if((var) == (max)) (var) = (min); else (var)++;}	// roll up from min to max
#define RollDownMax(var, min, max)	{if((var) == (min)) (var) = (max); else (var)--;}	

#define RollUpRange(var, start, range)	{if((var) == (range - 1)) (var) = (start); else (var)++;}	// min -> (min + range - 1)
#define RollDownRange(var, start, range){if((var) == (start)) (var) = (range - 1); else (var)--;}

#define SetRange(var, min, max) {if((var) < (min)) (var) = (min); else if((var) > (max)) (var) = (max);}
#define SetRangeMax(var, max) 	{if((var）> (max)) (var) = (max);}
#define SetRangeMin(var, min)	{if((var）< (min)) (var) = (min);}

//============== 公有全局变量声明 Public Global Variables Defines ===============

extern volatile uint64_t main_circulateCount;	// 主函数 循环计数，一直增加，供其他函数读取


extern bool	main_testSign0; 	// 用于 其他模块的 测试
extern bool	main_testSign1; 
extern bool	main_testSign2; 
extern bool	main_testSign3; 
extern bool	main_testSign4; 

extern char  main_versionStr[24];
//================== 公有函数声明 Public Function Defines =======================


#endif


