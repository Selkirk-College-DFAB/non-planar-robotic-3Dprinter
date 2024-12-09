/*
  Modbus-Arduino Example - Test Led (Modbus IP ESP8266)
  Control a Led on GPIO0 pin using Write Single Coil Modbus Function 
  Send two values over Input Register using Ireg()
  Receives two values and display in terminal over Holding Register using Hreg()
  Original lib          rary
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

const int AUGER_STEP_PIN = 16;
const int AUGER_DIR_PIN = 17;
const int NOZZLE_STEP_PIN = 27;
const int NOZZLE_DIR_PIN = 14;

const int AUGER_REG = 101; // register for Auger Stepper Position
const int NOZZLE_REG = 102; // register for Nozzle Stepper Position
const int AUGER_SPEED_REG = 103; // register for Auger Stepper Max Speed
const int NOZZLE_SPEED_REG = 104; // Nozzle speed

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int ledPin = 2; //GPIO2

//ModbusIP object
ModbusIP mb;

int augerPos = 0;
int augerSpeed = 5000;
int nozzlePos = 0;
int nozzleSpeed = 5000;

// Define Nozzle Stepper and the pins it will use
AccelStepper nozzleStepper(AccelStepper::DRIVER, AUGER_STEP_PIN, AUGER_DIR_PIN);

// Define Auger Stepper and the pins it will use
AccelStepper augerStepper(AccelStepper::DRIVER, NOZZLE_STEP_PIN, NOZZLE_DIR_PIN);

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
  mb.addHreg(AUGER_SPEED_REG);
  mb.addHreg(NOZZLE_SPEED_REG);

  Serial.println("Testing Auger Stepper");
  augerStepper.setMaxSpeed(5000);
  augerStepper.setAcceleration(1000);
  augerStepper.setCurrentPosition(0);
  augerStepper.moveTo(50000);

  Serial.println("Testing Nozzle Stepper");
  nozzleStepper.setMaxSpeed(2500);
  nozzleStepper.setAcceleration(1000);
  nozzleStepper.setCurrentPosition(0);
  nozzleStepper.moveTo(50000);

  augerStepper.run();
  nozzleStepper.run();
}
 
void loop() {
  //Call once inside loop()
  mb.task();

  // set auger speed
  if (augerSpeed != mb.Hreg(AUGER_SPEED_REG)) {
    augerSpeed = mb.Hreg(AUGER_SPEED_REG);
    Serial.print("Setting auger speed: ");
    augerStepper.setMaxSpeed(augerSpeed);
    Serial.println(augerSpeed);
  }

  //set nozzle speed
  if (nozzleSpeed != mb.Hreg(NOZZLE_SPEED_REG)) {
    nozzleSpeed = mb.Hreg(NOZZLE_SPEED_REG);
    Serial.print("Setting nozzle speed");
    nozzleStepper.setMaxSpeed(nozzleSpeed);
    Serial.println(nozzleSpeed);
  }

  // when auger register is set to 0 run the auger continously at current speed. Else follow positioning mode
  if (mb.Hreg(AUGER_REG) == 0) {
    augerStepper.setSpeed(-1 * augerSpeed);
    augerStepper.runSpeed();
  } else if (augerPos != mb.Hreg(AUGER_REG)) {
    augerPos = mb.Hreg(AUGER_REG);
    Serial.print("Moving auger: ");
    augerStepper.moveTo(augerPos);
    Serial.println(augerPos);
    augerStepper.run();
  }

  // when nozzle register is set to 0 run the nozzle continously at current speed, otherwise go to position
  if (mb.Hreg(NOZZLE_REG) == 0) {
    nozzleStepper.setSpeed(nozzleSpeed);
    nozzleStepper.runSpeed();
  } else if (nozzlePos != mb.Hreg(NOZZLE_REG)) {
    nozzlePos = mb.Hreg(NOZZLE_REG);
    Serial.print("Moving nozzle pos: ");
    nozzleStepper.moveTo(nozzlePos);
    Serial.println(nozzlePos);
    nozzleStepper.run();
  }

  digitalWrite(ledPin, mb.Coil(LED_COIL));
}