#include <Keyboard.h>

// outcomment one of the above
// a) debug via serial (for developing / testing)
// #define BOARD_DEBUG

// b) create a virtual USB keyboard (for final use)
#define USB_KEYBOARD

// the arduino micro pin connections
// layout depends on the wiring
// for the files, ranks and the ky-012 buzzer
int FILE_A = 9;
int FILE_B = 8;
int FILE_C = 7;
int FILE_D = 6;
int FILE_E = 5;
int FILE_F = 4;
int FILE_G = 3;
int FILE_H = 2;

int RANK_8 = 21;
int RANK_7 = 20;
int RANK_6 = 19;
int RANK_5 = 18;
int RANK_4 = 15;
int RANK_3 = 14;
int RANK_2 = 16;
int RANK_1 = 10;


// array of ranks and files
int FILES[8] = {FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H};
int RANKS[8] = {RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8};


// holds the current state of the buttons and records
// if a button was pressed
bool STATUS[8][8] = {
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false},
    {false, false, false, false, false, false, false, false}};


void setup()
{
  // put your setup code here, to run once:

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


void loop()
{
  delay(100);

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
      }
      if (val == LOW)
      {
        // val == LOW: field is occupied
        // but we need to make sure that
        // we send the field info only once
        // even if the user presses the button
        // for a longer time period        
        if(!STATUS[i][j]) {
          // button was _not_ pressed before
          // first reset all other states
          for( int k=0;k<8;k++) {
            for( int l=0;l<8;l++) {
              STATUS[k][l] = false;
            }
          }
          // set button status
          STATUS[i][j] = true;
          char s[] = "a1\n";
          s[0] = 97 + j;
          s[1] = 49 + i;
          
          #ifdef USB_KEYBOARD
            for (int m = 0; m < 2; m++)
            {
              Keyboard.write(s[m]);
            }
          #endif
          #ifdef BOARD_DEBUG
            Serial.println("button press");    
            Serial.println(s);    
          #endif          
        } // no else: if STATUS[i][j] is true, we already sent the command once previously        
      }
    }
    digitalWrite(rank_i, HIGH);
  }

}
