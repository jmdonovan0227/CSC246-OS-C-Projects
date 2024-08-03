/**
  * @file exclude.c
  * @author Jake Donovan (jmpatte8)
  Exclude file is used to read input files and be able to removed passed line number from the input files and send them to a passed
  output file.
*/


#include <unistd.h>
#include <fcntl.h>

/** Minimum command-line arguments */
#define MIN_ARGS 4

/** A value that we will multiply when determining the passed line number from the command line*/
#define TENS 10

/** Our buffer's max size aka that max amount of bytes we will pull from each call to read() */
#define BUFFER_SIZE 64

/** Command-line argument 1 */
#define ARG_1 1

/** Command-line argument 2 */
#define ARG_2 2

/** Command-line argument 3 */
#define ARG_3 3


/**
  * Program starting point used to read input files and removed specified lines and pass to an output file
  * @param argc the number of command line arguments
  * @param argv a pointer to char arrays that point to each command-line argument
  * @return program exit status
*/
int main( int argc, char * argv[] ) 
{
    // Check for the the correct number of command-line arguments 
    if( argc != MIN_ARGS ) {
        char error[] = "usage: exclude <input-file> <output-file> <line-number>\n";
        write( STDERR_FILENO, error, sizeof( error ));
        _exit( 1 );
    }

    else {
        // determine if 3rd command line argument is valid (not valid = negative, going to check if it zero MIGHT NOT NEED THIS OR BE WRONG)
        char * argThree = argv[ ARG_3 ];

        int stringLen = 0;

        int lineNumber = 0;

        // Read the characters in the 3rd command line argument
        while( *( stringLen + argThree ) != '\n' && *( stringLen + argThree ) != '\0' ) {
            if( *( stringLen + argThree ) == '-' ) {
                char error[] = "usage: exclude <input-file> <output-file> <line-number>\n";
                write( STDERR_FILENO, error, sizeof( error ));
                _exit( 1 );
            }

            else {
               stringLen++; 
            }
        }

        // keep track of idx of character we want to multiply and add
        int idx = 0;

        for( int i = stringLen - 1; i >= 0; i-- ) {
            // Special case when we have the one's place
            if( i == 0 ){
                // convert to int
                lineNumber = lineNumber + ( int )(*( idx + argThree ) - '0');
                // increment idx
                idx++;
            }

            else {
                // calculate power value
                int pow = i;

                // save value
                int place = 1;

                while( pow != 0 ){
                    place *= TENS;
                    pow--;
                }

                // convert to int and multiple by correct power of 10
                lineNumber = lineNumber + (( int )(*( idx + argThree ) - '0')) * ( place );
                // increment idx
                idx++;
            }
        }

        // open input file
        int fd = open( argv[ ARG_1 ], O_RDONLY );
        // open output file
        int output = open( argv[ ARG_2 ], O_WRONLY | O_CREAT | O_TRUNC);

        // check that file is valid
        if( fd < 0 ){
            char error[] = "usage: exclude <input-file> <output-file> <line-number>\n";
            write( STDERR_FILENO, error, sizeof( error ));
            _exit( 1 );
        }

        // read contents
        int len;
        // our buffer to hold up to 64 bytes from the input file
        char buffer[ BUFFER_SIZE ];
        // keep track of lines we have examined
        int line = 0;

        // this will act as a boolean for when we have found our line
        // 0 = false, 1 = true
        int haveWeFoundOurLine = 0;
        //
        int done = 0;
        // keep track of previous line position in buffer
        int previousLinePos = 0;

        while(( len = read( fd, buffer, sizeof( buffer )) ) > 0 ){
            for( int i = 0; i < len; i++ ) {
                // look in here for the newline and count that as a line
                if( buffer[ i ] == '\n' ) {
                    if( line < lineNumber - 1 ) {
                        // we only care to update this when we get close to the the line we want to remove
                        previousLinePos = i;
                    }

                    line++;
                }

                if( line == lineNumber  && haveWeFoundOurLine == 0  && done == 0 ){
                    //char * afterLine = buffer;
                    // write the block of memory after our line
                    if( previousLinePos > 0 ){
                        write( output, buffer, previousLinePos + 1 );
                    }

                    // move past current line
                    write( output, i + 1 + buffer, len - i - 1 );
                    // also set our int acting as a bool to true so we know not to write our whole block below to our output file
                    haveWeFoundOurLine = 1;
                    //
                    done = 1;
                }
            }

            // we didn't find the line so write the whole block to our output file
            if( haveWeFoundOurLine == 0 ) {
                // wrgite up to len, confirm this works later
                write( output, buffer, len );
                previousLinePos = 0;
            }

            // we found the line in our loop don't write the block just set our line to false again which is zero
            else {
                haveWeFoundOurLine = 0;
                previousLinePos = 0;
            }
        }
            
        // close input file
        close( fd );
        // close output file
        close( output );
    }

    // Return successfully
    return 0;
}