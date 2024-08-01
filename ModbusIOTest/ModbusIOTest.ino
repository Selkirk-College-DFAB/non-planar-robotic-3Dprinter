/*
  Modbus-Arduino Example - Test Led (Modbus IP ESP8266)
  Control a Led on GPIO0 pin using Write Single Coil Modbus Function 
  Send two values based on an incrementing counter, over Input Registers, using Ireg()
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
#include "password.h"

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int ledPin = 0; //GPIO0

//ModbusIP object
ModbusIP mb;
int M_ONE_REG = 101; // register for stepper 1
int M_TWO_REG = 102;
int INPUT_ONE = 0;
int INPUT_TWO = 0;

int mOneVal = 0;
int mTwoVal = 0;
int counter = 0;
  
void setup() {
  Serial.begin(115200);
 
  WiFi.begin(MYSSID, MYPASS);
  
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

  // add input registers (for sending data over)
  mb.addIreg(INPUT_ONE);
  mb.addIreg(INPUT_TWO);

  // add holding registers (for receiving data)
  mb.addHreg(M_ONE_REG);
  mb.addHreg(M_TWO_REG);
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();

   //Attach ledPin to LED_COIL register
   if (mOneVal != mb.Hreg(M_ONE_REG)) {
    mOneVal = mb.Hreg(M_ONE_REG);
    Serial.println(mOneVal);
   }
  
   if (mTwoVal !=  mb.Hreg(M_TWO_REG)) {
    mTwoVal = mb.Hreg(M_TWO_REG);
    Serial.println(mTwoVal);
   }

   // set input registers
   mb.Ireg(INPUT_ONE, counter++);
   mb.Ireg(INPUT_TWO, counter++);

   // set LED based on LED coil boolean value
   digitalWrite(ledPin, mb.Coil(LED_COIL));

   delay(10);
}