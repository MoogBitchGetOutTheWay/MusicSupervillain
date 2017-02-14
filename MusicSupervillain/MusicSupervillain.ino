#include <werkstatt.h>

const int pinsToCheck[] = {23, 24, 27, 28, 31, 32, 35, 36, 39, 40, 43, 44, 46, 49, 50};
const int numPinsToCheck = 15;

const int MAJOR = 0;
const int PENTATONIC = 1;
int currMapping = 0;
bool togglePressed = 0;
int currVCFVal = 0;

// Left hand
const int L_PINKY_1 = 40;
const int L_PINKY_2 = 43;
const int L_PINKY_3 = 44;

const int L_RING_1 = 35;
const int L_RING_2 = 36;
const int L_RING_3 = 39;

const int L_MID_1 = 28;
const int L_MID_2 = 31;
const int L_MID_3 = 32;

const int L_INDEX_1 = 23;
const int L_INDEX_2 = 24;
const int L_INDEX_3 = 27;

// Right hand
const int R_PINKY_1 = 0;
const int R_PINKY_2 = 0;
const int R_PINKY_3 = 0;

const int R_RING_1 = 0;
const int R_RING_2 = 0;
const int R_RING_3 = 0;

const int R_MID_1 = 0;
const int R_MID_2 = 0;
const int R_MID_3 = 0;

const int R_INDEX_1 = 46;
const int R_INDEX_2 = 49;
const int R_INDEX_3 = 50;

int MAPPINGS[53];



const int VCO_PIN = 6;
const int VCA_PIN = 7;
const int GATE_PIN = 8;
const int VCF_PIN = 5;

int buttonState = 0;

void setup() {
  setupMappings(currMapping);
  
  for (int i=0; i<sizeof(pinsToCheck); i++) {
    int currPin = pinsToCheck[i];
    pinMode(currPin, INPUT);
  }

  pinMode(VCO_PIN, OUTPUT);
  pinMode(VCA_PIN, OUTPUT);
  
  Serial.begin(74880);
  Serial.println("begin");
}

void loop() {
  /*analogWrite(VCO_PIN, tonic);
  digitalWrite(VCA_PIN, HIGH);
  delay(5000);
  digitalWrite(VCA_PIN, LOW);
  delay(100);
  analogWrite(VCO_PIN, octave);
  digitalWrite(VCA_PIN, HIGH);
  delay(5000);
  digitalWrite(VCA_PIN, LOW);
  delay(100);*/
  
  int max = 0;
  int maxPin = pinsToCheck[0];
  bool maxPinChanged = 0;
  bool toggleStillPressed = 0;
  for (int i=0; i<numPinsToCheck; i++) {
    int currPin = pinsToCheck[i];
    buttonState = digitalRead(currPin);
    if (buttonState == HIGH) {
      if (MAPPINGS[currPin] <= 255 && MAPPINGS[currPin] >= max) {
        max = MAPPINGS[currPin];
        maxPin = currPin;
        maxPinChanged = 1;
      } else if (MAPPINGS[currPin] == 300) {
        toggleStillPressed = 1;
        if (!togglePressed) {
          Serial.println("toggle scale");
          if (currMapping == MAJOR) {
            currMapping = PENTATONIC;
          } else if (currMapping == PENTATONIC) {
            currMapping = MAJOR;
          }
          setupMappings(currMapping);
          togglePressed = 1;
        }
      } else if (MAPPINGS[currPin] == 301) {
        Serial.println("increase cutoff");
        currVCFVal += 25;
        if (currVCFVal > 255) {
          currVCFVal = 255;
        }
        Serial.println(currVCFVal);
      } else if (MAPPINGS[currPin] == 302) {
        Serial.println("decrease cutoff");
        currVCFVal -= 25;
        if (currVCFVal < 0) {
          currVCFVal = 0;
        }
        Serial.println(currVCFVal);
      }
      //Serial.println(currPin);
      //analogWrite(VCO_PIN, MAPPINGS[currPin]);
    }
    delay(10);
  }
  if (!toggleStillPressed) {
    togglePressed = 0;
  }
  
  if (maxPinChanged) {
    analogWrite(VCO_PIN, MAPPINGS[maxPin]);
    digitalWrite(VCA_PIN, HIGH);
  } else {
    digitalWrite(VCA_PIN, LOW);
  }
  analogWrite(VCF_PIN, currVCFVal);
  
  //analogWrite(VCO_PIN, 0);
}

void setupMappings(int scale_sel) {
  int scale[12];
  int majorScale[] = {0, 10, 20, 25, 35, 45, 55, 60, 70, 80, 85, 95};
  int pentatonicScale[] = {0, 10, 20, 35, 45, 60, 70, 80, 95, 105, 120, 130};

  if (scale_sel == MAJOR) {
    for (int i=0; i<12; i++) {
      scale[i] = majorScale[i];
    }
  } else if (scale_sel == PENTATONIC) {
    for (int i=0; i<12; i++) {
      scale[i] = pentatonicScale[i];
    }
  }

  
  MAPPINGS[L_PINKY_1] = scale[9];
  MAPPINGS[L_PINKY_2] = scale[10];
  MAPPINGS[L_PINKY_3] = scale[11];

  MAPPINGS[L_RING_1] = scale[6];
  MAPPINGS[L_RING_2] = scale[7];
  MAPPINGS[L_RING_3] = scale[8];
  
  MAPPINGS[L_MID_1] = scale[3];
  MAPPINGS[L_MID_2] = scale[4];
  MAPPINGS[L_MID_3] = scale[5];
  
  MAPPINGS[L_INDEX_1] = scale[0];
  MAPPINGS[L_INDEX_2] = scale[1];
  MAPPINGS[L_INDEX_3] = scale[2];

  // Right hand
  MAPPINGS[R_PINKY_1] = tonic;
  MAPPINGS[R_PINKY_2] = tonic;
  MAPPINGS[R_PINKY_3] = tonic;
  
  MAPPINGS[R_RING_1] = tonic;
  MAPPINGS[R_RING_2] = tonic;
  MAPPINGS[R_RING_3] = tonic;
  
  MAPPINGS[R_MID_1] = tonic;
  MAPPINGS[R_MID_2] = tonic;
  MAPPINGS[R_MID_3] = tonic;
  
  MAPPINGS[R_INDEX_1] = 300;
  MAPPINGS[R_INDEX_2] = 301;
  MAPPINGS[R_INDEX_3] = 302;
}

void nextStepEq() {
  int attack = 50;
  int gate = digitalRead(GATE_PIN);
  if (gate == HIGH) {
    // fade in from 0 to 255 in steps of 5 points:
    for(int egValue = 0 ; egValue <= 255; egValue +=5) {
      // sets the value (range from 0 to 255):
      analogWrite(VCA_PIN, egValue);
      
      // wait for time in milliseconds to see the dimming effect
      delay(attack);
    }
  }
  else {
    /*if (egValue > 0) {
      // fade out from 0 to 255 in steps of 5 points:
      for(int egValue = 255 ; egValue >= 0; egValue -=5) {
        
        // sets the value (range from 0 to 255):
        analogWrite(VCA_PIN, egValue);
        
        // wait for time in milliseconds to see the dimming effect
        delay(decay);
      }
    } else {
      analogWrite(VCA_PIN, 0);
    }*/
  }
}

