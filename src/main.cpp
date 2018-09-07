#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>

#define DEBUG

const char verTag[] = "beta 1.0";

#include <MFRC522.h>
#include "pitch.h"
#include <LiquidCrystal_I2C.h>

MFRC522 mfrc522 = MFRC522();
LiquidCrystal_I2C lcd(0x3F,20,4);

bool isWifiConnected = false;
bool inAPMode = false;
unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
unsigned long passMillis = 0;

#include "dogFiles/rfid.dog"
#include "dogFiles/wifi.dog"
#include "dogFiles/config.dog"

void setup() {

    Serial.begin(9600);
    #ifdef DEBUG
        Log.begin   (LOG_LEVEL_VERBOSE, &Serial);
    #else
        Log.begin   (LOG_LEVEL_SILENT, &Serial);
    #endif
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
}