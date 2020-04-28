/*
 Name:		WiFi_range_test.ino
 Created:	23.04.2020 19:03:10
 Author:	Lukas Granzer
*/

#include <WiFiType.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFi.h>


int8_t db;
void setup() {
	Serial.begin(19200);
	bool internetAvailable;

	Serial.println("\nConnecting to WiFi");
	Serial.print("Status .");

	WiFi.mode(WIFI_MODE_STA);					
	WiFi.begin("Hafik", "nasepeggy78");
	for (uint8_t i = 0; i < 30; i++)
	{
		if (WiFi.status() == WL_CONNECTED) {
			internetAvailable = true;
			break;
		}

		if (i == 10) {	// Last chance ...
			WiFi.begin("zahrada", "nasepeggy78");
		}

		if (i == 29) {	// See you later :(
			Serial.println(" Connection timeout");
			internetAvailable = false;
			break;
		}

		Serial.print(".");
		delay(500);
	}


}

// the loop function runs over and over again until power down or reset
void loop() {
	WiFi.begin("Hafik", "nasepeggy78");
	delay(500);
	db = WiFi.RSSI();
	Serial.print(db);
	Serial.println(" dB");
	WiFi.disconnect();
	delay(500);
}
