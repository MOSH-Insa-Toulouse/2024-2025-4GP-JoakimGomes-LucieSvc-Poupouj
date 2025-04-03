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

int menuState = 0;  // 0 = Menu principal, 1 = Config, 2 = Mesure, 3 = Flex, 4 = Graphite
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
#define GRAPHITE_SENSOR_PIN A0
#include <Wire.h>
#include <Adafruit_GFX.h>

const float R_DIV = 47000.0;
const float flatresistance = 25000.0;
const float bendresistance = 100000.0;

const float R5 = 10000.0;
const float R1 = 100000.0;


// === Déclaration de la fonction avant setup ===
void doEncoder();

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

}

void loop() {
  if (encoderChanged) {
    encoderChanged = false;
    afficherMenu();
  }

  detecterAppuiBouton();

  // Mise à jour de l'affichage des mesures Flex en temps réel
  if (menuState == 3) {
    afficherValeurFlex();
  }

  if (menuState == 4) {
    afficherValeurGraphite(40000.0);
  }

}

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
  } else if (menuState == 3) {  // Menu du capteur Flex
    afficherOption("Mesurer", 0);
    afficherOption("Retour", 1);
  } else if (menuState == 4) {  // Menu du capteur Graphite
    afficherOption("Mesurer", 0);
    afficherOption("Retour", 1);
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

void afficherValeurFlex() {
  while (menuState == 3) {  // Reste dans cette boucle tant que l'utilisateur ne sort pas
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(SSD1306_WHITE);

    int VFlexbrute = analogRead(FLEX_SENSOR_PIN);
    float VFlexSensor = (VFlexbrute / 1024.0) * 5.0; // Conversion en tension
    float RFlexSensor = R_DIV * (5.0 / VFlexSensor - 1.0);
    float angle = map(RFlexSensor, flatresistance, bendresistance, 0, 90.0);

    ecranOLED.setCursor(10, 0);
    ecranOLED.println("Flex Sensor:");

    ecranOLED.setCursor(10, 20);
    ecranOLED.print(VFlexSensor);
    ecranOLED.println(" V");

    ecranOLED.setCursor(10, 30);
    ecranOLED.print(RFlexSensor);
    ecranOLED.println(" Ohms");

    ecranOLED.setCursor(10, 40);
    ecranOLED.print(angle);
    ecranOLED.println(" degrees");

    ecranOLED.setCursor(10, 50);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println("Retour");

    ecranOLED.display();

    detecterAppuiBouton();  // Vérifie si l'utilisateur appuie sur "Retour"
    delay(200);  // Petite pause pour éviter les rafraîchissements trop rapides
  }
}

void afficherValeurGraphite(float Rpot) {
  while (menuState == 4) {  // Reste dans cette boucle tant que l'utilisateur ne sort pas
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(1);
    ecranOLED.setTextColor(SSD1306_WHITE);

    int VGraphiteBrute = analogRead(GRAPHITE_SENSOR_PIN);
    float VGraphiteSensor = (VGraphiteBrute / 1024.0) * 5.0; // Conversion en tension
    float RGraphiteSensor = (1 + R5 / Rpot) * R1 * (5.0/VGraphiteSensor) - R1 - R5;

    ecranOLED.setCursor(10, 0);
    ecranOLED.println("Graphite Sensor:");

    ecranOLED.setCursor(10, 30);
    ecranOLED.print(RGraphiteSensor);
    ecranOLED.println(" Ohms");

    ecranOLED.setCursor(10, 50);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println("Retour");

    ecranOLED.display();

    detecterAppuiBouton();  // Vérifie si l'utilisateur appuie sur "Retour"
    delay(200);  // Petite pause pour éviter les rafraîchissements trop rapides
  }
}

void changerMenu() {
  if (menuState == 0) {  // Menu principal
    menuState = (selection == 0) ? 1 : 2;
  } else if (menuState == 1 || menuState == 2) {  // Sous-menus Config / Mesure
    if (selection == 2) {
      menuState = 0;  // Retour au menu principal
    } else if (menuState == 2 && selection == 1) {
      menuState = 3;  // Aller au menu du capteur Flex
    } else if (menuState == 2 && selection == 0) {
      menuState = 4;  // Aller au menu du capteur Graphite
    }
  } else if (menuState == 3) {  // Menu du capteur Flex
    if (selection == 1) {  // Si "Retour" est sélectionné
      menuState = 2;  // Retour au menu "Mesure"
    } else {
      afficherValeurFlex();
    }
  } else if (menuState == 4) {  // Menu du capteur Graphite
    if (selection == 1) {  // Si "Retour" est sélectionné
      menuState = 2;  // Retour au menu "Mesure"
    } else {
      afficherValeurGraphite(40000.0);
    }
  }

  selection = 0;  // Réinitialiser la sélection
  afficherMenu();
}

void doEncoder() {
  int maxOptions = (menuState == 1 || menuState == 2) ? 3 : 2;  // 3 options pour Config/Mesure, 2 pour Flex/Graphite

  if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH) {
    selection = (selection + 1) % maxOptions;
  } else if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == LOW) {
    selection = (selection - 1 + maxOptions) % maxOptions;
  }

  encoderChanged = true;
}
