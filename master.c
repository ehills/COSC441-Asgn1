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
#include <pthread.h>
#include "mylib.h"
#include "disc.h"
#include "worker.h"

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
    printf("\ne.g.\t./simulation -D 16 -W 16 -L 125000\n");
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
/*    int j; */

    /* Will not perform any sanity checks */
    if (argc < 2) {
        fprintf(stderr, "You must enter at least one argument.\n");
        usage();
        exit(1);
    } 
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

    create_disk_threads(num_disks);
    create_worker_threads(num_worker_threads);

    /* Wait for threads to finish before exiting */
    /* TODO figure out how c deals with this.. i.e how do access other files array? */
#if 0
    for(j=0; j < num_worker_threads; j++) {
        pthread_join(thread_id[j], NULL); 
    }
#endif

    /* Now that all threads are complete I can print the final result.     */
    /* Without the join I could be printing a value before all the threads */
    /* have been completed.                                                */

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

