#include "WiFiUser.h"
#include "EEPROM.h"
#include <SoftwareSerial.h>

const int resetPin = 0;                    //设置重置按键引脚,用于删除WiFi信息
int connectTimeOut_s = 15;                 //WiFi连接超时时间，单位秒

String Production_base = "";
String Batch_number = "";
String Unique_code = "";
String User_name = "";
String Phone_number = "";


void ScanQRcode();
void setup() 
{
  EEPROM.begin(256);
  pinMode(resetPin, INPUT_PULLUP);     //按键上拉输入模式(默认高电平输�?,按下时下拉接到低电平)
  Serial.begin(9600);                //波特�?
  LEDinit();                           //LED用于显示WiFi状�?
  connectToWiFi(connectTimeOut_s);     //连接wifi，传入的是wifi连接等待时间15s
}

void loop() 
{
  
  if (!digitalRead(resetPin)) //长按5�?(P0)清除网络配置信息
  {
    delay(5000);              //哈哈哈哈，这样不准确
    if (!digitalRead(resetPin)) 
    {
      Serial.println("\n按键已长�?5�?,正在清空网络连保存接信息.");
      restoreWiFi();     //删除保存的wifi信息
      ESP.restart();              //重启复位esp32
      Serial.println("已重启设�?.");//有机会读到这里吗�?
    }
  }
  
  checkDNS_HTTP();                  //检测客户端DNS&HTTP请求，也就是检查配网页面那部分
  checkConnect(true);               //检测网络连接状态，参数true表示如果断开重新连接
  
  ScanQRcode();

}

void ScanQRcode(){  //扫描生产编号或人员编�?

  const char *batchsymbol = "#";
  const char *staffsymbol = "?";

  if(Serial.available()>0){
    String QR_val = "";
    QR_val = Serial.readString();
    Serial.println(QR_val);
    //QR_val = "茶叶基地哈哈哈哈哈哈#4asdasda56#73333389";
    char buffer[QR_val.length()+1];
    strcpy(buffer, QR_val.c_str());
    if( strstr(buffer,batchsymbol) ){//判断为批次码
      Production_base = strtok(buffer,batchsymbol);
      Batch_number = strtok(NULL,batchsymbol);
      Unique_code  = strtok(NULL,batchsymbol);
      Serial.println(Production_base);
      Serial.println(Batch_number);
      Serial.println(Unique_code);      
    }
    else if( strstr(buffer,staffsymbol) ){//判断为员工码
      User_name = strtok(buffer,staffsymbol);
      Phone_number = strtok(NULL,batchsymbol);
      Serial.println(User_name);
      Serial.println(Phone_number);
    }
    else return;
  }

}