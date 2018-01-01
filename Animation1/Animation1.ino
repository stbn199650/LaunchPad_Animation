//This code will let you use any Arduino combined with 4 Adafruit Trellises to mimic the basic functions of a launchpad (Or just use it as a MIDI controller)
//For this code to work, you will need a few extra programs on the computer
//You will need to use a program called loopMIDI to create virtual MIDI channels
//And you also need a program called HairlessMIDISerial to convert Serial to Midi (You need to set the Baud Rate to 256000 in the options)
//You also need to install the Arduino MIDI library and the Adafruit Trellis library

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

int arrow[16][4][4] = {{{0, -1, -1, -1}, {1, 4, 5, -1}, {6, 9, 10, -1}, {11, 14, 15, -1}},
  {{1, -1, -1, -1}, {0, 2, 5, -1}, {4, 6, 9, -1}, {8, 10, 13, -1}},
  {{2, -1, -1, -1}, {1, 3, 6, -1}, {5, 7, 10, -1}, {9, 11, 14, -1}},
  {{3, -1, -1, -1}, {2, 6, 7, -1}, {5, 9, 10, -1}, {8, 12, 13, -1}},
  {{4, -1, -1, -1}, {0, 5, 8, -1}, {1, 6, 9, -1}, {2, 7, 10, -1}},
  {{5, -1, -1, -1}, {1, 4, 6, 9}, {7, 13, -1, -1}, { -1, -1, -1, -1}},
  {{6, -1, -1, -1}, {2, 5, 7, 10}, {4, 14, -1, -1}, { -1, -1, -1, -1}},
  {{7, -1, -1, -1}, {3, 6, 11, -1}, {2, 5, 10, -1}, {1, 4, 9, -1}},
  {{8, -1, -1, -1}, {4, 9, 12, -1}, {5, 10, 13, -1}, {6, 11, 14, -1}},
  {{9, -1, -1, -1}, {5, 8, 10, 13}, {1, 11, -1, -1}, { -1, -1, -1, -1}},
  {{10, -1, -1, -1}, {6, 9, 11, 14}, {2, 8, -1, -1}, { -1, -1, -1, -1}},
  {{11, -1, -1, -1}, {7, 10, 15, -1}, {6, 9, 14, -1}, {5, 8, 13, -1}},
  {{12, -1, -1, -1}, {8, 9, 13, -1}, {5, 6, 10, -1}, {2, 3, 7, -1}},
  {{13, -1, -1, -1}, {9, 12, 14, -1}, {5, 8, 10, -1}, {1, 4, 6, -1}},
  {{14, -1, -1, -1}, {10, 13, 15, -1}, {6, 9, 11, -1}, {2, 5, 7, -1}},
  {{15, -1, -1, -1}, {10, 11, 14, -1}, {5, 6, 9, -1}, {0, 1, 4, -1}}
};

int horizental_flash[8][3] = {{0, 1, 2}, {1, 2, 3}, {2, 3, 16}, {3, 16, 17}, {16, 17, 18}, {17, 18, 19}, {18, 19, -1}, {19, -1, -1}};

int m = 0, n = 0;
int row[8] = {0, 1, 2, 3, 16, 17, 18, 19};
int column = 0;
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
              column = Buttons[0][j];              
            }
          }
        }

        //push right most button, clear all led
        if (column == 19) {
          for (i = 0; i < 64; i++)
            trellis.clrLED(i);
          trellis.writeDisplay();
        }

        if (rightbutton == 1 && column != 19) {
          Serial.println("rightbutton is 1 ");
          m = h / 16;   //button is in which matrix
          n = h % 16;
          animation1(m, n);
        } else if (rightbutton == 2 && column != 19) {
          m = h / 16;   //button is in which matrix
          n = h % 16;
          Serial.print("h ");
          Serial.println(h);
          animation2(m, n, h);
        }

      }
    }
  }
}

void animation1(int m, int n) { //m:0~3, n:0~15
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (arrow[n][i][j] != -1) {
        trellis.setLED(arrow[n][i][j] + 16 * m);
      }
    }
    trellis.writeDisplay();
    delay(130);
    for (j = 0; j < 4; j++) {
      if (arrow[n][i][j] != -1) {
        trellis.clrLED(arrow[n][i][j] + 16 * m);
      }
    }
    trellis.writeDisplay();
  }
}

void animation2(int m, int n, int h) {

  int row = 0, col = n % 4;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      if (h == Buttons[i][j]) {
        row = Buttons[i][0];
        break;
      }
    }
  }

  if (m == 0) {
    for (i = col; i < 8; i++) {
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.setLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
      delay(150);
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.clrLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
    }
  } else if (m == 1) {
    for (i = col+2; i >= 0; i--) {
      for (j = 0; j < 3; j++){
        if (horizental_flash[i][j] != -1)
          trellis.setLED(horizental_flash[i][j] + row);          
        
        if(i<=1)
        Serial.println(horizental_flash[i][j] + row);
      }
      trellis.writeDisplay();
      delay(150);
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.clrLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
    }
  } else if (m == 2) {
    for (i = col; i < 8; i++) {
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.setLED(horizental_flash[i][j] + row);

      trellis.writeDisplay();
      delay(150);
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.clrLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
    }
  } else if (m == 3) {
    for (i = col+2; i >= 0; i--) {
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.setLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
      delay(150);
      for (j = 0; j < 3; j++)
        if (horizental_flash[i][j] != -1)
          trellis.clrLED(horizental_flash[i][j] + row);
      trellis.writeDisplay();
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

