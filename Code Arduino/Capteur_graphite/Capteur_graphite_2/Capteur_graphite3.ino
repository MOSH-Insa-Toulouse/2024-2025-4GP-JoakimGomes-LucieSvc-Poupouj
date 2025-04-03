// Définition des pins
#define encoder0PinA  2  // CLK Output A
#define encoder0PinB  4  // DT Output B
#define Switch 5  // Bouton de l'encodeur

#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_I2C_ADDR 0x3C

Adafruit_SSD1306 ecranOLED(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

// Variables globales
volatile int encoder0Pos = 0;
volatile bool encoderChanged = false;

int menuState = 0;  // 0 = Menu principal, 1 = Config, 2 = Mesure
int selection = 0;  // Position de sélection (0, 1 ou 2)

bool lastButtonState = HIGH;
bool buttonPressed = false;

//Bluetooth
#include <SoftwareSerial.h>
#define RX 8  // Broche RX du module Bluetooth
#define TX 7  // Broche TX du module Bluetooth
#define ADC A0  // Broche de lecture de la résistance
SoftwareSerial bluetooth(TX, RX);  // Création d'un port série logiciel

//FlexSensor
#define FLEX_SENSOR_PIN A1
#include <Wire.h>
#include <Adafruit_GFX.h>

void setup() {
  Serial.begin(9600);

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(Switch, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    while (1);
  }

  afficherMenu();

<<<<<<< HEAD:Code Arduino/Capteur_graphite/Capteur_graphite_2/Capteur_graphite_2.ino
  //Flexsensor
  if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adresse I2C : 0x3C
        Serial.println("Échec de l'initialisation de l'écran OLED");
        for (;;);
    }

  // Ecran OLED 
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR))
    while(1);
  // Arrêt du programme (boucle infinie) si échec d'initialisation
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(3);                   // Taille des caractères (2:1)
  ecranOLED.setTextColor(SSD1306_WHITE); 
  ecranOLED.println("Config");
  ecranOLED.println("Mesure");
  ecranOLED.display();
  
=======
>>>>>>> 36da538ddb2458cd6bcdd9a603444d4445305723:Code Arduino/Capteur_graphite/Capteur_graphite_2/Capteur_graphite3.ino
  //Bluetooth
  bluetooth.begin(9600);  // Communication avec le module Bluetooth
  Serial.println("Module Bluetooth prêt");      
}

<<<<<<< HEAD:Code Arduino/Capteur_graphite/Capteur_graphite_2/Capteur_graphite_2.ino
=======
void loop() {
  if (encoderChanged) {
    encoderChanged = false;
    afficherMenu();
  }

  detecterAppuiBouton();

  //Bluetooth
  int valeurBrute = analogRead(ADC);
  float tension = (valeurBrute / 1024.0) * 5.0;  // Conversion en tension (0-5V)
  Serial.print("Tension mesurée : ");
  Serial.print(tension);
  Serial.println(" V");

  //bluetooth.print("Tension: ");
  bluetooth.print(tension);
  //bluetooth.println(" V");

  //FlexSensor
  int VFlexbrute = analogRead(FLEX_SENSOR_PIN);
  float VFlexSensor = (VFlexbrute / 1024.0) * 5.0; // Conversion en tension

  Serial.print("Valeur brute : ");
  Serial.print(VFlexbrute);
  Serial.print(" | Tension : ");
  Serial.print(VFlexSensor);
  Serial.println(" V");
}
>>>>>>> 36da538ddb2458cd6bcdd9a603444d4445305723:Code Arduino/Capteur_graphite/Capteur_graphite_2/Capteur_graphite3.ino

void detecterAppuiBouton() {
  bool buttonState = digitalRead(Switch);

  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50);
    if (digitalRead(Switch) == LOW) {
      buttonPressed = true;
    }
  }

  if (buttonState == HIGH && lastButtonState == LOW) {
    if (buttonPressed) {
      changerMenu();
      buttonPressed = false;
    }
  }

  lastButtonState = buttonState;
}

void loop() {
  if (encoderChanged) {
  //Rotatif
  if (encoderChanged) {  // Si la valeur de l'encodeur a changé
    encoderChanged = false;
    afficherMenu();
    afficherMenu();  // Afficher la nouvelle valeur
  }

  detecterAppuiBouton();
}




void afficherMenu() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2);

  if (menuState == 0) {  // Menu principal
    afficherOption("Config", 0);
    afficherOption("Mesure", 1);
  } else if (menuState == 1) {  // Sous-menu "Config"
    afficherOption("Option A", 0);
    afficherOption("Option B", 1);
    afficherOption("Retour", 2);
  } else if (menuState == 2) {  // Sous-menu "Mesure"
    afficherOption("Graphite", 0);
    afficherOption("Flex", 1);
    afficherOption("Retour", 2);
  }

  ecranOLED.display();
}

void afficherOption(const char *texte, int position) {
  if (selection % 3 == position) {
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  } else {
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  }
  ecranOLED.setCursor(20, 10 + (position * 20));
  ecranOLED.println(texte);
}

void changerMenu() {
  if (menuState == 0) {  // Menu principal
    menuState = (selection == 0) ? 1 : 2;
  } else {  // Sous-menus
    if (selection == 2) {
      menuState = 0;  // Retour au menu principal
    }
  }

  selection = 0;  // Réinitialiser la sélection
  afficherMenu();
}

void doEncoder() {
  if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH) {
    selection = (selection + 1) % 3;
  } else if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == LOW) {
    selection = (selection - 1 + 3) % 3;
  }

  encoderChanged = true;
}
