#include <Arduino.h>
#include "JwtAuthenticator.h"

void setup() {
  Serial.begin(115200);
  Serial.println("kkjdsajkk jdeoang-un");
}

void loop() {
    JwtAuthenticator auth;
    auto claims = auth.authenticate("Bearer <your.jwt.token>");
    
    if (claims.find("error") != claims.end()) {
        Serial.println("Error: " + claims["error"]);
        // handle error
    } else {
        Serial.println("Done");
    }
}

