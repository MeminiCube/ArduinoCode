//importerer bibliotek
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include <SPI.h>
#include <MFRC522.h>
#include <stdint.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

//Definerer LCD-pins
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8
#define RST_PIN         4          
#define SS_PIN          7

//variabler for INPUT/OUTPUT
const int knapp = 2;
int valg = 0;
int pin1 = 3; 
int pin2 = 5;  
int pin3 = 6;

//Diverse
boolean skrift = false;

//array for data
String passord[3][3] = {
 
  {"SKYPE", "oldemur1867", "SkypePassord1"},
 
  {"FACEBOOK",  "oldemor1@me.com", "FacePassord1"},
 
  {"GMAIL",  "google@gmail.com", "GmailPassord1"}
 
};

//Definerer NFC
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

//Definerer LCD
Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  //INPUTs
  pinMode(knapp, INPUT);
  pinMode(pin1, OUTPUT);   // sets the pins as output
  pinMode(pin2, OUTPUT);   // sets the pins as output
  pinMode(pin3, OUTPUT);   // sets the pins as output

  //Starter LCD, roterer riktig vei og farger skjerm
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(0xAD55);
  
  SPI.begin(); // Init SPI bus
  tft.setTextColor(ILI9340_BLACK);
  rfid.PCD_Init(); // Init MFRC522 
  startSkjerm();
  
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);
}

void startSkjerm(){
  tft.setTextSize(3);
  tft.println();
  tft.println();
  tft.println();
  tft.println();
  tft.println("   Venter paa");
  tft.println("     kube...");
}

//Metode for utprinting
void printut(){
  tft.setTextSize(4);
  tft.setCursor(1, 1);
  tft.println();
  for(int i=0; i<=2; i++){
    tft.println("  " + passord[i][0]);
    tft.println();

  } 
}

//Metode for utskrift
void utskrift(int verdi){
  tft.fillScreen(ILI9340_WHITE);
  tft.setCursor(1, 1);
  tft.setTextSize(4);
  tft.setTextColor(ILI9340_BLACK);
  tft.println();
  tft.println(" Brukernavn: ");
  tft.setTextColor(ILI9340_RED);
  tft.setTextSize(3);
  tft.println("  " + passord[verdi][1]);
  tft.setTextColor(ILI9340_BLACK);
  tft.println();
  tft.println();
  tft.setTextSize(4);
  tft.println(" Passord: ");
  tft.setTextColor(ILI9340_RED);
  tft.setTextSize(3);
  tft.println("  " + passord[verdi][2]);
  tft.setTextColor(ILI9340_BLACK);
}

void loop(void) {
  //Variabler
  int knappStatus = digitalRead(knapp);
  int scrollVerdi = analogRead(A0);

  //Idle LED
  if (rfid.uid.uidByte[0] != 102){
    for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 10){
      analogWrite(pin3, fadeValue);
      analogWrite(pin1, fadeValue);
      analogWrite(pin2, fadeValue);
    }
    for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 10){
      analogWrite(pin1, fadeValue);
      analogWrite(pin2, fadeValue);
      analogWrite(pin3, fadeValue);
    }
  }
  
 // Ser etter nye kort
  if ( ! rfid.PICC_IsNewCardPresent()){
      return;
  }
  // Verifiserer
  if ( ! rfid.PICC_ReadCardSerial()){
    return;
  }

  //Start
  if (rfid.uid.uidByte[0] == 102 && skrift == false) {
    tft.fillScreen(ILI9340_WHITE);
    tft.setTextSize(4);
    if (skrift == false){
      printut(); 
    }
    skrift = true;
    analogWrite(pin3, 0);
    analogWrite(pin1, 0);
    analogWrite(pin2, 0);
    valg++;
  }
  //Utskrift 
  if(rfid.uid.uidByte[0] == 102 && (scrollVerdi >= 601 && scrollVerdi <= 900) && valg == 2){
    tft.fillScreen(ILI9340_WHITE);
    tft.setCursor(1, 1);
    tft.println();
    valg=1;
    tft.setTextSize(5);
    tft.setTextColor(ILI9340_BLACK);
    tft.println(" >" + passord[0][0]);
    tft.println();
    tft.setTextSize(4);
    tft.setTextColor(0x9492);      
    tft.println("  " + passord[1][0]);
    tft.println();
    tft.println("  " + passord[2][0]);
    tft.println();
  }
  else if(rfid.uid.uidByte[0] == 102 && (scrollVerdi >= 601 && scrollVerdi <= 900)){
    if (knappStatus == HIGH) {
      utskrift(0);
    }
  }   
  if(rfid.uid.uidByte[0] == 102 && (scrollVerdi >= 301 && scrollVerdi <= 600) && (valg == 1 || valg == 3)){
    tft.fillScreen(ILI9340_WHITE);
    tft.setCursor(1, 1);
    tft.println();
    valg=2;
    tft.setTextSize(4);
    tft.setTextColor(0x9492);
    tft.println("  " + passord[0][0]);
    tft.println();
    tft.setTextSize(5);
    tft.setTextColor(ILI9340_BLACK);
    tft.println(" >" + passord[1][0]);
    tft.println();
    tft.setTextSize(4);
    tft.setTextColor(0x9492);
    tft.println("  " + passord[2][0]);
    tft.println();
  }
  else if(rfid.uid.uidByte[0] == 102 && (scrollVerdi >= 301 && scrollVerdi <= 600)){
    if (knappStatus == HIGH) {
      utskrift(1);
    }
  }  
  if(rfid.uid.uidByte[0] == 102 && (scrollVerdi >= 0 && scrollVerdi <= 300) && valg == 2){
    tft.fillScreen(ILI9340_WHITE);
    tft.setCursor(1, 1);
    tft.println();
    valg=3;
    tft.setTextSize(4);
    tft.setTextColor(0x9492);
    tft.println("  " + passord[0][0]);
    tft.println();
    tft.println("  " + passord[1][0]);
    tft.println();
    tft.setTextSize(5);
    tft.setTextColor(ILI9340_BLACK);
    tft.println(" >" + passord[2][0]);
    tft.println();
  }
  else if(rfid.uid.uidByte[0] == 102 && (scrollVerdi <= 300 && scrollVerdi >= 0)){
    if (knappStatus == HIGH) {
      utskrift(2);
    }
  }
}
