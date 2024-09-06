#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include "EEPROM.h"
#include "HX711.h"
#include "NetRequest.h"

String Production_base = "";//需要使用UTF-8编码
String Batch_number = "";
String User_name = "";
String Phone_number = "";
String Unique_code ="";
String Weight_val = "";


String remote_host = "code.server.hzyichuan.cn";
String base_url = "/hello";

void ScanQRcode();
int UploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight, bool isGet);
void setup() {
    // WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    EEPROM.begin(1024);
    Serial.begin(9600);
    pinMode(LED_BUILTIN,OUTPUT);
    digitalWrite(LED_BUILTIN,HIGH);
    Init_Hx711();//需要和单片机共�?

    pinMode(ZERO_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_MINUS_BUTTON,INPUT_PULLUP);
    pinMode(CHECK_PLUS_BUTTON,INPUT_PULLUP);
  
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
  weightConfig(); //是否需要更新毛皮?
  //UploadData(Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val,0); 
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


int UploadData(String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight, bool isGet){ 
  String url = base_url;

  Serial.print(remote_host);
  Serial.println(url);

  if (!isGet){
    String payload = "{";
    payload += "\"base\":\"" + String(baseName) + "\", ";
    payload += "\"batchNo\":\"" + String(batchNo) + "\", ";
    payload += "\"id\":\"" + String(boxID) + "\", ";
    payload += "\"name\":\"" + String(employerName) + "\", ";
    payload += "\"phone\":\"" + String(employerPhone) + "\", ";
    payload += "\"weight\":" + String(weight);
    payload += "}";
    Serial.println(payload);
    HTTPS_request_post(remote_host, url, payload, "", "", 443);
  }
  else if (isGet)
  {
    url += "/" + baseName + "/" + batchNo + "/" + boxID + "/" + employerName + "/" + employerPhone + "/" + weight;
    Serial.println(url);
    HTTPS_request_get(remote_host, url, "", "", 443);
  }
  return 1;
}