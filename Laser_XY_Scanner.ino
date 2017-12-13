/*
  Laser XY Scanner
  Low Cost DIY stepper motor Laser XY Scanner, Cutter, or Engraver

  Laser XY Scanner is an open source stepper motor based scanner. 
  The components used are low cost and very accessible. This type 
  of laser scanner is not extremely fast but it can still be used 
  in a variety of applications. Draw, cut and or point with a 
  laser with X and Y control.

  The AccelStepper library is required for this sketch to run.
  Download and documentation here: http://aka.ms/AccelStepper
  AccelStepper Author: Mike McCauley (mikem@airspayce.com)

  GitHub Project: https://github.com/ThingEngineer/Laser-XY-Scanner
  Thingiverse files for CHICK: https://www.thingiverse.com/thing:2710048

  Functions:
  - Draw, cut and or point with a laser with X and Y control.

 The circuit:
  * Arduino Nano
    * Controls ULN2003APG high−current darlington drivers
      * Drives 28BYJ-48 5V Stepper Motors
  * 12V TTL 1W/1.6W/2W/445nm/450nm Laser Diode LD Driver
  * Drives 2W 445nm M140 Blue Laser Diode w/ AixiZ aluminum mount and heat sink for 12mm modules

 Created 3 April 2017
 Modified 12 December 2017
 Author Josh Campbell

 v1.0 - 12/12/2017

 This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
 */
#include <AccelStepper.h>
#define HALFSTEP 8
#define FULLSTEP 4

// AccelStepper pins
#define s1_in1 2
#define s1_in2 3
#define s1_in3 4
#define s1_in4 5

#define s2_in5 6
#define s2_in6 7
#define s2_in7 8
#define s2_in8 9

const int xMin = 10;        // define Min/home inputs
const int yMin = 11;
byte hMinVal;               // temporary Min input variable

//Keeps track of the current direction relative to the face of the motor. 
//Clockwise (true) or Counterclockwise(false)
bool xClockwise = true;     // default to clockwise
bool yClockwise = true;     // default to clockwise

int TargetArr[] = {0, 1, 0, 1}; // 0 = x, 1 = y
int TargetPositionArr[] = {15, 20, -15, -20};

int moveCount = 0;          // loop used for drawing

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepperX(HALFSTEP, s1_in1, s1_in3, s1_in2, s1_in4);
AccelStepper stepperY(HALFSTEP, s2_in5, s2_in7, s2_in6, s2_in8);

void setup() {

  //Serial.begin(9600);

  // Setup home switch inputs (minimum travel)
  pinMode(xMin, INPUT);           // set pin to input
  pinMode(yMin, INPUT);
  digitalWrite(xMin, HIGH);       // turn on pullup resistors
  digitalWrite(yMin, HIGH);

  //Serial.println("Homing X");
  stepperX.setMaxSpeed(1000);     // slow speed for calibrating
  stepperX.moveTo(4096);          // try to move to max
  stepperX.setAcceleration(1000);
  xStepperHome();                 // runs routine to home X motor
  //Serial.println("Homing Y");
  stepperY.setMaxSpeed(1000);     // slow speed for calibrating
  stepperY.moveTo(4096);          // try to move to max
  stepperY.setAcceleration(1000);
  yStepperHome();                 // runs routine to home Y motor
  //Serial.println("Calibration complete");
  
  //Set the initial speed (read the AccelStepper docs on what "speed" means)
  stepperX.setSpeed(1000.0);
  stepperX.setMaxSpeed(1000.0);
  stepperX.setAcceleration(20000.0);
  stepperY.setSpeed(1000.0);
  stepperY.setMaxSpeed(1000.0);
  stepperY.setAcceleration(20000.0);
}

void loop() {

  // Uncomment only one drawing function at a time, then experiment to make your own!

  drawSquare();     // Draw a square, uses values in TargetArr and TargetPositionArr
  
  //xyScan();       // Example XY Scan (change either x or y speed to around 30, one slow one fast)

  //If the stepper still needs to move (distanceToGo() != 0) then continue to advance (step) the motor
  stepperX.run();
  stepperY.run();
}

void drawSquare(){
  // Draw a square  
  if(stepperX.distanceToGo() == 0 and stepperY.distanceToGo() == 0){
    if(moveCount == 3){
      moveCount = 0;
    } else {
      moveCount = moveCount + 1;
    }

    if (TargetArr[moveCount] == 0) {
      stepperX.moveTo(TargetPositionArr[moveCount]);
    }
    else {
      stepperY.moveTo(TargetPositionArr[moveCount]);
    }
  }
}

void xyScan(){
  // Slow X scan
  // Check to see if the stepper has reached the target:
  if(stepperX.distanceToGo() == 0){
    if(xClockwise == true){
      xClockwise = false;  // move counterclockwise
      stepperX.moveTo(-20); // go back to the "home" (original) position
    } else {
      xClockwise = true;   // move clockwise
      stepperX.moveTo(100);  // go to the target position
    }
  }

  // Fast Y scan
  // Check to see if the stepper has reached the target:
  if(stepperY.distanceToGo() == 0){
    if(yClockwise == true){
      yClockwise = false;  // move counterclockwise
      stepperY.moveTo(0);  // go back to the "home" (original) position
    } else {
      yClockwise = true;   // move clockwise
      stepperY.moveTo(100);  // go to the target position
    }
  }
}

void xStepperHome(){ //this routine should run the motor
  hMinVal = digitalRead(xMin);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperX.moveTo(4096);
    stepperX.run();
    hMinVal = digitalRead(xMin);
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
  stepperX.moveTo(-580);
  while (stepperX.distanceToGo() != 0){
    stepperX.run();
  }
  stepperX.setCurrentPosition(0); //set current motor position to zero
}

void yStepperHome(){ //this routine should run the motor
  hMinVal = digitalRead(yMin);
  while (hMinVal == LOW)
  {
    //backwards slowly till it hits the switch and stops
    stepperY.moveTo(4096);
    stepperY.run();
    hMinVal = digitalRead(yMin);
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
  stepperY.moveTo(-900);
  while (stepperY.distanceToGo() != 0){
    stepperY.run();
  }
  stepperY.setCurrentPosition(0); //set current motor position to zero
}

