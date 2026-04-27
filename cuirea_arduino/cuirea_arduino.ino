/*
 * ============================================================
 *  CUIREA — Sketch Arduino
 *  #define SIMULATION 0  → Carte réelle
 *
 *  LED rouge pin 7 :
 *    Allumée → alerte température OU poids NOK
 *    Éteinte → normal / poids OK
 * ============================================================
 */

#define SIMULATION 0

#include <DHT.h>
#if SIMULATION == 0
  #include <HX711.h>
#endif

#define DHT_PIN     2
#define DHT_TYPE    DHT11
#define LED_RED_PIN 7    // LED rouge partagée (temp + poids)

#if SIMULATION == 1
  #define WEIGHT_PIN  A0
#else
  #define HX711_DOUT  3
  #define HX711_SCK   4
  #define CALIBRATION 13657200.0f
#endif

#define WEIGHT_STABLE_DELTA  0.1f
#define WEIGHT_STABLE_COUNT  8
#define TEMP_MAX             28.0
#define TEMP_MIN              5.0

DHT dht(DHT_PIN, DHT_TYPE);
#if SIMULATION == 0
  HX711 scale;
#endif

String inputBuffer   = "";
bool   readingWeight = false;
float  lastWeight    = 0.0f;
int    stableCount   = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_RED_PIN, LOW);
    dht.begin();
#if SIMULATION == 0
    scale.begin(HX711_DOUT, HX711_SCK);
    scale.set_scale(CALIBRATION);
    scale.tare();
#endif
    delay(2000);
    Serial.println("CUIREA_READY");
}

void loop()
{
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n') {
            inputBuffer.trim();
            if (inputBuffer.length() > 0)
                processCommand(inputBuffer);
            inputBuffer = "";
        } else if (c != '\r') {
            inputBuffer += c;
        }
    }

    if (readingWeight) {
        float weight = readWeight();
        if (weight < 0) weight = 0;
        Serial.print("WEIGHT:");
        Serial.println(weight, 2);
        if (abs(weight - lastWeight) <= WEIGHT_STABLE_DELTA) {
            stableCount++;
            if (stableCount >= WEIGHT_STABLE_COUNT) {
                Serial.print("WEIGHT_STABLE:");
                Serial.println(weight, 2);
                readingWeight = false;
                stableCount   = 0;
            }
        } else {
            stableCount = 0;
        }
        lastWeight = weight;
        delay(500);
    }
}

float readWeight()
{
#if SIMULATION == 1
    return (float)analogRead(WEIGHT_PIN) * 100.0f / 1023.0f;
#else
    if (!scale.is_ready()) return lastWeight;
    float w = scale.get_units(10);
    if (w < 0) w = 0;
    return w;
#endif
}

void processCommand(String cmd)
{
    if (cmd == "READ_TEMP") {
        float temp = dht.readTemperature();
        if (isnan(temp)) {
            Serial.println("TEMP:ERROR");
            return;
        }
        Serial.print("TEMP:");
        Serial.println(temp, 1);
        if (temp > TEMP_MAX || temp < TEMP_MIN) {
            digitalWrite(LED_RED_PIN, HIGH);
        } else {
            digitalWrite(LED_RED_PIN, LOW);
        }
    }
    else if (cmd == "READ_WEIGHT") {
        readingWeight = true;
        stableCount   = 0;
        lastWeight    = 0.0f;
        digitalWrite(LED_RED_PIN, LOW);
#if SIMULATION == 0
        scale.tare();
#endif
        Serial.println("WEIGHT_READING_START");
    }
    else if (cmd == "LED_RED") {
        digitalWrite(LED_RED_PIN, HIGH);
        Serial.println("LED:RED");
    }
    else if (cmd == "LED_GREEN") {
        digitalWrite(LED_RED_PIN, LOW);
        Serial.println("LED:GREEN");
    }
    else if (cmd == "LED_OFF") {
        digitalWrite(LED_RED_PIN, LOW);
        Serial.println("LED:OFF");
    }
}
