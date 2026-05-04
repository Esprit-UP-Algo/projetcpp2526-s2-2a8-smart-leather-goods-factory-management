/*
 * CUIREA - Systeme de Pointage RFID (INPUT + OUTPUT)
 * Affiche le nom de l'employe sur LCD
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN    10
#define RST_PIN   8
#define SERVO_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo servo;

void setup() {
  Serial.begin(9600);
  Serial.println("ROLE:POINTAGE");  // Identification automatique multi-cartes
  
  SPI.begin();
  rfid.PCD_Init();
  
  lcd.init();
  lcd.backlight();
  afficherAccueil();
  
  servo.attach(SERVO_PIN);
  servo.write(0);
  
  Serial.println("=== CUIREA Pointage RFID ===");
}

void afficherAccueil() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CUIREA Pointage");
  lcd.setCursor(0, 1);
  lcd.print("Scannez badge...");
}

void loop() {
  // Lire reponse de Qt
  if (Serial.available() > 0) {
    String reponse = Serial.readStringUntil('\n');
    reponse.trim();
    
    if (reponse.startsWith("OK:")) {
      // Acces autorise - Afficher nom
      String nom = reponse.substring(3);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bienvenue!");
      lcd.setCursor(0, 1);
      lcd.print(nom.substring(0, 16)); // Max 16 caracteres
      
      // Ouvrir porte
      servo.write(90);
      delay(3000);
      servo.write(0);
      
      afficherAccueil();
    }
    else if (reponse.startsWith("BYE:")) {
      // Sortie - Afficher nom
      String nom = reponse.substring(4);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Au revoir!");
      lcd.setCursor(0, 1);
      lcd.print(nom.substring(0, 16));
      
      // Ouvrir porte
      servo.write(90);
      delay(3000);
      servo.write(0);
      
      afficherAccueil();
    }
    else if (reponse == "NO") {
      // Badge inconnu
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Acces refuse!");
      lcd.setCursor(0, 1);
      lcd.print("Badge inconnu");
      
      delay(2000);
      afficherAccueil();
    }
  }
  
  // Detecter carte RFID
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }
  
  // Construire UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();
  
  // Afficher "Verification..."
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Verification...");
  
  // Envoyer UID a Qt
  Serial.println("UID:" + uid);
  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  
  delay(500);
}
