#ifndef __HX711__H__
#define __HX711__H__

#include <Arduino.h>

#define HX711_SCK D7
#define HX711_DT D8

#define ZERO_BUTTON D6
#define CHECK_MINUS_BUTTON D5
#define CHECK_PLUS_BUTTON D3

//unsigned int OFFSET_VAL = 862; //ADC 偏移值

void Init_Hx711();
unsigned long HX711_Read(void);
unsigned long Get_Weight();
void Get_Maopi();
void weightConfig();

template<typename valueType>
void storeWeightConfiToEEPROM(int addr, valueType val);

template<typename valueType>
valueType readWeightConfiFromEEPROM(int addr);

#endif