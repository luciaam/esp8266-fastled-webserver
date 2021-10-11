#include <LiquidCrystal.h>

/*
   ESP8266 FastLED WebServer: https://github.com/jasoncoon/esp8266-fastled-webserver
   Copyright (C) 2015-2018 Jason Coon

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// extern "C"
// {
// #include "user_interface.h"
// }

#include <ESP8266WiFi.h>
#include "Settings.h"
#include "PatternManager.h"
#include "WebServer.h"

// #include <FS.h>
// #include "FSBrowser.h"

Settings settings;
PatternManager patternManager(settings);
WebServer webServer(settings, patternManager);

// const String SSID = "Schesa";
// const String PASSWORD = "BlingBling";

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  settings.load();
  patternManager.setup();

  // Serial.println();
  // Serial.print(F("Heap: "));
  // Serial.println(system_get_free_heap_size());
  // Serial.print(F("Boot Vers: "));
  // Serial.println(system_get_boot_version());
  // Serial.print(F("CPU: "));
  // Serial.println(system_get_cpu_freq());
  // Serial.print(F("SDK: "));
  // Serial.println(system_get_sdk_version());
  // Serial.print(F("Chip ID: "));
  // Serial.println(system_get_chip_id());
  // Serial.print(F("Flash ID: "));
  // Serial.println(spi_flash_get_id());
  // Serial.print(F("Flash Size: "));
  // Serial.println(ESP.getFlashChipRealSize());
  // Serial.print(F("Vcc: "));
  // Serial.println(ESP.getVcc());
  // Serial.print(F("MAC Address: "));
  // Serial.println(WiFi.macAddress());
  // Serial.println();

  SPIFFS.begin();
  {
    Serial.println("SPIFFS contents:");

    Dir dir = SPIFFS.openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  setupWifi(settings.ssid, settings.password);
  webServer.setup();
}

void setupWifi(const String &ssid, const String &password)
{
  WiFi.disconnect(true);
  delay(500);

  if (!ssid.isEmpty())
  {
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(5000);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.printf("Connected to local WiFi %s\nIP is %s\n", ssid.c_str(), WiFi.localIP().toString());
  }
  else
  {
    if (!WiFi.softAP(settings.AP_SSID.c_str(), settings.AP_PASSWORD.c_str()))
    {
      Serial.println("[ERROR] softAP setup failed!");
      return;
    }
    delay(500); // slight delay to make sure we got an AP IP
    Serial.println("Access Point IP is " + WiFi.softAPIP().toString());
  }
}

void loop()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(65535));

  webServer.handleClient();

  patternManager.show();
  patternManager.delay();
}