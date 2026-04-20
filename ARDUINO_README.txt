═══════════════════════════════════════════════════════════════════════════
  CONNEXION QT-ARDUINO - GUIDE D'UTILISATION
═══════════════════════════════════════════════════════════════════════════

📦 FICHIERS CRÉÉS:
  • arduino.h         - Classe de connexion Arduino (header)
  • arduino.cpp       - Implémentation de la connexion
  • mainwindow.h      - Ajout du slot onTestArduino()
  • mainwindow.cpp    - Implémentation du test de connexion
  • sans_titre.pro    - Ajout de serialport et fichiers Arduino

═══════════════════════════════════════════════════════════════════════════

🔧 FONCTIONNALITÉS DISPONIBLES:

1. CLASSE Arduino:
   ✓ connect_arduino()      - Connecte à l'Arduino automatiquement
   ✓ close_arduino()        - Ferme la connexion
   ✓ isConnected()          - Vérifie si connecté
   ✓ write_to_arduino()     - Envoie des données
   ✓ read_from_arduino()    - Lit les données
   ✓ getPortName()          - Retourne le port COM utilisé

2. DÉTECTION AUTOMATIQUE:
   • Recherche par VendorID Arduino (9025)
   • Teste les ports COM3 à COM8
   • Baud rate: 9600
   • Configuration: 8N1 (8 bits, No parity, 1 stop bit)

3. FONCTION DE TEST:
   • MainWindow::onTestArduino()
   • Affiche un message avec le résultat de connexion
   • Teste et ferme automatiquement la connexion

═══════════════════════════════════════════════════════════════════════════

💡 COMMENT UTILISER:

1. DANS L'INTERFACE (mainwindow.ui):
   Ajoutez un bouton et connectez-le au slot:
   
   connect(ui->btnTestArduino, &QPushButton::clicked, 
           this, &MainWindow::onTestArduino);

2. DANS VOTRE CODE:
   
   // Créer une instance
   Arduino arduino;
   
   // Se connecter
   if (arduino.connect_arduino() == 0) {
       // Envoyer des données
       arduino.write_to_arduino("LED_ON\n");
       
       // Lire des données
       QByteArray data = arduino.read_from_arduino();
       qDebug() << "Reçu:" << data;
       
       // Fermer
       arduino.close_arduino();
   }

═══════════════════════════════════════════════════════════════════════════

📝 EXEMPLE CODE ARDUINO:

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        
        if (cmd == "LED_ON") {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("OK:LED_ON");
        }
        else if (cmd == "LED_OFF") {
            digitalWrite(LED_BUILTIN, LOW);
            Serial.println("OK:LED_OFF");
        }
        else if (cmd == "STATUS") {
            Serial.println("STATUS:READY");
        }
    }
}

═══════════════════════════════════════════════════════════════════════════

✅ PRÊT À UTILISER!

La connexion Qt-Arduino est maintenant intégrée dans votre projet.
Vous pouvez appeler onTestArduino() pour tester la connexion.

═══════════════════════════════════════════════════════════════════════════
