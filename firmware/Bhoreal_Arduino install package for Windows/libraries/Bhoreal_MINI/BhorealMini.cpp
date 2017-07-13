#include "bhorealMini.h"

// DEFINITION PINS FOR MINI

    #define PIN_LED 11  // ??????

    // Auxiliary analog output definitions
    #define ANALOG0 A5 // SLIDER POT MINI
    #define ANALOG1 A1 // ?????    
    boolean adc[2] = { 1, 0 }; // ON or OFF state of the analog inputs
    byte analogval[2]; //The Slider Pot Sensor
    byte tempADC; //Temporary storage for comparison purposes

    uint16_t NUM_ROWS = 4; 
    int NUM_LEDS   =  16;  
    const uint16_t numBytes = 48;
    byte row[4]    = {         // ROW pins for matrix pushbottons 
      13, 5, 10, 9};
    byte column[4] = {         // COL pins for matrix pushbottons
      8, 6, 12, 4};

BhorealMini Bhoreal_;
uint8_t pixels[numBytes];

// boolean pressed[4][4] = {      // pushbottons states matrix
  // {1,1,1,1},
  // {1,1,1,1},
  // {1,1,1,1},
  // {1,1,1,1}
// };

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

const byte remap[4][4] =    // mapping matrix for Mini
{
  { 3, 4, 11, 12 }, 
  { 2, 5, 10, 13 }, 
  { 1, 6,  9, 14 }, 
  { 0, 7,  8, 15 } 
};


//////////////////////////////////////////////////////////////////////
//////////////////////       BHOREAL BEGIN      //////////////////////
//////////////////////////////////////////////////////////////////////

void BhorealMini::begin()
{
    port = portOutputRegister(digitalPinToPort(PIN_LED));
    pinMask = digitalPinToBitMask(PIN_LED);
    for(byte i = 0; i<4; i++) 
    {
      pinMode(column[i], INPUT);
      pinMode(row[i], OUTPUT);
      digitalWrite(row[i], LOW);
    }
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);
    
    PORTE |= B01000000;
    DDRE  |= B01000000;
    PORTB |= B00000010;
    DDRB  |= B00000010;
	
    // Setup the timer interrupt
    timer1Initialize();   
}

////////////////////////////////////////////////////////////////
//////////////////////       STARTUP      //////////////////////
////////////////////////////////////////////////////////////////

// Run this animation once at startup. Currently unfinished.
void BhorealMini::startup(){
  
  for(int x = 0; x < NUM_LEDS; ++x){ 
      uint32_t c = hue2rgb((x+1)*8); // 128 HUE steps / 16 leds, 8 steps x led
      if (x == (NUM_LEDS-1)) c = hue2rgb(1);  // 128 HUE steps / 64 leds, 2 steps x led
      uint8_t
      r = (uint8_t)(c >> 16),
      g = (uint8_t)(c >>  8),
      b = (uint8_t)c;  
      setPixelColor(remap[x>>2][x%4], r, g, b);
  }  
  show();
}

//////////////////////////////////////////////////////////////////////
//////////////////////  MIDI PRESS & RELEASE  //////////////////////
//////////////////////////////////////////////////////////////////////

void BhorealMini::on_press(byte r, byte c){
    MIDIEvent e1 = { 0x09, 0x90, ((c << 2) | r) , 64  };
    MIDIUSB.write(e1);
}

void BhorealMini::on_release(byte r, byte c){
    MIDIEvent e1 = { 0x09, 0x90, ((c << 2) | r) , 0  };
    MIDIUSB.write(e1);
}


///////////////////////////////////////////////////////////////
//////////////////////   CHECK BUTTONS   //////////////////////
///////////////////////////////////////////////////////////////

byte count_column = 0;
byte count_file = 0;
unsigned long time_button = 0;

void BhorealMini::checkButtons(){
      if ((micros()-time_button)>1000)
        {
          time_button = micros();
          for(int r= NUM_ROWS - 1; r >= 0; r--)
          {
            if(pressed[count_column][r] != digitalRead(column[r]))
            { // read the state
              //delay(1); // to prevent bounces!!!
              pressed[count_column][r] = digitalRead(column[r]);
              if(pressed[count_column][r]) on_press(count_column, r);
              else on_release(count_column, r);
            }
          }
          digitalWrite(row[count_column],LOW);
          count_column++;
          if (count_column>NUM_ROWS) count_column=0;
          digitalWrite(row[count_column],HIGH);
          
        }
}

////////////////////////////////////////////////////////////////
//////////////////////    REFRESH LED     //////////////////////
////////////////////////////////////////////////////////////////
byte refresh_led = 0;
unsigned long time_led = 0;
void BhorealMini::displayRefresh(){ 
  if (((refresh_led>0)&&((micros()-time_led)>1000))||(refresh_led>=NUM_LEDS)||((refresh_led<NUM_LEDS)&&((micros()-time_led)>25000))&&(refresh_led>0))
    {
      refresh_led=0;
      show();
    }
}

////////////////////////////////////////////////////////////////
////////////////////// REFRESH MIDI & LED  /////////////////////
////////////////////////////////////////////////////////////////

  void BhorealMini::midiRefresh(){ 
      while(MIDIUSB.available() > 0) 
      {
        MIDIEvent e;
        e = MIDIUSB.read();
            if((e.type == 0x09) && (e.m3))  //  NoteON midi message with vel > 0
            {  
              uint32_t c = hue2rgb(e.m3);
              uint8_t
                r = (uint8_t)(c >> 16),
                g = (uint8_t)(c >>  8),
                b = (uint8_t)c;
              setPixelColor(remap[e.m2>>2][e.m2%4], r, g, b);
              refresh_led++;
              time_led = micros();
            }
            else if( (e.type == 0x08) || ((e.type == 0x09) && !e.m3) ) // NoteOFF midi message
            {  
              setPixelColor(remap[e.m2>>2][e.m2%4], 0, 0, 0);
              refresh_led++;
              time_led = micros();
            }
          }
  }


////////////////////////////////////////////////////////////////
//////////////////////  CHECK ADC INPUTS  //////////////////////
////////////////////////////////////////////////////////////////

void BhorealMini::checkADC(){
        // For all of the ADC's which are activated, check if the analog value has changed,
        // and send a message if it has.
        if(adc[0]){
          tempADC = (analogRead(ANALOG0) >> 2);
          if(abs((int)analogval[0] - (int)tempADC) > 2 ){
          //if(analogval[0] != (analogRead(ANALOG0) >> 2)){
            //analogval[0] = (analogRead(ANALOG0) >> 2);
            analogval[0] = tempADC;
            // Send the control change message for the slider potentiometer,
            // by defect we use CC64 controller
            MIDIEvent e1 = {0x0B, 0xB0, 64, analogval[0]>>1};
            MIDIUSB.write(e1);
          }
        }
        
        if(adc[1]){
          if(analogval[1] != (analogRead(ANALOG1) >> 2)){
            analogval[1] = (analogRead(ANALOG1) >> 2);
          }
        }
}

///////////////////////////////////////////////////////////////
//////////////////////  TIMERS SETTINGS  //////////////////////
///////////////////////////////////////////////////////////////

#define RESOLUTION 65535    // Timer1 is 16 bit


void BhorealMini::timer1SetPeriod(long microseconds)		// AR modified for atomic access
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


void BhorealMini::timer1Initialize()
{
  TCCR1A = 0;                 // clear control register A 
  TCCR1B = _BV(WGM13);        // set mode 8: phase and frequency correct pwm, stop the timer
  timer1SetPeriod(5);         // Time interruption in ms
  TIMSK1 = _BV(TOIE1);                                  
}
 
ISR(TIMER1_OVF_vect)
{
       if (PORTE&B01000000) 
         {
           PORTB |= B00000010;
           PORTE &= B10111111;
         }
       else 
         {
           PORTE |= B01000000;
           PORTB &= B11111101;
         }
}


///////////////////////////////////////////////////////////////
//////////////////////     HUE -> RGB    //////////////////////
///////////////////////////////////////////////////////////////

  uint8_t rh;
  uint8_t gh;
  uint8_t bh;
  
uint32_t BhorealMini::hue2rgb(uint16_t hueValue)
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
//////////////////////    Control led    //////////////////////
///////////////////////////////////////////////////////////////  

void BhorealMini::show(void) {

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
void BhorealMini::setPixelColor(
 uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
  if(n < NUM_LEDS) {
    uint8_t *p = &pixels[n * 3];
    *p++ = g;
    *p++ = r; 
    *p = b;
  }
}