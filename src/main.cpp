
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
    auto authToken = auth.authenticate("Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IlRlc3QgVXNlciIsInJvbGUiOiJhZG1pbiIsImlhdCI6MTc3ODUwMjk1NiwiZXhwIjoxNzc4NTAzMTM2fQ.TNjxF7ZgJLycNXOvZLW3sNjZuSHfskZCMkInQY36CfmnuPhgdQU-eQwrpl0xtrmj-7peZwp28EuSdu34glf8wv0Ja_COCtwzfUeNa6IgfH9EjkmYCswQjplcpftaRKKygMrS-0JNRb3jswiETM0Ihlwcc4D008x3xrS0lqDBViajTmu2tmFC51dK0aiqJKvlDC-2dFxkNj28HkCJGGUtpt2nb8mtrhxzv5WtT66paBLAEE8VhJoCeQbSHeynK7W7sLTAExm0h1MEmtCmWX2a4a-4vOla7wMALnksZn-GvHNYpeIzK-YhpOHhyOQSn9WKLyX153MV-CixBbbk48m2Vw");

    if (!authToken.authenticated) {
        const auto errIt = authToken.claims.find("error");
        const std::string err = (errIt != authToken.claims.end()) ? errIt->second : "Authentication failed";
        Serial.println(("Error: " + err).c_str());
    } else {
        Serial.println("JWT validated successfully");
        Serial.println(("Principal: " + authToken.principal).c_str());
    }

    delay(5000); // avoid spamming
}

