/**
  * @file reset.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for creating a shared memory segment and initializing GameState for lightsout.c
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
#include <sys/ipc.h>
#include <string.h>
#include "common.h"

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message and exit.
static void usage() {
  fprintf( stderr, "usage: reset <board-file>\n" );
  exit( 1 );
}

/**
  * Program starting point. Creates a shared memory space and initializes GameState for lightsout.c
  * @param argc the number of command line arguments
  * @param argv a char pointer to an array of command line arguments
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  // Create key with ftok()
  key_t key = ftok( ".", 1 );

  // check to make sure key was created
  if( key == -1 ){
    fail( "Key could not be created using ftok" );
  }

  // key is 20273507

  if( argc != 2 ) {
    usage();
  }

  // create a file pointer
  FILE *fp = fopen( argv[ 1 ], "r" );

  // check if the file could be opened
  if( !fp ) {
    char * fileName = argv [ 1 ];
    char * message = "Invalid input file: ";
    char * result = strcat( message, fileName );
    fail( result );
  }

  else {
    // create shared memory and initialize board
    int schmid = shmget( key, BLOCK_SIZE, 0666 | IPC_CREAT );

    if( schmid == -1 ) {
      fail( "Can't create shared memory" );
    }

    GameState * state = ( GameState * )shmat( schmid, 0, 0 );
    // declare an int to get each character from passed file
    int ch;
    // row index
    int rowIdx = 0;
    // col index
    int colIdx = 0;
    // loop through contents of file
    while( ( ch = fgetc( fp )) != -1 ) {
      if( colIdx == 4  && ch != '\n') {
        state->previousState[ rowIdx ][ colIdx ] = ch;
        state->currentState[ rowIdx ][ colIdx ] = ch;
        rowIdx++;
        colIdx = 0;
      }

      else if( colIdx < 4  && ch != '\n') {
        state->currentState[ rowIdx ][ colIdx ] = ch;
        state->currentState[ rowIdx ][ colIdx ] = ch;
        colIdx++;
      }
    }

    // set is move to false
    state->isMoved = false;
    
    // Release our reference to the shared memory segment.
    shmdt( state );
    // close file
    fclose( fp );
  }

  // exit successfully
  return 0;
}
