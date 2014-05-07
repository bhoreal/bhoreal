
///////////////////////////////////////////////////////
// BHOREAL SLIM / MINI BETA FIRMWARE v0.1            //
// Programmed by M. A. de Heras y A. Posada          //
// Thanks to Adafruit for the great library Neopixel //
//                                                   // 
///////////////////////////////////////////////////////

#include <Arduino.h>
#include "Bhoreal.h"

// Serial data transfer rate
#define BAUD 57600

Bhoreal Bhoreal;

void setup() {

  //Bhoreal.begin(SLIM, BAUD);
  Bhoreal.begin(BAUD);
  // Run the startup animation
  Bhoreal.startup();
}
  
void loop () {
  // Turn on the lights
  Bhoreal.midiRefresh();
  // Check the button states
  Bhoreal.checkButtons();
  // Check and report the ADC states, if necessary
  Bhoreal.checkADC();
  

}


