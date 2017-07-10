/**
 * Bhoreal - Processing OSC communication example
 * This sketch works with the MAX patch "MIDI-OSC_com"
 *
 * Here you can find an example of how Bhoreal sends MIDI
 * and is converted and sended to Processing (vía port 8000)
 * Also you can build an OSC message, and send it transformed into
 * MIDI by MAX.
 */
 
import oscP5.*;
import netP5.*;
  
OscP5 oscP5;
NetAddress tosendAdress;
String ipToSend = "127.0.0.1"; // host, this machine!
int listenPort = 8000;
int tosendPort = 7001;

void setup() {
  size(200,200);
  frameRate(25);
  oscP5 = new OscP5(this, listenPort);
  // Establish connection 
  tosendAdress = new NetAddress(ipToSend,tosendPort);
}

void draw() {
  background(0);  
}

void mousePressed() {
 /* Everytime the sketch running windows is pressed, an OSC message is sended
  * As the MAX patch for Bhoreal MIDI-OSC communication accepts osc message 
  * with this look: "/led X Y" where X is the number of pad (0 - 15) and Y the 
  * color HUE value (0-255), the OSC messages are built that way. Here is an exmaple.
  */
  OscMessage myMessage = new OscMessage("/led");
  myMessage.add(3);    // add an int to the osc message, the 3rd pad in this case
  myMessage.add(150);  // add an other int, the HUE
  /* send the message */
  oscP5.send(myMessage, tosendAdress); 
}


/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the typetag, the address pattern and the value of the received OscMessage */
  print(" # received # typetag: "+ theOscMessage.typetag());
  println(" address: "+ theOscMessage.addrPattern());
  println(" value: "+ theOscMessage.get(0).intValue());
}