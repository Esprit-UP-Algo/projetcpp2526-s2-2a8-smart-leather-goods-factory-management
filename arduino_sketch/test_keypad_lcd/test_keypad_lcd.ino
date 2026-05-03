/*
 * Test Keypad 4x4 + LCD 16x2
 * Chaque touche pressée s'affiche en temps réel sur le LCD
 *
 * ── Câblage LCD (mode 4 bits, sans I2C) ──────────────────────────────────────
 *   RS=12, EN=11, D4=6, D5=7, D6=8, D7=4
 *   RW du LCD → GND (obligatoire)
 *   Résistance 220Ω entre 5V et Pin 15 (A) du LCD
 *
 * ── Câblage Keypad 4x4 ───────────────────────────────────────────────────────
 *   Lignes  : A0, A1, A2, A3
 *   Colonnes: A4, A5, 2, 3
 */

#include <Keypad.h>
#include <LiquidCrystal.h>

// ── LCD ───────────────────────────────────────────────────────────────────────
LiquidCrystal lcd(12, 11, 6, 7, 8, 4);

// ── Keypad 4x4 ────────────────────────────────────────────────────────────────
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','A','3'},
    {'4','5','B','6'},
    {'7','8','C','9'},
    {'*','0','D','#'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ── Variables ─────────────────────────────────────────────────────────────────
String inputBuffer = "";   // ce que l'utilisateur a tapé
int    cursorPos   = 0;    // position curseur sur la ligne 2

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);
    Serial.println("ROLE:LIVRAISON");  // Identification automatique multi-cartes

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Test Keypad+LCD");
    lcd.setCursor(0, 1);
    lcd.print("Appuyez...");

    Serial.println("=== TEST KEYPAD + LCD ===");
    Serial.println("* = effacer  |  # = valider");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
    char key = keypad.getKey();
    if (!key) return;

    Serial.print("Touche : ");
    Serial.println(key);

    if (key == '*') {
        // ── Effacer tout ──────────────────────────────────────────────────────
        inputBuffer = "";
        cursorPos   = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Efface !");
        lcd.setCursor(0, 1);
        lcd.print("                ");  // vider ligne 2
        delay(600);
        lcd.setCursor(0, 0);
        lcd.print("Test Keypad+LCD");

    } else if (key == '#') {
        // ── Valider / afficher le résultat ────────────────────────────────────
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saisi : ");
        lcd.print(inputBuffer);
        Serial.print("Valide : ");
        Serial.println(inputBuffer);
        delay(2000);

        // Retour à l'écran d'accueil
        inputBuffer = "";
        cursorPos   = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Test Keypad+LCD");
        lcd.setCursor(0, 1);
        lcd.print("Appuyez...");

    } else {
        // ── Ajouter le caractère au buffer ────────────────────────────────────
        if (cursorPos < 16) {
            inputBuffer += key;

            // Ligne 1 : afficher la dernière touche pressée
            lcd.setCursor(0, 0);
            lcd.print("Touche : ");
            lcd.print(key);
            lcd.print("       ");  // effacer le reste

            // Ligne 2 : afficher tout le buffer
            lcd.setCursor(0, 1);
            lcd.print("                ");  // effacer ligne 2
            lcd.setCursor(0, 1);
            lcd.print(inputBuffer);

            cursorPos++;
        } else {
            // Ligne pleine → avertissement
            lcd.setCursor(0, 0);
            lcd.print("Ligne pleine!   ");
            Serial.println("Buffer plein (16 chars max) - appuyez * pour effacer");
        }
    }
}
