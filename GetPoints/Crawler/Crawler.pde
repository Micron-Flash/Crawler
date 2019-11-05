/*MIT License

Copyright (c) 2019 Hunter Movius

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/



import geomerative.*;
import g4p_controls.*;

RShape grp;
RPoint[][] points;
RPoint[][] movePoint;
RPoint[][] linePoint;
PrintWriter filePos;

GButton importButton, exportButton, moveButton;
GTextField scaleInput , anchorPoints, chainLength;
GLabel scaleLabel , anchorUnits, chainLabel;
int q = 1;
String file = "default.svg";
String scaleString;
float offsetX ;
float offsetY;

float scale = 100;
float scale2 = 1;
float anchor;
float chain;
float x3;
float y3;
Boolean moving = false;
Boolean render = false;
Boolean imp = false;
Boolean fileSelected = false;

void setup() {
  
  size(1200, 800);  
  surface.setResizable(true);
  frameRate(60);
  background(250);
  offsetX = width/2; // try and set the  default image to the center of the screen
  offsetY = height/2;
  RG.init(this);
  
  RG.setPolygonizer(RG.UNIFORMLENGTH);
  RG.setPolygonizerLength(1*scale2);
 
  importButton = new GButton(this , 5 , 15 , 170 , 20,"Import SVG");
  
  moveButton = new GButton(this , 5 , 40 , 170 , 20,"Move SVG");
  
  exportButton = new GButton(this , 5 , 65 , 170 , 20,"Render Points");
  
  scaleInput = new GTextField(this, 5 , 90 , 25, 20);
  scaleInput.setPromptText("Scale");
  scaleInput.setText("100");
  
  scaleLabel = new GLabel(this, 27 , 90 , 120, 20, "%  SVG scale");
  
  anchorPoints = new GTextField(this, 5 , 115 , 140 , 20);
  anchorPoints.setPromptText("Length Between Anchors");
  
  anchorUnits = new GLabel(this, 145, 115, 25, 20 , "mm");
  
  chainLength = new GTextField(this, 5 , 140 , 140, 20);
  chainLength.setPromptText("Chain Length");
  
  chainLabel = new GLabel(this, 145 , 140, 150,20, "mm");
}

void draw(){
   translate(0,0);
   background(240);
   stroke(0);
   fill(200);
   rect(0,0,180,height);
   fill(255);
   rect(200, 15, (anchor) * scale2, (chain) * scale2); // this will draw the workspace based on the distance between the anchors and chain length
   grp = RG.loadShape(file);
   grp.scale((scale/100) * scale2); // there are 2 different scales. scale2 is hidden and is adjusted based on the size of the screen. scale is user defined and will make the end image larger or smaller
   
 
   if ( chain * scale2 >= height){  // here we will automatically adjust the workspace to fit on the screen
       scale2 = (height- 30)/(chain);
   }
   else if (anchor * scale2 >= width){
     scale2 = (width - 215)/(anchor);
   }  
    if(moving){
      if (q == 1){
      movePoint = grp.getPointsInPaths();
      x3 = movePoint[0][0].x;
      y3 = movePoint[0][0].y;  // we set the base move point to the first point on the first curve. This will be where the robot starts
      q=0;
      }
         offsetX = (mouseX - x3);
         offsetY = (mouseY - y3);
         if (mousePressed && (mouseButton == LEFT)){
           moving = false;
          
         }
     }
       line(200, 15, offsetX + x3,offsetY + y3);
       line((anchor) * scale2 + 200, 15, offsetX+ x3,offsetY+ y3); // this shows the lines where the robot will start. right now its not very usefull but I plan on adding a max length to show the lowest point the robot can go to prevent it falling
       
       grp.translate(offsetX,offsetY);
       grp.draw();
   
   
     if (render && fileSelected){ 
       
     //   points = grp.getPointsInPaths();
    //  filePos.println(scale2/(points[0][0].x-200));
    //  filePos.println(scale2/(points[0][0].y-15));
      // grp.scale(scale/100);
      // grp.translate(offsetX,offsetY);
       points = grp.getPointsInPaths();
       for(int i=0; i<points.length; i++){
         for(int j = 0; j<points[i].length; j++){
            fill(50);
            ellipse(points[i][j].x, points[i][j].y,5,5);
            filePos.println((points[i][j].x -200)/scale2); // writes the coordinates for each point along a curve. We have to adjust the coordinates slightly to compensate for the scalling and off set of our workspace
            filePos.println((points[i][j].y -15)/scale2);
           
        }
      }
      
      filePos.flush();
      filePos.close();
      render = false;
      fileSelected = false; // close the file
    }
    
  }



void handleButtonEvents(GButton button, GEvent event) {
  if (button == importButton){
     imp = true;
     q = 1;
     selectInput("Select an SVG file:", "fileSelected");
  }
  else if (button == moveButton){
     moving = true;
  //   q = 1;
  }
  else if (button == exportButton){
     render = true;   
     selectOutput("Select a file to write to:", "fileSelected");
  }
}


void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } else {
    if (imp){
    file = selection.getAbsolutePath();
    imp = false;
    }
    else if (render){
    filePos = createWriter(selection.getAbsolutePath()); // this will be the file exported. The name of the file must match the name in the firmware on the arduino. 
    fileSelected = true;
    }
  }
}


public void handleTextEvents(GEditableTextControl textControl, GEvent event) { 
   if(textControl == scaleInput){
   scale = Float.parseFloat(textControl.getText());
   q = 1;
   }
   else if (textControl == anchorPoints){
    anchor = Float.parseFloat(textControl.getText());
    scale2 = 1;
    q = 1;
   }
   else if (textControl == chainLength){
     chain = Float.parseFloat(textControl.getText());
     scale2 = 1;
     q = 1;
   }
}
