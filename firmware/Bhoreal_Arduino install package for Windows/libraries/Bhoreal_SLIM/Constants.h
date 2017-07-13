/* 

Bhoreal models

*/

#define SLIM  0  //Tamaño de la matriz
#define SLIMPRO  1  //Tamaño de la matriz

/* 

WIFI modes

*/


#define AP     0
#define CLIENT 1
#define PROG_CLIENT 2
#define PROG_AP     3

/* 

WIFI AND SERVER STATICS - WiFly, Http server parameters.

*/

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
#define HTML 16

#define SELECTOR 0 
#define MIDI  2


#define EXT_ANT "1" // External Antenna
#define INT_ANT "0" // Internal Antenna

/* 

WIFLY Firmware Setting

*/

// WIFLY FIRMWARE AUTO UPDATE AND VERSIONS
#define WIFLY_LATEST_VERSION 441
#define DEFAULT_WIFLY_FIRMWARE "ftp update wifly3-441.img"
#define DEFAULT_WIFLY_FTP_UPDATE "set ftp address 198.175.253.161"

// const char mySSIDAP[] = "bhoreal";  // SSID AP MODE
// const char myPassAP[] = "";         // PASSWORD AP MODE
// const char *IPHOST_AP = "192.168.0.255"; // IP destination AP MODE
// const byte myAuthAP = OPEN;
  
const char antenna[] = INT_ANT; // INTERNAL ANTENNA

/* 

ARDUINO ports definitions - GPIOS and ADCs 

*/

// Pin definitions for the 74HC164 SIPO shift register (drives button rows high)
#define DATAPIN     9 
#define CLOCKPIN    8
// Pin definitions for the 74HC165 PISO shift register (reads button column state)
#define INDATAPIN  13
#define INCLOCKPIN  5
#define INLOADPIN  10 // toggling this tell the 165 to read the value into its memory for reading

#define VBAT  A0      // Battery State
#define FACTORY  A5   // FACTORY RESET Wifly
#define AWAKE  22     // AWAKE Wifly
#define RST 11        // RESET ATMEGA328
#define MUX 12        // MUX FOR WIFI OR ATMEGA328
#define BOT 7         // sleep pushbotom
#define POWER_VCC 4   // Power control PMOS

#define CHANNEL 3 // Channel for i2c

//Accelerometer parameters
#define lim 512         //Central position of the values for each axis (acel = 0) 
#define DEVICE 0x53     //ADXL345 device address
#define TO_READ 6       //num of bytes we are going to read each time (two bytes for each axis)
int regAddress = 0x32;  //first axis-acceleration-data register on the ADXL345

int mode = 1; // mode = MIDI, UDP, TCP, ACEL, .... (1 = MIDI BY DEFAULT)
boolean charge_on = true; // CHARGER Activated
int WIFIMode = CLIENT;    // Client Mode by default (connection to external Wifi network)
uint16_t NUM_ROWS =  8; 
int NUM_LEDS   =  64; 
const uint16_t numBytes = 192;
#define PIN_LED 6    //Pin for control of WS2812

//Constant parameters
#define VCC_BATTERY 4618  //Maximum Voltage in mV
#define BAT_MIN 3300  // Minimum Voltage in mV
#define RESOLUTION 1023.  //Resolution for ADC
  
  
/* 

Internal EEPROM Memory Addresses

*/ 

#define EE_ADDR_TIME_VERSION        0   //32BYTES 
#define EE_ADDR_POWER               32   //1BYTE 

static char* WEB[4]={                  
                  "apps.ioapps.net",
//                  "GET /xavidev/bhoreal/notifications.php HTTP/1.1 \n",
                  "GET /xavidev/bhoreal/notifications.php?user=miguel HTTP/1.1 \n",
                  "Host: apps.ioapps.net \n",
                  "User-Agent: Bhoreal \n\n"  
                  };