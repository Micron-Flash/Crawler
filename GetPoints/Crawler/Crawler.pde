import geomerative.*;
import g4p_controls.*;

RShape grp;
RPoint[][] points;
RPoint[][] movePoint;
PrintWriter filePos;

GButton importButton, exportButton, moveButton;
GTextField scaleInput , anchorPoints, chainLength;
GLabel scaleLabel , anchorUnits, chainLabel;
int q = 1;
String file = "default.svg";
String scaleString;
float offsetX;
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
  
  size(600, 400);  
  surface.setResizable(true);
  frameRate(60);
  background(250);
  //offsetX = width/2;
 // offsetY = height/2;
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
   rect(200, 15, (anchor) * scale2, (chain) * scale2);
   grp = RG.loadShape(file);
   grp.scale((scale/100) * scale2);
   
 
   if ( chain * scale2 >= height){
       scale2 = (height- 30)/(chain);
   }
   else if (anchor * scale2 >= width){
     scale2 = (width - 215)/(anchor);
   }

  
    if(moving){
      if (q == 1){
      movePoint = grp.getPointsInPaths();
      x3 = movePoint[1][1].x;
      y3 = movePoint[1][1].y; 
      q=0;
      }
         offsetX = (mouseX - x3);
         offsetY = (mouseY - y3);
         if (mousePressed && (mouseButton == LEFT)){
           moving = false;
          
         }
     }
  
    grp.translate(offsetX,offsetY);
       grp.draw();
   
   
     if (render && fileSelected){  
       grp.scale(scale/100);

       points = grp.getPointsInPaths();
       for(int i=0; i<points.length; i++){
         for(int j = 0; j<points[i].length; j++){
            fill(50);
            ellipse(points[i][j].x, points[i][j].y,5,5);
            filePos.println((points[i][j].x + offsetX));
            filePos.println((points[i][j].y + offsetY));
           
        }
      }
      
      filePos.flush();
      filePos.close();
      render = false;
      fileSelected = false;
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
   q =1;
   }
   else if (textControl == anchorPoints){
    anchor = Float.parseFloat(textControl.getText());
    scale2 = 1;
    q = 1;
   }
   else if (textControl == chainLength){
     chain = Float.parseFloat(textControl.getText());
     scale2 =1;
     q = 1;
   }
}
