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
#include "password.h"
#include <ModbusIP_ESP8266.h>
#include <AccelStepper.h>

const int stepsPerRevAuger = 800; 
const int stepsPerRevNozzle = 400;

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int ledPin = 2; //GPIO2

//ModbusIP object
ModbusIP mb;
int AUGER_REG = 101; // register for Auger stepper
int NOZZLE_REG = 102; // register for Nozzle stepper
int augerVal = 0;
int augerSpeed = 0;
int nozzleVal = 0;
int nozzlePos = 0;

int pos = 1000;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 0, 4);

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

  mb.server();

  pinMode(ledPin, OUTPUT);
  mb.addCoil(LED_COIL);

  mb.addHreg(AUGER_REG);
  mb.addHreg(NOZZLE_REG);
}
 
void loop() {
  //Call once inside loop()
  mb.task();

  if (augerVal != mb.Hreg(AUGER_REG)) {
    augerVal = mb.Hreg(AUGER_REG);
    augerSpeed = map(augerVal, 0, 5000, 0, 100);
    Serial.println("set Auger speed to " + augerSpeed);
  }
  if (nozzlePos != mb.Hreg(NOZZLE_REG)) {
    nozzlePos = mb.Hreg(NOZZLE_REG);
    Serial.println("set nozzle orientation to " + nozzlePos);
  }

  // move auger
  if (augerSpeed > 0) {
    stepper.setMaxSpeed(augerSpeed);

  }
  
  if (stepper.distanceToGo() == 0)
  {
    delay(500);
    pos = -pos;
    stepper.moveTo(pos);
  }

  stepper.run();
  digitalWrite(ledPin, mb.Coil(LED_COIL));

  delay(10);
}