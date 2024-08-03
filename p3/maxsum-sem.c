/**
  * @file maxsum-sem.c
  * @author Jake Donovan (jmpatte8)
  * This class is responsible for utilizing threads and semaphores to find the maximum sum from lists of integers. With the use of semaphores
  * we are able to protect our values so we can correctly modify and protect our values to return the maximum sum
  * received help in Yuheng's office hours from 2 - 4 on 03-08-2023 to help resolve errors in file
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/syscall.h>

/**
  * Prints param error message and exits unsuccessfully
  * @param message the passed error message
*/
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

/**
  * Prints error messages and exits unsuccessfully
*/
static void usage() {
  printf( "usage: maxsum-sem <workers>\n" );
  printf( "       maxsum-sem <workers> report\n" );
  exit( 1 );
}

// True if we're supposed to report what we find.
bool report = false;

// Maximum sum we've found.
int max_sum = INT_MIN;

// Keeps track of when we have input to process
sem_t available;
// Protects access to idx when called getWork()
sem_t getIdx;
// Protects max_sum so only one worker can modify this value at a time
sem_t updateSum;
// Protects track, our worker idx variable when updating, so only one worker gets an idx at a time
sem_t protectIdx;

// Fixed-sized array for holding the sequence.
#define MAX_VALUES 500000
int vList[ MAX_VALUES ];

// Current number of values on the list.
int vCount = 0;

// helps us dispatch an idx to a worker
int track = 0;

// keeps track of the number of workers we are using. This will be used in readList() after reading all input.
int num_workers = 0;

/**
  * Read our list of values
*/
void readList() {
  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Make sure we have enough room, then store the latest input.
    if ( vCount > MAX_VALUES )
      fail( "Too many input values" );

    // Store the latest value.
    vList[ vCount++ ] = v;

    //
    sem_post( &available );
  }

  for( int i = 0; i < num_workers; i++ ) {
      sem_post( &available );
  }
}

/**
  * Gets worker indices for each worker, returns the worker idx if there is more work or -1 if we are done processing
  * @return the worker indices
*/
int getWork() {
  // wait for input
  sem_wait( &available );
  // protect our track worker idx when updating
  sem_wait( &protectIdx );
  track++;
  sem_post( &protectIdx );

  // keep iterating until we hit the end of input
  if( track < vCount ) {
    return track;
  }

  // return -1 to signify that we are done
  else {
    return -1;
  }
}

/**
  * Start routine for each worker
  * @param arg the passed arguments
*/
void *workerRoutine( void *arg ) {
  sem_wait( &getIdx );
  // protect idx so it can only be accessed by one worker at a time
  int idx = getWork();
  sem_post( &getIdx );
  int localMax = 0;

  // keep looping until we get -1 which tells us we are done
  while( idx != -1 ) {
    int currentValue = 0;
    for( int i = idx; i >= 0; i-- ){
      currentValue += vList[ i ];
      if( currentValue > localMax ) {
        localMax = currentValue;
      }
    }

    // compare local max to global max_sum make sure to protect the max_sum so it doesn't get modified by more than one
    // worker at a time
    if( localMax > max_sum ) {
      sem_wait( &updateSum );
      max_sum = localMax;
      sem_post( &updateSum );
    }

    // get the next idx
    sem_wait( &getIdx );
    idx = getWork();
    sem_post( &getIdx );
  }

  
    if( report ) {
      printf("I’m thread " );
      printf( "%ld", pthread_self());
      printf( ". The maximum sum I found is " );
      printf( "%d", localMax );
      printf("\n");
    }

  return NULL;
}

/**
  * Program starting point. Gets workers, initializes all required semaphores, creates all worker threads, waits for them to terminate, and
  * prints the maximum sum found at the end.
  * @param argc the number of command line arguments
  * @param argv a char pointer to an array holding each command line argument
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  int workers = 4;
  
  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();
  
  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be "report"
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  // set the number of workers
  num_workers = workers;

  sem_init( &available, 0, 0 );

  sem_init( &getIdx, 0, 1 );

  sem_init( &updateSum, 0, 1 );

  sem_init( &protectIdx, 0, 1 );

  // Make each of the workers.
  pthread_t worker[ workers ];
  for ( int i = 0; i < workers; i++ )
     if ( pthread_create( &worker[ i ], NULL, workerRoutine, NULL ) != 0 ) {
           fail( "Can't create worker" );
     }

  // Then, start getting work for them to do.
  readList();

  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ ) {
        pthread_join( worker[ i ], NULL );
  }

  sem_destroy( &available );
  sem_destroy( &getIdx );
  sem_destroy( &updateSum );
  sem_destroy( &protectIdx );

  // Report the max product and release the semaphores.
  printf( "Maximum Sum: %d\n",  max_sum );
  
  // exit successfully
  return EXIT_SUCCESS;
}