#include "RTClib.h" //https://github.com/adafruit/RTClib
#include <Wire.h>  
#include <FastLED.h> //https://github.com/FastLED/FastLED

/*
 * Wordclock for WS2812B LED Pixel Stripes and 3231 RTC
 * More Information at https://diygadget.wordpress.com
 * and https://github.com/FastLED/FastLED/wiki/Overview
 */

FASTLED_USING_NAMESPACE
#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    164      // Gesamtanzahl der WS2812 LEDs
CRGB leds[NUM_LEDS];

uint8_t BRIGHTNESS        = 180;  // Helligkeit

uint8_t Stunde     = 0;
uint8_t Minute     = 15; 
uint8_t WochenTag  = 0;   
uint8_t Tag        = 30;
uint8_t Monat      = 10;
DateTime now;
boolean DST        = false;
boolean swDST      = true;     // Falls dieser Sketch während Sommerzeit kompiliert wurde = true, Winterzeit = false;
boolean Debug      = false;     // Debugging

//***HIER LED POSITIONEN EINTRAGEN***//
int EsIst[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

int Eins[]      = {86, 87, 88, 89, 90, 91};
int Ein[]       = {86, 87, 88, 89, 90};
int Zwei[]      = {96, 97, 98, 99, 100, 101};
int Drei[]      = {102, 103, 104, 105, 106, 107, 108};
int Vier[]      = {109, 110, 111, 112, 113, 114};
int Fuenf[]     = {80, 81, 82, 83, 84, 85};
int Sechs[]     = {115, 116, 117, 118, 119, 120, 121, 122};
int Sieben[]    = {130, 131, 132, 133, 134, 135, 136, 137, 138, 139};
int Acht[]      = {123, 124, 125, 126, 127, 128, 129};
int Neun[]      = {73, 74, 75, 76, 77, 78, 79};
int Zehn[]      = {148, 149, 150, 151, 152, 153, 154};
int Elf[]       = {92, 93, 94, 95};
int Zwoelf[]    = {140, 141, 142, 143, 144, 145, 146, 147};
int MZwanzig[]  = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
int MFuenf[]    = {12, 13, 14, 15, 16, 17, 18};
int MZehn[]     = {36, 37, 38, 39, 40, 41, 42};

int Viertel[]   = {43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54};
int Vor[]       = {55, 56, 57, 58, 59};
int Nach[]      = {66, 67, 68, 69, 70, 71, 72};
int Halb[]      = {60, 61, 62, 63, 64, 65};
int Uhr[]       = {155, 156, 157, 158, 159};
int Und[]       = {19, 20, 21, 22, 23};

int EinsM[]     = {160}; //Minuten Punkt 1
int ZweiM[]     = {161}; //Minuten Punkt 2
int DreiM[]     = {162}; //Minuten Punkt 3
int VierM[]     = {163}; //Minuten Punkt 4
//**********************************//

int i;

RTC_DS3231 rtc;

void setup() {
  delay(3000); // Sicherheits Delay

  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif

  Serial.begin(19200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //Setzt die RTC Zeit auf Kompilierungszeit
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));  //Um die Zeit selbst zu setzen

  
  //LEDs werden eingefügt + Farbkorrektur und Farbtemperatur auf Wolfram (warmweiß)
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalPixelString);
  FastLED.setTemperature( Tungsten40W );
  Serial.println("Starte Wordclock ...");

}

void SwitchLED(int MyArray[], int n) {
  //Funktion zum Anschalten der LEDs in warmweiß (oder gewünschter Farbe)
  for (i = MyArray[0]; i < MyArray[0]+n; i++) {
      leds[i] = 0xFFC58F;                         // HEX Warmweiß | Hier gewünschte LED Farbe (HEX) eintragen
      FastLED.setBrightness(BRIGHTNESS);
      //leds[i] = CHSV(140, 27, BRIGHTNESS);      // ODER hier gewünschte Farbe in HSV (  CHSV(FARBE, SÄTTIGUNG, BRIGHTNESS)  )
      //leds[i].setRGB(255, 68, 221);             // ODER hier gewünschte Farbe in RGB (  .setRGB(Rot,Grün,Blau)   )
  }
}

void displaytime(void){
  //zuerst setzten wir alle LEDs zurück
  fill_solid( leds, NUM_LEDS, CHSV(0, 0, 0));

  // Nun suchen wir die richtigen LEDs und übergeben sie an die Funktion zum Anschalten
  if(Debug==true)Serial.print("Es ist ");
  SwitchLED(EsIst, (sizeof(EsIst)/2));
  
  if (((Minute>4) && (Minute<10)) || (Minute>54)) { 
    SwitchLED(MFuenf, (sizeof(MFuenf)/2));
    if(Debug==true)Serial.print("5 Minuten ");
  }
  if (((Minute>9) && (Minute<15)) || ((Minute>49) && (Minute<55))) { 
    SwitchLED(MZehn, (sizeof(MZehn)/2));
    if(Debug==true)Serial.print("10 Minuten ");
  }
  if (((Minute>14) && (Minute<20)) || ((Minute>44) && (Minute<50))) {
    SwitchLED(Viertel, (sizeof(Viertel)/2)); 
    if(Debug==true)Serial.print("Viertel ");
  }
  if (((Minute>19) && (Minute<25)) || ((Minute>39) && (Minute<45))) { 
    SwitchLED(MZwanzig, (sizeof(MZwanzig)/2)); 
    if(Debug==true)Serial.print("20 Minuten ");
  }
  if (((Minute>24) && (Minute<30)) || ((Minute>34) && (Minute<40))) { 
    SwitchLED(MFuenf, (sizeof(MFuenf)/2)); 
    SwitchLED(Und, (sizeof(Und)/2));
    SwitchLED(MZwanzig, (sizeof(MZwanzig)/2)); 
    if(Debug==true)Serial.print("25 Minuten ");
  }  
  if ((Minute>29) && (Minute<35)) {
    SwitchLED(Halb, (sizeof(Halb)/2));
    if(Debug==true)Serial.print("Halb ");
  }

  if (Minute <5){
    switch (Stunde) {
      case 1: 
        SwitchLED(Ein, (sizeof(Ein)/2));
        if(Debug==true)Serial.print("1 ");
        break;
      case 2: 
        SwitchLED(Zwei, (sizeof(Zwei)/2));
        if(Debug==true)Serial.print("2 ");
        break;
      case 3: 
        SwitchLED(Drei, (sizeof(Drei)/2));
        if(Debug==true)Serial.print("3 ");
        break;
      case 4: 
        SwitchLED(Vier, (sizeof(Vier)/2));
        if(Debug==true)Serial.print("4 ");
        break;
      case 5: 
        SwitchLED(Fuenf, (sizeof(Fuenf)/2));
        if(Debug==true)Serial.print("5 ");
        break;
      case 6: 
        SwitchLED(Sechs, (sizeof(Sechs)/2));
        if(Debug==true)Serial.print("6 ");
        break;
      case 7: 
        SwitchLED(Sieben, (sizeof(Sieben)/2));
        if(Debug==true)Serial.print("7 ");
        break;
      case 8: 
        SwitchLED(Acht, (sizeof(Acht)/2));
        if(Debug==true)Serial.print("8 ");
        break;
      case 9: 
        SwitchLED(Neun, (sizeof(Neun)/2));
        if(Debug==true)Serial.print("9 ");
        break;
      case 10: 
        SwitchLED(Zehn, (sizeof(Zehn)/2));
        if(Debug==true)Serial.print("10 ");
        break;
      case 11: 
        SwitchLED(Elf, (sizeof(Elf)/2));
        if(Debug==true)Serial.print("11 ");
        break;
      case 12: 
        SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
        if(Debug==true)Serial.print("12 ");
        break;
    }
    SwitchLED(Uhr, (sizeof(Uhr)/2));
    if(Debug==true)Serial.println("Uhr");
  }
  else if ((Minute < 30) && (Minute >4)) {
    SwitchLED(Nach, (sizeof(Nach)/2));
    if(Debug==true)Serial.print("nach ");
    switch (Stunde) {
      case 1: 
        SwitchLED(Eins, (sizeof(Eins)/2));
        if(Debug==true)Serial.println("1 ");
        break;
      case 2: 
        SwitchLED(Zwei, (sizeof(Zwei)/2));
        if(Debug==true)Serial.println("2 ");
        break;
      case 3: 
        SwitchLED(Drei, (sizeof(Drei)/2));
        if(Debug==true)Serial.println("3 ");
        break;
      case 4: 
        SwitchLED(Vier, (sizeof(Vier)/2));
        if(Debug==true)Serial.println("4 ");
        break;
      case 5: 
        SwitchLED(Fuenf, (sizeof(Fuenf)/2));
        if(Debug==true)Serial.println("8 ");
        break;
      case 6: 
        SwitchLED(Sechs, (sizeof(Sechs)/2));
        if(Debug==true)Serial.println("6 ");
        break;
      case 7: 
        SwitchLED(Sieben, (sizeof(Sieben)/2));
        if(Debug==true)Serial.println("7 ");
        break;
      case 8: 
        SwitchLED(Acht, (sizeof(Acht)/2));
        if(Debug==true)Serial.println("8 ");
        break;
      case 9: 
        SwitchLED(Neun, (sizeof(Neun)/2));
        if(Debug==true)Serial.println("9 ");
        break;
      case 10: 
        SwitchLED(Zehn, (sizeof(Zehn)/2));
        if(Debug==true)Serial.println("10 ");
        break;
      case 11: 
        SwitchLED(Elf, (sizeof(Elf)/2));
        if(Debug==true)Serial.println("11 ");
        break;
      case 12: 
        SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
        if(Debug==true)Serial.println("12 ");
        break;
    }
  }
  else{
    if ((Minute>29) && (Minute<35)){
      switch (Stunde) {
        case 1: 
          SwitchLED(Zwei, (sizeof(Zwei)/2));
          if(Debug==true)Serial.println("2 ");
          break;
        case 2: 
          SwitchLED(Drei, (sizeof(Drei)/2));  
          if(Debug==true)Serial.println("3");
          break;
        case 3: 
          SwitchLED(Vier, (sizeof(Vier)/2));  
          if(Debug==true)Serial.println("4 ");
          break;
        case 4: 
          SwitchLED(Fuenf, (sizeof(Fuenf)/2));
          if(Debug==true)Serial.println("5 ");
          break;
        case 5: 
          SwitchLED(Sechs, (sizeof(Sechs)/2)); 
          if(Debug==true)Serial.println("6 ");
          break;
        case 6: 
          SwitchLED(Sieben, (sizeof(Sieben)/2));
          if(Debug==true)Serial.println("7 ");
          break;
        case 7: 
          SwitchLED(Acht, (sizeof(Acht)/2));
          if(Debug==true)Serial.println("8 ");
          break;
        case 8: 
          SwitchLED(Neun, (sizeof(Neun)/2));  
          if(Debug==true)Serial.println("9 ");
          break;
        case 9: 
          SwitchLED(Zehn, (sizeof(Zehn)/2)); 
          if(Debug==true)Serial.println("10 ");
          break;
        case 10: 
          SwitchLED(Elf, (sizeof(Elf)/2)); 
          if(Debug==true)Serial.println("11 ");
          break;
        case 11: 
          SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
          if(Debug==true)Serial.println("12 ");
          break;
        case 12: 
          SwitchLED(Eins, (sizeof(Eins)/2));
         if(Debug==true) Serial.println("1 ");
          break;
        }
      }
      else{
        SwitchLED(Vor, (sizeof(Vor)/2));
        Serial.print("vor ");
        switch (Stunde) {
          case 1: 
            SwitchLED(Zwei, (sizeof(Zwei)/2));
            if(Debug==true)Serial.println("2 ");
            break;
          case 2: 
            SwitchLED(Drei, (sizeof(Drei)/2));  
            if(Debug==true)Serial.println("3");
            break;
          case 3: 
            SwitchLED(Vier, (sizeof(Vier)/2));  
            if(Debug==true)Serial.println("4 ");
            break;
          case 4: 
            SwitchLED(Fuenf, (sizeof(Fuenf)/2));
            if(Debug==true)Serial.println("5 ");
            break;
          case 5: 
            SwitchLED(Sechs, (sizeof(Sechs)/2)); 
            if(Debug==true)Serial.println("6 ");
            break;
          case 6: 
            SwitchLED(Sieben, (sizeof(Sieben)/2));
            if(Debug==true)Serial.println("7 ");
            break;
          case 7: 
            SwitchLED(Acht, (sizeof(Acht)/2));
            if(Debug==true)Serial.println("8 ");
            break;
         case 8: 
            SwitchLED(Neun, (sizeof(Neun)/2));  
            if(Debug==true)Serial.println("9 ");
            break;
          case 9: 
            SwitchLED(Zehn, (sizeof(Zehn)/2)); 
            if(Debug==true)Serial.println("10 ");
            break;
         case 10: 
            SwitchLED(Elf, (sizeof(Elf)/2)); 
            if(Debug==true)Serial.println("11 ");
            break;
          case 11: 
            SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
            if(Debug==true)Serial.println("12 ");
            break;
          case 12: 
            SwitchLED(Eins, (sizeof(Eins)/2));
            if(Debug==true)Serial.println("1 ");
            break;
        }
      }
    }
  // Minuten Zähler
  uint8_t MinCount = Minute-(floor(Minute/10)*10);
  if(MinCount > 5)
    MinCount = MinCount - 5;
  switch(MinCount){
    case 4:
      SwitchLED(VierM, (sizeof(VierM)/2));
    case 3:
      SwitchLED(DreiM, (sizeof(DreiM)/2));
    case 2:
      SwitchLED(ZweiM, (sizeof(ZweiM)/2));
    case 1:
      SwitchLED(EinsM, (sizeof(EinsM)/2));
  }
  FastLED.show();
}

void CheckDST(void){
  //Funktion zum automatischen Umstellen zwischen Sommer- und Winterzeit
  Tag = now.day();
  Monat = now.month();
 
  WochenTag = (now.dayOfTheWeek()-1);
  int Hilf = Tag-WochenTag;
  if(Monat <= 2 || Monat >= 11)
    DST = false;                                 // Winterzeit

  if(Monat >= 4 && Monat <= 9)
    DST = true;                                  // Sommerzeit
      
  if((Monat == 3) && (Hilf >= 25))
     DST = true;                                 // Beginn der Sommerzeit
       
  if((Monat == 10) && (Hilf >= 25))
    DST = false;                                 // Beginn der Winterzeit
 
  if(DST == true){
    Stunde += 1;
    if(Stunde > 12)
      Stunde -= 12;
  }
  if(Debug==true)Serial.print("DST:");
  if(Debug==true)Serial.println(DST);
}

void loop() {
  /* Schleifen Funktion, die die Uhrzeit ausliest und dann in die 
   * entsprechende Funktionen übergibt.
   */
    now = rtc.now();
    Minute = now.minute();
    int Std = now.hour();
    if(swDST)
      Std -= 1;
    if(Std > 12)
      Stunde -= 12;
    else if (Std == 0)
      Stunde = 12;
    else if (Std == -1)
      Stunde = 11;
    else
      Stunde = Std;
   
    CheckDST();
    displaytime();
    FastLED.delay(250);
    Serial.println(Stunde);
    Serial.println(Minute);
}
