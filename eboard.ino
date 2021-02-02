#include <Keyboard.h>

// outcomment one of the above
// a) debug via serial (for developing / testing)
//#define BOARD_DEBUG

// b) create a virtual USB keyboard (for final use)
#define USB_KEYBOARD

// the arduino micro pin connections
// layout depends on the wiring
// for the files, ranks and the ky-012 buzzer
int BUZZER = 1;
int RANK_8 = 2;
int RANK_7 = 3;
int RANK_6 = 4;
int RANK_5 = 5;
int RANK_4 = 6;
int RANK_3 = 7;
int RANK_2 = 8;
int RANK_1 = 9;

int FILE_A = 21;
int FILE_B = 20;
int FILE_C = 10;
int FILE_D = 19;
int FILE_E = 18;
int FILE_F = 15;
int FILE_G = 14;
int FILE_H = 16;

// array of ranks and files
int FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
int RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};

// we record for each field wether 1) there is a pawn or a piece
// and 2) the color of the piece
// 1) is to check for promotions and not submit them (currently not implemented)
// and 2) is to identify captures
int EMPTY = 0;
int WHITE_PAWN = 1;
int WHITE_PIECE = 2;
int BLACK_PAWN = 3;
int BLACK_PIECE = 4;

// tracking the current turn
bool WHITE = false;
bool BLACK = true;
bool CURRENT_TURN = WHITE;

// trigger to identify wether the board was just turned on
// in that case we wait until the pieces are placed in the initial position
bool STARTING_UP = true;

// to track castling rights
bool WHITE_MAY_CASTLE = true;
bool BLACK_MAY_CASTLE = true;

bool INITIAL_POS[8][8] = {
    {true, true, true, true, true, true, true, true},
    {true, true, true, true, true, true, true, true},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {true, true, true, true, true, true, true, true},
    {true, true, true, true, true, true, true, true}};

// holds the previously recorded position
bool OLD_POS[8][8] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}};

// holds the currently recorded position
bool CURRENT_POS[8][8] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}};

// tracks the current _chess_ position of the boards, i.e.
// piece vs pawn and color of the piece
int CURRENT_CHESS_POSITION[8][8] = {
    {WHITE_PIECE, WHITE_PIECE, WHITE_PIECE, WHITE_PIECE, WHITE_PIECE, WHITE_PIECE, WHITE_PIECE, WHITE_PIECE},
    {WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
    {BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN},
    {BLACK_PIECE, BLACK_PIECE, BLACK_PIECE, BLACK_PIECE, BLACK_PIECE, BLACK_PIECE, BLACK_PIECE, BLACK_PIECE}};

// returns the color of the piece at the given x,y location
// return -1, if the field is empty
int getColor(int x, int y)
{
  int c = CURRENT_CHESS_POSITION[x][y];
  if (c == 1 || c == 2)
  {
    return WHITE;
  }
  if (c == 3 || c == 4)
  {
    return BLACK;
  }
  return -1;
}

// scans the current state of the reed sensor matrix
// and stores the result in CURRENT_POS
// sets true in CURRENT_POS if there is a piece
void scanPosition()
{
  for (int i = 0; i < 8; i++)
  {
    int rank_i = RANKS[i];
    digitalWrite(rank_i, LOW);
    for (int j = 0; j < 8; j++)
    {
      int file_j = FILES[j];
      int val = digitalRead(file_j);
      if (val == HIGH)
      {
        // val == HIGH: field is empty
        CURRENT_POS[i][j] = false;
      }
      if (val == LOW)
      {
        // val == LOW: field is occupied
        CURRENT_POS[i][j] = true;
      }
    }
    digitalWrite(rank_i, HIGH);
  }
}

// compares the old and current state of the 
// reed sensor matrix.
// returns:
// 0 if there is no change
// -1 if a piece/pawn was _removed_
// +1 if a piece/pawn was _added_
// the x,y location of that change
// is stored in the pointer vars x,y
// if there are multiple changes, it
// returns the _first_ change that is detected
// will buzz if a change is detected
int compareOldCurrent(int *x, int *y)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (CURRENT_POS[i][j] && !OLD_POS[i][j])
      {
        // a piece was added.
        *x = i;
        *y = j;
        buzz(1);
        return 1;
      }
      if (!CURRENT_POS[i][j] && OLD_POS[i][j])
      {
        // a piece was removed.
        *x = i;
        *y = j;
        buzz(1);
        return -1;
      }
    }
  }
  // we scanned everything, and the current
  // and old position are the same
  return 0;
}

// copies the array of the current position
// to the array of the previous position
void updateOldPosition()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      OLD_POS[i][j] = CURRENT_POS[i][j];
    }
  }
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(BUZZER, OUTPUT);

  /*
  Approach that was used in Berger's Solus chess. It requires that the diodes point correctly (cf. wiring diagram)
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  - Connect the rows to 8 pins of the microcontroller, setting them as OUTPUT, and initializing them all to '1' (High).
  - Connect the columns to 8 pins of the microcontroller, setting them all as INPUT with pull-up resistor enabled.
  - Set to '0' (Low) one row and read the value of all columns. Do the same with the other seven rows, one by one.
  - In every full sampling of the board (of the 8 rows), we obtain a matrix of 8 x 8 bits.
  - Detecting changes that occur in the matrix at every sampling, we can know what piece was moved.
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
  pinMode(RANK_1, OUTPUT);
  pinMode(RANK_2, OUTPUT);
  pinMode(RANK_3, OUTPUT);
  pinMode(RANK_4, OUTPUT);
  pinMode(RANK_5, OUTPUT);
  pinMode(RANK_6, OUTPUT);
  pinMode(RANK_7, OUTPUT);
  pinMode(RANK_8, OUTPUT);

  pinMode(FILE_A, INPUT_PULLUP);
  pinMode(FILE_B, INPUT_PULLUP);
  pinMode(FILE_C, INPUT_PULLUP);
  pinMode(FILE_D, INPUT_PULLUP);
  pinMode(FILE_E, INPUT_PULLUP);
  pinMode(FILE_F, INPUT_PULLUP);
  pinMode(FILE_G, INPUT_PULLUP);
  pinMode(FILE_H, INPUT_PULLUP);
#ifdef BOARD_DEBUG
  Serial.begin(115200);
  Serial.println("Arduino Chess Board");
#endif
#ifdef USB_KEYBOARD
  Keyboard.begin();
#endif
}

// buzzes several _times_
void buzz(int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(BUZZER, HIGH);
    delay(10);
    digitalWrite(BUZZER, LOW);
  }
}

// for debugging: prints the current
// state of the reed sensor matrix
// i.e. the array CURRENT_POS formatted
// via the serial interface
void printBoard()
{
#ifdef BOARD_DEBUG
  Serial.println("board: ");
#endif
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (CURRENT_POS[i][j])
      {
#ifdef BOARD_DEBUG
        Serial.print("1");
#endif
      }
      else
      {
#ifdef BOARD_DEBUG
        Serial.print("0");
#endif
      }
    }
#ifdef BOARD_DEBUG
    Serial.print("\n");
#endif
  }
#ifdef BOARD_DEBUG
  Serial.println("");
#endif
}

// checks if the curent state of the reed sensor matrix
// corresponds to the initial board position
// only checks LOW/HIGH of the reed sensor, no chess logic
bool isInitial()
{
  bool foundInitial = true;
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      if (CURRENT_POS[i][j] != INITIAL_POS[i][j])
      {
        foundInitial = false;
        break;
      }
    }
  }
  return foundInitial;
}

// waits until the user has set up the initial
// board position
void waitForInitialPos()
{
  bool foundInitial = false;
  while (!foundInitial)
  {
    delay(50);
    scanPosition();
    int x = 0;
    int y = 0;
    int cmp = compareOldCurrent(&x, &y);
    if (cmp != 0)
    {
#ifdef BOARD_DEBUG
      if (cmp > 0)
      {
#ifdef BOARD_DEBUG
        Serial.println("piece placed\n");
#endif
      }
      if (cmp < 0)
      {
#ifdef BOARD_DEBUG
        Serial.println("piece picked up\n");
#endif
      }
#endif
    }
    updateOldPosition();
    // now check if we have the initial position
    foundInitial = isInitial();
    //printBoard();
  }
}


// some variables to store
// the location and type of the piece
// that a user has picked up
int pickUpX = -1;
int pickUpY = -1;
int pickUpPieceType = -1;

// to trigger if the user has entered a castling 
// move, e.g. King from e1 to g1. In that case,
// this variable helps to ensure that the following 
// Rook move from h1 to f1 will not 
// be submitted. 
bool justCastled = false;

// by default, _NO_ moves are output, neither by white
// nor by black. To enable move output by one side, lift 
// the (white/black) King up and down to toggle between
// move output for (white/black)
bool printWhiteMoves = false;
bool printBlackMoves = false;

void loop()
{
  delay(100);

  if (STARTING_UP)
  {

    waitForInitialPos();
#ifdef BOARD_DEBUG
    Serial.println("initial position setup complete\n");
#endif
    STARTING_UP = false;
    buzz(5);
    CURRENT_TURN = WHITE;
  }

  // configure board, i.e. white, black or both moves are output
  // general approach
  // loop
  // - look for a piece that is picked up
  //   - if the piece is of the enemy color, we have a capture
  //     - update the current chess position, remove the piece
  //   - if we have a piece of our own color, record the source field
  //     - update the current chess position, remove the piece
  //     - also record wether it's a pawn or a piece by updating CURRENT_CHESS_POS
  // - look for a piece that is placed
  //   - if we have a valid source field
  //     - if it's a king move, check if that's a castling move
  //       - if so, set justCastled to true, and make sure that we don't change the TURN
  //         and don't output the next (rook) move
  //       - disable ability to castle (WHITE_MAY_CASTLE / BLACK_MAY_CASTLE)
  //     - if not a king move:
  //       - if not just castled:
  //         - output the move
  //   - update the current chess position

  scanPosition();

  /* TODO scan constantly to check
     if the board is re-set to the initial position
     right now, user has to dis/re-connect USB cable 
     to reset

  if(isInitial()) {
    if(!wasInInitial) {
    // reset everything
    pickUpX = -1;
    pickUpY = -1;
    pickUpPieceType = -1;
    CURRENT_TURN = WHITE;
    justCastled = false;
    wasInInitial = true;
    updateOldPosition();
    buzz(5);
    Serial.println("reset to initial\n");
    //printBoard();
    }
  } else {
    wasInInitial = false;
  }
  */

  int x = -1;
  int y = -1;
  int cmp = compareOldCurrent(&x, &y);
  int color = getColor(x, y);
  
  // case1: enemy piece was captured and thus picked up
  if (cmp == -1 && color != CURRENT_TURN)
  {
    CURRENT_CHESS_POSITION[x][y] = EMPTY;
#ifdef BOARD_DEBUG
    Serial.println("case 1: enemy piece is picked up");
    Serial.println("xy:");
    Serial.println(x);
    Serial.println(y);
#endif
    updateOldPosition();
  }

  // case 2: own piece is picked up, i.e. start of a move
  if (cmp == -1 && color == CURRENT_TURN)
  {
    pickUpX = x;
    pickUpY = y;
    pickUpPieceType = CURRENT_CHESS_POSITION[x][y];
    CURRENT_CHESS_POSITION[x][y] = EMPTY;
#ifdef BOARD_DEBUG
    Serial.println("case 2: own piece is picked up");
    Serial.println("xy:");
    Serial.println(x);
    Serial.println(y);
#endif
    updateOldPosition();
  }

  // case 3: piece is put down, i.e. end of a move
  if (cmp == 1)
  {
#ifdef BOARD_DEBUG
    Serial.println("case 3: piece is put down");
    Serial.println("xy:");
    Serial.println(x);
    Serial.println(y);
    Serial.println("pickup piece x,y location and type:");
    Serial.println(pickUpX);
    Serial.println(pickUpY);
    Serial.println(pickUpPieceType);
#endif
    if (pickUpX >= 0 && pickUpY >= 0 && pickUpPieceType > 0)
    {
      CURRENT_CHESS_POSITION[x][y] = pickUpPieceType;
      char s[] = "a1a1\n";
      s[0] = 97 + pickUpY;
      s[1] = 49 + pickUpX;
      s[2] = 97 + y;
      s[3] = 49 + x;

      // don't send if piece was just picked up and placed down
      if (!(s[0] == s[2] && s[1] == s[3]))
      {
#ifdef BOARD_DEBUG
        Serial.println("not just up down");
        Serial.println("current turn, false = white:");
        Serial.println(CURRENT_TURN);
        Serial.println("just castled:");
        Serial.println(justCastled);
#endif
        if (!justCastled)
        {
          if (printWhiteMoves && CURRENT_TURN == WHITE)
          {
#ifdef BOARD_DEBUG
            Serial.println(s);
#endif
#ifdef USB_KEYBOARD
            for (int k = 0; k < 4; k++)
            {
              Keyboard.write(s[k]);
            }
#endif
          }
          if (printBlackMoves && CURRENT_TURN == BLACK)
          {
#ifdef BOARD_DEBUG
            Serial.println(s);
#endif
#ifdef USB_KEYBOARD
            for (int k = 0; k < 4; k++)
            {
              Keyboard.write(s[k]);
            }
#endif
          }
        }
        else
        {
          justCastled = false;
        }

        if (WHITE_MAY_CASTLE && CURRENT_TURN == WHITE && pickUpY == 4 && pickUpX == 0)
        {
#ifdef BOARD_DEBUG
          Serial.println("white moves king");
#endif
          // handle castling or moving of king
          WHITE_MAY_CASTLE = false;
          if ((y == 6 && x == 0) || (y == 2 && x == 0))
          {
            justCastled = true;
            if (printWhiteMoves)
            {
#ifdef BOARD_DEBUG
              Serial.println(s);
#endif
#ifdef USB_KEYBOARD
              for (int k = 0; k < 4; k++)
              {
                Keyboard.write(s[k]);
              }
#endif
            }
            // switch turn here. it is (again) switched down below
            // so we keep the current turn, i.e. player will be able
            // to do the rook move of the castle
            CURRENT_TURN = !CURRENT_TURN;
          }
        }
        if (BLACK_MAY_CASTLE && CURRENT_TURN == BLACK && pickUpY == 4 && pickUpX == 7)
        {
#ifdef BOARD_DEBUG
          Serial.println("black moves king");
#endif
          // handle castling or moving of king
          BLACK_MAY_CASTLE = false;
          if ((y == 6 && x == 7) || (y == 2 && x == 7))
          {
            justCastled = true;
            if (printBlackMoves)
            {
#ifdef BOARD_DEBUG
              Serial.println(s);
#endif

#ifdef USB_KEYBOARD
              for (int k = 0; k < 4; k++)
              {
                Keyboard.write(s[k]);
              }
#endif
            }
            // switch turn here. it is (again) switched down below
            // so we keep the current turn, i.e. player will be able
            // to do the rook move of the castle
            CURRENT_TURN = !CURRENT_TURN;
          }
        }
        CURRENT_TURN = !CURRENT_TURN;
      }
      // if piece was just picked up and placed down, check if it was the e1 or
      // e8 square. in that case, activate/deactivate outputting moves for that side
      if ((s[0] == s[2] && s[1] == s[3]))
      {
#ifdef BOARD_DEBUG
        Serial.println("JUST up down");
#endif
        if (pickUpY == 4 && pickUpX == 0)
        {
#ifdef BOARD_DEBUG
          Serial.println("setting output move white to");
          Serial.println(!printWhiteMoves);
#endif
          printWhiteMoves = !printWhiteMoves;
        }
        if (pickUpY == 4 && pickUpX == 7)
        {
#ifdef BOARD_DEBUG
          Serial.println("setting output move black to");
          Serial.println(!printBlackMoves);
#endif
          printBlackMoves = !printBlackMoves;
        }
      }
      updateOldPosition();
      int pickUpX = -1;
      int pickUpY = -1;
      int pickUpPieceType = -1;
    }
    else
    {
      // something went wrong, reset everything
      pickUpPieceType = -1;
      pickUpX = -1;
      pickUpY = -1;
#ifdef BOARD_DEBUG
      Serial.println("something went wrong. resetting");
#endif
      updateOldPosition();
    }
  }

}
