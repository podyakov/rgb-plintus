/* ====================================================
 * super parket v.3
 * ====================================================
 */

// INCLUDES

#include <SPI.h>
#include "FastLED.h"

// HARWARE CONFIG

#define WS_PIN     2
#define WS_LEDS    400

// DESIGN CONFIG

#define SEGMENTS   14

int segments[SEGMENTS] = {
  0,
  20,
  40,
  60,
  80,
  100,
  120,
  140,
  160,
  180,
  200,
  220,
  240,
  WS_LEDS
};

// PRIVATE

#define CHANNELS SEGMENTS * 3
CRGB wsLeds[WS_LEDS];
int currentColor[CHANNELS];
int targetColor[CHANNELS];

#define MIN_DIFF 50

void setup() {
  makeRandomSeed();
  
  // WS2812b INIT
  FastLED.addLeds<WS2812B, WS_PIN, BRG>(wsLeds, WS_LEDS);
  FastLED.setBrightness(255);

  // set minimal light (useful for sm16703p)
  for (int i = 0; i < WS_LEDS; i++) {
    wsLeds[i] = CRGB(1, 1, 1);
  }
  
  FastLED.show();

  delay(2500); // recovery delay

  // setup not-null initial values for current color channelss
  for (int channel = 0; channel < CHANNELS; channel++) {
    currentColor[channel] = random(0, 255);
  }
}

// сделать рандом более рандомным на основании шума с аналогового входа
void makeRandomSeed() {
  randomSeed(analogRead(A0));
}

// установить случайные цели для достижения всем каналам
void randmomizeChannels() {
  makeRandomSeed();
  
  for (int channel = 0; channel < CHANNELS; channel++) {
    randmoizeChannel(channel);
  }
}

// обновить значения всех каналов, вернуть TRUE, елси все каналы достигли целевого значения
boolean iterateChannels() {
  int totalUnreached = CHANNELS;
  
  for (int channel = 0; channel < CHANNELS; channel++) {
    if (iterateChannel(channel)) {
      totalUnreached--;
    }
  }

  bool allTragetsReached = totalUnreached == 0;

  return allTragetsReached;
}

void randmoizeChannel(int channel) {
  targetColor[channel] = (currentColor[channel] + random(MIN_DIFF, 255)) % 255;
}

// сделать текущий цвет канала ближе к целевому цвету на шаг.
// Вернуть ПРАВДУ если целевое значение достигнуто
boolean iterateChannel(int channel) {
  if (abs(targetColor[channel] - currentColor[channel]) < 1) {
    return true; // channel has already reached target value
  }
  
  currentColor[channel] = targetColor[channel] < currentColor[channel] ? -1 : 1;

  // out of range correction
  if (currentColor[channel] > 255) {
    currentColor[channel] = 255;

    return true;
  } else if (currentColor[channel] < 0) {
    currentColor[channel] = 0;

    return true;
  }
  
  return false;
}

// применить каналы к цветам сегментов и обновить ленту
void updateLedStrip() {
  for (int i = 0; i < WS_LEDS - 1; i++) {
    for (int segment = 0; segment < SEGMENTS; segment++) {
       if (i <= segments[segment]) {
           wsLeds[i] = CRGB(
            currentColor[segment * 3], // red
            currentColor[segment * 3 + 1], // green
            currentColor[segment * 3 + 2] // blue
           );
           
           break;
       }
    }
  }

  FastLED.show();
}

void loop() {
  if (iterateChannels()) {
    randmomizeChannels();
    
    delay(3000); // застывание, если цвета достигнуты
  } else {
    updateLedStrip();
  }
  
  delay(10);
}
