#include "GroundWaterMonitor.h"
#include "WString.h"
#include "string.h"

GroundWaterMonitor::GroundWaterMonitor() {
	this->s = &Serial;
	this->counter = 0;
}
GroundWaterMonitor::~GroundWaterMonitor() {}

/*
	Return value:
		Result of connecting to local WiFi network
*/
bool GroundWaterMonitor::initialize() {
	bool internetAvailable = false;

	// Pin inicialization

	// EEPROM connect
	memory.begin(200);
	loadEEPROMconfig();

	// Serial line for debugging
	s->begin(9600);
	s->println("\n\n\n> Ground water monitor <");

	
	// Wifi configuration
	s->print("\nConnecting to WiFi(");
	s->print(this->wifiName);
	s->println(")");
	s->print("Status .");

	wifi.mode(WIFI_MODE_STA);					// Work as end-device
	wifi.begin(this->wifiName, this->wifiPasswd);
	for (uint8_t i = 0; i < 30; i++)
	{	
		if (wifi.status() == WL_CONNECTED) {	// Bingo
			internetAvailable = true;
			break;
		}

		if (i == 10) {	// Last chance ...
			wifi.begin(this->wifiName, this->wifiPasswd);
		}
		
		if (i == 29) {	// See you later :(
			s->println(" Connection timeout");
			internetAvailable = false;
			break;
		}

		s->print(".");
		delay(500);
	}

	if (internetAvailable) {
		s->println(" Connected");
		s->print("AP: ");
		s->print(this->wifiName);
		s->print(" ");
		s->print(wifi.RSSI());
		s->println("dBm");
		s->print("IP adress: ");
		s->println(wifi.localIP());	
	}

	return internetAvailable;
}

void GroundWaterMonitor::webserverHandler() {
	server.handleClient();
	
}
void GroundWaterMonitor::sendDataToCloud(float value) {
	String postStr = this->apiKey;
	
	if (this->client.connect(this->thingspeakWWW,80)) {
		postStr += "&field1=";
		postStr += String(value);
		/*
		postStr += "&field2=";
		postStr += String(temp);  
		*/
		postStr += "\r\n\r\n";

		this->client.print("POST /update HTTP/1.1\n");
		this->client.print("Host: api.thingspeak.com\n");
		this->client.print("Connection: close\n");
		this->client.print("X-THINGSPEAKAPIKEY: " + this->apiKey + "\n");
		this->client.print("Content-Type: application/x-www-form-urlencoded\n");
		this->client.print("Content-Length: ");
		this->client.print(postStr.length());
		this->client.print("\n\n");
		this->client.print(postStr);
		delay(800);

		this->s->println("Data send to Thingspeak API.");
		this->s->print("Response:");
		while (this->client.available()) {
			String line = this->client.readStringUntil('\r');
			if (line.indexOf("Status:") >= 0) {
				this->s->print(line);
			}
		}
	}
	else {
		this->s->println("Thingspeak API doesn't answer.");
	}
	this->client.flush();
	this->client.stop();	
}
float GroundWaterMonitor::measure() {
	return 123.45f;
}

void GroundWaterMonitor::enableWebserver() {
	wifi.disconnect();

	wifi.mode(WIFI_MODE_AP);	// Work as router
	IPAddress ip(192, 168, 100, 101), mask(255, 255, 255, 0), gateway(192, 168, 100, 100);
	wifi.softAPConfig(ip, gateway, mask);
	wifi.setHostname("Groundwater monitor");	// Name for this access point
	wifi.softAP(this->wifiAPname, this->wifiAPpasswd, this->wifiAPchannel,150,2);

	s->println("\n\nStarting access point ...");
	s->print("AP name: ");
	s->println(this->wifiAPname);
	s->print("Password: ");
	s->println(this->wifiAPpasswd);
	s->println("More on configuration page: http://configuration/ \n\n");

	// Webserver for device configuration
	this->server.on("/", [&]() {
		page_index();
	});
	this->server.on("/manual_measure", [&]() {
		runManualMeasure();
	});
	this->server.onNotFound([&]() {
		page_notFound();
	});
	this->server.begin(80);	// port 80

	// DNS for request handling
	this->dns = MDNSResponder();
	dns.begin("configuration");
}
void GroundWaterMonitor::loadEEPROMconfig() {
	int isInit = memory.readByte(Address.INICIALIZED);
	String buff;

	// Check if eeprom is initialized
	if (isInit != 123) {
		resetEEPROMdata();
	}
	// Copy data EEPROM => RAM
	this->apiKey = memory.readString(Address.ApiKey);
	
	buff = memory.readString(Address.APname);
	buff.toCharArray(this->wifiAPname, 30, 0);

	buff = memory.readString(Address.APpasswd);
	buff.toCharArray(this->wifiAPpasswd, 40, 0);
	
	this->wifiAPchannel = memory.readUChar(Address.APchannel);
	this->sensorHeight = memory.readShort(Address.SensorHeight);

	buff = memory.readString(Address.WiFiName);
	buff.toCharArray(this->wifiName, 40, 0);

	buff = memory.readString(Address.WiFipasswd);
	buff.toCharArray(this->wifiPasswd, 40, 0);

}
void GroundWaterMonitor::resetEEPROMdata() {
	memory.writeBytes(Address.WiFiName, "Default name\0",13);
	memory.writeBytes(Address.WiFipasswd, "not set\0", 8);
	
	memory.writeBytes(Address.APname, "Groundwater monitor\0", 20);
	memory.writeBytes(Address.APpasswd, "password\0", 9);
	memory.writeUChar(Address.APchannel, 5);

	memory.writeBytes(Address.ApiKey, "xxxXXXxxx\0", 10);
	memory.writeShort(Address.SensorHeight, 10);

	memory.writeByte(Address.INICIALIZED, 123);
	memory.commit();
}
void GroundWaterMonitor::storeEEPROMconfig() {
	memory.writeBytes(Address.WiFiName, this->wifiName, 40);
	memory.writeBytes(Address.WiFipasswd, this->wifiPasswd,40);

	memory.writeBytes(Address.APname, this->wifiAPname,30);
	memory.writeBytes(Address.APpasswd, this->wifiAPpasswd, 40);
	memory.writeUChar(Address.APchannel, this->wifiAPchannel);

	memory.writeString(Address.ApiKey, this->apiKey);
	memory.writeShort(Address.SensorHeight, this->sensorHeight);
	memory.writeByte(Address.INICIALIZED, 123);
	memory.commit();
}
void GroundWaterMonitor::page_index() {

	// Data from HTML form
	if (server.hasArg("lan_ssid")) {
		 strcpy(this->wifiName, server.arg("lan_ssid").c_str());
	}
	if (server.hasArg("lan_passwd")) {
		strcpy(this->wifiPasswd, server.arg("lan_passwd").c_str());
	}
	if (server.hasArg("hotspot_ssid")) {
		strcpy(this->wifiAPname, server.arg("hotspot_ssid").c_str());
	}
	if (server.hasArg("hotspot_passwd")) {
		strcpy(this->wifiAPpasswd, server.arg("hotspot_passwd").c_str());
	}
	if (server.hasArg("hotspot_channel")) {
		long int ch = server.arg("hotspot_channel").toInt();
		if (ch > 0 && ch < 12)
			this->wifiAPchannel = (uint8_t)ch;
	}
	if (server.hasArg("api_key")) {
		this->apiKey = server.arg("api_key");
	}
	if (server.hasArg("height")) {
		long int h = server.arg("height").toInt();
		this->sensorHeight = (uint16_t) h;
	}
	if(server.args() != 0)
		storeEEPROMconfig();

	// Formating HTML page with data
	String html = completeHTMLValue(this->htmlIndex, "lan_ssid",this->wifiName);
	html = completeHTMLValue(html,"lan_passwd",this->wifiPasswd);
	html = completeHTMLValue(html, "hotspot_ssid", this->wifiAPname);
	html = completeHTMLValue(html, "hotspot_passwd", this->wifiAPpasswd);
	
	html = completeHTMLValue(html, "hotspot_channel",(int) this->wifiAPchannel);
	html = completeHTMLValue(html, "api_key", this->apiKey);
	html = completeHTMLValue(html, "height", this->sensorHeight);

	sprintf(this->HTML_buffer, "%s%s%s", this->htmlPrefix, html.c_str(), this->htmlPostfix);
	server.send(200, "text/html", this->HTML_buffer);
}
void GroundWaterMonitor::page_notFound() {
	char html[] = "<h4>404 ERROR - page not found!</h4>";

	sprintf(this->HTML_buffer, "%s%s%s", this->htmlPrefix, html, this->htmlPostfix);
	server.send(404, "text/html", this->HTML_buffer);
}
void GroundWaterMonitor::runManualMeasure() {
	float value = measure();
	// 
	page_index();
}
// Functions find in HTML form <input> element with selected "name" and complete value attribute
String GroundWaterMonitor::completeHTMLValue(const char* html, String input_name, char* value) {
	String s = html;
	unsigned int position = s.indexOf(input_name, 0);	// Find input in html code
	position = s.indexOf("value=", position);		// Find value parameter

	String replace = value;
	String x = s.substring(position, position+10);		// Complete value
	x.replace("''", "'" + replace + "'");

	s = s.substring(0, position) + x + s.substring(position + 8);

	return s;
}
String GroundWaterMonitor::completeHTMLValue(String html, String input_name, char* value) {
	String s = html;
	uint16_t position = s.indexOf(input_name, 0);	// Find input in html code
	position = s.indexOf("value=", position);		// Find value parameter

	String replace = value;
	String x = s.substring(position, position + 10);		// Complete value
	x.replace("''", "'" + replace + "'");

	s = s.substring(0, position) + x + s.substring(position + 8);

	return s;
}
String GroundWaterMonitor::completeHTMLValue(String html, String input_name, String value) {
	String s = html;
	uint16_t position = s.indexOf(input_name, 0);	// Find input in html code
	position = s.indexOf("value=", position);		// Find value parameter

	String x = s.substring(position, position + 10);		// Complete value
	x.replace("''", "'" + value + "'");

	s = s.substring(0, position) + x + s.substring(position + 8);

	return s;
}
String GroundWaterMonitor::completeHTMLValue(String html, String input_name, int value) {
	String s = html;
	uint16_t position = s.indexOf(input_name, 0);	// Find input in html code
	position = s.indexOf("value=", position);		// Find value parameter

	String s_val = String(value);
	String x = s.substring(position, position + 10);		// Complete value
	x.replace("''", "'" + s_val + "'");

	s = s.substring(0, position) + x + s.substring(position + 8);

	return s;
}

///////////////////////////////////////////////
