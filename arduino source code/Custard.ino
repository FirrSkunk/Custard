
//  Custard source code
//  Made for fur squared 2020
//  breakout board for an adafruit trinket m0
//  contains a strip of 7 neopixels on pin 4 and 
//  a cap touch pad on pin 3
//  Thanks to Kataze @kataze  for making most of this work
//  Thanks to Kbooki @kbooki for the badge art
//  Thanks to Mark Kriegsman @MarkKriegsman for the origianl fastled demo code this is based on
//  Thanks to the F2 attendeed that bought this dumb thing <3

#include <FastLED.h>
#include "Adafruit_FreeTouch.h"


Adafruit_FreeTouch qt_4 = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
FASTLED_USING_NAMESPACE  //I have no idea what this means but if I remove it it breaks

#define DATA_PIN    4
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define NUM_LEDS    10
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         64 //bump this up if you want a brighter donkey
#define FRAMES_PER_SECOND  120




void setup() {

Serial.begin(115200);

// tell FastLED about the LED strip configuration
FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
// set master brightness control
FastLED.setBrightness(BRIGHTNESS);
  
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint16_t gHue = 0; // rotating "base color" used by many of the patterns

unsigned long lastPress;
bool active = true;

void loop()
{
  int counter, result = 0;
  counter = millis();
  result = qt_4.measure();
  bool debounce;

  //Serial.println(result);
  
  //boop trigges new scene
  if (result > 800  && debounce == false) {
    nextPattern();
    debounce = true;
    gHue = random(255);
    //Serial.println(gHue);
    lastPress = millis();
    active = true;
  }
  else if (result < 800 ) {
    debounce = false;
    //Serial.println(result);
  }

  
  if (active) {
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
  }
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  

  // Go to sleep if it's been more than 10 seconds since boop
  if ((counter - lastPress > 10000) && active) {
    Serial.println("Naptime!");
    active = false;
    //fadeToBlackBy( leds, NUM_LEDS, 10);
    for (int i; i < NUM_LEDS; i++) {
      delay(100); 
      leds[i] = CRGB::Black;
      FastLED.show();
    }
   // FastLED.show();
  }


}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS - 1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // 2 colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 2; i++) {
    leds[beatsin16( i + 7, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
