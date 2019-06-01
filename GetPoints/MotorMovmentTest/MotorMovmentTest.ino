  #include <Arduino.h>
  #include "BasicStepperDriver.h"
  #include "MultiDriver.h"
  #include <Servo.h>
  #include <SD.h>
  
  File posFile;
  float X;
  float Y;
  int rocker = 0;
  bool drawing = true;
  
  #define MOTOR_STEPS 200
  // Target RPM for motors
  #define MOTOR_RPM 60 //starting rpm
  #define MOTOR_ACCEL 200
  #define MOTOR_DECEL 100
  #define DIR_1 4 // Pins for motor 1
  #define STEP_1 5 //
  #define HOMING1 7 // Left endstop
  #define HOMING2 8 // Right endstop
  #define DIR_2 2 // Pins for motor 2
  #define STEP_2 3
  
  #define MICROSTEPS 16 // Microstepping 
  /*
  200  full steps * 16 = 3200 steps per rotation + 1:4 gear ratio
   2.51307087 in per rotation
   12,800 steps per rotation
  */
  BasicStepperDriver stepper1(MOTOR_STEPS, DIR_1, STEP_1);
  BasicStepperDriver stepper2(MOTOR_STEPS, DIR_2, STEP_2);

 Servo servo1;

 int servoPos = 100;
 
  MultiDriver controller(stepper1, stepper2);

  /*Variables Set up:

  
                   AB
       A(aX,Ay)---------------B(bX,bY)
        \                    /
         \                  /
          \                /
           \              /   
     AC     \            /   BC
             \          /
              \        /
               \      /
                \    /
                 \  /
                  C(cX,cY)  ----> (targetCx,targetCy)  25,22

              Set to mm
                  
  */
  
  const double AB = 1270; //mm
  double scale = 1;
  float AC;
  float BC;
  float newAC;
  float newBC;
  float targetCx;
  float targetCy;
  double cX;
  double cY; // manually measured for now
  int aX = 0;
  int aY = 0;
  int ACMax = 1400;
  int BCMax = 1400;
  double bX = AB + aX;
  int bY = 0;
  float changeAC; // The amount the motor needs to move inorder to get to the target points
  float changeBC;
  int moveACsteps;
  int moveBCsteps;
  bool homed1 = false;
  bool homed2 = false;
  bool homedFinal = false;
  bool triggered1 = false;
  bool triggered2 = false;
 
  float testX[] = {};
  float testY[] = {};
  
  
void setup() {

  pinMode(HOMING1, INPUT);
  pinMode(HOMING2, INPUT);
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
  

  
  stepper1.begin(MOTOR_RPM, MICROSTEPS);
  stepper2.begin(MOTOR_RPM, MICROSTEPS);
  servo1.attach(6);
  
  //stepper1.setSpeedProfile(stepper1.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
 // stepper2.setSpeedProfile(stepper2.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);
   
 // AC = sqrt(sq((aX-cX)) + sq((aY-cY))); //1211.012
 // BC = sqrt(sq((bX-cX)) + sq((bY-cY)));
// AC = 31;
//BC = 32.5;

  Cy = (sq(AB)+sq(ACMax) - sq(BCMax))/(2 *AB);
  Cx = sqrt(sq(ACMax - sq(Cy));
}

void loop() {
  
  unsigned stepper1Action = stepper1.nextAction();
  
  if(homed == false){
      stepper1.startMove(round(4/0.0049087421875));
      homed = true;
    }
  if (digitalRead(HOMING1) == HIGH){
      triggered1 = true;
  }
  if (stepper1Action <= 0){
    if (triggered1){
      homed = false;
    }
    else{
      homed = true;
      AC = ACMax;
    }
    
  }
   

   File posFile = SD.open ("test.txt", FILE_READ);
     if(homedFinal){   
      if(posFile){
        
          char fileContents[9]; // Probably can be smaller
       byte index = 0;
       while (posFile.available()) 
      {
          char aChar = posFile.read();
          if(aChar != '\n' && aChar != '\r')
          {
             fileContents[index++] = aChar;
             fileContents[index] = '\0'; // NULL terminate the array
          }
          else // the character is CR or LF
          {
            // Serial.print("fileContents: [");
           //  Serial.print(fileContents);
           //  Serial.println("]");
             if(strlen(fileContents) > 0)
             {
              if (rocker == 0){
                X = atof(fileContents);
                rocker = 1;
              // Serial.print("X: ");
              //   Serial.println(X);
              }
              else {
                Y = atof(fileContents);
                rocker = 0;
              //  Serial.print("Y: ");
             //    Serial.println(Y);
              }
                //float aVal = atof(fileContents);
    
                if (rocker == 0 ){
                  targetCx = X;
                  targetCy = Y;
                   newAC = sqrt(sq((aX-targetCx)) + sq((aY-targetCy)));
                   newBC = sqrt(sq((bX-targetCx)) + sq((bY-targetCy)));
    
                   changeAC = newAC - AC;// 
                   changeBC = BC - newBC;
                   Serial.println(changeAC);
                   if (sqrt(sq(changeAC)) >= 5 || sqrt(sq(changeBC)) >= 5){
                    if (drawing == true){
                      Serial.println("LOL");
                     for (servoPos = 140; servoPos >= 110; servoPos -= 1) { // goes from 180 degrees to 0 degrees
                     servo1.write(servoPos);              // tell servo to go to position in variable 'pos'
                     delay(15);                       // waits 15ms for the servo to reach the position
                      }
                    }
                     drawing = false;                 
                   }
                  else{
                     if (drawing == false){
                      for (servoPos = 110; servoPos <= 140; servoPos += 1) { // goes from 0 degrees to 180 degrees
                     // in steps of 1 degree
                     servo1.write(servoPos);              // tell servo to go to position in variable 'pos'
                      delay(15);                       // waits 15ms for the servo to reach the position
                            }
                     
                     
                     }
                    drawing = true;  
                  }
    
                   moveACsteps = round(changeAC/0.0049087421875)*scale;
                   moveBCsteps = round(changeBC/0.0049087421875)*scale;
                  //Serial.println(moveBCsteps);
                  
                  controller.move(moveACsteps,moveBCsteps);
                  
                  AC = newAC;
                  BC = newBC;
                }
             }
    
             fileContents[0] = '\0';
             index = 0;
          }
       }
  
     }
  
  }
}













