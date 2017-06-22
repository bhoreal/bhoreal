/* 
 Created by David Dalmazzo
 Bhoral Emulator
 http://bhoreal.com/
 http://goteo.org/project/bhoreal/home
 24th July 2013
 */

import controlP5.*;
import oscP5.*;
import netP5.*;


OscP5 oscP5;
ControlP5 cp5;
Textarea myTextarea;
NetAddress myRemoteLocation;


int nx = 8;
int ny = 8;
int space = 85;
int num = nx*ny;
Button[] myButton = new Button[num];
int change = 0;
int [][] buttons;
String ipValue = "127.0.0.1";
String port = "8000";
String in_port = "8080";
String header = "/bhoreal/press";
String headerIn = "/bhoreal/led";
String colorIn = "/bhoreal/rgb";
String clear = "/bhoreal/clear";
String ledRow = "/bhoreal/led_row";
String ledCol = "/bhoreal/led_col";
String debugText;
int intChannel;
int intInChannel;
PImage background;
PImage logo;

//-----------------------------------------------------------------------------------------------------
void setup() {
  size(1200, 745, OPENGL);
  frameRate(60);
  intChannel = Integer.parseInt(port);
  intInChannel = Integer.parseInt(in_port);
  oscP5 = new OscP5(this, intInChannel);
  myRemoteLocation = new NetAddress(ipValue, intChannel);
  smooth();
  strokeWeight(2.0);
  background = loadImage("fondo_2.png");
  //background = loadImage("fondoBlack.png");
  logo = loadImage("logo.png");
  int counter = 0;
  for (int i = 0; i < nx; i++) {
    for (int j = 0; j < ny; j++) {
      myButton[counter] = new Button(new PVector((i*space)+13, (j*space)+15), counter, i, j);
      //myButton[counter].setLedColor(0,150,255);
      counter++;
    }
  }
  textArea();
}

//-----------------------------------------------------------------------------------------------------
void draw() {
  background(250);
  tint(240);
  image(background, 0, 0);
  tint(255, 150);
  image(logo, 732, 655, 455, 83);
  //  fill(0,50);
  //  stroke(150);
  //  rect(800,200,300,300);
  for (int i = 0; i < num; i++) {
    myButton[i].update();
  }
}

//-----------------------------------------------------------------------------------------------------
public void ipValue(String theText) {
  myRemoteLocation = new NetAddress(theText, intChannel);
  // automatically receives results from controller input
    println("a textfield event for controller 'input' : "+theText);
    debugText += "\n" + "IP = " + theText;
    myTextarea.setText(debugText);
}

public void port(String theText) {
  intChannel = Integer.parseInt(theText);
  myRemoteLocation = new NetAddress(ipValue, intChannel);
  // automatically receives results from controller input
    debugText += "\n" + "port = " + theText;
    myTextarea.setText(limitSize(debugText));
  //  println("a textfield event for controller 'input' : "+theText);
}

public void in_port(String theText) {
  intInChannel = Integer.parseInt(theText);
  oscP5 = new OscP5(this, intInChannel);
  // automatically receives results from controller input
    debugText += "\n" + "receive_port = " + theText;
    myTextarea.setText(limitSize(debugText));
  //  println("a textfield event for controller 'input' : "+theText);
}

//-----------------------------------------------------------------------------------------------------
void mouseClicked() {
  if (mouseX > 745 && mouseX < 1177 && mouseY > 660 && mouseY < 720) { 
    link("http://bhoreal.com/");
    mouseX = 0;
    mouseY = 0;
  }
}

//-----------------------------------------------------------------------------------------------------
String limitSize(String theText) {
  int tailIndex = (theText.length() < 1020) ? 0 : theText.length() - 1020;
  String tail = theText.substring(tailIndex);
  debugText = tail;
  return tail;
}

