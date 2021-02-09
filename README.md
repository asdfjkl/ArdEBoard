# ArdEBoard

This repository contains source code for an ATmega32U4 to control an electronic chess board. The chess board is based upon reed switches and magnets  glued below every chess piece.

https://raw.githubusercontent.com/asdfjkl/ArdEBoard/main/board_final.jpg

Boards must be based on a ATmega32U4 in order to emulate a USB keyboard. Examples are the Arduino Micro, the Arduino Leonardo, the SparkFun Pro Micro or other clones. When moving a piece, the resulting move is sent in UCI notation (e.g. "e2e4") as a keyboard sequence - many chess programs support that. A good choice for an Android based program (a USB OTG adapter is required is [Art Bik's](https://www.aartbik.com/android.php) ["Chess for Android"](https://play.google.com/store/apps/details?id=com.google.android.chess)). 

More details on how to build such a board can be found at my [blog](https://buildingjerry.wordpress.com/) or [here](https://sites.google.com/site/bergersprojects/home)

# Operation

- When connecting the board, it will wait until all pieces are placed in the initial position. If the board recognizes all pieces to be placed correctly, it will confirm it with a long beep.
- By default, no moves are submitted. After setting up the initial position, lifting up and placing down the white rook on its initial square will trigger move submission for White. Same for the black player.
- For castling moves, only the first move (e.g. "e1g1") is submitted. The next rook move is not submitted.

