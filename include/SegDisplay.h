#ifndef SEG_DISPLAY_H
#define SEG_DISPLAY_H

#include<MAX7219.h>

void SegDisplayinit();
void SegWrite(String writePart,String fullContent = "",String upperContent = "",String lowerContent = "");
uint8_t SegCharToNum(char c);

#endif