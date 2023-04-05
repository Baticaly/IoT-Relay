#define DHTPIN 2
#define DHTTYPE DHT11
#define USE_ARDUINO_INTERRUPTS true

#include "DHT.h"
#include <ArduinoJson.h>

DynamicJsonDocument sensorData(200);

DHT dht(DHTPIN, DHTTYPE);

const int emergencyButton = 4;
const int pulsePin = A0;

volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

int accelerometerSample()
{
    long accelerometerX = analogRead(A1);
    long accelerometerY = analogRead(A2);
    long accelerometerZ = analogRead(A3);

    int total = accelerometerX + accelerometerY + accelerometerZ;

    return total;
}

void fallDetectAlert(int *total)
{
    int firstsample = accelerometerSample();
    delay(100);
    int secondsample = accelerometerSample();

    *total = abs(secondsample - firstsample);
}

void setup()
{
    Serial.begin(9600);
    interruptSetup();
    pinMode(emergencyButton, INPUT_PULLUP);
    dht.begin();
}

void loop()
{
    if (Serial.available())
    {
        // Temperature & Humidity
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        float hic = dht.computeHeatIndex(t, h, false);

        sensorData["temperature"] = t;
        sensorData["humidity"] = h;
        sensorData["heat_index"] = hic;

        // Pulse sensor
        int currentBPM = 0;
        if (QS == true)
        {
            sensorData["bpm"] = BPM;
            QS = false;
        }

        // Accelerometer
        int accelerometerTotal = 0;
        fallDetectAlert(&accelerometerTotal);
        sensorData["fall_detected"] = accelerometerTotal;

        // Emergency Button
        sensorData["emergency_button"] = false;
        if (digitalRead(emergencyButton) == LOW)
        {
            sensorData["emergency_button"] = true;
        }

        // Send JSON package
        serializeJson(sensorData, Serial);
        Serial.println(" ");
        delay(1000);
    }
}

volatile int rate[10];
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 530;
volatile int amp = 0;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;

void interruptSetup()
{
    TCCR2A = 0x02;
    TCCR2B = 0x06;
    OCR2A = 0X7C;
    TIMSK2 = 0x02;
    sei();
}

ISR(TIMER2_COMPA_vect)
{
    cli();
    Signal = analogRead(pulsePin);
    sampleCounter += 2;
    int N = sampleCounter - lastBeatTime;

    if (Signal < thresh && N > (IBI / 5) * 3)
    {
        if (Signal < T)
        {
            T = Signal;
        }
    }

    if (Signal > thresh && Signal > P)
    {
        P = Signal;
    }

    if (N > 250)
    {
        if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3))
        {
            Pulse = true;
            IBI = sampleCounter - lastBeatTime;
            lastBeatTime = sampleCounter;

            if (secondBeat)
            {
                secondBeat = false;
                for (int i = 0; i <= 9; i++)
                {
                    rate[i] = IBI;
                }
            }

            if (firstBeat)
            {
                firstBeat = false;
                secondBeat = true;
                sei();
                return;
            }

            word runningTotal = 0;

            for (int i = 0; i <= 8; i++)
            {
                rate[i] = rate[i + 1];
                runningTotal += rate[i];
            }

            rate[9] = IBI;
            runningTotal += rate[9];
            runningTotal /= 10;
            BPM = 60000 / runningTotal;
            QS = true;
        }
    }

    if (Signal < thresh && Pulse == true)
    {
        Pulse = false;
        amp = P - T;
        thresh = amp / 2 + T;
        P = thresh;
        T = thresh;
    }

    if (N > 2500)
    {
        thresh = 530;
        P = 512;
        T = 512;
        lastBeatTime = sampleCounter;
        firstBeat = true;
        secondBeat = false;
    }

    sei();
}
