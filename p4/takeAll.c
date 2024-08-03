/**
  * @file takeAll.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for implementing the no deadlock solution -> no-hold-wait to allow chefs to share appliances
  * and cook without deadlocking. We achieve this with the use of a mutex lock, condition variable, and boolean flags for appliances.
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

/** A little record used to keep up with each of our threads. */
typedef struct {
  // Thread handle for this chef.
  pthread_t thread;

  // Name for this chef.
  char name[ 25 ];

  // Poitner to the start routine for this thread.
  void *(*start)( void * );

  // Number of dishes prepared by this chef.
  int dishCount;
} ChefRec;

/** To tell all the chefs when they can quit running. */
static bool running = true;

/** Print out an error message and exit. */
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}
/** Called by a chef after they have locked all the required appliances
    and are ready to cook for about the given number of milliseconds. */
static void cook( int duration, ChefRec *chef )
{
  printf( "%s is cooking\n", chef->name );
  // Wait for between duration / 2 and duration ms
  usleep( 500 * ( (long) rand() * duration / RAND_MAX + duration  ) );
  chef->dishCount++;
}

/** Called by a chef between dishes, to let them rest about the given
    number of milliseconds before cooking another dish. */
static void rest( int duration, ChefRec *chef )
{
  printf( "%s is resting\n", chef->name );
  // Wait for between duration / 2 and duration ms
  usleep( 500 * ( (long) rand() * duration / RAND_MAX + duration  ) );
}

// A mutex representing the lock on each appliance.
// Acquiring the needed mutexes before cooking prevents two
// chefs from trying to use the same appliance at the same time.
// false -> not available
// true -> available
/** Mutex lock for controlling access to modifying appliance flags so only one chef can set them to use and unused at a time */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/** Condition variable to wait for appliances that we need for our chefs to cook */
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
/** Our griddle flag */
bool griddle = true;
/** Our mixer flag */
bool mixer = true;
/** Our oven flag */
bool oven = true;
/** Our blender flag */
bool blender = true;
/** Our grill flag */
bool grill = true;
/** Our fryer flag */
bool fryer = true;
/** Our microwave flag */
bool microwave = true;
/* Our coffeeMaker flag */
bool coffeeMaker = true;

/** Mandy is a chef needing 105 milliseconds to prepare a dish. */
void *Mandy( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    pthread_mutex_lock( &lock );
    // Get the appliances this chef uses.
    if( microwave && coffeeMaker ) {
      // put them in use = false
      microwave = false;
      coffeeMaker = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( microwave && coffeeMaker ) {
          hasTools = true;
        }
      }

      microwave = false;
      coffeeMaker = false;
    }

    pthread_mutex_unlock( &lock );

    cook( 105, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    microwave = true;
    coffeeMaker = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Edmund is a chef needing 30 milliseconds to prepare a dish. */
void *Edmund( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( blender && oven && mixer ) {
      blender = false;
      oven = false;
      mixer = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( blender && oven && mixer ) {
          hasTools = true;
        }
      }

      blender = false;
      oven = false;
      mixer = false;
    }

    pthread_mutex_unlock( &lock );
    
    cook( 30, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    blender = true;
    oven = true;
    mixer = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Napoleon is a chef needing 60 milliseconds to prepare a dish. */
void *Napoleon( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    pthread_mutex_lock( &lock );
    if( blender && grill ) {
      blender = false;
      grill = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( blender && grill ) {
          hasTools = true;
        }
      }

      blender = false;
      grill = false;
    }

    pthread_mutex_unlock( &lock );

    cook( 60, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    blender = true;
    grill = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Prudence is a chef needing 15 milliseconds to prepare a dish. */
void *Prudence( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( coffeeMaker && microwave && griddle ) {
      coffeeMaker = false;
      microwave = false;
      griddle = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( coffeeMaker && microwave && griddle ) {
          hasTools = true;
        }
      }

      coffeeMaker = false;
      microwave = false;
      griddle = false;
    }

    pthread_mutex_unlock( &lock );

    cook( 15, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    coffeeMaker = true;
    microwave = true;
    griddle = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Kyle is a chef needing 45 milliseconds to prepare a dish. */
void *Kyle( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( fryer && oven ) {
      fryer = false;
      oven = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( fryer && oven ) {
          hasTools = true;
        }
      }

      fryer = false;
      oven = false;
    }
  
    pthread_mutex_unlock( &lock );

    cook( 45, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    fryer = true;
    oven = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Claire is a chef needing 15 milliseconds to prepare a dish. */
void *Claire( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( grill && griddle ) {
      grill = false;
      griddle = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( grill && griddle ) {
          hasTools = true;
        }
      }

      grill = false;
      griddle = false;
    }
    
    pthread_mutex_unlock( &lock );

    cook( 15, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    grill = true;
    griddle = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Lucia is a chef needing 15 milliseconds to prepare a dish. */
void *Lucia( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( griddle && mixer ) {
      griddle = false;
      mixer = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( griddle && mixer ) {
          hasTools = true;
        }
      }

      griddle = false;
      mixer = false;
    }
    pthread_mutex_unlock( &lock );

    cook( 15, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    griddle = true;
    mixer = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Marcos is a chef needing 60 milliseconds to prepare a dish. */
void *Marcos( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( microwave && fryer && blender ) {
      microwave = false;
      fryer = false;
      blender = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( microwave && fryer && blender ) {
          hasTools = true;
        }
      }

      microwave = false;
      fryer = false;
      blender = false;
    }
    pthread_mutex_unlock( &lock );

    cook( 60, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    microwave = true;
    fryer = true;
    blender = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Roslyn is a chef needing 75 milliseconds to prepare a dish. */
void *Roslyn( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( fryer && grill ) {
      fryer = false;
      grill = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( fryer && grill ) {
          hasTools = true;
        }
      }

      fryer = false;
      grill = false;
    }

    pthread_mutex_unlock( &lock );

    cook( 75, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    fryer = true;
    grill = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

/** Stephenie is a chef needing 30 milliseconds to prepare a dish. */
void *Stephenie( void * arg )
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *) arg;

  while ( running ) {
    // Get the appliances this chef uses.
    pthread_mutex_lock( &lock );
    if( mixer && coffeeMaker && oven ) {
      mixer = false;
      coffeeMaker = false;
      oven = false;
    }

    else {
      bool hasTools = false;

      // keep looping until we have all tools we need
      while( !hasTools ) {
        pthread_cond_wait( &cond, &lock );

        if( mixer && coffeeMaker && oven ) {
          hasTools = true;
        }
      }

      mixer = false;
      coffeeMaker = false;
      oven = false;
    }

    pthread_mutex_unlock( &lock );

    cook( 30, rec );

    pthread_mutex_lock( &lock );
    // we are done cooking, release all appliances by setting them to true meaning they are available to use
    mixer = true;
    coffeeMaker = true;
    oven = true;
    pthread_cond_broadcast( &cond );
    pthread_mutex_unlock( &lock );

    rest( 25, rec );
  }

  return NULL;}

int main( void )
{
  // Seed the random number generator, so we get variation in behavior.
  srand( time( NULL ) );

  // Make a record for each chef.
  ChefRec chefList[] = {
    { .name = "Mandy", .start = Mandy },
    { .name = "Edmund", .start = Edmund },
    { .name = "Napoleon", .start = Napoleon },
    { .name = "Prudence", .start = Prudence },
    { .name = "Kyle", .start = Kyle },
    { .name = "Claire", .start = Claire },
    { .name = "Lucia", .start = Lucia },
    { .name = "Marcos", .start = Marcos },
    { .name = "Roslyn", .start = Roslyn },
    { .name = "Stephenie", .start = Stephenie },
  };

  // Make a thread for each chef.
  for ( int i = 0; i < sizeof(chefList) / sizeof(chefList[0]); i++ ) {
    // Give each chef a pointer to its ChefRec struct.
    if ( pthread_create( &chefList[ i ].thread,
                         NULL,
                         chefList[ i ].start,
                         chefList + i ) != 0 )
      fail( "Can't create thread" );
  }

  // Let the chefs cook for a while, then ask them to stop.
  sleep( 10 );
  running = false;

  // Wait for all our chefs to finish, and collect up how much
  // cooking was done.
  int total = 0;
  for ( int i = 0; i < sizeof(chefList) / sizeof(chefList[0]); i++ ) {
    pthread_join( chefList[ i ].thread, NULL );
    printf( "%s cooked %d dishes\n",
            chefList[ i ].name,
            chefList[ i ].dishCount );
    total += chefList[ i ].dishCount;
  }
  printf( "Total dishes cooked: %d\n", total );
}