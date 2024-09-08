#ifndef __TIMER_H__
#define __TIMER_H__

#include "Ticker.h"

struct Tickers
{
  Ticker tDisplay;
  Ticker tScan;
  Ticker tButton;
};

void handleDisplay();
void handletScan();
void handleDtButton();

#endif