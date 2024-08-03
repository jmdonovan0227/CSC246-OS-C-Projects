/**
  * @file common.h
  * @author Jake Donovan (jmpatte8)
  * Header file that defines GameState struct and all necessary macros for lightsout.c and reset.c
*/

// Height and width of the playing area.
#define GRID_SIZE 5

// Size of the shared block of memory.
#define BLOCK_SIZE 1024

// Define GameState struct
struct GameStateStruct {
    // need to keep track of when we perform a move command
    bool isMoved;
    // need this for undo move
    char previousState[ GRID_SIZE ][ GRID_SIZE ];
    // current board
    char currentState[ GRID_SIZE ][ GRID_SIZE ];
};

/** Typedef GameState */
typedef struct GameStateStruct GameState;
