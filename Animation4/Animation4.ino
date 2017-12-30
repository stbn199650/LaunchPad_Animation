#include <Wire.h>
#include "Adafruit_Trellis.h"

Adafruit_Trellis matrix0 = Adafruit_Trellis();  //create Adafruit_Trellis constructor
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

#define NUMTRELLIS 4
#define numKeys (NUMTRELLIS * 16) //64
#define INTPIN A2
#define pinLED 13

int state, i = 0, j = 0, k = 0, count = 0;
int russian[64];

int rightbutton = 0;
int show[8][8][8] = {{{0}}};

void setup() {

  pinMode(pinLED, OUTPUT);
  Serial.begin(256000);
  pinMode(INTPIN, INPUT);  // INT pin requires a pullup
  digitalWrite(INTPIN, HIGH);
  trellis.begin(0x70, 0x71, 0x72, 0x73);
  Wire.setClock(400000L);

  for (i = 0; i < 64; i++)
    russian[i] = 0;

  trellis.setLED(0);     //divide 64 led into 4 4*4 matrix
  trellis.writeDisplay();
  delay(200);
  trellis.clrLED(0);
  trellis.writeDisplay();
}

int Buttons[8][8] = { //Grid of button ID's
  { 0, 1,  2,  3,  16, 17, 18, 19},
  { 4, 5,  6,  7,  20, 21, 22, 23},
  { 8, 9,  10, 11, 24, 25, 26, 27},
  {12, 13, 14, 15, 28, 29, 30, 31},
  {32, 33, 34, 35, 48, 49, 50, 51},
  {36, 37, 38, 39, 52, 53, 54, 55},
  {40, 41, 42, 43, 56, 57, 58, 59},
  {44, 45, 46, 47, 60, 61, 62, 63}
};

int getButton(int pv) { //Returns the ID of the cooresponding button when given a MIDI note
  if (pv > 83) {
    pv = pv - 84;
    int floored = floor(pv / 4);
    return Buttons[3 - floored][(pv % 4) + 4];
  }
  if (pv > 67) {
    pv = pv - 68;
    int floored = floor(pv / 4);
    return Buttons[(3 - floored) + 4][(pv % 4) + 4];
  }
  if (pv > 51) {
    pv = pv - 52;
    int floored = floor(pv / 4);
    return Buttons[3 - floored][pv % 4];
  }
  else {
    pv = pv - 36;
    int floored = floor(pv / 4);
    return Buttons[(3 - floored) + 4][pv % 4];
  }
}

int h_to_column[8][8] = {{0, 4, 8, 12, 32, 36, 40, 44}, {1, 5, 9, 13, 33, 37, 41, 45}, {2, 6, 10, 14, 34, 38, 42, 46}, {3, 7, 11, 15, 35, 39, 43, 47}, {16, 20, 24, 28, 48, 52, 56, 60}, {17, 21, 25, 29, 49, 53, 57, 61}, {18, 22, 26, 30, 50, 54, 58, 62}, {19, 23, 27, 31, 51, 55, 59, 63}};

int column = 0, m = 0, n = 0;
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed

  // If a button was just pressed or released
  if (trellis.readSwitches()) {
    for (uint8_t h = 0; h < numKeys; h++) {   //numKeys=64
      if (trellis.justPressed(h)) { //press led h

        for (i = 0; i < 8; i++) { //right most button
          if (h == h_to_column[7][i]) {
            rightbutton = (h - 19) / 4;
          }
        }

        //decide button in which column
        for (i = 0; i < 8; i++) {
          for (j = 0; j < 8; j++) {
            if (h == h_to_column[i][j]) {
              column = h_to_column[i][0];
              break;
            }
          }
        }

        //press right most button, clear all led
        if (column == 19) {
          for (i = 0; i < 64; i++)
            trellis.clrLED(i);
          trellis.writeDisplay();
        }
        Serial.print("rightbutton");
        Serial.println(rightbutton);
        if (rightbutton == 8 && column != 19) {
          Serial.println("rightbutton is 4 ");
          m = h / 16;
          animation4(m, h);
        }

      }
    }
  }
}
int line1[8][8] = {{19, 44, -1, -1, -1, -1, -1, -1}, {18, 23, 40, 45, -1, -1, -1, -1}, {17, 22, 27, 36, 41, 46, -1, -1}, {16, 21, 26, 31, 32, 37, 42, 47},
  {20, 25, 30, 51, 33, 38, 43, 60}, {24, 29, 50, 55, 34, 39, 56, 61}, {28, 49, 54, 59, 35, 52, 57, 62}, {48, 53, 58, 63, -1, -1, -1, -1}
};
int line4[8][8] = {{19, 44, -1, -1, -1, -1, -1, -1}, {18, 23, 40, 45, -1, -1, -1, -1}, {17, 22, 27, 36, 41, 46, -1, -1}, {16, 21, 26, 31, 32, 37, 42, 47},
  {3, 20, 25, 30, 33, 38, 43, 12}, {2, 7, 24, 29, 8, 13, 34, 39}, {1, 6, 11, 28, 4, 9, 14, 35}, {0, 5, 10, 15, -1, -1, -1, -1}
};
int line2[12][8] = {{0, 1, 2, 4, 5, 8, -1, -1}, {2, 5, 8, 3, 6, 9, 12, -1}, {3, 6, 9, 12, 7, 10, 13, 32},
  {7, 10, 13, 32, 11, 14, 33, 36}, {11, 14, 33, 36, 15, 34, 37, 40}, {15, 34, 37, 40, 35, 38, 41, 44},
  {35, 38, 41, 44, 48, 39, 42, 45}, {48, 39, 42, 45, 49, 52, 43, 46}, {49, 52, 43, 46, 50, 53, 56, 47},
  {50, 53, 56, 47, 51, 54, 57, 60}, {51, 54, 57, 60, 55, 58, 61, -1}, {55, 58, 61, 59, 62, 63, -1, -1}
};
int line3[12][8] = {{55, 58, 61, 59, 62, 63, -1, -1}, {51, 54, 57, 60, 55, 58, 61, -1}, {31, 50, 53, 56, 51, 54, 57, 60},
  {27, 30, 49, 52, 31, 50, 53, 56}, {23, 26, 29, 48, 27, 30, 49, 52}, {19, 22, 25, 28, 23, 26, 29, 48},
  {18, 21, 24, 15, 19, 22, 25, 28}, {17, 20, 11, 14, 18, 21, 24, 15}, {16, 7, 10, 13, 17, 20, 11, 14},
  {3, 6, 9, 12, 16, 7, 10, 13}, {2, 5, 8, 3, 6, 9, 12, -1}, {0, 1, 2, 4, 5, 8, -1, -1}
};

void animation4(int m, int h) {
  trellis.setLED(h);
  trellis.writeDisplay();
  if (m == 0) {
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++)
        if (line1[i][j] != -1)
          trellis.setLED(line1[i][j]);
      trellis.writeDisplay();
      delay(50);
      
      for (j = 0; j < 8; j++)
        if (line1[i][j] != -1)
          trellis.clrLED(line1[i][j]);
      trellis.writeDisplay();
    }
  } else if (m == 1) {
    for (i = 0; i < 12; i++) {
      for (j = 0; j < 8; j++)
        if (line2[i][j] != -1)
          trellis.setLED(line2[i][j]);
      trellis.writeDisplay();
      delay(30);
      
      for (j = 0; j < 8; j++)
        if (line2[i][j] != -1)
          trellis.clrLED(line2[i][j]);
      trellis.writeDisplay();
    }
  } else if (m == 2) {
    for (i = 0; i < 12; i++) {
      for (j = 0; j < 8; j++)
        if (line2[i][j] != -1)
          trellis.setLED(line3[i][j]);
      trellis.writeDisplay();
      delay(30);
      
      for (j = 0; j < 8; j++)
        if (line2[i][j] != -1)
          trellis.clrLED(line3[i][j]);
      trellis.writeDisplay();
    }
    
  } else if (m == 3) {
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++)
        if (line3[i][j] != -1)
          trellis.setLED(line4[i][j]);
      trellis.writeDisplay();
      delay(50);
      
      for (j = 0; j < 8; j++)
        if (line3[i][j] != -1)
          trellis.clrLED(line4[i][j]);
      trellis.writeDisplay();
    }
  }
  trellis.clrLED(h);
  trellis.writeDisplay();
}
unsigned long timeForCheck = 0;
byte delayTime = 30; // Time between checks for button presses in milliseconds
void loop() {

  if (millis() > timeForCheck) {  //button has been pressed
    buttonCheck();
    trellis.writeDisplay();
    timeForCheck = millis() + delayTime;
  }
}

