#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "EEPROM.h"
#include "HX711.h"
#include "Ticker.h"
#include "NetRequest.h"
#include <SegDisplay.h>
#include <Timer.h>
#include <Control.hpp>
#include <QRScanner.hpp>
class Control
{
	private:
	String Production_base = "";//需要使用UTF-8编码
	String Batch_number = "";
	String User_name = "";
	String Phone_number = "";
	String Unique_code ="";
	String Weight_val = "";

	String remote_host = "code.server.hzyichuan.cn";
	String base_url = "/hello";

	//unsigned long Weight_Maopi = 0;
	int tickerDispalycount = 0;

	QRScanner scanner;

	public:
		void init();
		void update();
};


#endif