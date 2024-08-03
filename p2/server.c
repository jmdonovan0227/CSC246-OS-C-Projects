/**
  * @file server.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for reprompting client for messages until we receive a SIGNAL to close the server
  * this file will use two message queues for receiving messages from the client and sending responses back to the client
*/

#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Flag for telling the server to stop running because of a sigint.
// This is safer than trying to print in the signal handler.
static int running = 1;

/**
  * Creates two servers for receiving messages from client and sending responses to client via message queues
  * @param argc the number of command line arguments
  * @param argv all command line arguments as strings
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;

  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open( SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr );
  mqd_t clientQueue = mq_open( CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr );
  
  if ( serverQueue == -1 || clientQueue == -1 )
    fail( "Can't create the needed message queues" );

  char msgPtr[ MESSAGE_LIMIT ];

  // Repeatedly read and process client messages.
  while ( running ) {
    int len = mq_receive( serverQueue, msgPtr, sizeof( msgPtr ), NULL );

    if( len > 0 ){
      printf("GOT MESSAGE!");

      for( int i = 0; i < len; i++ ) {
        printf( "%c", msgPtr[ i ] );
      }

      printf("\n");
    }

    else {
      //
    }
  }

  // Close our two message queues (and delete them).
  mq_close( clientQueue );
  mq_close( serverQueue );

  mq_unlink( SERVER_QUEUE );
  mq_unlink( CLIENT_QUEUE );

  return 0;
}
