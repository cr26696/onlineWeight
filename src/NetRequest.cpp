
#include "NetRequest.h"

#include "Arduino.h"
/**
 * 功能: http请求
 * @param method: 请求方式 GET/POST (String)
 * @param host: 请求主机地址 (String)
 */
void HTTPS_request(String method, String host, String url, String payload)
{
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint_sni_cloudflaressl_com);
    // 这里不使用服务器证书
    client->setInsecure();

    HTTPClient https;
    int httpCode = -1;
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host)) {  // HTTPS
      if(method=="GET"){
        httpCode = https.GET();
        Serial.print("[HTTPS] GET...\n");
        Serial.print("[HTTPS] GET...\n");
      }
      else if(method=="POST")
      {
        httpCode = https.POST(payload);
        Serial.print("[HTTPS] POST...\n");
      }
    }
    if (httpCode > 0) {
      Serial.print("[HTTPS] Got code: ");
      Serial.println(httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String response = https.getString();
        Serial.println(response);//请求响应返回内容
      } else {
        Serial.print("[HTTPS] Failed, error: ");
        Serial.println(https.errorToString(httpCode));
      }
      https.end();
    }else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
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

  Serial.print(host);
  Serial.println(path);
  if(digitalRead(CONFIRM_BUTTON) == LOW){
    delay(50);
    if(digitalRead(CONFIRM_BUTTON) == LOW){
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
        HTTPS_request("POST", host, path, payload);
      }
      else if (isGet)
      {
        path += "/" + baseName + "/" + batchNo + "/" + boxID + "/" + employerName + "/" + employerPhone + "/" + weight;
        Serial.println(path);
        HTTPS_request("GET",host, path);
      }
    }
  } 
  return 1;
}