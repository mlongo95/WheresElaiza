/*  SIM808 Software Serial test
	
	By Marco Longoria
*/

#include <SoftwareSerial.h>

SoftwareSerial ss(2,3);

char inChar = 0;

void setup(){
	Serial.begin(115200);
  	Serial.println("Device ready");
  	// set the data rate for the SoftwareSerial port
  	mySerial.begin(4800);
}

void loop() // run over and over
{
  	if (mySerial.available()){
    	inChar = mySerial.read();
    	Serial.write(inChar);
    	delay(20);
    }
    
  	if (Serial.available()>0){
    mySerial.write(Serial.read());
 
  	}
}