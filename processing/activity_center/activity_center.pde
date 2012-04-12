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

static final int LED_LINES[] = { 4, 16, 4, 29, 4, 16, 4, 0 };

static final int LED_COUNT = 77;

static final int LED_COLOUR_MAX = 127;
static final int P5_COLOUR_MAX = 255;

static final int LINE_IDX[] = new int[LED_COUNT+6];
static final int LINE_FIRST_LED[] = new int[N_LINES+1];
static final int LINE_LAST_LED[] = new int[N_LINES+1];

static final int CONNECTION_D[] = { 5, 2, 4, 1, 4, 0, 4 };
static final int LINE_X[] = { 0,  0,   0,   0,  29,  29, 29 };
static final int LINE_Y[] = { 0,  0, -16, -16, -16, -16,  0 };
static final int LINE_Z[] = { 0, -4,  -4,  -4,  -4,  -4, -4 };

static final int CONNECTIONS[][] = { { 2, 12, 0, 48, 0, 64, 0 }, { 0, 1, 10, 6, 40, 24, 32 } };
static final int DIRECTION_FLIP[][] = { { 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 0, 1, 0 } };

// LEFT CONNECTIONS
//   1234 567
// 1 0000 000  0
// 2 1000 000  1
// 3 0101 000 10
// 4 0110 000  6
// 5 0001 010 40
// 6 0001 100 24
// 7 0000 010 32

// RIGHT CONNECTIONS
//   1234 567
// 1 0100 000  2
// 2 0011 000 12
// 3 0000 000  0
// 4 0000 110 48
// 5 0000 000  0
// 6 0000 001 64
// 7 0000 000  0

// data structure defined by functions
int nChasers = 16;
int chasers[] = new int[nChasers];

static final int CHASER_BRIGHTEN = 127;

float brickSize = 0.5;
Cube brick = new Cube(brickSize, brickSize, brickSize);

int lr[] = new int[LED_COUNT];
int lg[] = new int[LED_COUNT];
int lb[] = new int[LED_COUNT];

float lrm[] = new float[LED_COUNT];
float lgm[] = new float[LED_COUNT];
float lbm[] = new float[LED_COUNT];

void setup() {
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
    LINE_LAST_LED[i] = ledIdx-1;
  }
}

long nextAddChaser = 0;
long nextAdvanceChaser = 0;
boolean toggle = false;

void draw() {
  long ms = millis();
  
  if (ms >= nextAdvanceChaser) {
    advanceChasers();
    nextAdvanceChaser += 50;
  }
  
  if (ms >= nextAddChaser) {
    if (toggle) {
      for (int i = 0; i < N_LINES; i++) {
        int ledNum = LINE_FIRST_LED[i];
        if (ledNum >= 0) {
          addChaser(ledNum, 5, 0, 5);
          break;
        }
      }
    } else {
      for (int i = N_LINES-1; i >= 0; i--) {
        int ledNum = LINE_LAST_LED[i];
        if (ledNum >= 0) {
          addChaser(ledNum, 5, 1, 5);
          break;
        }
      }
    }
    
    toggle = !toggle;
//    addChaser(LINE_FIRST_LED[2]-1, 5, 1, 5);
    nextAddChaser += 500;
  }
  
  background(127);
  
  translate(width/2, height/2);
  rotateX(-PI/2.0);
  rotateZ(frameCount / 200.0);
  scale(10);
  
  int ledCount = 0;
  
  for (int i = 0; i < N_LINES; i++) {
    float sx = LINE_X[i] - 14.5;
    float sy = LINE_Y[i] + 8;
    float sz = LINE_Z[i] + 2;
    
    float ex = LINE_X[i] + LED_LINES[i] * dx[CONNECTION_D[i]] - 14.5;
    float ey = LINE_Y[i] + LED_LINES[i] * dy[CONNECTION_D[i]] + 8;
    float ez = LINE_Z[i] + LED_LINES[i] * dz[CONNECTION_D[i]] + 2;
    
    stroke(0);
    line(sx, sy, sz, ex, ey, ez);
    
    sx += dx[CONNECTION_D[i]] * 0.5;
    sy += dy[CONNECTION_D[i]] * 0.5;
    sz += dz[CONNECTION_D[i]] * 0.5;
        
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

int clamp(int x, int minimum, int maximum) {
  return min(max(x, minimum), maximum);
}

// data structure:
// - 8 bits for LED number
// - 4 bits for the line number
// - 4 bits for the trail number

int getChaserLed(int x) {
  int res = x & 0x7F;
  if (res >= 120) {
    return res - 127;
  } else {
    return res;
  }
}

int getChaserLine(int x) {
  return (x >> 7) & 0x7;
}

int getChaserTrailSize(int x) {
  return (x >> 10) & 0x7;
}

int getChaserDirection(int x) {
  return (x >> 13) & 0x1;
}

int getChaserRemaining(int x) {
  return (x >> 14) & 0xF;
}

int getChaserSpawned(int x) {
  return (x >> 18) & 0x1;
}

int makeChaser(int ledNum, int lineNum, int trailSize, int direction, int remaining, int spawned) {
  if (ledNum < 0) {
    ledNum += 127;
  }
  
  return ((clamp(spawned, 0, 1) & 0x1) << 18)
       | ((clamp(remaining, 0, 0xF) & 0xF) << 14)
       | ((clamp(direction, 0, 1) & 0x1) << 13)
       | ((clamp(trailSize, 1, 5) & 0x7) << 10)
       | ((lineNum & 0x7) << 7)
       | (ledNum & 0x7F);
}

void addChaser(int ledNum, int trailSize, int direction, int remaining) {
  for (int i = 0; i < nChasers; i++) {
    if (chasers[i] == -1) {
      chasers[i] = makeChaser(ledNum, LINE_IDX[ledNum], trailSize, direction, remaining, 0);
      brighten(ledNum, CHASER_BRIGHTEN);
      System.out.printf("new chaser idx %d led %d trailSize %d direction %d rem %d\n", i, ledNum, trailSize, direction, remaining);
      break;
    }
  }
}

void brighten(int idx, float v) {
  lrm[idx] += v;
  lgm[idx] += v;
  lbm[idx] += v;
}

int nBits(int x) {
  int res = 0;
  
  for (int i = 0, m = 1; i < 16; i++, m <<= 1) {
    if ((m & x) == m) {
      res++;
    }
  }
  
  return res;
}

int bitIndex(int x, int idx) {
  for (int i = 0, j = 0; x != 0; i++, x >>= 1) {
    if ((x & 1) != 0) {
      if (j++ == idx) {
        System.out.printf("bitIndex x %d idx %d\n", x, idx);
        return i;
      }
    }
  }
  
  return -1;
}

void advanceChasers() {
  for (int i = 0; i < nChasers; i++) {
    if (chasers[i] != -1) {
      int ledNum = getChaserLed(chasers[i]);
      int lineNum = getChaserLine(chasers[i]);
      int trailSize = getChaserTrailSize(chasers[i]);
      int odirection = getChaserDirection(chasers[i]);
      int direction = (odirection << 1) - 1;
      int remaining = getChaserRemaining(chasers[i]);
      int spawned = getChaserSpawned(chasers[i]);
      
      float bright = (float) CHASER_BRIGHTEN / trailSize;
      
      for (int j = ledNum, k = 0; k < trailSize; j += direction, k++) {
        if (0 <= j && j < LINE_IDX.length && lineNum == LINE_IDX[j]) {
          brighten(j, -bright);
        }
      }
      
      ledNum -= direction;
      
      int trailingLedNum = ledNum + direction * (trailSize-1);
      
      boolean done = false;
      
      if (ledNum < 0 || LINE_IDX[ledNum] != lineNum) {
        if (trailingLedNum < 0 || LINE_IDX[trailingLedNum] != lineNum) { // done
            chasers[i] = -1;
            System.out.printf("finished chaser idx %d trail %d ledNum %d (%d) trailingLedNum %d (%d) lineNum %d\n", i, trailSize, ledNum, ledNum >= 0 ? LINE_IDX[ledNum] : -1, trailingLedNum, trailingLedNum >= 0 ? LINE_IDX[trailingLedNum] : -1, lineNum);
            done = true;
        } else if (spawned == 0) {
          if (remaining > 0) { // spawn a new chaser
            int edges = CONNECTIONS[odirection][lineNum];
            int nEdges = nBits(edges);
            
            if (nEdges > 0) {
              int nextEdge = 0;
              if (nEdges > 1) {
                int nextEdgeIndex = int(random(nEdges));
                nextEdge = bitIndex(edges, nextEdgeIndex);
              } else {
                nextEdge = bitIndex(edges, 0);
              }
              System.out.printf("line %d to line %d odir %d edges %d\n", lineNum, nextEdge, odirection, CONNECTIONS[odirection][lineNum]);
              int nextODirection = DIRECTION_FLIP[odirection][nextEdge] == 0 ? odirection : 1-odirection;
              int nextLedNum = nextODirection == 0 ? LINE_FIRST_LED[nextEdge] : LINE_LAST_LED[nextEdge];
              addChaser(nextLedNum, trailSize, nextODirection, remaining-1);
            }
          }
          
          spawned = 1; // regardless, don't consider it again for any respawning
        }
      }
      
      if (!done) {
        chasers[i] = makeChaser(ledNum, lineNum, trailSize, direction, remaining, spawned);
        if (ledNum >= 0 && lineNum == LINE_IDX[ledNum]) {
          brighten(ledNum, CHASER_BRIGHTEN);
        }
//      System.out.printf("chaser idx %d at led %d trail %d rem %d\n", i, ledNum, trailSize, remaining);
      }
    }
  }
}

