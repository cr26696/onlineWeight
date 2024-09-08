#ifndef __SEG_DISPLAY_HPP__
#define __SEG_DISPLAY_HPP__

#include<MAX7219.h>

enum SegDisplayMode
{
  upper,
  lower
};

class SegDisplay
{
  private:
  byte _numbers[8] = {0};
  bool _needRefresh = false;

  void SegDisplay::updateParam(String str);
  void refresh();
  public:
  SegDisplay();
  void display(String Content);
  void display(String writePart, String Content);
};

#endif