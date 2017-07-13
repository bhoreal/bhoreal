#include <Arduino.h>

class BhorealMini {
  public:  
  
    // Constructor: number of LEDs, pin number, LED type
    void show(void);
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    
    void begin();
    void startup();
    void checkButtons();
    void displayRefresh();
    void midiRefresh();

    void checkADC();

    
  private:
    uint32_t hue2rgb(uint16_t hueValue);
    
    void timer1Initialize();
    void timer1SetPeriod(long microseconds);
    void timer1Stop();
    void serialRequests();

    //Variables for WS2812
    uint32_t endTime;                     // Latch timing reference
    const volatile uint8_t *port;         // Output PORT register
    uint8_t pinMask;                // Output PORT bitmask    
    //Variables for timer1
    unsigned int pwmPeriod;
    unsigned char clockSelectBits;
    char oldSREG;                   // To hold Status 

    void on_press(byte r, byte c);
    void on_release(byte r, byte c);

};
