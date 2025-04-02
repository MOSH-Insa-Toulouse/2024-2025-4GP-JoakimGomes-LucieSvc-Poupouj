// Definition des pins

// Encodeur Rotatoire
#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 0;

// Flex Sensor
const int flexPin = A1;      // Pin connected to voltage divider output
const float VCC = 5;      // voltage at Ardunio 5V line
const float R_DIV = 33000.0;  // resistor used to create a voltage divider  !!!!!!!!!!!!!!!
const float flatResistance = 33000.0; // resistance when flat  !!!!!!!!!!!!!!!!!!!!!!!!!!!!
const float bendResistance = 75000.0;  // resistance at 90 deg  !!!!!!!!!!!!!!!!!!!!!!!!!!

// Ecran OLED
#include <Adafruit_SSD1306.h>
#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D) !!!!!!!!!!!!!!!!!!!!!!!!!!
Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

// Module Bluetooth
#include <SoftwareSerial.h>
#define rxPin 8 //Broche 8 en tant que RX, � raccorder sur TX du HC-05
#define txPin 7 //Broche 7 en tant que TX, � raccorder sur RX du HC-05
#define baudrate 9600
SoftwareSerial mySerial(rxPin ,txPin); //D�finition du software serial


// Potentiomètre Digital (TP MCP41100_R_Test) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <SPI.h>
const byte csPin           = 10;      // MCP42100 chip select pin
const int  maxPositions    = 256;     // wiper can move from 0 to 255 = 256 positions
const long rAB             = 92500;   // 100k pot resistance between terminals A and B, 
                                      // mais pour ajuster au multimètre, je mets 92500
const byte rWiper          = 125;     // 125 ohms pot wiper resistance
const byte pot0            = 0x11;    // pot0 addr // B 0001 0001
const byte pot0Shutdown    = 0x21;    // pot0 shutdown // B 0010 0001



// Pour le potentiom_tre Digital mais j'ai pas compris ce que ça fait et si ça nous est utile !!!!!!!!!!!!!!!!!!!
void setPotWiper(int addr, int pos) {
  pos = constrain(pos, 0, 255);            // limit wiper setting to range of 0 to 255
  digitalWrite(csPin, LOW);                // select chip
  SPI.transfer(addr);                      // configure target pot with wiper position
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);               // de-select chip

  // print pot resistance between wiper and B terminal
  long resistanceWB = ((rAB * pos) / maxPositions ) + rWiper;
  Serial.print("Wiper position: ");
  Serial.print(pos);
  Serial.print(" Resistance wiper to B terminal: ");
  Serial.print(resistanceWB);
  Serial.println(" ohms");
}


void setup() {
  Serial.begin(baudrate);

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

  // Module Bluetooth
  pinMode(rxPin,INPUT);
  pinMode(txPin,OUTPUT);
    
  mySerial.begin(baudrate);
  

  // Potentiomètre digital 
  Serial.println("MCP41100 Demo");
  digitalWrite(csPin, HIGH);        // chip select default to de-selected
  pinMode(csPin, OUTPUT);           // configure chip select as output
  SPI.begin();
}

void loop() {
  // fonctions de bases qui étaient dans le TP, à voir ce qu'on veux vraiment faire

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

  // Module Bluetooth //
    int i = 0; 
	char someChar[32] ={0};
	while (Serial.available()) {
	   do{
		someChar[i++] = Serial.read();
		delay(3);		
	   }while (Serial.available() > 0);
	   
	   mySerial.println(someChar); 
	   Serial.println(someChar); 
	}
	while (mySerial.available()) {
		Serial.print((char)mySerial.read());
	}

  // Potentiomètre Digital // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}



// Encodeur Rotatoire : Ajustement du cran de l'encodeur
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  }
  
}
