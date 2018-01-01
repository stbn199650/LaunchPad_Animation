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

int Buttons[8][8] = { //Grid of button ID's
  { 0, 1, 2, 3, 16, 17, 18, 19},
  { 4, 5, 6, 7, 20, 21, 22, 23},
  { 8, 9, 10, 11, 24, 25, 26, 27},
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

int show[16][4] = {{0, 1, 4, 5}, {1, 2, 5, 6}, {2, 3, 6, 7}, {3, 7, 16, 20}, {4, 5, 8, 9}, {5, 6, 9, 10},
  {6, 7, 10, 11}, {7, 11, 20, 24}, {8, 9, 12, 13}, {9, 10, 13, 14}, {10, 11, 14, 15}, {11, 24, 15, 28},
  {12, 13, 32, 33}, {13, 14, 33, 34}, {14, 15, 34, 35}, {15, 28, 35, 48}
};
int column = 0, m = 0, n = 0;
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed

  // If a button was just pressed or released
  if (trellis.readSwitches()) {
    for (uint8_t h = 0; h < numKeys; h++) {   //numKeys=64
      if (trellis.justPressed(h)) { //press led h

        for (i = 0; i < 8; i++) { //right most button
          if (h == Buttons[i][7]) {
            rightbutton = (h - 19) / 4;
          }
        }
        //decide button in which column
        for (i = 0; i < 8; i++) {
          for (j = 0; j < 8; j++) {
            if (h == Buttons[i][j]) {
              column = Buttons[0][j];              
            }
          }
        }

        //press right most button, clear all led
        if (column == 19) {
          for (i = 0; i < 64; i++)
            trellis.clrLED(i);
          trellis.writeDisplay();
        }

        if (rightbutton == 9 && column != 19) {
          //Serial.println("rightbutton is 9 ");
          m = h / 16;   //button is in which matrix
          n = h % 16;
          animation5(m, n, h);

        }

      }
    }
  }
}
int temp[64] = {0};
void animation5(int m, int n, int h) { 
 int x;
 
  if (m == 0) {    
    for (i = 0; i < 16; i++) {
      if (show[i][0] == n) {
        for (j = 0; j < 4; j++) {
          x=show[i][j];
          if (temp[x] == 0) {
            trellis.setLED(x);
            temp[x] = 1;

          } else {
            trellis.clrLED(x);
            temp[x] = 0;
          }
        }
        trellis.writeDisplay();
        break;
      }
    }

  } else if (m == 1) {
    for (i = 0; i < 16; i++) {
      if (show[i][0] == n) {
        for (j = 0; j < 4; j++) {
          x=show[i][j]+16;          
          if (temp[x] == 0) {
            trellis.setLED(x);
            temp[x] = 1;
          } else {
            trellis.clrLED(x);
            temp[x] = 0;
          }
        }
        trellis.writeDisplay();
        break;
      }
    }
  } else if (m == 2) {
    for (i = 0; i < 16; i++) {
      if (show[i][0] == n) {
        for (j = 0; j < 4; j++) {
          x=show[i][j]+32;
          if (temp[x] == 0) {
            trellis.setLED(x);
            temp[x] = 1;
          } else {
            trellis.clrLED(x);
            temp[x] = 0;
          }
        }
        trellis.writeDisplay();
        break;
      }
    }
  } else if (m == 3) {   
    for (i = 0; i < 16; i++) {
      if (show[i][0] == n) {
        for (j = 0; j < 4; j++) {
          x=show[i][j]+48;          
          if (temp[x] == 0) {
            trellis.setLED(x);
            temp[x] = 1;
          } else {
            trellis.clrLED(x);
            temp[x] = 0;
          }
        }
        trellis.writeDisplay();
        break;
      }
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

