#include <SD.h>
float X;
float Y;
int rocker = 0;
File myFile;
 
void setup()
{
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
   pinMode(10, OUTPUT);
 
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
 
}
 
void loop()
{
  
 File myFile = SD.open ("test.txt", FILE_READ);
  if(myFile){
    
      char fileContents[10]; // Probably can be smaller
   byte index = 0;
   while (myFile.available()) 
  {
      char aChar = myFile.read();
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
           
          }
          else {
            Y = atof(fileContents);
            rocker = 0;
            
          }
            //float aVal = atof(fileContents);

            if (rocker == 0 ){
             Serial.print("X: ");
             Serial.println(X);
            Serial.print("Y: ");
             Serial.println(Y);
             delay(500);
            }
         }

         fileContents[0] = '\0';
         index = 0;
      }
   }
   }
   

}
