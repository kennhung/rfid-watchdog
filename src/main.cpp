#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <NtpClientLib.h>
#include <SPI.h>
#include <Wire.h>
#include <stdlib.h>
#include <ESPAsyncWebServer.h>

#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include "messageType.h"
#include "pitch.h"

#define DEBUG


int doorId = 1;
char *doorAuthToken = NULL;

const char verTag[] = "beta 1.0";

// io pins
int wifipin = 255;
int relayPin = 16;
int relayType = 0;
int buttonPin = 255;

#define LEDoff HIGH
#define LEDon LOW

MFRC522 mfrc522 = MFRC522();
LiquidCrystal_I2C lcd(0x3F, 20, 4);
WiFiClient dogClient;

boolean syncEventTriggered = false;  // True if a time even has been triggered
NTPSyncEvent_t ntpEvent;             // Last triggered event

AsyncWebServer server(80);

// Variables for whole scope
bool isWifiConnected = false;
bool inAPMode = false;
bool inStationMode = false;
bool isWatcherConnected = false;
bool activateRelay = false;
bool isNTPInited = false;
bool shouldReboot = false;
bool configSuccess = false;

unsigned long autoRestartIntervalSeconds = 0;
unsigned long wifiTimeout = 0;
char *deviceHostname = NULL;

unsigned long currentMillis = 0;
unsigned long prevMillis = 0;
unsigned long deltaTime = 0;
unsigned long uptime = 0;

unsigned long cooldown = 0;
unsigned long watchercooldown = 0;
unsigned long relayActiveMillis = 0;
unsigned long relayActiveTime = 3500;
unsigned long wifiCheckCooldown = 0;

int connectedStationCount = 0;

int ntpInterval = 60;
int timeZone = 0;
char *ntpserver = NULL;

char *watchdogServer = NULL;
int watchdogPort = 6083;

#include "dogFiles/control.dog"
#include "dogFiles/watcher.dog"
#include "dogFiles/ntp.dog"
#include "dogFiles/rfid.dog"
#include "dogFiles/utils.dog"
#include "dogFiles/wifi.dog"
#include "dogFiles/config.dog"
#include "dogFiles/webServer.dog"

void setup() {
    Serial.begin(9600);
    if (Serial) {
#ifdef DEBUG
        Log.begin(LOG_LEVEL_VERBOSE, &Serial);
#else
        Log.begin(LOG_LEVEL_WARNING, &Serial);
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

    NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
        ntpEvent = event;
        syncEventTriggered = true;
    });

    configSuccess = loadConfig();
    if (!configSuccess) {
        fallbacktoAPMode();
    }

    setupWebServer();
}

void loop() {
    currentMillis = millis();
    deltaTime = currentMillis - prevMillis;
    prevMillis = currentMillis;

    uptime = millis() / 1000;

    if (currentMillis >= cooldown) {
        rfidloop();
    }

    if (currentMillis >= watchercooldown && configSuccess) {
        updateWatcherConn();
    }

    if ((millis() - relayActiveMillis >= relayActiveTime) && activateRelay) {
        deactiveRelay();
    }

    if (currentMillis >= wifiCheckCooldown) {
        updateWifiStatus();
    }

    if (isWifiConnected && !isNTPInited) {
        isNTPInited = true;
        if (ntpserver != NULL) {
            NTP.begin(ntpserver, timeZone, true, 0);
            NTP.setInterval(ntpInterval);
        } else {
            Log.error("[NTP] server address is NULL!!\n");
        }
    }

    if (autoRestartIntervalSeconds > 0 &&
        uptime > autoRestartIntervalSeconds * 1000) {
        shouldReboot = true;
    }

    if (shouldReboot) {
        Log.notice("[sys] System is going to reboot\n");
        delay(1000);
        ESP.restart();
    }

    if (syncEventTriggered) {
        processSyncEvent(ntpEvent);
        syncEventTriggered = false;
    }

    if (isWatcherConnected) {
        if (dogClient.available() > 0) {
            String readIn = dogClient.readStringUntil(';');
            handleWatchDogServerRequest(readIn);
        }
    }
}