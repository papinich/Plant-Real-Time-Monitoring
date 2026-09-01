#define BLYNK_TEMPLATE_ID "TMPL6ohMF7zhD"
#define BLYNK_TEMPLATE_NAME "KIR"
#define BLYNK_AUTH_TOKEN "Ql7yEQo-jhAQ-6TALakJCSaVNJc1M-Lx"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <Blynk.h>
#include <BlynkSimpleESP8266.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

char ssid[] = "Hanif"; 
char pass[] = "121206281006"; 

void setup() {
 Serial.begin(9600);
 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  if (Serial.available()) {
    String data = Serial.readString();
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, data);

    if (!error) {
      int soil11 = doc["soil1"];
      int soil22 = doc["soil2"];
      float temp = doc["temp"];
      float hum = doc["hum"];
      String rain1 = doc["rain"];

      Blynk.virtualWrite(V8, soil11);
      Blynk.virtualWrite(V9, soil22);
      Blynk.virtualWrite(V5, temp);
      Blynk.virtualWrite(V6, hum);
      Blynk.virtualWrite(V10, rain1);
    }
  }
}
