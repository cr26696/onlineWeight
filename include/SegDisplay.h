#ifndef SEG_DISPLAY_H
#define SEG_DISPLAY_H

#include<MAX7219.h>

void SegDisplayinit();
void SegWrite(String writePart, String fullContent);
uint8_t SegCharToNum(char c);

enum SegDisplayMode
{
  all,
  upper,
  lower
};

#endif