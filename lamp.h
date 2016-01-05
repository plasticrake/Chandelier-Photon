#ifndef LAMP_H
#define LAMP_H

#include "FastLED.h"
FASTLED_USING_NAMESPACE;

#include "Adafruit_TLC5947.h"
#include "Adafruit_DHT.h"
#include "Adafruit_TSL2561_U.h"

#define TLC_OE_PIN -1  // set to -1 to not use the enable pin (its optional)
#define NUM_LEDS 1

class Lamp {
 public:
  Lamp(Adafruit_TLC5947& tlc, Adafruit_TSL2561_Unified& tsl, DHT& dht);

  void begin(void);
  void show(void);

  uint16_t scaleChannel(uint8_t channel, uint8_t scale);

  void readSensors(void);

  void setBrightness(uint8_t value);
  void setColorCorrection(CRGB value);
  void setColorTemperature(CRGB value);
  void calcColorAdjustment(void);

  CRGB leds[NUM_LEDS];

  uint8_t _brightness = 255;
  CRGB _colorCorrection = Typical8mmPixel;
  CRGB _colorTemperature = UncorrectedTemperature;
  CRGB _colorAdjustment = Typical8mmPixel;
  String _colorHex;

  double _temperature = 0;
  double _humidity = 0;
  double _ambientLight = 0;

  Adafruit_TLC5947* _tlc;          // LED Driver
  Adafruit_TSL2561_Unified* _tsl;  // Light Sensor
  DHT* _dht;                       // Temp/Humidity Sensor
};

#endif
