
/**
 * Bhoreal - Processing MIDI to OSC communication example
 */
 
import oscP5.*;
import netP5.*;
import controlP5.*;
import themidibus.*;

MidiBus myBus; // The MidiBus
ControlP5 cp5;
Range range;
OscP5 oscP5;
NetAddress tosendAdress;
String ipToSend = "127.0.0.1"; // host, this machine!
int listenPort = 8000;
int tosendPort = 7001;
// 
// flags for console control
boolean printMIDI, printOSC;
// Midi stuff to use in each action
int channel = 0;
int pitch = 64;
int velocity = 127;
int sliderpos = 0;
int padColor = 0;
int PAD_Color = 0;
Note note = new Note(channel, pitch, velocity);

int[][] matrixLed = {  {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0},
                       {0, 0, 0, 0}  };

void setup() {
  size(400,150);
  frameRate(25);
  printMIDI = false;
  printOSC = false;
  oscP5 = new OscP5(this, listenPort);
  // Establish connection 
  tosendAdress = new NetAddress(ipToSend,tosendPort);
  
  // list all MIDI devices availables, with name and index
  MidiBus.list();
  // create a MIDI bus by using device name on "In" and "Out" device arguments.
  myBus = new MidiBus(this, "Arduino Leonardo", "Arduino Leonardo");
  
  // graphic stuff
  cp5 = new ControlP5(this);
  cp5.addSlider("PAD_Color")
     .setPosition(300,20)
     .setSize(20,80)
     .setRange(0,127)
     ;
   for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      cp5.addButton("cpPad" + (i+4*j))
        .setValue(0)
        .setPosition(200 +i*20, 20 +j*20)
        .setSize(19,19)
        ;
      cp5.getController("cpPad" + (i+4*j)).getCaptionLabel().hide();
    }
   }
}

void draw() {
  
  background(0);

  // a graphic indicator of PAD pressed/unpressed
  noFill();
  stroke(255);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if( matrixLed[i][j] == 1) { fill(255);}
      else { noFill(); }
      rect(20+ i*20,20+ j*20,20,20);
    }
  }
  
  // a graphic indicator of Slider position
  noFill();
  stroke(255);
  rect(120, 20, 20, 80);  
  fill(255);
  rect(120, map(sliderpos, 0, 127, 90, 20), 20, 10);
  
  // a Hue color indicator for color selection in "PAD_COLOR"
  noStroke();
  colorMode(HSB, 255);
  for (int i = 0; i < 76; i++) {
    fill(color(map(i, 0 , 76, 255, 0), 255, 255));
    rect(289, 22 + i, 10, 1);
  }
  colorMode(RGB, 255);
  fill(255);
  rect(289, 20, 10, 2);
  noFill();
  stroke(255);
  rect(289, 20, 10, 80);  
  
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
  if ( printOSC == true) {
    println();
    print("# received # typetag: " + theOscMessage.typetag());
    print(" | address: " + theOscMessage.addrPattern());
    print(" | value: " + theOscMessage.get(0).intValue());
  }
  // turn into MIDI
  if( theOscMessage.addrPattern().equals("/led")) {
    channel = 1;                                  // the channel
    pitch = theOscMessage.get(0).intValue();      // the pad
    velocity = theOscMessage.get(1).intValue();   // the color
    note.setChannel(channel);
    note.setPitch(pitch);
    note.setVelocity(velocity);
    // send the MIDI
    myBus.sendNoteOn(note);
  }
}



void noteOn(Note note) {
  // Receive a noteOn
  if(printMIDI == true ) {
    println();
    print("Note On:");
    print(" | Chan: " + note.channel());
    print(" | Pitch: " + note.pitch());
    print(" | Vel: " + note.velocity());
  }
  // turn into OSC
  OscMessage myMessage = new OscMessage("/pad"+ str(note.pitch()));  // /padX, where X is the number of pad (0-15)
  if(note.velocity() == 64) {myMessage.add(1);}                      // value of the pad pressed/unpressed (1/0)
  else {myMessage.add(0);} 
  oscP5.send(myMessage, tosendAdress);
  // tell the matrix a pad is pressed
  matrixLed[fromPadtoMatrix(note.pitch())[0]]
           [fromPadtoMatrix(note.pitch())[1]] = 1;
}

void noteOff(Note note) {
  // Receive a noteOff
  if(printMIDI == true ) {
    println();
    print("Note Off:");
    print(" | Chan: " + note.channel());
    print(" | Pitch: " + note.pitch());
    print(" | Vel: " + note.velocity());
  }
  // turn into OSC
  OscMessage myMessage = new OscMessage("/pad"+ str(note.pitch())); // /padX, where X is the number of pad (0-15)
  if(note.velocity() == 0) {myMessage.add(0);}                      // value of the pad pressed/unpressed (1/0)
  else {myMessage.add(1);} 
  oscP5.send(myMessage, tosendAdress);
  // tell the matrix a pad is unpressed 
  matrixLed[fromPadtoMatrix(note.pitch())[0]]
           [fromPadtoMatrix(note.pitch())[1]] = 0;
}

void controllerChange(ControlChange change) {
  // Receive a controllerChange
  if(printMIDI == true ) {
    println();
    print("CC:");
    print(" | Chan: " + change.channel());
    print(" | Num: " + change.number());
    print(" | Val: " + change.value());
  }
  // turn into OSC
  OscMessage myMessage = new OscMessage("/slider"+ str(note.pitch()));  // /slider, a Control Change value
  myMessage.add(map(change.value(), 0, 127, 0, 255));                   // the CC value of the slider (0-255)
  oscP5.send(myMessage, tosendAdress);
  // tell the graphic indicator the new slider position
  sliderpos = change.value();
}