#define BLYNK_TEMPLATE_ID "BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "BLYNK_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "BLYNK_AUTH_TOKEN"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>

char ssid[] = "WIFI-SSID";
char pass[] = "WIFI-PASSWORD";

BlynkTimer timer;
DynamicJsonDocument sensorData(200);

void uploadData()
{
    if (Serial.available())
    {
        String teststr = Serial.readString();
        teststr.trim();
        DeserializationError error = deserializeJson(sensorData, teststr);

        double temperature = sensorData["temperature"];
        double humidity = sensorData["humidity"];
        double heat_index = sensorData["heat_index"];
        int bpm = sensorData["bpm"];
        int fall_detected = sensorData["fall_detected"];
        int emergency_button = sensorData["emergency_button"];

        Blynk.virtualWrite(V0, millis() / 1000);
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);
        Blynk.virtualWrite(V3, heat_index);
        Blynk.virtualWrite(V4, bpm);
        Blynk.virtualWrite(V5, fall_detected);
        Blynk.virtualWrite(V6, emergency_button);
        Blynk.virtualWrite(V7, error.f_str());
    }
}

void setup()
{
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    timer.setInterval(1000L, uploadData);

    Serial.begin(9600);
    Serial.write("Connection init");
}

void loop()
{
    Blynk.run();
    timer.run();
}
