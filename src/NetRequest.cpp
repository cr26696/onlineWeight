
#include "NetRequest.h"

#include "Arduino.h"

// /**
//  * 功能：HTTPS请求封装！
//  * @param host：请求域名（String类型）
//  * @param url：请求地址（String类型）
//  * @param parameter：请求参数(String类型)(默认""")
//  * @param fingerprint：服务器证书指纹 (String类型)(默认""")
//  * @param Port：请求端口(int类型)(默认：443)
//  * @param Receive_cache：接收缓存(int类型)(默认：1024)
//  * @return 成功返回请求的内容(String类型) 失败则返回"0"
//  * */
// String HTTPS_request_aux(String host, String url, String parameter = "", String fingerprint = "", int Port = 443, int Receive_cache = 1024)
// {
    // WiFiClientSecure HTTPS; //建立WiFiClientSecure对象
    // if (parameter != "")
    //     parameter = "?" + parameter;
    // String postRequest = (String)("GET ") + url + parameter + " HTTP/1.1\r\n" +
    //                      "Host: " + host + "\r\n" +
    //                      "User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1 Edg/103.0.5060.53" +
    //                      "\r\n\r\n";

    // if (fingerprint.length() == 0)
    //     HTTPS.setInsecure(); //不进行服务器身份认证
    // else
    // {
    //     HTTPS.setFingerprint(fingerprint.c_str()); //服务器证书指纹进行服务器身份认证
    // }
    // int cache = sizeof(postRequest) + 10;
    // Serial.print("发送缓存：");
    // Serial.println(cache);
    // HTTPS.setBufferSizes(Receive_cache, cache); //接收和发送缓存大小
    // HTTPS.setTimeout(15000);                    //设置等待的最大毫秒数
    // Serial.println("初始化参数完毕！\n开始连接服务器==>>>>>");
    // if (!HTTPS.connect(host, Port))
    // {
    //     delay(100);
    //     Serial.println();
    //     Serial.println("服务器连接失败！");
    //     return "0";
    // }
    // else
    //     Serial.println("服务器连接成功！\r");
    // Serial.println("发送请求：\n" + postRequest);
    // HTTPS.print(postRequest.c_str()); // 发送HTTP请求

    // // 检查服务器响应信息。通过串口监视器输出服务器状态码和响应头信息
    // // 从而确定ESP8266已经成功连接服务器
    // Serial.println("获取响应信息========>：\r");
    // Serial.println("响应头：");
    // while (HTTPS.connected())
    // {
    //     String line = HTTPS.readStringUntil('\n');
    //     Serial.println(line);
    //     if (line == "\r")
    //     {
    //         Serial.println("响应头输出完毕！"); // Serial.println("响应头屏蔽完毕！\r");
    //         break;
    //     }
    // }
    // Serial.println("截取响应体==========>");
    // String line;
    // while (HTTPS.connected())
    // {
    //     line = HTTPS.readStringUntil('\n'); // Serial.println(line);
    //     if (line.length() > 10)
    //         break;
    // }
    // Serial.println("响应体信息：\n" + line);
    // Serial.println("====================================>");
    // Serial.println("变量长度：" + String(line.length()));
    // Serial.println("变量大小：" + String(sizeof(line)) + "字节");
    // Serial.println("====================================>");
    // HTTPS.stop(); //操作结束，断开服务器连接
    // delay(500);
    // return line;

/**
 * 功能：HTTPS请求(结束销毁HTTPClient对象)
 * @param host：请求域名（String类型）
 * @param url：请求地址（String类型）
 * @param parameter：请求参数(String类型)(默认""")
 * @param fingerprint：服务器证书指纹 (String类型)(默认""")
 * @param Port：请求端口(int类型)(默认：443)
 * */
void HTTPS_request_get(String host, String url, String parameter = "", String fingerprint = "", int Port = 443)
{
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    // 这里不使用服务器证书
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host, Port, url, true)) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);//请求响应返回内容
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
}
void HTTPS_request_post(String host, String url, String payload, String parameter = "", String fingerprint = "", int Port = 443)
{
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    // 这里不使用服务器证书
    client->setInsecure();
    HTTPClient https;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host, Port, url, true)) {  // HTTPS
      https.addHeader("Content-Type", "application/json");
      Serial.print("[HTTPS] POST...\n");
      // start connection and send HTTP header
      int httpCode = https.POST(payload);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String resPayload = https.getString();
          Serial.println(resPayload);//请求响应返回内容
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
}