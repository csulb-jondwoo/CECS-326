/* shmc1.cpp */

/*
import necessary libraries
*/
#include "registration.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <semaphore.h> // POSIX implementation of semaphores
#include <pthread.h>

using namespace std;

/*
initialize struct, variables, and methods
*/
CLASS *class_ptr;
void 	*memptr;
char	*pname;
int	shmid, ret;

// semaphore pointer
sem_t *sem_ptr;

// name of semaphore
char *shmSem;

void rpterror(char *), srand(), perror(), sleep();
void sell_seats();

int main(int argc, char* argv[])
{
	/*
	if there are less than 3 argc in the argv array, 
	write contents of the first element in argv array to "stderr" in the specified format
	*/
	if (argc < 3) {
		fprintf (stderr, "Usage:, %s shmid\n", argv[0]);
		exit(1);
	}

	/*
	assign pname to first element in argv array
	read in element 1 in argv array and store them to shmid in the specified format
	*/
	pname = argv[0];
	sscanf (argv[1], "%d", &shmid);

	shmSem = argv[2];
	/*
	attaches the shared memory segment identified by shmid to the address space of the calling process
	returns the address of the attached shared memory segment and assigns it to memptr
	*/
	memptr = shmat (shmid, (void *)0, 0);

	// failed
	if (memptr == (char *)-1 ) {
		rpterror ("shmat failed");
		exit(2);
	}

	// assign memptr to class_ptr. class_ptr holds address of shared mem segment
	class_ptr = (struct CLASS *)memptr;

	//open previously created semaphore
	sem_ptr = sem_open(shmSem, 0);

	// invoke sell_seats
	sell_seats();
	
	/*
	detaches the shared mem segment located at address pointed to by mem_ptr
	*/
	ret = shmdt(memptr);
	exit(0);
}

//defines sell_seats function
void sell_seats() 
{
	//initialize variables
	int all_out = 0;

	//returns pseudo-random integer with the seed getpid()

	srand ( (unsigned) getpid() );

	//loop while all_out is not true
	while ( !all_out) {   /* loop to sell all seats */
		/*
		decrement(lock) the named semaphore by 1
		when semaphore value = 0, blocks the process
		*/
		sem_wait(sem_ptr);

		if (class_ptr->seats_left > 0) {

			//pause until randomly generated number by srand
			sleep ( (unsigned)rand()%5 + 1);

			//decrement number of seats left
			class_ptr->seats_left--;

			//pause until randomly generated number by srand
			sleep ( (unsigned)rand()%5 + 1);

			//print out
			cout << pname << " SOLD SEAT -- " 
			     << class_ptr->seats_left << " left" << endl;
		}

		//if no more seats, increment all_out, which will cause the while loop to break
		else {
			all_out++;
			cout << pname << " sees no seats left" << endl;
		}

		/*
		increment(unlock) the named semaphore by 1
		If the semaphore's value consequently becomes greater than zero, then another
        process or thread blocked in a sem_wait(3) call will be woken up and proceed to lock the semaphore.
		*/
		sem_post(sem_ptr);


		//pause until randomly generated number by srand
		sleep ( (unsigned)rand()%10 + 1);
	}
}

//define rpterror function
void rpterror(char* string)
{
	//initialize variable
	char errline[50];

	//store contents of string and pname to buffer pointed to by errline in the format specified
	sprintf (errline, "%s %s", string, pname);

	//print the error
	perror (errline);
}
