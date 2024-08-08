/*
  Modbus Extruder Control

  Uses ESP8266 to connect to WiFi
  Read max speed value from a potentiameter connected to A0 and set the speed
  Read amount of movement from EXTRUDER_REG using modbus over WiFi
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
const int LED_PIN = 2; // GPIO2 = D4
const int STEP_PIN = 5; // GPIO 5 = D1
const int DIR_PIN = 4;  // GPIO 4 = D2
const int MAX_SPEED_UPPER = 1000; // upper bound for max speed
const int MAX_SPEED_LOWER = 500; // lower bound for max speed
const int STEP_FACTOR = 3;

unsigned long previousMillis = 0;
const long interval = 400; // Interval to read analog value in milliseconds

//ModbusIP object
ModbusIP mb;
const int EXTRUDER_REG = 101; // register for extruder stepper

int extruderAccel = 500;
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

  extruder.setMaxSpeed(extruderSpeed);
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
    extruder.moveTo(extruderGoTo * STEP_FACTOR);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    // read value of pot and set the speed if it has changed
    int newSpeed = map(analogRead(A0), 0, 1023, MAX_SPEED_LOWER, MAX_SPEED_UPPER);
    if (abs(newSpeed - extruderSpeed) > 50) {
      extruderSpeed = newSpeed;
      extruder.setMaxSpeed(extruderSpeed);
      extruder.setAcceleration(extruderSpeed / 2); // TODO: whats the relationship between acceleration and speed?

      Serial.print("New speed set to: ");
      Serial.println(extruderSpeed);
    }
  }

  extruder.run();

  digitalWrite(LED_PIN, mb.Coil(LED_COIL));
}