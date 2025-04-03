// Définition des pins
#define encoder0PinA  2  // CLK Output A
#define encoder0PinB  4  // DT Output B
#define Switch 5  // Bouton de l'encodeur

#include <Adafruit_SSD1306.h>
#include <SPI.h>

const byte csPin = 10;
const int maxPositions = 256;
const long rAB = 92500;
const byte rWiper = 125;
const byte pot0 = 0x11;
const byte pot0Shutdown = 0x21;
int potValue = 0;  // Valeur du potentiomètre digital (0 à 255)
bool editingPotValue = false;  // Indique si on est en train de régler la valeur

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

uint8_t ValeurResbitAppli;

//Bluetooth
#include <SoftwareSerial.h>
#define RX 8  // Broche RX du module Bluetooth
#define TX 7  // Broche TX du module Bluetooth
#define ADC A0  // Broche de lecture de la résistance
SoftwareSerial bluetooth(RX, TX);  // Création d'un port série logiciel

//FlexSensor
#define FLEX_SENSOR_PIN A1
#define GRAPHITE_SENSOR_PIN A0
#include <Wire.h>
#include <Adafruit_GFX.h>

const float R_DIV = 33000.0;
const float flatresistance = 33000.0;
const float bendresistance = 75000.0;

const float R5 = 10000.0;
const float R1 = 100000.0;


// === Déclaration de la fonction avant setup ===
void doEncoder();

void setup() {
  Serial.begin(9600);

  digitalWrite(csPin, HIGH);
  pinMode(csPin, OUTPUT);
  SPI.begin();

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);
  pinMode(Switch, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoder0PinA), doEncoder, CHANGE);

  if (!ecranOLED.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    while (1);
  }

  afficherMenu();  

  //Bluetooth
  pinMode(RX,INPUT); // On définit le pin Rx en INPUT
  pinMode(TX,OUTPUT); // On définit le pin Tx en OUTPUT
  pinMode(ADC,INPUT); // On définit l'entrée analogique A0 en INPUT, c'est celle qui va recevoir les valeurs de la résistance du capteur graphite
  bluetooth.begin(9600);  // Communication avec le module Bluetooth                 
  
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
    //Bluetooth
    int valeurBrute = analogRead(ADC);
    float tension = (valeurBrute / 1024.0) * 5.0;  // Conversion en tension (0-5V)

    uint8_t ValeurResBitAppli;
    uint8_t ValeurResBit;
    ValeurResBit=analogRead(ADC);
    ValeurResBitAppli=valeurBrute/4;
    bluetooth.write(ValeurResBitAppli);
  }

}

void setPotWiper(int addr, int pos) {
  pos = constrain(pos,0,255);
  digitalWrite(csPin, LOW);
  SPI.transfer(addr);
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);

  long resistanceWB = ( (rAB * pos) / maxPositions ) + rWiper;
  Serial.print("Wiper position: ");
  Serial.print(pos);
  Serial.print("Resistance wiper to B: ");
  Serial.print(resistanceWB);
  Serial.println(" ohms");

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
      if (menuState == 5) {  
        setPotWiper(pot0, potValue);  // Appliquer la valeur
        Serial.print("Potentiomètre réglé sur: ");
        Serial.println(potValue);
        menuState = 1;  // Retour au menu CONFIG après validation
        selection = 0;  // Réinitialisation de la sélection
        encoderChanged = true;  // Forcer la mise à jour
      } else {
        changerMenu();
      }
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
    afficherOption("Regler Pot", 0);
    afficherOption("Retour", 1);
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
  } else if (menuState == 5) {  // Menu de réglage du potentiomètre
    afficherValeurPotentiometre();
  }

  ecranOLED.display();
}

void afficherOption(const char *texte, int position) {
  ecranOLED.setTextSize(1);
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

void afficherValeurPotentiometre() {
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setTextColor(SSD1306_WHITE);
    
  ecranOLED.setCursor(10, 10);
  ecranOLED.println("Potentiom.");
    
  ecranOLED.setCursor(10, 30);
  ecranOLED.print("Valeur: ");
  ecranOLED.print(potValue);
    
  ecranOLED.setCursor(10, 50);
  if (selection == 0) {
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  } else {
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  }
  ecranOLED.println("OK");

  ecranOLED.display();
}

void changerMenu() {
  if (menuState == 0) {  // Menu principal
    menuState = (selection == 0) ? 1 : 2;
  } else if (menuState == 1) {  // Sous-menu Config
    if (selection == 1) {
      menuState = 0;  // Retour au menu principal
    } else if (selection == 0) {
      menuState = 5;  // Aller au réglage du potentiomètre
    }
  } else if (menuState == 2) {  // Sous-menu Mesure
    if (selection == 2) {
      menuState = 0;  // Retour au menu principal
    } else if (selection == 1) {
      menuState = 3;  // Aller au menu du capteur Flex
    } else if (selection == 0) {
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
  } else if (menuState == 5) {  // Menu de réglage du potentiomètre
    menuState = 1;  // Retour au menu "Config" après validation
  }

  selection = 0;  // Réinitialiser la sélection
  afficherMenu();
}

void doEncoder() {

  if (menuState == 5) {  // Si on est dans "Régler Pot."
    if (selection == 0) {  // Modification de la valeur du potentiomètre
      if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH) {
        potValue = min(potValue + 10, 255);
      } else if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == LOW) {
        potValue = max(potValue - 10, 0);
      }
    } else {  // Navigation sur "OK"
      selection = (selection == 0) ? 1 : 0;
    }
    encoderChanged = true;
  } else {  // Navigation normale dans les menus
    int maxOptions = (menuState == 1 || menuState == 2) ? 3 : 2;
    if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == HIGH) {
      selection = (selection + 1) % maxOptions;
    } else if (digitalRead(encoder0PinA) == HIGH && digitalRead(encoder0PinB) == LOW) {
      selection = (selection - 1 + maxOptions) % maxOptions;
    }
    encoderChanged = true;
  }

  encoderChanged = true;
}
