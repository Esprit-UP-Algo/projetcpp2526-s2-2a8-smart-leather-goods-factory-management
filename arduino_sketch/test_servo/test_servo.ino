#include <Servo.h>

Servo bras;

void setup() {
    Serial.begin(9600);
    bras.attach(9);
    bras.write(90);  // position repos
    delay(1000);
}

void loop() {
    // Attendre '1' sur le port série
    if (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '1') {
            // Séquence moteur — angle réduit pour test
            bras.write(45);   // pousse (45° au lieu de 0°)
            delay(1000);
            bras.write(90);   // revient
            delay(800);
            Serial.println("DONE");
        }
    }
}
