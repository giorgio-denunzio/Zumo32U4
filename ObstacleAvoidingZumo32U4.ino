// by Giorgio De Nunzio
// giorgio.denunzio@unisalento.it

// Licence: 
// "Do with this code what you wish. If you find it useful, please write to me and tell me about it. If you make any improvement, please let me know so we grow together!"

// Useful links:
// http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html#a2789f740828a81fe3c942998639355ae

// NUM_OF_SENSORS = 1 or 3
#define NUM_OF_SENSORS 1

// ROBOT_MOVES = 0 (sensor readings are shown to display) or 1 (the robot also moves according to sensor readings)
#define ROBOT_MOVES 01

// A sensor reading must be greater than or equal to this
// threshold in order for the program to consider that sensor as
// seeing an object.
#define SENSOR_THRESHOLD 20

#define SENSOR_THRESHOLD_1 20
#define SENSOR_THRESHOLD_2 16

// delta between left and right readings, to consider the two readings as different
#define SENSOR_DELTA 1



#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LCD lcd;

const int motorSpeed = 200;

void setup()
{
  #if NUM_OF_SENSORS == 1
    // Initialize the front-sensor mode
    proxSensors.initFrontSensor();
  #else   // NUM_OF_SENSORS == 3
    // Initialize the three-sensor mode
    proxSensors.initThreeSensors();
  #endif

// setBrightnessLevels)
// http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_proximity_sensors.html#a47820baf67dfa58dedb41cb7bb26dc65
// http://pololu.github.io/zumo-32u4-arduino-library/_zumo32_u4_proximity_sensors_8cpp_source.html (setBrightnessLevels)
// Default period is 420:
// http://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_i_r_pulses.html#ae72ab04d5b682b3170a1542344cb3f75
//  uint16_t myBrightnessLevels[] = { 4, 9, 15, 23, 32, 42, 55, 70, 85, 100, 120 };   // default is { 4, 15, 32, 55, 85, 120 }
//  uint16_t myBrightnessLevels[] = {1, 2, 4, 9, 15, 23, 32, 42, 55, 70, 85, 100, 120, 135, 150, 170};   //  OK 16 levels, threshold at 15
  uint16_t myBrightnessLevels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 23, 32, 42, 55, 70, 85, 100, 120, 130, 140, 150, 160, 170, 180, 190};   // 25 levels, used 20 as threshold
//  uint16_t myBrightnessLevels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 19, 23, 27, 32};   // 20 levels, used 16 as threshold
  int numOfBrightnessLevels = sizeof(myBrightnessLevels)/sizeof(myBrightnessLevels[0]);
  proxSensors.setBrightnessLevels(myBrightnessLevels, numOfBrightnessLevels);

  // Wait for the user to press button A.
  lcd.clear();
  lcd.print(F("Press A"));
  buttonA.waitForButton();
  lcd.clear();

  // Delay so that the robot does not move away while the user is
  // still touching it.
  delay(1000);

//  motorTest();
  
}


void loop()
{
  loopFrontSensor(); 
  delay(200);
}


////////////////////////////////////////////
//                                        //
//   void loopFrontSensor()               //
//                                        //
// Read sensors and decide how to move.   //
//                                        //
////////////////////////////////////////////

void loopFrontSensor()
{

  int obs = obstacle();
  
  if (obs) ledYellow(1);
  else ledYellow(0);

#if ROBOT_MOVES == 1
  switch (obs) {
    case -1:    //  The obstacle is on the front left. Turn to the right.
      mRotateRight();
      break;
    case +1:    //  The obstacle is on the front right. Turn to the left.
      mRotateLeft();
      break;
    case 2:     //  The obstacle is in front. Turn 180° and go back.
      mStop();
      delay(500);
      mRotateLeft180();
      break;
    default:    // 0, no obstacle
      mForward();      
      break;
  }  // switch obs
#endif

}

////////////////////////////////////////
//                                    //
//        int obstacle()              //
//                                    //
// Check if an obstacle is present.   //
// Return:                            //
//   0 if no obstacle                 //
//  -1 if obstacle is on the left     //
//  +1 if obstacle is on the right    //
//  +2 if obstacle is in front        //
//                                    //
////////////////////////////////////////

int obstacle()
{

  // Read the front proximity sensor and get its left and right values (the
  // amount of reflectance detected while using the left or right LEDs)
  proxSensors.read();
  
  uint8_t leftSensor = 0, centerLeftSensor, centerRightSensor, rightSensor = 0; 
  
  centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
  centerRightSensor = proxSensors.countsFrontWithRightLeds();
  
#if NUM_OF_SENSORS == 3
  // not used yet
  leftSensor = proxSensors.countsLeftWithLeftLeds();
  rightSensor = proxSensors.countsRightWithRightLeds();
#endif

// LCD stuff from "FaceTowardsOpponent"

  lcd.gotoXY(0, 1);
  lcd.print(leftSensor);
  lcd.print(" ");
  lcd.print(centerLeftSensor);
  lcd.print(" ");
  lcd.print(centerRightSensor);
  lcd.print(" ");
  lcd.print(rightSensor);
  lcd.print(" ");

  int retValue = 0;

#if 0
  if (centerLeftSensor >= SENSOR_THRESHOLD || centerRightSensor >= SENSOR_THRESHOLD)  // An object is visible
    if (centerRightSensor - centerLeftSensor > SENSOR_DELTA)    // The right value is larger, and delta is at least SENSOR_DELTA: the object is probably nearer the right LED
      retValue = +1;
    else if (centerLeftSensor - centerRightSensor > SENSOR_DELTA)  // The left value is larger, and delta is at least SENSOR_DELTA: the object is probably nearer the left LED
      retValue = -1;
    else retValue = +2;    // The two readings are equal: the obstacle is in front
#else // observing that when the obstacle is in front, readings are larger than when it is on the side, so I cannot use just one threshold
  int del = centerRightSensor - centerLeftSensor;
  if (centerLeftSensor >= SENSOR_THRESHOLD_1 || centerRightSensor >= SENSOR_THRESHOLD_1)  // An object must be in front!
    retValue = +2;    // The two readings are equal: the obstacle is in front
  else if (centerRightSensor >= SENSOR_THRESHOLD_2 && del >= SENSOR_DELTA)    // The right value is larger, and delta is at least SENSOR_DELTA: the object is probably nearer the right LED
      retValue = +1;
  else if (centerLeftSensor >= SENSOR_THRESHOLD_2  && -del >= SENSOR_DELTA)  // The left value is larger, and delta is at least SENSOR_DELTA: the object is probably nearer the left LED
      retValue = -1;
#endif

  lcd.gotoXY(0, 1);
  lcd.print(retValue);

  return retValue;
}


////////////////////////////////////////
//                                    //
//        void motorTest()            //
//                                    //
////////////////////////////////////////
void motorTest()
{
  mForward();      
  delay(1000);
  mStop();
  delay(1000);
  mRotateRight90();
  delay(1000);
  mRotateLeft90();
  mRotateLeft90();
  delay(1000);
  mRotateRight90();
  delay(1000);
  mBackward();
  delay(1000);
  mStop();
}

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//        Motor functions                                              //
//                                                                     //
// Rotations by a given angle are obtained with simple temporization;  //
// this naive approach will be replaced in the future by measurements  //
// based on the gyro (see TurnSensor.cpp)                              //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

void mForward()
{
  motors.setLeftSpeed(motorSpeed);
  motors.setRightSpeed(motorSpeed);
}

void mBackward()
{
  motors.setLeftSpeed(-motorSpeed);
  motors.setRightSpeed(-motorSpeed);
}

void mRotateLeft()
{
  motors.setLeftSpeed(-motorSpeed);
  motors.setRightSpeed(motorSpeed);
}

void mRotateRight()
{
  motors.setLeftSpeed(motorSpeed);
  motors.setRightSpeed(-motorSpeed);
}

void mStop()
{
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
}

void mRotateRight90() {
  mRotateRight();
  delay(400);
  mStop();
}

void mRotateLeft90() {
  mRotateLeft();
  delay(380);
  mStop();
}

void mRotateRight45() {
  mRotateRight();
  delay(200);
  mStop();
}

void mRotateLeft45() {
  mRotateLeft();
  delay(190);
  mStop();
}

void mRotateLeft180() {
  mRotateLeft();
  delay(760);
  mStop();
}





