/**
*
* File:		master.c
* Author: 	Edward Hills
* Date:		7/08/12
* Description:	Implements the master thread. The parent thread of all other
*		disk and worker threads. Will only exit once all other threads
*		have finished executing.
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
* Prints usage of program.
*/
void
usage(void)
{
	printf("Options are:\n");
	printf("\t-D\tNumber of disks\n");
	printf("\t-W\tNumber of worker threads\n");
	printf("\t-L\tNumber of iterations\n");
}

/* 
* Executes main thread. 
*/
int 
main(int argc, char **argv) 
{
	int opt;
	int num_disks = 16;
	int num_worker_threads = 16;
	int num_iterations = 125000;
	extern char *optarg;

	/* Will not perform any sanity checksi */
	while ((opt = getopt(argc, argv, "D:W:L:")) != -1) {
		switch (opt) {
			case 'D':
				num_disks = atoi(optarg);
				break;
			case 'W':
				num_worker_threads = atoi(optarg);
				break;
			case 'L':
				num_iterations = atoi(optarg);
				break;
			case 'h':
				usage();
				exit(0);
				break;
			case '?':
				usage();
				exit(1);
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}

	/* Create D queues */
	/* start d disc threads */
	/* initialise w read-write locks */
	/* start w worker threads (1 <= w <= 16) */
	/* wait for worker threads to complete */
	/* wait for disc threads to complete */
	/* report how long each worker took */

	printf("Num disks: %d\nNum worker threads: %d\nNum iterations: %d\n"
		, num_disks, num_worker_threads, num_iterations);
	exit(0);
}
