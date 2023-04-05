#define BLYNK_TEMPLATE_ID "Template ID"
#define BLYNK_TEMPLATE_NAME "Template Name"
#define BLYNK_AUTH_TOKEN "Auth Token"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>

char ssid[] = "WIFI-SSID";
char pass[] = "WIFI-PASSWORD";

BlynkTimer timer;
DynamicJsonDocument sensorData(200);

void connectionAnimation()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
    delay(75);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
}

void idleAnimation()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
}

void uploadData()
{
    if (Serial.available())
    {
        idleAnimation();

        String teststr = Serial.readString();
        teststr.trim();

        DeserializationError error = deserializeJson(sensorData, teststr);

        double temperature = sensorData["temperature"];
        double humidity = sensorData["humidity"];
        double heat_index = sensorData["heat_index"];

        Blynk.virtualWrite(V0, millis() / 1000);
        Blynk.virtualWrite(V1, temperature);
        Blynk.virtualWrite(V2, humidity);
        Blynk.virtualWrite(V9, error.f_str());
    }
}

void setup()
{
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    timer.setInterval(1000L, uploadData);

    Serial.begin(9600);
    connectionAnimation();
    Serial.write("Connection init");
}

void loop()
{
    Blynk.run();
    timer.run();
}
