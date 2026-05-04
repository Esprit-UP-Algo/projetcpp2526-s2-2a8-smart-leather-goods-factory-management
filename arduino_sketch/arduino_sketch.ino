#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

// ── LCD 16x2 (mode 4 bits, sans I2C) ─────────────────────────────────────────
// RS=12, EN=11, D4=6, D5=7, D6=8, D7=4
// ⚠️  RW du LCD → GND (obligatoire)
// Résistance 220Ω entre 5V et Pin 15 (A) du LCD
LiquidCrystal lcd(12, 11, 6, 7, 8, 4);

// ── Servos ────────────────────────────────────────────────────────────────────
Servo bras;      // broche 9  — pousse le colis
Servo barriere;  // broche 10 — barrière (fermée=90°, ouverte=0°)

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

// ── État interne ──────────────────────────────────────────────────────────────
String inputBuffer     = "";
bool   waitingExpedier = false;  // true = ID validé, attente clic bouton Qt

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(9600);

    // ⚠️  Délai obligatoire : Qt ouvre le port APRÈS le démarrage de l'Arduino.
    //     Sans ce délai, le message ROLE: est envoyé avant que Qt écoute → perdu.
    //     2 secondes suffisent dans la plupart des cas.
    delay(2000);
    Serial.println("ROLE:LIVRAISON");  // Identification automatique multi-cartes

    // Servos — positions de repos d'abord (avant LCD pour éviter pic de courant)
    // ⚠️  Alimenter les servos sur source externe, GND commun avec Arduino
    bras.attach(9);
    bras.write(90);      // position repos
    barriere.attach(10);
    barriere.write(90);  // barrière fermée
    delay(500);          // laisser les servos se stabiliser

    // LCD — initialisé après les servos
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Systeme pret");
    lcd.setCursor(0, 1);
    lcd.print("Saisir ID + D");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {

    // ── 1. Réponse reçue depuis Qt ────────────────────────────────────────────
    if (Serial.available() > 0) {
        char rep = Serial.read();

        if (rep == '1') {
            // Statut Terminé confirmé + bouton Expédier cliqué → lancer les moteurs
            waitingExpedier = false;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Expedition OK");
            lcd.setCursor(0, 1);
            lcd.print("Barriere s ouvre");
            expedierColis();
            retourAccueil();

        } else if (rep == '0') {
            // Erreur générale
            waitingExpedier = false;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("!! ERREUR !!");
            delay(2000);
            retourAccueil();

        } else if (rep == '2') {
            // ID invalide → rester en attente de saisie
            waitingExpedier = false;
            inputBuffer = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ID invalide");
            lcd.setCursor(0, 1);
            lcd.print("Ressaisir + D");

        } else if (rep == '3') {
            // ID trouvé, statut mis à Terminé — attendre clic bouton Expédier Qt
            waitingExpedier = true;
            inputBuffer = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("ID OK - Termine");
            lcd.setCursor(0, 1);
            lcd.print("Cliquer Expedier");
        }
    }

    // ── 2. Saisie clavier (bloquée si on attend le bouton Expédier) ───────────
    if (waitingExpedier) return;

    char key = keypad.getKey();
    if (!key) return;

    if (key == 'D' || key == '#') {
        // D ou # → confirmer et envoyer "ID:xxx" à Qt
        if (inputBuffer.length() > 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Envoi ID...");
            lcd.setCursor(0, 1);
            lcd.print(inputBuffer);
            Serial.println("ID:" + inputBuffer);
            inputBuffer = "";
        } else {
            // Buffer vide → avertir
            lcd.setCursor(0, 0);
            lcd.print("Saisir ID d abord");
        }

    } else if (key == '*') {
        // Effacer buffer
        inputBuffer = "";
        Serial.println("CLEAR");
        retourAccueil();

    } else if ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'C')) {
        // Ajouter chiffre OU lettre A/B/C au buffer
        inputBuffer += key;
        Serial.println("INPUT:" + inputBuffer);

        // Ligne 1 : fixe pendant la saisie
        lcd.setCursor(0, 0);
        lcd.print("Saisir Ref + D  ");

        // Ligne 2 : buffer complet (max 16 chars LCD)
        lcd.setCursor(0, 1);
        lcd.print("                ");  // effacer ligne 2
        lcd.setCursor(0, 1);
        lcd.print(inputBuffer);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Retour à l'écran d'accueil
// ─────────────────────────────────────────────────────────────────────────────
void retourAccueil() {
    inputBuffer = "";
    waitingExpedier = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Systeme pret");
    lcd.setCursor(0, 1);
    lcd.print("Saisir ID + D");
}

// ─────────────────────────────────────────────────────────────────────────────
// Séquence expédition :
//   1. Barrière s'ouvre  (90° → 0°)
//   2. Bras pousse le colis (90° → 0°)
//   3. Bras revient      (0° → 90°)
//   4. Barrière se referme (0° → 90°)
// ─────────────────────────────────────────────────────────────────────────────
void expedierColis() {
    // Étape 1 — barrière s'ouvre
    barriere.write(0);
    delay(800);

    // Étape 2 — bras pousse le colis
    bras.write(0);
    delay(1000);

    // Étape 3 — bras revient en position repos
    bras.write(90);
    delay(800);

    // Étape 4 — barrière se referme
    barriere.write(90);
    delay(500);

    // Confirmation à Qt — println pour ligne complète (\n)
    Serial.println("1");
}
