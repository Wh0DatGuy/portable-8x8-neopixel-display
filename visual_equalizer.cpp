#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Battery.h>
///////////////////////////////////PINS///////////////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////DELAYS/////////////////////////////////////////////////////////////////////////////////////////
#define InitSensy 630
#define SwDelay 300
#define LettDelay 1000
#define LettOffDelay 500
///////////////////////////////////CONSTANT VARIABLES/////////////////////////////////////////////////////////////////////////////
#define EqAnBars 7
#define VOres 8
///////////////////////////////////GLOBAL VARIABLES///////////////////////////////////////////////////////////////////////////////
int bandValues[EqAnBars], oBandValues, bandNo;
byte j;
byte MenuMode = 0, DisplayMode = 0;
int Sensy, Brighty, Speedy = 8;//Lower means faster;
bool Direction = 0;
byte fMode;
long t1, t4, t5 = 1000;
byte LettNumb = 0, MenuLett;
#include <letters.h> //include the letter array
///////////////////////////////////LIBRARY INITIALIZATIONS////////////////////////////////////////////////////////////////////////
Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, WS2812_PIN, NEO_GRB + NEO_KHZ800);
Battery battery(3400, 4000, BattRead);
///////////////////////////////////INCREASE OR DECREASE VALUES////////////////////////////////////////////////////////////////////
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
///////////////////////////////////RAINBOW TOOLS//////////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////VISUAL EQUALIZER///////////////////////////////////////////////////////////////////////////////
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
}
void BarResize() {
  if (oBandValues >= Sensy)
    oBandValues--;
  else if (Sensy == InitSensy)
    oBandValues = Sensy; //always fit bars on 8 pixels
}
void VisualEq() {
  readMSGEQ7();
  BarResize();
  for (bandNo = 0; bandNo < EqAnBars; bandNo++) {
    int toY = map(bandValues[bandNo], 30, oBandValues, 0, VOres);
    for (int y = 0; y < toY; y++) {
      switch (DisplayMode) {
      case 0 /*vertical bars*/:
        fMode = (bandNo * VOres) + y;
        break;
      case 1 /*hotizontal bars*/:
        fMode = (y * VOres) + ((VOres - 1) - bandNo);
        break;
      }
      strip.setPixelColor(fMode, Wheel(((bandNo * VOres) + y) + j));
    }
  }
}
///////////////////////////////////DISPLAY BATTERY LEVEL//////////////////////////////////////////////////////////////////////////
void BattLevel() {
  static byte BattToPixel;
  if (millis() - t5 >= 1000) {
    BattToPixel = map(battery.level(), 0, 100, 0, VOres);
    t5 = millis();
  }
  for (byte i = 0; i < BattToPixel; i++) {
    switch (DisplayMode) {
    case 0 /*vertical*/:
      fMode = i + VOres * EqAnBars;
      break;
    case 1 /*horizontal*/:
      fMode = i * VOres;
      break;
    }
    strip.setPixelColor(fMode, Wheel((VOres * EqAnBars) + i + j));
  }
}
///////////////////////////////////DISPLAY LETTERS OR RAINBOW/////////////////////////////////////////////////////////////////////
void DAlphabetOrRainbow(byte DMode, byte SLettNumber) {
  increasej();
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
      switch (DMode) {
      case 0:
        DVal = (pgm_read_byte(&LettNumber[SLettNumber][(VOres - 1) - l][h]));
        break;
      case 1:
        DVal = 1;
        break;
      }
      strip.setPixelColor(fMode, (Wheel(((h * VOres) + l) + j) * DVal));
    }
  }
}
///////////////////////////////////LETTER MENU////////////////////////////////////////////////////////////////////////////////////
void ClearWithDelay(int DelayVal) {
  strip.clear();
  strip.show();
  delay(DelayVal);
}
void LettMenu() {
  static bool MemEn = false;
  static byte k, Phrase[32];
  static long t2, t3;
  if (digitalRead(B_1) || digitalRead(B_3)) {
    t2 = millis();
    MemEn = true;
  } else if ((millis() - t2 >= LettDelay) && MemEn) {
    ClearWithDelay(LettOffDelay);
    Phrase[k] = LettNumb;
    k++;
    MemEn = false;
  }
  if (digitalRead(B_1) && digitalRead(B_3)) {
    byte i = 0;
    ClearWithDelay(LettOffDelay);
    t3 = millis();
    while (i < k) {
      strip.clear();
      DAlphabetOrRainbow(0, Phrase[i]);
      strip.show();
      if (millis() - t3 >= LettDelay) {
        ClearWithDelay(10);
        i++;
        t3 = millis();
      }
    }
    k = 0;
    MemEn = false;
    ClearWithDelay(LettOffDelay);
  }
  DAlphabetOrRainbow(0, LettNumb);
}
///////////////////////////////////MENU///////////////////////////////////////////////////////////////////////////////////////////
bool fMenuLabel;
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
  DAlphabetOrRainbow(0, MenuLett);
}
void BttItf() {
  if (millis() - t1 >= SwDelay) {
    if (digitalRead(B_2)) {
      t1 = millis();
      t4 = millis();
      if (MenuMode < 5)
        MenuMode++;
      else
        MenuMode = 0;
    } else if (!fMenuLabel) {
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
        break;
      }
    }
  } else if ((!digitalRead(B_1)) && (!digitalRead(B_2)) && (!digitalRead(B_3)))
    t1 = millis() + SwDelay;
}


///////////////////////////////////MAIN PROGRAM///////////////////////////////////////////////////////////////////////////////////
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

  BttItf();
  if (((millis() - t4) <= LettDelay)) {
    fMenuLabel = true;
    MenuLettSel();
  } else fMenuLabel = false;

  if (digitalRead(B_2) || fMenuLabel) {
  } else if (MenuMode == 3)
    LettMenu();
  else if ((MenuMode == 4) || (MenuMode == 5))
    DAlphabetOrRainbow(1, 0);
  else {
    increasej();
    //if(digitalRead(BATTVS)) //remove "//" when circuit upgraded
    VisualEq();
    BattLevel();
  }
  strip.show();
}
