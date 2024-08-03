/**
  * @file lightsout.c
  * @author Jake Donovan (jmpatte8)
  * This class is able to accept commands for move, undo, exit, report, and test for lightsout game
*/

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "common.h"

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Make a move at the given row, column location, returning true
// if successful.
bool move( GameState *state, int r, int c ) {
  if( r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE ) {
    // save current state as previous state
    for( int i = 0; i < GRID_SIZE; i++ ) {
      for( int j = 0; j < GRID_SIZE; j++ ) {
        state->previousState[ i ][ j ] = state->currentState[ i ][ j ];
      }
    }

    // set isMoved to true everywhere

    // top  left corner
    if( r == 0 && c == 0 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    //  top right corner
    else if( r == 0 && c == 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1 ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // top center rows
    else if( r == 0 && c > 0 && c < 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1 ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // bottom left corner
    else if( r == 4 && c == 0 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // bottom right corner
    else if( r == 4 && c == 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1 ] = '.';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // bottom middle rows
    else if( r == 4 && c > 0 && c < 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1  ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // far left center rows
    else if( r > 0 && r < 4 && c == 0 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // center of board
    else if( r > 0 && r < 4 && c > 0 && c < 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1 ] = '.';
      }

      if( state->currentState[ r ][ c + 1 ] == '.' ) {
        state->currentState[ r ][ c + 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c + 1 ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    // far right center rows
    else if( r > 0 && r < 4 && c == 4 ) {
      if( state->currentState[ r ][ c ] == '.' ) {
        state->currentState[ r ][ c ] = '*';
      }

      else {
        state->currentState[ r ][ c ] = '.';
      }

      if( state->currentState[ r ][ c - 1 ] == '.' ) {
        state->currentState[ r ][ c - 1 ] = '*';
      }

      else {
        state->currentState[ r ][ c - 1 ] = '>';
      }

      if( state->currentState[ r - 1 ][ c ] == '.' ) {
        state->currentState[ r - 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r - 1 ][ c ] = '.';
      }

      if( state->currentState[ r + 1 ][ c ] == '.' ) {
        state->currentState[ r + 1 ][ c ] = '*';
      }

      else {
        state->currentState[ r + 1 ][ c ] = '.';
      }

      state->isMoved = true;
      return true;
    }

    else {
      return false;
    }
  }

  else {
    return false;
  }
}

// Undo the most recent move, returning true if successful.
bool undo( GameState *state ) {
  if( state->isMoved ) {
    for( int i = 0; i < GRID_SIZE; i++ ) {
      for( int j = 0; j < GRID_SIZE; j++ ) {
        state->currentState[ i ][ j ] = state->previousState[ i ][ j ];
      }
    }

    state->isMoved = false;
    return true;
  }

  else {
    return false;
  }
}

// Print the current state of the board.
void report( GameState *state ) { 
  // THIS WILL WORK FOR REPORT
  for( int i = 0; i < GRID_SIZE; i++ ) {
    for( int j = 0; j < GRID_SIZE; j++ ) {
      printf("%c", state->currentState[ i ][ j ]);
    }
    printf("\n");
  }
}

// Test interface, for quickly making a given move over and over.
bool test( GameState *state, int n, int r, int c ) {
  // Make sure the row / colunn is valid.
  if ( r < 0 || r >= GRID_SIZE || c < 0 || c >= GRID_SIZE )
  return false;
  
  // Make the same move a bunch of times.
  for ( int i = 0; i < n; i++ )
  move( state, r, c );
  return true;
}


//
bool exitFunction( int schmid ) {
  // Tell the OS we no longer need the segment.
  shmctl( schmid, IPC_RMID, 0 );
  // return true
  return true;
}


/**
  * 
  * @param argc the number of command line arguments 
  * @param argv a char pointer to command line arguments 
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  // Retrieve shared memory
  int schmid = shmget( ftok( ".", 1 ), BLOCK_SIZE, 0666 );
  
  // Check schmid
  if( schmid == -1 ){
    fail( "error" );
  }

  // Get shared memory game state
  GameState * state = ( GameState * )shmat( schmid, 0, 0 );

  // Check state
  if( state == ( GameState *)-1 ){
    fail( "error" );
  }
  
  if( argc < 2 ) {
    fail( "error" );
  }

  // most likely a undo, report, or exit command
  if( argc ==  2 ) {
    // undo command
    if( strcmp( argv[ 1 ], "undo" ) == 0 ) {
      // if a move commmand has been performed
      if( undo( state ) ) {
        printf( "success\n" );
      }

      // otherwise no
      else {
        fail( "error" );
      }
    }

    else if( strcmp( argv[ 1 ], "report" ) == 0 ) {
      report( state );
    }

    else if( strcmp( argv[ 1 ], "exit" ) == 0 ) {
      if( exitFunction( schmid ) ) {
          // print success
          printf("success\n");
      }
    }

    else {
      fail( "error" );
    }
  }

  // most likely a move command
  else if( argc == 4 ) {
    // use sscanf() to convert and check values
    if( strcmp( argv[ 1 ], "move" ) == 0 ) {
      // for arg at 2 and arg at 3
      int r = 0;
      int c = 0;
      int matches = sscanf( argv[ 2 ], "%d", &r );

      if( matches == 1 ) {
        matches = sscanf( argv[ 3 ], "%d", &c );
        if( matches != 1 ) {
          fail( "error" );
        }
      }

      else {
        fail( "error" );
      }

      if( move( state, r, c )) {
        printf( "success\n" );
      }

      else {
        fail( "error" );
      }
    }

    else {
      fail( "error" );
    }
  }

  // most likely the test command
  else if( argc == 5 ) {
    //
    if( strcmp( argv[ 1 ], "test" ) == 0 ) {
      int n = 0;
      int r = 0;
      int c = 0;
      int matches = sscanf( argv[ 2 ], "%d", &n );

      if( matches == 1 ){
        matches = sscanf( argv[ 3 ], "%d", &r );

        if( matches == 1 ) {
          matches = sscanf( argv[ 4 ], "%d", &c );

          if( matches != 1 ) {
            fail( "error" );
          }
        }

        else {
          fail( "error" );
        }
      }

      else {
        fail( "error" );
      }

      if( test( state, n, r, c ) ) {
        printf( "success\n" );
      }

      else {
        fail( "error" );
      }
    }

    else {
      fail( "error" );
    }
  }

  // we didn't get the right number of arguments for a valid command
  else {
    fail( "error" );
  }

  // return successfully
  return 0;
}
