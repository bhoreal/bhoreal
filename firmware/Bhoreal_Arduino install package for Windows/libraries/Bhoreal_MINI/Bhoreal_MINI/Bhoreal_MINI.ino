//
// BHOREAL SLIM / MINI BETA FIRMWARE v0.1            //
// Programmed by M. A. de Heras y A. Posada          //
// Thanks to Adafruit for the great library Neopixel //
//                                                   // 
///////////////////////////////////////////////////////

#include <Arduino.h>
#include <BhorealMini.h>

BhorealMini Bhoreal;

void setup() {
  Bhoreal.begin();
  // Run the startup animation
  Bhoreal.startup();
}
  
void loop () {
 
  Bhoreal.midiRefresh();
  
  Bhoreal.checkButtons();
  // Check and report the ADC states, if necessary
  Bhoreal.checkADC();
  
  Bhoreal.displayRefresh();

}


