#include "WebServer.h"

#include "Settings.h"
#include "PatternManager.h"

#include "Field.h"
#include "PridePlayground.h"

WebServer::WebServer(Settings &settings, PatternManager &patternManager)
    : _settings(settings), _patternManager(patternManager),
      fields({
          {"power", "Power", BooleanFieldType, 0, 1, [this]()
           { return getPower(); }},
          {"brightness", "Brightness", NumberFieldType, 1, 255, [this]()
           { return getBrightness(); }},
          {"pattern", "Pattern", SelectFieldType, 0, (uint8_t)_patternManager.getPatterns().size(), [this]()
           { return getPattern(); },
           [this]()
           { return getPatterns(); }},
          {"theme", "Theme", SelectFieldType, 0, (uint8_t)_patternManager.getThemes().size(), [this]()
           { return getTheme(); },
           [this]()
           { return getThemes(); }},
          {"speed", "Speed", NumberFieldType, 1, 255, [this]()
           { return getSpeed(); }},

          {"autoplaySection", "Autoplay", SectionFieldType},
          {"autoplay", "Autoplay", BooleanFieldType, 0, 1, [this]()
           { return getAutoplay(); }},
          {"autoplayDuration", "Autoplay Duration", NumberFieldType, 0, 255, [this]()
           { return getAutoplayDuration(); }},

          {"solidColorSection", "Solid Color", SectionFieldType},
          {"solidColor", "Color", ColorFieldType, 0, 255, [this]()
           { return getSolidColor(); }},

          {"twinklesSection", "Twinkles", SectionFieldType},
          {"twinkleSpeed", "Twinkle Speed", NumberFieldType, 0, 8, [this]()
           { return getTwinkleSpeed(); }},
          {"twinkleDensity", "Twinkle Density", NumberFieldType, 0, 8, [this]()
           { return getTwinkleDensity(); }},
          {"coolLikeIncandescent", "Incandescent Cool", BooleanFieldType, 0, 1, [this]()
           { return getCoolLikeIncandescent(); }},

          {"prideSection", "Pride", SectionFieldType},

          {"saturationBpm", "Saturation BPM", NumberFieldType, 0, 255, [this]()
           { return getSaturationBpm(); },
           NULL, [this](String val)
           { setSaturationBpm(val); }},
          {"saturationMin", "Saturation Min", NumberFieldType, 0, 255, [this]()
           { return getSaturationMin(); },
           NULL, [this](String val)
           { setSaturationMin(val); }},
          {"saturationMax", "Saturation Max", NumberFieldType, 0, 255, [this]()
           { return getSaturationMax(); },
           NULL, [this](String val)
           { setSaturationMax(val); }},

          {"brightDepthBpm", "Brightness Depth BPM", NumberFieldType, 0, 255, [this]()
           { return getBrightDepthBpm(); },
           NULL, [this](String val)
           { setBrightDepthBpm(val); }},
          {"brightDepthMin", "Brightness Depth Min", NumberFieldType, 0, 255, [this]()
           { return getBrightDepthMin(); },
           NULL, [this](String val)
           { setBrightDepthMin(val); }},
          {"brightDepthMax", "Brightness Depth Max", NumberFieldType, 0, 255, [this]()
           { return getBrightDepthMax(); },
           NULL, [this](String val)
           { setBrightDepthMax(val); }},

          {"brightThetaIncBpm", "Bright Theta Inc BPM", NumberFieldType, 0, 255, [this]()
           { return getBrightThetaIncBpm(); },
           NULL, [this](String val)
           { setBrightThetaIncBpm(val); }},
          {"brightThetaIncMin", "Bright Theta Inc Min", NumberFieldType, 0, 255, [this]()
           { return getBrightThetaIncMin(); },
           NULL, [this](String val)
           { setBrightThetaIncMin(val); }},
          {"brightThetaIncMax", "Bright Theta Inc Max", NumberFieldType, 0, 255, [this]()
           { return getBrightThetaIncMax(); },
           NULL, [this](String val)
           { setBrightThetaIncMax(val); }},

          {"msMultiplierBpm", "Time Multiplier BPM", NumberFieldType, 0, 255, [this]()
           { return getMsMultiplierBpm(); },
           NULL, [this](String val)
           { setMsMultiplierBpm(val); }},
          {"msMultiplierMin", "Time Multiplier Min", NumberFieldType, 0, 255, [this]()
           { return getMsMultiplierMin(); },
           NULL, [this](String val)
           { setMsMultiplierMin(val); }},
          {"msMultiplierMax", "Time Multiplier Max", NumberFieldType, 0, 255, [this]()
           { return getMsMultiplierMax(); },
           NULL, [this](String val)
           { setMsMultiplierMax(val); }},

          {"hueIncBpm", "Hue Inc BPM", NumberFieldType, 0, 255, [this]()
           { return getHueIncBpm(); },
           NULL, [this](String val)
           { setHueIncBpm(val); }},
          {"hueIncMin", "Hue Inc Min", NumberFieldType, 0, 255, [this]()
           { return getHueIncMin(); },
           NULL, [this](String val)
           { setHueIncMin(val); }},
          {"hueIncMax", "Hue Inc Max", NumberFieldType, 0, 255, [this]()
           { return getHueIncMax(); },
           NULL, [this](String val)
           { setHueIncMax(val); }},

          {"sHueBpm", "S Hue BPM", NumberFieldType, 0, 255, [this]()
           { return getSHueBpm(); },
           NULL, [this](String val)
           { setSHueBpm(val); }},
          {"sHueMin", "S Hue Min", NumberFieldType, 0, 255, [this]()
           { return getSHueMin(); },
           NULL, [this](String val)
           { setSHueMin(val); }},
          {"sHueMax", "S Hue Max", NumberFieldType, 0, 255, [this]()
           { return getSHueMax(); },
           NULL, [this](String val)
           { setSHueMax(val); }},
      })
{
}

void WebServer::setup()
{
    _httpUpdateServer.setup(&_webServer);

    _webServer.on("/all", HTTP_GET, [this]()
                  {
                      String json = getFieldsJson(fields);
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      _webServer.send(200, "application/json", json);
                  });

    _webServer.on("/fieldValue", HTTP_GET, [this]()
                  {
                      String name = _webServer.arg("name");
                      String value = getFieldValue(name, fields);
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      _webServer.send(200, "text/json", value);
                  });

    _webServer.on("/fieldValue", HTTP_POST, [this]()
                  {
                      String name = _webServer.arg("name");
                      String value = _webServer.arg("value");
                      String newValue = setFieldValue(name, value, fields);
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      _webServer.send(200, "text/json", newValue);
                  });

    _webServer.on("/power", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      setPower(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.power));
                  });

    _webServer.on("/speed", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _settings.speed = value.toInt();
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.speed));
                  });

    _webServer.on("/twinkleSpeed", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _settings.twinkleSpeed = value.toInt() % 8;
                      _settings.save();
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.twinkleSpeed));
                  });

    _webServer.on("/twinkleDensity", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _settings.twinkleDensity = value.toInt() % 8;

                      _settings.save();
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(twinkleDensity));
                  });

    _webServer.on("/coolLikeIncandescent", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _settings.coolLikeIncandescent = value.toInt();
                      if (_settings.coolLikeIncandescent < 0)
                          _settings.coolLikeIncandescent = 0;
                      else if (_settings.coolLikeIncandescent > 1)
                          coolLikeIncandescent = 1;
                      _settings.save();
                      sendString(String(_settings.coolLikeIncandescent));
                  });

    _webServer.on("/solidColor", HTTP_POST, [this]()
                  {
                      String r = _webServer.arg("r");
                      String g = _webServer.arg("g");
                      String b = _webServer.arg("b");
                      _patternManager.setSolidColor(r.toInt(), g.toInt(), b.toInt());
                      sendString(String(_settings.solidColor.r) + "," + String(_settings.solidColor.g) + "," + String(_settings.solidColor.b));
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                  });

    // TODO pattern and patternName needed?
    _webServer.on("/pattern", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _patternManager.switchTo(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.currentPatternIndex));
                  });

    _webServer.on("/patternName", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _patternManager.switchTo(value);
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.currentPatternIndex));
                  });

    // _webServer.on("/palette", HTTP_POST, [this]()
    _webServer.on("/theme", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _patternManager.setColorTheme(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.currentThemeIndex));
                  });

    _webServer.on("/themeName", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _patternManager.setColorTheme(value);
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.currentThemeIndex));
                  });

    _webServer.on("/brightness", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      _patternManager.setBrightness(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.brightness));
                  });

    _webServer.on("/autoplay", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      setAutoplay(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.autoplay));
                  });

    _webServer.on("/autoplayDuration", HTTP_POST, [this]()
                  {
                      String value = _webServer.arg("value");
                      setAutoplayDuration(value.toInt());
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(String(_settings.autoplayDuration));
                  });

    _webServer.on("/wifi", HTTP_POST, [this]()
                  {
                      String ssid = _webServer.arg("ssid");
                      String password = _webServer.arg("password");
                      setWiFi(ssid, password);

                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      _webServer.send(200, "text/json", "wifi saved");
                  });

    // //list directory
    // _webServer.on("/list", HTTP_GET, handleFileList);
    // //load editor
    // _webServer.on("/edit", HTTP_GET, [this]()
    //               {
    //                   if (!handleFileRead("/edit.htm"))
    //                       _webServer.send(404, "text/plain", "FileNotFound");
    //               });
    // //create file
    // _webServer.on("/edit", HTTP_PUT, handleFileCreate);
    // //delete file
    // _webServer.on("/edit", HTTP_DELETE, handleFileDelete);
    // //first callback is called after the request has ended with all parsed arguments
    // //second callback handles file uploads at that location
    // _webServer.on(
    //     "/edit", HTTP_POST, [this]()
    //     {
    //         _webServer.sendHeader("Access-Control-Allow-Origin", "*");
    //         _webServer.send(200, "text/plain", "");
    //     },
    //     handleFileUpload);

    _webServer.serveStatic("/", SPIFFS, "/", "max-age=86400");

    _webServer.begin();
    Serial.println("HTTP web server started");
}

void WebServer::onPost(const String &uri, uint8_t &storeValue)
{
    _webServer.on(uri, HTTP_POST, [this, &storeValue]()
                  {
                      auto value = _webServer.arg("value");
                      storeValue = value.toInt();
                      _webServer.sendHeader("Access-Control-Allow-Origin", "*");
                      sendString(value);
                  });
}

void WebServer::handleClient()
{
    _webServer.handleClient();
}

void WebServer::sendString(const String &value)
{
    _webServer.send(200, "text/plain", value);
}

void WebServer::setPower(uint8_t value)
{
    _settings.power = value == 0 ? 0 : 1;
    _settings.save();
}

void WebServer::setAutoplay(uint8_t value)
{
    _settings.autoplay = value == 0 ? 0 : 1;
    _settings.save();
}

void WebServer::setAutoplayDuration(uint8_t value)
{
    _settings.autoplayDuration = value;
    _settings.save();
    // autoPlayTimeout = millis() + (_settings.autoplayDuration * 1000);
}

void WebServer::setWiFi(const String &ssid, const String &password)
{
    _settings.ssid = ssid;
    _settings.password = password;
    _settings.save();
    ESP.reset();
}

//Getter

String WebServer::getPattern()
{
    return String(_settings.currentPatternIndex);
}

String WebServer::getPatterns()
{
    String json;
    auto patternCount = _patternManager.getPatterns().size();

    for (uint8_t i = 0; i < patternCount; i++)
    {
        json += "\"" + _patternManager.getPatterns()[i].first + "\"";
        if (i < patternCount - 1)
            json += ",";
    }

    return json;
}

String WebServer::getTheme()
{
    return String(_settings.currentThemeIndex);
}

String WebServer::getThemes()
{
    String json = "";
    auto themeCount = _patternManager.getThemes().size();

    for (uint8_t i = 0; i < themeCount; i++)
    {
        json += "\"" + _patternManager.getThemes()[i].first + "\"";
        if (i < themeCount - 1)
            json += ",";
    }

    return json;
}

String WebServer::getPower()
{
    return String(_settings.power);
}

String WebServer::getBrightness()
{
    return String(_settings.brightness);
}

String WebServer::getAutoplay()
{
    return String(_settings.autoplay);
}

String WebServer::getAutoplayDuration()
{
    return String(_settings.autoplayDuration);
}

String WebServer::getSolidColor()
{
    return String(_settings.solidColor.r) + "," + String(_settings.solidColor.g) + "," + String(_settings.solidColor.b);
}

String WebServer::getSpeed()
{
    return String(_settings.speed);
}

String WebServer::getTwinkleSpeed()
{
    return String(_settings.twinkleSpeed);
}

String WebServer::getTwinkleDensity()
{
    return String(_settings.twinkleDensity);
}

String WebServer::getCoolLikeIncandescent()
{
    return String(_settings.coolLikeIncandescent);
}

String WebServer::getSaturationBpm()
{
    return String(saturationBpm);
}

void WebServer::setSaturationBpm(String value)
{
    saturationBpm = value.toInt();
    // TODO ??? get/set ???
    // return value;
}

String WebServer::getSaturationMin()
{
    return String(saturationMin);
}

void WebServer::setSaturationMin(String value)
{
    saturationMin = value.toInt();
    // return value;
}

String WebServer::getSaturationMax()
{
    return String(saturationMax);
}

void WebServer::setSaturationMax(String value)
{
    saturationMax = value.toInt();
    // return value;
}

String WebServer::getBrightDepthBpm()
{
    return String(brightDepthBpm);
}

void WebServer::setBrightDepthBpm(String value)
{
    brightDepthBpm = value.toInt();
    // return value;
}

String WebServer::getBrightDepthMin()
{
    return String(brightDepthMin);
}

void WebServer::setBrightDepthMin(String value)
{
    brightDepthMin = value.toInt();
    // return value;
}

String WebServer::getBrightDepthMax()
{
    return String(brightDepthMax);
}

void WebServer::setBrightDepthMax(String value)
{
    brightDepthMax = value.toInt();
    // return value;
}

String WebServer::getBrightThetaIncBpm()
{
    return String(brightThetaIncBpm);
}

void WebServer::setBrightThetaIncBpm(String value)
{
    brightThetaIncBpm = value.toInt();
    // return value;
}

String WebServer::getBrightThetaIncMin()
{
    return String(brightThetaIncMin);
}

void WebServer::setBrightThetaIncMin(String value)
{
    brightThetaIncMin = value.toInt();
    // return value;
}

String WebServer::getBrightThetaIncMax()
{
    return String(brightThetaIncMax);
}

void WebServer::setBrightThetaIncMax(String value)
{
    brightThetaIncMax = value.toInt();
    // return value;
}

String WebServer::getMsMultiplierBpm()
{
    return String(msMultiplierBpm);
}

void WebServer::setMsMultiplierBpm(String value)
{
    msMultiplierBpm = value.toInt();
    // return value;
}

String WebServer::getMsMultiplierMin()
{
    return String(msMultiplierMin);
}

void WebServer::setMsMultiplierMin(String value)
{
    msMultiplierMin = value.toInt();
    // return value;
}

String WebServer::getMsMultiplierMax()
{
    return String(msMultiplierMax);
}

void WebServer::setMsMultiplierMax(String value)
{
    msMultiplierMax = value.toInt();
    // return value;
}

String WebServer::getHueIncBpm()
{
    return String(hueIncBpm);
}

void WebServer::setHueIncBpm(String value)
{
    hueIncBpm = value.toInt();
    // return value;
}

String WebServer::getHueIncMin()
{
    return String(hueIncMin);
}

void WebServer::setHueIncMin(String value)
{
    hueIncMin = value.toInt();
    // return value;
}

String WebServer::getHueIncMax()
{
    return String(hueIncMax);
}

void WebServer::setHueIncMax(String value)
{
    hueIncMax = value.toInt();
    // return value;
}

String WebServer::getSHueBpm()
{
    return String(sHueBpm);
}

String WebServer::setSHueBpm(String value)
{
    sHueBpm = value.toInt();
    // return value;
}

String WebServer::getSHueMin()
{
    return String(sHueMin);
}

void WebServer::setSHueMin(String value)
{
    sHueMin = value.toInt();
    // return value;
}

String WebServer::getSHueMax()
{
    return String(sHueMax);
}

void WebServer::setSHueMax(String value)
{
    sHueMax = value.toInt();
    // return value;
}
