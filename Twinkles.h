#ifndef Twinkles_h
#define Twinkles_h

#include "FastLED.h"
#include "Constants.h"

// based on ColorTwinkles by Mark Kriegsman: https://gist.github.com/kriegsman/5408ecd397744ba0393e

#define STARTING_BRIGHTNESS 64
#define FADE_IN_SPEED       32
#define FADE_OUT_SPEED      20
#define DENSITY            255

extern CRGBPalette16 gCurrentTheme;

enum { GETTING_DARKER = 0, GETTING_BRIGHTER = 1 };

extern CRGB makeBrighter( const CRGB& color, fract8 howMuchBrighter);

extern CRGB makeDarker( const CRGB& color, fract8 howMuchDarker);

// Compact implementation of
// the directionFlags array, using just one BIT of RAM
// per pixel.  This requires a bunch of bit wrangling,
// but conserves precious RAM.  The cost is a few
// cycles and about 100 bytes of flash program memory.
extern uint8_t  directionFlags[ (NUM_LEDS + 7) / 8];

extern bool getPixelDirection( uint16_t i);
extern void setPixelDirection( uint16_t i, bool dir);
extern void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount);
extern void colortwinkles();
extern void cloudTwinkles();
extern void rainbowTwinkles();
extern void snowTwinkles();
extern void incandescentTwinkles();

#endif