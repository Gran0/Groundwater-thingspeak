#pragma once
#include <cstdint>
#include <cstdbool>
#include <HardwareSerial.h>

#include <ArduinoJson.hpp>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTP_Method.h>
#include "MemoryAdd.h"

class GroundWaterMonitor
{
public:
	// Variables
	uint16_t counter;

	// Methods
	GroundWaterMonitor();
	~GroundWaterMonitor();

	bool initialize();

	void webserverHandler();
	float measure();
	void sendDataToCloud(float);
	void enableWebserver();

private:
	// Variables
	HardwareSerial *s;
	WiFiClass wifi;
	WiFiClient client;
	WebServer server;
	MDNSResponder dns;
	EEPROMClass memory;
	MemoryAdd Address;

	// This device access point
	char wifiAPname[30];
	char wifiAPpasswd[40];
	uint8_t wifiAPchannel;

	// Local network
	char wifiName[40];
	char wifiPasswd[40];

	// Thingverse
	String apiKey = "";

	// Measure constant
	uint16_t sensorHeight;

	char HTML_buffer[4000];		// Memory for formating html pages

	// Methods
	void page_notFound();
	void page_index();

	String completeHTMLValue(const char*,String,char*);
	String completeHTMLValue(String, String, char*);
	String completeHTMLValue(String, String, String);
	String completeHTMLValue(String, String, int);

	void loadEEPROMconfig();
	void resetEEPROMdata();
	void storeEEPROMconfig();

	void runManualMeasure();
	
	// Constants - stored in FLASH memory
	char* const htmlPrefix = "<!DOCTYPE html><html lang='en' xmlns='http://www.w3.org/1999/xhtml'><head><meta charset='utf-8' /><title>Groundwater monitor</title><style type='text/css'>html {background-color: #1F1F2D;}body {width: 640px;margin: auto;background-image: radial-gradient( circle farthest-corner at 50.3% 44.5%,  rgba(116,147,179,1) 0%, rgba(62,83,104,1) 100.2% );;color: #e7e7e7;padding: 0px;border-radius: 10px;font-family: century;}h1 {text-align: center;color: white;background: linear-gradient(to bottom,#027db3,#00bf8a);margin: 0px;border-top-left-radius: 10px;border-top-right-radius: 10px;height: 100px;padding-top:40px;}h2{background-color:darkslategrey;text-indent: 20px;padding-top: 5px;padding-bottom: 3px;margin-bottom: 0px;width: 200px;text-align:left;border-top-left-radius: 5px;border-top-right-radius: 15px;font-size: 13pt;border-bottom: solid 1px gray;}h3{color:slategrey;font-size:13pt;margin-top:0px;padding-top:12px;text-indent:15px;margin-bottom:5px;}section{padding: 10px 40px;}article{background-color:black;display:block;margin:0px;font-family:'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;}span{color:aqua;font-size:15pt;margin-left:15px;}label{width: 150px;display:inline-block;margin-left:40px;}button {background-color: #FFE805;border-radius: 5px;border-style: none outset outset none;padding: 4px 10px;margin-left:150px;}p {margin-left: 30px;text-align: justify;margin-top:0px;padding: 10px;}input {margin-left: 5px;text-align:center;border-radius: 3px;border-style:hidden;font-family:Consolas;margin-right: 8px;}footer{text-align:center;margin-top:50px;background-color:darkslategrey;}</style></head><body><h1>Groundwater monitor</h1>";
	char* const htmlPostfix = "</body></html>";

	char* const htmlIndex = "<section><div id='first'><h2>Device status</h2><article><p>Last measured value: <span id='number'>485 cm</span> <a href='/manual_measure'><button>Start measuring</button></a></p></article></div><form action='/' method='post'><div><h2>Network</h2><article><h3>Internet connection over WiFi</h3><label>WiFi name:</label><input type='text' name='lan_ssid' value='' maxlength='40' /><br /><label>Password:</label><input type='text' name='lan_passwd' value='' maxlength='40' /><br /><h3>Device PAN WiFi</h3><label>WiFi hotspot name:</label><input type='text' name='hotspot_ssid' value='' maxlength='30' /><br /><label>Password:</label><input type='text' name='hotspot_passwd' value='' maxlength='40' /><br /><label>Channel:</label><input type='number' name=hotspot_channel' value='' min='1' step='1' max='11' /><h3>Thingspeak API</h3><label>Write API key:</label><input type='text' name='api_key' value='' /><br /><label></label></article></div><div><h2>Sensor location</h2><article><label>Sensor height <br />from well bottom:</label><input type='number' min='10' max='1000' name='height' value='' />cm<button type='submit'>Save settings</button><br /><label></label></article></div></form></section><footer>Build by J. Klokočník and L. Granzer - 2020</footer>";
	char* const thingspeakWWW = "api.thingspeak.com";
	


};

