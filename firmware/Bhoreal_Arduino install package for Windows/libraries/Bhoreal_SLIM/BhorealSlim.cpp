#include "bhorealSlim.h"
#include "Constants.h"
#include <EEPROM.h>


BhorealSlim Bhoreal_;
uint8_t pixels[numBytes];
uint32_t baud[3]={57600, 9600, 115200};

  
boolean pressed[8][8] = {      // pushbottons states matrix
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1}
};

const byte remapMini[4][4] =    // mapping matrix for Mini
{
  { 3, 4, 11, 12 }, 
  { 2, 5, 10, 13 }, 
  { 1, 6,  9, 14 }, 
  { 0, 7,  8, 15 } 
};


uint8_t GIR = 0;

byte remapSlim[4][8][8] = 
{
  {             // mapping matrix for Slim
    {7,8,23,24,       39,40,55,56}, 
    {6,9,22,25,       38,41,54,57}, 
    {5,10,21,26,      37,42,53,58}, 
    {4,11,20,27,      36,43,52,59}, 
    {3,12,19,28,      35,44,51,60}, 
    {2,13,18,29,      34,45,50,61}, 
    {1,14,17,30,      33,46,49,62}, 
    {0,15,16,31,      32,47,48,63}, 
  },
  {             // mapping matrix for Slim
    {56,57,58,59,      60,61,62,63}, 
    {55,54,53,52,      51,50,49,48}, 
    {40,41,42,43,      44,45,46,47}, 
    {39,38,37,36,      35,34,33,32}, 
    {24,25,26,27,      28,29,30,31}, 
    {23,22,21,20,      19,18,17,16}, 
    {8,9,10,11,        12,13,14,15}, 
    {7,6,5,4,              3,2,1,0}, 
  },
  {             // mapping matrix for Slim
    {63,48,47,32,       31,16,15,0}, 
    {62,49,46,33,       30,17,14,1}, 
    {61,50,45,34,       29,18,13,2}, 
    {60,51,44,35,       28,19,12,3}, 
    {59,52,43,36,        27,20,11,4}, 
    {58,53,42,37,       26,21,10,5}, 
    {57,54,41,38,        25,22,9,6}, 
    {56,55,40,39,        24,23,8,7}, 
  },
  {             // mapping matrix for Slim
    {0,1,2,3,              4,5,6,7}, 
    {15,14,13,12,        11,10,9,8}, 
    {16,17,18,19,      20,21,22,23}, 
    {31,30,29,28,      27,26,25,24}, 
    {32,33,34,35,      36,37,38,39}, 
    {47,46,45,44,      43,42,41,40}, 
    {48,49,50,51,      52,53,54,55}, 
    {63,62,61,60,      59,58,57,56}, 
  }
};


byte remapMATRIX[4][8][8] = 
{
  {             // mapping matrix for Slim
    {0,1,2,3,              4,5,6,7}, 
    {8,9,10,11,        12,13,14,15}, 
    {16,17,18,19,      20,21,22,23}, 
    {24,25,26,27,      28,29,30,31}, 
    {32,33,34,35,      36,37,38,39}, 
    {40,41,42,43,      44,45,46,47}, 
    {48,49,50,51,      52,53,54,55}, 
    {56,57,58,59,      60,61,62,63}
  },  
  {             // mapping matrix for Slim
    {56,48,40,32,       24,16,8,0}, 
    {57,49,41,33,       25,17,9,1}, 
    {58,50,42,34,      26,18,10,2}, 
    {59,51,43,35,      27,19,11,3},
    {60,52,44,36,      28,20,12,4}, 
    {61,53,45,37,      29,21,13,5},
    {62,54,46,38,      30,22,14,6},
    {63,55,47,39,      31,23,15,7}
  },
  {             // mapping matrix for Slim
    {63,62,61,60,       59,58,57,56}, 
    {55,54,53,52,       51,50,49,48}, 
    {47,46,45,44,       43,42,41,40}, 
    {39,38,37,36,       35,34,33,32}, 
    {31,30,29,28,       27,26,25,24}, 
    {23,22,21,20,       19,18,17,16}, 
    {15,14,13,12,         11,10,9,8}, 
    {7,6,5,4,               3,2,1,0} 
  },
  {             // mapping matrix for Slim
    {7,15,23,31,      39,47,55,63}, 
    {6,14,22,30,      38,46,54,62}, 
    {5,13,21,29,      37,45,53,61}, 
    {4,12,20,28,      36,44,52,60}, 
    {3,11,19,27,      35,43,51,59}, 
    {2,10,18,26,      34,42,50,58}, 
    {1,9,17,25,       33,41,49,57}, 
    {0,8,16,24,       32,40,48,56} 
  }
};


void BhorealSlim::slaveSend(byte val) {
   Wire.beginTransmission(4); //start transmission to device 
   Wire.write(val);        // write value to write
   Wire.endTransmission(); //end transmission
}

byte BhorealSlim::slaveRead(byte reg) 
  {
    unsigned long time_on = millis();
    slaveSend(reg);
    Wire.requestFrom(4, 1);
    while(!Wire.available()&&((millis()-time_on)<5000)) {slaveSend(reg); Wire.requestFrom(4, 1); delay(4);}//Apaga atmega328    // slave may send less than requested
    byte c = 0;
    while (Wire.available()) c = Wire.read(); // receive byte as a character
    return c;
  }
  
//////////////////////////////////////////////////////////////////////
//////////////////////       BHOREAL BEGIN      //////////////////////
//////////////////////////////////////////////////////////////////////

boolean config_wifi = false;

void BhorealSlim::begin(boolean battery)
{
    port = portOutputRegister(digitalPinToPort(PIN_LED));
    pinMask = digitalPinToBitMask(PIN_LED);
	
    DDRB |= B11110000;  // 11,10,9,8 OUTPUT
    DDRC |= B01000000;  // 5  OUTPUT
    DDRC &= B01111111;  // 13 INPUT
    DDRE &= B10111111;  // 7 INPUT
    DDRD |= B11010000;  // 12, 6, 4 OUTPUT
    DDRF |= B00000011;  // A4, A5   OUTPUT
    PORTF |= B00000010; // A4 HIGH
    PORTF &= B11111110; // A5 LOW
    PORTD |= B01010000; // 12, 4  HIGH
    PORTD &= B01111111; // 6 LOW
    PORTB &= B01111111; // 11 LOW, Reset atmega328 ON
    PORTB |= B10000000; // 11 HIGH

    //Inicializa como encendido o apagado
    Wire.begin();    
    slaveRead(3);  //Check slave ON
	
    //Gestion de sleep del Bhoreal

	if ((EEPROM.read(EE_ADDR_POWER)>0)&&(compareData(__TIME__, readData(EE_ADDR_TIME_VERSION)))&&(battery))
	 {
		EEPROM.write(EE_ADDR_POWER, 0);       
		slaveSend(2); //Apaga atmega328
		sleep(); 
		sleepNow();
	 }
	else 
	 {
	    if (!compareData(__TIME__, readData(EE_ADDR_TIME_VERSION)))
		 {
		   writeData(EE_ADDR_TIME_VERSION, __TIME__);//Provisional  
		   config_wifi = true;
		 }
	    if (battery) EEPROM.write(EE_ADDR_POWER, 1);   
	    PORTD &= B11101111; //digitalWrite(POWER_VCC, LOW);
	    slaveSend(1); //Activa atmega328
	 } 
	// Start the serial port   
	Serial.begin(57600); //USB inicializado a 57600 
	AttachInterrupt6(CHANGE); //Cambio de 0 a 1 Provisional
    show();
    writeTo(DEVICE, 0x2D, 0x08);
//  writeTo(DEVICE, 0x31, 0x00); //2g
//  writeTo(DEVICE, 0x31, 0x01); //4g
    writeTo(DEVICE, 0x31, 0x02); //8g
//  writeTo(DEVICE, 0x31, 0x03); //16g
    //AttachInterrupt6(RISING); //Cambio de 0 a 1
}


void BhorealSlim::wifiBegin(int Mode, char *SSID, char *Pass, int Auth, char *IPHOST, int protocol, uint16_t outPort, uint16_t localPort)
{

	  WIFIMode = Mode;
    // Start the wifi
      Serial1.begin(baud[0]); //WIFI inicializado a 57600
      #if BAT_MONITOR
        if ((readBattery()<2000)||(slaveRead(3)>0)) //Low battery level or usb connected
          {
            PORTD &= B11101111; //digitalWrite(POWER_VCC, LOW);
            WIFIsleep();
          }
        else if (readBattery()>BAT_MIN)
      #else
        if (true) // ????????
      #endif
        {
            awake();
			if (config_wifi) config();
			if (WIFIMode == AP)
			  {
				if (apMode(SSID, Pass, Auth, IPHOST, protocol, outPort, localPort))
				  {
					Serial.print("MAC: "); Serial.println(getMAC()); 
					Serial.print("IP: "); Serial.println(getIP()); 
				  }
				else Serial.println("Desconectado :(");
			  }
			else 
			  {
				if (clientMode(SSID, Pass, Auth, IPHOST, protocol, outPort, localPort)) 
				  {
					Serial.println("Conectado!!");
					int report = checkWiFly();
					if (report == 1) Serial.println(F("Wifly Updated."));
					else if (report == 2) Serial.println(F("Update Fail."));
					else if (report == 0) Serial.println(F("WiFly up to date."));
					else if (report == -1) Serial.println(F("Error reading the wifi version."));
					Serial.print("MAC: "); Serial.println(getMAC()); 
					Serial.print("IP: "); Serial.println(getIP()); 
				  }
				else Serial.println("Desconectado :(");
			  }
        }
      else 
        {
          sleep();
        } 
      timer1Initialize();  
}
	
  void BhorealSlim::config(){
      BaudSetup();
      reset();
      delay(1000);
      BaudSetup();
      delay(1000);
  }

boolean BhorealSlim::compareData(char* text, char* text1)
{
  if ((strlen(text))!=(strlen(text1))) return false;
  else 
  {
    for(int i=0; i<strlen(text); i++)
    {
      if (text[i]!=text1[i]) return false;
    }
  }
  return true;
}

char* BhorealSlim::readData(uint16_t eeaddress)
{
  uint16_t i;
  uint8_t temp = EEPROM.read(eeaddress);
  for ( i = eeaddress; ((temp!= 0x00)&&(temp<0x7E)&&(temp>0x1F)&&((i - eeaddress)<buffer_length)); i++) 
  {
    buffer[i - eeaddress] = EEPROM.read(i);
    temp = EEPROM.read(i + 1);
  }
  buffer[i - eeaddress] = 0x00; 
  return buffer;
}

void BhorealSlim::writeData(uint32_t eeaddress, char* text)
{
  for (uint16_t i = eeaddress; i< (eeaddress + buffer_length); i++) EEPROM.write(i, 0x00);
  for (uint16_t i = eeaddress; text[i - eeaddress]!= 0x00; i++) 
    {
      EEPROM.write(i, text[i - eeaddress]); 
    }
}

////////////////////////////////////////////////////////////////
//////////////////////       STARTUP      //////////////////////
////////////////////////////////////////////////////////////////

// Run this animation once at startup. Currently unfinished.
void BhorealSlim::startup(){
  
  for(int x = 0; x < NUM_LEDS; ++x)
   { 
	  uint32_t c = hue2rgb((x+1)*2);  // 128 HUE steps / 64 leds, 2 steps x led
	  if (x == (NUM_LEDS-1)) c = hue2rgb(1);  // 128 HUE steps / 64 leds, 2 steps x led
	  uint8_t
	  r = (uint8_t)(c >> 16),
	  g = (uint8_t)(c >>  8),
	  b = (uint8_t)c;
	  setPixelColor(remapSlim[GIR][x>>3][x%8], r, g, b);
   }  
  show();
}

//////////////////////////////////////////////////////////////////////
//////////////////////  SERIAL PRESS & RELEASE  //////////////////////
//////////////////////////////////////////////////////////////////////

byte value_send = 0;

void BhorealSlim::on_press(byte r, byte c, byte sel){
    value_send = remapMATRIX[GIR][c][r];
    if (sel == MIDI) 
      {
        MIDIEvent e1 = { 0x09, 0x90, value_send , 64  };
        MIDIUSB.write(e1);
      }
    else
      {
        WIFISend(value_send, 1);
      }
}

void BhorealSlim::on_release(byte r, byte c, byte sel){
    value_send = remapMATRIX[GIR][c][r];
    if (sel == MIDI) 
      {
        MIDIEvent e1 = { 0x09, 0x90, value_send , 0  };
        MIDIUSB.write(e1);
      }
    else
      {
        WIFISend(value_send, 0);
      }
}


///////////////////////////////////////////////////////////////
//////////////////////   CHECK BUTTONS   //////////////////////
///////////////////////////////////////////////////////////////

byte count_column = 0;
byte count_file = 0;
unsigned long time_button = 0;

byte modeMAX = 6;
byte model_=0;
void BhorealSlim::checkMenu(boolean model){
	  model_=model;
	  if (model) 
	   {
		modeMAX=6;
		switch (mode) {
        case 0:
          selectMode(model);
          break;
        case 1:
          midiRefresh();
          // Check the button states
          checkMatrix(MIDI);
          break;
        case 2:
          // Check the button states
          checkMatrix(UDP);
          break;
        case 3:
          programMode();
          break;
        case 4:
          demoAccel();
          break;
        case 5:
          white();
          break;
        case 6:
          checkServer();
          break;
		}
	   }
	  else 
	   {
		 modeMAX=4;
	     switch (mode) {
			case 0:
			  selectMode(model);
			  break;
			case 1:
			  midiRefresh();
			  // Check the button states
			  checkMatrix(MIDI);
			  break;
			case 2:
			  programMode();
			  break;
			case 3:
			  demoAccel();
			  break;
			case 4:
			  white();
			  break;
		}
	   }
}

void BhorealSlim::checkButtonsMIDI()
{
	checkMatrix(MIDI);
}

void BhorealSlim::checkButtonsUDP()
{
	checkMatrix(UDP);
}

byte r[8] = {4, 5, 6, 7, 0, 1, 2, 3};

int mode_ant = mode - 1;

void BhorealSlim::checkMatrix(byte sel)
{
 if (sel==UDP) timer1Initialize();
 else if(sel==MIDI)  timer1Stop();
 
 PORTB &= B11101111; //digitalWrite(CLOCKPIN,LOW);
 PORTB |= B00100000; //digitalWrite(DATAPIN, HIGH); 
 for(byte c = 0; c < NUM_ROWS; c++)
   {
	PORTB |= B00010000; //digitalWrite(CLOCKPIN, HIGH);
	PORTB &= B10111111; //digitalWrite(INLOADPIN, LOW); // read into register
	delayMicroseconds(1);
	PORTB |= B01000000; //digitalWrite(INLOADPIN, HIGH); // done reading into register, ready for us to read
	
	for(int i= 0; i < NUM_ROWS; i++){ // read each of the 165's 4 inputs (or its snapshot of it rather)
	  // tell the 165 to send the first inputs pin state
	  PORTC &= B10111111; //digitalWrite(INCLOCKPIN, LOW);
	  // read the current output
	  if(pressed[c][r[i]] != PINC>>7){ //digitalRead(INDATAPIN)){ // read the state
		pressed[c][r[i]] = PINC>>7; //digitalRead(INDATAPIN);
		if(!pressed[c][r[i]]){
		  if (sel!=SELECTOR) on_press(c, r[i], sel);
		  else
		   {
			 if ((c + r[i]*8)== 63) 
			   {
				 if (WIFIMode == CLIENT) WIFIMode = PROG_AP;
				 else if (WIFIMode == AP) WIFIMode = PROG_CLIENT;
			   }
			 else if ((c + r[i]*8)== 62) charge_on=!charge_on;
			 else if ((c + r[i]*8)<modeMAX) mode_ant = c + r[i]*8;  
		   }
		}
		else if (sel!=SELECTOR) on_release(c, r[i], sel);
	  }
	  // tell the 165 we are done reading the state, the next inclockpin=0 will output the next input value
	   PORTC |= B01000000; //digitalWrite(INCLOCKPIN, HIGH);
	}
	
	PORTB &= B11101111; //digitalWrite(CLOCKPIN,LOW);
	PORTB &= B11011111; //digitalWrite(DATAPIN, LOW);  
   }
}

////////////////////////////////////////////////////////////////
//////////////////////    REFRESH LED     //////////////////////
////////////////////////////////////////////////////////////////
byte refresh_led = 0;
unsigned long time_led = 0;
void BhorealSlim::displayRefresh(){ 
  if (((refresh_led>0)&&((micros()-time_led)>1000))||(refresh_led>=NUM_LEDS)||((refresh_led<NUM_LEDS)&&((micros()-time_led)>25000))&&(refresh_led>0))
    {
      refresh_led=0;
      show();
    }
}

////////////////////////////////////////////////////////////////
////////////////////// REFRESH MIDI & LED  /////////////////////
////////////////////////////////////////////////////////////////

  void BhorealSlim::midiRefresh(){ 
      while(MIDIUSB.available() > 0) 
      {
       MIDIEvent e;
       e = MIDIUSB.read();
	   if((e.type == 0x09) && (e.m3))  // NoteON midi message with vel > 0
		{  
		  uint32_t c = hue2rgb(e.m3);  // velocity is used to HUE color selection and HUE is converted to RGB uint32 
		  uint8_t
			r = (uint8_t)(c >> 16),
			g = (uint8_t)(c >>  8),
			b = (uint8_t)c;
		  setPixelColor(remapSlim[GIR][e.m2>>3][e.m2%8], r, g, b);
		  refresh_led++;
		  time_led = micros();
		}
		else if( (e.type == 0x08) || ((e.type == 0x09) && !e.m3) ) // NoteOFF midi message
		{  
		  setPixelColor(remapSlim[GIR][e.m2>>3][e.m2%8], 0, 0, 0);
		  refresh_led++;
		  time_led = micros();
		}  
      }
  }

////////////////////////////////////////////////////////////////
//////////////////////  CHECK ADC INPUTS  //////////////////////
////////////////////////////////////////////////////////////////

float BhorealSlim::readBattery()
{
	 return analogRead(VBAT)*(VCC_BATTERY/RESOLUTION);
}

//---------------- Functions
//Writes val to address register on device
void BhorealSlim::writeTo(int device, byte address, byte val) {
	 Wire.beginTransmission(device); //start transmission to device 
	 Wire.write(address);        // write register address
	 Wire.write(val);        // write value to write
	 Wire.endTransmission(); //end transmission
}

//reads num bytes starting from address register on device in to buff array
void BhorealSlim::readFrom(int device, byte address, int num, byte buff[]) {
	Wire.beginTransmission(device); //start transmission to device 
	Wire.write(address);        //writes address to read from
	Wire.endTransmission(); //end transmission

	Wire.beginTransmission(device); //start transmission to device
	Wire.requestFrom(device, num);    // request 6 bytes from device

	int i = 0;
	while(Wire.available())    //device may write less than requested (abnormal)
	 { 
		buff[i] = Wire.read(); // read a byte
		i++;
	 }
	Wire.endTransmission(); //end transmission
}

int x=0;
int y=0;
int z=0;     
byte buff[TO_READ] ;    //6 bytes buffer for saving data read from the device
	  
void BhorealSlim::checkAccel(){  
	checkBattery();
	readFrom(DEVICE, regAddress, TO_READ, buff); //read the acceleration data from the ADXL345
	x = (((int)buff[1]) << 8) | buff[0]; 
	x = map(x,-lim,lim,0,1023);  
	y = (((int)buff[3])<< 8) | buff[2];
	y = map(y,-lim,lim,0,1023); 
	z = (((int)buff[5]) << 8) | buff[4];
	z = map(z,-lim,lim,0,1023); 
	int limitx = map(x, 448, 575, 7, 0);
	if (limitx==0) limitx =1;
	int limity = map(y, 448, 575, 0, 7);
	if (limity==7) limity=6;
	if (limitx<2) GIR=2;
	else if (limitx>5) GIR=0;
	else if (limity<2) GIR=1;
	else if (limity>5) GIR=3;  
	//Serial.println(GIR);
}

boolean charge_state = false;
float charge = 0;

void BhorealSlim::checkBattery()
  {
	#if (MODEL == SLIMPRO)
	  #if BAT_MONITOR
		charge = readBattery();
		if ((charge<(BAT_MIN - 300))&&(charge>2000)&&(!charge_state))
		{        
		  charge_state = true;
		  slaveSend(2); //Duerme atmega328
		  sleep();
		  sleepNow();
		}
		else if (((charge>BAT_MIN)||(charge<2000))&&(charge_state))
				 {
				   charge_state = false;
				   slaveRead(1); //Activa atmega328
				   awake();
				 }
	  #endif
	#endif
  }

///////////////////////////////////////////////////////////////
//////////////////////     HUE -> RGB    //////////////////////
///////////////////////////////////////////////////////////////

uint8_t rh;
uint8_t gh;
uint8_t bh;
  
uint32_t BhorealSlim::hue2rgb(uint16_t hueValue)
{
      if (hueValue==0)
        {
            rh = 0; 
            gh = 0; 
            bh = 0; 
        } 
      else if (hueValue>=127)
        {
            rh = 255; 
            gh = 255; 
            bh = 255; 
        }
      else
        {
          hueValue<<= 3;  // 128 midi steps -> 1024 hue steps
    
          if (hueValue < 341)  { // Lowest third of the potentiometer's range (0-340)
            hueValue = (hueValue * 3) / 4; // Normalize to 0-255
        
            rh = 255 - hueValue;  // Red from full to off
            gh = hueValue;        // Green from off to full
            bh = 1;               // Blue off
          }
          else if (hueValue < 682) { // Middle third of potentiometer's range (341-681)
            hueValue = ( (hueValue-341) * 3) / 4; // Normalize to 0-255
        
            rh = 1;              // Red off
            gh = 255 - hueValue; // Green from full to off
            bh = hueValue;       // Blue from off to full
          }
          else  { // Upper third of potentiometer"s range (682-1023)
            hueValue = ( (hueValue-683) * 3) / 4; // Normalize to 0-255
        
            rh = hueValue;       // Red from off to full
            gh = 1;              // Green off
            bh = 255 - hueValue; // Blue from full to off
          }
        }
      
      return ((uint32_t)rh << 16) | ((uint32_t)gh <<  8) | bh;
}

///////////////////////////////////////////////////////////////
//////////////////////     WIFI     ///////////////////////////
///////////////////////////////////////////////////////////////

boolean FindInResponse(const char *toMatch,
                                    unsigned int timeOut = 1000) {
  int byteRead;

  unsigned long timeOutTarget; // in milliseconds

  for (unsigned int offset = 0; offset < strlen(toMatch); offset++) {
    timeOutTarget = millis() + timeOut; // Doesn't handle timer wrapping
    while (!Serial1.available()) {
      // Wait, with optional time out.
      if (timeOut > 0) {
        if (millis() > timeOutTarget) {
          return false;
        }
      }
      delay(1); // This seems to improve reliability slightly
    }
    byteRead = Serial1.read();
    //Serial.print((char)byteRead);
    delay(1); // Removing logging may affect timing slightly

    if (byteRead != toMatch[offset]) {
      offset = 0;
      // Ignore character read if it's not a match for the start of the string
      if (byteRead != toMatch[offset]) {
        offset = -1;
      }
      continue;
    }
  }

  return true;
}

boolean SendCommand(const __FlashStringHelper *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {                              
  Serial1.print(command);
  //delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!FindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //sckFindInResponse(expectedResponse);
  }
  return true;
}

boolean SendCommand(const char *command,
                                 boolean isMultipartCommand = false,
                                 const char *expectedResponse = "AOK") {                               
  Serial1.print(command);
  //delay(20);
  if (!isMultipartCommand) {
    Serial1.flush();
    Serial1.println();

    // TODO: Handle other responses
    //       (e.g. autoconnect message before it's turned off,
    //        DHCP messages, and/or ERR etc)
    if (!FindInResponse(expectedResponse, 3000)) {
      return false;
    }
    //findInResponse(expectedResponse);
  }
  return true;
}

#define COMMAND_MODE_ENTER_RETRY_ATTEMPTS 2

#define COMMAND_MODE_GUARD_TIME 250 // in milliseconds


boolean EnterCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.print(F("$$$"));
      delay(COMMAND_MODE_GUARD_TIME);
      Serial1.println();
      Serial1.println();
      if (FindInResponse("\r\n<", 100))
      {
        return true;
      }
    }
    return false;
}

boolean ExitCommandMode() {
    for (int retryCount = 0; retryCount < COMMAND_MODE_ENTER_RETRY_ATTEMPTS; retryCount++) 
     {
      if (SendCommand(F("exit"), false, "EXIT")) 
      {
      return true;
      }
    }
    return false;
}

void SkipRemainderOfResponse(unsigned int timeOut) {
  unsigned long time = millis();
  while (((millis()-time)<timeOut))
  {
    if (Serial1.available())
      { 
        byte temp = Serial1.read();
        //Serial.write(temp);
        time = millis();
      }
  }
}

void BhorealSlim::WIFIsleep() {
      EnterCommandMode();
      SendCommand(F("sleep"));
}

char* BhorealSlim::itoa(int32_t number)
  {
   byte count = 0;
   uint32_t temp;
   if (number < 0) {temp = number*(-1); count++;} 
   while ((temp/10)!=0) 
   {
     temp = temp/10;
     count++;
   }
   int i;
   if (number < 0) {temp = number*(-1);} 
   else temp = number;
   for (i = count; i>=0; i--) 
   { 
     buffer[i] = temp%10 + '0'; 
     temp = temp/10; 
   }
   if (number < 0) {buffer[0] = '-';} 
   
   buffer[count + 1] = 0x00;
   return buffer;   
  }
  
void BhorealSlim::sleep() {
      digitalWrite(POWER_VCC, HIGH); 
}

boolean BhorealSlim::awake() {
      PORTD &= B11101111; //digitalWrite(POWER_VCC, LOW); 
      delay(500);
      writeTo(DEVICE, 0x2D, 0x08);
  //  writeTo(DEVICE, 0x31, 0x00); //2g
  //  writeTo(DEVICE, 0x31, 0x01); //4g
      writeTo(DEVICE, 0x31, 0x02); //8g
  //  writeTo(DEVICE, 0x31, 0x03); //16g
}

boolean Ready()
{
  //if (apmode) return true;
  if (EnterCommandMode())
    {
      Serial1.println(F("join"));
      if (FindInResponse("Associated!", 8000)) 
      {
        SkipRemainderOfResponse(3000);
        ExitCommandMode();
        return(true);
      }
   } 
  else return(false);
}

char *SSID_;  
char *Pass_;
byte Auth_;
char *IPHOST_;
int  protocol_;
uint16_t outPort_;
uint16_t localPort_;

boolean BhorealSlim::clientMode(char *SSID, char *Pass, int Auth, char *IPHOST, int protocol, uint16_t outPort, uint16_t localPort)
  {
    SSID_=SSID;  
	Pass_=Pass;
	Auth_=Auth;
	IPHOST_=IPHOST;
	protocol_=protocol;
	outPort_=outPort;
	localPort_=localPort;
    if (!Ready())
    {
      return reConnect();   
    }
     else return true;  
  }


  
boolean BhorealSlim::apMode(char *SSID, char *Pass, int Auth, char *IPHOST, int protocol, uint16_t outPort, uint16_t localPort)
  {
	  
      if(EnterCommandMode())
        {    
            SendCommand(F("set opt deviceid Bhoreal"));
            SendCommand(F("set wlan join 7")); // Enable AP mode
            SendCommand(F("set ip dhcp 4")); //Enables DHCP server in soft AP mode
            SendCommand(F("set ip address 192.168.1.2"));
            SendCommand(F("set ip gateway 192.168.1.2"));
            SendCommand(F("set ip net 255.255.255.0"));
            SendCommand(F("set comm time 20"));//default 5
            SendCommand(F("set sys sleep 0"));
            SendCommand(F("set ip flags 0x6"));
            SendCommand(F("set sys trig 0"));
            SendCommand(F("set uart mode 0x10"));
            SendCommand(F("set wlan rate 12"));
            SendCommand(F("set comm size 1420"));
			
            SendCommand(F("set ip proto "), true);
            SendCommand(itoa(protocol));
            SendCommand(F("set ip host "), true);
            SendCommand(IPHOST);
            SendCommand(F("set ip localport "), true);
            SendCommand(itoa(localPort));
            SendCommand(F("set ip remote "), true);
            SendCommand(itoa(outPort));
            SendCommand(F(DEFAULT_WIFLY_FTP_UPDATE)); //ftp server update
            SendCommand(F("set ftp mode 1"));
            SendCommand(F("set wlan auth "), true);
            SendCommand(itoa(Auth));
            Serial.print(Auth);
            SendCommand(F("set wlan ssid "), true);
            SendCommand(SSID);
            Serial.print(F(" "));
            Serial.print(SSID);
            if ((Auth==WPA1)||(Auth==WPA2)) 
              {
                SendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, OPEN
                SendCommand(Pass);
                Serial.print(F(" "));
                Serial.print(Pass);
                
              }
            else if ((Auth==WEP)||(Auth==WEP64)) 
              {
                SendCommand(F("set wlan key "), true);
                SendCommand(Pass);
                Serial.print(F(" "));
                Serial.print(Pass);
              }
            SendCommand(F("set wlan ext_antenna "), true);
            SendCommand(antenna);
            Serial.print(F(" "));
            Serial.println(antenna);
            SendCommand(F("save"), false, "Storing in config"); // Store settings
            SendCommand(F("reboot"), false, "*READY*");
            return true;
        }
        return false;   
  }
  
boolean BhorealSlim::reConnect()
  {
      if(EnterCommandMode())
        {    
            SendCommand(F("set opt deviceid Bhoreal"));
            SendCommand(F("set wlan join 1")); // Disable AP mode
            SendCommand(F("set ip dhcp 1")); // Turns DHCP on.
            SendCommand(F("set comm time 5"));
            SendCommand(F("set ip flags 0x6"));
            SendCommand(F("set sys trig 0"));
            SendCommand(F("set uart mode 0x10"));
            SendCommand(F("set wlan rate 12"));
            SendCommand(F("set comm size 1420"));
            SendCommand(F("set ip proto "), true);
            SendCommand(itoa(protocol_));
            SendCommand(F("set ip host "), true);
            SendCommand(IPHOST_);
            SendCommand(F("set ip localport "), true);
            SendCommand(itoa(localPort_));
            SendCommand(F("set ip remote "), true);
            SendCommand(itoa(outPort_));
            SendCommand(F(DEFAULT_WIFLY_FTP_UPDATE)); //ftp server update
            SendCommand(F("set ftp mode 1"));
            SendCommand(F("set wlan auth "), true);
            SendCommand(itoa(Auth_));
            Serial.print(F("AUTH: "));
            Serial.println(Auth_);
            SendCommand(F("set wlan ssid "), true);
            SendCommand(SSID_);
            Serial.print(F("SSID: "));
            Serial.println(SSID_);
            if ((Auth_==WPA1)||(Auth_==WPA2)||(Auth_==MIXED)) 
              {
                SendCommand(F("set wlan phrase "), true);  // WPA1, WPA2, MIXED
                SendCommand(Pass_);
                
                Serial.print(F("PASS: "));
                Serial.println(Pass_);
                
              }
            else if ((Auth_==WEP)||(Auth_==WEP64)) 
              {
                SendCommand(F("set wlan key "), true);
                SendCommand(Pass_);
                Serial.print(F("PASS: "));
                Serial.println(Pass_);
              }
            SendCommand(F("set wlan ext_antenna "), true);
            SendCommand(antenna);
            Serial.print(F("ANTENNA: "));
            Serial.println(antenna);
            SendCommand(F("save"), false, "Storing in config"); // Store settings
            SendCommand(F("reboot"), false, "*READY*");
            if (Ready()) 
              {
                Serial.println("Modulo configurado.");
                return true;
              }
        }
        return false;   
  }
  
  boolean BhorealSlim::reset() {
    if (EnterCommandMode())
      {
        SendCommand(F("factory R"), false, "Set Factory Defaults"); // Store settings
        SendCommand(F("save"), false, "Storing in config"); // Store settings
        SendCommand(F("reboot"), false, "*READY*");
        Serial.println("Modulo reseteado");
      }
  }

  int BhorealSlim::checkWiFly() {
    int ver = getWiFlyVersion();
    if (ver > 0)
    {
      if (ver < WIFLY_LATEST_VERSION)
       {
        int state = 1;
        if(update()); //Wifly Updated.
        else state = 2; //Update Fail.
        reset();
        BaudSetup();
        return state;
       }   
      else return 0; //WiFly up to date.
    }
    else return -1; //Error reading the wifi version.
  }
  
  #define MAC_ADDRESS_BUFFER_SIZE 18 // "FF:FF:FF:FF:FF:FF\0"
  
  char* BhorealSlim::getMAC() {
    if (EnterCommandMode()) 
    {
      if (SendCommand(F("get mac"), false, "Mac Addr="))
      {
        char newChar;
        byte offset = 0;
  
        while (offset < MAC_ADDRESS_BUFFER_SIZE) {
          if (Serial1.available())
          {
            newChar = Serial1.read();
            //Serial.println(newChar);
            if ((newChar == '\n')||(newChar < '0')) {
              buffer[offset] = '\x00';
              break;
            } 
            else if (newChar != -1) {
              buffer[offset] = newChar;
              offset++;
            }
          }
        }
        buffer[MAC_ADDRESS_BUFFER_SIZE-1] = '\x00';
        ExitCommandMode();
      }        
    }
  
    return buffer;
  }
  
  char* BhorealSlim::getIP() {
    if (EnterCommandMode()) 
    {
      if (SendCommand(F("get ip"), false, "IP="))
      {
        char newChar;
        byte offset = 0;
  
        while (offset < MAC_ADDRESS_BUFFER_SIZE) {
          if (Serial1.available())
          {
            newChar = Serial1.read();
            //Serial.println(newChar);
            if (newChar == '\n') {
              buffer[offset] = '\x00';
              break;
            } 
            else if (newChar != -1) {
              buffer[offset] = newChar;
              offset++;
            }
          }
        }
        buffer[MAC_ADDRESS_BUFFER_SIZE-1] = '\x00';
        ExitCommandMode();
      }        
    }
  
    return buffer;
  }
  
  int BhorealSlim::getWiFlyVersion() {
    if (EnterCommandMode()) 
    {
      if (SendCommand(F("ver"), false, "wifly-GSX Ver"))
      {
        char newChar;
        byte offset = 0;
        boolean prevWasNumber = false;
        while (offset < 3) {
          if (Serial1.available())
          {
            newChar = Serial1.read();
            if ((newChar != -1 && isdigit(newChar)) || newChar == '.') {
              if (newChar != '.') {
                buffer[offset] = newChar;
                offset++;
              }
              prevWasNumber = true;
            } 
            else {
              if (prevWasNumber){
                break;
              }
              prevWasNumber = false; 
            }
          }
        }
        ExitCommandMode();
        buffer[offset] = 0x00;
       return atoi(buffer);
      } 
      return 0;
    }
    return 0;
  }
  
  
  boolean BhorealSlim::update() {
    if (EnterCommandMode())
    {
      Serial.println("Actualizando modulo WIFI...");
      SendCommand(F(DEFAULT_WIFLY_FIRMWARE));
      delay(1000);
      if (FindInResponse("FTP OK.", 60000))
      {
        return true;
      }
    }
    else return false;
  }
  
  void BhorealSlim::BaudSetup()
  {
//    while (Serial1.available()) Serial1.read();
    if(!EnterCommandMode())
    {
      Serial.println("Baudrate erroneo!!");
      boolean repair = true;
      for (int i=2; ((i>0)&&repair); i--)
      {
        Serial1.begin(baud[i]);
        delay(1000);
        if(EnterCommandMode()) 
        {
          SendCommand(F("set u b "), true);
          SendCommand(itoa(baud[0]));
          SendCommand(F("save"), false, "Storing in config"); // Store settings
          SendCommand(F("reboot"), false, "*READY*");
          repair = false;
          Serial.println("Baudrate actualizado!!");
        }
      }
      Serial1.begin(baud[0]);
      delay(500);
    }
  }

  
  void BhorealSlim::WIFISend(byte value, boolean state)
  {
	timer1Stop();
    //byte val = (state<<7)|((r << 3) | c);
    Serial1.write((state<<7)|value);
    //Serial.write(val);
	timer1Initialize();
  }

  boolean connected = false;

  boolean BhorealSlim::open(const char *addr, int port) {
  
    if (connected) {
      close();
    } 
    if (EnterCommandMode())
    {
      SendCommand(F("open "), true);
      SendCommand(addr, true);
      Serial1.print(F(" "));
      Serial1.print(port);
      if (SendCommand("", false, "*OPEN*")) 
      {
        connected = true;
        return true;
      }
      else return false;
    }
    EnterCommandMode();
    return false;
  }
  
  boolean BhorealSlim::close() {
    if (!connected) {
      return true;
    }
    if (SendCommand(F("close"), false, "*CLOS*")) {
      connected = false;
      return true;
    }
    connected = false;
    return false;
  }

///////////////////////////////////////////////////////////////
//////////////////////    Interrupccion 6    //////////////////////
///////////////////////////////////////////////////////////////  


//Esta interrupcion no esta soportada por la libreria arcore

  void BhorealSlim::AttachInterrupt6(int mode)
  {
      EICRB = (EICRB & ~((1<<ISC60) | (1<<ISC61))) | (mode << ISC60);
      EIMSK |= (1<<INT6);
  }
  
  void BhorealSlim::detachInterrupt6()
  {
      EIMSK &= ~(1<<INT6);
  }
  
  void BhorealSlim::sleepNow()         // here we put the arduino to sleep
  {    
      delay(100);
      cli();
  
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
      
      
      sleep_enable();          // enables the sleep bit in the mcucr register
                               // so sleep is possible. just a safety pin
    
      sleep_cpu();            // here the device is actually put to sleep!!
                               // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
   
      //sleep_disable();         // first thing after waking from sleep:
                               // disable sleep...
   
  }
  
  unsigned long time_mode = 0;
  ISR(INT6_vect) {

        if (digitalRead(BOT))
          {
             if ((millis()-time_mode)>500)
              {
                 mode = 0;
                 time_mode = millis();   
              }
          }
         else
          {
             mode = mode_ant + 1;
             if (mode > modeMAX) mode = 1;
          }
       
  }
  
  ///////////////////////////////////////////////////////////////
  //////////////////////    Program mode   //////////////////////
  ///////////////////////////////////////////////////////////////    

byte const_program = 3;

void BhorealSlim::programMode()
    {
      unsigned long time = 0;
      boolean flagprog = false;
      Serial1.begin(57600);
      digitalWrite(MUX, LOW);
      for(int x = 0; x < NUM_LEDS; ++x) setPixelColor(x, 0, 0, 0);
      setPixelColor(0, 255, 0, 0);
      show();
      int limit_ant = 0;
      int limit = 1;
	  if (model_)
	   {
		 timer1Stop();
		 const_program = 3;
	   }
	  else	const_program = 2;
      while (mode==const_program)
        {
          if (model_)
		  {
           float charge = readBattery();
           limit = map(charge, 3000, 4200, 0, 7);
           if (limit!=limit_ant)
            {
              for(int x = 0; x <=limit; ++x) setPixelColor(x, 255, 0, 0);
              if (limit>=3) for(int x = 3; x <=limit; ++x) setPixelColor(x, 0, 0, 255);
              if (limit>=6) for(int x = 6; x <=limit; ++x) setPixelColor(x, 0, 255, 0);
              show();
              limit_ant = limit;
            }
           }
           if (Serial.available())
            {
              if (flagprog)
              {
                digitalWrite(RST, LOW);
                delay(1);
                digitalWrite(RST, HIGH);
                flagprog = false;
              }
              Serial1.write(Serial.read());
              time = millis();
            }
          while (!flagprog)
            {
              if (Serial.available())
                {
                  Serial1.write(Serial.read());
                  time = millis();
                }
              if (Serial1.available())
                Serial.write(Serial1.read());
              if ((millis()- time)>=1000) flagprog = true;
            }
          slaveSend(1); //Activa atmega328
       }
       digitalWrite(MUX, HIGH); 
       if (model_)  timer1Initialize();
       Serial1.begin(baud[0]);
    }
	
///////////////////////////////////////////////////////////////
//////////////////////       Demo        //////////////////////
///////////////////////////////////////////////////////////////  

byte const_accel = 4;
void BhorealSlim::demoAccel()
{  
  if (model_)
  {
    timer1Stop();
    const_accel = 4;
  }
  else	const_accel = 3;

  while (mode==const_accel)
    {
      checkAccel();
      for (int i=0; i<8; i++) 
        {
          for (int j=0; j<8; j++)  
            {
              setPixelColor(remapSlim[GIR][i][j], 0, 0, 255);
            }
        } 
      int limitx = map(x, 448, 575, 7, 0);
      if (limitx==0) limitx =1;
      int limity = map(y, 448, 575, 0, 7);
      if (limity==7) limity=6;
      setPixelColor(remapSlim[0][limitx][limity], 255, 0, 0);
      setPixelColor(remapSlim[0][limitx-1][limity], 255, 0, 0);
      setPixelColor(remapSlim[0][limitx][limity+1], 255, 0, 0);
      setPixelColor(remapSlim[0][limitx-1][limity+1], 255, 0, 0);
      show();
   }
   
   if (model_) timer1Initialize();
}

byte const_white = 5;

  void BhorealSlim::white()
  { 
	if (model_)
	  {
		timer1Stop();
		const_white = 5;
	  }
	else const_white = 4;

	while (mode==const_white)
      {
        for(int x = 0; x < NUM_LEDS; ++x) setPixelColor(x, 255, 255, 255);
        show();  
      }
    if (model_)timer1Initialize();
  }
  
  void BhorealSlim::printChar(char* text)
  {
    for (int x=0; x<64; x++) 
    {
      uint32_t c = hue2rgb(text[x]);  // 128 HUE steps / 64 leds, 2 steps x led
      uint8_t r = (uint8_t)(c >> 16);
      uint8_t g = (uint8_t)(c >>  8);
      uint8_t b = (uint8_t)c;
      setPixelColor(remapSlim[GIR][x>>3][x%8],r, g, b);
    }
    show();
  }

///////////////////////////////////////////////////////////////
//////////////////////    Control led    //////////////////////
///////////////////////////////////////////////////////////////  

void BhorealSlim::show(void) {

  if(!pixels) return;

  // Data latch = 50+ microsecond pause in the output stream.  Rather than
  // put a delay at the end of the function, the ending time is noted and
  // the function will simply hold off (if needed) on issuing the
  // subsequent round of data until the latch time has elapsed.  This
  // allows the mainline code to start generating the next frame of data
  // rather than stalling for the latch.
  while((micros() - endTime) < 50L);
  // endTime is a private member (rather than global var) so that mutliple
  // instances on different pins can be quickly issued in succession (each
  // instance doesn't delay the next).

  // In order to make this code runtime-configurable to work with any pin,
  // SBI/CBI instructions are eschewed in favor of full PORT writes via the
  // OUT or ST instructions.  It relies on two facts: that peripheral
  // functions (such as PWM) take precedence on output pins, so our PORT-
  // wide writes won't interfere, and that interrupts are globally disabled
  // while data is being issued to the LEDs, so no other code will be
  // accessing the PORT.  The code takes an initial 'snapshot' of the PORT
  // state, computes 'pin high' and 'pin low' values, and writes these back
  // to the PORT register as needed.

  noInterrupts(); // Need 100% focus on instruction timing
  
  volatile uint16_t i   = numBytes; // Loop counter
  volatile uint8_t  *ptr = pixels;   // Pointer to next byte
  volatile uint8_t  b   = *ptr++;   // Current byte value
  volatile uint8_t  hi;             // PORT w/output bit set high
  volatile uint8_t  lo;             // PORT w/output bit set low
    
  // WS2811 and WS2812 have different hi/lo duty cycles; this is
  // similar but NOT an exact copy of the prior 400-on-8 code.

  // 20 inst. clocks per bit: HHHHHxxxxxxxxLLLLLLL
  // ST instructions:         ^   ^        ^       (T=0,5,13)

  volatile uint8_t next, bit;

  hi   = *port |  pinMask;
  lo   = *port & ~pinMask;
  next = lo;
  bit  = 8;

  asm volatile(
   "head20:"                   "\n\t" // Clk  Pseudocode    (T =  0)
    "st   %a[port],  %[hi]"    "\n\t" // 2    PORT = hi     (T =  2)
    "sbrc %[byte],  7"         "\n\t" // 1-2  if(b & 128)
     "mov  %[next], %[hi]"     "\n\t" // 0-1   next = hi    (T =  4)
    "dec  %[bit]"              "\n\t" // 1    bit--         (T =  5)
    "st   %a[port],  %[next]"  "\n\t" // 2    PORT = next   (T =  7)
    "mov  %[next] ,  %[lo]"    "\n\t" // 1    next = lo     (T =  8)
    "breq nextbyte20"          "\n\t" // 1-2  if(bit == 0) (from dec above)
    "rol  %[byte]"             "\n\t" // 1    b <<= 1       (T = 10)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 12)
    "nop"                      "\n\t" // 1    nop           (T = 13)
    "st   %a[port],  %[lo]"    "\n\t" // 2    PORT = lo     (T = 15)
    "nop"                      "\n\t" // 1    nop           (T = 16)
    "rjmp .+0"                 "\n\t" // 2    nop nop       (T = 18)
    "rjmp head20"              "\n\t" // 2    -> head20 (next bit out)
   "nextbyte20:"               "\n\t" //                    (T = 10)
    "ldi  %[bit]  ,  8"        "\n\t" // 1    bit = 8       (T = 11)
    "ld   %[byte] ,  %a[ptr]+" "\n\t" // 2    b = *ptr++    (T = 13)
    "st   %a[port], %[lo]"     "\n\t" // 2    PORT = lo     (T = 15)
    "nop"                      "\n\t" // 1    nop           (T = 16)
    "sbiw %[count], 1"         "\n\t" // 2    i--           (T = 18)
     "brne head20"             "\n"   // 2    if(i != 0) -> (next byte)
    : [port]  "+e" (port),
      [byte]  "+r" (b),
      [bit]   "+r" (bit),
      [next]  "+r" (next),
      [count] "+w" (i)
    : [ptr]    "e" (ptr),
      [hi]     "r" (hi),
      [lo]     "r" (lo));

  interrupts();
  endTime = micros(); // Save EOD time for latch on next call
}

// Set pixel color from separate R,G,B components:
void BhorealSlim::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < NUM_LEDS) {
    uint8_t *p = &pixels[n * 3];
    *p++ = g;
    *p++ = r; 
    *p = b;
  }
}

/*TIMER*/
   
  boolean offsetWIFI = 0;
  byte inByte = 0;
  byte ledNumber = 0;
  uint32_t color;
  uint8_t red, green, blue;
  void BhorealSlim::wifiRead()
    {
     while ((Serial1.available())&&(mode!=0))
      {
        inByte = Serial1.read();
        //Serial.println(inByte);
        if ((inByte>127)&&(inByte<192))
          {
                offsetWIFI = true; 
                ledNumber = inByte&0x7F;
          }
        else if (offsetWIFI==true)
          {
                color = hue2rgb(inByte);  // velocity is used to HUE color selection and HUE is converted to RGB uint32 
                red = (uint8_t)(color >> 16);
                green = (uint8_t)(color >>  8);
                blue = (uint8_t)color;
                setPixelColor(remapSlim[GIR][ledNumber>>3][ledNumber%8], red, green, blue);
				// setPixelColor(remapSlim[GIR][ledNumber%8][ledNumber>>3], red, green, blue);
                refresh_led++;
                time_led = millis();
                offsetWIFI = false; 
          }
        else  offsetWIFI = false; 
      }
    }

	
///////////////////////////////////////////////////////////////
//////////////////////  TIMERS SETTINGS  //////////////////////
///////////////////////////////////////////////////////////////


  void BhorealSlim::serialRequests()
  {
//    sei();
    Bhoreal_.timer1Stop();
    wifiRead();
    Bhoreal_.timer1Initialize(); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)  
  }

  ISR(TIMER1_OVF_vect)
  {
    Bhoreal_.serialRequests();
  }
  
  #define RESOLUTION 65536    // Timer1 is 16 bit
  unsigned int pwmPeriod;
  unsigned char clockSelectBits;
  char oldSREG;					// To hold Status 
  
  void BhorealSlim::timer1SetPeriod(long microseconds)		// AR modified for atomic access
  {
  
    long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
    if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
    else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
    else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
    else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
    else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
    else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum
  
    oldSREG = SREG;				
    cli();							// Disable interrupts for 16 bit register access
    ICR1 = pwmPeriod = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
    SREG = oldSREG;
  
    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
    TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
  }
  
  void BhorealSlim::timer1Initialize()
  {
    TCCR1A = 0;                 // clear control register A 
    TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
    timer1SetPeriod(2000);     //Time in microseconds
    TIMSK1 = _BV(TOIE1);                                  
  }
  
  void BhorealSlim::timer1Stop()
  {
    TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));          // clears all clock selects bits
    TIMSK1 &= ~(_BV(TOIE1));     
  }

///////////////////////////////////////////////////////////////
///////////////////////  ICON CREATOR  ////////////////////////
///////////////////////////////////////////////////////////////

#if (MODEL == SLIMPRO) 

char icon[65] = {0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0};

void BhorealSlim::checkServer() {
  timer1Stop();
  protocolDefine(HTML + TCP);
  while ((mode==6)&&(WIFIMode==CLIENT))
    {
      boolean ok=false;
      uint8_t count = 0;
      byte retry=0;
      while ((retry<5)&&(mode!=0))
      {
       retry++;
       if ((EnterCommandMode())&&(mode!=0))
        {
          if ((open(WEB[0], 80))&&(mode!=0))
           {
             for(byte i = 1; i<4; i++) Serial1.print(WEB[i]); //Requests to the server time
//             while (Serial1.available()) Serial.write(Serial1.read());
             if ((FindInResponse("icon:", 1000))&&(mode!=0)) 
               {
                    byte offset = 0;
                    unsigned long time = millis();
                    while ((offset < 65)&&(mode!=0)) {
                      if (Serial1.available())
                      {
                        icon[offset] = Serial1.read();
                        time = millis();
                        
//                        Serial.print(icon[offset], DEC);
//                        Serial.print(' ');
                        if (icon[offset]>127) 
                          {
                            ok = false;
                            break;
                          }
                        else if ((offset==64)&&(icon[offset]=='#'))
                         {
                           ok = true;
                         }
                        offset++;  
//                        if ((offset%8)==0) Serial.println();
                      }
                      else if((millis()-time)>1000)
                      {
                        ok = false;
                        break; 
                      }
                    }
                   break;
                 }
             close();
           }
        }
      }
      if (retry>5) Ready();
      if (ok) printChar(icon);
  }
 protocolDefine(UDP);
 timer1Initialize();
}
#endif

///////////////////////////////////////////////////////////////
///////////////////////  MENU OPTIONS  ////////////////////////
///////////////////////////////////////////////////////////////

  void BhorealSlim::selectMode(boolean model) {
    
    if (model)  timer1Stop();
    unsigned long time_blink = millis();
    while (mode == 0)
      {
         checkAccel();
         checkMatrix(SELECTOR);
         for(int x = 0; x < modeMAX; ++x) setPixelColor(remapSlim[GIR][x>>3][x%8], 255, 0, 0);
         for(int x = modeMAX; x < NUM_LEDS; ++x) setPixelColor(remapSlim[GIR][x>>3][x%8], 0, 0, 0);
         setPixelColor(remapSlim[GIR][(mode_ant)>>3][(mode_ant)%8], 0, 255, 0);
         if (model) 
		 {
           if (charge_on) 
             {
               slaveSend(5); 
               setPixelColor(remapSlim[GIR][62>>3][62%8], 0, 255, 0);
             }
           else 
             {
               slaveSend(4);
               setPixelColor(remapSlim[GIR][62>>3][62%8], 255, 0, 0);
             }
           if (WIFIMode == AP)
            {
              if ((millis()-time_blink)<250) setPixelColor(remapSlim[GIR][63>>3][63%8], 255, 255, 0);
              else if ((millis()-time_blink)>=500) time_blink = millis();
              else  setPixelColor(remapSlim[GIR][63>>3][63%8], 0, 0, 0);
            }
           else if (WIFIMode == CLIENT)
            {
              if ((millis()-time_blink)<250) setPixelColor(remapSlim[GIR][63>>3][63%8], 0, 0, 255);
              else if ((millis()-time_blink)>=500) time_blink = millis();
              else  setPixelColor(remapSlim[GIR][63>>3][63%8], 0, 0, 0);
            }
           else if (WIFIMode == PROG_CLIENT)
            {
              setPixelColor(remapSlim[GIR][63>>3][63%8], 255, 0, 255);
              show();
              while (!reConnect());
              WIFIMode = CLIENT;
            }
           else if (WIFIMode == PROG_AP)
            {
              setPixelColor(remapSlim[GIR][63>>3][63%8], 255, 0, 255);
              show();
              apMode("Bhoreal", "", OPEN, "192.168.1.255", UDP, 8000, 9000);
              WIFIMode = AP;
            }
		 }
         show();
      }
	  if (model) timer1Initialize();
  }

void BhorealSlim::protocolDefine(byte protocol)
{
  if(EnterCommandMode())
	  {               
		  SendCommand(F("set ip proto "), true);
		  SendCommand(itoa(protocol));
		  //SendCommand(F("set u b "), true);
		  SendCommand(F("set uart instant "), true);
		  if (protocol==(TCP+HTML)) SendCommand(itoa(baud[1]));
		  else SendCommand(itoa(baud[0]));
//            SendCommand(F("save"), false, "Storing in config"); // Store settings
//            SendCommand(F("reboot"), false, "*READY*");
		  if (protocol==(TCP+HTML))
		   {
			 Serial1.begin(baud[1]);
			 Serial.println(baud[1]);
		   }
		  else 
			{
			  Serial1.begin(baud[0]);
			  Serial.println(baud[0]);
			}
		  ExitCommandMode();
	  }
}
