/*
 * picture.c
 *
 *  Created on: Apr 22, 2021
 *      Author: Teodor
 */


const char picture[]  = {  // w Image2LCD Data hor/Byte ver and Antitone pixel in byte
		0X00,0X00,0X00,0X00,0X00,0X78,0X78,0X78,0X78,0X78,0X78,0XF8,0XF8,0XF8,0XF8,0X78,
		0X78,0X78,0X78,0X78,0X78,0X78,0X00,0X00,0X00,0XF8,0XF8,0XF8,0XF8,0X78,0X78,0X78,
		0X78,0X78,0X78,0X78,0X78,0X78,0X78,0X78,0X78,0X00,0X00,0X00,0X00,0X80,0XC0,0XE0,
		0XF0,0XF0,0XF8,0X78,0X78,0X78,0X78,0XF8,0XF0,0XF0,0XE0,0XC0,0X80,0X00,0X00,0X00,
		0X00,0X00,0X00,0XE0,0XE0,0XE0,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0XE0,0XE0,0XE0,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XE0,0XE0,0XE0,
		0XE0,0XE0,0XE0,0XE0,0XE0,0XE0,0XE0,0X00,0X00,0X00,0X00,0X00,0XFC,0XFF,0XFF,0XFF,
		0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XFC,0X00,0X00,
		0X00,0X1C,0X1C,0XFF,0XFF,0XFF,0XFF,0X1C,0X1C,0X00,0X00,0X00,0X00,0XC0,0XE0,0XF0,
		0XF8,0X7C,0X3C,0X3C,0X3C,0X3C,0X7C,0XF8,0XF8,0XF0,0XC0,0X00,0X00,0X00,0X00,0XE0,
		0XF0,0XF8,0XFC,0X3C,0X3C,0X3C,0X3C,0X3C,0X3C,0X7C,0X78,0X38,0X10,0X00,0X00,0X00,
		0X1C,0X1C,0XFF,0XFF,0XFF,0XFF,0X1C,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X81,0X81,0X81,
		0X81,0X81,0X81,0X81,0X81,0X81,0X81,0X80,0X80,0X00,0X00,0X00,0X0F,0X7F,0XFF,0XFF,
		0XF8,0XC0,0XC0,0X80,0X80,0X80,0X80,0XC0,0XC0,0XF8,0XFF,0XFF,0X7F,0X0F,0X00,0X00,
		0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X80,0X80,0X00,0X00,0X00,0X00,0X7F,0XFF,0XFF,
		0XFF,0XCE,0X8E,0X8E,0X8E,0X8E,0X8E,0XCE,0XCF,0X8F,0X0F,0X00,0X00,0X00,0X00,0XC3,
		0XE7,0XC7,0XCF,0X8F,0X8F,0X8E,0X8E,0X8E,0X8E,0XDE,0XFE,0XFC,0XF8,0X00,0X00,0X00,
		0X00,0X00,0XFF,0XFF,0XFF,0XFF,0X80,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X07,0X07,0X07,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X07,0X07,0X07,0X07,0X07,0X07,
		0X07,0X07,0X07,0X07,0X07,0X07,0X87,0X87,0X87,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
		0X03,0X03,0X07,0X07,0X07,0X07,0X07,0X07,0X03,0X03,0X01,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X01,0X03,0X07,0X07,0X07,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
		0X03,0X07,0X07,0X07,0X07,0X07,0X07,0X07,0X03,0X03,0X81,0X80,0X80,0X00,0X01,0X01,
		0X03,0X03,0X07,0X07,0X07,0X07,0X07,0X07,0X07,0X07,0X03,0X01,0X00,0X00,0X00,0X00,
		0X00,0X00,0X01,0X03,0X07,0X07,0X07,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X80,0XC0,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0XE0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XE0,0X00,0X00,0XF0,0XF0,0XF8,
		0XFC,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0XFC,0XFC,0XFC,0XFC,0XFC,
		0XFC,0XF8,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XE0,0X00,
		0X00,0XF0,0XF0,0XF8,0XFC,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0XFC,
		0XFC,0XFC,0XFC,0XFC,0XFC,0XF8,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0XFF,0XFF,0XFF,0X7F,0X7F,0XFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0X7F,0X7F,0XFF,0XFF,0XFF,0X00,
		0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
		0XFF,0XFF,0XFF,0XFF,0XFF,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X0F,0X0F,0X0F,0X0C,0X0E,0X0F,0X0F,0X0F,0X00,0X00,0X0F,0X0F,0X1F,
		0X1F,0X1F,0X3F,0X3F,0X3F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,0X3F,0X1F,
		0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X0F,0X0F,0X0C,0X0E,0X0F,0X0F,0X0F,0X00,
		0X00,0X0F,0X0F,0X1F,0X1F,0X1F,0X3F,0X3F,0X3F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,0X7F,
		0X7F,0X7F,0X3F,0X1F,0X01,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
		0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};