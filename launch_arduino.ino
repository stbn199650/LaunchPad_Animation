//This code will let you use any Arduino combined with 4 Adafruit Trellises to mimic the basic functions of a launchpad (Or just use it as a MIDI controller)
//For this code to work, you will need a few extra programs on the computer
//You will need to use a program called loopMIDI to create virtual MIDI channels
//And you also need a program called HairlessMIDISerial to convert Serial to Midi (You need to set the Baud Rate to 256000 in the options)
//You also need to install the Arduino MIDI library and the Adafruit Trellis library

#include <MIDI.h>
#include <Wire.h>
#include "Adafruit_Trellis.h"
MIDI_CREATE_DEFAULT_INSTANCE();
Adafruit_Trellis matrix0 = Adafruit_Trellis();  //create Adafruit_Trellis constructor
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

#define NUMTRELLIS 4
#define numKeys (NUMTRELLIS * 16) //64
#define INTPIN A2

#define pinLED 13
#define MIDI_NOTE_ON 144
#define MIDI_NOTE_OFF 128
#define STATE_NONE 0
#define STATE_NOTE_ON 1
#define STATE_NOTE_OFF 5

int state, i = 0, count = 0;
int russian[64];

byte midiByte, midiCommand, midiNote;
const byte interruptPin = 2;
int rightbutton = 0;
int show[8][8][8] = {{{0}}};

void setup() {

  pinMode(pinLED, OUTPUT);
  MIDI.begin();
  Serial.begin(256000);
  pinMode(INTPIN, INPUT);  // INT pin requires a pullup
  digitalWrite(INTPIN, HIGH);
  trellis.begin(0x70, 0x71, 0x72, 0x73);
  Wire.setClock(400000L);
  attachInterrupt(digitalPinToInterrupt(interruptPin), change_state, CHANGE); //if pin state change(button is pressed, goto interrupt)

  for (i = 0; i < 64; i++)
    russian[i] = 0;

  for (uint8_t i = 0; i < numKeys; i++) { //Turn on all of LEDs (For diagnostic purposes)
    trellis.setLED(i);     //divide 64 led into 4 4*4 matrix
    trellis.writeDisplay();
  }
  delay(1000);
  for (uint8_t i = 0; i < numKeys; i++) {   //Turn off all of LEDs (For diagnostic purposes)
    trellis.clrLED(i);
    trellis.writeDisplay();
    delay(30);
  }

}

void MIDIRead () {  // checks incoming MIDI messages and turns on corresponding LED's
over:
  if (Serial.available() > 0) { // is there something to be read?
    midiByte = Serial.read();     // read MIDI byte
    Serial.print("I received: ");
    Serial.println(midiByte, DEC);

    switch (state) {
      case STATE_NONE:  //0
        midiCommand = midiByte & B11110000;
        if (midiCommand == MIDI_NOTE_ON) {
          state = STATE_NOTE_ON;
        }
        if (midiCommand == MIDI_NOTE_OFF) {
          state = STATE_NOTE_OFF;
        }
        break;

      case STATE_NOTE_ON: //1
        midiNote = midiByte;
        state = STATE_NONE;
        if (midiNote < 36 || midiNote > 99) { //Checks if the note is outside of the range of the trellis (If it is it will ignore it, or else weird issues with getButton() ensues)
          goto over;
        }
        else {
          trellis.setLED(getButton(midiNote));
        }
        break;

      case STATE_NOTE_OFF:  //5
        midiNote = midiByte;
        state = STATE_NONE;
        if (midiNote < 36 || midiNote > 99) { //See above
          goto over;
        }
        else {
          trellis.clrLED(getButton(midiNote));
        }
        break;
    }
  }
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

int MidiOut[8][8] = { //Grid of MIDI notes
  {64, 65, 66, 67, 96, 97, 98, 99},
  {60, 61, 62, 63, 92, 93, 94, 95},
  {56, 57, 58, 59, 88, 89, 90, 91},
  {52, 53, 54, 55, 84, 85, 86, 87},
  {48, 49, 50, 51, 80, 81, 82, 83},
  {44, 45, 46, 47, 76, 77, 78, 79},
  {40, 41, 42, 43, 72, 73, 74, 75},
  {36, 37, 38, 39, 68, 69, 70, 71}
};

int getMIDI(int pv) { //Returns the MIDI note of the cooresponding button ID
  if (pv < 16) {
    int floored = floor(pv / 4);
    return MidiOut[floored][pv % 4];
  }
  else if (pv < 32) {
    pv = pv - 16;
    int floored = floor(pv / 4);
    return MidiOut[floored][(pv % 4) + 4];
  }
  else if (pv < 48) {
    pv = pv - 32;
    int floored = floor(pv / 4);
    return MidiOut[floored + 4][pv % 4];
  }
  else {
    pv = pv - 48;
    int floored = floor(pv / 4);
    return MidiOut[floored + 4][(pv % 4) + 4];
  }
}

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

int randnum = 0, pressed = 0, blink_led1, blink_led2, blink_led3, blink_led4;
void buttonCheck () {           // checks buttons and sends out a MIDI note cooresponding to that button if pressed
  if (trellis.readSwitches()) {
    for (uint8_t h = 0; h < numKeys; h++) {   //numKeys=64      
      if (trellis.justPressed(h)) { //press led h
        //MIDI.sendNoteOn(getMIDI(h), 127, 1);  //play music
        if(rightbutton==0){
          if(h==0
          randnum = random(9);//generate a number between 0~8
          generate(randnum);
        }
        
      }
      if (trellis.justReleased(h)) {
        //MIDI.sendNoteOff(getMIDI(h), 0, 1);
      }
    }
  }
}

void generate(int x) {

  switch (x) {
    case 0:
      blink_led1 = 0; blink_led2 = blink_led1 + 1;

      for (i = 0; i < 8; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 44) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay(); delay(200);
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4;
        if (blink_led1 == 16) {
          blink_led1 = 32;
        }
        blink_led2 = blink_led1 + 1;
      }

      break;

    case 1:
      blink_led1 = 0; blink_led2 = 4;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 45 ) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay(); delay(200);
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }
        blink_led1 += 4; blink_led2 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 16)
          blink_led2 = 32;

      }
      break;

    case 2:
      blink_led1 = 0; blink_led2 = blink_led1 + 5;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 45 ) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay(); delay(200);
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4; blink_led2 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 17)
          blink_led2 = 33;
      }
      break;

    case 3:
      blink_led1 = 1; blink_led2 = blink_led1 + 3;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led2 + 4] == 0 && blink_led2 != 44 ) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2);
          trellis.writeDisplay(); delay(200);
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;
        if (blink_led1 == 17)
          blink_led1 = 33;
        if (blink_led2 == 16)
          blink_led2 = 32;
      }
      break;

    case 4:
      blink_led1 = 0; blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 4; blink_led4 = blink_led1 + 5;
      for (i = 0; i < 7; i++) {
        if (russian[blink_led3 + 4] == 0 && russian[blink_led4 + 4] == 0 && blink_led4 != 45) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3); trellis.setLED(blink_led4);
          trellis.writeDisplay(); delay(300);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3); trellis.clrLED(blink_led4);
          trellis.writeDisplay(); delay(300);
        } else if (blink_led3 == 45 || russian[blink_led3 + 4] == 1 || russian[blink_led4 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3); trellis.setLED(blink_led4);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1; russian[blink_led4] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;  blink_led4 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 17)
          blink_led2 = 33;
        if (blink_led3 == 16)
          blink_led3 = 32;
        if (blink_led4 == 17)
          blink_led4 = 33;
      }

      break;

    case 5:
      blink_led1 = 0; blink_led2 = blink_led1 + 4; blink_led3 = blink_led1 + 5;
      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 45 ) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay(); delay(200);
        } else if (blink_led3 == 45 || russian[blink_led2 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 16)
          blink_led2 = 32;
        if (blink_led3 == 17)
          blink_led3 = 33;
      }

      break;

    case 6:
      blink_led1 = 0; blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 4;
      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 44) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();  delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay(); delay(200);
        } else {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();

          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 17)
          blink_led2 = 33;
        if (blink_led3 == 16)
          blink_led3 = 32;
      }

      break;

    case 7:
      blink_led1 = 0; blink_led2 = blink_led1 + 1; blink_led3 = blink_led1 + 5;

      for (i = 0; i < 7; i++) {
        if (russian[blink_led1 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 45) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay(); delay(200);
        } else if (blink_led3 == 45 || russian[blink_led1 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (blink_led1 == 16)
          blink_led1 = 32;
        if (blink_led2 == 17)
          blink_led2 = 33;
        if (blink_led3 == 17)
          blink_led3 = 33;
      }

      break;

    case 8:
      blink_led1 = 1; blink_led2 = blink_led1 + 3; blink_led3 = blink_led1 + 4;
      for (i = 0; i < 7; i++) {
        if (russian[blink_led2 + 4] == 0 && russian[blink_led3 + 4] == 0 && blink_led3 != 45) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay(); delay(200);
          trellis.clrLED(blink_led1); trellis.clrLED(blink_led2); trellis.clrLED(blink_led3);
          trellis.writeDisplay(); delay(200);
        } else if (blink_led3 == 45 || russian[blink_led2 + 4] == 1 || russian[blink_led3 + 4] == 1) {
          trellis.setLED(blink_led1); trellis.setLED(blink_led2); trellis.setLED(blink_led3);
          trellis.writeDisplay();
          russian[blink_led1] = 1; russian[blink_led2] = 1; russian[blink_led3] = 1;
          break;
        }

        blink_led1 += 4;  blink_led2 += 4;  blink_led3 += 4;
        if (blink_led1 == 17)
          blink_led1 = 33;
        if (blink_led2 == 16)
          blink_led2 = 32;
        if (blink_led3 == 17)
          blink_led3 = 33;
      }

      break;

  }
}

unsigned long timeForCheck = 0;
byte delayTime = 30; // Time between checks for button presses in milliseconds
void loop() {
  
  MIDIRead(); //busy waiting to check if button is pressed
  
  if (millis() > timeForCheck) {  //button has been pressed
    buttonCheck();
    trellis.writeDisplay();
    timeForCheck = millis() + delayTime;

  }
}

void change_state() {

  if (trellis.readSwitches()) { //check which button is pressed
    for (uint8_t h = 0; h < numKeys; h++) {
      if (h == 19 || h == 23 || h == 27 || h == 31 || h == 51 || h == 55 || h == 59 || h == 63) {
        if (trellis.justPressed(h)) { //press button h
          
        }
        
        if (trellis.justReleased(h)) {  //release led h
          if (h == 19)
            rightbutton = 0;
          else if (h == 23)
            rightbutton = 1;
          else if (h == 27)
            rightbutton = 2;
          else if (h == 31)
            rightbutton = 3;
          else if (h == 51)
            rightbutton = 4;
          else if (h == 55)
            rightbutton = 5;
          else if (h == 59)
            rightbutton = 6;
          else if (h == 63)
            rightbutton = 7;

        }
      }
    }
  }

}

