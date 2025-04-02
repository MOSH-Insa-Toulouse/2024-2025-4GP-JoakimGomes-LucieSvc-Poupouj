// Definition des pins

// Encodeur Rotatoire //
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 0;

// Ecran OLED
#include <Adafruit_SSD1306.h>
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);


void setup() {
  // Encodeur Rotatoire
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor
  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor
  attachInterrupt(0, doEncoder, RISING); // encoder pin on interrupt 0 - pin2

  // Ecran OLED 
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);                               // Arrêt du programme (boucle infinie) si échec d'initialisation
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(2:1);                   // Taille des caractères (2:1)
  ecranOLED.setTextColor(SSD1306_WHITE); 
  ecranOLED.println("Configuration");
  ecranOLED.println("Mesure");
  ecranOLED.display();                          // Transfert le buffer à l'écran


}




void loop() {
  // put your main code here, to run repeatedly:





}






// Encodeur Rotatoire : Ajustement du cran de l'encodeur
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  } 

  if (encoder0Pos%2 == 0) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(2:1);
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println("Configuration");
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 
    ecranOLED.println("Mesure");
    ecranOLED.display(); 
  } else if (encoder0Pos%2 == 1) {
    ecranOLED.clearDisplay();
    ecranOLED.setTextSize(2:1);
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println("Configuration");
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
    ecranOLED.println("Mesure");
    ecranOLED.display(); 
  }
}
