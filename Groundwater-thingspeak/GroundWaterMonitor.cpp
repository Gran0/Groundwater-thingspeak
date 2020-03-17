#include "GroundWaterMonitor.h"
#include "WString.h"

GroundWaterMonitor::GroundWaterMonitor() {
	this->s = &Serial;
	this->counter = 0;
}
GroundWaterMonitor::~GroundWaterMonitor() {}

bool GroundWaterMonitor::initialize(const char wifiName[],const char wifiPass[]) {
	// Serial line for debugging
	s->begin(9600);
	s->println("\n\n\n> Ground water monitor <");

	
	// Wifi configuration
	s->print("\nConnecting WiFi(");
	s->print(wifiName);
	s->println(")");
	s->print("Status .");

	wifi.setHostname("Groundwater monitor");	// Name for this access point in Server mode
	wifi.mode(WIFI_MODE_AP);
	wifi.begin(wifiName, wifiPass);
	for (uint8_t i = 0; i < 30; i++)
	{	
		if (wifi.status() == WL_CONNECTED)	// Bingo
			break;

		if (i == 10) {	// Last chance ...
			wifi.begin(wifiName, wifiPass);
		}
		
		if (i == 29) {	// See you later :(
			s->println(" Connection timeout");
			return false;
		}

		s->print(".");
		delay(500);
	}
	s->println(" Connected");
	s->print("AP: ");
	s->println(wifiName);
	s->print("IP adress: ");
	s->println(wifi.localIP());


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

	return true;
}
void GroundWaterMonitor::webserver_handler() {
	server.handleClient();
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
