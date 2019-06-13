/********************** Auteurs *****************************/
//  DABBOUS Tala
//  DELAHAYE Laure 
//  KINDONGO Andy 
//  EI2I - Polytech Sorbonne 
//  12/06/2019
/************************************************************/



/**********************************************************************/
// Sharp GP2Y1014AU0F Dust Sensor Demo
//
// Board Connection:
//   GP2Y1014    Arduino
//   V-LED       Between R1 and C1
//   LED-GND     C1 and GND
//   LED         Pin 7
//   S-GND       GND
//   Vo          A5
//   Vcc         5V
//  
//  Clk = 13 / DATA = 11 / CS = 10 / SA0 = 9 / RST = 8
//
// Serial monitor setting:
//   9600 baud
/**********************************************************************/

///////////////////////////////////////////////////////// CONFIGURATION /////////////////////////////////////////////////////////////////////////

/************************* Choix du programme *************************/
#define USE_AVG
/**********************************************************************/



/************************* Arduino Pin Numbers ************************/
const int sharpLEDPin = 7;   // Arduino digital pin 7 connect to sensor LED.
const int sharpVoPin = A5;   // Arduino analog pin 5 connect to sensor Vo.
/**********************************************************************/


/***** Pour faire la moyenne des derniers N entrées pour le capteur de poussière ****/
#ifdef USE_AVG
#define N 100
static unsigned long VoRawTotal = 0;
static int VoRawCount = 0;
#endif                 // USE_AVG
/************************************************************************************/

/***** Réglage de la valeur de sortie en Volts du capteur de poussière quand il y a  poussière *****/
static float Voc = 0.6;
// Use the typical sensitivity in units of V per 100ug/m3.
const float K = 0.5;
/***************************************************************************************************/

/************************ Inlcure les librairies des capteurs ************************/
#include "DHT.h"
#include <DHT_U.h>
#include "TM1637.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/*************************************************************************************/

/************************ Définire les pins connectés aux composants ************************/

#define CLK 3                 //Pins for TM1637       
#define DIO 4
#define DHTPIN A0             //Choisir le pin auquel t'es connecté 
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    9
#define OLED_CS    10
#define OLED_RESET 8
#define NUMFLAKES  10         // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
// Choisis le capteur que tu utilises!
//#define DHTTYPE DHT11       // DHT 11
#define DHTTYPE DHT22         // DHT 22  (AM2302)
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

/**** Pins associés à l'écran OLED SSD1306 codé avec le software SPI ***/
//OLED SSD1306
//Clk = pin 13 / DATA = pin 11 / CS = pin 10 / SA0 = pin 9 / RST = pin 8 
/*********************************************************************************************/

/* Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);
*/

/************************ Définire les objets connectés  ************************/
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
DHT dht(DHTPIN, DHTTYPE);
TM1637 tm1637(CLK, DIO);
/********************************************************************************/


///////////////////////////////////////////////////////// CODE /////////////////////////////////////////////////////////////////////////


// Helper functions to print a data value to the serial monitor.
void printValue(String text, unsigned int value, bool isLast = false) {
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  if (!isLast) {
    Serial.print(", ");
  }
}
void printFValue(String text, float value, String units, bool isLast = false) {
  Serial.print(text);
  Serial.print("=");
  Serial.print(value);
  Serial.print(units);
  if (!isLast) {
    Serial.print(", ");
  }
}

/****** Fonction principale qui dessine sur l'écran ******/
void draw(int t, int h, float dustDensity) {
  display.clearDisplay();
  display.setTextSize(1);               //Tu peux choisir la taille de l'écriture sur l'écran
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  /** Affichage sur l'écran de la température **/
  display.print(F("T : "));
  display.print(t);
  display.print(F(" C"));
  display.setCursor(10, 20);
  /** Affichage sur l'écran de l'humidité **/
  display.print(F("H : "));
  display.print(h);
  display.print(F(" %"));
  display.setCursor(10, 40);
  /** Affichage sur l'écran de la qualité de l'air **/
  display.print(F("Dust : "));
  display.print(dustDensity);
  display.print(F(" ug/m3"));
  display.display();
  delay(2000);
  int fact_num = factnumber(t);
  showFacts(fact_num);
}

int factnumber(int t) {
  int factnumber;
  if (t >= 25) 
  {
    if (t < 26) 
    {
      factnumber = 1;
    }
    else 
    {
      if ( t < 27)
      {
        factnumber = 2;
      }
      else if ( t < 28)
      {
        factnumber = 3;
      }
      else if ( t < 29)
      {
        factnumber = 4;
      }
      else if ( t < 30)
      {
        factnumber = 5;
      }
    } 
  }
  else
  {
    
  }
  return factnumber;
}

void showFacts(int factnumber) {
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.setCursor(0, 25);
  
  switch(factnumber) {
    case 1:
      display.println(F("L air pollue cause une mort sur huit dans le monde")) ;
      break;

    case 2:
      display.println(F("La qualite de l air est mauvaise dans le monde"));
      break;

    case 3:
      display.println(F("L air pollue supprime 1-2 ans de ta vie"));
      break;

    case 4:
      display.println(F("Aerer ton logement ameliore la qualite de l air"));
      break;

     case 5:
      display.println(F("La moisissure degrade la qualite de l air"));
      break;
  }
  display.display();
  delay(2000);
}


/****** Fonction d'initialisation de l'Arduino ******/
void setup() {
  // Réglage de la LED en mode OUTPUT c'est-à-dire en "sortie"
  pinMode(sharpLEDPin, OUTPUT);

  // Démarrage du hardware 
  Serial.begin(9600);
  dht.begin();
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
 
  // Affichage dans le moniteur série afin de suivre le déroulement du code
  delay(2000);
  Serial.println("");
  Serial.println("DUST GP2Y1014AU0F & DHT & 7SEG demo");
  Serial.println("=======================================");

  // Détection d'érreur avec l'écran
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
      }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Cette fonction efface le contenu de l'écran
  display.clearDisplay();

  // Cette fonction DOIT être appelée après avoir dessiné sur l'écran pour que ce soit visibile!
  display.display();
  delay(2000);
  

  // Les lignes ci-dessous permettent de recharger le contenu de l'écran à afficher
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);

}

/****** Fonction principale qui appele les autres fonctions ******/
void loop() {
  // On allume le capteur de poussière en allumant la LED (état LOW c'est-à-dire BAS)
  digitalWrite(sharpLEDPin, LOW);

  // On attend 0.28ms avant de lire la valeur en sortie
  delayMicroseconds(280);

  // On lit la valeur de sortie du capteur de poussière. Cette opération prend environ 100 us.
  int VoRaw = analogRead(sharpVoPin);

  // On éteint le capteur de poussière en éteignant la LED (état HIGH c'est-à-dire HAUT)
  digitalWrite(sharpLEDPin, HIGH);

  // On attend pendant le temps restant du cycle de 10 ms(10000 - 280 - 100 microseconds).
  delayMicroseconds(9620);

  // Imprimer la valeur VoRaw (numéro de 0 à 1023).
  #ifdef PRINT_RAW_DATA
  printValue("VoRaw", VoRaw, true);

#endif // PRINT_RAW_DATA

  // On utilise la moyenne si besoin
  float Vo = VoRaw;
#ifdef USE_AVG
  VoRawTotal += VoRaw;
  VoRawCount++;
  if ( VoRawCount >= N ) {
    Vo = 1.0 * VoRawTotal / N;
    VoRawCount = 0;
    VoRawTotal = 0;
  } else {
    return;
  }
#endif // USE_AVG

  // On convertir la valeur sortie afin de l'avoir en Volts
  Vo = Vo / 1024.0 * 5.0;
  printFValue("Vo", Vo * 1000.0, "mV");

  // Convert to Dust Density in units of ug/m3.
  float dV = Vo - Voc;
  if ( dV < 0 ) {
    dV = 0;
    Voc = Vo;
  }
  float dustDensity = dV / K * 100.0;
  printFValue("DustDensity", dustDensity, "ug/m3", true);
  Serial.println("");              // Cette fonction affiche dans le serial monitor
 
  // On lit les valeurs envoyées par le capteur d'humidité et de température
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int temp = dht.readTemperature();
  int humi = dht.readHumidity();
  int digitoneT = temp / 10;
  int digittwoT = temp % 10;
  // Réglage de l'écran afficheur 7 segments
  tm1637.display(1, digitoneT);
  tm1637.display(2, digittwoT);
  tm1637.display(3, 12); // put a C at the end
  
  
  // Fonction appelée pour dessiner sur l'écran
  draw(temp, humi,dustDensity);
  
  // Vérification que nous avons reçu les bonnes valeurs des capteurs
  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read from DHT");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
  }

} // END PROGRAM
