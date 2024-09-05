#include "WiFiUser.h"
#include <EEPROM.h>
#include <SoftwareSerial.h>
#include "NetRequest.h"
#include "HX711.h"
#include <Ticker.h>

const int resetPin = 0; // 设置重置按键引脚,用于删除WiFi信息, 默认HIGH,按下时下拉LOW

int connectTimeOut_s = 15; // WiFi连接超时时间，单位秒

// String wifi_ssid = "";                     //暂时存储wifi账号密码
// String wifi_pass = "";                     //暂时存储wifi账号密码

// 目标服务器的地址和路径 信息变量
String remote_host = "code.server.hzyichuan.cn";
String base_url = "/hello";

String Production_base = "茶叶基地";
String Batch_number = "123";
String Unique_code = "abc";
String User_name = "王斌彬";
String Phone_number = "8755";
String Weight_val = "200";

Ticker TickerButtons;
unsigned long buttonStart_reset = 0; // 记录按钮起始时间
unsigned long Weight_Maopi = 0;

void scanQRcode();
int uploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight);
void t_checkButton();
void setup()
{
  EEPROM.begin(256);
  pinMode(resetPin, INPUT_PULLUP); // 按键上拉输入模式(默认高电平输出,按下时下拉接到低电平)
  Serial.begin(9600);              // 波特率
  LEDinit();                       // LED用于显示WiFi状态
  // WiFi.mode(WIFI_AP_STA);
  // connectToWiFi(connectTimeOut_s);     //连接wifi，传入的是wifi连接等待时间15s
  //  wifi_ssid = getWiFiInfoFromEEPROM(wifi_ssidval_add);
  //  wifi_pass = getWiFiInfoFromEEPROM(wifi_passval_add);
  TickerButtons.attach(0.1, t_checkButton);
  // 申请一个客户端对象
  //WiFiClient client;
  connectToWiFi(15);
  // connectToWiFi(connectTimeOut_s);     //连接wifi，传入的是wifi连接等待时间15s
  Init_Hx711();
  Weight_Maopi = Get_Maopi();
}

void loop()
{

  // if (!digitalRead(resetPin)) // 长按5s(P0)清除网络配置信息
  // {
  //   delay(5000); // 哈哈哈哈，这样不准确
  //   if (!digitalRead(resetPin))
  //   {
  //     Serial.println("\n按键已长按5s,正在清空网络连保存接信息.");
  //     restoreWiFi();                 // 删除保存的wifi信息
  //     ESP.restart();                 // 重启复位esp32
  //     Serial.println("已重启设备."); // 有机会读到这里吗?
  //   }
  // }

  Serial.println(Get_Weight(Weight_Maopi));
  delay(500);

  // checkDNS_HTTP();                  //检测客户端DNS&HTTP请求，也就是检查配网页面那部分
  // checkConnect(true);               //检测网络连接状态，参数true表示如果断开重新连接

  scanQRcode();
  if (WiFi.status() == WL_CONNECTED)
  {
    uploadData(Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val);
  }
  else if (WiFi.status() != WL_CONNECTED)
  {
    while (WiFi.status() != WL_CONNECTED)
    {
      connectToWiFi(connectTimeOut_s);
    }
  }

  delay(2000);
}

void scanQRcode()
{ // 扫描生产编号或人员编号

  const char *batchsymbol = "#";
  const char *staffsymbol = "?";

  if (Serial.available() > 0)
  {
    String QR_val = "";
    QR_val = Serial.readString();
    Serial.println(QR_val);
    // QR_val = "茶叶基地哈哈哈哈哈哈#4asdasda56#73333389";
    char buffer[QR_val.length() + 1];
    strcpy(buffer, QR_val.c_str());
    if (strstr(buffer, batchsymbol))
    { // 判断为批次码
      Production_base = strtok(buffer, batchsymbol);
      Batch_number = strtok(NULL, batchsymbol);
      Unique_code = strtok(NULL, batchsymbol);
      Serial.println(Production_base);
      Serial.println(Batch_number);
      Serial.println(Unique_code);
    }
    else if (strstr(buffer, staffsymbol))
    { // 判断为员工码
      User_name = strtok(buffer, staffsymbol);
      Phone_number = strtok(NULL, batchsymbol);
      Serial.println(User_name);
      Serial.println(Phone_number);
    }
    else
      return;
  }
}
/**
 * 功能：发送get请求上报数据
 * @param baseName ：基地名称(String类型)
 * @param batchNo：批次号（String类型）
 * @param boxID：箱体唯一编号（String类型）
 * @param employerName：员工姓名(String类型)
 * @param employerPhone：员工尾号(int类型)(4位手机尾号)
 * @return 成功返回1 失败则返回0
 * */
int uploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight)
{
  String url = base_url;

  url += "/" + baseName + "/" + batchNo + "/" + boxID + "/" + employerName + "/" + employerPhone + "/" + weight;
  HTTPS_request(remote_host, url, "", "", 443);
  return 1;
}

/**
 * @brief 按钮ticker回调函数
 * 
 * 功能: 检测按钮按下时长,调用对应函数
 * 
 * reset 按下5s 清除信息并重启设备 
 */
void t_checkButton()
{
  // 重置按钮检测
  if (digitalRead(resetPin))
  {                               // 未按下
    buttonStart_reset = millis(); // 更新为当前时间
  }
  else if (millis() - buttonStart_reset > 5000)
  { // 按下,判定按下时长
    Serial.println("\n按键已长按5s,正在清空网络连保存接信息.");
    restoreWiFi(); // 删除保存的wifi信息
    ESP.restart(); // 重启复位
    Serial.println("已重启设备.");
  }
};