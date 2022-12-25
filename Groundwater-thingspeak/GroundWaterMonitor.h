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

// Pinout
#define TRIGGER 33
#define ECHO 17
#define LED 19
#define RESET 5

// Error code
#define HW_ERROR_CODE   -99.0f
#define ECHO_ERROR_CODE -88.0f

class GroundWaterMonitor
{
public:
	// Variables
	uint16_t counter;

	// Methods
	GroundWaterMonitor();
	~GroundWaterMonitor();

	bool initialize();

	void idleTask();
	float measure();
	void sendDataToCloud(float);
	void sendDataToLocal(float);
	void enableWebserver(bool);

	bool useTSserver();


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
	uint8_t fieldID;

	// Local server information
	bool useOnlyLocalServer;
	IPAddress localServerIP;
	uint16_t  localServerPort;


	// Measurement
	uint16_t sensorHeight;
	float waterLevel;
	bool  periodicalMode;
	uint16_t period;		// minutes
	uint32_t timeOfLastMeas;	// milisecond
	uint16_t echoDelay;
	
	char HTML_buffer[4500];		// Memory for formating html pages

	// Methods
	void page_notFound();
	void page_index();

	String completeHTMLValue(const char*,String,char*);
	String completeHTMLValue(String, String, char*);
	String completeHTMLValue(String, String, String);
	String completeHTMLValue(String, String, int);
	String completeHTMLCheckbox(String, String, String);

	void loadEEPROMconfig();
	void resetEEPROMdata();
	void storeEEPROMconfig();

	void runManualMeasure();
	void periodicalMeasure();
	
	// Constants - stored in FLASH memory
	char* const htmlPrefix = "<!DOCTYPE html><html lang='en' xmlns='http://www.w3.org/1999/xhtml'><head><meta charset='utf-8' /><title>Groundwater monitor</title><style type='text/css'>html {background-color: #1F1F2D;}body {width: 640px;margin: auto;background-image: radial-gradient( circle farthest-corner at 50.3% 44.5%, rgba(116,147,179,1) 0%, rgba(62,83,104,1) 100.2% );;color: #e7e7e7;padding: 0px;border-radius: 10px;font-family: century;}h1 {text-align: center;color: white;background: linear-gradient(to bottom,#027db3,#00bf8a);margin: 0px;border-top-left-radius: 10px;border-top-right-radius: 10px;height: 100px;padding-top: 40px;}h2 {background-color: darkslategrey;text-indent: 20px;padding-top: 5px;padding-bottom: 3px;margin-bottom: 0px;width: 200px;text-align: left;border-top-left-radius: 5px;border-top-right-radius: 15px;font-size: 13pt;border-bottom: solid 1px gray;}h3 {color: slategrey;font-size: 13pt;margin-top: 0px;padding-top: 12px;text-indent: 15px;margin-bottom: 5px;}section {padding: 10px 40px;}article {background-color: black;display: block;margin: 0px;font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;}span {color: aqua;font-size: 15pt;margin-left: 15px;}label {width: 150px;display: inline-block;margin-left: 40px;}#setting label {width: 200px;}#setting button {margin-left: 60px;}button {background-color: #FFE805;border-radius: 5px;border-style: none outset outset none;padding: 4px 10px;margin-left: 100px;}p {margin-left: 30px;text-align: justify;margin-top: 0px;padding: 10px;}input {margin-left: 5px;text-align: center;border-radius: 3px;border-style: hidden;font-family: Consolas;margin-right: 8px;}footer {text-align: center;margin-top: 50px;background-color: darkslategrey;}</style></head><body><h1>Groundwater monitor</h1>";
	char* const htmlPostfix = "<footer>Build by J. Klokocnik and L. Granzer - 2020</footer></body></html>";

	char* const htmlIndex = "<section><div id='first'><h2>Device status</h2><article><p>Last measured value: <span id='number'>xxx cm</span> <a href='/manual_measure'><button>Start measuring</button></a></p></article></div><form action='/' method='get'><div><h2>Network</h2><article><h3>Internet connection over WiFi</h3><label>WiFi name:</label><input type='text' name='lan_ssid' value='' maxlength='40' /><br /><label>Password:</label><input type='text' name='lan_passwd' value='' maxlength='40' /><br /><h3>Device PAN WiFi</h3><label>WiFi hotspot name:</label><input type='text' name='hotspot_ssid' value='' maxlength='30' /><br /><label>Password:</label><input type='text' name='hotspot_passwd' value='' maxlength='40' /><br /><label>Channel:</label><input type='number' name='hotspot_channel' value='' min='1' step='1' max='11' /><h3>Thingspeak channel settings</h3><label>Write API key:</label><input type='text' name='api_key' value='' /><br /><label>Field ID:</label><input type='number' name='field_id' value='' min='1' step='1' max='8' /><br /><label>Activate sending</label><input type='radio' value='thcloud' name='activ' /><br /><h3>Local server</h3><label>Server IP</label><input type='number' name='ip1' value='' min='0' step='1' max='255'/>.<input type='number' name='ip2' value='' min='0' step='1' max='255' />.<input type='number' name='ip3' value='' min='0' step='1' max='255' />.<input type='number' name='ip4' value='' min='0' step='1' max='255' /><br /><label>Server port</label><input type='number' name='port' value='' min='10' step='1' max='50000' /><br/><label>Activate sending</label><input type='radio' value='local' name='activ' /> <br/><label></label></article></div><div><h2>Sensor settings</h2><article id='setting'><label>Sensor height <br />from well bottom:</label><input type='number' min='10' max='1000' name='height' value='' />cm<hr /><label>Measuring mode</label><input type='radio' value='off' name='periodical' />On start<input type='radio' value='on' name='periodical' />Periodical<br /><label>Period </label><input type='time' name='period' value='' />hh:mm<button type='submit'>Save settings</button><br /><p>Device sends data only once in <strong>On start</strong> mode.This mode is suitable for intermittent power supply applications.<strong>Periodical mode</strong> measures and sends data in set interval.</p></article></div></form></section>";
	char* const thingspeakWWW = "api.thingspeak.com";

};

