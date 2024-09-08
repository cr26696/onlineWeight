#include "SegDisplay.h"
#include "Timer.h"

void handleDisplay(String phoneNumber,String weight){
  SegWrite("upper", phoneNumber);
  SegWrite("lower", weight);
}
void handletScan(){

}
void handleDtButton();

