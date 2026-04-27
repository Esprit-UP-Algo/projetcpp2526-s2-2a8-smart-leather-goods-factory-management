/*
 * CUIREA - Systeme de Pointage RFID (INPUT + OUTPUT)
 * 
 * INPUT:  Module RFID RC522 (lecture carte)
 * OUTPUT: LCD I2C 16x2 + Servo-moteur
 * 
 * Cablage RC522:
 *   SDA  -> Pin 10
 *   SCK  -> Pin 13
 *   MOSI -> Pin 11
 *   MISO -> Pin 12
 *   RST  -> Pin 8
 *   3.3V -> 3.3V
 *   GND  -> GND
 * 
 * Cablage LCD I2C:
 *   SDA  -> A4
 *   SCL  -> A5
 *   VCC  -> 5V
 *   GND  -> GND
 * 
 * Cablage Servo:
 *   Signal -> Pin 9
 *   VCC    -> 5V
 *   GND    -> GND
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Pins
#define SS_PIN    10
#define RST_PIN   8
#define SERVO_PIN 9

// Objets
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

void setup() {
  Serial.begin(9600);
  
  // Init SPI et RFID
  SPI.begin();
  rfid.PCD_Init();
  
  // Init LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUIREA Pointage");
  lcd.setCursor(0, 1);
  lcd.print("Scannez badge...");
  
  // Init Servo (position fermee)
  servo.attach(SERVO_PIN);
  servo.write(0);
  
  Serial.println("=== CUIREA Pointage RFID ===");
  Serial.println("En attente de badge...");
}

void loop() {
  // Verifier reponse de Qt (1 = autorise, 2 = refuse)
  if (Serial.available() > 0) {
    char reponse = Serial.read();
    
    if (reponse == '1') {
      // Acces autorise
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acces autorise");
      lcd.setCursor(0, 1);
      lcd.print("Bienvenue!");
      
      // Ouvrir la porte (servo a 90 degres)
      servo.write(90);
      delay(3000);
      servo.write(0);
      
      // Retour ecran accueil
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CUIREA Pointage");
      lcd.setCursor(0, 1);
      lcd.print("Scannez badge...");
    }
    else if (reponse == '2') {
      // Acces refuse
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acces refuse!");
      lcd.setCursor(0, 1);
      lcd.print("Badge inconnu");
      
      delay(2000);
      
      // Retour ecran accueil
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CUIREA Pointage");
      lcd.setCursor(0, 1);
      lcd.print("Scannez badge...");
    }
  }
  
  // Verifier si une carte est presente
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Lire la carte
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  // Construire l'UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  // Afficher sur LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Badge detecte");
  lcd.setCursor(0, 1);
  lcd.print(uid);
  
  // Envoyer l'UID a Qt
  Serial.println("UID:" + uid);
  
  // Arreter la communication avec la carte
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
  delay(500);
}
