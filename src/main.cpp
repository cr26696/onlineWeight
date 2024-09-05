#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include "EEPROM.h"
#include "HX711.h"
#include "NetRequest.h"

String Production_base = "茶叶基地";
String Batch_number = "123";
String Unique_code = "abc";
String User_name = "王斌彬";
String Phone_number = "8755";
String Weight_val = "200";


String remote_host = "code.server.hzyichuan.cn";
String base_url = "/hello";

unsigned long Weight_Maopi = 0;

void ScanQRcode();
int UploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight);
void setup() {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    Init_Hx711();//需要和单片机共地
    Weight_Maopi = Get_Maopi();
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
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

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

}

void loop() {
  ScanQRcode();
  UploadData(Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val);
  
  Serial.print("weight: ");
  Serial.println(Get_Weight(Weight_Maopi));
  delay(500);
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


int UploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight){
  String url = base_url;

  url += "/" + baseName + "/" + batchNo + "/" + boxID + "/" + employerName + "/" + employerPhone + "/" + weight;
  Serial.println(url);
  HTTPS_request(remote_host, url, "", "", 443);
  return 1;
}