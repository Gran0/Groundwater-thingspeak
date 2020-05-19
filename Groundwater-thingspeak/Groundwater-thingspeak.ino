/*
 Name:		Groundwater_thingspeak.ino
 Created:	17.03.2020 11:59:51
 Author:	Lukas Granzer
*/


#include "GroundwaterMonitor.h"

GroundWaterMonitor gwm;

void setup() {
	bool deviceOnline;
	float data;
	uint8_t countdown = 15;


	deviceOnline = gwm.initialize();

	if (deviceOnline) {
		digitalWrite(LED, HIGH);
		data = gwm.measure();
		if(data != HW_ERROR_CODE && data != ECHO_ERROR_CODE)
			gwm.sendDataToCloud(data);
		digitalWrite(LED, LOW);
		/*
			Device is powered only 10 second, during normal work. In this case is not
			neccessary to turn on webserver (and mess WiFi band) - device only send data to cloud.

			In case of manual configuration, device is powered from USB port and after 
			short delay device turn on webserver with config interface.
		*/
		while (countdown--) {
			delay(1000);
		}
	}
	gwm.enableWebserver(true);
}


void loop() {
	gwm.idleTask();
	delay(10);
}
