#include <Servo.h>
#include <Keypad.h>

// ── Servos ────────────────────────────────────────────────────
Servo bras;      // broche 9  — pousse le colis
Servo bloqueur;  // broche 10 — bloque/libère

// ── Keypad 4x4 ───────────────────────────────────────────────
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 4, 5};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ── Pins ──────────────────────────────────────────────────────
const int PIN_LED_ERR = 7;

// ── Buffer saisie ID ─────────────────────────────────────────
String inputBuffer = "";

// ─────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);

    bras.attach(9);
    bloqueur.attach(10);
    bras.write(90);      // position repos
    bloqueur.write(90);  // position bloquée

    pinMode(PIN_LED_ERR, OUTPUT);
    digitalWrite(PIN_LED_ERR, LOW);
}

// ─────────────────────────────────────────────────────────────
void loop() {

    // ── 1. Réponse reçue depuis Qt ────────────────────────────
    if (Serial.available() > 0) {
        char rep = Serial.read();
        if (rep == '1') {
            expedierColis();
        } else if (rep == '0') {
            signalerErreur();
        }
    }

    // ── 2. Saisie clavier ─────────────────────────────────────
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
        } else {
            // Ajouter caractère au buffer
            inputBuffer += key;
            Serial.println("INPUT:" + inputBuffer);
        }
    }
}

// ─────────────────────────────────────────────────────────────
// Expédier un colis (séquence bras + bloqueur)
// ─────────────────────────────────────────────────────────────
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

    Serial.write('1');
}

