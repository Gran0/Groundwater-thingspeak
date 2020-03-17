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

	deviceOnline = gwm.initialize("zahrada", "nasepeggy78");
	
	if (deviceOnline) {
		data = gwm.measure();
		gwm.sendDataToCloud(data);
	}

}


void loop() {
	gwm.countup();
	gwm.webserverHandler();
	delay(100);
}
