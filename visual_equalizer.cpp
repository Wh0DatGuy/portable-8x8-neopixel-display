#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Battery.h>
#include <letters.h>
//pins
#define STROBE_PIN 4
#define ANALOG_PIN A0
#define BattRead A4
#define RESET_PIN 2
#define WS2812_PIN 7
#define PHOTOR A2
#define BATTVS 11
#define B_1 10
#define B_2 9
#define B_3 8
//delay values
#define InitSensy 630
#define LettDelay 1600
#define LettOffDelay 500
unsigned int SwDelay = 300;
//constant variables
#define EqAnBars 7
#define VOres 8
//global variables
int bandValues[EqAnBars], oBandValues, bandNo;
byte j;
byte MenuMode = 0, DisplayMode = 0;
int Sensy;
byte Brighty, Speedy = 8;
bool Direction = 0, fMenuLabel, MemEn = false;
byte fMode;
long t1, t5, t6 = 0;
byte LettNumb = 0, MenuLett;
int aMaxLettNumber = 0;
//library initializations
Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, WS2812_PIN, NEO_GRB + NEO_KHZ800);
Battery battery(3400, 4000, BattRead);
//cycle trough colours to create rainbow effect
void increasej() {
  static long t0;
  if ((millis() - t0) >= Speedy) {
    if (j < 256) {
      if (Direction == 1)
        j++;
      else
        j--;
    } else
      j = 0;
    t0 = millis();
  }
}
//generates a color based on a 255 value
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
bool DispBuffer[8][8];
//read MSGEQ7 and store in an array
void readMSGEQ7() {
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  for (bandNo = 0; bandNo < EqAnBars; bandNo++) {
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(30);
    bandValues[bandNo] = analogRead(ANALOG_PIN);
    digitalWrite(STROBE_PIN, HIGH);

    if (bandValues[bandNo] > oBandValues)
      oBandValues = bandValues[bandNo];
  }
  if (oBandValues >= Sensy)
    oBandValues--;
  else if (Sensy == InitSensy)
    oBandValues = Sensy; //always fit bars on 8 pixels
}
//visual equalizer
void VisualEq() {
  readMSGEQ7();
  for (bandNo = 0; bandNo < EqAnBars; bandNo++) {
    byte toY = map(bandValues[bandNo], 30, oBandValues, 0, VOres);
    for (byte y = 0; y < 8; y++) {
      if (y < toY) {
        DispBuffer[bandNo][y] = 1;
      } else {
        DispBuffer[bandNo][y] = 0;
      }
    }
  }
}
//Calculate and display battery level
void BattLevel() {
  static byte BattToPixel;
  if ((millis() - t5) >= 1000) {
    BattToPixel = map(battery.level(), 0, 100, 0, VOres);
    t5 = millis();
  }
  for (byte i = 0; i < 8; i++) {
    if (i < BattToPixel) {
      DispBuffer[7][i] = 1;
    } else {
      DispBuffer[7][i] = 0;
    }
  }
}
//display letters or full screen rainbow effect
void DispAlphabetOrRainbow(byte DMode, byte SLettNumber = 0, int NxSLettNumber = 0, int MaxLettNumber = 1) {
  static int k = 0, oaMaxLettNumber;
  if ((aMaxLettNumber != oaMaxLettNumber)) {
    k = 0;
    oaMaxLettNumber = aMaxLettNumber;
    t6 = millis();
  }
  if (aMaxLettNumber < (MaxLettNumber - 1)) {
    if (millis() - t6 >= (LettDelay / (VOres + 1))) {
      if (k < (VOres)) {
        k++;
      }
      t6 = millis();
    }
  }
  for (byte h = 0; h < VOres; h++) {
    for (byte l = 0; l < VOres; l++) {
      bool DVal;
      switch (DMode) {
      case 0:
        DVal = (pgm_read_byte(&LettNumber[SLettNumber][(VOres - 1) - l][h]));
        break;
      case 1:
        DVal = 1;
        break;
      case 2:
        if (h < (VOres - k)) {
          DVal = (pgm_read_byte(&LettNumber[SLettNumber][(VOres - 1) - l][h + k]));
        } else {
          DVal = (pgm_read_byte(&LettNumber[NxSLettNumber][(VOres - 1) - l][h - (VOres - (k - 1))]));
        }
        break;
      }
      DispBuffer[h][l] = DVal;
      DispBuffer[(VOres - k)][l] = 0;
    }
  }
}
void VerticalOrHorizontal() {
  for (byte h = 0; h < VOres; h++) {
    for (byte l = 0; l < VOres; l++) {
      switch (DisplayMode) {
      case 0 /*vertical*/:
        fMode = (h * VOres) + l;
        break;
      case 1 /*horizontal*/:
        fMode = (l * VOres) + ((VOres - 1) - h);
        break;
      }
      bool DVal;
      DVal = DispBuffer[h][l];
      strip.setPixelColor(fMode, (Wheel(((h * VOres) + l) + j) * DVal));
    }
  }

}
//clear display for DelayVal seconds
void ClearWithDelay(int DelayVal) {
  strip.clear();
  strip.show();
  delay(DelayVal);
}
//letter menu to compose phrases
void LettMenu() {
  static byte k, Phrase[32];
  static long t2, t3;
  static bool fStop;
  if (digitalRead(B_1) || digitalRead(B_3)) {
    t2 = millis();
    MemEn = true;
  } else if (((millis() - t2) >= LettDelay) && MemEn) {
    ClearWithDelay(LettOffDelay);
    Phrase[k] = LettNumb;
    k++;
    MemEn = false;
  }
  if (digitalRead(B_1) && digitalRead(B_3)) {
    byte i;
    ClearWithDelay(LettOffDelay);
    fStop = false;
    i = 0;
    t3 = millis();
    t6 = millis();
    while (!fStop) {
      strip.clear();
      increasej();
      DispAlphabetOrRainbow(2, Phrase[i], Phrase[i + 1], k);
      VerticalOrHorizontal();
      strip.show();
      if (millis() - t3 >= LettDelay) {
        if (i < (k - 1)) {
          aMaxLettNumber++;
          i++;
        } else {
          i = 0;
          ClearWithDelay(LettDelay);
          aMaxLettNumber = 0;
          t6 = millis();
        }
        t3 = millis();
      }
      if (digitalRead(B_2)) {
        fStop = true;
      }
    }
    aMaxLettNumber = 0;
    k = 0;
    MemEn = false;
    ClearWithDelay(LettOffDelay);
  }
  DispAlphabetOrRainbow(0, LettNumb);
}
//increase or decrease to 0 given value using B1 and B3
int IncrDecr(int MaxVal, int Val, byte Step) {
  if (digitalRead(B_3)) {
    Val += Step;
    t1 = millis();
  } else if (digitalRead(B_1)) {
    Val -= Step;
    t1 = millis();
  }
  if (Val > MaxVal)
    Val = MaxVal;
  else if (Val < Step)
    Val = 0;
  return Val;
}
//display menu letter when selected
void MenuLettSel() {
  switch (MenuMode) {
  case 0 /*brightness*/:
    MenuLett = 1; //B
    break;
  case 1 /*volume*/:
    MenuLett = 21; //V
    break;
  case 2 /*orientation*/:
    MenuLett = 14; //O
    break;
  case 3 /*writing mode*/:
    MenuLett = 22; //W
    break;
  case 4 /*direction*/:
    MenuLett = 3; //D
    break;
  case 5 /*speed*/:
    MenuLett = 18; //S
    break;
  }
  DispAlphabetOrRainbow(0, MenuLett);
}
//modify values of said menu
void SettChange() {
  switch (MenuMode) {
  case 0 /*brightness*/:
    Brighty = IncrDecr(255, Brighty, 15);
    if (Brighty == 0)
      Brighty = 15;
    strip.setBrightness(Brighty);
    break;
  case 1 /*volume*/:
    Sensy = IncrDecr(InitSensy, Sensy, 90);
    break;
  case 2 /*orientation*/:
    DisplayMode = IncrDecr(1, DisplayMode, 1);
    break;
  case 3 /*writing mode*/:
    LettNumb = IncrDecr(26, LettNumb, 1);
    break;
  case 4 /*direction*/:
    Direction = IncrDecr(1, Direction, 1);
    break;
  case 5 /*speed*/:
    Speedy = IncrDecr(10, Speedy, 1);
    if (Speedy < 4)
      Speedy = 4;
    break;
  }
}
//change selected menu using B2
void Menu() {
  static long t4;
  if ((!digitalRead(B_1)) && (!digitalRead(B_2)) && (!digitalRead(B_3))) {
    SwDelay = 300;
    t1 = millis() + SwDelay;
  } else if ((millis() - t1) >= SwDelay) {
    SwDelay -= 25;
    if (SwDelay <= 90) {
      SwDelay = 100;
    }
    if (digitalRead(B_2)) {
      t1 = millis();
      t4 = millis();
      if (MenuMode < 5)
        MenuMode++;
      else
        MenuMode = 0;
    } else if (!fMenuLabel)
      SettChange();
  }
  if (((millis() - t4) <= LettDelay)) {
    fMenuLabel = true;
    MenuLettSel();
  } else
    fMenuLabel = false;
}
//main program
void setup() {
  strip.begin();
  strip.show();
  battery.begin(5000, 1.0);
  oBandValues = InitSensy;
  Sensy = InitSensy;
  Brighty = map(analogRead(PHOTOR), 0, 700, 15, 255);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(STROBE_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
  strip.setBrightness(Brighty);
}

void loop() {
  strip.clear();
  increasej();

  Menu();

  if (digitalRead(B_2) || fMenuLabel) {
    MemEn = false; //if exit letter menu reset letter timer
  } else switch (MenuMode) {
  default:
    //if(digitalRead(BATTVS)) //remove "//" when circuit upgraded
    VisualEq();
    BattLevel();
    break;
  case 3:
    LettMenu();
    break;
  case 4:
  case 5:
    DispAlphabetOrRainbow(1);
    break;
  }

  VerticalOrHorizontal();

  strip.show();
}
