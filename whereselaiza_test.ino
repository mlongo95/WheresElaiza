
/*
  THIS CODE IS STILL IN PROGRESS!

  Open up the serial console on the at 115200 baud to interact with SIM808

  This code will receive an SMS, identify the sender's phone number, and automatically send a  GPRSloc response

  For use with SIM808

      Written by Marco Longoria , 2018
*/

#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

// this is a large buffer for replies
char replybuffer[255];
char gpsbuffer[20];

// We default to using software serial. If you want to use hardware serial
// (because softserial isnt supported) comment out the following three lines
// and uncomment the HardwareSerial line
#include <SoftwareSerial.h>
//SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
//SoftwareSerial *fonaSerial = &fonaSS;

// Hardware serial is also possible!
  HardwareSerial *fonaSerial = &Serial1;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);
// Use this one for FONA 3G
//Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {
  while (!Serial);

  Serial.begin(115200);
  Serial.println(F("FONA SMS caller ID test"));
  Serial.println(F("Initializing....(May take 3 seconds)"));

  // make it slow so its easy to read!
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is OK"));

  // Print SIM card IMEI number.
  char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
  }

  fonaSerial->print("AT+CNMI=2,1\r\n");  //set up the FONA to send a +CMTI notification when an SMS is received
  // enable GPRS
  fona.enableGPRS(false);
  Serial.println(F("Disabling GPRS"));
  delay(5000);
  if (!fona.enableGPRS(true)) {
    Serial.println(F("Failed to enable GPRS"));
  }
  Serial.println("FONA Ready");
}


char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];

void loop() {

  char* bufPtr = fonaNotificationBuffer;    //handy buffer pointer

  if (fona.available())      //any data available from the FONA?
  {
    int slot = 0;            //this will be the slot number of the SMS
    int charCount = 0;
    //Read the notification into fonaInBuffer
    do  {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

    //Add a terminal NULL to the notification string
    *bufPtr = 0;

    //Scan the notification string for an SMS received notification.
    //  If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
      Serial.print("slot: "); Serial.println(slot);

      char callerIDbuffer[32];  //we'll store the SMS sender number in here
      char location[32];
      uint16_t returncode;
      float lat, lon;

      fona.getGSMLoc(&returncode, gpsbuffer, 120); //Pulls GPRS Location 
      
      char *lonp = strtok(gpsbuffer, ","); //Begin to parse GPRS location to strings
      char *latp = strtok(NULL, ",");
      lat = atof(latp);                   //convert from string to floats
      lon = atof(lonp);
      
      char latresult[20] = "";            //char lat+lon results pointer
      char lonresult[20] = "";

  
      dtostrf(lat, 6, 6, latresult);      //converting lat+lon to latresult+lonresult string
      dtostrf(lon, 6, 6, lonresult);  

      String stringgmap, stringlat, stringlon;
      
      stringgmap = ("https://www.google.com/maps/place/"); //adding strings together 
      stringlat += stringgmap; 
      stringlat += String(latresult);
      stringlon = String(lonresult);
      stringlat += ",";
      stringlat +=stringlon;                              // strings added to finalchar[]

      char finalchar[60];
      stringlat.toCharArray(finalchar, 60);
      
      //stringlat += (char)finalchar;
      
      


//      char finalresult[35] = ""; // you have to be aware of how long your data can be
//                      // not forgetting unprintable and null term chars
//      sprintf(finalresult,"%d,%d",latresult,lonresult);
//      
            
      Serial.print("GPSLOC TEST:");       // GPRS DEBUG INIT
      Serial.println();
      Serial.print(lat);
      Serial.print(",");
      Serial.print(lon);
      Serial.println();
      Serial.println("STRINGLAT");
      Serial.println(stringlat);
      Serial.println();
      Serial.println("SMS Message:");
      Serial.println(finalchar);      // GPRS DEBUG END
      
      // Retrieve SMS sender address/phone number.
      if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
        Serial.println("Didn't find SMS message in slot!");
      }
      Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;
      if (fona.readSMS(slot, smsBuffer, 250, &smslen)) { // pass in buffer and max len!
        Serial.println(smsBuffer);
      }

      //Send back an automatic response
      Serial.println("Sending reponse...");
      if (!fona.sendSMS(callerIDbuffer, finalchar )) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }

      // delete the original msg after it is processed
      //   otherwise, we will fill up all the slots
      //   and then we won't be able to receive SMS anymore
      if (fona.deleteSMS(slot)) {
        Serial.println(F("Deleted SMS in slot!"));
      } else {
        Serial.print(F("Couldn't delete SMS in slot ")); Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
    }
  }
}
