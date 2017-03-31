/*  SIM808 Software Serial test
	
	By Marco Longoria
*/

#include <SoftwareSerial.h>

#define SIM808_RX 2
#define SIM808_TX 3

SoftwareSerial sim(SIM808_RX,SIM808_TX);

char inChar = 0;

void setup(){
	Serial.begin(115200);
  	Serial.println("Device ready");
  	sim.begin(4800); // set the data rate for the SoftwareSerial port
}

void loop() {

  	if (sim.available()){
    	inChar = sim.read();
    	Serial.write(inChar);
    	delay(20);
    }
    
  	if (Serial.available()>0){
    sim.write(Serial.read());
  	}
}