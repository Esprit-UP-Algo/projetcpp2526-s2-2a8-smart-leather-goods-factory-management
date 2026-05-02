/**
 * CUIREA Arduino - Monitoring Température + Capteur Poids
 * 
 * Capteurs:
 * - DHT11 #1 (pin 2): Température matière première
 * - DHT11 #2 (pin 3): Température ambiante/météo
 * - HX711 (DOUT=4, SCK=5): Capteur de poids
 * 
 * Actionneurs:
 * - LED Rouge (pin 6): Alerte température
 */

#include <DHT.h>
#include <HX711.h>

// Configuration DHT11
#define DHT_MATIERE_PIN 2
#define DHT_AMBIANCE_PIN 3
#define DHT_TYPE DHT11

DHT dhtMatiere(DHT_MATIERE_PIN, DHT_TYPE);
DHT dhtAmbiance(DHT_AMBIANCE_PIN, DHT_TYPE);

// Configuration HX711
#define HX711_DOUT 4
#define HX711_SCK 5
HX711 scale;

// Configuration LED Rouge
#define LED_RED 6

// Variables globales
unsigned long lastTempRead = 0;
const unsigned long TEMP_INTERVAL = 2000;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println("ROLE:TEMP_BALANCE");  // Identification automatique multi-cartes
  Serial.println("=== CUIREA Arduino Starting ===");
  
  // Initialiser DHT
  Serial.println("Init DHT sensors...");
  dhtMatiere.begin();
  dhtAmbiance.begin();
  Serial.println("DHT OK");
  
  // Initialiser HX711
  Serial.println("Init HX711...");
  scale.begin(HX711_DOUT, HX711_SCK);
  if (scale.is_ready()) {
    Serial.println("HX711 OK");
  } else {
    Serial.println("HX711 not detected");
  }
  
  // Initialiser LED Rouge
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, LOW);
  
  Serial.println("=== CUIREA Arduino Ready ===");
  Serial.println("Commands: GET_TEMP, GET_WEIGHT, LED_RED, LED_OFF");
}

void loop() {
  // Lecture température périodique
  if (millis() - lastTempRead >= TEMP_INTERVAL) {
    lastTempRead = millis();
    readAndSendTemperature();
  }
  
  // Traiter commandes série
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    processCommand(command);
  }
}

void readAndSendTemperature() {
  float tempMatiere = dhtMatiere.readTemperature();
  float tempAmbiance = dhtAmbiance.readTemperature();
  
  if (isnan(tempMatiere) || isnan(tempAmbiance)) {
    Serial.println("ERROR:DHT_READ_FAILED");
    return;
  }
  
  Serial.print("TEMP:");
  Serial.print(tempMatiere, 1);
  Serial.print(",");
  Serial.println(tempAmbiance, 1);
  
  if (tempMatiere > tempAmbiance) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }
}

void processCommand(String cmd) {
  if (cmd == "GET_TEMP") {
    readAndSendTemperature();
  }
  else if (cmd == "GET_WEIGHT") {
    if (scale.is_ready()) {
      long raw = scale.read_average(10);
      float weight_kg = raw / 1000000.0; // Convertir en kg
      Serial.print("WEIGHT:");
      Serial.println(weight_kg, 2);
    } else {
      Serial.println("ERROR:SCALE_NOT_READY");
    }
  }
  else if (cmd == "LED_RED") {
    digitalWrite(LED_RED, HIGH);
    Serial.println("LED_RED_ON");
  }
  else if (cmd == "LED_OFF") {
    digitalWrite(LED_RED, LOW);
    Serial.println("LED_OFF");
  }
  else {
    Serial.println("Unknown command");
  }
}
