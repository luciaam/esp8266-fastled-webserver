#ifndef PatternManager_h
#define PatternManager_h

#include <vector>
#include <utility>
#include <functional>
#include <FastLED.h>

#include "Settings.h"

class PatternManager
{
public:
    PatternManager(Settings &settings);
    void setup();
    void switchTo(String patternName);
    void switchTo(size_t patternIdx);
    void nextPattern();
    void show();
    void delay();

    void setSolidColor(uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(uint8_t value);
    void setColorTheme(uint8_t value);
    void setColorTheme(String name);

    const std::vector<std::pair<String, std::function<void()>>>& getPatterns() const;
    const std::vector<std::pair<String, CRGBPalette16>>& getThemes() const;

private:
    Settings &_settings;
    std::vector<std::pair<String, std::function<void()>>> _patterns;
    std::vector<std::pair<String, CRGBPalette16>> _themes;

    void showSolidColor();
    void rainbow();
    void rainbowWithGlitter();
    void rainbowSolid();
    void confetti();
    void sinelon();
    void bpm();
    void juggle();
    void pride();
    void radialPaletteShift();
    void addGlitter(uint8_t chanceOfGlitter);
    uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                            uint32_t timebase = 0, uint8_t phase_offset = 0);
    void colorWaves();
    void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette);
};

#endif