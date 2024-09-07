#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include "EEPROM.h"
#include "HX711.h"
#include "Ticker.h"
#include "NetRequest.h"
#include <SegDisplay.h>


/*
D0 711_DT
D1 711_CK
D2 CHECK_M
D3 CHECK_P
D4 QUPI
D5 CONFIRM
D6 MAX_DIN
D7 MAX_CS
D8 MAX_CLK
*/
String Production_base = "";//需要使用UTF-8编码
String Batch_number = "";
String Unique_code ="";
String User_name = "";
String Phone_number = "";
String Weight_val = "";


String remote_host = "code.server.hzyichuan.cn";
String base_url = "/hello";

Ticker TickerDisplay;
int tickerDispalycount = 0;


void ScanQRcode();
String intToString4(int number);
void setup() {
    EEPROM.begin(1024);
    Serial.begin(9600);
    // pinMode(LED_BUILTIN,OUTPUT);
    // digitalWrite(LED_BUILTIN,HIGH);
    Init_Hx711();//需要和单片机共地

    pinMode(ZERO_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_MINUS_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_PLUS_BUTTON,INPUT_PULLUP);
    pinMode(CONFIRM_BUTTON,INPUT_PULLUP);

    WiFiManager wm;


    bool res;
    res = wm.autoConnect(); 
    if(!res) {
        Serial.println("Failed to connect");
        Serial.println("please reset to try again...");
        while(1){
          delay(1000);
          //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
        //digitalWrite(LED_BUILTIN,LOW);
    }
    SegDisplayinit();
    TickerDisplay.attach_ms(20, [](){
      int val = Weight_val.toInt();
      if(val > 9999){
        SegWrite("lower","EEEE");
      }else{
        SegWrite("lower",intToString4(val));
      }
    });
}

void loop() {
  if(!lockWeight){Weight_val = Get_Weight();}
  ScanQRcode();
  weightConfig(); //是否需要更新毛皮?
  UploadData(IS_POST, remote_host, base_url, Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val);
  // Serial.print("weight: ");
  // Serial.println(Get_Weight());
  //UploadData(Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val,0); 
  // SegWrite("upper",Phone_number);  
  // SegWrite("lower",intToString4(Weight_val.toInt()));
  // Serial.print("[RAW Weight] ");
  // Serial.println(HX711_Read());
}

void ScanQRcode(){  //扫描生产编号或人员编码?

  const char *batchsymbol = "#";
  const char *staffsymbol = "?";

  if(Serial.available()>0){
    String QR_val = "";
    QR_val = Serial.readString();
    Serial.println(QR_val);
    Weight_val = Get_Weight();
    Serial.print("get SangYe weight: ");
    Serial.println(Weight_val);
    char buffer[QR_val.length()+1];
    strcpy(buffer, QR_val.c_str());
    if( strstr(buffer,batchsymbol) ){//判断为批次码
      Production_base = strtok(buffer,batchsymbol);
      Batch_number = strtok(NULL,batchsymbol);
      Unique_code  = strtok(NULL,batchsymbol);
      Serial.println(Production_base);
      Serial.println(Batch_number);
      Serial.println(Unique_code);
      lockWeight = true;
      Serial.print("[lock]lockWeight state");
      Serial.println(lockWeight);
    }
    else if( strstr(buffer,staffsymbol) ){//判断为员工码
      User_name = strtok(buffer,staffsymbol);
      Phone_number = strtok(NULL,batchsymbol);
      Serial.println(User_name);
      Serial.println(Phone_number);
      SegWrite("upper", Phone_number);
      Serial.println("[phone]"+Phone_number);
      lockWeight = false;
    }
    else return;
  }
}
String intToString4(int number) {
  // 提取后四位
  int lastFour = number % 10000;
  // 创建一个字符串对象
  String result = String(lastFour);
  // 补零直到长度达到4
  for(int i =  result.length(); i < 4; i++){
    result = "-" + result;
  }
  return result;
}