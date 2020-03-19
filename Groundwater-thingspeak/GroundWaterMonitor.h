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
	uint32_t wifiAPchannel;

	// Local network
	char wifiName[40];
	char wifiPasswd[40];

	// Thingverse
	String apiKey = "6KXQXSUO8O1319UP";

	char HTML_buffer[4000];		// Memory for formating html pages

	// Methods
	void page_notFound();
	void page_index();

	void loadEEPROMconfig();
	void resetEEPROMdata();
	
	// Constants - stored in FLASH memory
	char* const htmlPrefix = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Studna</title><style type=\"text/css\">html{background-color:#1F1F2D;}body{width:680px;margin:auto;background-color:#2E2E52;color:#e7e7e7;padding:10px 40px;border-radius:10px;font-family:century;}h1{text-align:center;color:#2121C6;}a{color:#FF9405;}button{background-color:#FFE805;border-radius:5px;border-style:none outset outset none;padding:4px 10px;}p{margin-left:30px;text-align:justify;}input{margin-left:5px 15px;}</style></head><body><h1>Hladinomer ve studni</h1>";
	char* const htmlPostfix = "</body></html>";
	char* const thingspeakWWW = "api.thingspeak.com";
	


};

