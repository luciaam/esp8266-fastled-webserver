#include "PatternManager.h"
#include "Constants.h"

#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

CRGB leds[NUM_LEDS];

#include "Twinkles.h"
#include "TwinkleFOX.h"
#include "PridePlayground.h"
#include "ColorWavesPlayground.h"

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const std::array<TProgmemRGBGradientPalettePtr,33> gGradientPalettes;

uint8_t gCurrentThemeNumber = 0;
CRGBPalette16 gCurrentTheme(CRGB::Black);
CRGBPalette16 gTargetTheme(gGradientPalettes[0]);

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

PatternManager::PatternManager(Settings &settings) : _settings(settings)
{
    // List of patterns to cycle through. Each is defined as a separate function below.
    _patterns = {
        {"Pride", [this](){pride();}},
        {"Color Waves", [this](){colorWaves();}},

        {"Pride Playground", pridePlayground},
        {"Color Waves Playground", colorWavesPlayground},

        // twinkle patterns
        {"Rainbow Twinkles", rainbowTwinkles},
        {"Snow Twinkles", snowTwinkles},
        {"Cloud Twinkles", cloudTwinkles},
        {"Incandescent Twinkles", incandescentTwinkles},

        // TwinkleFOX patterns
        {"Retro C9 Twinkles", retroC9Twinkles},
        {"Red & White Twinkles", redWhiteTwinkles},
        {"Blue & White Twinkles", blueWhiteTwinkles},
        {"Red, Green & White Twinkles", redGreenWhiteTwinkles},
        {"Fairy Light Twinkles", fairyLightTwinkles},
        {"Snow 2 Twinkles", snow2Twinkles},
        {"Holly Twinkles", hollyTwinkles},
        {"Ice Twinkles", iceTwinkles},
        {"Party Twinkles", partyTwinkles},
        {"Forest Twinkles", forestTwinkles},
        {"Lava Twinkles", lavaTwinkles},
        {"Fire Twinkles", fireTwinkles},
        {"Cloud 2 Twinkles", cloud2Twinkles},
        {"Ocean Twinkles", oceanTwinkles},

        {"Rainbow", [this](){rainbow();}},
        {"Rainbow With Glitter", [this](){rainbowWithGlitter();}},
        {"Solid Rainbow", [this](){rainbowSolid();}},
        {"Confetti", [this](){confetti();}},
        {"Sinelon", [this](){sinelon();}},
        {"Beat",[this](){ bpm();}},
        {"Juggle", [this](){juggle();}},

        {"Solid Color", [this](){showSolidColor();}}};

    _themes = {
        {"Rainbow", RainbowColors_p},
        {"Rainbow Stripe", RainbowStripeColors_p},
        {"Cloud", CloudColors_p},
        {"Lava", LavaColors_p},
        {"Ocean", OceanColors_p},
        {"Forest", ForestColors_p},
        {"Party", PartyColors_p},
        {"Heat", HeatColors_p},
    };

    if (_settings.currentPatternIndex > _patterns.size())
    {
        _settings.currentPatternIndex = 0;
        _settings.save();
    }
}

void PatternManager::setup()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); // for WS2812 (Neopixel)
    FastLED.setDither(false);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(_settings.brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void PatternManager::switchTo(String patternName)
{
    for (size_t i = 0; i < _patterns.size(); i++)
    {
        if (_patterns[i].first == patternName)
        {
            switchTo(i);
            break;
        }
    }
}

void PatternManager::switchTo(size_t patternIdx)
{
    _settings.currentPatternIndex = patternIdx % _patterns.size();

    if (_settings.autoplay == 0)
    {
        _settings.save();
    }
}

void PatternManager::nextPattern()
{
    _settings.currentPatternIndex = ++_settings.currentPatternIndex % _patterns.size();

    // if (autoplay == 0)
    // {
    //     writeAndCommitSettings();
    // }
}

void PatternManager::show()
{
    static unsigned long autoPlayTimeout = 0;

    if (_settings.power == 0)
    {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        return;
    }

    // change to a new cpt-city gradient palette
    EVERY_N_SECONDS(10)
    {
        gCurrentThemeNumber = (gCurrentThemeNumber+1) % gGradientPalettes.size();
        gTargetTheme = gGradientPalettes[gCurrentThemeNumber];
    }

    EVERY_N_MILLISECONDS(40)
    {
        // slowly blend the current palette to the next
        nblendPaletteTowardPalette(gCurrentTheme, gTargetTheme, 8);
        gHue++; // slowly cycle the "base color" through the rainbow
    }

    if (_settings.autoplay && (millis() > autoPlayTimeout))
    {
        nextPattern();
        autoPlayTimeout = millis() + (_settings.autoplayDuration * 1000);
    }

    // Call the current pattern function once, updating the 'leds' array
    _patterns[_settings.currentPatternIndex].second();

    FastLED.show();
}

void PatternManager::delay()
{
    // insert a delay to keep the framerate modest
    FastLED.delay(1000 / FRAMES_PER_SECOND);
}

const std::vector<std::pair<String, std::function<void()>>>& PatternManager::getPatterns() const {
    return _patterns;
}

const std::vector<std::pair<String, CRGBPalette16>>& PatternManager::getThemes() const {
    return _themes;
}

void PatternManager::setSolidColor(uint8_t r, uint8_t g, uint8_t b)
{
    _settings.solidColor = CRGB(r, g, b);
    _settings.save();
    switchTo("Solid Color");
}

void PatternManager::setBrightness(uint8_t value)
{
    _settings.brightness = value;

    FastLED.setBrightness(_settings.brightness);
    _settings.save();
}

void PatternManager::setColorTheme(uint8_t value)
{
    if (value >= _themes.size())
        value = _themes.size() - 1;

    _settings.currentThemeIndex = value;
    _settings.save();
}

void PatternManager::setColorTheme(String name)
{
    for (uint8_t i = 0; i < _themes.size(); i++)
    {
        if (_themes[i].first == name)
        {
            setColorTheme(i);
            break;
        }
    }
}

void PatternManager::showSolidColor()
{
    fill_solid(leds, NUM_LEDS, _settings.solidColor);
}

// Patterns from FastLED example DemoReel100: https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino

void PatternManager::rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 255 / NUM_LEDS);
}

void PatternManager::rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void PatternManager::rainbowSolid()
{
    fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
}

void PatternManager::confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    // leds[pos] += CHSV( gHue + random8(64), 200, 255);
    leds[pos] += ColorFromPalette(_themes[_settings.currentThemeIndex].second, gHue + random8(64));
}

void PatternManager::sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(_settings.speed, 0, NUM_LEDS);
    static int prevpos = 0;
    CRGB color = ColorFromPalette(_themes[_settings.currentThemeIndex].second, gHue, 255);
    if (pos < prevpos)
    {
        fill_solid(leds + pos, (prevpos - pos) + 1, color);
    }
    else
    {
        fill_solid(leds + prevpos, (pos - prevpos) + 1, color);
    }
    prevpos = pos;
}

void PatternManager::bpm()
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t beat = beatsin8(_settings.speed, 64, 255);
    CRGBPalette16 theme = _themes[_settings.currentThemeIndex].second;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = ColorFromPalette(theme, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void PatternManager::juggle()
{
    static uint8_t numdots = 4;                // Number of dots in use.
    static uint8_t faderate = 2;               // How long should the trails be. Very low value = longer trails.
    static uint8_t hueinc = 255 / numdots - 1; // Incremental change in hue between each dot.
    static uint8_t thishue = 0;                // Starting hue.
    static uint8_t curhue = 0;                 // The current hue
    static uint8_t thissat = 255;              // Saturation of the colour.
    static uint8_t thisbright = 255;           // How bright should the LED/display be.
    static uint8_t basebeat = 5;               // Higher = faster movement.

    static uint8_t lastSecond = 99;              // Static variable, means it's only defined once. This is our 'debounce' variable.
    uint8_t secondHand = (millis() / 1000) % 30; // IMPORTANT!!! Change '30' to a different value to change duration of the loop.

    if (lastSecond != secondHand)
    { // Debounce to make sure we're not repeating an assignment.
        lastSecond = secondHand;
        switch (secondHand)
        {
        case 0:
            numdots = 1;
            basebeat = 20;
            hueinc = 16;
            faderate = 2;
            thishue = 0;
            break; // You can change values here, one at a time , or altogether.
        case 10:
            numdots = 4;
            basebeat = 10;
            hueinc = 16;
            faderate = 8;
            thishue = 128;
            break;
        case 20:
            numdots = 8;
            basebeat = 3;
            hueinc = 0;
            faderate = 8;
            thishue = random8();
            break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
        case 30:
            break;
        }
    }

    // Several colored dots, weaving in and out of sync with each other
    curhue = thishue; // Reset the hue values.
    fadeToBlackBy(leds, NUM_LEDS, faderate);
    for (int i = 0; i < numdots; i++)
    {
        //beat16 is a FastLED 3.1 function
        leds[beatsin16(basebeat + i + numdots, 0, NUM_LEDS)] += CHSV(gHue + curhue, thissat, thisbright);
        curhue += hueinc;
    }
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void PatternManager::pride()
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    uint8_t sat8 = beatsin88(87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; //gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 1, 3000);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        CRGB newcolor = CHSV(hue8, sat8, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (NUM_LEDS - 1) - pixelnumber;

        nblend(leds[pixelnumber], newcolor, 64);
    }
}

void PatternManager::radialPaletteShift()
{
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        // leds[i] = ColorFromPalette( gCurrentPalette, gHue + sin8(i*16), brightness);
        leds[i] = ColorFromPalette(gCurrentTheme, i + gHue, 255, LINEARBLEND);
    }
}

void PatternManager::addGlitter(uint8_t chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}


uint8_t PatternManager::beatsaw8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest,
                                 uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatsaw = beat + phase_offset;
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsaw, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

void PatternManager::colorWaves()
{
    colorwaves(leds, NUM_LEDS, gCurrentTheme);
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void PatternManager::colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette)
{
    static uint16_t sPseudotime = 0;
    static uint16_t sLastMillis = 0;
    static uint16_t sHue16 = 0;

    // uint8_t sat8 = beatsin88( 87, 220, 250);
    uint8_t brightdepth = beatsin88(341, 96, 224);
    uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
    uint8_t msmultiplier = beatsin88(147, 23, 60);

    uint16_t hue16 = sHue16; //gHue * 256;
    uint16_t hueinc16 = beatsin88(113, 300, 1500);

    uint16_t ms = millis();
    uint16_t deltams = ms - sLastMillis;
    sLastMillis = ms;
    sPseudotime += deltams * msmultiplier;
    sHue16 += deltams * beatsin88(400, 5, 9);
    uint16_t brightnesstheta16 = sPseudotime;

    for (uint16_t i = 0; i < numleds; i++)
    {
        hue16 += hueinc16;
        uint8_t hue8 = hue16 / 256;
        uint16_t h16_128 = hue16 >> 7;
        if (h16_128 & 0x100)
        {
            hue8 = 255 - (h16_128 >> 1);
        }
        else
        {
            hue8 = h16_128 >> 1;
        }

        brightnesstheta16 += brightnessthetainc16;
        uint16_t b16 = sin16(brightnesstheta16) + 32768;

        uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
        uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
        bri8 += (255 - brightdepth);

        uint8_t index = hue8;
        //index = triwave8( index);
        index = scale8(index, 240);

        CRGB newcolor = ColorFromPalette(palette, index, bri8);

        uint16_t pixelnumber = i;
        pixelnumber = (numleds - 1) - pixelnumber;

        nblend(ledarray[pixelnumber], newcolor, 128);
    }
}
