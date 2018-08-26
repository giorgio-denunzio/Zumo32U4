#include <Wire.h>
#include <Zumo32U4.h>

#include "TurnSensor.h"

Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;
L3G gyro;

void setup() {
  lcd.clear();
  lcd.gotoXY(0,0);
  lcd.print(F("Place robot"));
  lcd.gotoXY(0,1);
  lcd.print(F("Press A"));
  buttonA.waitForButton();

  // Calibrate the gyro. While the LCD
  // is displaying "Gyro cal", you should be careful to hold the robot
  // still.
  turnSensorSetup();
  
// This should be called to set the starting point for measuring
// a turn.  After calling this, turnAngle will be 0.
  turnSensorReset();
}

void loop() {
  // Update the angle value (contained in turnAngle, defined in TurnSensor.cpp)
  turnSensorUpdate();
  lcd.clear();
  lcd.print(turnAngle);
}
