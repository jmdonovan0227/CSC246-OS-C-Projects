/**
  * @file hall.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for creating a monitor to manage a variety of organizations who want to use our hall.
  * Our monitor uses a mutex lock and condition variable and a variety of other variables to effectively and efficiently move guests
  * between the hall.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "hall.h"

/** Our hall which keeps track of all organizations in an array of chars which use '*' to indicate free space and the first letter 
  * of an organizations name to indicate that they are occupying the hall in a certain position
*/
static char * hall;

/** Our key player, an int, in determing when there is enough space for an organization that is waiting. 1 indicates there is enough
  * space and 0 indicates there is not enough space
*/
static volatile int isSpace;

/** Our mutex lock to prevent two organizations from accessing the monitor at a time */
pthread_mutex_t lock;

/** Our condition variable to determine when we have enough space for an organization waiting to enter hall */
pthread_cond_t cond;

/** Keep track of length of hall aka the number of spaces */
static int len;

/** 
  * Initialize the monitor as a hall with n spaces that can be partitioned
  * off. 
  * @param n the number of spaces in hall
*/
void initMonitor( int n ) {
    // initialize mutex lock
    pthread_mutex_init( &lock, NULL );

    // initialize condition variable
    pthread_cond_init( &cond, NULL );

    // initialize hall
    hall = ( char * )malloc( sizeof( char ) * n );

    // initialize hall with aterisks = '*' indicating that space is available
    for( int i = 0; i < n; i++ ) {
        hall[ i ] = '*';
    }

    len = strlen( hall );

    isSpace = 0;
}

/** Destroy the monitor, freeing any resources it uses. */
void destroyMonitor() {
    // free hall
    free( hall );

    // destroy condition variable
    pthread_cond_destroy( &cond );

    // destroy mutex lock
    pthread_mutex_destroy( &lock );
}

/** 
  * Called when an organization wants to reserve the given number
  * (width) of contiguous spaces in the hall.  Returns the index of
  * the left-most (lowest-numbered) end of the space allocated to the
  * organization.
  * @param name
  * @param width 
*/
int allocateSpace( char const *name, int width ) {
    // lock
    pthread_mutex_lock( &lock );
    // reset isSpace to keep track of when we have found enough space for an organization
    isSpace = 0;
    //
    int start = 0;
    // used to count available spaces we find in a row when searching through availableSpaces int array
    int countAvailable = 0;

    // loop through availableSpaces to determine if we have enough space
    for( int i = 0; i < len; i++ ) {
        // if at any point we find the minimum required len = width set found to true
        // reset countAvailable at any point when we find a char that is not equal to '*'
        
        // we found free space!
        if( hall[ i ] == '*' ) {
            // if countAvailable is 0 set starting index
            if( countAvailable == 0 ) {              
                // leftmost starting index for space to be occupied
                start = i;
            }

            //
            countAvailable++;
        }

        // else this space is occupied
        else {
            // reset countAvailable
            countAvailable = 0;
        }

        if( countAvailable == width ) {
            // found required amount of space starting with smallest available index
            isSpace = 1;
            //
            break;
        }
    }

    // we have space
    if( isSpace == 1 ){
        char firstLetter = name[ 0 ];

        for( int i = start; i < start + width; i++ ) {
            hall[ i ] = firstLetter;
        }

        // print allocation message
        printf( "%s", name );
        printf( "%s", " allocated: " );
        printf("%s\n", hall );
    }

    // we don't have space
    else {
        // print waiting message
        printf( "%s", name );
        printf( "%s", " waiting: " );
        printf("%s\n", hall );

        // first letter of name
        char firstLetter = name[ 0 ];
        // keep track of when we have actually added the waiting organization to the hall
        bool isEdited = false;

        // keep looping until we have added the organization
        while( !isEdited ){
            // check if we have freed any organizations meaning we have more space to examine
            while( isSpace == 0 ) {
                pthread_cond_wait( &cond, &lock );
            }

            // keep track of max number of spaces in a row available
            int countAvailable = 0;
            // keep track of starting idx
            start = 0;

            // loop through contents of hall
            for( int i = 0; i < len; i++ ) {
                if( hall[ i ] == '*' ) {
                    if( countAvailable == 0 ) {
                        start = i;
                    }

                    countAvailable++;
                }

                else {
                    countAvailable = 0;
                }

                if( countAvailable == width ) {
                    break;
                }
            }

            // if we found enough space break loop after this iteration
            if( countAvailable == width ) {
                isEdited = true;
            }

            // if we found what we want edit the hall to include waiting organization
            if( isEdited ) {
                for( int i = start; i < start + width; i++ ) {
                    hall[ i ] = firstLetter;
                }
            }

            // otherwise set isSpace back to zero indicating we don't actually have enough space
            // this will be switched back to 1 on every call to free method
            else {
                isSpace = 0;
            }
        }

        // print allocation message
        printf( "%s", name );
        printf( "%s", " allocated: " );
        printf("%s\n", hall );
    }

    // unlock
    pthread_mutex_unlock( &lock );
    // return leftmost idx
    return start;
}


/** 
  * Release the allocated spaces from index start up to (and including)
  * index start + width - 1.
  * @param name
  * @param start
  * @param width  
*/
void freeSpace( char const *name, int start, int width ) {
    // lock
    pthread_mutex_lock( &lock );

    // remove passed organization
    for( int i = start; i < start + width; i++ ) {
        hall[ i ] = '*';
    }

    // tell allocate we have more space to look at
    isSpace = 1;

    // print freed message
    printf( "%s", name );
    printf("%s", " freed: " );
    printf("%s\n", hall );

    // unlock
    pthread_mutex_unlock( &lock );
    // signal threads
    pthread_cond_broadcast( &cond );
}