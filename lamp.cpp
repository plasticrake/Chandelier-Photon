#include "lamp.h"

Lamp::Lamp(Adafruit_TLC5947& tlc, Adafruit_TSL2561_Unified& tsl, DHT& dht) {
  _tlc = &tlc;
  _tsl = &tsl;
  _dht = &dht;
}

void Lamp::begin() {
  _dht->begin();

  if (!_tsl->begin()) {
    Particle.publish("No TSL2561 detected");
  }
  if (!_tlc->begin()) {
    Particle.publish("No TLC5947 detected");
  }
  if (TLC_OE_PIN >= 0) {
    pinMode(TLC_OE_PIN, OUTPUT);
    digitalWrite(TLC_OE_PIN, LOW);
  }

  _tsl->setGain(TSL2561_GAIN_1X);
  _tsl->setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
}

void Lamp::calcColorAdjustment() {
  CRGB adj(0, 0, 0);

  if (_brightness > 0) {
    for (uint8_t i = 0; i < 3; i++) {
      uint8_t cc = _colorCorrection.raw[i];
      uint8_t ct = _colorTemperature.raw[i];
      if (cc > 0 && ct > 0) {
        uint32_t work = (((uint32_t)cc) + 1) * (((uint32_t)ct) + 1) * _brightness;
        work /= 0x10000L;
        adj.raw[i] = work & 0xFF;
      }
    }
  }

  _colorAdjustment = adj;
}

void Lamp::setBrightness(uint8_t value) {
  _brightness = value;
  calcColorAdjustment();
}

void Lamp::setColorCorrection(CRGB value) {
  _colorCorrection = value;
  calcColorAdjustment();
}

void Lamp::setColorTemperature(CRGB value) {
  _colorTemperature = value;
  calcColorAdjustment();
}

void Lamp::readSensors() {
  sensors_event_t event;
  _tsl->getEvent(&event);

  if (event.light) {
    _ambientLight = event.light;
  } else {
    /* If event.light = 0 lux the sensor is probably saturated and no reliable data could be generated! */
  }

  _humidity = _dht->getHumidity();
  _temperature = _dht->getTempCelcius();
}

void Lamp::show() {
  _tlc->setLED(0, scaleChannel(leds[0].r, _colorAdjustment.r), scaleChannel(leds[0].g, _colorAdjustment.g), scaleChannel(leds[0].b, _colorAdjustment.b));
  _tlc->write();
}

uint16_t Lamp::scaleChannel(uint8_t channel, uint8_t scale) {
  uint16_t result;
  result = scale8(channel, scale);
  result = result * 4095 / 255;
  return result;
}
