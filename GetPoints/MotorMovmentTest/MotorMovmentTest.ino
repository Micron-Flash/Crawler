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
  #define MOTOR_STEPS 200// Target RPM for motors
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
   63.832000098 mm per rotation
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
  
  const double AB = 990; //mm
  double scale = 1;
  float AC = 1524;
  float BC = 1524;
  float newAC;
  float newBC;
  float targetCx;
  float targetCy;
  double cX;
  double cY; // manually measured for now
  int aX = 0;
  int aY = 0;
  int ACMax = 1524;
  int BCMax = 1524;
  double bX = AB + aX;
  int bY = 0;
  float changeAC; // The amount the motor needs to move inorder to get to the target points
  float changeBC;
  float moveACsteps;
  float moveBCsteps;
  bool homed1 = false;
  bool homed2 = false;
  bool homedFinal = true;
  bool homedFinal2 = true;
  long timer3A[4];
  long timer3B[4];
  long timer2A;
  long timer2B;
  long timer1A;
  long timer1B;
  long times;
  bool timer1Running = true;
  bool timer2Running = true;
  
  
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
  stepper1.enable();
  servo1.attach(6);

  //cY = (sq(AB)+sq(ACMax) - sq(BCMax))/(2 *AB);
 // cX = sqrt(sq(ACMax - sq(cY)));
 
  //stepper1.startMove(round(ACMax/0.0049087421875));
 //  stepper2.startMove(round(-BCMax/0.0049087421875));// start the motor movement to home
  bool homed2 = false;
  //controller.startMove(round(ACMax/0.0049087421875),0);
}


void loop() {
    
   if (homed1 == false){  //Motor 1     
    stepper1.startMove(round(ACMax/0.0049087421875));// Here we are calibrating how long it take for the ball chain to trigger the endstops. We get 4 samples 
   for (int i = 0; i <= 4;){
    
    stepper1.nextAction();
     if(timer1Running == true && digitalRead(HOMING1) == LOW){
       timer1A = millis();
       timer1Running = false;  
     }
     if(timer1Running == false && digitalRead(HOMING1) == HIGH){
       timer2A = millis();
       timer1Running = true;
       timer3A[i] = timer2A - timer1A;
    //   Serial.println(timer3A[i]); 
       homed1 = true;
       i++;
     }
   }
   timer3A[1] = ((timer3A[1] + timer3A[2] + timer3A[3]+ timer3A[4])/4)*1.50;  // we dont want to use to first number since we don't know where the chain starts. Could give a wrong number. We give it a 50% margine of error. Probably could be less but this should give a constistent result
 
  } 


   
if (homed1 == true){
 stepper1.nextAction(); 

 if(timer1Running == true && digitalRead(HOMING1) == LOW){ // we start timmer when there is nothing in front of the sensor
    timer1A = millis();
    timer1Running = false;
   }
  if(timer1Running == false && digitalRead(HOMING1) == HIGH){ // resets timer here
    timer2A = millis();
    timer1Running = true;
  }
  if(timer1Running == false && digitalRead(HOMING1) == LOW){ // here we check to see if there is too much time passing
    timer2A = millis();
    if (timer2A - timer1A > timer3A[1]){
   //   Serial.println("Homed"); 
    stepper1.stop();
    AC = ACMax;
    homedFinal = true;
    }
  }
}
  
 if (homedFinal == true && homed2 == false){ // calibrating the 2nd motor
     stepper2.startMove(round(-BCMax/0.0049087421875));
   for (int i = 0; i <= 4;){
     stepper2.nextAction();
     if(timer2Running == true && digitalRead(HOMING2) == LOW){
       timer1B = millis();
       timer2Running = false;  
     }
     if(timer2Running == false && digitalRead(HOMING2) == HIGH){
       timer2B = millis();
       timer2Running = true;
       timer3B[i] = timer2B - timer1B;
      // Serial.println(timer3B[i]);
       homed2 = true;
       i++;
     }

  }
    timer3B[1] = ((timer3B[1] + timer3B[2] + timer3B[3]+ timer3B[4])/4)*1.50;
}

 if(homedFinal == true && homed2 == true){
     stepper2.nextAction();
     if(timer2Running == true && digitalRead(HOMING2) == LOW){ // we start timmer when there is nothing in front of the sensor
      timer1B = millis();
      timer2Running = false;
     }
    if(timer2Running == false && digitalRead(HOMING2) == HIGH){ // resets timer here
      timer2B = millis();
      timer2Running = true;
    }
    if(timer2Running == false && digitalRead(HOMING2) == LOW){ // here we check to see if there is too much time passing
      timer2B = millis();
      if (timer2B - timer1B > timer3B[1]){
      //Serial.println("Homed 2"); 
      stepper2.stop();
      BC = BCMax;
      homedFinal2 = true;
      }
    }
   }

  
  
   if(homedFinal == true && homedFinal2 == true){   
       File posFile = SD.open ("test.txt", FILE_READ); // !!!Change the file name on this to whatever you will put on the SD card!!!!
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
             if (rocker == 0){ // we will use the rocker to set the first 5 or 6 numbers on the fille to set machine settings. ie: machine size (Not Implemented)
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
                changeBC = newBC - BC;
            //    Serial.println((String)"AC: " + newAC + " : " + changeAC);
              // Serial.println((String)"BC: " + newBC + " : " + changeBC);
                if (sqrt(sq(changeAC)) >= 5 || sqrt(sq(changeBC)) >= 5){ // We check for a large movement. This usually idicates when the pen should be lifted. (Seems to be working well and consistant)
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
                  Serial.println((String)changeAC + "A");
                  Serial.println((String)changeBC + "B");
                  moveACsteps = (changeAC/0.0049087421875); // we move the amount of stepps it will take to move the chain a certain amount of mm. The long number is the conversion of steps to mm (example: to move 1 mm it would take 203.718 steps)
                  moveBCsteps = (changeBC/0.0049087421875);
                  controller.move(moveACsteps,-moveBCsteps);
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













