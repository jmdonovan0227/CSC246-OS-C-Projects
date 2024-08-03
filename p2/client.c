/**
  * @file client.c
  * @author Jake Donovan
  * This file is responsible for sending messages to the client to determine if each option (aka a move, undo, etc) is valid
  * with the current board via the server's response.
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

/**
  * This program is responsible for operating on a game board for lights out game to determine if each move or specified
  * action is valid by communicating with server.c through messsage queues.
  * @param argc the number of command line arguments
  * @param argv a char array of char pointers to each command line argument
  * @return program exit status
*/
int main( int argc, char *argv[] )
{
    return 0;
}