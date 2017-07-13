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

// WiFly Auth Modes
#define OPEN   0
#define WEP    1
#define WPA2   2
#define WPA1   3
#define MIXED  4
#define WEP64  8

// WiFly transmision Modes
#define UDP  1
#define TCP  2

// WiFly Mode
#define AP  0
#define CLIENT 1

BhorealSlim Bhoreal;

char mySSID[] = "Hello_pepe";  
char myPass[] = "labinteractius";
byte myAuth = WPA2;
char *IPHOST = "192.168.1.255";
int protocol = UDP;
uint16_t outPort = 8000;  // DESTINATION UDP PORT
uint16_t localPort = 9000;   // LOCAL UDP PORT

void setup() {
  Bhoreal.begin(true); //True for use battery
  
  Bhoreal.wifiBegin(CLIENT, mySSID, myPass, myAuth, IPHOST, protocol, outPort, localPort); //Only for SLIMPRO
  // Run the startup animation
  Bhoreal.startup();
}
  
void loop () {
 
  Bhoreal.checkButtonsUDP();

  // Check and report the ADC states, if necessary
  Bhoreal.checkAccel();
  
  Bhoreal.displayRefresh();

}


