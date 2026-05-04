/**
 * CUIREA Arduino - Monitoring Température + Capteur Poids
 * Rôle : TEMP_BALANCE (forcé via .env — COM7:TEMP_BALANCE)
 *
 * Capteurs:
 * - DHT11 #1 (pin 2): Température matière première
 * - DHT11 #2 (pin 3): Température ambiante/météo
 * - HX711 (DOUT=4, SCK=5): Capteur de poids
 *
 * Actionneurs:
 * - LED Rouge (pin 6): Alerte température
 *
 * Commandes reçues : GET_TEMP | GET_WEIGHT | TARE | LED_RED | LED_OFF
 * Messages envoyés : TEMP:xx.x,yy.y | WEIGHT:xx.xx | ERROR:xxx
 */

#include <DHT.h>
#include <HX711.h>

// ── DHT11 ────────────────────────────────────────────────────────────────────
#define DHT_MATIERE_PIN  2
#define DHT_AMBIANCE_PIN 3
#define DHT_TYPE         DHT11

DHT dhtMatiere (DHT_MATIERE_PIN,  DHT_TYPE);
DHT dhtAmbiance(DHT_AMBIANCE_PIN, DHT_TYPE);

// ── HX711 ────────────────────────────────────────────────────────────────────
#define HX711_DOUT 4
#define HX711_SCK  5
HX711 scale;

// ── LED Rouge ─────────────────────────────────────────────────────────────────
#define LED_RED 6

// ── État ──────────────────────────────────────────────────────────────────────
bool readingWeight = false;
unsigned long lastTempRead = 0;
const unsigned long TEMP_INTERVAL = 2000;

// ─────────────────────────────────────────────────────────────────────────────
float readWeight() {
  if (!scale.is_ready()) return -1;
  float weight = scale.read_average(10) / 1000.0;
  return weight;
}

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  delay(2000);

  dhtMatiere.begin();
  dhtAmbiance.begin();

  scale.begin(HX711_DOUT, HX711_SCK);
  if (scale.is_ready()) {
    scale.tare();
  }

  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);

  Serial.println("READY");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  // Envoi température périodique
  if (millis() - lastTempRead >= TEMP_INTERVAL) {
    lastTempRead = millis();
    readAndSendTemperature();
  }

  // Lecture poids si demandée
  if (readingWeight) {
    float weight = readWeight();
    if (weight < 0) weight = 0;
    Serial.print("WEIGHT:");
    Serial.println(weight, 2);
    readingWeight = false;
  }

  // Traitement commandes série
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    processCommand(command);
  }
}

// ─────────────────────────────────────────────────────────────────────────────
void readAndSendTemperature() {
  float tempMatiere  = dhtMatiere.readTemperature();
  float tempAmbiance = dhtAmbiance.readTemperature();

  if (isnan(tempMatiere) || isnan(tempAmbiance)) {
    Serial.println("ERROR:DHT_READ_FAILED");
    return;
  }

  Serial.print("TEMP:");
  Serial.print(tempMatiere,  1);
  Serial.print(",");
  Serial.println(tempAmbiance, 1);

  digitalWrite(LED_RED, (tempMatiere > tempAmbiance) ? HIGH : LOW);
}

// ─────────────────────────────────────────────────────────────────────────────
void processCommand(String cmd) {
  if (cmd == "GET_TEMP") {
    readAndSendTemperature();

  } else if (cmd == "GET_WEIGHT") {
    if (!scale.is_ready()) {
      Serial.println("ERROR:SCALE_NOT_READY");
    } else {
      readingWeight = true;  // Déclencher la lecture dans loop()
    }

  } else if (cmd == "TARE") {
    scale.tare();
    Serial.println("TARE_OK");

  } else if (cmd == "LED_RED") {
    digitalWrite(LED_RED, HIGH);
    Serial.println("LED_RED_ON");

  } else if (cmd == "LED_OFF") {
    digitalWrite(LED_RED, LOW);
    Serial.println("LED_OFF");

  } else {
    Serial.println("ERROR:UNKNOWN_CMD");
  }
}
