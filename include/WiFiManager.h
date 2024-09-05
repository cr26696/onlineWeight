#ifndef __WIFIMANAGER_H__
#define __WIFIMANAGER_H__
 
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include "EEPROM.h"

//变量存储序号定义
#define ROM_INDEX_SSID 0
#define ROM_INDEX_PWD 1
#define DNS_PORT 53  //设置DNS端口号
#define WEB_PORT 80  //设置Web端口号
#define connectTimeOutSec  6;//默认6秒超时
class WiFiManager {
public:
  //构造函数
  WiFiManager(byte TimeoutSec);
  void checkConnect(bool reConnect);    //检测wifi是否已经连接
  void restoreWiFi();                   //删除保存的wifi信息
  // void LEDinit();                       //LED初始化
  void checkDNS_HTTP();                 //检测客户端DNS&HTTP请求
  bool connectToWiFi(int timeOut_s);    //连接WiFi
  void wifiWebConfig();
  void handleRoot();                    //处理网站根目录的访问请求
  void handleConfigWifi();             //提交数据后的提示页面
  void handleNotFound();                //处理404情况的函数'handleNotFound'
private:        
  const String ROOT_HTML_1 = "<!DOCTYPE html><html><head>  <meta charset=\"UTF-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />  <title>登录页面</title>  <style>   #content,.login,.login-card a,.login-card h1,.login-help{text-align:center}body,html{margin:0;padding:0;width:100%;height:100%;display:table}#content{font-family:\'Source Sans Pro\',sans-serif;-webkit-background-size:cover;-moz-background-size:cover;-o-background-size:cover;background-size:cover;display:table-cell;vertical-align:middle}.login-card{padding:40px;width:274px;background-color:#F7F7F7;margin:0 auto 10px;border-radius:20px;box-shadow:8px 8px 15px rgba(0,0,0,.3);overflow:hidden}.login-card h1{font-weight:400;font-size:2.3em;color:#1383c6}.login-card h1 span{color:#f26721}.login-card img{width:70%;height:70%}.login-card input[type=submit]{width:100%;display:block;margin-bottom:10px;position:relative}.login-card input[type=text],input[type=password]{height:44px;font-size:16px;width:100%;margin-bottom:10px;-webkit-appearance:none;background:#fff;border:1px solid #d9d9d9;border-top:1px solid silver;padding:0 8px;box-sizing:border-box;-moz-box-sizing:border-box}.login-card input[type=text]:hover,input[type=password]:hover{border:1px solid #b9b9b9;border-top:1px solid #a0a0a0;-moz-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);-webkit-box-shadow:inset 0 1px 2px rgba(0,0,0,.1);box-shadow:inset 0 1px 2px rgba(0,0,0,.1)}.login{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:36px;padding:0 8px}.login-submit{-webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#4d90fe}.login-submit:disabled{opacity:.6}.login-submit:hover{border:0;text-shadow:0 1px rgba(0,0,0,.3);background-color:#357ae8}.login-card a{text-decoration:none;color:#666;font-weight:400;display:inline-block;opacity:.6;transition:opacity ease .5s}.login-card a:hover{opacity:1}.login-help{width:100%;font-size:12px}.list{list-style-type:none;padding:0}.list__item{margin:0 0 .7rem;padding:0}label{display:-webkit-box;display:-webkit-flex;display:-ms-flexbox;display:flex;-webkit-box-align:center;-webkit-align-items:center;-ms-flex-align:center;align-items:center;text-align:left;font-size:14px;}input[type=checkbox]{-webkit-box-flex:0;-webkit-flex:none;-ms-flex:none;flex:none;margin-right:10px;float:left}.error{font-size:14px;font-family:Arial,sans-serif;font-weight:700;height:25px;padding:0 8px;padding-top: 10px; -webkit-appearance:none;-moz-appearance:none;appearance:none;border:0;color:#fff;text-shadow:0 1px rgba(0,0,0,.1);background-color:#ff1215}@media screen and (max-width:450px){.login-card{width:70%!important}.login-card img{width:30%;height:30%}}  </style></head><body style=\"background-color: #e5e9f2\"><div id=\"content\"> <form name=\'input\' action=\'/configwifi\' method=\'POST\'>  <div class=\"login-card\">    <h1>WiFi登录</h1>   <form name=\"login_form\" method=\"post\" action=\"$PORTAL_ACTION$\">   <input type=\"text\" name=\"ssid\" placeholder=\"请输入 WiFi 名称\" id=\"auth_user\" list = \"data-list\"; style=\"border-radius: 10px\">    <datalist id = \"data-list\">";
  const String ROOT_HTML_2 = "</datalist> <input type=\"password\" name=\"password\" placeholder=\"请输入 WiFi 密码\" id=\"auth_pass\"; style=\"border-radius: 10px\">      <div class=\"login-help\">        <ul class=\"list\">          <li class=\"list__item\">          </li>        </ul>      </div>   <input type=\"submit\" class=\"login login-submit\" value=\"确 定 连 接\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> <!-- <form name=\'input\' action=\'/English\' method=\'POST\'>    <input type=\"submit\" class=\"login login-submit\" value=\"English\" id=\"login\"; disabled; style=\"border-radius: 15px\"  >    </form> --></body></html>";
  const char* HOST_NAME = "ESP12F"; //网络环境中设备名
  const char* AP_SSID  = "设备配网WiFi";//设置AP热点名称
  //const char* AP_PASS  = "";//暂无AP热点密码
  const IPAddress apIP = IPAddress(192, 168, 4, 1); 
  const byte TimeoutSec = connectTimeOutSec;
  std::vector<EepromVariable> rom_variables;//rom变量容器
  //-----------------------
  String wifi_ssid = "";  //目标wifi名
  String wifi_pass = "";  //目标wifi密
  String scanNetworksID = ""; //扫描到的所有wifi，用于生成html
  bool configIChanged = 0; //配网信息更新标志

  DNSServer dnsServer;
  ESP8266WebServer webserver;
  Ticker T_restart;

  void initSoftAP();                    //进入AP模式
  void initDNS();                       //开启DNS服务器
  void initWebServer();                 //初始化WebServer
  bool initWiFi();                      //初始化WiFi
  bool scanWiFi();                      //扫描附近的WiFi，为了显示在配网界面
  void wifiWebConfig();                    //配置配网功能
  void blinkLED(int led, int n, int t); //LED闪烁函数        //用不上LED可删除
  void storeWiFiIfoToEEPROM();
  void getWiFiInfoFromEEPROM();
  void writeEEPROM(int address, String &str, int length);
  String readEEPROM(int address, int length);
  

};
#endif