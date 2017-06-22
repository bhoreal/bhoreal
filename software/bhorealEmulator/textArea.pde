void textArea() {
  PFont font = createFont("arial", 15);
  cp5 = new ControlP5(this);
  textFont(font);
  //text buttons parameters
  cp5.addTextfield("ipValue")
  .setPosition(800, 10)
  .setSize(300, 28)
  .setFont(font)
  .setFocus(true)
  .setAutoClear(false)
  .setColor(color(80))
  .setColorBackground(color(200))
  .setColorLabel(color(100))
  .setText( ipValue)
  .setColorCaptionLabel(color(0, 155, 255)) 
  ;
  cp5.addTextfield("port")
  .setPosition(800, 55)
  .setSize(145, 28)
  .setFont(font)
  .setFocus(true)
  .setAutoClear(false)
  .setColor(color(80))
  .setColorBackground(color(200))
  .setColorLabel(color(100))
  .setText( port)
  .setColorCaptionLabel(color(0, 155, 255)) 
  ;
  cp5.addTextfield("in_port")
  .setPosition(955, 55)
  .setSize(145, 28)
  .setFont(font)
  .setFocus(true)
  .setAutoClear(false)
  .setColor(color(80))
  .setColorBackground(color(200))
  .setColorLabel(color(100))
  .setText( in_port)
  .setColorCaptionLabel(color(0, 155, 255)) 
  ;
  //adding space for debugging text 
  myTextarea = cp5.addTextarea("txt")
  .setPosition(800, 120)
  .setSize(300, 500)
  .setFont(createFont("Monaco", 12))
  .setLineHeight(14)
  .setColor(color(80))
  .setBorderColor(color(20, 255)) 
  .setColorBackground(color(0, 200))
  .scroll(1.0)
  .setColorForeground(color(0, 180, 255, 220));
  myTextarea.setColor(0xffffffff);
  debugText = "Debuggin Console" + "\n" + "IP = " +  ipValue +  "\n" + "port = " + port+
  "\n" + "receive_port = " +in_port;
  myTextarea.setText(debugText);
}

