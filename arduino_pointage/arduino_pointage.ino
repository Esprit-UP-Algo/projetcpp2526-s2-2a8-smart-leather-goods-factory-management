/*
 * ============================================================
 * CUIREA - Systeme de Pointage RFID
 * ============================================================
 * 
 * COMPOSANTS :
 *   - Module RFID RC522 (lecture carte)
 *   - Ecran LCD I2C 16x2 (affichage)
 *   - Servomoteur (porte)
 * 
 * CABLAGE :
 *   Module RC522 :
 *     SDA  -> Pin 10
 *     SCK  -> Pin 13
 *     MOSI -> Pin 11
 *     MISO -> Pin 12
 *     RST  -> Pin 8
 *     3.3V -> 3.3V (PAS 5V!)
 *     GND  -> GND
 * 
 *   LCD I2C :
 *     SDA  -> A4
 *     SCL  -> A5
 *     VCC  -> 5V
 *     GND  -> GND
 * 
 *   Servo :
 *     Signal -> Pin 9
 *     VCC    -> 5V
 *     GND    -> GND
 * 
 * ============================================================
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Broches
#define RFID_SS_PIN   10
#define RFID_RST_PIN  8
#define PIN_SERVO     9

// Objets
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

String inputBuffer = "";

// ============================================================
// SETUP
// ============================================================
void setup() {
  Serial.begin(9600);
  
  // RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // LCD
  lcd.init();
  lcd.backlight();
  afficherAccueil();
  
  // Servo (position fermee)
  servo.attach(PIN_SERVO);
  servo.write(0);
  
  Serial.println("RFID:READY");
}

// ============================================================
// LOOP
// ============================================================
void loop() {
  // Detection carte RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = lireUID();
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lecture...");
    lcd.setCursor(0, 1);
    lcd.print(uid);
    
    // Envoyer a Qt
    Serial.println("UID:" + uid);
    
    // Attendre reponse Qt
    String reponse = attendreReponseQt(5000);
    
    if (reponse.length() > 0) {
      traiterReponse(reponse);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erreur systeme");
      delay(2000);
      afficherAccueil();
    }
    
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
  
  // Commandes directes de Qt
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer.trim();
      if (inputBuffer.length() > 0) {
        traiterReponse(inputBuffer);
      }
      inputBuffer = "";
    } else {
      inputBuffer += c;
    }
  }
}

// ============================================================
// FONCTIONS
// ============================================================

String lireUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  return uid;
}

String attendreReponseQt(unsigned long timeout) {
  String reponse = "";
  unsigned long startTime = millis();
  
  while (millis() - startTime < timeout) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '\n') {
        reponse.trim();
        return reponse;
      } else {
        reponse += c;
      }
    }
  }
  return "";
}

void traiterReponse(String msg) {
  if (msg.startsWith("GRANTED:")) {
    int premierColon = msg.indexOf(':');
    int deuxiemeColon = msg.lastIndexOf(':');
    String prenom = msg.substring(premierColon + 1, deuxiemeColon);
    String type = msg.substring(deuxiemeColon + 1);
    
    if (type == "E") {
      sequenceEntree(prenom);
    } else {
      sequenceSortie(prenom);
    }
  } else if (msg == "DENIED") {
    sequenceRefus();
  }
}

void sequenceEntree(String prenom) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bienvenue");
  lcd.setCursor(0, 1);
  lcd.print(prenom);
  
  // Ouvrir porte
  servo.write(90);
  delay(5000);
  servo.write(0);
  
  delay(500);
  afficherAccueil();
}

void sequenceSortie(String prenom) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Au revoir");
  lcd.setCursor(0, 1);
  lcd.print(prenom);
  
  // Ouvrir porte
  servo.write(90);
  delay(5000);
  servo.write(0);
  
  delay(500);
  afficherAccueil();
}

void sequenceRefus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acces refuse");
  lcd.setCursor(0, 1);
  lcd.print("Carte inconnue");
  
  // Porte reste fermee
  delay(3000);
  afficherAccueil();
}

void afficherAccueil() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUIREA Pointage");
  lcd.setCursor(0, 1);
  lcd.print("Badgez SVP...");
}
