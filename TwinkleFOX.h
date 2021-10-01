//  TwinkleFOX by Mark Kriegsman: https://gist.github.com/kriegsman/756ea6dcae8e30845b5a
//
//  TwinkleFOX: Twinkling 'holiday' lights that fade in and out.
//  Colors are chosen from a palette; a few palettes are provided.
//
//  This December 2015 implementation improves on the December 2014 version
//  in several ways:
//  - smoother fading, compatible with any colors and any palettes
//  - easier control of twinkle speed and twinkle density
//  - supports an optional 'background color'
//  - takes even less RAM: zero RAM overhead per pixel
//  - illustrates a couple of interesting techniques (uh oh...)
//
//  The idea behind this (new) implementation is that there's one
//  basic, repeating pattern that each pixel follows like a waveform:
//  The brightness rises from 0..255 and then falls back down to 0.
//  The brightness at any given point in time can be determined as
//  as a function of time, for example:
//    brightness = sine( time ); // a sine wave of brightness over time
//
//  So the way this implementation works is that every pixel follows
//  the exact same wave function over time.  In this particular case,
//  I chose a sawtooth triangle wave (triwave8) rather than a sine wave,
//  but the idea is the same: brightness = triwave8( time ).
//
//  Of course, if all the pixels used the exact same wave form, and
//  if they all used the exact same 'clock' for their 'time base', all
//  the pixels would brighten and dim at once -- which does not look
//  like twinkling at all.
//
//  So to achieve random-looking twinkling, each pixel is given a
//  slightly different 'clock' signal.  Some of the clocks run faster,
//  some run slower, and each 'clock' also has a random offset from zero.
//  The net result is that the 'clocks' for all the pixels are always out
//  of sync from each other, producing a nice random distribution
//  of twinkles.
//
//  The 'clock speed adjustment' and 'time offset' for each pixel
//  are generated randomly.  One (normal) approach to implementing that
//  would be to randomly generate the clock parameters for each pixel
//  at startup, and store them in some arrays.  However, that consumes
//  a great deal of precious RAM, and it turns out to be totally
//  unnessary!  If the random number generate is 'seeded' with the
//  same starting value every time, it will generate the same sequence
//  of values every time.  So the clock adjustment parameters for each
//  pixel are 'stored' in a pseudo-random number generator!  The PRNG
//  is reset, and then the first numbers out of it are the clock
//  adjustment parameters for the first pixel, the second numbers out
//  of it are the parameters for the second pixel, and so on.
//  In this way, we can 'store' a stable sequence of thousands of
//  random clock adjustment parameters in literally two bytes of RAM.
//
//  There's a little bit of fixed-point math involved in applying the
//  clock speed adjustments, which are expressed in eighths.  Each pixel's
//  clock speed ranges from 8/8ths of the system clock (i.e. 1x) to
//  23/8ths of the system clock (i.e. nearly 3x).
//
//  On a basic Arduino Uno or Leonardo, this code can twinkle 300+ pixels
//  smoothly at over 50 updates per seond.
//
//  -Mark Kriegsman, December 2015
#ifndef TwinkleFOX_h
#define TwinkleFOX_h

#include "Constants.h"
#include "FastLED.h"

extern CRGB leds[NUM_LEDS];

// Overall twinkle speed.
// 0 (VERY slow) to 8 (VERY fast).
// 4, 5, and 6 are recommended, default is 4.
extern uint8_t twinkleSpeed;

// Overall twinkle density.
// 0 (NONE lit) to 8 (ALL lit at once).
// Default is 5.
extern uint8_t twinkleDensity;

// Background color for 'unlit' pixels
// Can be set to CRGB::Black if desired.
extern CRGB gBackgroundColor;
// Example of dim incandescent fairy light background color
// CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

// If AUTO_SELECT_BACKGROUND_COLOR is set to 1,
// then for any palette where the first two entries
// are the same, a dimmed version of that color will
// automatically be used as the background color.
#define AUTO_SELECT_BACKGROUND_COLOR 0

// If COOL_LIKE_INCANDESCENT is set to 1, colors will
// fade out slighted 'reddened', similar to how
// incandescent bulbs change color as they get dim down.
extern uint8_t coolLikeIncandescent;

extern CRGBPalette16 twinkleFoxPalette;

// This function is like 'triwave8', which produces a
// symmetrical up-and-down triangle sawtooth waveform, except that this
// function produces a triangle wave with a faster attack and a slower decay:
//
//     / \
//    /     \
//   /         \
//  /             \
//

extern uint8_t attackDecayWave8(uint8_t i);

// This function takes a pixel, and if its in the 'fading down'
// part of the cycle, it adjusts the color a little bit like the
// way that incandescent bulbs fade toward 'red' as they dim.
extern void doCoolLikeIncandescent(CRGB &c, uint8_t phase);

//  This function takes a time in pseudo-milliseconds,
//  figures out brightness = f( time ), and also hue = f( time )
//  The 'low digits' of the millisecond time are used as
//  input to the brightness wave function.
//  The 'high digits' are used to select a color, so that the color
//  does not change over the course of the fade-in, fade-out
//  of one cycle of the brightness wave function.
//  The 'high digits' are also used to determine whether this pixel
//  should light at all during this cycle, based on the twinkleDensity.
extern CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);

//  This function loops over each pixel, calculates the
//  adjusted 'clock' that this pixel should use, and calls
//  "CalculateOneTwinkle" on each pixel.  It then displays
//  either the twinkle color of the background color,
//  whichever is brighter.
extern void drawTwinkles();

// A mostly red palette with green accents and white trim.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
extern const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM;

// A mostly (dark) green palette with red berries.
extern const TProgmemRGBPalette16 Holly_p FL_PROGMEM;

// A red and white striped palette
// "CRGB::Gray" is used as white to keep the brightness more uniform.
extern const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM;

// A mostly blue palette with white accents.
// "CRGB::Gray" is used as white to keep the brightness more uniform.
extern const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM;

// A pure "fairy light" palette with some brightness variations
extern const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM;

// A palette of soft snowflakes with the occasional bright one
extern const TProgmemRGBPalette16 Snow_p FL_PROGMEM;

// A palette reminiscent of large 'old-school' C9-size tree lights
// in the five classic colors: red, orange, green, blue, and white.
extern const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM;

// A cold, icy pale blue palette
extern const TProgmemRGBPalette16 Ice_p FL_PROGMEM;

extern void redGreenWhiteTwinkles();
extern void hollyTwinkles();
extern void redWhiteTwinkles();
extern void blueWhiteTwinkles();
extern void fairyLightTwinkles();
extern void snow2Twinkles();
extern void iceTwinkles();
extern void retroC9Twinkles();
extern void partyTwinkles();
extern void forestTwinkles();
extern void lavaTwinkles();
extern void fireTwinkles();
extern void cloud2Twinkles();
extern void oceanTwinkles();

#endif