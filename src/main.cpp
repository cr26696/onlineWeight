#include <Arduino.h> 
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "EEPROM.h"
#include "HX711.h"
#include "Ticker.h"
#include "NetRequest.h"
#include <SegDisplay.h>
#include <Timer.h>
#include <Control.hpp>

Control MainControl;

void ScanQRcode();
String intToString4(int number);
void setup() {
    MainControl.init();
}

void loop() {
    MainControl.update();
}

String intToString4(int number) {
  // 提取后四位
  int lastFour = number % 10000;
  if(number>9999){return "9999";}
  else{
    // 创建一个字符串对象
    String result = String(lastFour);
    // 补零直到长度达到4
    for(int i =  result.length(); i < 4; i++){
      result = "0" + result;
    }
    return result;
  }
}