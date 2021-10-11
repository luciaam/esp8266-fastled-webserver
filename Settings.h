#ifndef Settings_h
#define Settings_h

#include <EEPROM.h>
#include "TwinkleFOX.h"
#include "pixeltypes.h"

struct Settings
{
    const String AP_SSID = "Schesa";
    const String AP_PASSWORD = "BlingBling";

    String ssid = "";
    String password = "";

    CRGB solidColor = CRGB::Blue;
    uint8_t currentThemeIndex = 0;

    /* data */

    // // COOLING: How much does the air cool as it rises?
    // // Less cooling = taller flames.  More cooling = shorter flames.
    // // Default 50, suggested range 20-100
    // uint8_t cooling = 49;

    // // SPARKING: What chance (out of 255) is there that a new spark will be lit?
    // // Higher chance = more roaring fire.  Lower chance = more flickery fire.
    // // Default 120, suggested range 50-200.
    // uint8_t sparking = 60;

    uint8_t speed = 30;

    size_t currentPatternIndex = 0; // Index of current pattern

    uint8_t autoplay = 0;
    uint8_t autoplayDuration = 10;

    uint8_t power = 1;
    uint8_t brightness = 50;

    // TODO ???
    uint8_t &twinkleSpeed = ::twinkleSpeed;
    uint8_t &twinkleDensity = ::twinkleDensity;
    uint8_t &coolLikeIncandescent = ::coolLikeIncandescent;

    void load();
    void save();
};

#endif