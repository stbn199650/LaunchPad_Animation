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

int i = 0, j = 0;
int rightbutton = 0;
void setup() {

  pinMode(pinLED, OUTPUT);
  Serial.begin(256000);
  pinMode(INTPIN, INPUT);  // INT pin requires a pullup
  digitalWrite(INTPIN, HIGH);
  trellis.begin(0x70, 0x71, 0x72, 0x73);
  Wire.setClock(400000L);

  trellis.setLED(0);     //divide 64 led into 4 4*4 matrix
  trellis.writeDisplay();
  delay(200);
  trellis.clrLED(0);
  trellis.writeDisplay();
}

/* I have my Trellis's connected like this (Veiwed from the top, the side with the buttons and LED's):
    Arduino
       |
      0x70 - 0x71
       |       |
      0x72 - 0x73

  If yours are set up differently, you will have to move around the numbers below */
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

int show1[4][29] = {{15, 28, 35, 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10, 11, 14, 34, 38, 39, 52, 53, 49, 29, 25, 24, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {5, 6, 7, 20, 21, 9, 13, 33, 37, 41, 42, 43, 56, 57, 58, 54, 50, 30, 26, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 2, 3, 16, 17, 18, 19, 4, 8, 12, 32, 36, 40, 44, 45, 46, 47, 60, 61, 62, 63, 23, 27, 31, 51, 55, 59, 63}
};

int show2[9][24] = {{0, 5, 10, 15, 19, 22, 25, 28, 35, 38, 41, 44, 48, 53, 58, 63, -1, -1, -1, -1, -1, -1, -1, -1},
  {4, 9, 14, 35, 48, 39, 42, 45, 28, 49, 54, 59, 15, 24, 21, 18, -1, -1, -1, -1, -1, -1, -1, -1},
  {3, 16, 6, 7, 10, 11, 12, 32, 33, 37, 34, 38, 47, 60, 56, 57, 52, 53, 31, 51, 26, 30, 25, 29},
  {3, 16, 6, 7, 10, 12, 32, 33, 37, 38, 47, 60, 56, 57, 53, 31, 51, 30, 25, 26, 15, 28, 35, 48},
  {2, 3, 5, 6, 11, 24, 32, 36, 37, 41, 14, 34, 60, 61, 57, 58, 52, 39, 27, 31, 22, 26, 29, 49},
  {0, 1, 5, 6, 10, 19, 23, 22, 26, 25, 40, 44, 37, 41, 38, 62, 63, 57, 58, 53, -1, -1, -1, -1},
  {0, 1, 6, 40, 44, 37, 57, 62, 63, 19, 23, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 1, 40, 44, 62, 63, 19, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {0, 44, 63, 19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

int m = 0, n = 0;
int row = 0, column = 0;
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed

  // If a button was just pressed or released
  if (trellis.readSwitches()) {
    //Serial.println("Hello Arduino");

    for (uint8_t h = 0; h < numKeys; h++) {   //numKeys=64
      if (trellis.justPressed(h)) { //press led h
        //press right most button

        for (i = 0; i < 8; i++) { //right most button
          if (h == Buttons[i][7]) {
            rightbutton = (h - 19) / 4;
          }
        }
        //decide button in which column
        for (i = 0; i < 8; i++) {
          for (j = 0; j < 8; j++) {
            if (h == Buttons[i][j]) {
              m = i;
              n = j;
            }
            if (h == Buttons[i][j]) {
              column = Buttons[0][j];
              Serial.print(column);
            }
          }
        }

        //push right most button, clear all led
        if (column == 19) {
          for (i = 0; i < 64; i++)
            trellis.clrLED(i);
          trellis.writeDisplay();
        }

        if (rightbutton == 10 && column != 19) {
          Serial.println("rightbutton is 10 ");
          animation6(m, n);
        } else if (rightbutton == 11 && column != 19) {
          animation7(h);
        }

      }
    }
  }
}

void animation6(int m, int n) {
  int a = m - 1, b = m + 1, c = n - 1, d = n + 1;

  trellis.setLED(Buttons[m][n]);
  delay(80);
  trellis.clrLED(Buttons[m][n]);

  for (int x = 0; x < 8; x++) {
    if (a >= 0)
      trellis.setLED(Buttons[a][n]);
    if (b < 8)
      trellis.setLED(Buttons[b][n]);
    if (c >= 0)
      trellis.setLED(Buttons[m][c]);
    if (d < 8)
      trellis.setLED(Buttons[m][d]);

    trellis.writeDisplay();
    delay(80);
    trellis.clrLED(Buttons[a][n]);
    trellis.clrLED(Buttons[b][n]);
    trellis.clrLED(Buttons[m][c]);
    trellis.clrLED(Buttons[m][d]);
    trellis.writeDisplay();
    a--; b++; c--; d++;
  }
}

int count = 0;
void animation7(int h) {
  if (h == 15 || h == 28 || h == 35 || h == 48 || h == 10 || h == 11 || h == 24 || h == 25 || h == 14 || h == 34 || h == 38 || h == 39 || h == 52 || h == 53 || h == 29 || h == 49 ) {
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 29; j++) {
        if (show1[i][j] != -1)
          trellis.setLED(show1[i][j]);
      }
      trellis.writeDisplay();
      delay(80);
      for (j = 0; j < 29; j++)
        trellis.clrLED(show1[i][j]);
      trellis.writeDisplay();
    }
  } else {
    
    if (count == 0) {
      for (i = 0; i < 9; i++) {
        for (j = 0; j < 24; j++) {
          if (show2[i][j] != -1)
            trellis.setLED(show2[i][j]);
        }
        trellis.writeDisplay();
        delay(100);
        for (j = 0; j < 24; j++)
          trellis.clrLED(show2[i][j]);
        trellis.writeDisplay();
      }
      count = 1;
      
    } else if (count == 1) {
      for (i = 8; i >= 0; i--) {
        for (j = 0; j < 24; j++) {
          if (show2[i][j] != -1)
            trellis.setLED(show2[i][j]);
        }
        trellis.writeDisplay();
        delay(100);
        for (j = 0; j < 24; j++)
          trellis.clrLED(show2[i][j]);
        trellis.writeDisplay();
      }
      count = 0;
    }
  }

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

