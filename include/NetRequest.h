#ifndef __NETREQUEST_H__
#define __NETREQUEST_H__

#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecureBearSSL.h"

void HTTPS_request(String host, String url, String parameter, String fingerprint, int Port);

#endif