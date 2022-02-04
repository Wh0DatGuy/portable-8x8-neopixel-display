#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
//pins
#define STROBE_PIN 4
#define MIC_EN 5
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
#define LettDelay 1000
#define LettOffDelay 200
#define InitSwDelay 250
#define MinSwDelay 90
const int StepSwDelay = (InitSwDelay - MinSwDelay) / 8;
unsigned int SwDelay = InitSwDelay;
//constant variables
#define EqAnBars 7
#define VOres 8
#include <letter_experimental.h>
//global variables
int bandValues[EqAnBars], oBandValues, bandNo;
byte j;
byte MenuMode = 0, DisplayMode = 0;
int Sensy;
byte Brighty, Speedy = 8;
bool Direction = 0, fMenuLabel, LettMemEn = false, LettSave = false, LettDis = false;
byte fMode;
long t1;
byte LettNumb = 0, MenuLett;
bool fLettMenuReset = false;
bool DispBuffer[VOres + 1][VOres];
bool fMicOn;
//library initializations
Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, WS2812_PIN, NEO_GRB + NEO_KHZ800);
//cycle trough colours to create rainbow effect
void increasej() {
  static long t0;
  if ((millis() - t0) >= Speedy) {
    if (j < 256) {
      if (Direction == 1) {
        j++;
      } else {
        j--;
      }
    } else {
      j = 0;
    }
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
//read MSGEQ7 and store in an array
void readMSGEQ7() {
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(RESET_PIN, LOW);

  for (bandNo = 0; bandNo < EqAnBars; bandNo++) {
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(30);
    bandValues[bandNo] = analogRead(ANALOG_PIN);
    digitalWrite(STROBE_PIN, HIGH);

    if (bandValues[bandNo] > oBandValues) {
      oBandValues = bandValues[bandNo];
    }
  }
  if (oBandValues >= Sensy) {
    oBandValues--; //always fit bars on 8 pixels
  } else if (Sensy == InitSensy) {
    oBandValues = Sensy;
  }
}
//visual equalizer
void VisualEq() {
  readMSGEQ7();
  for (bandNo = 0; bandNo < EqAnBars; bandNo++) {
    int toY = map(bandValues[bandNo], 30, oBandValues, 0, VOres);
    for (byte h = 0; h < toY; h++) {
      DispBuffer[bandNo][h] = 1;
    }
  }
}
//Calculate and display battery level
void BattLevel() {
  float BattVolt = analogRead(BattRead) * (5.0 / 1023.0);
  int BattToPixel = (BattVolt - 3.2) * (VOres / (4 - 3.2));
  for (byte h = 0; h < BattToPixel; h++) {
    DispBuffer[VOres - 1][h] = 1;
  }
}
//display  full screen rainbow effect
void DispRainbow() {
  for (byte l = 0; l < VOres; l++) {
    for (byte h = 0; h < VOres; h++) {
      DispBuffer[l][h] = 1;
    }
  }
}
//display letters
void DispAlphabet(byte DMode, byte SLettNumber = 0, byte NxSLettNumber = 0, byte CurLettNumb = 0, byte MaxLettNumb = 1) {
  static byte offset = 0, oCurLettNumb = 28;
  static long t6 = 0;
  for (byte l = 0; l < VOres; l++) {
    for (byte h = 0; h < VOres; h++) {
      bool DVal;
      switch (DMode) {
      case 0:
        DVal = (((pgm_read_byte(&LettNumber[SLettNumber][(VOres - 1) - h])) >> l) & 1);
        DispBuffer[l][h] = DVal;
        break;
      case 1:
        if (CurLettNumb != oCurLettNumb) {
          offset = 0;
          oCurLettNumb = CurLettNumb;
          t6 = millis();
        }
        if (CurLettNumb < (MaxLettNumb - 1)) {
          if ((millis() - t6) >= (LettDelay / (VOres))) {
            if (offset < VOres) {
              offset++;
            }
            t6 = millis();
          }
        }
        if (l < (VOres - offset)) {
          DVal = (((pgm_read_byte(&LettNumber[SLettNumber][(VOres - 1) - h])) >> (l + offset)) & 1);
        } else {
          DVal = (((pgm_read_byte(&LettNumber[NxSLettNumber][(VOres - 1) - h])) >> (l - (VOres - (offset)))) & 1);
        }
        DispBuffer[l][h] = DVal;
        //DispBuffer[VOres - offset][h] = 0;
        break;
      }
    }
  }
}
//view display as horizzontal or vertical
void VerticalOrHorizontal() {
  for (byte l = 0; l < VOres; l++) {
    for (byte h = 0; h < VOres; h++) {
      switch (DisplayMode) {
      case 0 /*vertical*/:
        fMode = (l * VOres) + h;
        break;
      case 1 /*horizontal*/:
        fMode = (h * VOres) + ((VOres - 1) - l);
        break;
      }
      bool DVal;
      DVal = DispBuffer[l][h];
      strip.setPixelColor(fMode, (Wheel(((l * VOres) + h) + j) * DVal));
    }
  }
}

void buffer_clear() {
  for (byte l = 0; l < VOres; l++) {
    for (byte h = 0; h < VOres; h++) {
      DispBuffer[l][h] = false;
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
  static int PhraseL, Phrase[20];
  static long t2, t3;
  static bool LettSel = true;
  static byte i;
  static bool fkReset = false;
  if (fLettMenuReset) {
    LettSel = true;
    fkReset = true;
    LettDis = false;
    fLettMenuReset = false;
  }
  if (LettSel) {
    if (digitalRead(B_1) || digitalRead(B_3)) {
      t2 = millis();
      LettMemEn = true;
    } else if ((((millis() - t2) >= LettDelay) && LettMemEn) || LettSave) {
      ClearWithDelay(LettOffDelay);
      if (fkReset) {
        PhraseL = 0;
        fkReset = false;
      }
      Phrase[PhraseL] = LettNumb;
      PhraseL++;
      LettMemEn = false;
      LettSave = false;
    }
    if ((digitalRead(B_1) && digitalRead(B_3)) || LettDis) {
      ClearWithDelay(LettOffDelay);
      LettSel = false;
      i = 0;
      t3 = millis();
    }
    DispAlphabet(0, LettNumb);
  } else {
    if (millis() - t3 >= LettDelay) {
      if (i < (PhraseL - 1)) {
        i++;
      } else {
        i = 0;
        ClearWithDelay(LettDelay);
      }
      t3 = millis();
    }
    DispAlphabet(1, Phrase[i], Phrase[i + 1], i, PhraseL);
  }
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
  DispAlphabet(0, MenuLett);
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
    LettNumb = IncrDecr(52, LettNumb, 1);
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
    SwDelay = InitSwDelay;
    t1 = millis() + SwDelay;
  } else if ((millis() - t1) >= SwDelay) {
    SwDelay -= StepSwDelay;
    if (SwDelay <= MinSwDelay) {
      SwDelay = MinSwDelay;
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
    fLettMenuReset = true;
    fMenuLabel = true;
    MenuLettSel();
  } else
    fMenuLabel = false;
}

void bluetooth() {
  while (Serial.available() > 0) {
    static bool LettCapital = false;
    switch (Serial.read()) {
    case 'x':
      break;
    case 'O':
      DisplayMode = false;
      break;
    case 'o':
      DisplayMode = true;
      break;
    case 'd':
      Direction = true;
      break;
    case 'D':
      Direction = false;
      break;
    case 'T':
      MenuMode = 3;
      break;
    case 'A':
      LettCapital = true;
      break;
    case 'a':
      LettCapital = false;
      break;
    case 'M':
      LettNumb = Serial.parseInt();
      if ((LettCapital) && (LettNumb != 52)) {
        LettNumb += 26;
      }
      break;
    case 't':
      MenuMode = 0;
      break;
    case 's':
      LettSave = true;
      break;
    case 'b':
      LettDis = true;
      break;
    case 'B':
      MenuMode = 0;
      Brighty = (Serial.parseInt() * 15);
      strip.setBrightness(Brighty);
      break;
    case 'V':
      Sensy = (Serial.parseInt() * 90);
      break;
    case 'S':
      Speedy = Serial.parseInt();
      break;
    }
  }
}
//main program
void setup() {
  strip.begin();
  Serial.begin(9600);
  strip.show();
  oBandValues = InitSensy;
  Sensy = InitSensy;
  Brighty = map(analogRead(PHOTOR), 0, 700, 15, 255);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(MIC_EN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
  strip.setBrightness(Brighty);
}

void loop() {
  strip.clear();
  buffer_clear();
  increasej();

  Menu();
  bluetooth();

  if (digitalRead(B_2) || fMenuLabel) {
    LettMemEn = false; //if exit letter menu reset letter timer
  } else
    switch (MenuMode) {
    default:
      fLettMenuReset = true;
      if (digitalRead(BATTVS)) {
        fMicOn = true;
      } else {
        bandValues[3] = bandValues[3] - 60;
        fMicOn = false;
      }
      digitalWrite(MIC_EN, fMicOn);
      VisualEq();
      BattLevel();
      break;
    case 3:
      LettMenu();
      break;
    case 4:
    case 5:
      DispRainbow();
      break;
    }

  VerticalOrHorizontal();

  strip.show();
}
