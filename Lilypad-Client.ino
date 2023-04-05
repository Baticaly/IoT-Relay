#include "DHT.h"
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
DynamicJsonDocument sensorData(200);

bool fallDetected = false;

void idleAnimation()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(75);
    digitalWrite(LED_BUILTIN, LOW);
    delay(250);
}

void setup()
{
    Serial.begin(9600);
    dht.begin();
}

void loop()
{
    if (Serial.available())
    {
        idleAnimation();

        // Temperature & Humidity
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        float hic = dht.computeHeatIndex(t, h, false);

        sensorData["temperature"] = t;
        sensorData["humidity"] = h;
        sensorData["heat_index"] = hic;

        // Send JSON package
        serializeJson(sensorData, Serial);
        delay(1000);
    }
}
