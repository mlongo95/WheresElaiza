/*
	WheresElaiza

		SIM 808 based GSM/GPS Dog Tracker 
	
	By Marco Longoria 2017

*/

#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>
#define SIM808_RX 2
#define SIM808_TX 3
#define FONA_RST 4
uint8_t type;
char replybuffer[255];
SoftwareSerial fonaSS = SoftwareSerial(SIM808_RX, SIM808_TX);
SoftwareSerial *fonaSerial = &fonaSS;
//	HardwareSerial *fonaSerial = &Serial1; 
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
float latitude, longitude, speed_kph, heading, speed_mph, altitude;
unsigned long currentMillis;
unsigned long previousMillis = 0;
unsigned long timer;
unsigned long timer2 = 0;
boolean gps_success;
const int power = #;


void setup(){
	digitalWrite(power, OUTPUT);
	while (!Serial);
	Serial.begin(115200);
	Serial.println(F("WheresElaiza GSM/GPS Tracker"));
	Serial.println(F("Initializing Network..."));

	fonaSerial->begin(4800);
	if (! fona.begin(*fonaSerial)){
		Serial.print(F("Couldn't find Network"));
		while(1);
	}

	type = fona.type();
	Serial.println(F("Network OK"));
	Serial.print(F("Found "));
	switch (type) {
		case FONA808_V1:
			Serial.println(F("SIM808 (v1)")); break;
		case FONA808_V2:
			Serial.println(F("SIM808 (v2)")); break;
		default:
			Serial.println(F("???")); break;
	}
}

void loop(){
	WheresElaiza();
	sendSMS();
	Serial.println();
	Serial.print("Found Elaiza!");

}
void sendSMS(){
	fonaSS.print("AT+CMGF=1\r");
	delay(1000);
	fonaSS.print("AT+CMGS=\"2108186119\"\r");
	delay(1000);
	fonaSS.println(latitude, 6);
	fonaSS.println(longitude, 6);
	fonaSS.print("\r");
	delay(1000);
	fonaSS.println((char)26);
	fonaSS.println();
	delay(10000);
	digitalWrite(power, LOW);
}

void WheresElaiza(){
	gps_success=0;
	digitalWrite(power, HIGH);
	fonaSerial->begin(4800);
	if (!fona.begin(*fonaSerial)){
		while(1);
	}
	fona.enableGPS(true);
	while(!gps_success){
		delay(2000);
		gps_success = fona.getGPS(&latitude, &longitude, &speed_kph, &speed_mph, &heading, &altitude);
		if (gps_success){
			Serial.print("GPS lat:");
			Serial.println(latitude, 6);
			Serial.print("GPS Long:");
			Serial.println(longitude, 6);

		}
	}
}

void TimerReset(){
	timer = 0;
}






