// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.

// Modified by Jason Coon to replace "magic numbers" with customizable inputs via sliders in the web app.
#ifndef PridePlayground_h
#define PridePlayground_h

#include <stdint.h>

extern uint8_t saturationBpm;
extern uint8_t saturationMin;
extern uint8_t saturationMax;

extern uint8_t brightDepthBpm;
extern uint8_t brightDepthMin;
extern uint8_t brightDepthMax;

extern uint8_t brightThetaIncBpm;
extern uint8_t brightThetaIncMin;
extern uint8_t brightThetaIncMax;

extern uint8_t msMultiplierBpm;
extern uint8_t msMultiplierMin;
extern uint8_t msMultiplierMax;

extern uint8_t hueIncBpm;
extern uint8_t hueIncMin;
extern uint8_t hueIncMax;

extern uint8_t sHueBpm;
extern uint8_t sHueMin;
extern uint8_t sHueMax;

extern void pridePlayground();

#endif