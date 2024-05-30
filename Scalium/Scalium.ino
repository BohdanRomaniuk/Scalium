#include <FastLED.h>

#define STEPS_COUNT 14
#define LED_COUNT_PER_STEP 36
#define START_EFFECT RAINBOW

#define BOTTOM_SENSOR_PIN 23
#define UPPER_SENSOR_PIN 27
#define LED_PIN 22
#define BRIGHTNESS 255
#define NIGHT_BRIGHTNESS 100

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
#define VOLTAGE 5
#define CURRENT_LIMIT 3000

#define TIMEOUT 15 

const int ledCount = STEPS_COUNT*LED_COUNT_PER_STEP;

CRGB leds[ledCount];
CLEDController *strip;

struct PirSensor {
  int8_t effectDirection;
  int8_t pin;
  bool lastState;
};

PirSensor startPirSensor = { 1, UPPER_SENSOR_PIN, false};
PirSensor endPirSensor = { -1, BOTTOM_SENSOR_PIN, false};

uint32_t timeoutCounter;
bool systemIdleState;
bool systemOffState;
int8_t effectDirection;
int effSpeed;
enum {COLOR, RAINBOW, FIRE, EFFECTS_AMOUNT} curEffect = START_EFFECT;

void setup() {
  Serial.begin(9600);
  initStrip();
  
}

void loop() {
  handlePirSensor(&startPirSensor);
  handlePirSensor(&endPirSensor);
  if (systemIdleState || systemOffState) {
    delay(50);
  } else {
    effectFlow();
    handleTimeout();
  }
}

void handlePirSensor(PirSensor *sensor) {
  if (systemOffState)
  {
    return;
  }

  int newState = digitalRead(sensor->pin);
  if (newState && !sensor->lastState) {
    Serial.print("PIR sensor ");
    Serial.println(sensor->pin);
    timeoutCounter = millis();
    if (systemIdleState) {
      effectDirection = sensor->effectDirection;
      // if (ROTATE_EFFECTS) {
      //   curEffect = ++effectCounter % EFFECTS_AMOUNT;
      // }
      //stepFader(effectDirection == 1 ? 0 : 1,  0);
      systemIdleState = false;
    }
  }
  sensor->lastState = newState;
}

void handleTimeout() {
  if (millis() - timeoutCounter >= (TIMEOUT * 1000L)) {
    systemIdleState = true;
    Serial.print("TIMEOUt");
    if (effectDirection == 1) {
      //stepFader(0, 1);
    } else {
      //stepFader(1, 1);
    }
  }
}

void effectFlow() {
  static uint32_t tmr;
  if (millis() - tmr >= effSpeed) {
    tmr = millis();
    switch (curEffect) {
      //case COLOR: staticColor(effectDirection, 0, STEP_AMOUNT); break;
      //case RAINBOW: rainbowStripes(-effectDirection, 0, STEP_AMOUNT); break; 
    }
    show();
  }
}

void initStrip() {
  strip = &FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, ledCount).setCorrection(TypicalLEDStrip);
  setBrightness(BRIGHTNESS);
  FastLED.setBrightness(BRIGHTNESS);
  if (CURRENT_LIMIT > 0)
  {
    FastLED.setMaxPowerInVoltsAndMilliamps(VOLTAGE, CURRENT_LIMIT);
  }
  strip->setLeds(leds, ledCount);
  clear();
}

void setBrightness(int brightness) {
  FastLED.setBrightness(brightness);
}

void show() {
  strip->showLeds();
}

void clear() {
  strip->clearLedData();
}
