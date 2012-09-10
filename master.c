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

#define CBUF_SIZE 30
#define READ 0
#define WRITE 1

pthread_t *disk_thread_id;  /* Stores ids of all disc threads */
pthread_t *worker_thread_id; 

circular_buffer_t *disk_queues;

/**
 *  Circular Buffer struct.
 */
typedef struct circular_buffer_t {
    int start;
    int count;
    int jobs[CBUF_SIZE];
} circular_buffer;

/*
 * Check whether circular buffer is empty.
 */
int is_cb_empty(void) {
    return cbuf.count == 0;
}

/**
 * Add job into circular buffer.
 */
int cbuffer_add(int job) {
    int end = (cbuf.start + cbuf.count) % CBUF_SIZE;

    cbuf.jobs[end] = job;
    if (!is_cb_full()) {
        cbuf.count++;
        return 0;
    }
    return -1;
}

/**
 * Remove job from circular buffer.
 */
char cbuffer_get_job(void) {
    int job;

    job = cbuf.jobs[cbuf.start];
    cbuf.start = (cbuf.start + 1) % CBUF_SIZE;
    cbuf.count--;
    return job;

}

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
 *  * Will create disk threads
 *   */
int
create_worker_threads(int num_threads)
{
    int i;
    worker_thread_id = emalloc(sizeof (pthread_t) * num_threads);

    for(i=0; i < num_threads; i++) {
        if (pthread_create(&worker_thread_id[i], NULL, worker_listen, NULL) != 0) {
            perror("Cannot create thread.");
            return -1; 
        }   
        fprintf(stderr, "Created worker thread:\t%ld\n", worker_thread_id[i]);
    }   
    return 0;
}


/* 
 *  * Will create disk threads
 */
int
create_disk_threads(int num_threads) 
{
    int i;

    disk_thread_id = emalloc((sizeof(pthread_t)) * num_threads);

    for(i=0; i < num_threads; i++) {
        if (pthread_create(&disk_thread_id[i], NULL, disk_listen, NULL) != 0) {
            perror("Cannot create thread.");
            return -1; 
        }   
        fprintf(stderr, "Created disk thread:\t%ld\n", disk_thread_id[i]);
        create_disk_filesystem();
    }   
    return 0;
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
    int j;

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

    /* Create D queues */
    disk_queues = emalloc(sizeof(circular_buffer) * num_disks);

    create_disk_threads(num_disks);
    /* initialise w read-write locks */
    create_worker_threads(num_worker_threads);
    for(j=0; j < num_worker_threads; j++) {
        pthread_join(worker_thread_id[j], NULL); 
    }
    for(j=0; j < num_disks; j++) {
        pthread_join(disk_thread_id[j], NULL); 
    }
    /* report how long each worker took */


    /* Wait for threads to finish before exiting */

    /* Now that all threads are complete I can print the final result.     */
    /* Without the join I could be printing a value before all the threads */
    /* have been completed.                                                */


    printf("Num disks: %d\nNum worker threads: %d\nNum iterations: %d\n"
            , num_disks, num_worker_threads, num_iterations);
    exit(0);
}
