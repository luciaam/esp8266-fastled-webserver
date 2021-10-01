#ifndef WebServer_h
#define WebServer_h

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266HTTPClient.h>
#include "Settings.h"
#include "PatternManager.h"
#include "Field.h"

class WebServer
{
public:
    WebServer(Settings &settings, PatternManager &patternManager);
    void setup();
    void handleClient();

private:
    ESP8266WebServer _webServer;
    ESP8266HTTPUpdateServer _httpUpdateServer;
    Settings &_settings;
    PatternManager &_patternManager;
    void sendString(const String &value);
    void onPost(const String &uri, uint8_t &storeValue);
    void setPower(uint8_t value);
    void setAutoplay(uint8_t value);
    void setAutoplayDuration(uint8_t value);
    
    std::vector<Field> fields;
    String getPattern();
    String getPatterns();
    String getTheme();
    String getThemes();
    String getPower();
    String getBrightness();
    String getAutoplay();
    String getAutoplayDuration();
    String getSolidColor();
    String getSpeed();
    String getTwinkleSpeed();
    String getTwinkleDensity();
    String getCoolLikeIncandescent();
    String getName();
    // Pride Playground fields
    String getSaturationBpm();
    void setSaturationBpm(String value);
    String getSaturationMin();
    void setSaturationMin(String value);
    String getSaturationMax();
    void setSaturationMax(String value);
    String getBrightDepthBpm();
    void setBrightDepthBpm(String value);
    String getBrightDepthMin();
    void setBrightDepthMin(String value);
    String getBrightDepthMax();
    void setBrightDepthMax(String value);
    String getBrightThetaIncBpm();
    void setBrightThetaIncBpm(String value);
    String getBrightThetaIncMin();
    void setBrightThetaIncMin(String value);
    String getBrightThetaIncMax();
    void setBrightThetaIncMax(String value);
    String getMsMultiplierBpm();
    void setMsMultiplierBpm(String value);
    String getMsMultiplierMin();
    void setMsMultiplierMin(String value);
    String getMsMultiplierMax();
    void setMsMultiplierMax(String value);
    String getHueIncBpm();
    void setHueIncBpm(String value);
    String getHueIncMin();
    void setHueIncMin(String value);
    String getHueIncMax();
    void setHueIncMax(String value);
    String getSHueBpm();
    String setSHueBpm(String value);
    String getSHueMin();
    void setSHueMin(String value);
    String getSHueMax();
    void setSHueMax(String value);
    
};

#endif