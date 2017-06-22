class Button {
  PVector pos;
  int on;
  int id;
  int corX;
  int corY; 
  PImage buttonOff;
  PImage buttonOn;
  PImage light;
  int size_x = 72;
  int size_y = 72;
  int change = 0;
  Boolean click = true;
  int isClicked = 1;
  Boolean touching;
  Boolean showInfo = false;
  color myLightColor = color(255, 255, 255);
  color myOldColor = color(255, 255, 255);
  int alpha = 170;


  Button(PVector in, int i, int x, int y) {
    corX = x;
    corY = y;
    pos = in;
    id = i;
    buttonOff = loadImage("button_3.png");
    buttonOn = loadImage("button_1.png");
    light = loadImage("light.png");
  }

  //-------------------------------------------------------------------------------
  void display() {
    tint(255);
    if (mouseClick(new PVector(mouseX, mouseY), mousePressed)) { 
      image(buttonOn, pos.x, pos.y);
    } 
    else {
      image(buttonOff, pos.x, pos.y);
    }


    if (showInfo) {
      String myText = Integer.toString(corX) + " "+  Integer.toString(corY);
      fill(100);
      text(myText, pos.x+32, pos.y+50);
    }

    if (on == 1) {
      if(myLightColor == color(255,255,255)) alpha = 230; 
      else alpha = 150;
      tint(myLightColor, alpha);
      image(light, pos.x, pos.y);
    }
  }

  //-------------------------------------------------------------------------------
  Boolean mouseOver(PVector mouse) {
    Boolean over = false;
    if (mouse.x > pos.x && mouse.x < pos.x+size_x && mouse.y > pos.y && mouse.y < pos.y+size_y) over = true;
    return over;
  }

  //-------------------------------------------------------------------------------
  Boolean mouseClick(PVector mouse, Boolean click) {
    touching = false; 
    if (mouse.x > pos.x+15 && mouse.x < pos.x+size_x && mouse.y > pos.y+15 && mouse.y < pos.y+size_y && click == true) {
      touching = true;
    }
    return touching;
  }

  //-------------------------------------------------------------------------------
  void setLedColor(int uno, int dos, int tres) {
    myOldColor = color(uno,dos,tres);
  }

  //-------------------------------------------------------------------------------
  void showCord(Boolean showThisInfo) {
    showInfo = showThisInfo;
  }

  //-------------------------------------------------------------------------------
  void oneMessage() {
    if (mouseClick(new PVector(mouseX, mouseY), mousePressed) && change == 0) {
      change = 1;
      sendingInfo(change);
    } 
    else if (mousePressed == false && change == 1) {
      change = 0;
      sendingInfo(change);
    }
  }

  //-------------------------------------------------------------------------------
  void sendingInfo(int clickIn) {
    isClicked = clickIn;
    OscMessage myMessage = new OscMessage(header);
    myMessage.add(corX);
    myMessage.add(corY);
    myMessage.add(isClicked);
    oscP5.send(myMessage, myRemoteLocation);
//    println(corX + " " +corY + " " + isClicked);
    debugText += "\n" + "send:    " + header +" "+ corX + " " +corY + " " + isClicked;
    myTextarea.setText(limitSize(debugText));
  }

  //-------------------------------------------------------------------------------
  void setButton(int onIn) {
    on = onIn;
    myLightColor = myOldColor;
  }
  
    void setButton(boolean turnOn) {  
    on = turnOn? 1 : 0;
    myLightColor = myOldColor;
  }

  //-------------------------------------------------------------------------------  
  int getButton() {
    return on;
  }
  
  //-------------------------------------------------------------------------------
  void clear(){
  on = 0;
  }

  //-------------------------------------------------------------------------------
  void update() {
    display();
    oneMessage();
  }
}

