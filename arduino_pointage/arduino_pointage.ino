// ====================================================
// SYSTÈME DE POINTAGE RFID AVEC ARDUINO
// Compatible avec l'atelier existant
// ====================================================

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SERVO_PIN  6
#define RFID_RST   3
#define RFID_SS    10

MFRC522 rfid(RFID_SS, RFID_RST);
Servo   barriere;

// UID du badge autorisé (à adapter selon vos badges)
byte badge_autorise[] = {0x12, 0xED, 0x19, 0x06};

void setup() {
    // Conforme atelier : Serial.begin(9600)
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    barriere.attach(SERVO_PIN);
    barriere.write(0);
    Serial.println("Systeme pret");
    Serial.println("Scannez votre badge...");
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent()) return;
    if (!rfid.PICC_ReadCardSerial())   return;
    
    // Construire l'UID formaté → envoie "UID:12ED1906"
    String uid = "UID:";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    Serial.println("UID:" + uid);
    
    // Vérification badge local
    bool valide = true;
    for (int i = 0; i < 4; i++) {
        if (rfid.uid.uidByte[i] != badge_autorise[i]) {
            valide = false;
            break;
        }
    }
    
    if (valide) {
        Serial.println("ACCES:OK");
        barriere.write(90);
        delay(4000);
        barriere.write(0);
    } else {
        Serial.println("ACCES:REFUSE");
        delay(2000);
    }
    
    rfid.PICC_HaltA();
    Serial.println("Scannez votre badge...");
    delay(200);
}
