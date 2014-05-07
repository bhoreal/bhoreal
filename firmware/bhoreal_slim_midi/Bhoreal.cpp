
#include "bhoreal.h"
#include "Adafruit_NeoPixel.h"

byte tempR;
byte tempC;
byte lastread;
byte command = 0;
boolean ready = true;
boolean refresh_ok = false;
uint16_t IntensityMAX = 255;

// Default draw colour. Each channel can be between 0 and 255.
int red = 0;
int green = 0;
int blue = 0;

// Auxiliary analog output definitions
#define ANALOG0 A5 // SLIDER POT MINI
#define ANALOG1 A1
boolean adc[2] = { // On or off state analog inputs
  1, 0};
byte analogval[2]; //The last reported value
byte tempADC; //Temporary storage for comparison purposes

  #if MODEL == SLIM
    // Pin definitions for the 74HC164 SIPO shift register (drives button rows high)
    #define DATAPIN     9 // aka analog pin 2 (what, you didn't know that analog pins 0-5 are also digital pins 14-19? Well, now you do!)
    #define CLOCKPIN    8
    // Pin definitions for the 74HC165 PISO shift register (reads button column state)
    #define INDATAPIN  13
    #define INCLOCKPIN  5
    #define INLOADPIN  10 // toggling this tell the 165 to read the value into its memory for reading
    
    #define AWAKE  18 // AWAKE WIFLY
    #define DTR 11
    #define MUX 12
    #define BOT 7  // sleep pushbotom
    
    uint16_t MAX   =  8; 
    int NUM_LEDS   =  64; 
    #define PIN 6
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
  #else
    uint16_t MAX   =  4; 
    int NUM_LEDS   =  16;  
    #define PIN 11
    Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
    byte row[4]    = {         // ROW pins for matrix pushbottons 
      13, 5, 10, 9};
    byte column[4] = {         // COL pins for matrix pushbottons
      8, 6, 12, 4};
  #endif
  
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

byte remapSlim[8][8] = {             // mapping matrix for Slim
  {7,8,23,24,       39,40,55,56}, 
  {6,9,22,25,       38,41,54,57}, 
  {5,10,21,26,      37,42,53,58}, 
  {4,11,20,27,      36,43,52,59}, 
  {3,12,19,28,      35,44,51,60}, 
  {2,13,18,29,      34,45,50,61}, 
  {1,14,17,30,      33,46,49,62}, 
  {0,15,16,31,      32,47,48,63}, 
};

int levelR[64] = {                  // Red Led intensity matrix
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};
   
int levelG[64] = {                   // Green Led intensity matrix
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};
   
int levelB[64] = {                    // Blue Led intensity matrix
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0};


//////////////////////////////////////////////////////////////////////
//////////////////////       BHOREAL BEGIN      //////////////////////
//////////////////////////////////////////////////////////////////////


void Bhoreal::begin(uint32_t BAUD)
{
  #if MODEL == SLIM
     // 165 Setup
    pinMode(INDATAPIN, INPUT);
    pinMode(INCLOCKPIN, OUTPUT);
    pinMode(INLOADPIN, OUTPUT);
  
    // 164 Setup
    pinMode(DATAPIN, OUTPUT);
    pinMode(CLOCKPIN, OUTPUT);
    
    pinMode(DTR, OUTPUT);
    pinMode(MUX, OUTPUT);
    pinMode(AWAKE, OUTPUT);
    pinMode(BOT, INPUT); //Sleep Bottom
    digitalWrite(AWAKE, LOW); 
    digitalWrite(MUX, HIGH); //Modo Wifly ON
    digitalWrite(DTR, HIGH); //Reset atmega328 OFF
    
    strip.begin(); // Initialization of led matrix
    strip.show();
    
  #else
    for(byte i = 0; i<4; i++) 
    {
      pinMode(column[i], INPUT);
      pinMode(row[i], OUTPUT);
      digitalWrite(row[i], LOW);
    }
    // Start the serial port
    #if SERIAL_ENABLE
      Serial.begin(BAUD);
    #endif
    
    strip.begin(); // Initialization of led matrix
    strip.show();
    
    // Setup the timer interrupt
    PORTE |= B01000000;
    DDRE  |= B01000000;
  
    timer1Initialize();
    //timer3Initialize(); // Disable Serial interrupt!
    
  #endif

}



////////////////////////////////////////////////////////////////
//////////////////////       STARTUP      //////////////////////
////////////////////////////////////////////////////////////////

// Run this animation once at startup. Currently unfinished.
void Bhoreal::startup(){
  
  for(int x = 0; x < NUM_LEDS; ++x){ 
    #if MODEL == SLIM
      uint32_t c = hue2rgb(x*2);  // 128 HUE steps / 64 leds, 2 steps x led
      uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
      levelG[remapSlim[x>>3][x%8]] = g;
      levelB[remapSlim[x>>3][x%8]] = b;
    #else
      uint32_t c = hue2rgb(x*8); // 128 HUE steps / 16 leds, 8 steps x led
      uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;
      levelR[remapMini[x>>2][x%4]] = r;
      levelG[remapMini[x>>2][x%4]] = g;
      levelB[remapMini[x>>2][x%4]] = b;
    #endif
  }  
    
  for(int x = 0; x < NUM_LEDS; ++x) strip.setPixelColor(x, levelR[x], levelG[x], levelB[x]);
//  for(int x = 0; x < NUM_LEDS; ++x) strip.setPixelColor(x, 255, 255, 255);
  strip.show();
  
}

//////////////////////////////////////////////////////////////////////
//////////////////////  SERIAL PRESS & RELEASE  //////////////////////
//////////////////////////////////////////////////////////////////////


void Bhoreal::on_press(byte r, byte c){

#if SERIAL_DATA
  Serial.print(1);
  Serial.print(" ");
  Serial.println( (r << 4) | c, HEX);
#endif
  #if MODEL == SLIM
    MIDIEvent e1 = { 0x09, 0x90, ((r << 3) | c) , 64  };
  #else
    MIDIEvent e1 = { 0x09, 0x90, ((r << 2) | c) , 64  };
  #endif
  MIDIUSB.write(e1);

}

void Bhoreal::on_release(byte r, byte c){
  
#if SERIAL_DATA
  Serial.print(0);
  Serial.print(" ");
  Serial.println( (r << 4) | c, HEX); 
#endif
  #if MODEL == SLIM
    MIDIEvent e1 = { 0x09, 0x90, ((r << 3) | c) , 0  };
  #else
    MIDIEvent e1 = { 0x09, 0x90, ((r << 2) | c) , 0  };
  #endif

  MIDIUSB.write(e1);

}



///////////////////////////////////////////////////////////////
//////////////////////   CHECK BUTTONS   //////////////////////
///////////////////////////////////////////////////////////////


void Bhoreal::checkButtons(){
  #if MODEL == SLIM
    if (digitalRead(BOT))
    {
       Serial.println("OFF"); 
       for(int x = 0; x < NUM_LEDS; ++x){ 
          levelR[x] = 0;
          levelG[x] = 0;
          levelB[x] = 0;
      }  
      for(int x = 0; x < NUM_LEDS; ++x) strip.setPixelColor(x, levelR[x], levelG[x], levelB[x]);
      strip.show();
    }
    digitalWrite(CLOCKPIN,LOW);
    digitalWrite(DATAPIN, HIGH);
    
     for(byte c = 0; c < MAX; c++){
      digitalWrite(CLOCKPIN, HIGH);
  
      digitalWrite(INLOADPIN, LOW); // read into register
      digitalWrite(INLOADPIN, HIGH); // done reading into register, ready for us to read
  
      for(int r= MAX/2; r < MAX; r++){ // read each of the 165's 8 inputs (or its snapshot of it rather)
  
        // tell the 165 to send the first inputs pin state
        digitalWrite(INCLOCKPIN, LOW);
        // read the current output
  
        if(pressed[c][r] != digitalRead(INDATAPIN)){ // read the state
          pressed[c][r] = digitalRead(INDATAPIN);
          if(!pressed[c][r]){
            on_press(c, r);
          }
          else {
            on_release(c, r);
          }
        }
      // tell the 165 we are done reading the state, the next inclockpin=0 will output the next input value
      digitalWrite(INCLOCKPIN, 1);
      }
      
      for(int r= 0; r < MAX/2; r++){ // read each of the 165's 8 inputs (or its snapshot of it rather)
  
        // tell the 165 to send the first inputs pin state
        digitalWrite(INCLOCKPIN, LOW);
        // read the current output
  
        if(pressed[c][r] != digitalRead(INDATAPIN)){ // read the state
          pressed[c][r] = digitalRead(INDATAPIN);
          if(!pressed[c][r]){
            on_press(c, r);
          }
          else {
            on_release(c, r);
          }
        }
      // tell the 165 we are done reading the state, the next inclockpin=0 will output the next input value
      digitalWrite(INCLOCKPIN, 1);
      }
      digitalWrite(CLOCKPIN, LOW);
      digitalWrite(DATAPIN, LOW);
      
  
     }
  #else
    for(byte c = 0; c < MAX; c++)
    { 
      digitalWrite(row[c],HIGH);
      for(int r= MAX - 1; r >= 0; r--)
      {
        if(pressed[c][r] != digitalRead(column[r]))
        { // read the state
          delay(1); // to prevent bounces!!!
          pressed[c][r] = digitalRead(column[r]);
          if(pressed[c][r]) on_press(c, r);
          else on_release(c, r);
        }
      }
      digitalWrite(row[c],LOW);
    } 
  #endif
}



////////////////////////////////////////////////////////////////
//////////////////////    REFRESH LED     //////////////////////
////////////////////////////////////////////////////////////////

unsigned long time = 0;

void Bhoreal::refresh(){ 
  if (refresh_ok) 
  {
    strip.show();
    refresh_ok=false;
  }
  // if ((millis() - time)>=100) 
  //   {
  //     strip.show();
  //     time = millis();
  //   }

}



////////////////////////////////////////////////////////////////
////////////////////// REFRESH MIDI & LED  /////////////////////
////////////////////////////////////////////////////////////////


void Bhoreal::midiRefresh(){ 
  while(MIDIUSB.available() > 0) 
  {
    MIDIEvent e;
    e = MIDIUSB.read();

#if SERIAL_ENABLE
    if(MIDI_DEBUG)
    {
      if(e.type != 0x0F) // timestamp 1 BYTE
      {
        Serial.print("Midi Packet: ");
        Serial.print(e.type);
        Serial.print("\t");
        Serial.print(e.m1);
        Serial.print("\t");
        Serial.print(e.m2);
        Serial.print("\t");
        Serial.println(e.m3);
      }
    }
#endif
#if MODEL == SLIM
    if((e.type == 0x09) && (e.m3))  // NoteON midi message with vel > 0
    {  
      uint32_t c = hue2rgb(e.m3);  // velocity is used to HUE color selection and HUE is converted to RGB uint32 
      uint8_t
        r = (uint8_t)(c >> 16),
        g = (uint8_t)(c >>  8),
        b = (uint8_t)c;

      strip.setPixelColor(remapSlim[e.m2>>3][e.m2%8], r, g, b);
      strip.show();
    }
    else if( (e.type == 0x08) || ((e.type == 0x09) && !e.m3) ) // NoteOFF midi message
    {  
      strip.setPixelColor(remapSlim[e.m2>>3][e.m2%8], 0, 0, 0);
      strip.show();
    }  
#else
    if((e.type == 0x09) && (e.m3))  //  NoteON midi message with vel > 0
    {  
      uint32_t c = hue2rgb(e.m3);
      uint8_t
        r = (uint8_t)(c >> 16),
        g = (uint8_t)(c >>  8),
        b = (uint8_t)c;

      strip.setPixelColor(remapMini[e.m2>>2][e.m2%4], r, g, b);
      strip.show();
    }
    else if( (e.type == 0x08) || ((e.type == 0x09) && !e.m3) ) // NoteOFF midi message
    {  
      strip.setPixelColor(remapMini[e.m2>>2][e.m2%4], 0, 0, 0);
      strip.show();
    }
#endif
    MIDIUSB.flush(); // delete it???
  
  }
}

////////////////////////////////////////////////////////////////
//////////////////////  CHECK ADC INPUTS  //////////////////////
////////////////////////////////////////////////////////////////


void Bhoreal::checkADC(){
  // For all of the ADC's which are activated, check if the analog value has changed,
  // and send a message if it has.
  if(adc[0]){
    tempADC = (analogRead(ANALOG0) >> 2);
    if(abs((int)analogval[0] - (int)tempADC) > 2 ){
    //if(analogval[0] != (analogRead(ANALOG0) >> 2)){
      //analogval[0] = (analogRead(ANALOG0) >> 2);
      analogval[0] = tempADC;
#if SERIAL_DATA
      Serial.write(14 << 4);
      Serial.write(analogval[0]);
#endif
      // Send the control change message for the slider potentiometer,
      // by defect we use CC64 controller
      MIDIEvent e1 = {0x0B, 0xB0, 64, analogval[0]>>1};
      MIDIUSB.write(e1);
    }
  }
  
  if(adc[1]){
    if(analogval[1] != (analogRead(ANALOG1) >> 2)){
      analogval[1] = (analogRead(ANALOG1) >> 2);
#if SERIAL_DATA
      Serial.write(14 << 4 | 1);
      Serial.write(analogval[1]);
#endif
    }
  }
}


////////////////////////////////////////////////////////////////
//////////////////////  SERIAL INTERRUPT  //////////////////////
////////////////////////////////////////////////////////////////


//boolean flag = true;


/*TIMER*/
ISR(TIMER3_OVF_vect)
{
  cli();
  do{ // This do ensures that the data is always parsed at least once per cycle
#if SERIAL_DATA
    if(Serial.available()){
      if(ready){ // If the last command has finished executing, read in the next command and reset the command flag
        command = Serial.read();
        ready = false;
      }
      switch (command >> 4) { //Execute the appropriate command, but only if we have received enough bytes to complete it. We might one day add "partial completion" for long command strings.
      case 1: // set colour
        if( Serial.available() > 2 ) {
          red = Serial.read();
          green = Serial.read();
          blue = Serial.read();
          ready=true;
        }
        break;
      case 2: // led_on
        if( Serial.available() ) {
          lastread = Serial.read();
          tempR = lastread >> 4;
          tempC = lastread & B1111;
          if ((tempR < MAX)&&(tempC < MAX))
          {
            levelR[remapMini[tempC][tempR]] = red;
            levelG[remapMini[tempC][tempR]] = green;
            levelB[remapMini[tempC][tempR]] = blue;
            strip.setPixelColor(remapMini[tempC][tempR], red, green, blue);
            refresh_ok=true;
          }
          ready = true;
        }

        break;
      case 3: // led_off
        if( Serial.available() ) {
          lastread = Serial.read();
          tempR = lastread >> 4;
          tempC = lastread & B1111;
          if ((tempR < MAX)&&(tempC < MAX))
          {
            levelR[remapMini[tempC][tempR]] = 0;
            levelG[remapMini[tempC][tempR]] = 0;
            levelB[remapMini[tempC][tempR]] = 0;
            strip.setPixelColor(remapMini[tempC][tempR], 0, 0, 0);
            refresh_ok=true;
          }
          ready = true;
        }
        break;
      case 4: // led_row1
        if( Serial.available() ) {
          tempR = command & B1111;
          lastread = Serial.read();
          if (tempR < MAX)
          {
            for(tempC = 0; tempC < MAX; ++tempC){
              if(lastread & (1 << tempC) ){
                levelR[remapMini[tempR][tempC]] = red;
                levelG[remapMini[tempR][tempC]] = green;
                levelB[remapMini[tempR][tempC]] = blue;
                strip.setPixelColor(remapMini[tempR][tempC], red, green, blue);
              }
              else {
                levelR[remapMini[tempR][tempC]] = 0;
                levelG[remapMini[tempR][tempC]] = 0;
                levelB[remapMini[tempR][tempC]] = 0;
                strip.setPixelColor(remapMini[tempR][tempC], 0, 0, 0);
              }
            }
          }
          refresh_ok=true;
          ready = true;
        }
        break;
      case 5: // led_col1
        if( Serial.available() ) {
          tempC = command & B1111;
          lastread = Serial.read();
          if (tempC < MAX)
          {
            for(tempR = 0; tempR < MAX; ++tempR){
              if(lastread & (1 << tempR) ){
                levelR[remapMini[tempR][tempC]] = red;
                levelG[remapMini[tempR][tempC]] = green;
                levelB[remapMini[tempR][tempC]] = blue;
                strip.setPixelColor(remapMini[tempR][tempC], red, green, blue);
              }
              else {
                levelR[remapMini[tempR][tempC]] = 0;
                levelG[remapMini[tempR][tempC]] = 0;
                levelB[remapMini[tempR][tempC]] = 0;
                strip.setPixelColor(remapMini[tempR][tempC], 0, 0, 0);
              }
            }
          }
          refresh_ok=true;
          ready = true;
        }
        break;
      case 8: //frame
        if( Serial.available() > 7 ) {

          for(tempR=0; tempR < MAX; ++tempR){
            lastread = Serial.read();
            for(tempC = 0; tempC < MAX; ++tempC){
              if(lastread & (1 << tempC) ){
                levelR[remapMini[tempR][tempC]] = red;
                levelG[remapMini[tempR][tempC]] = green;
                levelB[remapMini[tempR][tempC]] = blue;
                strip.setPixelColor(remapMini[tempR][tempC], red, green, blue);
              }
              else {
                levelR[remapMini[tempR][tempC]] = 0;
                levelG[remapMini[tempR][tempC]] = 0;
                levelB[remapMini[tempR][tempC]] = 0;
                strip.setPixelColor(remapMini[tempR][tempC], 0, 0, 0);
              }
            }
          }
          refresh_ok=true;
          ready = true;
        }
        break;
      case 9: // FULL COLOR OR CLEAR
        if(command & 1){
          byte TEMPMAX = MAX*MAX;
          for(int x = 0; x< TEMPMAX;++x){
            levelR[x] = red;
            levelG[x] = green;
            levelB[x] = blue;
            strip.setPixelColor(x, red, green, blue);
          }
        }
        else{
          byte TEMPMAX = MAX*MAX;
          for(int x = 0; x< TEMPMAX;++x){
            levelR[x] = 0;
            levelG[x] = 0;
            levelB[x] = 0;
            strip.setPixelColor(x, 0, 0, 0);
          }
        }
        refresh_ok=true;
        ready = true;
        break;
      case 12:
        switch(command & 15){
        case 0:
          adc[0] = true;
          analogval[0] = (analogRead(ANALOG0) >> 2);
          Serial.write(14 << 4);
          Serial.write(analogval[0]);
          break;
        case 1:
          adc[1] = true;
          analogval[1] = (analogRead(ANALOG1) >> 2);
          Serial.write(14 << 4 | 1);
          Serial.write(analogval[1]);
          break;
        default:
          break;
        }
        ready = true;
        break;
      case 13:
        adc[command & 15] = false;
        ready = true;
        break;
      default:
        break;
      }
    }
#endif  
  }
  // If the serial buffer is getting too close to full, keep executing the parsing until it falls below a given level
  // This might cause flicker, or even dropped messages, but it should prevent a crash.
  while (Serial.available() > TOOFULL);
  strip.show();
  sei();
}



///////////////////////////////////////////////////////////////
//////////////////////  TIMERS SETTINGS  //////////////////////
///////////////////////////////////////////////////////////////



#define RESOLUTION 65535    // Timer1 is 16 bit
unsigned int pwmPeriod;
unsigned char clockSelectBits;
char oldSREG;					// To hold Status 

void setPeriodTimer1(long microseconds)		// AR modified for atomic access
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
  ICR1 = pwmPeriod = cycles;                                    // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));
  TCCR1B |= clockSelectBits;                                    // reset clock select register, and starts the clock
}


void Bhoreal::timer1Initialize()
{
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  setPeriodTimer1(5);         // Time interruption in ms
  TIMSK1 = _BV(TOIE1);                                  
}
 
boolean flag = false;
 
ISR(TIMER1_OVF_vect)
{
       if (flag) { PORTE |= B01000000; flag=0;}
       else if (!flag) { PORTE &= B10111111; flag=1;}
 
}


void setPeriodTimer3(long microseconds)		// AR modified for atomic access
{

  long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
  if(cycles < RESOLUTION)              clockSelectBits = _BV(CS30);              // no prescale, full xtal
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS31);              // prescale by /8
  else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS31) | _BV(CS30);  // prescale by /64
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS32);              // prescale by /256
  else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS32) | _BV(CS30);  // prescale by /1024
  else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS32) | _BV(CS30);  // request was out of bounds, set as maximum

  oldSREG = SREG;				
  cli();							// Disable interrupts for 16 bit register access
  ICR3 = pwmPeriod = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
  SREG = oldSREG;

  TCCR3B &= ~(_BV(CS30) | _BV(CS31) | _BV(CS32));
  TCCR3B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}

void Bhoreal::timer3Initialize()
{
  TCCR3A = 0;                 // clear control register A 
  TCCR3B = _BV(WGM33);        // set mode 8: phase and frequency correct pwm, stop the timer
  setPeriodTimer3(10000); 
  TIMSK3 = _BV(TOIE3);    

  //    TCCR3A = 0;
  //    TCCR3B = 0<<CS32 | 0<<CS31 | 1<<CS30;
  //    //Timer1 Overflow Interrupt Enable
  //    TIMSK3 = 1<<TOIE3;
}


///////////////////////////////////////////////////////////////
//////////////////////     HUE -> RGB    //////////////////////
///////////////////////////////////////////////////////////////

uint32_t Bhoreal::hue2rgb(uint16_t hueValue)
{
  
  uint8_t r;
  uint8_t g;
  uint8_t b;
  hueValue<<= 3;  // 128 midi steps -> 1024 hue steps
  
  if (hueValue < 341)  { // Lowest third of the potentiometer's range (0-340)
    hueValue = (hueValue * 3) / 4; // Normalize to 0-255

    r = 255 - hueValue;  // Red from full to off
    g = hueValue;        // Green from off to full
    b = 1;               // Blue off
  }
  else if (hueValue < 682) { // Middle third of potentiometer's range (341-681)
    hueValue = ( (hueValue-341) * 3) / 4; // Normalize to 0-255

    r = 1;              // Red off
    g = 255 - hueValue; // Green from full to off
    b = hueValue;       // Blue from off to full
  }
  else  { // Upper third of potentiometer"s range (682-1023)
    hueValue = ( (hueValue-683) * 3) / 4; // Normalize to 0-255

    r = hueValue;       // Red from off to full
    g = 1;              // Green off
    b = 255 - hueValue; // Blue from full to off
  }
  
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;

}

