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

int h_to_column[8][8] = {{0, 4, 8, 12, 32, 36, 40, 44}, {1, 5, 9, 13, 33, 37, 41, 45}, {2, 6, 10, 14, 34, 38, 42, 46}, {3, 7, 11, 15, 35, 39, 43, 47}, {16, 20, 24, 28, 48, 52, 56, 60}, {17, 21, 25, 29, 49, 53, 57, 61}, {18, 22, 26, 30, 50, 54, 58, 62}, {19, 23, 27, 31, 51, 55, 59, 63}};

int circle[4][26] = {{0, 1, 2, 3, 16, 17, 18, 22, 26, 30, 50, 54, 58, 4, 8, 12, 32, 36, 40, 44, 45, 46, 47, 60, 61, 62},
  {5, 6, 7, 20, 21, 25, 29, 49, 53, 57, 9, 13, 33, 37, 41, 42, 43, 56, -1, -1, -1, -1, -1, -1, -1, -1},
  {10, 11, 24, 28, 14, 34, 38, 39, 52, 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {15, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

int show1[10][8] = {{0, 1, 2, 3, 47, 60, 61, 62}, {1, 2, 3, 16, 46, 47, 60, 61}, {2, 3, 16, 17, 45, 46, 47, 60}, {3, 16, 17, 18, 44, 45, 46, 47},
  {16, 17, 18, 22, 40, 44, 45, 46}, {17, 18, 22, 26, 36, 40, 44, 45}, {18, 22, 26, 30, 32, 36, 40, 44}, {22, 26, 30, 50, 12, 32, 36, 40},
  {26, 30, 50, 54, 8, 12, 32, 36}, {30, 50, 54, 58, 4, 8, 12, 32}
};

int show2[7][6] = {{5, 6, 7, 43, 56, 57}, {6, 7, 20, 42, 43, 56}, {7, 20, 21, 41, 42, 43}, {20, 21, 25, 37, 41, 42},
  {21, 25, 29, 33, 37, 41}, {25, 29, 49, 13, 33, 37}, {29, 49, 53, 9, 13, 33}
};

int show3[5][4] = {{10, 11, 39, 52}, {11, 24, 38, 39}, {24, 28, 34, 38}, {28, 48, 14 , 34}, {10, 14, 48, 52}};

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

        if (rightbutton == 3 && column != 19) {
          Serial.println("rightbutton is 3 ");
          for (i = 0; i < 4; i++) 
            for (j = 0; j < 26; j++) 
              if (h == circle[i][j]) 
                m = i;               
          
          animation3(m);          
        } 
        
      }
    }
  }
}

void animation3(int m) {
  if (m == 0) {
    for (i = 0; i < 10; i++) {
      for (j = 0; j < 8; j++) {
        if (show1[i][j] != -1)
          trellis.setLED(show1[i][j]);
      }
      trellis.writeDisplay();

      delay(60);
      for (j = 0; j < 8; j++) {
        if (show1[i][j] != -1)
          trellis.clrLED(show1[i][j]);
      }
    }
  } else if (m == 1) {
    for (i = 0; i < 7; i++) {
      for (j = 0; j < 6; j++) {
        if (show2[i][j] != -1)
          trellis.setLED(show2[i][j]);        
      }
      trellis.writeDisplay();
      delay(70);
      for (j = 0; j < 6; j++) {
        if (show2[i][j] != -1)
          trellis.clrLED(show2[i][j]);
      }
    }
  } else if (m == 2) {
    for (i = 0; i < 5; i++) {
      for (j = 0; j < 4; j++) {
        if (show3[i][j] != -1)
          trellis.setLED(show3[i][j]);
      }
      trellis.writeDisplay();
      delay(70);
      for (j = 0; j < 4; j++) {
        if (show3[i][j] != -1)
          trellis.clrLED(show3[i][j]);
      }
    }
  } else if (m == 3) {

    for (i = 3; i >= 0; i--) {
      for (j = 0; j < 26; j++) {
        if (circle[i][j] != -1)
          trellis.setLED(circle[i][j]);
      }
      trellis.writeDisplay();
      delay(70);
      for (j = 0; j < 26; j++) {
        if (circle[i][j] != -1)
          trellis.clrLED(circle[i][j]);
      }
    }
    //return
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 26; j++) {
        if (circle[i][j] != -1)
          trellis.setLED(circle[i][j]);
      }
      trellis.writeDisplay();
      delay(100);
      for (j = 0; j < 26; j++) {
        if (circle[i][j] != -1)
          trellis.clrLED(circle[i][j]);
      }
    }
  }
}

void animation4(int m) {
  
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

