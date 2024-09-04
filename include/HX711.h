#ifndef __HX711__H__
#define __HX711__H__

#include <Arduino.h>

#define HX711_SCK D7
#define HX711_DT D8

#define OFFSET_VAL 862 //ADC 偏移值
void Init_Hx711();
unsigned long HX711_Read(void);
unsigned long Get_Weight(unsigned long Weight_Maopi);
unsigned long Get_Maopi();

#endif
