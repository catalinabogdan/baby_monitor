
#include <Arduino.h>
#include "TFT_HX8357.h"
#include <TFT_HX8357.h> 
#include "images.h"
#include <LedControl.h>

// Definirea pinilor pentru LedControl (Matricea de LED-uri)
#define DIN_PIN 12
#define CLK_PIN 11
#define CS_PIN 10

// Definirea pinilor pentru senzorul de sunet și buzzer
#define SOUND_SENSOR_PIN A1
#define BUZZER_PIN A0

// Definim instanțele pentru TFT și LedControl

TFT_HX8357 tft = TFT_HX8357(); // Configurăm TFT
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 1); // Configurăm LedControl

unsigned long targetTime = 0;
const int slideshowInterval = 3000; // Intervalul între schimbarea imaginilor (în milisecunde)
int currentImage;

#define IMAGE_WIDTH  480
#define IMAGE_HEIGHT 320

// Pin pentru fotorezistor
#define PHOTO_PIN A2
// Prag de lumină pentru activarea matricei de LED-uri
#define LIGHT_THRESHOLD 200

// Prag de zgomot pentru activarea buzzer-ului
#define NOISE_THRESHOLD 1000
unsigned long buzzerEndTime = 0; // Timpul la care buzzer-ul trebuie să se oprească

// Notele și durata lor pentru o melodie simplă (Do, Re, Mi)
int melody[] = {
  262, 294, 330, 349, 392, 440, 494, 523
};
int noteDurations[] = {
  4, 4, 4, 4, 4, 4, 4, 4
};

// Configurarea animației cu inimioare pentru matricea de LED-uri

const byte heart0[8] = {
  B00000000,
  B00000000,
  B00000000,
  B00011000,
  B00011000,
  B00000000,
  B00000000,
  B00000000
};

const byte heart1[8] = {
  B00000000,
  B00000000,
  B00011000,
  B00111100,
  B00111100,
  B00011000,
  B00000000,
  B00000000
};

const byte heart2[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

const byte heart3[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000
};

const byte heart4[8] = {
  B01100110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000,
  B00000000,
  B00000000
};

const byte* heartFrames[5] = { heart0, heart1, heart2, heart3, heart4 };

void setup(void) {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  targetTime = millis() + 1000;
  tft.drawBitmap(0, 0, image1, IMAGE_WIDTH, IMAGE_HEIGHT, TFT_CYAN);

  currentImage = 1;

  // Inițializăm LedControl
  lc.shutdown(0, false); 
  lc.setIntensity(0, 8);
  lc.clearDisplay(0); 

  // Configurăm pinii pentru senzorul de sunet și buzzer
  pinMode(PHOTO_PIN, INPUT);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // Oprire buzzer la start
}

void displayHeartFrame(const byte* frame) {
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, frame[row]);
  }
}

void animateHearts() {
  for (int i = 0; i < 5; i++) {
    displayHeartFrame(heartFrames[i]);
    delay(100); // Ajustăm viteza animației
  }
}

void loop() {
  static unsigned long lastImageChangeTime = 0; // Variabila pentru a urmări momentul ultimei schimbări de imagine
  static int currentImage = 1; // Contorul pentru imagini

  int lightLevel = analogRead(PHOTO_PIN);
  int soundLevel = analogRead(SOUND_SENSOR_PIN);

  // Dacă nivelul de lumină este sub pragul stabilit, activăm matricea de LED-uri
  if (lightLevel < LIGHT_THRESHOLD) {
    animateHearts();
  } else {
    lc.clearDisplay(0); // Stingem toate LED-urile
  }

  
  if (millis() - lastImageChangeTime >= 4000) { // 4000 milisecunde = 4 secunde
    // Schimba imaginea la următoarea în slideshow
    switch (currentImage) {
      case 1:
        tft.drawBitmap(0, 0, image1, IMAGE_WIDTH, IMAGE_HEIGHT, TFT_CYAN);
        currentImage = 2;
        delay(2000);
        tft.fillScreen(TFT_DARKCYAN);
        break;
      case 2:
        tft.drawBitmap(0, 0, image2, IMAGE_WIDTH, IMAGE_HEIGHT, TFT_CYAN);
        currentImage = 1; // Resetăm contorul pentru a începe slideshow-ul de la început
        delay(2000);
        tft.fillScreen(TFT_DARKCYAN);
        break;
    }
    lastImageChangeTime = millis(); // Actualizăm momentul ultimei schimbări de imagine
  }

  // Dacă nivelul de zgomot este peste pragul stabilit, activăm buzzer-ul
  if (soundLevel > NOISE_THRESHOLD) {
    buzzerEndTime = millis() + 1 * 1000; // Activăm buzzer-ul pentru 1 secundă
    // Redăm o melodie simplă
    for (int thisNote = 0; thisNote < 8; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZZER_PIN, melody[thisNote], noteDuration);
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZZER_PIN); // Oprește tonul curent
    }
  }

  // Oprim buzzer-ul după timpul setat
  if (millis() > buzzerEndTime) {
    noTone(BUZZER_PIN); // Oprește buzzer-ul
    digitalWrite(BUZZER_PIN, HIGH);
  }
}
