#include <LiquidCrystal.h>
#include "String.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Ezo_uart.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const uint8_t bufferlen = 32;                         //total buffer size for the response_data array
char response_data[bufferlen];                        //character array to hold the response data from modules
String inputstring = "";                              //a string to hold incoming data from the PC
uint8_t index = 0;

String temperature,ph;
void nodeMCU();
void read_data(Ezo_uart &Module);

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};
// create objects to represent the Modules you're connecting to
Ezo_uart Module1(Serial1, "PH");
Ezo_uart Module2(Serial2, "RTD");

// In this code the Modules[] array determines the number assigned to the circuit
const uint8_t module_count = 2;                       //total size fo the Modules array
Ezo_uart Modules[module_count] = {                    //create an array to hold all the modules
  Module1, Module2
};

StaticJsonBuffer<2000> jsonBuffer;
JsonObject& root =jsonBuffer.createObject();

SoftwareSerial mySerial(6,7);//TX ,RX
void setup() {
  mySerial.begin(9600);
  StaticJsonBuffer<2000> jsonBuffer;
  JsonObject& root =jsonBuffer.createObject ();
  
  lcd.begin(16,2);
  lcd.createChar(0, heart);
  // create a new character
  lcd.createChar(1, smiley);
  // create a new character
  lcd.createChar(2, frownie);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("RIVER NDARUGO");
  lcd.setCursor(0,1);
  lcd.print(" IoT MONITORING");
  Serial.begin(9600);                                 
  Serial1.begin(9600);                                
  Serial2.begin(9600);                                                            
  inputstring.reserve(20);                            
  
  for (uint8_t i = 0; i < module_count; i++) {        
    Modules[i].send_cmd_no_resp("c,0");               
                                                      
    delay(100);
    Modules[i].send_cmd_no_resp("*ok,0");             
                                                      
    delay(100);
    Modules[i].flush_rx_buffer();                     
  }
}

void loop() {
  if (Serial.available() > 0) {                       
    inputstring = Serial.readStringUntil(13);         
    index = parse_input(inputstring);                  
    
    if (inputstring != "") {                          
      Modules[index].send_cmd(inputstring, response_data, bufferlen); 
      Serial.print(index);                             
      Serial.print("-");
      Serial.print(Modules[index].get_name());     
      Serial.print(": ");
      Serial.println(response_data);                                        
      response_data[0] = 0; 
    }
    else {
      Serial.print("Index is set to ");                
      Serial.println(index);
    }
  }

  for (uint8_t i = 0; i < module_count; i++) {        
    print_reading(Modules[i]);
    Serial.print(" ");
  }
  Serial.println();
  
}


//parse the variables to nodeMCU via software serial

void nodeMCU(){
  root["temp"] = temperature;
  root["pH"]   =  ph;
  if(mySerial.available()){
  root.printTo(mySerial);}
  }


void print_reading(Ezo_uart &Module) {                
  
  //takes a reference to a Module
  //send_read() sends the read command to the module then converts the 
  //answer to a float which can be retrieved with get_reading()
  //it returns a bool indicating if the reading was obtained successfully
  if (Module.send_read()) {                           
    /*Serial.print(Module.get_name());                  //prints the module's name
    Serial.print(": ");
    Serial.print(Module.get_reading());*/               //prints the reading we obtained
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print(Modules[0].get_name() + String(" : "));
      lcd.setCursor(6,0);
      lcd.print(Modules[1].get_reading());
      Serial.println(" ");
      temperature = Modules[0].get_reading();
      Serial.print("temperature:");
      Serial.print(temperature);
      Serial.println("");

      ph = Modules[1].get_reading();
      Serial.print("PH:");
      Serial.print(ph);
      Serial.println("");
      nodeMCU();
      
      lcd.setCursor(1,1);
      lcd.print(Modules[1].get_name() + String(": "));
      lcd.setCursor(6,1);
      lcd.print(Modules[0].get_reading());
      ph = Modules[1].get_reading();

      
      
      lcd.setCursor(12,1);
      lcd.write(byte(0));
      lcd.setCursor(13,1);
      lcd.write(byte(0));
      lcd.setCursor(14,1);
      lcd.write(byte(0));
      //serial write data
      if (mySerial.available()) {
      lcd.setCursor(12,0);
      lcd.write(1);
      lcd.setCursor(13,0);
      lcd.write(1);
      lcd.setCursor(14,0);
      lcd.write(1);

  }
  if (!mySerial.available()){
      lcd.setCursor(12,0);
      lcd.write(2);
      lcd.setCursor(13,0);
      lcd.write(2);
      lcd.setCursor(14,0);
      lcd.write(2);

  }
    
  }
}


uint8_t parse_input(String &inputstring) {                 //this function will decode the string (example 4:cal,1413)
  int colon = inputstring.indexOf(':');                  //find the location of the colon in the string
  if ( colon > 0) {                                       //if we found a colon
    String port_as_string = inputstring.substring(0, colon);  //extract the port number from the string and store it here
    inputstring = inputstring.substring(colon + 1);    //extract the message from the string and store it here
    return port_as_string.toInt();                     //convert the port number from a string to an int
  }
  else {                                              //if theres no colon
    return index;                                      //return the current port and dont modify the input string
  }
}
