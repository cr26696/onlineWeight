#ifndef __MAX7219__H__
#define __MAX7219__H__

#include <Arduino.h>

#define Max7219_pinCLK D0
#define Max7219_pinDIN D2
#define Max7219_pinCS D1

/**
 * 初始化MAX7219
 */
void Init_MAX7219();
/**
 * 功能：向MAX7219
 * 入口参数：DATA
 * 出口参数：无
 */
void Write_Max7219_byte(byte DATA);
/**
 * 功能：向MAX7219写入数据
 * 入口参数：address、dat
 */
void Write_Max7219(byte address,byte dat);
#endif