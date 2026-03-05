#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h> // Stellen Sie sicher, dass Sie die richtige ADS1115 Library installiert haben

// ADS1115 ADS(0x48);

// Erstellen des BluetoothSerial-Objekts
BluetoothSerial SerialBT;

// Erstellen des ADS1115-Objekts (nutzt Standard-I2C-Adresse 0x48)
Adafruit_ADS1115 ads; 

// Einstellbare Verstärkung (Gain): kann an Ihren Sensorbereich angepasst werden
// Beispiel: GAIN_TWOTHIRDS für +/- 6.144V (gut für 5V-Signale)
#define GAIN_SETTING GAIN_TWOTHIRDS 

// Definieren Sie die I2C-Pins, falls sie nicht die Standard-Pins sind (Standard: SDA=GPIO21, SCL=GPIO22)
#define I2C_SDA 21
#define I2C_SCL 22

String incoming = "0";

void setup() {
  Serial.begin(115200);
  
  // Initialisiere ADS1115
  Wire.begin(); // Optional: Wenn Sie benutzerdefinierte I2C-Pins verwenden: Wire.begin(I2C_SDA, I2C_SCL);
  
  
 if (!ads.begin(0x48)) {
    Serial.println("Fehler beim Initialisieren des ADS1115!");
    while (1);
  }

  
  // Setze die Gain-Einstellung
  ads.setGain(GAIN_SETTING);
  
  int16_t adc0 = ads.readADC_SingleEnded(0);
  int16_t adc1 = ads.readADC_SingleEnded(1);

 String dataString = String(adc0) + "," + String(adc1); 

 // Serial.println(dataString);
Serial.println(adc0);
Serial.println(adc1);


  Serial.println();


  // Starte Bluetooth Classic SPP mit einem Namen
  SerialBT.begin("SWR_Meter_ESP32"); // Ändern Sie den Namen, falls gewünscht
  Serial.println("BluetoothSerial gestartet. Gerät ist sichtbar.");
}

void loop() {
  // 1. ADS1115 Werte lesen
  // Gehen Sie davon aus, dass Forward-Power (Vf) auf A0 und Reverse-Power (Vr) auf A1 liegt
  int16_t adc0 = ads.readADC_SingleEnded(0);
  int16_t adc1 = ads.readADC_SingleEnded(1);

  // Beispiel: Umrechnung in Millivolt (für Debugging oder weitere Berechnung)
  // float mv0 = ads.computeVolts(adc0) * 1000.0;
  // float mv1 = ads.computeVolts(adc1) * 1000.0;

  // HINWEIS: Hier müssen Sie Ihre spezifische SWR-Berechnungslogik einfügen, 
  // die die ADC-Werte (adc0, adc1) in SWR und Leistung (oder nur die Rohdaten) umwandelt.
  
  // *** VEREINFACHUNG: Wir senden die beiden Rohwerte, getrennt durch ein Komma. ***
  // Das Android-Gerät muss diese Werte parsen und die SWR/Leistung anzeigen.
  
  // Erstellen des Datenstrings im Format "WERT_A0,WERT_A1"
  String dataString = String(adc0) + "," + String(adc1); 

  // 2. Daten senden, falls verbunden
  if (SerialBT.hasClient()) {
    SerialBT.println(incoming);
    SerialBT.println(dataString);
   // Sende den String über Bluetooth
   // Serial.print("Gesendet: ");
   // Serial.println(dataString);
  }
  
  // 3. Optional: Empfangene Daten vom Telefon verarbeiten
  if (SerialBT.available()) {
    incoming = SerialBT.readStringUntil('\n');
    Serial.print("Empfangen: ");
   // Serial.println(incoming);
    // Hier könnten Sie Logik für Steuerung oder Status-Anfragen einfügen
    if (incoming == "0") {
      Serial.println("Antenne 1");
    }
    if (incoming == "1") {
      Serial.println("Antenne 2");
    }
  }

  delay(200); // Sendeintervall (z.B. alle 500ms)
}


