#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// ── LCD 16x2 (mode 4 bits, sans I2C) ─────────────────────────────────────────
// RS=12, EN=11, D4=6, D5=7, D6=8, D7=13
// Résistance 220Ω obligatoire entre 5V et Pin 15 (A) du LCD
LiquidCrystal lcd(12, 11, 6, 7, 8, 13);

// ── Servos ────────────────────────────────────────────────────────────────────
Servo bras;      // broche 9  — pousse le colis
Servo bloqueur;  // broche 10 — bloque/libère

// ── Keypad 4x4 ───────────────────────────────────────────────────────────────
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 3};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ── Buffer saisie ID ──────────────────────────────────────────────────────────
String inputBuffer = "";

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);

    // LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Systeme pret");
    lcd.setCursor(0, 1);
    lcd.print("Saisir ID + #");

    // Servos
    bras.attach(9);
    bloqueur.attach(10);
    bras.write(90);      // position repos
    bloqueur.write(90);  // position bloquée
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {

    // ── 1. Réponse reçue depuis Qt ────────────────────────────────────────────
    if (Serial.available() > 0) {
        char rep = Serial.read();
        if (rep == '1') {
            // Commande trouvée et terminée → expédier
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Expedition OK");
            expedierColis();
            // Retour accueil
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Systeme pret");
            lcd.setCursor(0, 1);
            lcd.print("Saisir ID + #");
        } else if (rep == '0') {
            // Erreur générale
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("!! ERREUR !!");
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Systeme pret");
            lcd.setCursor(0, 1);
            lcd.print("Saisir ID + #");
        } else if (rep == '2') {
            // ID invalide → rester en attente, pas de retour accueil
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ID invalide");
            lcd.setCursor(0, 1);
            lcd.print("Ressaisir + #");
            inputBuffer = "";
        }
    }

    // ── 2. Saisie clavier ─────────────────────────────────────────────────────
    char key = keypad.getKey();
    if (key) {
        if (key == '#') {
            // Confirmer → envoyer "ID:xxx" à Qt
            if (inputBuffer.length() > 0) {
                Serial.println("ID:" + inputBuffer);
                inputBuffer = "";
                Serial.println("CLEAR");
            }
        } else if (key == '*') {
            // Effacer
            inputBuffer = "";
            Serial.println("CLEAR");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Systeme pret");
            lcd.setCursor(0, 1);
            lcd.print("Saisir ID + #");
        } else {
            // Ajouter caractère au buffer
            inputBuffer += key;
            Serial.println("INPUT:" + inputBuffer);
            // Afficher saisie en cours sur LCD ligne 2
            lcd.setCursor(0, 1);
            lcd.print("                "); // effacer ligne 2
            lcd.setCursor(0, 1);
            lcd.print(inputBuffer);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Expédier un colis (séquence bras + bloqueur)
// ─────────────────────────────────────────────────────────────────────────────
void expedierColis() {
    // Étape 1 — bloqueur s'ouvre
    bloqueur.write(180);
    delay(800);
    // Étape 2 — bras pousse le colis
    bras.write(0);
    delay(1000);
    // Étape 3 — bras revient
    bras.write(90);
    delay(800);
    // Étape 4 — bloqueur se referme
    bloqueur.write(90);
    delay(500);

    Serial.write('1');  // confirmation Qt
}
