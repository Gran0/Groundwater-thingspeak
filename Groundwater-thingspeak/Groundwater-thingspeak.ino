/*
 Name:		Groundwater_thingspeak.ino
 Created:	17.03.2020 11:59:51
 Author:	Lukas Granzer
*/


#include "GroundwaterMonitor.h"

GroundWaterMonitor gwm;
uint16_t x;

void setup() {
	gwm.initialize("zahrada","tajneheslo");

}


void loop() {
	gwm.countup();
	gwm.webserver_handler();
	delay(100);
}
