/**
  * @file scrabbleServer.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for creating a server in order to be able to communicate between clients so they can play scrabble against
  * at the same time.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

/** Port number used by my server, original = 28123 */
#define PORT_NUMBER "26198"

/** Maximum word length */
#define WORD_LIMIT 26

/** Our semaphore lock to prevent race conditions */
sem_t lock;

/** Our thread for each client */
pthread_t client;

/** The number of rows in our server table */
int rows;

/** The number of cols in our server table */
int cols;

/** Our scrabble board represented as a pointer to a pointer while will point to each array of values in our scrabble board */
char ** scrabbleBoard;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

/** handle a client connection, close it when we're done. 
  * @param arg the passed arguments which should be our socket which we specified when we create our threads
*/
void *handleClient( void *arg ) {
  // Here's a nice trick, wrap a C standard IO FILE around the
  // socket, so we can communicate the same way we would read/write
  // a file.

  // detach thread
  pthread_detach( client );

  // Get socket
  int * sock = (int *)arg;

  // Open file
  FILE *fp = fdopen( *( sock ), "a+" );
  
  // Prompt the user for a command.
  fprintf( fp, "cmd> " );

  // Temporary values for parsing commands.
  // get up to 36 characters which should be enough space for all valid commands
  char cmd[ 37 ];
  while ( fscanf( fp, "%36[ a-zA-Z0-9]", cmd ) == 1 &&
          strcmp( cmd, "quit" ) != 0 ) {
    // acquire sem lock to protect threads from modifying the board at the same time
    sem_wait( &lock );

    // check for across command, first
    if( strncmp( cmd, "across", strlen( "across" )) == 0 ) {
      // use this to move sscanf forward
      int total = strlen( "across" );
      // our rowVal where we want to insert a word
      int rowVal;
      // our colVal where we want to insert a word
      int colVal;
      // our passed word
      char word[ WORD_LIMIT + 1 ];
      // make sure sscanf is matching all required fields for across command
      int matches = sscanf( cmd + total, "%d%d%26s", &rowVal, &colVal, word );
      // if we have the valid number of fields...
      if( matches == 3 ) {
        // check bounds for a word going from left to right across cols
        if( rowVal <= rows - 1 && colVal <= cols - 1 && strlen( word ) + colVal <= cols ) {
          int len = strlen( word );
          // is the word valid?
          bool valid = false;

          // check for non lowercase letters
          for( int i = 0; i < len; i++ ) {
            if( !( word[ i ] >= 'a' && word[ i ] <= 'z' ) ){
              break;
            }

            else if( i == len - 1 ){
              valid = true;
            }
          }

          // if not valid
          if( !valid ){
            fprintf( fp, "%s\n", "Invalid command");
          }

          else {
            // get total distance our word will go from starting col idx to finish
            int distance = strlen( word ) + colVal;
            // keep track of idx in word char array
            int idx = 0;
            // check for letters that already exist on board and make sure they line up with the user's passed word
            bool valid = false;
            // check for any issue with new words not lining up with current board here
            for( int i = colVal; i < distance; i++ ) {
              if( scrabbleBoard[ rowVal ][ i ] >= 'a' && scrabbleBoard[ rowVal ][ i ] <= 'z' && word[ idx ] != scrabbleBoard[ rowVal ][ i ] ){
                break;
              }

              else if( i == distance - 1 ){
                valid = true;
              }

              idx++;
            }

            // error message
            if( !valid ){
              fprintf( fp, "%s\n", "Invalid command");
            }

            else {
              // otherwise update the board
              idx = 0;
              // here
              for( int i = colVal; i < distance; i++ ) {
                scrabbleBoard[ rowVal ][ i ] = word[ idx++ ];
              }
            }
          }
        }

        else {
          fprintf( fp, "%s\n", "Invalid command");
        }
      }

      else {
        fprintf( fp, "%s\n", "Invalid command" );
      }
    }

    // check for down command, same idea as across.
    else if( strncmp( cmd, "down", strlen( "down" )) == 0 ) {
      int total = strlen( "down" );
      //
      int rowVal;
      //
      int colVal;
      //
      char word[ WORD_LIMIT + 1 ];

      int matches = sscanf( cmd + total, "%d%d%26s", &rowVal, &colVal, word );

      if( matches == 3 ) {
        if( rowVal <= rows - 1 && colVal <= cols - 1 &&  strlen( word ) + rowVal <= rows ) {
          int len = strlen( word );

          bool valid = false;

          for( int i = 0; i < len; i++ ) {
            if( !( word[ i ] >= 'a' && word[ i ] <= 'z' ) ){
              break;
            }

            else if( i == len - 1 ){
              valid = true;
            }
          }

          if( !valid ){
            fprintf( fp, "%s\n", "Invalid command");
          }

          else {
            int distance = strlen( word ) + rowVal;

            int idx = 0;

            bool valid = false;

            for( int i = rowVal; i < distance; i++ ) {
              if( scrabbleBoard[ i ][ colVal ] >= 'a' && scrabbleBoard[ i ][ colVal ] <= 'z' && word[ idx ] != scrabbleBoard[ i ][ colVal ] ){
                break;
              }

              else if( i == distance - 1 ){
                valid = true;
              }

              idx++;
            }
            
            if( !valid ){
              fprintf( fp, "%s\n", "Invalid command");
            }

            else {
              idx = 0;
              
              for( int i = rowVal; i < distance; i++ ) {
                scrabbleBoard[ i ][ colVal ] = word[ idx++ ];
              }
            }
          }
        }

        else {
          fprintf( fp, "%s\n", "Invalid command");
        }
      }

      else {
        fprintf( fp, "%s\n", "Invalid command" );
      }
    }
    // check for board command
    else if( strcmp( cmd, "board" ) == 0 ) {
      // create header for top and bottom of scrabble board
      int headerLen = cols + 2;

      char header[ headerLen ];

      for( int i = 0; i < headerLen; i++ ) {
        if( i == 0 ) {
          header[ i ] = '+';
        }

        else if( i == headerLen - 1 ) {
          header[ i ] = '+';
        }

        else {
          header[ i ] = '-';
        }
      }

      // make sure it is null terminated
      header[ headerLen ] = '\0';
      // print top header
      fprintf( fp, "%s\n", header );

      // print current board putting a bar at the front and end of each row ending each line with a newline
      for( int i = 0; i < rows; i++ ) {
        for( int j = 0; j < cols; j++ ) {
          if( j == 0 ) {
            fprintf( fp, "%c", '|' );
            fprintf( fp, "%c", scrabbleBoard[ i ][ j ] );
          }

          else if( j == cols - 1 ) {
            fprintf( fp, "%c", scrabbleBoard[ i ][ j ] );
            fprintf( fp, "%c", '|' );
            fprintf( fp, "%c", '\n' );
          }

          else {
            fprintf( fp, "%c", scrabbleBoard[ i ][ j ] );
          }
        }
      }

      // print bottom header
      fprintf( fp, "%s\n", header );
    }

    // release sem lock for next thread so it can modify the board
    sem_post( &lock );

    // Prompt the user for the next command.
    fprintf( fp, "cmd> " );
  }

  // Close the connection with this client.
  fclose( fp );
  // Return
  return NULL;
}

/**
  * Create connection between server and clients and create a thread for each client and call start routine to examine commands typed by the user
  * @param argc the number of command line arguments
  * @param argv command line arguments ( pointers to each string )
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  // Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( NULL, PORT_NUMBER, &addrCriteria, &servAddr) )
    fail( "Can't get address info" );

  // Try to just use the first one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Create a TCP socket
  int servSock = socket( servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol);
  if ( servSock < 0 )
    fail( "Can't create socket" );

  // Bind to the local address
  if ( bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) != 0 )
    fail( "Can't bind socket" );
  
  // Tell the socket to listen for incoming connections.
  if ( listen( servSock, 5 ) != 0 )
    fail( "Can't listen on socket" );

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  // Fields for accepting a client connection.
  struct sockaddr_storage clntAddr; // Client address
  socklen_t clntAddrLen = sizeof(clntAddr);

  // check command line arguments, there should be an argument for rows/cols + execute file argument (by default)
  if( argc != 3 ) {
    fail( "usage: scrabbleServer <rows> <cols>");
  }

  // initialize semaphore so it can be used
  sem_init( &lock, 0, 1 );
  // get rows command line argument
  rows = atoi( argv[ 1 ] );
  // get cols command line argument
  cols = atoi( argv[ 2 ] );
  // dynamically allocate memory for our scrabble board ( first allocate memory for points to each row )
  scrabbleBoard = ( char ** )malloc( rows * sizeof( char * ) );
  // allocate memory here as well for each row of the array that point to a specific chunk of the array ( each one of these should be the length of the passed col)
  for( int i = 0; i < rows; i++ ) {
    scrabbleBoard[ i ] = ( char * )malloc( cols * sizeof( char ) );
  }
  // populate initial board with whitespace
  for( int i = 0; i < rows; i++ ) {
    for( int j = 0; j < cols; j++ ) {
      scrabbleBoard[ i ][ j ] = ' ';
    }
  }

  // keep checking user command until we close socket connection from server using ctrl-c
  while ( true ) {
    // Accept a client connection.
    int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
    pthread_create( &client, NULL, handleClient, &sock );
  }

  // first free all memory for each pointer to a row ( that holds all values in a specific row )
  for( int i = 0; i < rows; i++ ) {
    free( scrabbleBoard[ i ] );
  }

  // also free the memory allocated for a pointer to the pointers of the rows
  free( scrabbleBoard );
  
  // destroy semaphore before exiting
  sem_destroy( &lock );

  // Stop accepting client connections (never reached).
  close( servSock );
  
  // return successfully
  return 0;
}
