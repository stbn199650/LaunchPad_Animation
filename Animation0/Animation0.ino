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
  delay(1000);
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

int show1[40] = {0, 1, 2, 3, 16, 17, 18, 19, 22, 21, 20, 7, 6, 5, 10, 11, 24, 25, 28, 15, 35, 48, 53, 52, 39, 38, 41, 42, 43, 56, 57, 58, 63, 62, 61, 60, 47, 46, 45, 44};
int show2[40] = {0, 4, 8, 12, 32, 36, 40, 44, 41, 37, 33, 13, 9, 5, 10, 14, 34, 38, 35, 15, 28, 48, 53, 49, 29, 25, 22, 26, 30, 50, 54, 58, 63, 59, 55, 51, 31, 27, 23, 19};
int show3[6][20] = {
  {15, 28, 35, 48, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {11, 14, 34, 39, 24, 29, 49, 52, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {10, 11, 24, 25, 14, 34, 38, 39, 52, 53, 29, 49, -1, -1, -1, -1, -1, -1, -1, -1},
  {7, 10, 13, 33, 38, 43, 20, 25, 30, 50, 53, 56, -1, -1, -1, -1, -1, -1, -1, -1},
  {5, 6, 7, 20, 21, 22, 9, 13, 33, 37, 41, 42, 43, 56, 57, 58, 26, 30, 50, 54},
  {2, 5, 8, 36, 41, 46, 55, 58, 61, 17, 22, 27, -1, -1, -1, -1, -1, -1, -1, -1}
};

int column = 0, m = 0, n = 0;
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed

  // If a button was just pressed or released
  if (trellis.readSwitches()) {

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

        if (rightbutton == 0 && column != 19) { //animation mode 1
          for (i = 0; i < 14; i++) {
            if (h == show1[i]) {
              m = 1;
              n = i;
              break;
            }else if(h==show1[39-i]){
               m = 2;
              n = i;
              break;
            }else if (h == show2[i]) {
              m = 3; n = i;
              break;
            }else if (h == show2[39-i]) {
              m = 4; n = i;
              break;
            }
            
          }
          for (i = 0; i < 2; i++) {
            for (j = 0; j < 8; j++) {
              if (h == show3[i][j]) {
                m = 5;
                n = i;
                break;
              }
            }
          }
          animation0(m, n);
        }

      }
    }
  }
}

void animation0(int m, int n) {
  Serial. print("In generate");
  if (m == 1) {
    
    for (i = n; i < 40 - n; i++) {
      trellis.setLED(show1[i]);      
      trellis.writeDisplay();
      delay(80);
      trellis.clrLED(show1[i]);
      trellis.writeDisplay();
    }
        
  } else if (m == 2) {
    
    for (i = 39-n; i >= n; i--) {
      trellis.setLED(show1[i]);      
      trellis.writeDisplay();
      delay(80);
      trellis.clrLED(show1[i]);
      trellis.writeDisplay();
    }    
    
  }else if (m == 3) {
    
    for (i = n; i < 40 - n; i++) {
      trellis.setLED(show2[i]);      
      trellis.writeDisplay();
      delay(80);
      trellis.clrLED(show2[i]);
      trellis.writeDisplay();
    }    
    
  }else if (m == 4) {
    
    for (i = 39-n; i >= n; i--) {
      trellis.setLED(show2[i]);      
      trellis.writeDisplay();
      delay(80);
      trellis.clrLED(show2[i]);
      trellis.writeDisplay();
    }
    
  } else if(m==5){ 
    for (i = n; i < 6; i++) {
      for (j = 0; j < 20; j++) {
        if (show3[i][j] != -1)
          trellis.setLED(show3[i][j]);
      }
      trellis.writeDisplay();
      delay(100);
      for (j = 0; j < 20; j++) {
        if (show3[i][j] != -1)
          trellis.clrLED(show3[i][j]);
      }
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


