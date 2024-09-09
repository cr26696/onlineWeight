
#include "NetRequest.h"

#include "Arduino.h"

volatile bool lockWeight = false;

/**
 * 功能: http请求
 * @param method: 请求方式 GET/POST (String)
 * @param host: 请求主机地址 (String)
 */
byte HTTPS_request(String method, String host, String url, String payload)
{
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    // 这里不使用服务器证书
    client->setInsecure();

    HTTPClient https;
    int httpCode = -1;
    String response = "-1";
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host, 443, url, true)) {  // HTTPS
      if(method=="GET"){
        Serial.print("[HTTPS] GET...\n");
        httpCode = https.GET();
      }
      else if(method=="POST")
      {
        Serial.print("[HTTPS] POST...\n");
        https.addHeader("Content-Type", "application/json");
        httpCode = https.POST(payload);
      }
    }else{
      Serial.print("[HTTPS] begin failed!!\n");
      return -1;
    }
    if (httpCode > 0) {
      Serial.print("[HTTPS] Got code: ");
      Serial.println(httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        response = https.getString();
        Serial.println(response);//
      } else {
        Serial.print("[HTTPS] Failed, error: ");
        Serial.println(https.errorToString(httpCode));
      }
      https.end();
    }else {
      Serial.print("[HTTPS] Unable to connect, code :");
      Serial.println(httpCode);
    }

    return (byte)response.toInt();

}


/**
 * 上传数据函数
 * 
 * @param baseName 基础名称
 * @param batchNo 批次编号
 * @param boxID 箱子ID
 * @param employerName 发货人名称
 * @param employerPhone 发货人电话
 * @param weight 重量
 * @param isGet 是否为GET请求，若为true，则执行GET请求，否则执行POST请求
 * 
 * @return 返回1，表示函数执行完成
 */
int UploadData(bool isGet, String host, String path, String baseName, String batchNo, String boxID, String employerName, String employerPhone, String weight){

  if(digitalRead(CONFIRM_BUTTON) == LOW){
    delay(50);
    if(digitalRead(CONFIRM_BUTTON) == LOW){
      Serial.print("[Upload]full path: ");
      Serial.print(host);
      Serial.println(path);
      if (!isGet){
        String payload = "{";
        payload += "\"base\":\"" + String(baseName) + "\", ";
        payload += "\"batchNo\":\"" + String(batchNo) + "\", ";
        payload += "\"id\":\"" + String(boxID) + "\", ";
        payload += "\"name\":\"" + String(employerName) + "\", ";
        payload += "\"phone\":\"" + String(employerPhone) + "\", ";
        payload += "\"weight\":" + String(weight);
        payload += "}";
        Serial.print("[Upload]payload: ");
        Serial.println(payload);
        byte state = HTTPS_request("POST", host, path, payload);
        Serial.print("[Upload]request state: ");
        Serial.println(state);
        lockWeight = !(state == 200);
        
      }
      else if (isGet)
      {
        path += "/" + baseName + "/" + batchNo + "/" + boxID + "/" + employerName + "/" + employerPhone + "/" + weight;
        Serial.println(path);
        if(HTTPS_request("GET",host, path) == 200){
          lockWeight = false;
        }
      }
      while(digitalRead(CONFIRM_BUTTON)==LOW){}
    }
  } 
  return 1;
}