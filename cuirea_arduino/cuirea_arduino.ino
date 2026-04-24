/*
 * ============================================================
 *  CUIREA — Sketch Arduino
 *  Compatible : Wokwi (simulation) + Carte réelle
 *
 *  #define SIMULATION 1  → Wokwi (potentiomètre A0)
 *  #define SIMULATION 0  → Carte réelle (HX711)
 *
 *  LEDs :
 *    Pin 7  → LED rouge  TEMPÉRATURE (alerte si T anormale)
 *    Pin 9  → LED rouge  POIDS (livraison NOK)
 *    Pin 10 → LED verte  POIDS (livraison OK)
 *
 *  Capteurs :
 *    Pin 2  → DHT22 (température)
 *    A0     → Potentiomètre (simulation balance Wokwi)
 *    Pin 3  → HX711 DOUT (balance réelle)
 *    Pin 4  → HX711 SCK  (balance réelle)
 * ============================================================
 */

#define SIMULATION 1     // ← 1 = Wokwi   |   0 = carte réelle

#include <DHT.h>
#if SIMULATION == 0
  #include <HX711.h>
#endif

// ── Brochage ──────────────────────────────────────────────
#define DHT_PIN           2
#define DHT_TYPE          DHT22

#define LED_TEMP_RED      7    // Rouge température
#define LED_WEIGHT_RED    9    // Rouge poids NOK
#define LED_WEIGHT_GREEN  10   // Verte poids OK

#if SIMULATION == 1
  #define WEIGHT_PIN      A0
#else
  #define HX711_DOUT      3
  #define HX711_SCK       4
  #define CALIBRATION     2280.0f
#endif

// ── Paramètres ────────────────────────────────────────────
#define WEIGHT_STABLE_DELTA  0.5f
#define WEIGHT_STABLE_COUNT  5
#define TEMP_MAX             30.0
#define TEMP_MIN              5.0

// ── Objets ────────────────────────────────────────────────
DHT dht(DHT_PIN, DHT_TYPE);
#if SIMULATION == 0
  HX711 scale;
#endif

// ── Variables état ────────────────────────────────────────
String inputBuffer   = "";
bool   readingWeight = false;
float  lastWeight    = 0.0f;
int    stableCount   = 0;

// ─────────────────────────────────────────────────────────
void setup()
{
    Serial.begin(9600);

    pinMode(LED_TEMP_RED,    OUTPUT);
    pinMode(LED_WEIGHT_RED,  OUTPUT);
    pinMode(LED_WEIGHT_GREEN,OUTPUT);

    digitalWrite(LED_TEMP_RED,    LOW);
    digitalWrite(LED_WEIGHT_RED,  LOW);
    digitalWrite(LED_WEIGHT_GREEN,LOW);

    dht.begin();

#if SIMULATION == 0
    scale.begin(HX711_DOUT, HX711_SCK);
    scale.set_scale(CALIBRATION);
    scale.tare();
#endif

    delay(2000);
    Serial.println("CUIREA_READY");
}

// ─────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────
float readWeight()
{
#if SIMULATION == 1
    return (float)analogRead(WEIGHT_PIN) * 100.0f / 1023.0f;
#else
    if (scale.is_ready()) return scale.get_units(3);
    return lastWeight;
#endif
}

// ─────────────────────────────────────────────────────────
void processCommand(String cmd)
{
    // ── SCÉNARIO 1 : Température ─────────────────────────
    if (cmd == "READ_TEMP") {
        float temp = dht.readTemperature();
        if (isnan(temp)) {
            Serial.println("TEMP:ERROR");
            return;
        }
        Serial.print("TEMP:");
        Serial.println(temp, 1);

        if (temp > TEMP_MAX || temp < TEMP_MIN) {
            // Anormale → LED rouge température allumée
            digitalWrite(LED_TEMP_RED, HIGH);
        } else {
            // Normale → LED rouge température éteinte
            digitalWrite(LED_TEMP_RED, LOW);
        }
    }

    // ── SCÉNARIO 2 : Pesée ───────────────────────────────
    else if (cmd == "READ_WEIGHT") {
        readingWeight = true;
        stableCount   = 0;
        lastWeight    = 0.0f;
        // Éteindre LEDs poids pendant la pesée
        digitalWrite(LED_WEIGHT_RED,   LOW);
        digitalWrite(LED_WEIGHT_GREEN, LOW);
#if SIMULATION == 0
        scale.tare();
#endif
        Serial.println("WEIGHT_READING_START");
    }

    // ── Commandes LED poids (Qt décide après comparaison) ─
    else if (cmd == "LED_RED") {
        // NOK : poids incorrect → LED rouge poids
        digitalWrite(LED_WEIGHT_RED,   HIGH);
        digitalWrite(LED_WEIGHT_GREEN, LOW);
        Serial.println("LED:RED");
    }
    else if (cmd == "LED_GREEN") {
        // OK : poids conforme → LED verte poids
        digitalWrite(LED_WEIGHT_RED,   LOW);
        digitalWrite(LED_WEIGHT_GREEN, HIGH);
        Serial.println("LED:GREEN");
    }
    else if (cmd == "LED_OFF") {
        digitalWrite(LED_WEIGHT_RED,   LOW);
        digitalWrite(LED_WEIGHT_GREEN, LOW);
        Serial.println("LED:OFF");
    }
}
