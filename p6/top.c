/**
  * @file top.c
  * @author Jake Donovan (jmpatte8)
  * This file is responsible for printing all information in relation to a terminal "top" command without having to refresh like a normal top command
*/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/** The max number of processes for our procs array */
#define	MAX_PROC_NUM	1024

/** Our struct that holds all information for our processes */
typedef struct _my_process
{
	int pid;
	char pname[64];
	char pstate[8];
	int ppid;
	int memsize;
}my_process;

/** All processes in our system */
my_process procs[MAX_PROC_NUM];

// Ouputs current time
void output_time()
{
	// get our path for retrieving rtc time and date
	char path[] = "/proc/driver/rtc";
	// open a file for reading
	FILE * fp = fopen( path, "r" );
	// array to hold entire rtc_time string
	char rtc_time_string[ 30 ];
	// array to hold entire rtc_date string
	char rtc_date_string[ 30 ];
	// get rtc_time string
	fgets( rtc_time_string, 29, fp );
	// get rtc_date string
	fgets( rtc_date_string, 29, fp );
	// use this array to find the point in the string where we have numbers
	char clearString[ 30 ];
	// save time in this array
	char rtc_time[ 9 ];
	// save how far we are in string
	int n;
	// scan everything until we hit a number
	sscanf( rtc_time_string, "%[^0-9]%n", clearString, &n );
	// get the rtc_time
	sscanf( n + rtc_time_string, "%[0-9:0-9:0-9]", rtc_time );

	// same process for rtc_date;
	// save our rtc_date
	char rtc_date[ 11 ];
	// parse until we hit a number
	sscanf( rtc_date_string, "%[^0-9]%n", clearString, &n );
	// parse the date
	sscanf( n + rtc_date_string, "%[0-9-0-9-0-9-0-9]", rtc_date );
	// update the time as it is 8hrs off
	int hr;
	sscanf( rtc_time, "%d%n", &hr, &n );

	// if hours + 8 is greater than or equal to 10 and less than 24 for military time print normal
	if( hr + 8 >= 10 && hr + 8 < 24 ) {
		hr = hr + 8;
		// print string
		char result[ 100 ];
		sprintf( result, "Time: %s ", rtc_date );
		//printf("%s\n", result );
		sprintf( result + strlen( result ), "%d%s", hr, n + rtc_time );
		printf("%s\n", result );
	}

	// if we have an integer for hr less than 10 we need to add a 0 in front of hr when printing
	else if( hr + 8 < 10 ) {
		hr = hr + 8;
		char result[ 100 ];
		sprintf( result, "Time: %s ", rtc_date );
		sprintf( result + strlen( result ), "0%d%s", hr, n + rtc_time );
		printf("%s\n", result );
	}

	// if we have an hour greater than 24 when adding 8 we need to reset by subtracting 24 since military time starts back at 0 for hr when it hits 24
	else if( hr + 8 >= 24 ){
		// add
		hr = hr + 8;
		// convert
		hr = hr - 24;
		char result[ 100 ];
		sprintf( result, "Time: %s ", rtc_date );
		sprintf( result + strlen( result ), "0%d%s", hr, n + rtc_time );
		printf("%s\n", result );
	}

	// close file
	fclose( fp );
}

// Outputs system uptime
void output_uptime()
{
	// get our path for retrieving uptime
	char path[] = "/proc/uptime";
	// open a file for reading
	FILE * fp = fopen( path, "r" );
	// uptime
	float uptime;
	// get the uptime of the system
	fscanf( fp, "%f", &uptime );

	// first get days
	float days = ( uptime / 60 / 60 / 24 );

	// check for hours
	float hours;

    // if our uptime is greater than a day
	if( days >= 1 ) {
			// Then hours
			hours = ( uptime / 60 / 60 ) - ( (int)days * 24 );
	}

    // otherwise
	else {
			// Then hours
			hours = ( uptime / 60 / 60 );
	}

	// Then minutes
	// subtract hours from the integer version of hours meaning we obtain the decimal for minutes (out of an hour)
	// when subtracting the two values then we multiply by 60 to get the remaining minutes.
	float minutes = 0;
	if( hours >= 1 ) {
		minutes = ( hours - (int)hours ) * 60;
	}

	else {
		minutes = days - (int)days * 60;
	}

	if( (int)minutes >= 10 && (int)hours >= 10 ) {
		// Print calculate uptime
		printf( "Uptime: %d days, %d:%d\n", (int)days, (int)hours, (int)minutes );
	}

	else if( (int)minutes < 10 && (int)hours >= 10 ) {
		// Print calculate uptime
		printf( "Uptime: %d days, %d:0%d\n", (int)days, (int)hours, (int)minutes );
	}

	else if( (int)minutes >= 10 && (int)hours < 10 ) {
		// Print calculate uptime
		printf( "Uptime: %d days, 0%d:%d\n", (int)days, (int)hours, (int)minutes );
	}

	else {
		// Print calculate uptime
		printf( "Uptime: %d days, 0%d:0%d\n", (int)days, (int)hours, (int)minutes );
	}

	// close file
	fclose( fp );
}

// Outputs load average
void output_loadavg()
{
	// get our path for retrieving uptime
	char path[] = "/proc/loadavg";
	// open a file for reading
	FILE * fp = fopen( path, "r" );
	// get the first 3 fields from loadavg
	float f1;
	float f2;
	float f3;
	fscanf( fp, "%f%f%f", &f1, &f2, &f3 );
	// print results
	printf( "Load avg: %.6f %.6f %.6f\n", f1, f2, f3 );
	// close file
	fclose( fp );
}

// Outputs CPU
void output_cpu()
{
	// get our path for retrieving uptime
	char path[] = "/proc/stat";
	// open a file for reading
	FILE * fp = fopen( path, "r" );
	// get rid of cpu name
	char cpu[ 5 ];
	// save all eight cpu values
	// value 1
	int v1;
	// value 2
	int v2;
	// value 3
	int v3;
	// value 4
	int v4;
	// value 5
	int v5;
	// value 6
	int v6;
	// value 7
	int v7;
	// value 8
	int v8;
	// scan for contents
	fscanf( fp, "%[^0-9]%d%d%d%d%d%d%d%d", cpu, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8 );
	// print results
	printf( "Cpu: %dus %dsy %dni %did %dwa %dhi %dsi %dst\n", v1, v2, v3, v4, v5, v6, v7, v8 );
	// close file
	fclose( fp );
}

// Outputs memory information
void output_mem()
{
	// get path for memory info
	char path[] = "/proc/meminfo";
	// open the file with path
	FILE * fp = fopen( path, "r" );
	// save mem_total info line
	char mem_total_line[ 30 ];
	// save mem_free line
	char mem_free_line[ 30 ];
	// get mem_total line
	fgets( mem_total_line, 29, fp );
	// get mem_free line
	fgets( mem_free_line, 29, fp );
	// use this as a place holder as we scan through our mem_total and mem_free lines
	char scanThrough[ 30 ];

	// scan through mem_total_line until we hit an integer
	sscanf( mem_total_line, "%[^0-9]", scanThrough );

	// start scanning from that spot and save int
	int totalMemory = 0;
	sscanf( mem_total_line + strlen( scanThrough ), "%d", &totalMemory );

	// scan through mem_free_line until we hit an integer
	sscanf( mem_free_line, "%[^0-9]", scanThrough );

	// start scanning from that spot and save int
	int freeMemory = 0;
	sscanf( mem_free_line + strlen( scanThrough ), "%d", &freeMemory );

	// calculate used memory
	int usedMemory = totalMemory - freeMemory;

	// print contents
	printf( "Mem: %d total %d used %d free\n", totalMemory, usedMemory, freeMemory );
	// close file
	fclose( fp );
}

// Outputs swap information
void output_swap()
{
	// the path for swaps information in /proc directory
	char path[] = "/proc/swaps";
	// open a file for /proc/swaps so we can view information we need to display for swaps
	FILE * fp = fopen( path, "r" );
	// save header information here
	char header[ 100 ];
	// get the first line
	fgets( header, 99, fp );
	// save info for swap here
	char info[ 100 ];
	// get the info for swaps
	fgets( info, 99, fp );
	// use an array to skip over information we don't want (we want size and used)
	char scanThrough[ 100 ];
	// keep track of number of characters read
	int total = 0;
	// save our position when using sscanf
	int n;
	// scan up until we hit an integer (there is one in filename)
	sscanf( info, "%[^0-9]%n", scanThrough, &n );
	// add n + 1 to total so we don't read the same int that stopped sscanf before
	total += n + 1;
	// sscan past name until we find first integer 
	sscanf( info + total, "%[^0-9]%n", scanThrough, &n );
	// increment total
	total += n;
	// get total size
	int totalSize = 0;
	// find total size
	sscanf( info + total, "%d%n", &totalSize, &n );
	// add chars read to total
	total += n;
	// get used 
	int used = 0;
	// find used
	sscanf( info + total, "%d%n", &used, &n );
	// increment total
	total += n;
	// calculate free
	int free = totalSize - used;
	// print contents
	printf("Swap: %d total %d used %d free\n", totalSize, used, free );
	// close file
	fclose( fp );
}

// Gets all current running processes 
void getAllProcess()
{
	// use this direct pointer to cycle through each file/sub-directory in our /proc directory
	struct dirent * subDirAndProcs;
	// open /proc directory
	DIR * dir = opendir( "/proc" );
	// bool to determine when we have found the first process in /proc directory
	bool foundProcesses = false;
	// keep track of idx in procs array
	int idx = 0;
	// search through /proc directory and find all processes and set all fields in procs array to match those processes
	while( ( subDirAndProcs = readdir( dir )) != NULL ) {
		// if we have found the first process, set all process fields in the procs array at idx
		if( strcmp( subDirAndProcs->d_name, "1" ) == 0 ){
			foundProcesses = true;
			char path[ 100 ] = "/proc/";
			strcat( path, subDirAndProcs->d_name );
			strcat( path, "/stat" );
			FILE * fp = fopen( path, "r" );
			int pid;
			char pname[64];
			char pstate[8];
			int ppid;
			int memsize;
			fscanf( fp, "%d%s%s%d%d", &pid, pname, pstate, &ppid, &memsize );
			procs[ idx ].pid = pid;
			strcpy( procs[ idx ].pname, pname );
			strcpy( procs[ idx ].pstate, pstate );
			procs[ idx ].ppid = ppid;
			procs[ idx ].memsize = memsize;
			idx++;
			fclose( fp );
		}

		// if we have already found the first process set all fields in the procs array at idx
		else if( foundProcesses ) {
			char path[ 100 ] = "/proc/";
			strcat( path, subDirAndProcs->d_name );
			strcat( path, "/stat" );
			FILE * fp = fopen( path, "r" );
			int pid;
			char pname[64];
			char pstate[8];
			int ppid;
			int memsize;
			fscanf( fp, "%d%s%s%d%d", &pid, pname, pstate, &ppid, &memsize );
			procs[ idx ].pid = pid;
			strcpy( procs[ idx ].pname, pname );
			strcpy( procs[ idx ].pstate, pstate );
			procs[ idx ].ppid = ppid;
			procs[ idx ].memsize = memsize;
			// increment idx
			idx++;
			// close current fp
			fclose( fp );
		}
	}
}

// Outputs process statistics
void output_procstat()
{
	// find number of sleeping vs running tasks
	// number of sleeping tasks
	int sleeping = 0;
	// number of running tasks
	int running = 0;

	// find sleeping and running tasks
	for( int i = 0; i < MAX_PROC_NUM; i++ ) {
		if( strcmp( procs[ i ].pstate, "S" ) == 0 ) {
			sleeping++;
		}

		else if( strcmp( procs[ i ].pstate, "R" ) == 0) {
			running++;
		}
	}

	// calculate total tasks = running + sleeping
	int totalTasks = running + sleeping;
	// print results
	printf( "Tasks: %d total %d running %d sleeping\n", totalTasks, running, sleeping );
}

// Outputs top 10 processes that have the largest memory sizes in descending order
void output_proc()
{
	// hold our top ten processes in this array
	my_process top_ten[ 10 ] = {};
	// keep track of current max memsize
	int currentMax = 0;
	// keep track of idx in procs array
	int idx = 0;
	//
	bool found = false;
	// Loop through entire procs array and find the current largest max memsize and set it to be top_ten[i]
	for( int i = 0; i < 10; i++ ) {
		// Loop through each process
		for( int j = sizeof( procs ) / sizeof( procs[ 0 ]); j >= 0; j-- ) {
			if( procs[ j ].memsize > currentMax ) {
				// if top_ten has an element
				if( i > 0 ) {
					// if memsize that is greater than current max is not greater than the last element in top 10
					if( procs[ j ].memsize < top_ten[ i - 1 ].memsize ) {
						currentMax = procs[ j ].memsize;
						// set idx for procs array of process we want 
						idx = j;
					}

					// if memsize is equal to currentMax and the process we have is not equal to the current process at top_ten[ i - 1 ] and it's process id is greater than top_ten[ i - 1 ] then add to top ten
					else if( procs[ j ].memsize == top_ten[ i - 1 ].memsize && procs[ j ].pid != top_ten[ i - 1 ].pid && procs[ j ].pid > top_ten[ i - 1 ].pid && !found ) {
						currentMax = procs[ j ].memsize;
						// set idx for procs array of the process we want
						found = true;
						idx = j;
					}
				}

				// else
				else {
					currentMax = procs[ j ].memsize;
					// set idx for procs array of the process we want
					idx = j;
				}
			}
		}
		// set top_ten[ i ] to be largest process found (memsize)
		top_ten[ i ] = procs[ idx ];
		// rest idx
		idx = 0;
		// reset current max
		currentMax = 0;
		//
		found = false;
	}

	
	// save our resulting string in this char array to print each of top ten processes
	char result[ 100 ] = "";
	// print top_ten processes
	for( int i = 0; i < 10; i++ ) {
		sprintf( result, "%-9d", top_ten[ i ].pid );
		sprintf( result + strlen( result ), "%-20s", top_ten[ i ].pname );
		sprintf( result + strlen( result ), "%-5s", top_ten[ i ].pstate );
		sprintf( result + strlen( result ), "%-9d", top_ten[ i ].ppid );
		sprintf( result + strlen( result ), "%d", top_ten[ i ].memsize );
		printf("%s\n", result );
	}
}

/**
  * Call all functions to perform top command
  * @param argc the number of command line arguments
  * @param argv the command line arguments in a char pointer array
  * @return program exit status
*/
int main(int argc, char* argv[])
{
	memset(&procs, 0, MAX_PROC_NUM*sizeof(my_process));
	getAllProcess();
	output_time();
	output_uptime();
	output_loadavg();
	output_procstat();
	output_cpu();
	output_mem();
	output_swap();
	output_proc();
	return 0;
}

