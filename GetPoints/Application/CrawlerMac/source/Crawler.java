import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import geomerative.*; 
import g4p_controls.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class Crawler extends PApplet {

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






RShape grp;
RPoint[][] points;
RPoint[][] movePoint;
RPoint[][] linePoint;
PrintWriter filePos;

GButton importButton, exportButton, moveButton, settingsButton, load, save;
GTextField scaleInput , anchorPoints, chainLength, whiteBoardXSize, whiteBoardYSize, whiteBoardOffset;
GWindow settingsWindow;


int q = 1;
String file = "default.svg";
String profile = "Default.txt";
String [] rawProfile;
String scaleString;
float offsetX ;
float offsetY;
Boolean settingDraw = true;
float scale = 100;
float scale2 = 1;
float anchor;
float chain;
float whiteboardL;
float whiteboardH;
float whiteboardVOffset;
float whiteboardHOffset;
float x3;
float y3;
Boolean moving = false;
Boolean render = false;
Boolean imp = false;
Boolean fileSelected = false;
Settings settings;
PApplet apps;


public void setup() {
  G4P.setGlobalColorScheme(5);
  
  getSettings();
    
  surface.setResizable(true);
  frameRate(60);
  background(250);
  offsetX = width/2; // try and set the  default image to the center of the screen
  offsetY = height/2;
  getSettings();
  RG.init(this);
  
  RG.setPolygonizer(RG.UNIFORMLENGTH);
  RG.setPolygonizerLength(1*scale2);
 
  importButton = new GButton(this , 5 , 15 , 170 , 20,"Import SVG");
  
  moveButton = new GButton(this , 5 , 40 , 170 , 20,"Move SVG");
  
  exportButton = new GButton(this , 5 , 65 , 170 , 20,"Render Points");
  
  scaleInput = new GTextField(this, 5 , 90 , 25, 20);
  scaleInput.setPromptText("Scale");
  scaleInput.setText("100");
  
  GLabel scaleLabel = new GLabel(this, 29 , 90 , 120, 20, "%  SVG scale");
  
   settingsButton = new GButton(this , 5 , 115 , 170 , 20,"Settings");

}

public void draw(){
  
   translate(0,0);
   background(240);
   stroke(0);
   fill(200);
   rect(0,0,180,height);
   fill(150);
   rect(200, 15, (anchor) * scale2, (chain) * scale2); // this will draw the workspace based on the distance between the anchors and chain length
   fill(255);
   rect(200+(whiteboardHOffset*scale2),(whiteboardVOffset*scale2)+15,whiteboardL*scale2,whiteboardH*scale2);
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
         fill(10);
         text("X: "+ ((mouseX-200)/scale2), 15, height-25 );
         text("Y: "+ ((mouseY-15)/scale2),15, height-10 );
         if (mousePressed && (mouseButton == LEFT)){
           moving = false;
          
         }
     }
       line(200, 15, offsetX + x3,offsetY + y3);
       line((anchor) * scale2 + 200, 15, offsetX+ x3,offsetY+ y3); // this shows the lines where the robot will start. right now its not very usefull but I plan on adding a max length to show the lowest point the robot can go to prevent it falling

       grp.translate(offsetX,offsetY);
       grp.draw();
   
   
     if (render && fileSelected){ 
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



public void handleButtonEvents(GButton button, GEvent event) {
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
  else if (button == settingsButton){
    openSettings();
  }
  
}


public void fileSelected(File selection) {
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
   else if (textControl == whiteBoardXSize){
   // anchor = Float.parseFloat(textControl.getText());
    
   }
}

public void openSettings(){
  settingDraw = true;
  settingsWindow =  GWindow.getWindow(this, "Settings", 100, 50, 350, 400, JAVA2D);
  settingsWindow.addDrawHandler(this, "windowDraw");
  settings = new Settings(settingsWindow, profile);
}

public void windowDraw(PApplet app, GWinData data){
    app.background(240);
}


public void getSettings(){
  
  rawProfile = loadStrings(profile);
  for (int i = 0; i <= 5; i++){
   switch(i){
      case 0:
      anchor = Float.parseFloat(rawProfile[i]);
      break;
      case 1:
      chain = Float.parseFloat(rawProfile[i]);
      break;
      case 2:
      whiteboardL = Float.parseFloat(rawProfile[i]);
      break;
      case 3:
      whiteboardH = Float.parseFloat(rawProfile[i]);
      break;
      case 4:
      whiteboardVOffset = Float.parseFloat(rawProfile[i]);
      break;
      case 5:
      whiteboardHOffset = Float.parseFloat(rawProfile[i]);
      break;
     }
   }
  }
public class  Settings {
  PApplet pa;
  String profileSet;
  Settings(PApplet papp, String profile) {
    pa = papp;
    profileSet = profile;
    
    
    load = new GButton(pa , 90 , 370 , 170 , 20,"Load Profile");
    
    save = new GButton(pa , 90 , 345 , 170 , 20,"Save Profile");
    
    anchorPoints = new GTextField(pa, 5 , 115 , 140 , 20);
    anchorPoints.setPromptText(""+anchor);
  
    GLabel anchorUnits = new GLabel(pa, 145, 115, 25, 20 , "mm");
  
    chainLength = new GTextField(pa,  5 , 140 , 140, 20);
    chainLength.setPromptText(""+chain);
  
    GLabel chainLabel = new GLabel(pa, 145 , 140, 140,20, "mm");
    
    GLabel whiteBoard = new GLabel(pa, 5 , 165, 150,20, "Whiteboard size:");
    
    whiteBoardXSize = new GTextField(pa, 5 , 190, 50,20);
    whiteBoardXSize.setPromptText(""+whiteboardL);
    GLabel mmX = new GLabel(pa, 55, 190, 50,20,"mm");
    
    whiteBoardYSize = new GTextField(pa, 80 , 190, 50,20);
    whiteBoardYSize.setPromptText(""+whiteboardH);
    GLabel mmY = new GLabel(pa, 130, 190, 50,20,"mm");
    
    GLabel whiteBoardLabel = new GLabel(pa, 5, 215, 200,20, "Distance from top anchor");
    
 
    whiteBoardOffset = new GTextField(pa, 5 , 235, 50,20);
    GLabel whiteBoardLabel2 = new GLabel(pa, 55 , 235, 50,20, "mm");
    whiteBoardOffset.setPromptText("" + whiteboardVOffset);
   }

  
  public void draw() {
    fill(10);
    text("!!CURREN" ,90 ,10);
  }
 


}
  public void settings() {  size(1200, 800); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "Crawler" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
