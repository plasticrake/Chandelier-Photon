#include "FastLED.h"
FASTLED_USING_NAMESPACE;

#include "Adafruit_TLC5947.h"
#include "Adafruit_DHT.h"
#include "Adafruit_TSL2561_U.h"

#include "lamp.h"

#define NUM_TLC5974 1
#define TLC_DATA_PIN D4
#define TLC_CLOCK_PIN D5
#define TLC_LATCH_PIN D6
#define TLC_OE_PIN -1  // set to -1 to not use the enable pin (its optional)

#define DHT_PIN D2
#define DHTTYPE DHT11

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5974, TLC_CLOCK_PIN, TLC_DATA_PIN, TLC_LATCH_PIN);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);  // SDA=D0 SCL=D1
DHT dht = DHT(DHT_PIN, DHTTYPE);

Lamp lamp = Lamp(tlc, tsl, dht);

void setup() {
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);

  Particle.publish("Begin Setup");

  lamp.begin();

  Particle.variable("temp", lamp._temperature);
  Particle.variable("humidity", lamp._humidity);
  Particle.variable("ambientLight", lamp._ambientLight);
  Particle.variable("colorHex", lamp._colorHex);
  //Particle.variable("colorRed", colorRed);
  //Particle.variable("colorGreen", colorGreen);
  //Particle.variable("colorBlue", colorBlue);
  //Particle.variable("colorRgb", colorRgb);

  Particle.function("setColor", setColor);

  Particle.publish("End Setup");
}

void loop() {
  EVERY_N_SECONDS(1) {
    lamp.readSensors();
  }

  EVERY_N_SECONDS(1) {
    lamp.show();
  }
}

int setColor(String command) {
  Particle.publish("setColor", command);
  if (command.charAt(0) == '#') {
    lamp._colorHex = command.substring(1);
    char colorChar[6];
    lamp._colorHex.toCharArray(colorChar, 7);

    char* ps = colorChar;
    ps++;  // skip #

    lamp.leds[0] = strtol(colorChar, NULL, 16);

    return 1;
  } else {
    return -1;
  }
}
