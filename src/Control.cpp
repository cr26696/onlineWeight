#include "Control.hpp"

// 初始化函数
void Control::init() {
	// 初始化WiFiManager
	WiFiManager wifiManager;
	// 如果在设备上没有设置或设置不正确，则启动AP配置模式
	if (!wifiManager.autoConnect("YourDeviceName")) {
		Serial.println("Failed to connect and hit timeout");
		// 如果超时则重启
		delay(3000);
		ESP.reset();
		delay(5000);
	}
	Serial.println("Connected to WiFi");

	// 初始化其他组件
	EEPROM.begin(512); // 假设使用512字节的EEPROM


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
	res = wm.autoConnect();
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
	SegDisplayinit();
	SegWrite("upper","1234");
	SegWrite("lower","1234");

	HX711::init();     // 假设HX711有静态初始化函数
	SegDisplay::init(); // 假设段式显示器有静态初始化函数
	Timer::init();     // 假设计时器有静态初始化函数
	QRScanner::init();
};

// 更新函数
void Control::update() {
	// 更新显示
	tickerDispalycount++;
	if (tickerDispalycount >= 10) { // 每秒更新一次显示
		tickerDispalycount = 0;
		// 获取当前重量值
		Weight_val = String(HX711::getWeight()); // 假设HX711有一个获取重量的方法
		// 更新段式显示器
		SegDisplay::show(Weight_val); // 假设段式显示器有一个显示字符串的方法
	}

	// 发送网络请求
	NetRequest::post(remote_host + base_url, 
		"{\"Production_base\":\"" + Production_base + "\",\"Batch_number\":\"" + Batch_number +
		"\",\"User_name\":\"" + User_name + "\",\"Phone_number\":\"" + Phone_number +
		"\",\"Unique_code\":\"" + Unique_code + "\",\"Weight_val\":\"" + Weight_val + "\"}");

	ScanQRcode();
  	weightConfig(); //是否需要更新毛皮?
  //UploadData(Production_base, Batch_number, Unique_code, User_name, Phone_number, Weight_val,0);

}