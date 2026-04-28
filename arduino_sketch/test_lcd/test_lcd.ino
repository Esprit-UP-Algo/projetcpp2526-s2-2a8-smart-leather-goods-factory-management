#include <LiquidCrystal.h>

// RS=12, EN=11, D4=6, D5=7, D6=8, D7=4
LiquidCrystal lcd(12, 11, 6, 7, 8, 4);

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TEST LCD OK");
    lcd.setCursor(0, 1);
    lcd.print("Ligne 2 OK");
}

void loop() {
    // rien
}
