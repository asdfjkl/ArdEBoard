# ArdEBoard with tactile switches

I've created an electronic chess board based on tactile switches. It emulates a USB keyboard and sends the moves as key sequences. This works much better in practice than the board based on reed switches (see below), and is also simpler to build and to program.

This repository contains source code for an ATmega32U4 based Arduino to control the electronic chess board.

![ArdEBoard](https://raw.githubusercontent.com/asdfjkl/ArdEBoard/main/board_tactile_final.jpg)

The controller board must be based on a ATmega32U4 in order to emulate a USB keyboard. Examples are 
- the Arduino Micro, 
- the Arduino Leonardo, 
- the SparkFun Pro Micro 
- or other clones. 

More details on how to build such a board can be found at my [blog](https://buildingjerry.wordpress.com/) or [here](https://sites.google.com/site/bergersprojects/home)

A video of playing with the board against Aart Bik's Chess for Android is available [here](https://youtu.be/BVx8kUXQ85c).

## Operation

Just plug in the board and trigger a move by pressing the switch on the source and the target field.

# ArdEBoard with reed switches

Before I've created a board based on reed-switches and magnets glued under every chess piece. Building it is however more complex and expensive, and due to issues w

![ArdEBoard](https://raw.githubusercontent.com/asdfjkl/ArdEBoard/main/board_final.jpg)

The controller board must be based on a ATmega32U4 in order to emulate a USB keyboard. Examples are 
- the Arduino Micro, 
- the Arduino Leonardo, 
- the SparkFun Pro Micro 
- or other clones. 

When moving a piece, the resulting move is sent in UCI notation (e.g. "e2e4") as a keyboard sequence - many PC chess programs support that. As for mobiles, a good choice for an Android based program (a USB OTG adapter is required) is [Art Bik's](https://www.aartbik.com/android.php) [Chess for Android](https://play.google.com/store/apps/details?id=com.google.android.chess)). 

More details on how to build such a board can be found at my [blog](https://buildingjerry.wordpress.com/) or [here](https://sites.google.com/site/bergersprojects/home)

Two videos of playing with the board against the chess GUI [Jerry](https://github.com/asdfjkl/jerry) and ShredderChess are available [here](https://youtu.be/BVx8kUXQ85c) and [here](https://youtu.be/WxEr-5x00cQ).

## Operation

- When connecting the board, it will wait until all pieces are placed in the initial position. If the board recognizes all pieces to be placed correctly, it will confirm it with a long beep.
- By default, no moves are submitted. After setting up the initial position, lifting up and placing down the white king on its initial square will trigger move submission for White. Same for the black player.
- For castling moves, only the first move (e.g. "e1g1") is submitted. The next rook move is not submitted.

