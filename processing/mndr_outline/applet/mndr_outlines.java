import processing.core.*; 
import processing.xml.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class mndr_outlines extends PApplet {

// 0 = YDOWN
// 1 = RIGHT
// 2 = YUP
// 3 = LEFT
// 4 = ZDOWN
// 5 = ZUP

static final int dx[] = { 0, 1, 0, -1, 0, 0 };
static final int dy[] = { 1, 0, -1, 0, 0, 0 };
static final int dz[] = { 0, 0, 0, 0, 1, -1 };

static final int N_LINES = 7;

static final int LED_LINES[] = { 6, 19, 6, 29, 6, 19, 6, 0 };

static final int LED_COUNT = 91;

static final int LED_COLOUR_MAX = 127;
static final int P5_COLOUR_MAX = 255;

static final int LINE_IDX[] = new int[LED_COUNT+6];
static final int LINE_FIRST_LED[] = new int[N_LINES+1];

static final int CONNECTION_D[] = { 5, 2, 4, 1, 4, 0, 4 };
static final int LINE_X[] = { 0,  0,   0,   0,  29,  29, 29 };
static final int LINE_Y[] = { 0,  0, -19, -19, -19, -19,  0 };
static final int LINE_Z[] = { 0, -6,  -6,  -6,  -6,  -6, -6 };

static final int CONNECTIONS_L[] = { 0, 1, 10, 6, 40, 24, 32 };

//   1234 567
// 1 0000 000  0
// 2 1000 000  1
// 3 0101 000 10
// 4 0110 000  6
// 5 0001 010 40
// 6 0001 100 24
// 7 0000 010 32

static final int CONNECTIONS_R[] = { 2, 12, 0, 48, 0, 64, 0 };

//   1234 567
// 1 0100 000  2
// 2 0011 000 12
// 3 0000 000  0
// 4 0000 110 48
// 5 0000 000  0
// 6 0000 001 64
// 7 0000 000  0

// data structure:
// - 8 bits for the LED number
// - 4 bits for the trail number
int nChasers = 16;
int chasers[] = new int[nChasers];

static final int CHASER_BRIGHTEN = 127;

float brickSize = 0.5f;
Cube brick = new Cube(brickSize, brickSize, brickSize);

int lr[] = new int[LED_COUNT];
int lg[] = new int[LED_COUNT];
int lb[] = new int[LED_COUNT];

float lrm[] = new float[LED_COUNT];
float lgm[] = new float[LED_COUNT];
float lbm[] = new float[LED_COUNT];

public void setup() {
  size(640, 480, P3D);
  
  // Initialize chasers
  
  for (int i = 0; i < nChasers; i++) {
    chasers[i] = -1;
  }
  
  // Construct LED index to line index map
  
  int lineIdx = 0;
  int lineIdxNext = LED_LINES[0];
  
  for (int i = 0; i < LED_COUNT; i++) {
    if (i >= lineIdxNext) {
      lineIdxNext += LED_LINES[++lineIdx];
    }
    LINE_IDX[i] = lineIdx;
  }
  
  // Construct line first LED index map
  
  int ledIdx = 0;
  
  for (int i = 0; i < N_LINES+1; i++) {
    LINE_FIRST_LED[i] = ledIdx;
    if (i < N_LINES) {
      ledIdx += LED_LINES[i];
    }
  }
}

long nextAddChaser = 0;
long nextAdvanceChaser = 0;
boolean toggle = false;

public void draw() {
  long ms = millis();
  
  if (ms >= nextAdvanceChaser) {
    advanceChasers();
    nextAdvanceChaser += 50;
  }
  
  if (ms >= nextAddChaser) {
    if (toggle) {
      for (int i = 0; i < N_LINES; i++) {
        int ledNum = LINE_FIRST_LED[i+1]-1;
        if (ledNum >= 0) {
          addChaser(ledNum, 5, 1);
        }
      }
    } else {
      for (int i = 0; i < N_LINES; i++) {
        int ledNum = LINE_FIRST_LED[i];
        if (ledNum >= 0) {
          addChaser(ledNum, 5, 0);
        }
      }
    }
    
    toggle = !toggle;
//    addChaser(LINE_FIRST_LED[1]-1, 5, 1);
    nextAddChaser += 500;
  }
  
  background(127);
  
  translate(width/2, height/2);
  rotateX(-PI/2.0f);
  rotateZ(frameCount / 100.0f);
  scale(10);
  
  int ledCount = 0;
  
  for (int i = 0; i < N_LINES; i++) {
    float sx = LINE_X[i] - 14.5f;
    float sy = LINE_Y[i] + 9.5f;
    float sz = LINE_Z[i] + 3;
    
    float ex = LINE_X[i] + LED_LINES[i] * dx[CONNECTION_D[i]] - 14.5f;
    float ey = LINE_Y[i] + LED_LINES[i] * dy[CONNECTION_D[i]] + 9.5f;
    float ez = LINE_Z[i] + LED_LINES[i] * dz[CONNECTION_D[i]] + 3;
    
    stroke(0);
    line(sx, sy, sz, ex, ey, ez);
    
    sx += dx[CONNECTION_D[i]] * 0.5f;
    sy += dy[CONNECTION_D[i]] * 0.5f;
    sz += dz[CONNECTION_D[i]] * 0.5f;
        
    for (int j = 0; j < LED_LINES[i]; j++) {
      noStroke();
      fill(
        (lr[ledCount] + lrm[ledCount]) / LED_COLOUR_MAX * P5_COLOUR_MAX,
        (lg[ledCount] + lgm[ledCount]) / LED_COLOUR_MAX * P5_COLOUR_MAX,
        (lb[ledCount] + lbm[ledCount]) / LED_COLOUR_MAX * P5_COLOUR_MAX);
      brick.create(sx, sy, sz);
      
      sx += dx[CONNECTION_D[i]];
      sy += dy[CONNECTION_D[i]];
      sz += dz[CONNECTION_D[i]];
      
      ledCount++;
    }
  }
}

public int clamp(int x, int minimum, int maximum) {
  return min(max(x, minimum), maximum);
}

// data structure:
// - 8 bits for LED number
// - 4 bits for the line number
// - 4 bits for the trail number

public int getChaserLed(int x) {
  int res = x & 0xFF;
  if (res >= 230) {
    return res - 255;
  } else {
    return res;
  }
}

public int getChaserLine(int x) {
  return (x >> 8) & 0xF;
}

public int getChaserTrailSize(int x) {
  return (x >> 12) & 0x7;
}

public int getChaserDirection(int x) {
  return (x >> 15) & 0x1;
}

public int makeChaser(int ledNum, int lineNum, int trailSize, int direction) {
  if (ledNum < 0) {
    ledNum += 255;
  }
  
  return ((clamp(direction, 0, 1) & 0x1) << 15)
       | ((clamp(trailSize, 1, 5) & 0x7) << 12)
       | ((lineNum & 0xF) << 8)
       | (ledNum & 0xFF);
}

public void addChaser(int ledNum, int trailSize, int direction) {
  for (int i = 0; i < nChasers; i++) {
    if (chasers[i] == -1) {
      chasers[i] = makeChaser(ledNum, LINE_IDX[ledNum], trailSize, clamp(direction, 0, 1));
      brighten(ledNum, CHASER_BRIGHTEN);
      System.out.printf("new chaser idx %d led %d trailSize %d direction %d\n", i, ledNum, trailSize, direction);
      break;
    }
  }
}

public void brighten(int idx, float v) {
  lrm[idx] += v;
  lgm[idx] += v;
  lbm[idx] += v;
}

public void advanceChasers() {
  for (int i = 0; i < nChasers; i++) {
    if (chasers[i] != -1) {
      int ledNum = getChaserLed(chasers[i]);
      int lineNum = getChaserLine(chasers[i]);
      int trailSize = getChaserTrailSize(chasers[i]);
      int direction = 2*getChaserDirection(chasers[i])-1;
      
      float bright = (float) CHASER_BRIGHTEN / trailSize;
      
      for (int j = ledNum, k = 0; k < trailSize; j += direction, k++) {
        if (0 <= j && j < LINE_IDX.length&& lineNum == LINE_IDX[j]) {
          brighten(j, -bright);
        }
      }
      
      ledNum -= direction;
      
      int trailingLedNum = ledNum + direction * (trailSize-1);

      System.out.printf("trailingLed %d led %d\n", trailingLedNum, ledNum);
      
      if ((ledNum < 0 || LINE_IDX[ledNum] != lineNum) && (trailingLedNum < 0 || LINE_IDX[trailingLedNum] != lineNum)) {
        chasers[i] = -1;
        System.out.printf("finished chaser idx %d trail %d ledNum %d (%d) trailingLedNum %d (%d) lineNum %d\n", i, trailSize, ledNum, ledNum >= 0 ? LINE_IDX[ledNum] : -1, trailingLedNum, trailingLedNum >= 0 ? LINE_IDX[trailingLedNum] : -1, lineNum);
      } else {
        chasers[i] = makeChaser(ledNum, lineNum, trailSize, direction);
        if (ledNum >= 0 && lineNum == LINE_IDX[ledNum]) {
          brighten(ledNum, CHASER_BRIGHTEN);
        }
        System.out.printf("chaser idx %d at led %d trail %d\n", i, ledNum, trailSize);
      }
    }
  }
}

class Cube {

  PVector[] vertices = new PVector[24];
  float w, h, d;

  Cube(){ }

  Cube(float w, float h, float d){
    this.w = w;
    this.h = h;
    this.d = d;

    // Cube composed of 6 quads
    // Front
    vertices[0] = new PVector(-w/2, -h/2, d/2);
    vertices[1] = new PVector(w/2, -h/2, d/2);
    vertices[2] = new PVector(w/2, h/2, d/2);
    vertices[3] = new PVector(-w/2, h/2, d/2);

    // Left
    vertices[4] = new PVector(-w/2, -h/2, d/2);
    vertices[5] = new PVector(-w/2, -h/2, -d/2);
    vertices[6] = new PVector(-w/2, h/2, -d/2);
    vertices[7] = new PVector(-w/2, h/2, d/2);

    // Right
    vertices[8] = new PVector(w/2, -h/2, d/2);
    vertices[9] = new PVector(w/2, -h/2, -d/2);
    vertices[10] = new PVector(w/2, h/2, -d/2);
    vertices[11] = new PVector(w/2, h/2, d/2);

    // Back
    vertices[12] = new PVector(-w/2, -h/2, -d/2);  
    vertices[13] = new PVector(w/2, -h/2, -d/2);
    vertices[14] = new PVector(w/2, h/2, -d/2);
    vertices[15] = new PVector(-w/2, h/2, -d/2);

    // Top
    vertices[16] = new PVector(-w/2, -h/2, d/2);
    vertices[17] = new PVector(-w/2, -h/2, -d/2);
    vertices[18] = new PVector(w/2, -h/2, -d/2);
    vertices[19] = new PVector(w/2, -h/2, d/2);

    // Bottom
    vertices[20] = new PVector(-w/2, h/2, d/2);
    vertices[21] = new PVector(-w/2, h/2, -d/2);
    vertices[22] = new PVector(w/2, h/2, -d/2);
    vertices[23] = new PVector(w/2, h/2, d/2);
  }

  public void create(float x, float y, float z) {
    pushMatrix();
    translate(x, y, z);
    
    for (int i=0; i<6; i++){
      beginShape(QUADS);
      for (int j = 0; j < 4; j++){
        vertex(vertices[j+4*i].x, vertices[j+4*i].y, vertices[j+4*i].z);
      }
      endShape();
    }
    
    popMatrix();
  }
} 
  static public void main(String args[]) {
    PApplet.main(new String[] { "--present", "--bgcolor=#666666", "--hide-stop", "mndr_outlines" });
  }
}
