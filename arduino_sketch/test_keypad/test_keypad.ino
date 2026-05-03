/*
 * Test simple du Keypad 4x4
 * Ce sketch teste uniquement le keypad sans LCD ni servos
 */

#include <Keypad.h>

// Configuration Keypad 4x4
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','A','3'},
    {'4','5','B','6'},
    {'7','8','C','9'},
    {'*','0','D','#'}
};

// IMPORTANT : Vérifiez que ces pins correspondent à votre câblage réel
byte rowPins[ROWS] = {A0, A1, A2, A3};  // Lignes du keypad
byte colPins[COLS] = {A4, A5, 2, 3};    // Colonnes du keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(9600);
    Serial.println("=== TEST KEYPAD 4x4 ===");
    Serial.println("Appuyez sur une touche...");
    Serial.println();
    Serial.println("Configuration:");
    Serial.println("  Rows: A0, A1, A2, A3");
    Serial.println("  Cols: A4, A5, 2, 3");
    Serial.println();
}

void loop() {
    char key = keypad.getKey();
    
    if (key) {
        Serial.print("Touche appuyee : ");
        Serial.println(key);
        
        // Afficher le code ASCII pour debug
        Serial.print("  Code ASCII : ");
        Serial.println((int)key);
        Serial.println();
    }
}
