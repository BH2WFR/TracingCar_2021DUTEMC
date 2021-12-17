#include "FONT.h"



/* ======================================
 宽度6  高度8 （实际宽度5  高度7 ） ASCII+控制字符  
 访问方式：Font_ASCII_6x8[ 6 * p_str[index]  +  i]  , i 为该字符的第几个 x 座标（0 ~ 6）
 =============================================*/
const uint8_t Font_ASCII_6x8[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x00	\0 		禁止使用
	0x00, 0x21, 0x51, 0x5D, 0x55, 0x27,   // \x01
	0x00, 0x10, 0x54, 0x56, 0x39, 0x10,   // \x02
	0xE1, 0xA1, 0xB9, 0xA9, 0xA9, 0xEF,   // \x03
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x04
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x05
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x06
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x07	\a 响铃
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x08	\b 退格
	0x00, 0x00, 0x1C, 0x08, 0x1C, 0x00,   // \x09	\t
	0x00, 0x10, 0x38, 0x54, 0x10, 0x1F,   // \x0A	\n
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x0B   \v 垂直制表			半字符
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x0C	\f 换页	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x0D	\r 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x0E
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x0F
	0x00, 0x7F, 0x41, 0x41, 0x41, 0x7F,   // \x10	□
	0x00, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F,   // \x11	■
	0x00, 0x3E, 0x22, 0x22, 0x22, 0x3E,   // \x12	□小
	0x00, 0x22, 0x42, 0x58, 0x42, 0x22,   // \x13	脸
	0x00, 0x06, 0x0F, 0x7F, 0x01, 0x7F,   // \x14	¶
	0x00, 0x44, 0x44, 0x5F, 0x44, 0x44,   // \x15	±
	0x00, 0x22, 0x14, 0x08, 0x14, 0x22,   // \x16	×
	0x00, 0x08, 0x08, 0x2A, 0x08, 0x08,   // \x17	÷
	0x00, 0x04, 0x06, 0x7F, 0x06, 0x04,   // \x18	↑
	0x00, 0x10, 0x30, 0x7F, 0x30, 0x10,   // \x19	↓
	0x00, 0x08, 0x08, 0x3E, 0x1C, 0x08,   // \x1A	→
	0x00, 0x08, 0x1C, 0x3E, 0x08, 0x08,   // \x1B	←
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // \x1C	▯
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   // \x1D	▮
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08,   // \x1E	长-
	0x14, 0x14, 0x14, 0x14, 0x14, 0x14,   // \x1F	长=
	
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // space
    0x00, 0x00, 0x00, 0x5F, 0x00, 0x00,   // !
    0x00, 0x00, 0x07, 0x00, 0x07, 0x00,   // "
    0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14,   // #
    0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12,   // $
    0x00, 0x23, 0x13, 0x08, 0x64, 0x62,   // %
    0x00, 0x36, 0x49, 0x55, 0x22, 0x50,   // &
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00,   // '
    0x00, 0x00, 0x00, 0x3E, 0x41, 0x00,   // (
    0x00, 0x00, 0x41, 0x3E, 0x00, 0x00,   // )
    0x00, 0x14, 0x08, 0x3E, 0x08, 0x14,   // *5
    0x00, 0x08, 0x08, 0x3E, 0x08, 0x08,   // +
	0x00, 0x00, 0x50, 0x30, 0x00, 0x00,   // ,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08,   // -
	0x00, 0x00, 0x30, 0x30, 0x00, 0x00,   // .
	0x00, 0x20, 0x10, 0x08, 0x04, 0x02,   // /
	0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E,   // 0
	0x00, 0x00, 0x42, 0x7F, 0x40, 0x00,   // 1
	0x00, 0x42, 0x61, 0x51, 0x49, 0x46,   // 2
	0x00, 0x21, 0x41, 0x45, 0x4B, 0x31,   // 3
	0x00, 0x18, 0x14, 0x12, 0x7F, 0x10,   // 4
	0x00, 0x27, 0x45, 0x45, 0x45, 0x39,   // 5
	0x00, 0x3E, 0x49, 0x49, 0x49, 0x32,   // 6
	0x00, 0x03, 0x01, 0x79, 0x05, 0x03,   // 7
	0x00, 0x36, 0x49, 0x49, 0x49, 0x36,   // 8
	0x00, 0x06, 0x49, 0x49, 0x29, 0x1E,   // 9
	0x00, 0x00, 0x36, 0x36, 0x00, 0x00,   // :
	0x00, 0x00, 0x56, 0x36, 0x00, 0x00,   // ;
	0x00, 0x00, 0x08, 0x14, 0x22, 0x00,   // <
	0x00, 0x14, 0x14, 0x14, 0x14, 0x14,   // =
	0x00, 0x00, 0x22, 0x14, 0x08, 0x00,   // >
	0x00, 0x02, 0x01, 0x51, 0x09, 0x06,   // ?
	0x00, 0x32, 0x49, 0x79, 0x41, 0x3E,   // @
	0x00, 0x7E, 0x09, 0x09, 0x09, 0x7E,   // A
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x36,   // B
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x22,   // C
	0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C,   // D
	0x00, 0x7F, 0x49, 0x49, 0x49, 0x41,   // E
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x01,   // F
	0x00, 0x3E, 0x41, 0x49, 0x49, 0x38,   // G
	0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F,   // H
	0x00, 0x00, 0x41, 0x7F, 0x41, 0x00,   // I
    0x00, 0x20, 0x40, 0x41, 0x3F, 0x01,   // J
    0x00, 0x7F, 0x08, 0x14, 0x22, 0x41,   // K
    0x00, 0x7F, 0x40, 0x40, 0x40, 0x40,   // L
	0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F,   // M
	0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F,   // N
	0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E,   // O
	0x00, 0x7F, 0x09, 0x09, 0x09, 0x06,   // P
	0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E,   // Q
	0x00, 0x7F, 0x09, 0x19, 0x29, 0x46,   // R
	0x00, 0x26, 0x49, 0x49, 0x49, 0x32,   // S
	0x00, 0x01, 0x01, 0x7F, 0x01, 0x01,   // T
	0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F,   // U
	0x00, 0x0F, 0x30, 0x40, 0x30, 0x0F,   // V 
	0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F,   // W
    0x00, 0x63, 0x14, 0x08, 0x14, 0x63,   // X
    0x00, 0x07, 0x08, 0x70, 0x08, 0x07,   // Y
    0x00, 0x61, 0x51, 0x49, 0x45, 0x43,   // Z
    0x00, 0x00, 0x00, 0x7F, 0x41, 0x00,   // [
    0x00, 0x02, 0x04, 0x08, 0x10, 0x20,   // 反斜线
    0x00, 0x00, 0x41, 0x7F, 0x00, 0x00,   // ]
    0x00, 0x04, 0x02, 0x01, 0x02, 0x04,   // ^
    0x00, 0x40, 0x40, 0x40, 0x40, 0x40,   // _
    0x00, 0x00, 0x01, 0x02, 0x04, 0x00,   // `
    0x00, 0x20, 0x54, 0x54, 0x54, 0x78,   // a
    0x00, 0x7F, 0x48, 0x44, 0x44, 0x38,   // b
    0x00, 0x38, 0x44, 0x44, 0x44, 0x20,   // c
    0x00, 0x38, 0x44, 0x44, 0x48, 0x7F,   // d
    0x00, 0x38, 0x54, 0x54, 0x54, 0x18,   // e
    0x00, 0x08, 0x7E, 0x09, 0x01, 0x02,   // f
    0x00, 0x08, 0x54, 0x54, 0x54, 0x3C,   // g
    0x00, 0x7F, 0x08, 0x04, 0x04, 0x78,   // h
    0x00, 0x00, 0x44, 0x7D, 0x40, 0x00,   // i
    0x00, 0x20, 0x40, 0x44, 0x3D, 0x00,   // j
    0x00, 0x7F, 0x10, 0x28, 0x28, 0x44,   // k
    0x00, 0x00, 0x01, 0x7F, 0x40, 0x00,   // l
    0x00, 0x7C, 0x04, 0x38, 0x04, 0x78,   // m
    0x00, 0x7C, 0x08, 0x04, 0x04, 0x78,   // n
    0x00, 0x38, 0x44, 0x44, 0x44, 0x38,   // o
    0x00, 0x7C, 0x14, 0x14, 0x14, 0x08,   // p
	0x00, 0x08, 0x14, 0x14, 0x14, 0x7C,   // q
    0x00, 0x08, 0x7C, 0x08, 0x04, 0x04,   // r
    0x00, 0x48, 0x54, 0x54, 0x54, 0x20,   // s
    0x00, 0x04, 0x3E, 0x44, 0x40, 0x20,   // t
    0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C,   // u
    0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C,   // v
    0x00, 0x3C, 0x40, 0x38, 0x40, 0x3C,   // w
    0x00, 0x44, 0x28, 0x10, 0x28, 0x44,   // x
    0x00, 0x0C, 0x50, 0x50, 0x50, 0x3C,   // y
    0x00, 0x44, 0x64, 0x54, 0x4C, 0x44,   // z
	0x00, 0x00, 0x08, 0x77, 0x41, 0x00,	  // {
    0x00, 0x00, 0x00, 0x7F, 0x00, 0x00,   // |
	0x00, 0x00, 0x41, 0x77, 0x08, 0x00,	  // }
	0x00, 0x08, 0x04, 0x08, 0x10, 0x08,	  // ~
	0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA    // \x7F	DEL
};

/* ======================================
 宽度8  高度16 （实际宽度  高度 ） ASCII+控制字符  
 访问方式：
 =============================================*/
const uint8_t Font_ASCII_8X16[]=
{
	0x00
};

const uint8_t nonside[] =
{
	0x00
};
