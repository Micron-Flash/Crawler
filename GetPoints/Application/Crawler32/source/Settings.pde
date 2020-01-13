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

  
  void draw() {
    fill(10);
    text("!!CURREN" ,90 ,10);
  }
 


}
