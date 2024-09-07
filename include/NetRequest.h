#ifndef __NETREQUEST_H__
#define __NETREQUEST_H__

#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecureBearSSL.h"

#define CONFIRM_BUTTON D4
#define IS_POST 0
#define IS_GET 1

extern volatile bool lockWeight;

byte HTTPS_request(String method, String host, String url, String payload = "");
//上传一条信息数据
int UploadData(bool isGet, String host, String path, String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight);
#endif
