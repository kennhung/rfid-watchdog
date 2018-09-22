#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <SPI.h>
#include <Wire.h>

#define DEBUG

const int doorId = 1;
const char verTag[] = "beta 1.0";

int relayPin = 16;
int relayType = 0;
unsigned long relayActiveTime = 3500;

#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include "messageType.h"
#include "pitch.h"

MFRC522 mfrc522 = MFRC522();
LiquidCrystal_I2C lcd(0x3F, 20, 4);

WiFiClient dogClient;

bool isWifiConnected = false;
bool inAPMode = false;
bool inStationMode = false;
bool isWatcherConnected = false;
bool activateRelay = false;

unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
unsigned long passMillis = 0;
unsigned long cooldown = 0;
unsigned long watchercooldown = 0;
unsigned long relayActiveMillis = 0;
unsigned long wifiCheckCooldown = 0;

int stationConnected = 0;


#include "dogFiles/control.dog"
#include "dogFiles/watcher.dog"
#include "dogFiles/rfid.dog"
#include "dogFiles/wifi.dog"
#include "dogFiles/config.dog"

void setup() {
    Serial.begin(9600);
    if (Serial) {
#ifdef DEBUG
        Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#else
        Log.begin(LOG_LEVEL_SILENT, &Serial);
#endif
    }
    Log.notice("[BOOT] wifi-watchdog %s\n", verTag);

    if (!SPIFFS.begin()) {
        Log.warning("[SPIFFS] Formatting filesystem...");
        if (SPIFFS.format()) {
            Serial.println(F(" completed!"));
        } else {
            Serial.println(F(" failed!"));
            Log.warning("[SPIFFS] Could not format filesystem!\n");
        }
    }

    loadConfig();
}

void loop() {
    currentMillis = millis();
    passMillis = currentMillis - prevMillis;
    prevMillis = currentMillis;

    if (currentMillis >= cooldown) {
        rfidloop();
    }

    if (currentMillis >= watchercooldown) {
        updateWatcherConn();
    }

    if ((millis() - relayActiveMillis >= relayActiveTime) && activateRelay) {
        deactiveRelay();
    }

    if (currentMillis >= wifiCheckCooldown) {
        updateWifiStatus();
    }

    if (isWatcherConnected) {
        if (dogClient.available() > 0) {
            String readIn = dogClient.readStringUntil(';');
            Log.verbose("[dogClient] %s\n", readIn.c_str());
            DynamicJsonBuffer jsonBuffer;
            JsonObject& readJson = jsonBuffer.parseObject(readIn);

            int type = readJson["type"];
            switch (type) {
                case 2:
                    String reply = "{'type':";
                    reply += 5;
                    reply += "};";
                    dogClient.print(reply);
                    break;
            }
        }
    }
}