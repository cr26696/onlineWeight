#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include "EEPROM.h"
#include "HX711.h"
#include "Ticker.h"
#include "NetRequest.h"
#include <SegDisplay.h>


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
void setup() {
    EEPROM.begin(1024);
    Serial.begin(9600);
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    Init_Hx711();//需要和单片机共地

    pinMode(ZERO_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_MINUS_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_PLUS_BUTTON,INPUT_PULLUP);
    pinMode(CONFIRM_BUTTON,INPUT_PULLUP);
  
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect(); 
    if(!res) {
        Serial.println("Failed to connect");
        Serial.println("please reset to try again...");
        while(1){
          delay(1000);
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
        digitalWrite(LED_BUILTIN,LOW);
    }
    SegDisplayinit();
}

void loop() {
  ScanQRcode();
  weightConfig(); //是否需要更新毛皮?
  UploadData(IS_POST, remote_host, base_url, Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val);
  Serial.print("weight: ");
  Serial.println(Get_Weight());
  delay(1000);
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
