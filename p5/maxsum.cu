/**
  * @file maxsum.cu
  * @author Jake Donovan (jmpatte8)
  * This file utilizes CUDA functions in order to calculate the maximum sum of a number of values by utilizing threads and CUDA functions.
*/

// Elapsed Real Time for input-5.txt: (real) = 1.172 seconds.
// Type of GPU: RTX 2070

#include <stdio.h>
#include <stdbool.h>
#include <cuda_runtime.h>

// Input sequence of values.
int * vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

// Our results array which holds all local max sums calculated by each thread
int * results;


/**
  * General function to report a failure and exit.
  * @param message the passed error message
*/
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: maxsum [report]\n" );
  exit( 1 );
}

// Read the list of values.
__host__ void readList() {
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
  * This function is responsible for calculating a local max value starting at an indicated position and saves to our output
  * array using the values from the passed input_vList to determine the localMax starting from the indicated position, vCount is the total number
  * of values in our vList, and report is a boolean that lets us know whether each thread should report their localMax or simply save the localMax to our results
  * array
  * @param input_vList our passed values that we want to use to determing our local max for each thread
  * @param output our results array which holds each localMax calculated by our threads
  * @param vCount the number of values in our vList array aka input_vList
  * @param report a boolean that alerts our threads whether they should report their localMax or only save their value to our results array ( aka the output param )
*/
__global__ void checkSum( int * input_vList, int * output, int vCount, bool report ) {
  // Compute a unique index for this thread, based on its location in its block location in its grid
  int idx = blockDim.x * blockIdx.x + threadIdx.x;

  // make sure our thread idx is not larger than the number of values we should examine
  // this helps in the case where we have more threads than we need to calculate our max sum
  if ( idx < vCount ) { 
    // our current sum calculated by current thread
    int currentSum = 0;
    // max sum found by current thread
    int localMax = 0;

    // go through each value in input list starting at idx to find the local max
    for( int i = idx; i >= 0; i-- ) {
      // update current sum
      currentSum += input_vList[ i ];

      // is our current sum larger than our current local max?
      // then update it
      if( currentSum > localMax ) {
        localMax = currentSum;
      }
    }

    // update our output list
    output[ idx ] = localMax;

    // if report is true then print our thread at idx and our localMax found by our current thread
    if( report ) {
      printf( "I'm thread %d. The maximum sum I found is %d\n", idx, localMax );
    }
  }
}

/**
  * Use command line arguments, CUDA functions, and this file's functions in order to determine the max sum of a list of values
  * and report the max sum found
  * @param argc the number of command line arguments
  * @param argv pointers to each command line argument (as strings)
  * @return program exit status
*/
int main( int argc, char *argv[] ) {
  if ( argc < 1 || argc > 2 )
    usage();

  // If there's an argument, it better be "report"
  bool report = false;
  if ( argc == 2 ) {
    if ( strcmp( argv[ 1 ], "report" ) != 0 )
      usage();
    report = true;
  }

  readList();

  // get space for results array
  // vCount = cap for results
  results = ( int * )malloc( vCount * sizeof( int ) );
  
  // Add code to allocate memory on the device and copy over the list.
  int *devList = NULL;

  if( cudaMalloc( ( void **)&devList, vCount * sizeof( int ) ) != cudaSuccess ) {
    fail( "Failed to allocate space for length list on device" );
  }

  // Add code to copy the list over to the device.
  if( cudaMemcpy( devList, vList, vCount * sizeof( int ), cudaMemcpyHostToDevice ) != cudaSuccess ) {
    fail( "Failed to copy list to device" );
  }

  // create device pointer for our output array
  int *devResult = NULL;

  // Add code to allocate space on the device to hold the results.
  if( cudaMalloc( ( void ** )&devResult, vCount * sizeof( int ) ) != cudaSuccess ) {
    fail( "Failed to allocate space for results list on device" );
  }

  // Block and grid dimensions. ( HOW MANY THREADS I NEED FOR GPU TASK )
  int threadsPerBlock = 100;
  // Round up for the number of blocks we need.
  int blocksPerGrid = ( vCount + threadsPerBlock - 1 ) / threadsPerBlock;

  // Run our kernel on these block/grid dimensions (you'll need to add some parameters)
  checkSum<<<blocksPerGrid, threadsPerBlock>>>( devList, devResult, vCount, report );
  if ( cudaGetLastError() != cudaSuccess )
    fail( "Failure in CUDA kernel execution." );

  // Add code to copy results back to the host, compare the local largest products
  // and report the final largest product
  if( cudaMemcpy( results, devResult, vCount * sizeof( int ), cudaMemcpyDeviceToHost ) != cudaSuccess ) {
    fail( "Can't copy list from device to host" );
  }

  // save our largest max
  int maxSum = 0;

  // report final largest sum
  for( int i = 0; i < vCount; i++ ) {
    // if current localMax in results array is larger than maxSum, then update maxSum
    if( results[ i ] > maxSum ) {
      maxSum = results[ i ];
    }
  }

  // report maxSum found
  printf( "Maximum Sum: %d\n", maxSum );

  // Free memory on the device and the host.
  cudaFree( devList );
  // free vList
  free( vList );
  // free results array
  free( results );

  // reset
  cudaDeviceReset();
  // return successfully
  return 0;
}
