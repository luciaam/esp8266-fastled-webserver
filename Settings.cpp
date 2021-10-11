#include "Settings.h"

void Settings::load()
{
    EEPROM.begin(512);

    // check for "magic number" so we know settings have been written to EEPROM
    // and it's not just full of random bytes
    if (EEPROM.read(511) != 55)
    {
        return;
    }

    brightness = EEPROM.read(0);

    currentPatternIndex = EEPROM.read(1);

    byte r = EEPROM.read(2);
    byte g = EEPROM.read(3);
    byte b = EEPROM.read(4);

    if (r || g || b)
    {
        solidColor = CRGB(r, g, b);
    }

    power = EEPROM.read(5);

    autoplay = EEPROM.read(6);
    autoplayDuration = EEPROM.read(7);

    currentThemeIndex = EEPROM.read(8);

    twinkleSpeed = EEPROM.read(9);
    twinkleDensity = EEPROM.read(10);

    coolLikeIncandescent = EEPROM.read(13);

    ssid = "";
    auto ssidLength = EEPROM.read(21);
    for (int i = 0; i < ssidLength; ++i)
    {
        ssid += (char)EEPROM.read(23 + i);
    }

    password = "";
    auto passwordLength = EEPROM.read(22);
    for (int i = 0; i < passwordLength; ++i)
    {
        password += (char)EEPROM.read(23 + ssidLength + i);
    }
}

void Settings::save()
{
    EEPROM.write(0, brightness);
    EEPROM.write(1, currentPatternIndex);
    EEPROM.write(2, solidColor.r);
    EEPROM.write(3, solidColor.g);
    EEPROM.write(4, solidColor.b);
    EEPROM.write(5, power);
    EEPROM.write(6, autoplay);
    EEPROM.write(7, autoplayDuration);
    EEPROM.write(8, currentThemeIndex);
    EEPROM.write(9, twinkleSpeed);
    EEPROM.write(10, twinkleDensity);
    EEPROM.write(13, coolLikeIncandescent);

    EEPROM.write(21, ssid.length());
    EEPROM.write(22, password.length());
    for (int i = 0; i < ssid.length(); ++i)
    {
        EEPROM.write(23 + i, ssid[i]);
    }
    for (int i = 0; i < password.length(); ++i)
    {
        EEPROM.write((23 + ssid.length()) + i, password[i]);
    }

    EEPROM.write(511, 55);
    EEPROM.commit();
};