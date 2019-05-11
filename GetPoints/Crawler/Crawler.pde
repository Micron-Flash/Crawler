import geomerative.*;
import g4p_controls.*;


RShape grp;
RPoint[][] points;
PrintWriter filePos;

GButton importButton, exportButton, moveButton;
GTextField scaleInput;

String file = "default.svg";
String scaleString;
int offsetX;
int offsetY;
float scale = 100;
Boolean moving = false;
Boolean render = false;
Boolean imp = false;
Boolean fileSelected = false;

void setup() {
  
  size(600, 400);  
  surface.setResizable(true);
  frameRate(60);
  background(250);
  offsetX = width/2;
  offsetY = height/2;
  RG.init(this);
  
  RG.setPolygonizer(RG.UNIFORMLENGTH);
  RG.setPolygonizerLength(map(1,1,height, 3, 200));
 
  importButton = new GButton(this , 5 , 15 , 170 , 20,"Import SVG");
  
  moveButton = new GButton(this , 5 , 40 , 170 , 20,"Move SVG");
  
  exportButton = new GButton(this , 5 , 65 , 170 , 20,"Render Points");
  
  scaleInput = new GTextField(this, 5 , 90 , 170, 20);
  scaleInput.setPromptText("Scale");
  scaleInput.setText("100");
}

void draw(){
   translate(0,0);
   background(250);
   stroke(0);
   fill(200);
   rect(0,0,180,height);
  //scaleString = scaleInput.getText();
   
   //importButton.setLocalColor(4,#CECECE);
    if (moving){
         offsetX = mouseX;
         offsetY = mouseY;
         if (mousePressed && (mouseButton == LEFT)){
           moving = false;
         }
     }
   grp = RG.loadShape(file);
   grp.scale(scale/100);
   grp.translate(offsetX,offsetY);
   grp.draw();
   points = grp.getPointsInPaths();
   
   if(points != null){
     if (render && fileSelected){
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
      
    }
    
  }
}


void handleButtonEvents(GButton button, GEvent event) {
  if (button == importButton){
     imp = true;
     selectInput("Select an SVG file:", "fileSelected");
  }
  else if (button == moveButton){
     moving = true;   
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
   scale = Float.parseFloat(textControl.getText());
}
