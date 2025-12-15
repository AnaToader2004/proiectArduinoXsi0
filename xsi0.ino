#include <FastLED.h>
#include <Keypad.h>
//pe pinul 6 este conectata matricea de leduri   
#define LED_PIN       6 
//latimea si inaltimea matricei de leduri    
#define W             8 
#define H             8 

#define NUM_LEDS      (W * H)
//luminozitatea globala a ledurilor/intensitatea
#define BRIGHTNESS    40 

CRGB leds[NUM_LEDS]; //un array de 8x8 elemente unde fiecare element este o culoare RGB

// Culori
const CRGB COL_X    = CRGB(255, 0, 0);   //atribui culori pentru x,0, grid si modul de joc
const CRGB COL_O    = CRGB(0, 0, 255);   
const CRGB COL_GRID = CRGB(0, 120, 0);   
const CRGB COL_CPU_MODE = CRGB(200, 0, 200); 

const byte ROWS = 4; //numarul de randuri si coloane 
                      //pentru matricea de butoane
const byte COLS = 4; 

//eu controlez ledurile din primele 3 coloane si 3 randuri
//si coloana 4 rand 1=> adica A este pentru schimbarea modului de joc
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

//pinii arduino conectati la randurile si coloanele matricei de butoane
byte rowPins[ROWS] = {7, 8, 9, 10}; 
byte colPins[COLS] = {2, 3, 4, 5};   

//se creeaza un obiect keypad care stie cum sa citeasca butoanele
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//tabla de joc x si 0
byte board[3][3];
byte currentPlayer = 1;   // primul jucator este mereu =>1
bool gameOver = false;
bool vsComputer = false; 

//transforma coordonatele matricei de leduri (rand,coloana)
//intr-un indec de la 0-63, adica fiecare led e indexat
uint16_t getLEDIndex(uint8_t row, uint8_t col) {
  return row * W + col;
}


//coloreaza ceulele de joc cu rosu-albastru 
//oarecum coloreaza o celula de 2x2 din matricea de 8x8 alege care le coloreaza
void fillCell(uint8_t cellRow, uint8_t cellCol, CRGB color) {
  //este un multiplu de 3 ledul de unde incepe o celula fie 0,3,6
  //celulele pot sa fie in intrevalul 3x3 
  //spre ex (1,2) atunci cu functi asta alege ce leduri din matrice coloreaza
  uint8_t startRow = cellRow * 3;
  uint8_t startCol = cellCol * 3;
  
  //parcurge acel led de la randul si coloana respectiva pe gridul nostru si il deseneaza 
  for (uint8_t r = startRow; r < startRow + 2; r++) {
    //aceste foruri verifica daca se deseneaza doar o matrice de 2x2
    //dupa ce ajunge la un startRow 0,3,6 de acolo mai deseneaza doar soua leduri adica matricea aia de 2x2 
    for (uint8_t c = startCol; c < startCol + 2; c++) {
      leds[getLEDIndex(r, c)] = color;
    }
  }
}

void drawGrid() {
  //deseneaza grid ul de joc pe randuri si coloane 
  for (uint8_t r = 0; r < H; r++) {
    leds[getLEDIndex(r, 2)] = COL_GRID;
    leds[getLEDIndex(r, 5)] = COL_GRID;
  }
  for (uint8_t c = 0; c < W; c++) {
    leds[getLEDIndex(2, c)] = COL_GRID;
    leds[getLEDIndex(5, c)] = COL_GRID;
  }
  
  //aici daca e modul vsComputer aprinde un led mov in colt dreapta jos
  if(vsComputer) {
     leds[getLEDIndex(7, 7)] = COL_CPU_MODE;
  }
}


void redrawAll() {
  //functie care face ca toate ledurile din matrice sa fie sterse
  //sa se stearga ce era pe ele inainte
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  drawGrid();

  //verific toate celulde de joc de pe matricea 3x3
  for (uint8_t r = 0; r < 3; r++)
    for (uint8_t c = 0; c < 3; c++) {

      //daca e =1 atunci deseneaza rosu x 
      if (board[r][c] == 1) 
        fillCell(r, c, COL_X);
        //daca e 2 atunci albastru 0
      else if (board[r][c] == 2)
         fillCell(r, c, COL_O);
    }

    //cu functia asta se aprind efectiv ledurile in matricea de leduri
  FastLED.show();
}

//verifica cine a castigat jocul 
//in w se salveaza cele 3 celule care au dus la castig
byte checkWin(uint8_t w[3][2]) {

  //verificam daca pe rand a castigat cineva
  //pe orizontala-fiecare linie
  for (uint8_t r=0; r<3; r++) {
    if (board[r][0] && board[r][0]==board[r][1] && board[r][0]==board[r][2]) {
      for (uint8_t i=0;i<3;i++){ w[i][0]=r; w[i][1]=i; }
      return board[r][0];
    }
  }

  //verific daca pe coloana s-a castigat
  //pe verticala fiecare coloana
  for (uint8_t c=0; c<3; c++) {
    if (board[0][c] && board[0][c]==board[1][c] && board[0][c]==board[2][c]) {
      for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=c; }
      return board[0][c];
    }
  }


//verifica diagonala principala
  if (board[0][0] && board[0][0]==board[1][1] && board[0][0]==board[2][2]) {
    for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=i; }
    return board[0][0];
  }

  //diagonala secundara
  if (board[0][2] && board[0][2]==board[1][1] && board[0][2]==board[2][0]) {
    for (uint8_t i=0;i<3;i++){ w[i][0]=i; w[i][1]=2-i; }
    return board[0][2];
  }
  return 0;
}


//face ca cele 3 celule castigatoare sa clipeasca
void blinkWin(uint8_t w[3][2], byte winner) {
  //primeste cele 3 celule castigatoare si aleje cum le face sa clipeasca 
  CRGB col = (winner==1) ? COL_X : COL_O;
  //stinge si aprinde celulele de 6 ori 
  for (uint8_t k=0; k<6; k++) {
    //aprinde celulele castigatoare
    redrawAll();
    for (uint8_t i=0;i<3;i++)
    //sterge celulele 
     fillCell(w[i][0], w[i][1], CRGB::Black);
    FastLED.show();
     delay(200);

    //aprinde celulele 
    redrawAll();
    for (uint8_t i=0;i<3;i++) 
    fillCell(w[i][0], w[i][1], col);
    FastLED.show(); delay(200);
  }
}

//verifica daca e remiza
//adica nici un castigator
bool isDraw() {
  for (uint8_t r=0;r<3;r++)
    for (uint8_t c=0;c<3;c++)
      if (board[r][c]==0) 
        return false;
  return true;
}

//reseteaza jocul
void resetGame() {
  memset(board, 0, sizeof(board));
  currentPlayer = 1;
  gameOver = false;
  redrawAll();
}

//returneaza daca o gasit sau nu o mutare buna
bool findBestMove(byte targetPlayer, int &bestR, int &bestC) {
  //targetPlayer este jucatorul pentru care vreau sa gasesc miscarea cea mai buna
  for (int r = 0; r < 3; r++) {
    for (int c = 0; c < 3; c++) {
      if (board[r][c] == 0) {
        // Simuleaza mutarea, presupunem doar ca este targetPlayer
        board[r][c] = targetPlayer;
        uint8_t tempW[3][2];
        //daca as muta aici as castiga?
        if (checkWin(tempW) == targetPlayer) {
          //anulam simularea
          board[r][c] = 0;

          //salveaza muatrea gasita
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
  //daca exita mutare ca 0 sa castige, muta=> adica pune 2 acolo unde e 0
  if (findBestMove(2, r, c)) {
    board[r][c] = 2;
    return;
  }

  //daca ar fi o mutare a lui x=> adica 1 care sa castige atunci blockeaza
  if (findBestMove(1, r, c)) {
    board[r][c] = 2;
    return;
  }

  //daca nu am nimic in 1,1 pune acolo, cea mai buna mutare
  if (board[1][1] == 0) {
    board[1][1] = 2;
    return;
  }

  //incerca o mutare aleatorie
  for (int i = 0; i < 50; i++) {
    r = random(0, 3);
    c = random(0, 3);
    if (board[r][c] == 0) {
      board[r][c] = 2;
      return;
    }
  }
  
  //o muta unde gaseste liber prima data
  for(r=0; r<3; r++) 
    for(c=0; c<3; c++) 
      if(board[r][c]==0) { 
        board[r][c]=2; return; }
}

void setup() {
  //Spune librariei FastLED ce tip de LED-uri am, pe ce pin sunt si unde este buffer-ul de culori.
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  //seteaza luminozitatea globala
  FastLED.setBrightness(BRIGHTNESS);
  //pentru random din functia de mutari
  randomSeed(analogRead(0));
  //reseteaza jocul
  resetGame();
}

void loop() {
  //citeste o tasta apasata
  char key = kpd.getKey();
  if (!key)
     return;

  if (key == 'A') {
    vsComputer = !vsComputer; 
    
    //face mov daca intra in modul vsComputer alb daca iese, toate ledurile
    fill_solid(leds, NUM_LEDS, vsComputer ? COL_CPU_MODE : CRGB::White);
    FastLED.show();
    delay(500);
    
    resetGame();
    return;
  }
  
  if (key == 'D') { 
    return;
  }

  //ignora daca nu e apasat 1 sau 9 sau daca jocul e terminat ignora
  if (key < '1' || key > '9' || gameOver) 
    return;

  //transforma pentru a ne raporta la celule 
  //la randurile si coloanele din matricea 3x3
  uint8_t k = key - '1';      
  uint8_t r = k / 3;          
  uint8_t c = k % 3;          

  //daca cel libera pune mutarea jucatorului
  if (board[r][c] == 0) {
    board[r][c] = currentPlayer; 
    redrawAll();
  //dupa mutare verifica daca cineva castiga 
    uint8_t wCells[3][2];
    byte w = checkWin(wCells);

    if (w != 0) { 
      blinkWin(wCells, w);
      resetGame(); 
      return; 
      }
    //daca e remiza
    if (isDraw()) { 
      delay(500);
      resetGame(); 
      return; }
    //modifica jucatorul ca sa dea celalalt data viitoare
    currentPlayer = (currentPlayer == 1) ? 2 : 1;

    if (vsComputer && currentPlayer == 2 && !gameOver) {
      //verifica modul calculator 
      delay(600); 
      //muta calculatorul
      computerMove();
      redrawAll();

    //verifica daca cineva castiga sau daca e remiza
      w = checkWin(wCells);
      if (w != 0) { 
        blinkWin(wCells, w); 
        resetGame(); 
        return; }

      if (isDraw()) { 
        delay(500); 
        resetGame(); 
        return; }

      currentPlayer = 1;
    }
  }
}
