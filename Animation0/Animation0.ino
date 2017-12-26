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

int h_to_column[8][8] = {{0, 4, 8, 12, 32, 36, 40, 44}, {1, 5, 9, 13, 33, 37, 41, 45}, {2, 6, 10, 14, 34, 38, 42, 46}, {3, 7, 11, 15, 35, 39, 43, 47}, {16, 20, 24, 28, 48, 52, 56, 60}, {17, 21, 25, 29, 49, 53, 57, 61}, {18, 22, 26, 30, 50, 54, 58, 62}, {19, 23, 27, 31, 51, 55, 59, 63}};
int russian_table[8][2] = {{16, 32}, {17, 33}, {18, 34}, {19, 35}, {32, 48}, {33, 49}, {34, 50}, {35, 51}};
int row[8] = {0, 1, 2, 3, 16, 17, 18, 19};
int column = 0, m = 0, n = 0;
int randnum = 0; 
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed

  // If a button was just pressed or released
  if (trellis.readSwitches()) {
    //Serial.println("Hello Arduino");

    for (uint8_t h = 0; h < numKeys; h++) {   //numKeys=64
      if (trellis.justPressed(h)) { //press led h
        //press right most button

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
            }
          }
        }

        //push right most button, clear all led
        if (column == 19) {
          for (i = 0; i < 64; i++)
            trellis.clrLED(i);
          trellis.writeDisplay();
        }

        Serial.print("column  ");
        Serial.print(column);
        Serial.println();
        Serial.print("h  ");
        Serial.print(h);
        Serial.println();
        if (rightbutton == 0 && column != 19) { //animation mode 1
          //Serial.println("rightbutton is 0 ");
          randnum = random(9);//generate a number between 0~8
          animation0(randnum, column);        

        } else {
          for (i = 0; i < 64; i++) 
            trellis.clrLED(i);         
          trellis.writeDisplay();
        }

      }
    }
  }
}

int blink_led1, blink_led2, blink_led3, blink_led4;
void animation0(int x, int column) {
  Serial. print("In generate");
  switch (x) {
    case 0:
      blink_led1 = column;
      if (column != 3)
        blink_led2 = blink_led1 + 1;
      else
        blink_led2 = blink_led1 + 13;

      for (i = 0; i < 8; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led1 != 44 && blink_led1 != 45 && blink_led1 != 46 && blink_led1 != 47 && blink_led1 != 60 && blink_led1 != 61 && blink_led1 != 62 && blink_led1 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0]) {
              blink_led1 = russian_table[j][1];
            }
          }
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
          }
        }

        if (column == 3)
          blink_led2 = blink_led1 + 13;
        else
          blink_led2 = blink_led1 + 1;
      }

      break;

    case 1:
      blink_led1 = column; blink_led2 = blink_led1 + 4;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 44 && blink_led2 != 45 && blink_led2 != 46 && blink_led2 != 47 && blink_led2 != 60 && blink_led2 != 61 && blink_led2 != 62 && blink_led2 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(150);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4; blink_led2 = blink_led1 + 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
        }
      }

      break;

    case 2:
      blink_led1 = column;

      if (column != 3)
        blink_led2 = blink_led1 + 5;
      else
        blink_led2 = blink_led1 + 17;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 45 && blink_led2 != 46 && blink_led2 != 47 && blink_led2 != 60 && blink_led2 != 61 && blink_led2 != 62 && blink_led2 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(150);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4; blink_led2 += 4;

        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
          if (blink_led2 == russian_table[4][0])
            blink_led2 = russian_table[4][1];
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
        }
      }
      break;

    case 3:
      blink_led1 = column;
      if (column != 16)
        blink_led2 = blink_led1 + 3;
      else
        blink_led2 = blink_led1 - 9;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led1 != 44 && blink_led1 != 45 && blink_led1 != 46 && blink_led1 != 47 && blink_led1 != 60 && blink_led1 != 61 && blink_led1 != 62 && blink_led1 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
          }
          if (blink_led2 == russian_table[3][0])
            blink_led2 = russian_table[3][1];
        }

      }
      break;

    case 4:   //square
      blink_led1 = column;
      if (column != 3) {
        blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 4; blink_led4 = blink_led1 + 5;
      } else {
        blink_led2 = blink_led1 + 13; blink_led3 = blink_led1 + 4; blink_led4 = blink_led1 + 17;
      }

      for (i = 0; i < 7; i++) {
        if (russian[blink_led3 + 4] == 0 && russian[blink_led4 + 4] == 0 && blink_led3 != 44 && blink_led3 != 45 && blink_led3 != 46 && blink_led3 != 47 && blink_led3 != 60 && blink_led3 != 61 && blink_led3 != 62 && blink_led3 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3); trellis.setLED(blink_led4);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3); trellis.clrLED(blink_led4);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3); trellis.setLED(blink_led4);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1; russian[blink_led4] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;  blink_led4 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
            if (blink_led4 == russian_table[j][0])
              blink_led4 = russian_table[j][1];
          }
          if (blink_led2 == russian_table[4][0])
            blink_led2 = russian_table[4][1];
          if (blink_led4 == russian_table[4][0])
            blink_led4 = russian_table[4][1];

        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
            if (blink_led4 == russian_table[j][0])
              blink_led4 = russian_table[j][1];
          }
        }

      }

      break;

    case 5:
      blink_led1 = column;
      if (column != 3) {
        blink_led2 = blink_led1 + 4; blink_led3 = blink_led1 + 5;
      } else {
        blink_led2 = blink_led1 + 4; blink_led3 = blink_led1 + 17;
      }
      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led2 != 44 && blink_led2 != 45 && blink_led2 != 46 && blink_led2 != 47 && blink_led2 != 60 && blink_led2 != 61 && blink_led2 != 62 && blink_led2 != 63 ) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay();
        } else if (russian[blink_led2 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
          if (blink_led3 == russian_table[4][0])
            blink_led3 = russian_table[4][1];
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
        }
      }

      break;

    case 6:
      blink_led1 = column;
      if (column != 3) {
        blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 4;
      } else {
        blink_led2 = blink_led1 + 13; blink_led3 = blink_led1 + 4;
      }

      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 44 && blink_led3 != 45 && blink_led3 != 46 && blink_led3 != 47 && blink_led3 != 60 && blink_led3 != 61 && blink_led3 != 62 && blink_led3 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();  delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay();
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
          if (blink_led1 == russian_table[4][0])
            blink_led1 = russian_table[4][1];
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
        }
      }

      break;

    case 7:
      blink_led1 = column;
      if (column != 3) {
        blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 5;
      } else {
        blink_led2 = blink_led1 + 16; blink_led3 = blink_led1 + 20;
      }

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 44 && blink_led3 != 45 && blink_led3 != 46 && blink_led3 != 47 && blink_led3 != 60 && blink_led3 != 61 && blink_led3 != 62 && blink_led3 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay();
        } else if (blink_led3 == 45 || russian[blink_led1 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
          if (blink_led1 == russian_table[4][0])
            blink_led1 = russian_table[4][1];
          if (blink_led3 == russian_table[4][0])
            blink_led3 = russian_table[4][1];
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
        }
      }

      break;

    case 8:
      blink_led1 = column;
      if (column != 16) {
        blink_led2 = blink_led1 + 3; blink_led3 = blink_led1 + 4;
      } else {
        blink_led2 = blink_led1 - 9; blink_led3 = blink_led1 + 4;
      }
      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led2 != 44 && blink_led2 != 45 && blink_led2 != 46 && blink_led2 != 47 && blink_led2 != 60 && blink_led2 != 61 && blink_led2 != 62 && blink_led2 != 63) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(100);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay();
        } else if (blink_led3 == 45 || russian[blink_led2 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (column < 4) {
          for (j = 0; j < 4; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
        } else {
          for (j = 4; j < 8; j++) {
            if (blink_led1 == russian_table[j][0])
              blink_led1 = russian_table[j][1];
            if (blink_led2 == russian_table[j][0])
              blink_led2 = russian_table[j][1];
            if (blink_led3 == russian_table[j][0])
              blink_led3 = russian_table[j][1];
          }
          if (blink_led1 == russian_table[3][0])
            blink_led1 = russian_table[3][1];
        }

      }

      break;

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


