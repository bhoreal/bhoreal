
 
#include <Arduino.h>
#include <Wire.h>
#include <avr/pgmspace.h>

#define MINI  0  //Tamaño de la matriz
#define MINISLIM  1  //Tamaño de la matriz
#define SLIM  2  //Tamaño de la matriz

#define  SERIAL_ENABLE     1
#define  MIDI_DEBUG        1
#define  SERIAL_DATA       0

#define TOOFULL 100

#define  MODEL  MINISLIM //Modelo
//#define  MODEL  SLIM //Modelo

class Bhoreal {
  public:  
    void begin(uint32_t BAUD);
    void checkButtons();
    void refresh();
    void midiRefresh();
    void startup();
    void checkADC();
    uint32_t hue2rgb(uint16_t hueValue);


  private:
    void on_press(byte r, byte c);
    void on_release(byte r, byte c);
    void timer1Initialize();
    void timer3Initialize();

};
