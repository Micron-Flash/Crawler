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
  #define DIR_1 2 // Pins for motor 1
  #define STEP_1 3 //
  #define HOMING1 7 // Left endstop
  #define HOMING2 8 // Right endstop
  #define DIR_2 4 // Pins for motor 2
  #define STEP_2 5
  
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

  cY = (sq(AB)+sq(ACMax) - sq(BCMax))/(2 *AB);
  cX = sqrt(sq(ACMax - sq(cY)));
}

void loop() {
  if (homedFinal == false){  
     if ((triggered1 == true) && (stepper1.nextAction() <= 0)){
      stepper1.startMove (round(4/0.0049087421875));
       triggered1 = false;
     }   
   if (digitalRead(HOMING1) == HIGH){
     triggered1 = true;  
   }
   if (stepper1.nextAction() <= 0){
      if(triggered1 == false){
        AC = ACMax;
        homedFinal = true;
      }
    }
  }
  
   if(homedFinal){   
       File posFile = SD.open ("test.txt", FILE_READ); // !!!Change the file name on this to what ever you will put on the SD card!!!!
     if(posFile){
       char fileContents[9]; // Probably can be smaller
       byte index = 0;
       while (posFile.available()){
         char aChar = posFile.read();
         if(aChar != '\n' && aChar != '\r'){
           fileContents[index++] = aChar;
           fileContents[index] = '\0';
         }
         else{
           if(strlen(fileContents) > 0){
             if (rocker == 0){ // we will use the rocker to set the first 5 or 6 numbers on the fille to set machine settings
               X = atof(fileContents);
               rocker = 1;
             }
             else {
               Y = atof(fileContents);
               rocker = 0;
             }
             if (rocker == 0 ){
                targetCx = X;
                targetCy = Y;
                newAC = sqrt(sq((aX-targetCx)) + sq((aY-targetCy))); // we find the new length of the chain
                newBC = sqrt(sq((bX-targetCx)) + sq((bY-targetCy)));
                changeAC = newAC - AC; // we figure out the difference between the actual and the target length
                changeBC = BC - newBC; 
                if (sqrt(sq(changeAC)) >= 5 || sqrt(sq(changeBC)) >= 5){ // We check for a large movement. This usually idicates when the pen should be lifted. (Seems to be working well)
                  if (drawing == true){
                    for (servoPos = 140; servoPos >= 110; servoPos -= 1) {
                      servo1.write(servoPos);             
                      delay(15);                      
                    }
                  }
                   drawing = false;                 
                  }
                  else{
                    if (drawing == false){
                      for (servoPos = 110; servoPos <= 140; servoPos += 1) { 
                        servo1.write(servoPos);              
                        delay(15);                       
                      }
                    }
                    drawing = true;  
                  }
                  moveACsteps = round(changeAC/0.0049087421875)*scale; // we move the amount of stepps it will take to move the chain a certain amount of mm. The long number is the conversion of steps to mm (example: to move 1 mm it would take 203.718 steps)
                  moveBCsteps = round(changeBC/0.0049087421875)*scale;
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













