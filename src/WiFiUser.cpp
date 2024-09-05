#include "WiFiUser.h"
#include "EEPROM.h"
#include <Ticker.h>

const byte DNS_PORT = 53;                  //设置DNS端口号
const int webPort = 80;                    //设置Web端口号

const char* AP_SSID  = "设备配网WiFi";        //设置AP热点名称
//const char* AP_PASS  = "";               //这里不设置设置AP热点密码
 
const char* HOST_NAME = "ESP12F";        //设置设备名
String scanNetworksID = "";                //用于储存扫描到的WiFi ID
 
IPAddress apIP(192, 168, 4, 1);            //设置AP的IP地址
 
String wifi_ssid = "";                     //暂时存储wifi账号密码
String wifi_pass = "";                     //暂时存储wifi账号密码

//EEPROM存储变量预定义
std::vector<EepromVariable> rom_variables;//变量容器
//变量存储序号定义
#define ROM_INDEX_SSID 0
#define ROM_INDEX_PWD 1

//定时器
Ticker T_restart;

const int LED = 2;                         //设置LED引脚
 
DNSServer dnsServer;                       //创建dnsServer实例
ESP8266WebServer server(webPort);                 //开启web服务, 创建TCP SERVER,参数: 端口号,最大连接数

// 上下两段HTML代码
String ROOT_HTML_1 = "<!DOCTYPE html><html><head>  <meta charset=\"UTF-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />  <title>登录页面</title>  <style>   #content,.login,.login-card a,.login-card h1,.login-help{text-align:center}body,html{margin:0;padding:0;width:100%;height:100%;display:table}#content{font-family:\'Source Sans Pro\',sans-serif;-webkit-background-size:cover;-moz-background-size:cover;-o-background-size:cover;background-size:cover;display:table-cell;vertical-align:middle}.login-card{padding:40px;width:274px;background-color:#F7F7F7;margin:0 auto 10px;border-radius:20px;box-shadow:8px 8px 15px rgba(0,0,0,.3);overflow:hidden}.login-card h1{font-weight:400;font-size:2.3em;color:#1383c6}.login-card h1 span{color:#f26721}.login-card img{width:70%;height:70%}.login-card input[type=submit]{width:100%;display:block;margin-bottom:10px;position:relative}.login-card input[type=text],input[type=password]{height:44px;font-size:16px;width:100%;margin-bottom:10px;-webkit-appearance:none;background:#fff;border:1px solid #d9d9d9;border-top:1px solid silver;padding:0 8px;box-sizing:border-box;-moz-box-sizing:border-box}.login-card input[type=text]:hover,input[type=password]:hover{border:1px solid #b9b9b9;border-top:1px solid #a0a0a0;-moz-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);-webkit-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);box-shadow:inset 0 1px 2px rgba(0,0,0,.1)}.login{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:36px;padding:0 8px}.login-submit{-webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#4d90fe}.login-submit:disabled{opacity:.6}.login-submit:hover{border:0;text-shadow:0 1px rgba(0,0,0,.3);background-color:#357ae8}.login-card a{text-decoration:none;color:#666;font-weight:400;display:inline-block;opacity:.6;transition:opacity ease .5s}.login-card a:hover{opacity:1}.login-help{width:100%;font-size:12px}.list{list-style-type:none;padding:0}.list__item{margin:0 0 .7rem;padding:0}label{display:-webkit-box;display:-webkit-flex;display:-ms-flexbox;display:flex;-webkit-box-align:center;-webkit-align-items:center;-ms-flex-align:center;align-items:center;text-align:left;font-size:14px;}input[type=checkbox]{-webkit-box-flex:0;-webkit-flex:none;-ms-flex:none;flex:none;margin-right:10px;float:left}.error{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:25px;padding:0 8px;padding-top: 10px; -webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#ff1215}@media screen and (max-width:450px){.login-card{width:70%!important}.login-card img{width:30%;height:30%}}  </style></head><body style=\"background-color: #e5e9f2\"><div id=\"content\"> <form name=\'input\' action=\'/configwifi\' method=\'POST\'>  <div class=\"login-card\">    <h1>WiFi登录</h1>   <form name=\"login_form\" method=\"post\" action=\"$PORTAL_ACTION$\">   <input type=\"text\" name=\"ssid\" placeholder=\"请输入 WiFi 名称\" id=\"auth_user\" list = \"data-list\"; style=\"border-radius: 10px\">    <datalist id = \"data-list\">";
String ROOT_HTML_2 = "<input type=\"password\" name=\"password\" placeholder=\"请输入 WiFi 密码\" id=\"auth_pass\"; style=\"border-radius: 10px\">      <div class=\"login-help\">        <ul class=\"list\">          <li class=\"list__item\">          </li>        </ul>      </div>   <input type=\"submit\" class=\"login login-submit\" value=\"确 定 连 接\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> <!-- <form name=\'input\' action=\'/English\' method=\'POST\'>    <input type=\"submit\" class=\"login login-submit\" value=\"English\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> --></body></html>";
/*
 * 处理网站根目录的访问请求
 */

const int wifi_ssidlen_add = 1;
const int wifi_passlen_add = 2;
const int wifi_ssidval_add = 10;
const int wifi_passval_add = 30;

static bool configIsChange = 0;

void handleRoot() 
{
  if (server.hasArg("selectSSID")) {
    server.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);   //scanNetWprksID是扫描到的wifi
  } else {
    server.send(200, "text/html", ROOT_HTML_1 + scanNetworksID + ROOT_HTML_2);   
  }
}
 
/*
 * 提交数据后的提示页面
 */
void handleConfigWifi()               //返回http状态
{
  if (server.hasArg("ssid"))          //判断是否有账号参数
  {
    Serial.print("got ssid:");
    wifi_ssid = server.arg("ssid");   //获取html表单输入框name名为"ssid"的内容
 
    Serial.println(wifi_ssid);
  } 
  else                                //没有参数
  { 
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid"); //返回错误页面
    return;
  }
  //密码与账号同理
  if (server.hasArg("password")) 
  {
    Serial.print("got password:");
    wifi_pass = server.arg("password");  //获取html表单输入框name名为"pwd"的内容
    Serial.println(wifi_pass);
  } 
  else 
  {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }
  server.send(200, "text/html", "<meta charset='UTF-8'>SSID：" + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); //返回保存成功页面
  // delay(2000);
  // WiFi.softAPdisconnect(true);     //参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
  // server.close();                  //关闭web服务
  // WiFi.softAPdisconnect();         //在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
  Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);
  configIsChange = 1;
  // if (WiFi.status() != WL_CONNECTED)    //wifi没有连接成功
  // {
  //   Serial.println("开始调用连接函数connectToWiFi()..");
  //   connectToWiFi(connectTimeOut_s);
  // } 
  // else {
  //   Serial.println("提交的配置信息自动连接成功..");
  // }
}
 
/*
 * 处理404情况的函数'handleNotFound'
 */
void handleNotFound()           // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{           
  handleRoot();                 //访问不存在目录则返回配置页面
  //server.send(404, "text/plain", "404: Not found");
}
 
/*
 * 进入AP模式
 */
void initSoftAP() {

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
void initDNS() 
{
  if (dnsServer.start(DNS_PORT, "*", apIP))   //判断将所有地址映射到esp32的ip上是否成功
  {
    Serial.println("DNS server started");
  } else {
    Serial.println("Failed start DNSserver.");
  }
}
 
/*
 * 初始化WebServer
 */
void initWebServer() 
{
  if (MDNS.begin("esp32"))      //给设备设定域名esp32,完整的域名是esp32.local
  {
    Serial.println("MDNS responder started");
  }
  //必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);                      //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
  server.on("/configwifi", HTTP_POST, handleConfigWifi);     //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理
                                                            
  server.onNotFound(handleNotFound);                         //当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理
 
  server.begin();                                           //启动TCP SERVER
 
  Serial.println("WebServer started!");
}


/*
 * 扫描附近的WiFi，为了显示在配网界面
 */
bool scanWiFi() {
  Serial.println("scan start");
  Serial.println("--------->");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    scanNetworksID += "<option>no networks found</option>";
    return false;
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      //Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<option>" + WiFi.SSID(i) + "</option>";
      delay(10);
    }
    scanNetworksID += "</datalist>";
    return true;
  }
}



/**
 * 初始化,为连接WiFi做准备
 * 设置设备名
 * 获取ssid、password
 */
void initWiFi(){
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
  }
  else{
    Serial.println("wifi初始化失败, 开启配网");
    wifiWebConfig();
  }
};
/** 
 * 功能: 尝试连接WiFi
 * @param timeOut_s: 连接超时时间
 * @return 连接成功/失败(bool)
 **/
bool connectToWiFi(int timeOut_s) { 
  Serial.println("进入connectToWiFi()函数");
  WiFi.mode(WIFI_STA);   //设置为STA模式并连接WIFI
  WiFi.begin(wifi_ssid,wifi_pass);
  unsigned long time_startConnect = millis();
  unsigned long time_startDot;
  while(WiFi.status() != WL_CONNECTED){
    unsigned long time_now = millis();
    if (time_now - time_startConnect >= timeOut_s*1000) {
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
  digitalWrite(LED, LOW);
  return true;
}
  // }
  // for(int i =0; i <= timeOut_s*2; i++){
  //   Serial.print(".");
  //   digitalWrite(LED, !digitalRead(LED));
  //   delay(200);
  //   if(WiFi.status() == WL_CONNECTED){
  //     Serial.println("");
  //     Serial.println("WIFI connect Success");
  //     Serial.printf("SSID:%s", WiFi.SSID().c_str());
  //     Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
  //     Serial.print("LocalIP:");
  //     Serial.print(WiFi.localIP());
  //     Serial.print(" ,GateIP:");
  //     Serial.println(WiFi.gatewayIP());
  //     Serial.print("WIFI status is:");
  //     Serial.print(WiFi.status());
  //     digitalWrite(LED, LOW);
  //     return;
  //   }
  // }

  
  // wifiWebConfig();
  // while(!configIsChange){     //等待配置信息改变
  //   Serial.println("wait for config change...");
  //   delay(2000);
  // }
  // configIsChange = 0;
  // WiFi.hostname(HOST_NAME);                       //设置设备名
  // WiFi.mode(WIFI_STA);                            //设置为STA模式并连接WIFI 
  // Serial.print("使用web配置信息连接中");
  // while(WiFi.status() != WL_CONNECTED){
  //   Serial.print(".");
  //   digitalWrite(LED, !digitalRead(LED));
  //   WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); //c_str(),获取该字符串的指针
  //   delay(200);
  // }
  // storeWiFiIfoToEEPROM();
  // Serial.println("");
  // Serial.println("WIFI connect Success");
  // Serial.printf("SSID:%s", WiFi.SSID().c_str());
  // Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
  // Serial.print("LocalIP:");
  // Serial.print(WiFi.localIP());
  // Serial.print(" ,GateIP:");
  // Serial.println(WiFi.gatewayIP());
  // Serial.print("WIFI status is:");
  // Serial.print(WiFi.status()); 
  // digitalWrite(LED, LOW);
  // return;





  // if (wifi_ssid != "")                        //wifi_ssid不为空，意味着从网页读取到wifi
  // {
  //   Serial.println("用web配置信息连接.");
  //   WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); //c_str(),获取该字符串的指针
  //   storeWiFiIfoToEEPROM();
  // //  wifi_ssid = "";
  // //  wifi_pass = "";
  // } 
  // else                                        //未从网页读取到wifi
  // {
  //   Serial.println("使用上次WiFi信息连接中");
  //   //WiFi.begin();
  //   WiFi.begin(getWiFiInfoFromEEPROM(wifi_ssidval_add),getWiFiInfoFromEEPROM(wifi_passval_add));                             //begin()不传入参数，默认连接上一次连接成功的wifi
  // }
 
  // int Connect_time = 0;                       //用于连接计时，如果长时间连接不成功，复位设备
  // while (WiFi.status() != WL_CONNECTED)       //等待WIFI连接成功
  // {  
  //   Serial.print(".");                        //一共打印30个点点
  //   digitalWrite(LED, !digitalRead(LED));     
  //   delay(500);
  //   Connect_time ++;
                                       
  //   if (Connect_time > 2 * timeOut_s)         //长时间连接不上，重新进入配网页面
  //   { 
  //     digitalWrite(LED, !LOW);
  //     Serial.println("");                     //主要目的是为了换行符
  //     Serial.println("WIFI autoconnect fail, start AP for webconfig now...");
  //     wifiWebConfig();                           //开始配网功能
  //     return;                                 //跳出 防止无限初始化
  //   }
  // }
  
  // if (WiFi.status() == WL_CONNECTED)          //如果连接成功
  // {
  //   Serial.println("WIFI connect Success");
  //   Serial.printf("SSID:%s", WiFi.SSID().c_str());
  //   Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
  //   Serial.print("LocalIP:");
  //   Serial.print(WiFi.localIP());
  //   Serial.print(" ,GateIP:");
  //   Serial.println(WiFi.gatewayIP());
  //   Serial.print("WIFI status is:");
  //   Serial.print(WiFi.status());
  //   digitalWrite(LED, !HIGH);
  //   server.stop();                            //停止开发板所建立的网络服务器。
  // }

 
/**
 * 功能: 启动热点,提供配网页面（轮询响应网页事件）
 */
void wifiWebConfig() 
{
  initSoftAP();   
  initDNS();        
  initWebServer();  
  scanWiFi();
}
 
 
/**
 * 功能: 功能描述
 */
void restoreWiFi() {
  delay(500);
  //esp_wifi_restore();  //删除保存的wifi信息
  Serial.println("连接信息已清空,准备重启设备..");
  delay(10);
  blinkLED(LED, 5, 500); //LED闪烁5次         //关于LED，不需要可删除 
  //digitalWrite(LED, LOW);                    //关于LED，不需要可删除
}
 
/*
 * 检查wifi是否已经连接
 */
void checkConnect(bool reConnect) 
{
  if (WiFi.status() != WL_CONNECTED)           //wifi连接失败
  {
    if (digitalRead(LED) != !LOW) 
      digitalWrite(LED, !LOW);
    if (reConnect == true && WiFi.getMode() != WIFI_AP && WiFi.getMode() != WIFI_AP_STA ) 
    {
      Serial.println("WIFI未连接.");
      Serial.println("WiFi Mode:");
      Serial.println(WiFi.getMode());
      Serial.println("正在连接WiFi...");
      connectToWiFi(connectTimeOut_s);          //连接wifi函数 
    }
  } 
  else if (digitalRead(LED) != !HIGH)  
    digitalWrite(LED, !HIGH);                    //wifi连接成功 灯亮
}
 
/*
 * LED闪烁函数        //用不上LED可删除
 */
void blinkLED(int led, int n, int t) 
{
  for (int i = 0; i < 2 * n; i++) 
  {
    digitalWrite(led, !digitalRead(led));
    delay(t);
  }
}
 
 
/*
 * LED初始化
 */
void LEDinit()
{
  pinMode(LED, OUTPUT);                 //配置LED口为输出口
  digitalWrite(LED, LOW);               //初始灯灭
}
 
/*
 * 检测客户端DNS&HTTP请求
 */
void checkDNS_HTTP()
{
  dnsServer.processNextRequest();   //检查客户端DNS请求
  server.handleClient();            //检查客户端(浏览器)http请求
}


void storeWiFiIfoToEEPROM(){
    writeEEPROM(rom_variables[ROM_INDEX_SSID].address, wifi_ssid, rom_variables[ROM_INDEX_SSID].length);
    writeEEPROM(rom_variables[ROM_INDEX_PWD].address, wifi_pass, rom_variables[ROM_INDEX_PWD].length);
}

void getWiFiInfoFromEEPROM(){
  readEEPROM(rom_variables[ROM_INDEX_SSID].address,rom_variables[ROM_INDEX_SSID].length);
  readEEPROM(rom_variables[ROM_INDEX_PWD].address,rom_variables[ROM_INDEX_PWD].length);
}

/**
 * 功能: 写入数据到EEPROM
 * @param address EEPROM地址(int)
 * @param buffer 数据起始地址(char*)
 * @param length 写入数据长度(int)
 */
void writeEEPROM(int address, String &str, int length) {
    for (int i = 0; i < length; i++) {
        EEPROM.write(address + i, str[i]);
    }
    EEPROM.commit(); 
}
// void writeEEPROM(int address, String str) {
//     for (int i = 0; i < length; i++) {
//         EEPROM.write(address + i, str[i]);
//     }
//     EEPROM.commit(); 
// }
/**
 * 功能: 从EEPROM读取字符串
 * @param address: EEPROM地址(int)
 * @param length: 读取数据长度(int)
 * @return 读取内容(String)
 */
String readEEPROM(int address, int length) {
    char data[length]; // 创建字符数组
    for (int i = 0; i < length; i++) {
        data[i] = EEPROM.read(address + i); // 读取字符串内容
    }
    return String(data); // 返回字符串
}

