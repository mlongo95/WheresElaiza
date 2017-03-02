/*
	WheresElaiza

		SIM 808 based GSM/GPS Dog Tracker 
	
	By Marco Longoria 

*/

#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

#define 
char replybuffer[255];
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
//	HardwareSerial *fonaSerial = &Serial1; 
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

void setup(){
	while (!Serial);
	Serial.begin(115200);
	Serial.println(F("WheresElaiza!"));
	Serial.println(F("Initializing SIM808..."));

	fonaSerial->begin
}