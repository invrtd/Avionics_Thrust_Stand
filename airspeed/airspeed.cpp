#include "airspeed.h"
#include <Arduino.h>
#include <WiFi.h>
#include "../util.h"

// Replace with your Larus AP name (SSID)
const char* ssid     = "Larus_EE78";
const char* password = "LarusSensor";

WiFiClient client;
unsigned long lastAttempt = 0;

float current_dp = 0.0;
float current_ias = 0.0;

void connectToLarusWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Non-blocking wait for a bit
  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println("\nWiFi Connection Failed (Timeout)");
  }
}

bool connectToLarusStream() {
  Serial.println("Connecting to Larus TCP stream on port 8880...");
  if (client.connect("192.168.4.1", 8880)) {
    Serial.println("Connected to Larus stream!");
    return true;
  }
  Serial.println("Failed to connect to Larus TCP.");
  return false;
}

float computeIAS(float dp_pa) {
  const float rho = 1.225f;  // kg/m^3
  if (dp_pa < 0) dp_pa = 0;
  return sqrtf((2.0f * dp_pa) / rho);
}

void airspeed_init() {
    Serial.println("Airspeed Init (WiFi setup)...");
    connectToLarusWiFi();
    connectToLarusStream();
}

void airspeed_update() {
    // Reconnect WiFi if dropped
  if (WiFi.status() != WL_CONNECTED) {
    // Ideally wouldn't print here in update loop, but kept for debug
    // Serial.println("WiFi lost — reconnecting..."); 
    connectToLarusWiFi();
    connectToLarusStream();
    return;
  }

  // Reconnect TCP if dropped
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastAttempt > 2000) {
      lastAttempt = now;
      connectToLarusStream();
    }
    return;
  }

  // Process stream
  while (client.available()) {
    String line = client.readStringUntil('\n');

    if (line.startsWith("P_pitot")) {
      // Example: "P_pitot / Pa -14.19"
      int idx = line.lastIndexOf(" ");
      if (idx > 0) {
        float dp = line.substring(idx).toFloat();
        float ias = computeIAS(dp);
        
        // Store values
        current_dp = dp;
        current_ias = ias;
      }
    }
  }
}

float get_ias() {
    return current_ias;
}

void airspeed_output() {
    // $AIRSPEED,dp,ias*CS
    String msg = build_nmea("AIRSPEED", current_dp, current_ias);
    Serial.print(msg);
}