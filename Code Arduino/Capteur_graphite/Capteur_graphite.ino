// Devinition des pins

// Encodeur rotatoir
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 0;

// Flex Sensor
const int flexPin = A1;      // Pin connected to voltage divider output
const float VCC = 5;      // voltage at Ardunio 5V line
const float R_DIV = 47000.0;  // resistor used to create a voltage divider  !!!!!!!!!!!!!!!
const float flatResistance = 25000.0; // resistance when flat  !!!!!!!!!!!!!!!!!!!!!!!!!!!!
const float bendResistance = 100000.0;  // resistance at 90 deg  !!!!!!!!!!!!!!!!!!!!!!!!!!

// Ecran OLED
#include <Adafruit_SSD1306.h>
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D) !!!!!!!!!!!!!!!!!!!!!!!!!!
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);




void setup() {
  Serial.begin (9600);

  // Encodeur Rotatoire
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor

  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor

  attachInterrupt(0, doEncoder, RISING); // encoder pin on interrupt 0 - pin2

  // Flex Sensor
  pinMode(flexPin, INPUT);

  // Ecran OLED 
  // Initialisation de l'écran OLED
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);                               // Arrêt du programme (boucle infinie) si échec d'initialisation
  ecranOLED.clearDisplay();
}

void loop() {
  // Encodeur Rotatoire //
  Serial.print("Position:");
  Serial.println (encoder0Pos, DEC);  //Angle = (360 / Encoder_Resolution) * encoder0Pos


  // Flex Sensor //
  // Read the ADC, and calculate voltage and resistance from it
  int ADCflex = analogRead(flexPin);
  float Vflex = ADCflex * VCC / 1023.0;
  float Rflex = R_DIV * (VCC / Vflex - 1.0);
  Serial.println("Resistance: " + String(Rflex) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(Rflex, flatResistance, bendResistance, 0, 90.0);
  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();

  delay(500);


  // Ecran OLED //
  // fonctions de bases qui étaient dans le TP, à voir ce qu'on veux vraiment afficher
  for(byte tailleDeCaractere=1; tailleDeCaractere <=3; tailleDeCaractere++) {
    boolean bCouleurInverse = false;
    ecranOLED.clearDisplay();                                   // Effaçage de l'intégralité du buffer
    ecranOLED.setTextSize(tailleDeCaractere);                   // Taille des caractères (1:1, puis 2:1, puis 3:1)
    ecranOLED.setCursor(0, 0);                                  // Déplacement du curseur en position (0,0), c'est à dire dans l'angle supérieur gauche
    ecranOLED.setTextColor(SSD1306_WHITE); 
    ecranOLED.print("Ligne ");
    ecranOLED.println(numeroDeLigne);
    ecranOLED.display();                            // Transfert le buffer à l'écran
    delay(2000);
  }
}



// Encodeur Rotatoire : Ajustement du cran de l'encodeur
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  }
}