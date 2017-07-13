//
// BHOREAL SLIM / MINI BETA FIRMWARE v0.1            //
// Programmed by M. A. de Heras y A. Posada          //
// Thanks to Adafruit for the great library Neopixel //
//                                                   // 
///////////////////////////////////////////////////////

#include <Arduino.h>
#include <BhorealSlim.h>
#include <Wire.h>
#include <EEPROM.h>

BhorealSlim Bhoreal;

void setup() {
  Bhoreal.begin(false); //True for use battery
  // Run the startup animation
  Bhoreal.startup();
}
  
void loop () {
 
  Bhoreal.checkButtonsMIDI();

  Bhoreal.midiRefresh();
  // Check and report the ADC states, if necessary
  Bhoreal.checkAccel();
  
  Bhoreal.displayRefresh();

}


