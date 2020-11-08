#include<SoftwareSerial.h>
#include <ArduinoJson.h>
#include "ThingSpeak.h"
#include "secrets.h"
#include <ESP8266WiFi.h>

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;
SoftwareSerial mySerial(D5,D6);

void setup(){
  Serial.begin(9600);
  mySerial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network 
  delay(100);
    while(WiFi.status() != WL_CONNECTED){
      delay(500); 
      Serial.print(".");    
    } 
    Serial.println("\nConnected.");
    ThingSpeak.begin(client);
  }

void loop(){
StaticJsonBuffer<2000> jsonBuffer;
JsonObject& root = jsonBuffer.parseObject(mySerial);

String Temp = root["temp"];
String PH   = root["pH"];

Serial.println(Temp);
Serial.println(PH);


 //set the fields with the values
  ThingSpeak.setField(1,  Temp);
  ThingSpeak.setField(2,  PH);
  
   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
   delay(5000);
  if(x == 200){
    Serial.println("update successful.");
  }
  else{
    Serial.println("update not successful.");
  }
  delay(200);
}
