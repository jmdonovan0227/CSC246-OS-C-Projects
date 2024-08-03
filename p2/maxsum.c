/**
  * @file maxsum.c
  * @author Jake Donovan (jmpatte8)
  * Computes the max sum of a range of integers by utilizing multiple cores or "workers"
*/

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: maxsum <workers>\n" );
  printf( "       maxsum <workers> report\n" );
  exit( 1 );
}

// Input sequence of values.
int *vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

/**
  * Read the list of values
*/
void readList() {
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *) malloc( vCap * sizeof( int ) );

  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Grow the list if needed.
    if ( vCount >= vCap ) {
      vCap *= 2;
      vList = (int *) realloc( vList, vCap * sizeof( int ) );
    }

    // Store the latest value in the next array slot.
    vList[ vCount++ ] = v;
  }
}

/**
  * Program starting point, calculates the max sum of a range of values across multiple cores
  * @param argc number of command line arguments
  * @param argv char points to each command line argument
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  bool report = false;
  int workers = 4;

  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();

  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be the word, report
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  readList();

  // You get to add the rest.
  int pfd[ 2 ];

  if( pipe( pfd ) != 0 ) {
    fail("Can't create pipe");
  }

  // vCount = num of elements
  // vList = contents of list
  // max value across all children
  // max val
  int max = 0;

  for( int i = 0; i < workers; i++ ) {
    // create a child process
    pid_t id = fork();
    // keep track of the current sum for each worker
    int currentSum = 0;
    // worker max
    int workerMax = 0;

    if( id == 0 ) {
      // Close reading end of pipe
      close( pfd[ 0 ] );
      
      for( int j = i; j < vCount; j += workers ) {
        for( int k = j; k < vCount; k++ ) {
           currentSum += vList[ k ];
           
           if( currentSum > workerMax ) {
            workerMax = currentSum;
          }
        }

        // reset current sum
        currentSum = 0;
      }
      
      // lock file before writing to pipe
      lockf( pfd[ 0 ], F_LOCK, 0 );
      // write result to parent process
      write( pfd[ 1 ], &workerMax, sizeof( int ) );
      // unlock file
      lockf( pfd[ 0 ], F_ULOCK, 0 );
      
      if( report ){
        printf( "I'm process " );
        printf( "%d", (int)getpid() );
        printf( ". The maximum sum I found is " );
        printf( "%d", workerMax );
        printf( ".\n" );
      }

      // exit
      exit( 0 );
    }
  }

  // Determine the max value by waiting for each process to finish
  // and comparing each local max that was written in the pipe
  while( wait( NULL ) != -1 ) {
      close( pfd[ 1 ] );
      int localMax = 0;
      // read from pipe
      read( pfd[ 0 ], &localMax, sizeof( int ));
      // check local worker max vs max
      if( localMax > max ) {
        max = localMax;
      }
  }

  // Print max sum
  printf("Maximum Sum: %d\n", max );

  // return successfully
  return EXIT_SUCCESS;
}
