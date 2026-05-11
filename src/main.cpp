
#include <Arduino.h>
#include "JwtAuthenticator.h"

void setup() {
    Serial.begin(115200);
    Serial.println("ldskadjjkkjsdjkd JWT test...");
}

void loop() {
    Serial.println("loopxe");
    JwtAuthenticator auth;
    auto claims = auth.authenticate("Bearer eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IlRlc3QgVXNlciIsInJvbGUiOiJhZG1pbiIsImlhdCI6MTc3ODQ5NzE1N30.cYEdipYqM_P1FD51x0daXOjNexVJhx-rLntBMHnIZmSRTWmtczgHDaJouStEBduvK08Gp_XD1-GWXpOTtINUXIwfYF1vh3sHUmt24wfhvuvjMPTaMP3fATUc0hITGQf3zdpJt0qwOrGFXisHdmP5fYeCo6S6yvjUta4HGn0u2dNBdh0E8mtT4413Fx4TVv9jjl-F3mpPOaMIE8pBdLaex2LDtYLZd4oRv7WrBuQCkriQLsQ0kwON4lVwCIT6JfSrrymp6NduWdOtOZiMqPCByMq4vFmB6DEU16KybFjN13Jut14UnSiSAePy3V1Xy_siSE8_FkJz2ct4TeeXalsmFg");

    if (claims.find("error") != claims.end()) {
        Serial.println(("Error: " + claims["error"]).c_str());
    } else {
        Serial.println("JWT validated successfully");
    }

    delay(5000); // avoid spamming
}

