// Definition des pins

// Encodeur Rotatoire //
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 0;
volatile bool encoderChanged = false;

//Bluetooth
#include <SoftwareSerial.h>
#define RX 8  // Broche RX du module Bluetooth
#define TX 7  // Broche TX du module Bluetooth
#define ADC A0  // Broche de lecture de la résistance
SoftwareSerial bluetooth(TX, RX);  // Création d'un port série logiciel

// Ecran OLED
#include <Adafruit_SSD1306.h>
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

void setup() {
  Serial.begin(9600);

  // Encodeur Rotatoire
  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT); 
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);
  // Ecran OLED 
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);                               // Arrêt du programme (boucle infinie) si échec d'initialisation
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(3);                   // Taille des caractères (2:1)
  ecranOLED.setTextColor(SSD1306_WHITE); 
  ecranOLED.println("Config");
  ecranOLED.println("Mesure");
  ecranOLED.display();
  
  //Bluetooth
  bluetooth.begin(9600);  // Communication avec le module Bluetooth
  Serial.println("Module Bluetooth prêt");                          // Transfert le buffer à l'écran
}


void loop() {
  //Rotatif
  if (encoderChanged) {  // Si la valeur de l'encodeur a changé
    encoderChanged = false;
    afficherMenu();  // Afficher la nouvelle valeur
  }
  //Bluetooth
  int valeurBrute = analogRead(ADC);
    float tension = (valeurBrute / 1023.0) * 5.0;  // Conversion en tension (0-5V)
    Serial.print("Tension mesurée : ");
    Serial.print(tension);
    Serial.println(" V");

    bluetooth.print("Tension: ");
    bluetooth.print(tension);
    bluetooth.println(" V");

    delay(1000);  // Envoi toutes les secondes
}


void afficherMenu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(3);
  if (encoder0Pos % 2 == 0) {
      ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      ecranOLED.setCursor(20, 10); 
      ecranOLED.println("Config");
      ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      ecranOLED.setCursor(20, 40); 
      ecranOLED.println("Mesure");
  } else {
      ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      ecranOLED.setCursor(20, 10); 
      ecranOLED.println("Config");
      ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      ecranOLED.setCursor(20, 40); 
      ecranOLED.println("Mesure");
    }
    ecranOLED.display();
}


// Encodeur Rotatoire : Ajustement du cran de l'encodeur
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos--;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos++;
  } 
  encoderChanged = true;
  Serial.println(encoder0Pos);
}





