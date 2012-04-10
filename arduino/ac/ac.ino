#include "LPD8806.h"
#include <SPI.h>

//#define DEBUG

#define N_LINES 7
#define LED_COUNT 72
#define MAX_TRAIL 6

byte LED_LINES[N_LINES+1] = { 4, 16, 4, 24, 4, 16, 4, 0 };
byte LINE_IDX[LED_COUNT+MAX_TRAIL];
byte LINE_FIRST_LED[N_LINES+1];
byte LINE_LAST_LED[N_LINES+1];

// colours

#define N_FADE 32
int FADE[N_FADE+1] = { 0, 5, 7, 11, 13, 14, 20, 22, 25, 28, 31, 34, 37, 40, 43, 46, 50, 54, 58, 62, 67, 72, 77, 82, 87, 92, 97, 102, 107, 112, 117, 127 };

#define N_COLOURS 8
boolean COLOUR_RED[N_COLOURS]   = { 1, 1, 0, 0, 0, 1, 1, 0 };
boolean COLOUR_GREEN[N_COLOURS] = { 1, 0, 1, 0, 1, 0, 1, 0 };
boolean COLOUR_BLUE[N_COLOURS]  = { 1, 0, 0, 1, 1, 1, 0, 0 };

#define COLOUR_WHITE_IDX 0
#define COLOUR_RED_IDX 1
#define COLOUR_GREEN_IDX 2
#define COLOUR_BLUE_IDX 3
#define COLOUR_CYAN_IDX 4
#define COLOUR_MAGENTA_IDX 5
#define COLOUR_YELLOW_IDX 6
#define COLOUR_BLACK_IDX 7
#define COLOUR_LAST 7

int lr[LED_COUNT+MAX_TRAIL];
int lg[LED_COUNT+MAX_TRAIL];
int lb[LED_COUNT+MAX_TRAIL];

int lrl[N_LINES];
int lgl[N_LINES];
int lbl[N_LINES];

byte chaserColour = 0;
byte pulseColour = 0;

// chasers

#define N_CHASERS 40
#define CHASER_BRIGHTEN 125

#define CHASER_MODE_NONE 0
#define CHASER_MODE_MADNESS 1
#define CHASER_MODE_COLUMNS_UP 2
#define CHASER_MODE_COLUMNS_DOWN 3
#define CHASER_MODE_COLUMNS_UP_DOWN 4
#define CHASER_MODE_TOP 5
#define CHASER_MODE_LAST 5

int chasersLed[N_CHASERS];
byte chasersLine[N_CHASERS];
byte chasersDirection[N_CHASERS];
byte chasersTrail[N_CHASERS];
byte chasersSpeed[N_CHASERS];
byte chasersRemaining[N_CHASERS];
byte chasersColour[N_CHASERS];

int chaserMode = CHASER_MODE_NONE;
int chaserIndex = 0;

// pulses variables

#define N_PULSES 8
#define PULSES_BRIGHTEN 125

#define PULSE_FAST 1
#define PULSE_MED 5
#define PULSE_SLOW 25
#define PULSE_SLOWEST 50

#define PULSE_MODE_NONE 0
#define PULSE_MODE_POP 1
#define PULSE_MODE_FADE 2
#define PULSE_MODE_INDIVIDUAL 3
#define PULSE_MODE_TOP_BOTTOM 4
#define PULSE_MODE_SIDES 5
#define PULSE_MODE_BOTTOM 6
#define PULSE_MODE_TOP 7
#define PULSE_MODE_SLOW_FADE 8
#define PULSE_MODE_SLOW_POP 9
#define PULSE_MODE_FADE_DROP_FOUR 10
#define PULSE_MODE_SLOW_POP_DROP_FOUR 11
#define PULSE_MODE_LAST 11

byte pulsesLine[N_PULSES];
byte pulsesCount[N_PULSES];
byte pulsesFadeIn[N_PULSES];
byte pulsesFadeOut[N_PULSES];
byte pulsesColour[N_PULSES];

int pulseMode = PULSE_MODE_NONE;
int pulseIndex = 0;

// MIDI defines

#define MIDI_CHANNEL 0
#define MIDI_POSITION (0xF2)
#define MIDI_SONG_SELECT (0xF3)
#define MIDI_START (0xFA)
#define MIDI_STOP (0xFC)
#define MIDI_CLOCK (0xF8)
#define MIDI_CONTINUE (0xFB)
#define MIDI_NOTE_OFF (0x80|MIDI_CHANNEL)
#define MIDI_NOTE_ON (0x90|MIDI_CHANNEL)
#define MIDI_AFTERTOUCH (0xA0|MIDI_CHANNEL)
#define MIDI_CONT_CHANGE (0xB0|MIDI_CHANNEL)
#define MIDI_PATCH_CHANGE (0xC0|MIDI_CHANNEL)
#define MIDI_CHANNEL_PRESSURE (0xD0|MIDI_CHANNEL)
#define MIDI_PITCH_BEND (0xE0|MIDI_CHANNEL)
#define MIDI_COMMAND (0xF0|MIDI_CHANNEL)

#define MIDI_C-3 0
#define MIDI_C-3_SHARP 1
#define MIDI_D-3 2
#define MIDI_D-3_SHARP 3
#define MIDI_E-3 4
#define MIDI_F-3 5
#define MIDI_F-3_SHARP 6
#define MIDI_G-3 7
#define MIDI_G-3_SHARP 8
#define MIDI_A-3 9
#define MIDI_A-3_SHARP 10
#define MIDI_B-3 11
#define MIDI_C-2 12
#define MIDI_C-2_SHARP 13
#define MIDI_D-2 14
#define MIDI_D-2_SHARP 15
#define MIDI_E-2 16
#define MIDI_F-2 17
#define MIDI_F-2_SHARP 18
#define MIDI_G-2 19
#define MIDI_G-2_SHARP 20
#define MIDI_A-2 21
#define MIDI_A-2_SHARP 22
#define MIDI_B-2 23
#define MIDI_C-1 24
#define MIDI_C-1_SHARP 25
#define MIDI_D-1 26
#define MIDI_D-1_SHARP 27
#define MIDI_E-1 28
#define MIDI_F-1 29
#define MIDI_F-1_SHARP 30
#define MIDI_G-1 31
#define MIDI_G-1_SHARP 32
#define MIDI_A-1 33
#define MIDI_A-1_SHARP 34
#define MIDI_B-1 35
#define MIDI_C0 36
#define MIDI_C0_SHARP 37
#define MIDI_D0 38
#define MIDI_D0_SHARP 39
#define MIDI_E0 40
#define MIDI_F0 41
#define MIDI_F0_SHARP 42
#define MIDI_G0 43
#define MIDI_G0_SHARP 44
#define MIDI_A0 45
#define MIDI_A0_SHARP 46
#define MIDI_B0 47
#define MIDI_C1 48
#define MIDI_C1_SHARP 49
#define MIDI_D1 50
#define MIDI_D1_SHARP 51
#define MIDI_E1 52
#define MIDI_F1 53
#define MIDI_F1_SHARP 54
#define MIDI_G1 55
#define MIDI_G1_SHARP 56
#define MIDI_A1 57
#define MIDI_A1_SHARP 58
#define MIDI_B1 59
#define MIDI_C2 60
#define MIDI_C2_SHARP 61
#define MIDI_D2 62
#define MIDI_D2_SHARP 63
#define MIDI_E2 64
#define MIDI_F2 65
#define MIDI_F2_SHARP 66
#define MIDI_G2 67
#define MIDI_G2_SHARP 68
#define MIDI_A2 69
#define MIDI_A2_SHARP 70
#define MIDI_B2 71
#define MIDI_C3 72
#define MIDI_C3_SHARP 73
#define MIDI_D3 74
#define MIDI_D3_SHARP 75
#define MIDI_E3 76
#define MIDI_F3 77
#define MIDI_F3_SHARP 78
#define MIDI_G3 79
#define MIDI_G3_SHARP 80
#define MIDI_A3 81
#define MIDI_A3_SHARP 82
#define MIDI_B3 83
#define MIDI_C4 84
#define MIDI_C4_SHARP 85
#define MIDI_D4 86
#define MIDI_D4_SHARP 87
#define MIDI_E4 88
#define MIDI_F4 89
#define MIDI_F4_SHARP 90
#define MIDI_G4 91
#define MIDI_G4_SHARP 92
#define MIDI_A4 93
#define MIDI_A4_SHARP 94
#define MIDI_B4 95
#define MIDI_C5 96
#define MIDI_C5_SHARP 97
#define MIDI_D5 98
#define MIDI_D5_SHARP 99
#define MIDI_E5 100
#define MIDI_F5 101
#define MIDI_F5_SHARP 102
#define MIDI_G5 103
#define MIDI_G5_SHARP 104
#define MIDI_A5 105
#define MIDI_A5_SHARP 106
#define MIDI_B5 107
#define MIDI_C6 108
#define MIDI_C6_SHARP 109
#define MIDI_D6 110
#define MIDI_D6_SHARP 111
#define MIDI_E6 112
#define MIDI_F6 113
#define MIDI_F6_SHARP 114
#define MIDI_G6 115
#define MIDI_G6_SHARP 116
#define MIDI_A6 117
#define MIDI_A6_SHARP 118
#define MIDI_B6 119

int midiState = 0;
int midiCount = 0;
int midiMeasure = 0;

byte b;
int note;
int velocity;

int lastNote = -1;

// MNDR tracks

#define MNDR_DEFAULT_TRACK -1
#define MNDR_BURNING_HEARTS 69
#define MNDR_CALIGULA 71
#define MNDR_CLUB 72
#define MNDR_CALIGULA_CLUB_TRANSITION 86
#define MNDR_WE_CAN_FALL 66
#define MNDR_I_GO_AWAY 73
#define MNDR_STAY 82
#define MNDR_CUT_ME_OUT 78
#define MNDR_SPARROW 76
#define MNDR_NUMBER_ONE_IN_HEAVEN 79
#define MNDR_FASTER_HORSES 77
#define MNDR_DIAMONDS 74
#define MNDR_I_KNOW_WHAT_I_WROTE 81

int track = MNDR_STAY;
int lastTrack = -2;

// RELAYS

#define RELAY0_PIN 2
#define RELAY1_PIN 2

int relay0State = false;
int relay1State = false;

// LEDs

LPD8806 strip = LPD8806(LED_COUNT);

void setup() {
  // Initialze MIDI
  
  Serial.begin(31250);
  
  // Initialize variables
  
  clean();
  
  // Construct LED index to line index map
  
  int lineIdx = 0;
  int lineIdxNext = LED_LINES[0];
  
  for (int i = 0; i < MAX_TRAIL; i++) {
    LINE_IDX[i] = 0xFF;
  }
  
  for (int i = 0; i < LED_COUNT; i++) {
    if (i >= lineIdxNext) {
      lineIdxNext += LED_LINES[++lineIdx];
    }
    LINE_IDX[i+MAX_TRAIL] = lineIdx;
  }
  
  // Construct line first and last LED index map
  
  int ledIdx = 0;
  
  for (int i = 0; i < N_LINES+1; i++) {
    LINE_FIRST_LED[i] = ledIdx;
    if (i < N_LINES) {
      ledIdx += LED_LINES[i];
    }
    LINE_LAST_LED[i] = ledIdx-1;
  }
  
  // Initialize strip
  
  strip.begin();
  
  for (int i = 0; i < LED_COUNT; i++) {
    strip.setPixelColor(i, 0);
  }
  
  strip.show();
  
  delay(200);
  
  randomSeed(1);
  
#ifdef DEBUG
  strip.setPixelColor(32, strip.Color(255, 255, 0));
#endif

  // RELAY TIME
  
  pinMode(2, OUTPUT);
  
  addChaser(0, 1, 1, 12, 0);
}

void loop() {
//#ifdef DEBUG
//everythingUpTo(20);
//#endif
//  lr[8] = 127;
//  updateStrip();
//  strip.setPixelColor(7, 32, 32, 0);
//  strip.show();
  
  /************** MIDI **************/
  if (Serial.available() > 0) {
    b = Serial.read();
    
    switch (midiState) {
    case 0:
      switch (b) {
      case MIDI_START:
#ifdef DEBUG
//        strip.setPixelColor(0, strip.Color(255, 255, 255));
#endif
        break;
        
      case MIDI_CONTINUE:
#ifdef DEBUG
//        strip.setPixelColor(1, strip.Color(255, 255, 0));
#endif
        break;
        
      case MIDI_STOP:
#ifdef DEBUG
//        strip.setPixelColor(2, strip.Color(255, 0, 255));
#endif
        // Turn everything off
        for (int i = 0; i < LED_COUNT; i++) {
          strip.setPixelColor(i, 0);
        }
        strip.show();
        clean();
        break;
        
      case MIDI_CLOCK:
        if (midiCount == 0) {
#ifdef DEBUG
//          strip.setPixelColor(3, strip.Color(0, 255, 255));
#else
//          brighten(0, 0, 127);
#endif
          callPulse();
          callChaser();
        } else if (midiCount == 4) {
#ifdef DEBUG
//          strip.setPixelColor(3, strip.Color(0, 0, 0));
#else
//          brighten(0, 0, -127);
#endif
        }

        for (int i = 0; i < N_CHASERS; i++) {
          if (chasersLed[i] != -1) {
            chaserTick(i);
          }
        }
        
        for (int i = 0; i < N_PULSES; i++) {
          if (pulsesLine[i] != 0xFF) {
            pulseTick(i);
          }
        }
        
        if (++midiCount >= 24) {
          midiCount = 0;
          midiMeasure++;
        }
        
        updateStrip();
        
        break;
        
      case MIDI_NOTE_ON:
#ifdef DEBUG
//        strip.setPixelColor(6, strip.Color(255, 0, 0));
#endif
        midiState = 1;
        break;
        
      case MIDI_NOTE_OFF:
#ifdef DEBUG
//        strip.setPixelColor(6, strip.Color(0, 255, 0));
#endif DEBUG
        midiState = 3;
        break;
        
      case MIDI_AFTERTOUCH:
        midiState = -1; // nop
        break;
        
      case MIDI_CONT_CHANGE:
        midiState = -2; // nop
        break;
        
      case MIDI_PATCH_CHANGE:
        midiState = 5;
        break;
        
      case MIDI_CHANNEL_PRESSURE:
        midiState = -1; // nop
        break;
        
      case MIDI_PITCH_BEND:
        midiState = -2; // nop
        break;
      
      case MIDI_POSITION:
        midiState = -2; // nop
        break;
      
      case MIDI_SONG_SELECT:
        midiState = -1; // nop
        break;
      
#ifdef DEBUG
//      default:
//        strip.setPixelColor(6, 0, 0, 64);
#endif
      }
      
      break;
    
    case -2: // nop
      midiState = -1;
      break;
    
    case -1: // nop
      midiState = 0;
      break;
    
    case 1:
      note = b;
      midiState = 2;
      break;
      
    case 2:
      velocity = b;
      midiState = 0;
      noteOn(note, velocity);
      break;
    
    case 3:
      note = b;
      midiState = 4;
      break;
      
    case 4:
      velocity = b;
      midiState = 0;
      noteOff(note, velocity);
      break;
      
    case 5:
      programChange(b);
      midiState = 0;
      break;
    }
  }
  
#ifdef DEBUG
//  strip.setPixelColor(1, 255, 0, 0);
  strip.show();
#endif
}

void updateStrip() {
  for (int i = 0; i < LED_COUNT; i++) {
    int line = getLineNum(i);
    int redIdx = clamp(lr[i+MAX_TRAIL] + lrl[line], 0, 127) >> 2;
    int greenIdx = clamp(lg[i+MAX_TRAIL] + lgl[line], 0, 127) >> 2;
    int blueIdx = clamp(lb[i+MAX_TRAIL] + lbl[line], 0, 127) >> 2;
            
#ifdef DEBUG
if (i == 5+MAX_TRAIL) {
everythingUpTo(lg[i+MAX_TRAIL] + lgl[line]);
//everythingUpTo(clamp(lr[i]+lrl[getLineNum(i)],0,127)>>2);
//everythingUpTo(FADE[clamp(lr[i]+lrl[getLineNum(i)], 0, 127) >> 2]);
//everythingUpTo(FADE[31]);
}
#endif

#ifndef DEBUG
    strip.setPixelColor(i, FADE[redIdx], FADE[greenIdx], FADE[blueIdx]);
#endif
  }
        
#ifndef DEBUG
  strip.show();
#endif
}

void clean() {
  for (int i = 0; i < N_CHASERS; i++) {
    chasersLed[i] = -1;
  }
  
  for (int i = 0; i < N_PULSES; i++) {
    pulsesLine[i] = 0xFF;
  }
  
  for (int i = 0; i < LED_COUNT+MAX_TRAIL; i++) {
    lr[i] = lg[i] = lb[i] = 0;
  }
  
  for (int i = 0; i < N_LINES; i++) {
    lrl[i] = lgl[i] = lbl[i] = 0;
  }
}

void reset() {
  chaserMode = chaserIndex = pulseMode = pulseIndex = chaserColour = pulseColour = 0;
}

void resetMidi() {
  midiCount = -1;
  midiMeasure = 0;
}

int getLineNum(int x) {
  return LINE_IDX[x+MAX_TRAIL];
}

// TODO: to define
int clamp(int x, int minimum, int maximum) {
  return min(max(x, minimum), maximum);
}

void brighten(int idx, int colour, int v) {
  if (COLOUR_RED[colour]) {
    lr[idx+MAX_TRAIL] += v;
  }
  
  if (COLOUR_GREEN[colour]) {
    lg[idx+MAX_TRAIL] += v;
  }
  
  if (COLOUR_BLUE[colour]) {
    lb[idx+MAX_TRAIL] += v;
  }
}

void brightenLine(int idx, int colour, int v) {
  if (COLOUR_RED[colour]) {
    lrl[idx] += v;
  }
  
  if (COLOUR_GREEN[colour]) {
    lgl[idx] += v;
  }
  
  if (COLOUR_BLUE[colour]) {
    lbl[idx] += v;
  }
}

void brightenAll(int colour, int v) {
  for (int i = 0; i < N_LINES; i++) {
    if (COLOUR_RED[colour]) {
      lrl[i] += v;
    }
    
    if (COLOUR_GREEN[colour]) {
      lgl[i] += v;
    }
    
    if (COLOUR_BLUE[colour]) {
      lbl[i] += v;
    }
  }
}

/************** MIDI FUNCTIONS **************/

boolean manualChaserToggle = false;

void noteOn(int note, int velocity) {
  switch (note) {
  case MIDI_C3:
    pulseAll(PULSE_FAST, PULSE_MED, 0);
    return;
    
  case MIDI_C3_SHARP:
    toggleRelay(0);
    return;
    
//  case 1:
//    chasersVert(1, 1, 6, 0); // up
//    break;
//    
//  case 2:
//    chasersVert(1, 0, 6, 0); // down
//    break;
//    
//  case 3:
//    if (pulseMode++ == PULSE_MODE_LAST) pulseMode = 0;
//    clean();
//    break;
//
//  case 4:
//    if (chaserMode++ == CHASER_MODE_LAST) chaserMode = 0;
//    clean();
//    break;
//    
//  case 5:
//    if (chaserColour++ == COLOUR_LAST) chaserColour = 0;
//    clean();
//    break;
//    
//  case 6:
//    if (pulseColour++ == COLOUR_LAST) pulseColour = 0;
//    clean();
//    break;
  }

  switch (track) {
  case MNDR_BURNING_HEARTS:
    switch (note) {
    case MIDI_C0_SHARP:
      clean();
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
    case MIDI_D0_SHARP:
    case MIDI_F0_SHARP:
    case MIDI_G0_SHARP:
      clean();
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
    case MIDI_E0:
      clean();
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
    case MIDI_F0:
      clean();
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
    }
    break;
  
  case MNDR_CALIGULA:
    switch (note) {
    case MIDI_C0:
    case MIDI_C0_SHARP:
      clean();
      setSolidColour(COLOUR_CYAN_IDX, 127);
      break;
    
    case MIDI_D0:
      clean();
      setSolidTopColour(COLOUR_CYAN_IDX, 127);
//      pulseColour = COLOUR_CYAN_IDX;
//      pulseMode = PULSE_MODE_TOP;
      break;
      
    case MIDI_D0_SHARP:
      clean();
      setSolidBottomColour(COLOUR_CYAN_IDX, 127);
//      pulseColour = COLOUR_CYAN_IDX;
//      pulseMode = PULSE_MODE_BOTTOM;
      break;
      
    case MIDI_E0:
    case MIDI_F0:
      clean();
      setSolidColour(COLOUR_CYAN_IDX, 127);
//      pulseColour = COLOUR_CYAN_IDX;
//      pulseMode = PULSE_MODE_TOP_BOTTOM;
      break;
      
    case MIDI_F0_SHARP:
      clean();
      setSolidBottomColour(COLOUR_CYAN_IDX, 127);
//      pulseColour = COLOUR_CYAN_IDX;
//      pulseMode = PULSE_MODE_BOTTOM;
      break;
    
    case MIDI_G0:
      clean();
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
    }
    break;
    
  case MNDR_CALIGULA_CLUB_TRANSITION:
    clean();
    pulseColour = COLOUR_CYAN_IDX;
    pulseMode = PULSE_MODE_BOTTOM;
    chaserColour = COLOUR_RED_IDX;
    for (int i = 0; i < N_LINES; i++) {
      addLineChaser(i, 5, manualChaserToggle ? 0 : 1, 1, 0);
    }
    manualChaserToggle = !manualChaserToggle;
    
    break;
    
  case MNDR_CLUB:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_TOP;
      break;
      
    case MIDI_C0_SHARP:
//      pulseColour = COLOUR_RED_IDX;
//      pulseMode = PULSE_MODE_BOTTOM;
      setSolidBottomColour(COLOUR_RED_IDX, 127);
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_TOP;
      break;
    
    case MIDI_D0:
      setSolidBottomColour(COLOUR_RED_IDX, 127);
      break;
      
    case MIDI_D0_SHARP:
      setSolidBottomColour(COLOUR_RED_IDX, 127);
      pulseColour = COLOUR_RED_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_E0:
      break;
      
    case MIDI_F0:
    case MIDI_G0:
    case MIDI_G0_SHARP:
      pulseColour = COLOUR_RED_IDX;
      pulseMode = PULSE_MODE_FADE;
      break;
      
    case MIDI_F0_SHARP:
      break;
    }
    break;
    
  case MNDR_WE_CAN_FALL:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_BLUE_IDX, 32);
//      setSolidTopColour(COLOUR_BLUE_IDX, 127);
      chaserColour = COLOUR_BLUE_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP;
      break;
      
    case MIDI_G0:
      setSolidColour(COLOUR_BLUE_IDX, 32);
      break;
      
    case MIDI_G0_SHARP:
      break;
    }
    break;
    
  case MNDR_STAY:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      break;
      
    case MIDI_D0:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_F0:
      chaserColour = COLOUR_BLUE_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      chaserColour = COLOUR_BLUE_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      pulseColour = COLOUR_BLUE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
      
    case MIDI_G0:
      break;
    }
    break;
    
  case MNDR_CUT_ME_OUT:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SIDES;
      chaserColour = COLOUR_GREEN_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      chaserColour = COLOUR_GREEN_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
      
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      chaserColour = COLOUR_GREEN_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_TOP_BOTTOM;
      chaserColour = COLOUR_GREEN_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_GREEN_IDX, 20);
      break;
    }
    break;
    
  case MNDR_SPARROW:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      chaserColour = COLOUR_WHITE_IDX;
      chaserMode = CHASER_MODE_TOP;
      break;
      
    case MIDI_C0_SHARP:
      pulseColour = COLOUR_RED_IDX;
      pulseMode = PULSE_MODE_FADE;
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
      
    case MIDI_D0_SHARP:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_E0:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP;
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_F0:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      break;
      
    case MIDI_F0_SHARP:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
      
    case MIDI_G0:
      chaserColour = COLOUR_RED_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
      
    case MIDI_G0_SHARP:
      chaserColour = COLOUR_WHITE_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      pulseColour = COLOUR_RED_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_A0_SHARP:
      break;
    }
    break;
    
  case MNDR_NUMBER_ONE_IN_HEAVEN:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C1:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      break;
      
    case MIDI_C1_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_FADE;
      break;
    
    case MIDI_D1:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SIDES;
      break;
      
    case MIDI_D1_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SIDES;
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_TOP;
      break;
      
    case MIDI_E1:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
      
    case MIDI_F1:
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_FADE;
      break;
      
    case MIDI_F1_SHARP:
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_G1:
      setSolidColour(COLOUR_WHITE_IDX, 127);
      break;
      
    case MIDI_G1_SHARP:
      chaserColour = COLOUR_WHITE_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_A1:
      chaserColour = COLOUR_WHITE_IDX;
      chaserMode = CHASER_MODE_COLUMNS_DOWN;
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_TOP;
      break;
      
    case MIDI_A1_SHARP:
      chaserColour = COLOUR_WHITE_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      pulseColour = COLOUR_RED_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_B1:
      setSolidColour(COLOUR_CYAN_IDX, 20);
      break;
    }
    break;
    
  case MNDR_FASTER_HORSES:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      setSolidColour(COLOUR_WHITE_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SIDES;
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 20);
//      pulseColour = COLOUR_WHITE_IDX;
//      pulseMode = PULSE_MODE_FADE;
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_WHITE_IDX, 20);
//      pulseColour = COLOUR_WHITE_IDX;
//      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 20);
//      pulseColour = COLOUR_WHITE_IDX;
//      pulseMode = PULSE_MODE_FADE;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_WHITE_IDX, 20);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SIDES;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_WHITE_IDX, 20);
//      pulseColour = COLOUR_WHITE_IDX;
//      pulseMode = PULSE_MODE_FADE;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 20);
      break;
      
    case MIDI_G0:
      break;
    }
    break;
    
  case MNDR_DIAMONDS:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      pulseColour = COLOUR_MAGENTA_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      pulseColour = COLOUR_MAGENTA_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
    
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      pulseColour = COLOUR_MAGENTA_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      chaserColour = COLOUR_MAGENTA_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP_DOWN;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      pulseColour = COLOUR_MAGENTA_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      chaserColour = COLOUR_MAGENTA_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_MAGENTA_IDX, 60);
      pulseColour = COLOUR_MAGENTA_IDX;
      pulseMode = PULSE_MODE_SLOW_FADE;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 20);
      break;
      
    case MIDI_G0:
      break;
    }
    break;
    
  case MNDR_I_KNOW_WHAT_I_WROTE:
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_SLOW_POP;
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_FADE_DROP_FOUR;
      break;
    
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_COLUMNS_DOWN;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP_DROP_FOUR;
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_WHITE_IDX, 30);
      pulseColour = COLOUR_WHITE_IDX;
      pulseMode = PULSE_MODE_SLOW_POP_DROP_FOUR;
      break;
      
    case MIDI_F0_SHARP:
      break;
    }
    break;
    
  case MNDR_DEFAULT_TRACK: // THE DEFAULT TRACK
    reset();
    clean();
    
    switch (note) {
    case MIDI_C0:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      break;
      
    case MIDI_C0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      break;
    
    case MIDI_D0:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_FADE;
      break;
      
    case MIDI_D0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_POP;
      break;
      
    case MIDI_E0:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_TOP;
      break;
      
    case MIDI_F0:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_POP;
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_COLUMNS_UP;
      break;
      
    case MIDI_F0_SHARP:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      break;
      
    case MIDI_G0:
      setSolidColour(COLOUR_CYAN_IDX, 32);
      pulseColour = COLOUR_CYAN_IDX;
      pulseMode = PULSE_MODE_FADE;
      chaserColour = COLOUR_CYAN_IDX;
      chaserMode = CHASER_MODE_MADNESS;
      break;
    }
  }
  
  lastNote = note;
  lastTrack = track;
}

void noteOff(int note, int velocity) {
//  switch (note) {
//  case MIDI_C3_SHARP:
//    relay(0, 0);
//    break;
//  }
}

void toggleRelay(int idx) {
  switch (idx) {
  case 0:
    relay0State = !relay0State;
    digitalWrite(RELAY0_PIN, relay0State);
    break;
    
  case 1:
    relay1State = !relay1State;
    digitalWrite(RELAY1_PIN, relay1State);
    break;
  }
}

void programChange(int value) {
  clean();
  reset();
  resetMidi();
  
  track = value;
  
  switch (value) {
  case MNDR_BURNING_HEARTS:
    pulseColour = COLOUR_WHITE_IDX;
    break;
    
  case MNDR_CALIGULA:
  case MNDR_CLUB:
  case MNDR_CALIGULA_CLUB_TRANSITION:
  case MNDR_WE_CAN_FALL:
  case MNDR_STAY:
  case MNDR_CUT_ME_OUT:
  case MNDR_SPARROW:
  case MNDR_NUMBER_ONE_IN_HEAVEN:
  case MNDR_FASTER_HORSES:
  case MNDR_I_KNOW_WHAT_I_WROTE:
  case MNDR_DIAMONDS:
    break;
    
  case MNDR_I_GO_AWAY:
    setSolidColour(COLOUR_MAGENTA_IDX, 40);
    break;
  
  default:
    track = MNDR_DEFAULT_TRACK;
    break;
  }
}

/************** CHASER FUNCTIONS **************/

void callChaser() {
  switch (chaserMode) {
  case CHASER_MODE_NONE:
    break;
    
  case CHASER_MODE_MADNESS:
    if (chaserIndex++ >= 3) {
      chaserIndex = 0;
    }
    
    switch (chaserIndex) {
    case 0:
      chasersVert(5, 1, 2, chaserColour);
      break;
      
    case 1:
      addLineChaser(1, 5, 0, 2, chaserColour);
      addLineChaser(1, 5, 1, 2, chaserColour);
      addLineChaser(3, 5, 0, 2, chaserColour);
      addLineChaser(3, 5, 1, 2, chaserColour);
      addLineChaser(5, 5, 0, 2, chaserColour);
      addLineChaser(5, 5, 1, 2, chaserColour);
      break;

    case 2:
      chasersVert(5, 0, 2, chaserColour);
      break;
    }
    
    break;
    
  case CHASER_MODE_COLUMNS_UP:
    addLineChaser(0, 5, 1, 4, chaserColour);
    addLineChaser(2, 5, 0, 4, chaserColour);
    addLineChaser(4, 5, 0, 4, chaserColour);
    addLineChaser(6, 5, 0, 4, chaserColour);
    break;
    
  case CHASER_MODE_COLUMNS_DOWN:
    addLineChaser(0, 5, 0, 4, chaserColour);
    addLineChaser(2, 5, 1, 4, chaserColour);
    addLineChaser(4, 5, 1, 4, chaserColour);
    addLineChaser(6, 5, 1, 4, chaserColour);
    break;
    
  case CHASER_MODE_COLUMNS_UP_DOWN:
    if (chaserIndex++ >= 2) {
      chaserIndex = 0;
    }
    
    if (chaserIndex == 0) {
      addLineChaser(0, 5, 1, 4, chaserColour);
      addLineChaser(2, 5, 0, 4, chaserColour);
      addLineChaser(4, 5, 0, 4, chaserColour);
      addLineChaser(6, 5, 0, 4, chaserColour);
    } else if (chaserIndex == 1) {
      addLineChaser(0, 5, 0, 4, chaserColour);
      addLineChaser(2, 5, 1, 4, chaserColour);
      addLineChaser(4, 5, 1, 4, chaserColour);
      addLineChaser(6, 5, 1, 4, chaserColour);
    }
    
    break;
    
  case CHASER_MODE_TOP:
    if (chaserIndex++ >= 3) {
      chaserIndex = 0;
    }

    switch (chaserIndex) {
    case 0:
      addLineChaser(1, 5, 0, 2, chaserColour);
      addLineChaser(5, 5, 1, 2, chaserColour);
      break;
      
    case 1:
      addLineChaser(3, 5, 0, 2, chaserColour);
      addLineChaser(3, 5, 1, 2, chaserColour);
      break;

    case 2:
      addLineChaser(1, 5, 1, 2, chaserColour);
      addLineChaser(5, 5, 0, 2, chaserColour);
      break;
    }
  }
}

void addLineChaser(int line, int trail, int direction, int speed, byte colour) {
  if (direction == 0) {
    addChaser(LINE_LAST_LED[line], trail, direction, speed, colour);
  } else {
    addChaser(LINE_FIRST_LED[line], trail, direction, speed, colour);
  }
}

void chasersVert(int trail, int direction, int speed, int colour) {
  addLineChaser(0, trail, direction, speed, colour);
  addLineChaser(2, trail, 1-direction, speed, colour);
  addLineChaser(4, trail, 1-direction, speed, colour);
  addLineChaser(6, trail, 1-direction, speed, colour);
}

void makeChaser(int idx, int ledNum, byte lineNum, byte trailSize, byte direction, byte speed, byte remaining, byte colour) {
  chasersLed[idx] = ledNum;
  chasersLine[idx] = lineNum;
  chasersTrail[idx] = trailSize;
  chasersDirection[idx] = direction;
  chasersSpeed[idx] = speed;
  chasersRemaining[idx] = remaining;
  chasersColour[idx] = colour;
}

void chaserTick(int idx) {
  int remaining = --chasersRemaining[idx];
  if (remaining != 0) {
    return;
  }

  int ledNum = chasersLed[idx];
  byte lineNum = chasersLine[idx];
  byte trailSize = chasersTrail[idx];
  byte odirection = chasersDirection[idx];
  int direction = (chasersDirection[idx] << 1) - 1;
  byte colour = chasersColour[idx];
  
  for (int j = ledNum, k = 0; k < trailSize; j -= direction, k++) {
    if (lineNum == getLineNum(j)) {
      if (trailSize == 5) { //UGLY
        brighten(j, colour, -25);
      } else {
        brighten(j, colour, -CHASER_BRIGHTEN);
      }
    }
  }

  ledNum += direction;
  
  int trailingLedNum = ledNum - direction * (trailSize-1);

  if (getLineNum(ledNum) != lineNum && getLineNum(trailingLedNum) != lineNum) {
    chasersLed[idx] = -1; // done
  } else {
    int speed = chasersSpeed[idx];
    makeChaser(idx, ledNum, lineNum, trailSize, odirection, speed, speed, colour);
    
    if (lineNum == getLineNum(ledNum)) {
      brighten(ledNum, colour, CHASER_BRIGHTEN);
    }
  }
}

void addChaser(int ledNum, byte trailSize, byte direction, byte speed, byte colour) {
  int lineNum = getLineNum(ledNum);
  
  // terrible hack, lineNum == 0 and direction == 0 does not work for some reason. so flip it!! that way works fine
  if (lineNum == 0 && direction == 0) {
    ledNum = LINE_FIRST_LED[lineNum];
    direction = 1;
  }
  
  for (int i = 0; i < N_CHASERS; i++) {
    if (chasersLed[i] == -1) {
      makeChaser(i, ledNum, lineNum, trailSize, direction, speed, speed, colour);
      brighten(ledNum, colour, CHASER_BRIGHTEN);
      break;
    }
  }
}

/************** PULSE FUNCTIONS **************/

void pulseAll(int fadeIn, int fadeOut, byte colour) {
  for (int i = 0; i < N_LINES; i++) {
    addPulse(i, fadeIn, fadeOut, colour);
  }
}

void callPulse() {
//#ifdef DEBUG
//everythingUpTo(40);
//#endif
  switch (pulseMode) {
  case PULSE_MODE_NONE:
    break;
  
  case PULSE_MODE_POP:
    pulseAll(PULSE_FAST, PULSE_MED, pulseColour);
    break;
    
  case PULSE_MODE_FADE:

//#ifdef DEBUG
//everythingUpTo(70);
//#endif
    if ((midiMeasure & 1) == 0) {
      pulseAll(PULSE_SLOW, PULSE_SLOW, pulseColour);
    }
    break;
    
  case PULSE_MODE_INDIVIDUAL:
    if (pulseIndex >= N_LINES) {
      pulseIndex = 0;
    }
    addPulse(pulseIndex++, PULSE_FAST, PULSE_SLOW, pulseColour);
    break;
    
  case PULSE_MODE_TOP_BOTTOM:
    if (pulseIndex >= 2) {
      pulseIndex = 0;
    }
    
    if (pulseIndex == 0) {
      addPulse(0, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(2, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(4, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(6, PULSE_FAST, PULSE_SLOW, pulseColour);
    } else if (pulseIndex == 1) {
      addPulse(1, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(3, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(5, PULSE_FAST, PULSE_SLOW, pulseColour);
    }
    
    pulseIndex++;
    break;
    
  case PULSE_MODE_SIDES:
    if (pulseIndex >= 3) {
      pulseIndex = 0;
    }
    
    if (pulseIndex == 0) {
      addPulse(0, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(1, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(2, PULSE_FAST, PULSE_SLOW, pulseColour);
    } else if (pulseIndex == 1) {
      addPulse(2, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(3, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(4, PULSE_FAST, PULSE_SLOW, pulseColour);
    } else if (pulseIndex == 2) {
      addPulse(4, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(5, PULSE_FAST, PULSE_SLOW, pulseColour);
      addPulse(6, PULSE_FAST, PULSE_SLOW, pulseColour);
    }
    
    pulseIndex++;
    
    break;
    
  case PULSE_MODE_TOP:
    addPulse(1, PULSE_FAST, PULSE_SLOW, pulseColour);
    addPulse(3, PULSE_FAST, PULSE_SLOW, pulseColour);
    addPulse(5, PULSE_FAST, PULSE_SLOW, pulseColour);
    break;
    
  case PULSE_MODE_BOTTOM:
    addPulse(0, PULSE_FAST, PULSE_SLOW, pulseColour);
    addPulse(2, PULSE_FAST, PULSE_SLOW, pulseColour);
    addPulse(4, PULSE_FAST, PULSE_SLOW, pulseColour);
    addPulse(6, PULSE_FAST, PULSE_SLOW, pulseColour);
    break;
    
  case PULSE_MODE_SLOW_FADE:
    if ((midiMeasure & 3) == 0) {
      pulseAll(PULSE_SLOWEST, PULSE_SLOWEST, pulseColour);
    }
    break;
    
  case PULSE_MODE_SLOW_POP:
    if ((midiMeasure & 1) == 0) {
      pulseAll(PULSE_FAST, PULSE_MED, pulseColour);
    }
    break;
    
  case PULSE_MODE_FADE_DROP_FOUR:
    if ((midiMeasure & 3) != 0) {
      pulseAll(PULSE_FAST, PULSE_MED, pulseColour);
    }
    break;
    
  case PULSE_MODE_SLOW_POP_DROP_FOUR:
    if ((midiMeasure & 1) == 0 && (midiMeasure & 7) != 0) {
      pulseAll(PULSE_FAST, PULSE_MED, pulseColour);
    }
    break;
    
  }
          
}

void addPulse(int line, int fadeIn, int fadeOut, byte colour) {
  for (int i = 0; i < N_PULSES; i++) {
    if (pulsesLine[i] == 0xFF) {
      makePulse(i, line, fadeIn, fadeOut, colour);
      break;
    }
  }
}

// WARNING: ensure (PULSES_BRIGHTEN / fadeIn * fadeIn) == (PULSES_BRIGHTEN / fadeOut * fadeOut) is true!!!
// would have made it nicer but embedded programming is tricky
void makePulse(int idx, int line, int fadeIn, int fadeOut, byte colour) {
  pulsesLine[idx] = line;
  pulsesFadeIn[idx] = fadeIn;
  pulsesFadeOut[idx] = fadeOut;
  pulsesColour[idx] = colour;
  pulsesCount[idx] = 0;
}

void pulseTick(int idx) {
  int line = pulsesLine[idx];
  int count = pulsesCount[idx]++;
  int fadeIn = pulsesFadeIn[idx];
  int fadeOut = pulsesFadeOut[idx];
  byte colour = pulsesColour[idx];
  
  int amt = 0;
  
  if (count < fadeIn - 1) {
    brightenLine(line, colour, PULSES_BRIGHTEN/fadeIn);
  } else if (count == fadeIn - 1) {
    brightenLine(line, colour, PULSES_BRIGHTEN/fadeIn);
//    brightenLine(line, PULSES_BRIGHTEN-((PULSES_BRIGHTEN/fadeIn)*fadeIn));
  } else if (count < fadeIn + fadeOut - 1) {
    brightenLine(line, colour, -(PULSES_BRIGHTEN/fadeOut));
  } else if (count == fadeIn + fadeOut - 1) {
    brightenLine(line, colour, -(PULSES_BRIGHTEN/fadeOut));
//    brightenLine(line, -(PULSES_BRIGHTEN-((PULSES_BRIGHTEN/fadeOut)*fadeOut)));
  } else if (count >= fadeIn + fadeOut) {
    pulsesLine[idx] = 0xFF;
  }
}

/************** COLOUR FUNCTIONS **************/

void setSolidColour(int colour, int v) {
  for (int i = 0; i < LED_COUNT; i++) {
    brighten(i, colour, v);
  }
  
  updateStrip();
}

void setSolidTopColour(int colour, int v) {
  brightenLine(1, colour, v);
  brightenLine(3, colour, v);
  brightenLine(5, colour, v);
  
  updateStrip();
}

void setSolidBottomColour(int colour, int v) {
  brightenLine(0, colour, v);
  brightenLine(2, colour, v);
  brightenLine(4, colour, v);
  brightenLine(7, colour, v);
  
  updateStrip();
}

/************** DEBUG **************/

#ifdef DEBUG
void everythingUpTo(int x) {
  for (int i = 0; i < LED_COUNT; i++) {
    if (i < x) strip.setPixelColor(i, strip.Color(64, 64, 64));
    else if (i == x) strip.setPixelColor(i, strip.Color(64, 0, 0));
    else strip.setPixelColor(i, 0);
  }
}
#endif

#ifdef DEBUG
int availableChasers() {
  int res = 0;
  
  for (int i = 0; i < N_CHASERS; i++) {
    if (chasersLed[i] != -1) {
      res++;
    }
  }
  
  return res;
}
#endif

#ifdef DEBUG
int availablePulsers() {
  int res = 0;
  
  for (int i = 0; i < N_PULSES; i++) {
    if (pulsesLine[i] != 0xFF) {
      res++;
    }
  }
  
  return res;
}
#endif


