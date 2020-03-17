#include "GroundWaterMonitor.h"
#include "WString.h"

GroundWaterMonitor::GroundWaterMonitor() {
	this->s = &Serial;
	this->counter = 0;
}
GroundWaterMonitor::~GroundWaterMonitor() {}

/*
	Params:
		wifiName - WiFi name in local network
		wifiPass - Password to local network
	Return value:
		Result of connecting to local WiFi network
*/
bool GroundWaterMonitor::initialize(const char wifiName[],const char wifiPass[]) {
	bool internetAvailable = false;
	// Serial line for debugging
	s->begin(9600);
	s->println("\n\n\n> Ground water monitor <");

	
	// Wifi configuration
	s->print("\nConnecting WiFi(");
	s->print(wifiName);
	s->println(")");
	s->print("Status .");

	wifi.setHostname("Groundwater monitor");	// Name for this access point in Server mode
	wifi.mode(WIFI_MODE_STA);					// Work as end-device
	wifi.begin(wifiName, wifiPass);
	for (uint8_t i = 0; i < 30; i++)
	{	
		if (wifi.status() == WL_CONNECTED) {	// Bingo
			internetAvailable = true;
			break;
		}

		if (i == 10) {	// Last chance ...
			wifi.begin(wifiName, wifiPass);
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
		s->println(wifiName);
		s->print("IP adress: ");
		s->println(wifi.localIP());

		
	}
	// Connection failed to local WiFi network.
	else {

		// Webserver for device configuration
		this->server.on("/", [&]() {
			page_index();
		});
		this->server.onNotFound([&]() {
			page_notFound();
		});
		this->server.begin(80);

		// DNS for request handling
		this->dns = MDNSResponder();
		dns.begin("configuration");
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
void GroundWaterMonitor::countup() {
	this->counter++;
}
uint16_t GroundWaterMonitor::get() {
	s->printf("%d", counter);
	return this->counter;
}
