/*
  Modbus Extruder Control

  Uses ESP32 to connect to WiFi
  Read amount of movement from EXTRUDER_REG and EXTRUDER_SPEED_REG using modbus over WiFi
  Use AccelStepper library to move stepper with max speed to new desired location
*/

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include "password.h" // Local WiFi credentials are stored in separate file not in the git repo
#include <ModbusIP_ESP8266.h>
#include <AccelStepper.h>

//Modbus Registers Offsets
const int LED_COIL = 100;
//Used Pins
const int LED_PIN = 2;
const int STEP_PIN = 16; 
const int DIR_PIN = 17;
const int MAX_SPEED_UPPER = 5000; // upper bound for max speed
const int MAX_SPEED_LOWER = 500; // lower bound for max speed
const int STEP_FACTOR = 3000;

//ModbusIP object
ModbusIP mb;
const int EXTRUDER_REG = 110; // register for extruder stepper
const int EXTRUDER_SPEED_REG = 111;
int extruderAccel = 1000;
int extruderSpeed = 1000;
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
  mb.addHreg(EXTRUDER_SPEED_REG);
  extruder.setPinsInverted(true, false, false);
  extruder.setMaxSpeed(extruderSpeed);
  extruder.setAcceleration(extruderAccel);
  extruder.setCurrentPosition(0);
}
 
void loop() {
  //Call once inside loop()
  mb.task();

  if (extruderSpeed != mb.Hreg(EXTRUDER_SPEED_REG)){
    Serial.print("Setting extruder speed: ");
    extruderSpeed = mb.Hreg(EXTRUDER_SPEED_REG);
    extruder.setMaxSpeed(extruderSpeed);
    Serial.println(extruderSpeed);
  }

  // if register is set to 0 keep running the motor at current speed. Else use positioning system
  if (mb.Hreg(EXTRUDER_REG) == 0) {
    extruder.setSpeed(extruderSpeed);
    extruder.runSpeed();
  } else if (extruderGoTo != mb.Hreg(EXTRUDER_REG)) {
    Serial.print("Moving extruder: ");
    extruderGoTo = mb.Hreg(EXTRUDER_REG);
    extruder.moveTo(extruderGoTo * STEP_FACTOR);
    Serial.println(extruderGoTo);
    extruder.run();
  }

  digitalWrite(LED_PIN, mb.Coil(LED_COIL));
}