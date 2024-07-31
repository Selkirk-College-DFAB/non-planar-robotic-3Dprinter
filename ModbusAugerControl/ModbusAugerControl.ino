/*
  Modbus-Arduino Example - Test Led (Modbus IP ESP8266)
  Control a Led on GPIO0 pin using Write Single Coil Modbus Function 
  Send two values over Input Register using Ireg()
  Receives two values and display in terminal over Holding Register using Hreg()
  Original library
  Copyright by André Sarmento Barbosa
  http://github.com/andresarmento/modbus-arduino

  Current version
  (c)2017 Alexander Emelianov (a.m.emelianov@gmail.com)
  https://github.com/emelianov/modbus-esp8266
*/

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
#include <Stepper.h>

const int stepsPerRevAuger = 200; 
const int stepsPerRevNozzle = 200;

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int ledPin = 0; //GPIO0

//ModbusIP object
ModbusIP mb;
uint16_t M_ONE_REG = 101; // register for stepper 1
uint16_t M_TWO_REG = 102;

int mOneVal = 0;
int mTwoVal = 0;
  
void setup() {
  Serial.begin(115200);
 
  WiFi.begin("STAC", "STAC2020!");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(5000);
  mb.server();

  pinMode(ledPin, OUTPUT);
  mb.addCoil(LED_COIL);
  mb.addIreg(0);
  mb.addIreg(1);

  mb.addHreg(M_ONE_REG);
  mb.addHreg(M_TWO_REG);
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();

   //Attach ledPin to LED_COIL register
   mOneVal = mb.Hreg(M_ONE_REG);
   mTwoVal = mb.Hreg(M_TWO_REG);
   mb.Ireg(0, 500);
   mb.Ireg(1, 4444);
   digitalWrite(ledPin, mb.Coil(LED_COIL));

   Serial.println(mOneVal);
   Serial.println(mTwoVal);
  
   delay(10);
}