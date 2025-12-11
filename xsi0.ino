#include <FastLED.h>
#include <Keypad.h>
#define LED_PIN       6           
#define W             8
#define H             8
#define NUM_LEDS      (W * H)
#define BRIGHTNESS    40

CRGB leds[NUM_LEDS];

// Culori
const CRGB COL_X    = CRGB(255, 0, 0);   
const CRGB COL_O    = CRGB(0, 0, 255);   
const CRGB COL_GRID = CRGB(0, 120, 0);   
const CRGB COL_CPU_MODE = CRGB(200, 0, 200); 

const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {7, 8, 9, 10}; 
byte colPins[COLS] = {2, 3, 4, 5};   

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

byte board[3][3];
byte currentPlayer = 1;   
bool gameOver = false;
bool vsComputer = false; 

uint16_t getLEDIndex(uint8_t row, uint8_t col) {
  return row * W + col;
}

void fillCell(uint8_t cellRow, uint8_t cellCol, CRGB color) {
  uint8_t startRow = cellRow * 3;
  uint8_t startCol = cellCol * 3;
  
  for (uint8_t r = startRow; r < startRow + 2; r++) {
    for (uint8_t c = startCol; c < startCol + 2; c++) {
      leds[getLEDIndex(r, c)] = color;
    }
  }
}

void drawGrid() {
  for (uint8_t r = 0; r < H; r++) {
    leds[getLEDIndex(r, 2)] = COL_GRID;
    leds[getLEDIndex(r, 5)] = COL_GRID;
  }
  for (uint8_t c = 0; c < W; c++) {
    leds[getLEDIndex(2, c)] = COL_GRID;
    leds[getLEDIndex(5, c)] = COL_GRID;
  }
  
  if(vsComputer) {
     leds[getLEDIndex(7, 7)] = COL_CPU_MODE;
  }
}

void redrawAll() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawGrid();
  for (uint8_t r = 0; r < 3; r++)
    for (uint8_t c = 0; c < 3; c++) {
      if (board[r][c] == 1) fillCell(r, c, COL_X);
      else if (board[r][c] == 2) fillCell(r, c, COL_O);
    }
  FastLED.show();
}

byte checkWin(uint8_t w[3][2]) {
  for (uint8_t r=0; r<3; r++) {
    if (board[r][0] && board[r][0]==board[r][1] && board[r][0]==board[r][2]) {
      for (uint8_t i=0;i<3;i++){ w[i][0]=r; w[i][1]=i; }
      return board[r][0];
    }
  }
  for (uint8_t c=0; c<3; c++) {
    if (board[0][c] && board[0][c]==board[1][c] && board[0][c]==board[2][c]) {
      for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=c; }
      return board[0][c];
    }
  }
  if (board[0][0] && board[0][0]==board[1][1] && board[0][0]==board[2][2]) {
    for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=i; }
    return board[0][0];
  }
  if (board[0][2] && board[0][2]==board[1][1] && board[0][2]==board[2][0]) {
    for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=2-i; }
    return board[0][2];
  }
  return 0;
}

void blinkWin(uint8_t w[3][2], byte winner) {
  CRGB col = (winner==1) ? COL_X : COL_O;
  for (uint8_t k=0; k<6; k++) {
    redrawAll();
    for (uint8_t i=0;i<3;i++) fillCell(w[i][0], w[i][1], CRGB::Black);
    FastLED.show(); delay(200);

    redrawAll();
    for (uint8_t i=0;i<3;i++) fillCell(w[i][0], w[i][1], col);
    FastLED.show(); delay(200);
  }
}

bool isDraw() {
  for (uint8_t r=0;r<3;r++)
    for (uint8_t c=0;c<3;c++)
      if (board[r][c]==0) return false;
  return true;
}

void resetGame() {
  memset(board, 0, sizeof(board));
  currentPlayer = 1;
  gameOver = false;
  redrawAll();
}

bool findBestMove(byte targetPlayer, int &bestR, int &bestC) {
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      if (board[r][c] == 0) {
        // Simulează mutarea
        board[r][c] = targetPlayer;
        uint8_t tempW[3][2];
        if (checkWin(tempW) == targetPlayer) {
          board[r][c] = 0;
          bestR = r;
          bestC = c;
          return true;
        }
        board[r][c] = 0;
      }
    }
  }
  return false;
}

void computerMove() {
  int r, c;
  
  if (findBestMove(2, r, c)) {
    board[r][c] = 2;
    return;
  }

  if (findBestMove(1, r, c)) {
    board[r][c] = 2;
    return;
  }

  if (board[1][1] == 0) {
    board[1][1] = 2;
    return;
  }

  for (int i = 0; i < 50; i++) {
    r = random(0, 3);
    c = random(0, 3);
    if (board[r][c] == 0) {
      board[r][c] = 2;
      return;
    }
  }
  
  for(r=0; r<3; r++) 
    for(c=0; c<3; c++) 
      if(board[r][c]==0) { board[r][c]=2; return; }
}

void setup() {
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(0));
  resetGame();
}

void loop() {
  char key = kpd.getKey();
  if (!key) return;

  if (key == 'A') {
    vsComputer = !vsComputer; 
    
    fill_solid(leds, NUM_LEDS, vsComputer ? COL_CPU_MODE : CRGB::White);
    FastLED.show();
    delay(500);
    
    resetGame();
    return;
  }
  
  if (key == 'D') { 
    return;
  }

  if (key < '1' || key > '9' || gameOver) return;

  uint8_t k = key - '1';      
  uint8_t r = k / 3;          
  uint8_t c = k % 3;          

  if (board[r][c] == 0) {
    board[r][c] = currentPlayer; 
    redrawAll();

    uint8_t wCells[3][2];
    byte w = checkWin(wCells);
    if (w != 0) { blinkWin(wCells, w); resetGame(); return; }
    if (isDraw()) { delay(500); resetGame(); return; }

    currentPlayer = (currentPlayer == 1) ? 2 : 1;

    if (vsComputer && currentPlayer == 2 && !gameOver) {
      delay(600); 
      computerMove();
      redrawAll();

      w = checkWin(wCells);
      if (w != 0) { blinkWin(wCells, w); resetGame(); return; }
      if (isDraw()) { delay(500); resetGame(); return; }

      currentPlayer = 1;
    }
  }
}