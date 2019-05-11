import geomerative.*;

// Declare the objects we are going to use, so that they are accesible from setup() and from draw()
RFont f;
RShape grp;
RPoint[] points;
  
PrintWriter outputx;


void setup(){
  // Initilaize the sketch
  size(1300,1300);
  frameRate(24);
  outputx = createWriter("positionsx.txt"); 
  background(255);
  fill(255,102,0);
  stroke(0);
  
  // VERY IMPORTANT: Allways initialize the library in the setup
  RG.init(this);
  
  //  Load the font file we want to use (the file must be in the data folder in the sketch floder), with the size 60 and the alignment CENTER
  //grp = RG.getText("HI", "FreeSans.ttf", 72, CENTER);
    grp = RG.loadShape("Bird.svg");
    grp.scale(.5);
  // Enable smoothing
  smooth();
}

void draw(){
  // Clean frame
  background(255);
  
  // Set the origin to draw in the middle of the sketch
  translate(10,10);
  
  // Draw the group of shapes
  noFill();
  stroke(0,0,200,150);
  RG.setPolygonizer(RG.ADAPTATIVE);
  //grp.draw();
  
  // Get the points on the curve's shape
 // RG.setPolygonizer(RG.UNIFORMSTEP);
 // RG.setPolygonizerStep(map(250, 0.0, float(height), 0.0, 1.0));
  
  RG.setPolygonizer(RG.UNIFORMLENGTH);
  RG.setPolygonizerLength(map(1,1,height, 3, 200));
  points = grp.getPoints();
  
  // If there are any points
  if(points != null){
    noFill();
    stroke(0,200,0);
    beginShape();
    for(int i=0; i<points.length; i++){
     vertex(points[i].x+500, points[i].y+500);
    }
    vertex(0, 0);
    vertex(1270, 0);
    endShape();
    //ellipse(1270/2,558.8,5,5);
    fill(0);
    stroke(0);
    //outputx.println(points.length);
    for(int i=0; i<points.length; i++){
      ellipse(points[i].x+500, points[i].y+500,5,5);
      //text(points[i].x,points[i].x,points[i].y);
      outputx.println((points[i].x+500));
      outputx.println((points[i].y+500));
      //delay(1);
    }
  //  ellipse(points[0].x+500, points[0].y+500,5,5); 
    outputx.flush();  // Writes the remaining data to the file
    outputx.close();
  }
}
