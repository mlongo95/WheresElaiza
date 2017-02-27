/*
    Traceable Dog Collar Application 
    By Marco Longoria - 02/23/2016
    
   This program is for the Traceable Dog Collar Application that is 
   built with Xadow GSM+BLE, Xadow Audio and Xadow GPS v2. 
   Functions achieved:
   1. Your RePhone (on the dog collar) with automatically pick up phone calls from any cell phone number
   2. A message with the content 'GPS' will trigger your RePhone to send the GPS information back to the sender
*/

//Include the necessary libraries 
#include <LCheckSIM.h>
#include <LGSM.h>
#include <LGPS.h>
#include <stdio.h>
#include <string.h>


char num[20] = {0};
char buf_contex[100];

//Set up
void setup()
{
    Serial.begin(115200);
    Serial.print("\r\n");
    Serial.print("Dog Dog Test\r\n");   
}

//set a loop
void loop()
{
    char buffer[100] = {0,};
    unsigned KeyValue = 0;
    unsigned char *utc_date_time = 0;

    if(LVoiceCall.getVoiceCallStatus() == RECEIVINGCALL)              // if receives an incoming call
    {  
        LVoiceCall.retrieveCallingNumber(num,20);
        sprintf(buffer, "Call come in, number is %s \r\n", num);
        Serial.println(buffer);
        LVoiceCall.answerCall();                                      // pick up the phone call
    }
    
    if(LSMS.available())                                               // if received a SMS from another cellphone
    {
        LSMS.remoteNumber(num, 20); 
        LSMS.remoteContent(buf_contex, 50);
        
        sprintf(buffer, "Get new sms, content: %s, number: %s \r\n", buf_contex, num);
        Serial.println(buffer);
        
        if(strcmp("GPS",(char*)buf_contex) == 0)                       // if the SMS content is 'GPS', then send the GPS information of RePhone back to the sender
        {
            if(LGPS.check_online())
            {
                utc_date_time = LGPS.get_utc_date_time();
                sprintf(buffer, "GPS list:\r\nUTC:%d-%d-%d  %d:%d:%d,\r\nlatitude: %c:%f,\r\nlongitude: %c:%f,\r\naltitude: %f,\r\nspeed: %f,\r\ncourse: %f.", 
                                 utc_date_time[0], utc_date_time[1], utc_date_time[2], utc_date_time[3], utc_date_time[4],utc_date_time[5], 
                                 LGPS.get_ns(), LGPS.get_latitude(), LGPS.get_ew(), LGPS.get_longitude(), LGPS.get_altitude(), LGPS.get_speed(), LGPS.get_course());
                Serial.println(buffer);
                
                if(LSMS.ready())
                {
                    LSMS.beginSMS(num);
                    LSMS.print(buffer);
                    
                    if(LSMS.endSMS()) Serial.println("SMS sent ok!");
                    else Serial.println("SMS send fail!");
                }
                else Serial.println("SMS no ready!");
            }
        }
        LSMS.flush();
    }
    delay(100);   
}
