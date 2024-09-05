#ifndef __NETREQUEST_H__
#define __NETREQUEST_H__

#include "ESP8266WiFi.h"
#include "ESP8266WiFiMulti.h"
#include "ESP8266HTTPClient.h"
#include "WiFiClientSecureBearSSL.h"

void HTTPS_request_get(String host, String url, String parameter, String fingerprint, int Port);
void HTTPS_request_post(String host, String url, String payload, String parameter, String fingerprint, int Port);
#endif