
#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "JwtAuthenticator.h"

namespace {
constexpr long long MIN_VALID_UNIX_TIME = 946684800; // 2000-01-01 UTC
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;
constexpr unsigned long TIME_SYNC_TIMEOUT_MS = 20000;

#ifndef WIFI_SSID
#define WIFI_SSID "Garfield"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "123Madhu$$"
#endif

bool syncDeviceUtcTime() {
    if (strlen(WIFI_SSID) == 0) {
        Serial.println("WiFi credentials missing. Define WIFI_SSID/WIFI_PASSWORD in build flags.");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting WiFi");
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - wifiStart) < WIFI_CONNECT_TIMEOUT_MS) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed.");
        return false;
    }

    configTime(0, 0, "pool.ntp.org", "time.google.com");
    Serial.print("Syncing UTC time");
    unsigned long syncStart = millis();
    while ((millis() - syncStart) < TIME_SYNC_TIMEOUT_MS) {
        const long long nowUtc = static_cast<long long>(time(nullptr));
        if (nowUtc >= MIN_VALID_UNIX_TIME) {
            Serial.println();
            Serial.println("UTC time synced.");
            return true;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("UTC time sync failed.");
    return false;
}
} // namespace

void setup() {
    Serial.begin(115200);
    Serial.println("ldskadjjkkjsdjkd JWT test...");
    syncDeviceUtcTime();
}

void loop() {
    Serial.println("loopxe");
    JwtAuthenticator auth;
    auto claims = auth.authenticate("Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IlRlc3QgVXNlciIsInJvbGUiOiJhZG1pbiIsImlhdCI6MTc3ODQ5ODkyNywiZXhwIjoxNzc4NDk5MTA3fQ.op9JPZugDCdgsBHZ2WtkKXOBX6HIeFRZE2VMy2wrRsdXKLJSIMSt2U-cBFJV4Vg2chNfVN8jo4EJQCsNV_HNa2PmIpgmG-Fjn_ksj8foi88hiEU-1xuKubirqMNWlk3Hj2P0UIW5E2iEb72l4h9cxtrp1JVuYfYyfT13dUF_uTvTQ1RtU2lSx1jC82X3ZgqSO8ReNHqmrvsvVnZX-z79zTWAQK7bBSuFtocY7O0YtRIR8MXonpnDyL8nZeHuuRy8I-bigyJbKvuIqMlxnODqliLsgE_BkVFI71_LXIaM2yC-PPMhHZXbv8mFkaCdplbwbmRtJBpG0kP4iVycALRpOw");

    if (claims.find("error") != claims.end()) {
        Serial.println(("Error: " + claims["error"]).c_str());
    } else {
        Serial.println("JWT validated successfully");
    }

    delay(5000); // avoid spamming
}

