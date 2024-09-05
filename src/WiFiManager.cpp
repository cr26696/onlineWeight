#include "WiFiManager.h"

/**
 * 功能: 构造WiFiManager对象
 * @param TimeoutSec: 连接超时时间，秒(uint8_t)
 **/
WiFiManager::WiFiManager(byte TimeoutSec)
:TimeoutSec(TimeoutSec)
{

};

void WiFiManager::handleRoot() 
{
  if (webserver.hasArg("selectSSID")) {
    webserver.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);   //scanNetWprksID是扫描到的wifi
  } else {
    webserver.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);   
  }
};
 
/*
 * 提交数据后的提示页面
 */
void WiFiManager::handleConfigWifi()               //返回http状态
{
  if (webserver.hasArg("ssid"))          //判断是否有账号参数
  {
    wifi_ssid = webserver.arg("ssid");   //获取html表单输入框name名为"ssid"的内容
    Serial.print("got ssid:");
 
    Serial.println(wifi_ssid);
  } 
  else                                //没有参数
  { 
    Serial.println("error, not found ssid");
    webserver.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); //返回错误页面
    return;
  }
  //密码与账号同理
  if (webserver.hasArg("password")) 
  {
    Serial.print("got password:");
    wifi_pass = webserver.arg("password");  //获取html表单输入框name名为"pwd"的内容
    Serial.println(wifi_pass);
  } 
  else 
  {
    Serial.println("error, not found password");
    webserver.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  webserver.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); //返回保存成功页面
  // delay(2000);
  // WiFi.softAPdisconnect(true);     //参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  // webserver.close();                  //关闭web服务
  // WiFi.softAPdisconnect();         //在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);
  configIsChange = 1;
  // if (WiFi.status() != WL_CONNECTED)    //wifi没有连接成功
  // {
  //   Serial.println("开始调用连接函数connectToWiFi()..");
  //   connectToWiFi(connectTimeOutSec);
  // } 
  // else {
  //   Serial.println("提交的配置信息自动连接成功..");
  // }
}
 
/*
 * 处理404情况的函数'handleNotFound'
 */
void WiFiManager::handleNotFound()           // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{           
  handleRoot();                 //访问不存在目录则返回配置页面
  //webserver.send(404, "text/plain", "404: Not found");
}
 

 

 



 

 
 
/**
 * 功能: 清除WiFi信息
 */
void WiFiManager::restoreWiFi() {

};
 
/*
 * 检查wifi是否已经连接
 */
void WiFiManager::checkConnect(bool reConnect) 
{

};

 






/**
 * 初始化,为连接WiFi做准备
 * 设置设备名
 * 获取ssid、password
 **/
bool WiFiManager::initWiFi() {
  WiFi.hostname(HOST_NAME);     //设置设备名
  WiFi.setAutoConnect(true);    //设置自动连接
  getWiFiInfoFromEEPROM();

  Serial.println("尝试使用上次WiFi信息连接");
  Serial.print("wifi_ssid:");
  Serial.println(wifi_ssid);
  Serial.print("wifi_password:");
  Serial.println(wifi_pass);

  if(connectToWiFi(5)){
    Serial.println("wifi 连接成功，初始化结束");
    return true;
  }
  else{
    Serial.println("wifi初始化失败, 开启配网");
    wifiWebConfig();
    scanWiFi();
    return false;
  }
};
/** 
 * 功能: 尝试连接WiFi
 * @param timeOut_s: 连接超时时间
 * @return 连接成功/失败(bool)
 **/
bool WiFiManager::connectToWiFi(int timeOut_s) {
  Serial.println("进入connectToWiFi()函数");
  WiFi.mode(WIFI_STA);   //设置为STA模式并连接WIFI
  WiFi.begin(wifi_ssid,wifi_pass);
  unsigned long time_startConnect = millis();
  unsigned long time_startDot = 0;
  while(WiFi.status() != WL_CONNECTED){
    unsigned long time_now = millis();
    if (time_now - time_startConnect >= (unsigned long)timeOut_s*1000) {
      Serial.println("WIFI connect Timeout");
       return false; // 超时退出

    }
    // 打印进度
    if (time_now - time_startDot >= 200) {
      Serial.print(".");
      time_startDot = time_now;
    }
  }
  //WiFi.status() == WL_CONNECTED即连接成功

  Serial.println("WIFI connect Success");
  Serial.print("WiFi SSID:");
  Serial.println(WiFi.SSID());
  Serial.print("WiFi IP:");
  Serial.println(WiFi.localIP());
  Serial.print("WiFi GateIP:");
  Serial.println(WiFi.gatewayIP());
  Serial.print("WIFI status is:");
  Serial.print(WiFi.status());
  // digitalWrite(LED, LOW);
  return true;
}
/*
 * 扫描附近的WiFi，为了显示在配网界面
 * @return 成功/失败(bool)
 */
bool WiFiManager::scanWiFi() {
  Serial.print("scan start");
  Serial.println("---------↓");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done!");
  if (n == 0) {
    Serial.println("no networks found");
    scanNetworksID += "<option>no networks found</option>";
    return false;
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("No. | SSID | (信号强度) | open");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<option>" + WiFi.SSID(i) + "</option>";
      delay(10);
    }
    return true;
  }
}

void WiFiManager::storeWiFiIfoToEEPROM() {
  writeEEPROM(rom_variables[ROM_INDEX_SSID].address, wifi_ssid, rom_variables[ROM_INDEX_SSID].length);
  writeEEPROM(rom_variables[ROM_INDEX_PWD].address, wifi_pass, rom_variables[ROM_INDEX_PWD].length);
}

void WiFiManager::getWiFiInfoFromEEPROM() {
  readEEPROM(rom_variables[ROM_INDEX_SSID].address,rom_variables[ROM_INDEX_SSID].length);
  readEEPROM(rom_variables[ROM_INDEX_PWD].address,rom_variables[ROM_INDEX_PWD].length);
}
/*
 * 进入AP模式
 */
void WiFiManager::initSoftAP() {

  WiFi.mode(WIFI_AP);                                           //配置为AP模式
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   //设置AP热点IP和子网掩码
  if (WiFi.softAP(AP_SSID))                                     //开启AP热点,如需要密码则添加第二个参数
  {                           
    //打印相关信息
    Serial.println("ESP-32S SoftAP is right.");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());            //接入点ip
    Serial.print("MAC address = ");
    Serial.println(WiFi.softAPmacAddress());    //接入点mac
  }
  else                                                  //开启AP热点失败
  {
    T_restart.detach();
    Serial.println("WiFiAP Failed");
    Serial.println("restart 2s later...");
    T_restart.once(2, [](){ 
      Serial.println("restart now.");
      ESP.restart(); 
    });//重启复位esp32
  }
}

/*
 * 开启DNS服务器
 */
void WiFiManager::initDNS() 
{
  if (dnsServer.start(DNS_PORT, "*", apIP))   //判断将所有地址映射到esp32的ip上是否成功
  {
    Serial.println("DNS server started");
  } else {
    Serial.println("Failed start DNSserver.");
  }
}

void WiFiManager::initWebServer() {
  if (MDNS.begin("826612F"))      //给设备设定域名esp32,完整的域名是esp32.local
  {
    Serial.println("MDNS responder started");
  }
  //必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  webserver.on("/", HTTP_GET, handleRoot);                      //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  webserver.on("/configwifi", HTTP_POST, handleConfigWifi);     //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
  webserver.onNotFound(handleNotFound);                         //当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
 
  webserver.begin();                                           //启动TCP SERVER
 
  Serial.println("WebServer started!");
}
/**
 * 功能: 启动热点,提供配网页面（轮询响应网页事件）
 */
void WiFiManager::wifiWebConfig() 
{
  initSoftAP();   
  initDNS();        
  initWebServer();  
}
void WiFiManager::checkConnect(bool reConnect) {
  if (WiFi.status() != WL_CONNECTED)           //wifi连接失败
  {
    // if (digitalRead(LED) != !LOW) 
      // digitalWrite(LED, !LOW);
    if (reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA ) 
    {
      Serial.println("WIFI未连接.");
      Serial.println("WiFi Mode:");
      Serial.println(WiFi.getMode());
      Serial.println("正在连接WiFi...");
      connectToWiFi(connectTimeOutSec);          //连接wifi函数 
    }
  } 
  else if (digitalRead(LED) != !HIGH)  
    digitalWrite(LED, !HIGH);                    //wifi连接成功 灯亮
}
/**
* 检测客户端DNS&HTTP请求
**/
void WiFiManager::checkDNS_HTTP(){
  dnsServer.processNextRequest();   //检查客户端DNS请求
  server.handleClient();            //检查客户端(浏览器)http请求
}
/**
 * 功能: 写入数据到EEPROM
 * @param address EEPROM地址(int)
 * @param buffer 数据起始地址(char*)
 * @param length 写入数据长度(int)
 **/
void WiFiManager::writeEEPROM(int address, String &str, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(address + i, str[i]);
    }
    EEPROM.commit(); 
}
/**
 * 功能: 从EEPROM读取字符串
 * @param address: EEPROM地址(int)
 * @param length: 读取数据长度(int)
 * @return 读取内容(String)
 **/
String WiFiManager::readEEPROM(int address, int length) {
    char data[length]; // 创建字符数组
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + i); // 读取字符串内容
    }
    return String(data); // 返回字符串
}

