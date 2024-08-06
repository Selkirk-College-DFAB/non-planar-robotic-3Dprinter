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

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int LED_PIN = 2; // GPIO2 = D4
const int STEP_PIN = 5; // GPIO 5 = D1
const int DIR_PIN = 4;  // GPIO 4 = D2

//ModbusIP object
ModbusIP mb;
const int EXTRUDER_REG = 101; // register for extruder stepper

int extruderAccel = 1000;
int maxSpeed = 1000;
int extruderGoTo = 0;

// Define a stepper motor and the pins it will use
AccelStepper extruder(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

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

  pinMode(LED_PIN, OUTPUT);
  mb.addCoil(LED_COIL);

  mb.addHreg(EXTRUDER_REG);

  extruder.setMaxSpeed(maxSpeed);
  extruder.setAcceleration(extruderAccel);
  extruder.setCurrentPosition(0);
  extruder.moveTo(10000);
}
 
void loop() {
  //Call once inside loop()
  mb.task();

  if (extruderGoTo != mb.Hreg(EXTRUDER_REG)) {
    extruderGoTo = mb.Hreg(EXTRUDER_REG);
    //augerSpeed = map(augerVal, 0, 100, 0, 100);
    Serial.println(extruderGoTo);
    extruder.moveTo(extruderGoTo);
  }
  extruder.run();

  digitalWrite(LED_PIN, mb.Coil(LED_COIL));
}