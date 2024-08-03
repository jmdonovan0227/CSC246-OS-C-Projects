/**
  * @file stash.c
  * @author Jake Donovan (jmpatte8)
  * Our own shell program which will be used to read and process
  * user commands
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/** A user can enter a maximum command length of 1024 and we need plus 1 for null termination */
#define MAX_LINE_LEN 1025

/** Minimum length for our words pointer array to hold all words from a 1024 character line */
#define WORDS_MIN_LEN 513

/** Minimum command-line arguments */
#define MIN_ARGS 2
/**
  * Parse a user's command and individual words
  * @param line the user command line as a string
  * @param words a list of pointers to words in the user's commands
*/
int parseCommand( char * line, char *words[] )
{
  // current position on line 
  int pos = 0;
  // current word's length
  int wLength = 0;
  // number of words we have read 
  int count = 0;
  // the index we need to place our new word in words
  int idx = 0;
  // the position of the first character of a word we want to have a pointer to
  int savePos = 0;
  
  while( *( pos + line ) != '\0' && *( pos + line ) != '\n'){
    if( *( pos + line ) == ' ') {
      // we have a word
      if( wLength > 0 ){
        *( pos + line ) = '\0';
        count++;
        words[ idx++ ] = ( savePos + line );
        pos++;
        wLength = 0;
      }
      
      // we are just reading ws
      else {
        *( pos + line ) = '\0';
        pos++;
      }
   }

   else {
    if( wLength == 0 ) {
      savePos = pos;
    }

    wLength++;
    pos++;
   }
 }

 if( *(pos + line ) == '\n' ){
  // get rid of newline with a null term
  // helpful in exit function case when converting
  // our exit status number to an integer
  *(pos + line ) = '\0';
  // set final word
  words[ idx ] = ( savePos + line );
  // update count
  count++;
 }

 else {
  // set final word
  words[ idx ] = ( savePos + line );
  // update count
  count++;
 }

 // return count
 return count;
}

/**
  * Run an exit command from the user
  * @param words a list of pointers to words in the user's commands 
  * @param count the number of words in the words array
*/
void runExit( char *words[], int count )
{
  int status = atoi( words[ 1 ] );
  exit( status );
}

/**
  * Run a cd command received from the user
  * @param words a list of pointers to words in the user's commands
  * @param count the number of words in the words array
*/
void runCd( char * words[], int count )
{
  if( count == MIN_ARGS ) {
    // one command, and one argument is valid
    int ret = chdir( words[ count - 1 ]);

    // check if there was an error with the path
    if( ret == -1 ) {
      // throw an error
      fprintf( stdout, "Invalid command\n");
    }
  }

  else {
    // not valid, throw an error
    fprintf( stdout, "Invalid command\n");
  }
}

/**
  * Run a non-built in command received from the user
  * @param words a list of pointers to words in the user's commands
  * @param count the number of words in the words array
*/
void runCommand( char *words[], int count )
{
    // make a child process
  pid_t id = fork();

  // checck if call to fork succeeded
  if( id == -1 ) {
    // couldn't create child
    fprintf( stdout, "Can't run command ");
    fprintf( stdout, "%s", words[ 0 ] );
    fprintf( stdout, "\n");
  }

  if( id == 0 ) {
    // set words[ count ] to NULL
    // so execvp can determine the number of command-line arguments
    words[ count ] = NULL;
    int valid = execvp( words[ 0 ], words );

    if( valid == -1 ){
      fprintf( stdout, "Can't run command ");
      fprintf( stdout, "%s", words[ 0 ] );
      fprintf( stdout, "\n");
    } 
  }

  else {
    // we are the parent, so wait for the child to finish
    wait( NULL );
  }
}

/**
  * Program starting point, prompts user for input
  * and passes to all methods to read and process commands received from the terminal by the user
  * @return program exit status
*/
int main()
{
    // make an array to hold our user's input line
    char line[ MAX_LINE_LEN ];
    // hold pointers to each word from the line entered by our user
    char * words[ WORDS_MIN_LEN ];
    // use a bool to keep looping until we have an exit command then set to false (but we will probably just exit the program)
    bool loop = true;
    // keep track of count
    int count = 0;

    // keep prompting for input until we get an exit command then break the loop and call runExit()
    while( loop ) {
        // print stash prompt
        fprintf( stdout, "stash> " );
        // get user input
        fgets( line, MAX_LINE_LEN, stdin );
        // parse input
        count = parseCommand( line, words );

        // check if we have a cd command
        if( strcmp( words[ 0 ], "cd" ) == 0 ) {
            // run cd commmand
            runCd( words, count );
            count = 0;
        }

        // check if we have an exit command
        else if( strcmp( words[ 0 ], "exit" ) == 0 ) {
          if( count == MIN_ARGS ){
            // loop through each char in the 2nd argument to verify it's a valid number
            // if not set this to false
            bool isNotValid = false;
            // get len of string arg
            int len = strlen( words[ 1 ] );
            printf("%d\n", len);
            // declare a pointer so we can examine each char in the string arg
            char * check = words[ 1 ];
            printf("%s\n", check );
            
            for( int i = 0; i < len; i++ ) {
              char ch = *( i + check );
              // invalid input we did not get a digit or negative value
              if( !isdigit( ch ) && ch != '-' ) {
                isNotValid = true;
                break;
              }
            }

            // one of the characters was invalid
            if( isNotValid ) {
              // print invalid commmand
              fprintf( stdout, "Invalid command\n");
            }

            else {
              // break loop and call runExit() with status value
              loop = false;
            }
          }

          else {
            // invalid command
            fprintf( stdout, "Invalid command\n" );
          }
        }

        // check if we have another type of command
        else {
          // only enter runCommand if we have at least one valid word in our words
          if( count != 0 ) {
            runCommand( words, count );
            count = 0;
          }
        }
    }

    if( strcmp( words[ 0 ], "exit" ) == 0 ) {
      runExit( words, count ); 
    } 

    // return successfully
    return EXIT_SUCCESS;
}