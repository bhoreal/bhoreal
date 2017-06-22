
//receiving data from Max in the port 8080
void oscEvent(OscMessage theOscMessage) {
  //Turning on every led
  if (theOscMessage.checkAddrPattern(headerIn)) {
    if (theOscMessage.checkTypetag("iii")) {
      int first = theOscMessage.get(0).intValue();  
      int second = theOscMessage.get(1).intValue();  
      int third = theOscMessage.get(2).intValue();
      int theId = (first * 8) + second;
      myButton[theId].setButton(third);
      debugText += "\n" + "receive: " + headerIn + " "+ first + " " +second + " " + third;
      myTextarea.setText(limitSize(debugText));
    } else {
      println(theOscMessage);
    }
  }

  //set the color
  if (theOscMessage.checkAddrPattern(colorIn)) {
    if (theOscMessage.checkTypetag("iii")) {
      int first = theOscMessage.get(0).intValue();  
      int second = theOscMessage.get(1).intValue();  
      int third = theOscMessage.get(2).intValue();
      for (int i = 0; i < myButton.length; i++) {
        myButton[i].setLedColor(first, second, third);
      }
      debugText += "\n" + "receive: " + colorIn + " "+ first + " " +second + " " + third;
      myTextarea.setText(limitSize(debugText));
    }
  }

  //clear all
  if (theOscMessage.checkAddrPattern(clear)) {
    for (int i = 0; i < myButton.length; i++) {
      myButton[i].clear();
    }
    debugText += "\n" + "receive: " + clear;
    myTextarea.setText(limitSize(debugText));
  }

  //Set row
  if (theOscMessage.checkAddrPattern(ledRow)) {
    if (theOscMessage.checkTypetag("ii")) {
      int first = theOscMessage.get(0).intValue();  
      int second = theOscMessage.get(1).intValue();
      debugText += "\n" + "receive: " + ledRow + " "+ first + " " +second;
      myTextarea.setText(debugText);
      boolean[] bits = new boolean[8];
      for (int j = 7; j >= 0; j--) {
        bits[j] = (second & (1 << j)) != 0;
        myButton[j*8+first].setButton(bits[j]);
        println(first + " = " + bits[j]);
      }
    }
  }

  //Set col
  if (theOscMessage.checkAddrPattern(ledCol)) {
    if (theOscMessage.checkTypetag("ii")) {
      int first = theOscMessage.get(0).intValue();  
      int second = theOscMessage.get(1).intValue();
      debugText += "\n" + "receive: " + ledRow + " "+ first + " " +second;
      myTextarea.setText(limitSize(debugText));
      boolean[] bits = new boolean[8];
      for (int j = 7; j >= 0; j--) {
        bits[j] = (second & (1 << j)) != 0;
        myButton[j+first*8].setButton(bits[j]);
//        println(first + " = " + bits[j]);
      }
    }
  }
}

