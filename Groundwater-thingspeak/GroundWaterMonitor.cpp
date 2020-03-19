#include "GroundWaterMonitor.h"
#include "WString.h"

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
	
	this->wifiAPchannel = memory.readULong(Address.APchannel);

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
	memory.writeULong(Address.APchannel, 5);

	memory.writeBytes(Address.ApiKey, "xxxXXXxxx\0", 10);

	memory.writeByte(Address.INICIALIZED, 123);
	memory.commit();
}
void GroundWaterMonitor::page_index() {
	char html[30];
	sprintf(html, "This is the main page - %d", this->counter);

	sprintf(this->HTML_buffer, "%s%s%s", this->htmlPrefix, html, this->htmlPostfix);
	server.send(200, "text/html", this->HTML_buffer);
}
void GroundWaterMonitor::page_notFound() {
	char html[] = "Sorry - page not found!";

	sprintf(this->HTML_buffer, "%s%s%s", this->htmlPrefix, html,this->htmlPostfix);
	server.send(404, "text/html", this->HTML_buffer);
}

///////////////////////////////////////////////
