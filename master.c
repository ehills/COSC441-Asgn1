/**
 *  
 *    File:        master.c
 *     Author:     Edward Hills
 *      Date:      7/08/12
 *       Description:  Implements the master thread. The parent thread of all other
 *            disk and worker threads. Will only exit once all other threads
 *           have finished executing.
 *         
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mylib.h"
#include "cbuf.h"
#include "disc.h"
#include "worker.h"

disc_container *discs;  /* Contains location of all disc threads */
worker *workers;        /* contains location of all worker threads */
pthread_mutexattr_t attribute;
pthread_mutex_t *file_locks;

/**
 * Check circular buffer is full
 */  
int is_cb_full(circular_buffer *cbuf) {
    return (cbuf->end + 1) % CBUF_SIZE == cbuf->start;
}

/*
 * Check whether circular buffer is empty.
 * Doesn't maintain a lock, is not crucial to have accurate reading as will be re-checked at some point. If it does need a lock, the caller can wrap it.
 */
int is_cb_empty(circular_buffer *cbuf) {
    return cbuf->start == cbuf->end;
}

/**
 *  Add job into circular buffer.
 */
int cbuffer_add(job *entry, circular_buffer *cbuf) {

    if (is_cb_full(cbuf)) {
        return -1;
    }
    
    cbuf->jobs[cbuf->end] = entry;
    cbuf->end = (cbuf->end +1) % CBUF_SIZE;

    return 0;
}

/**
 *  Remove job from circular buffer.
 */
job *cbuffer_get_job(circular_buffer *cbuf) {
    job *entry;

    entry = cbuf->jobs[cbuf->start];
    cbuf->start = (cbuf->start + 1) % CBUF_SIZE;

    return entry;

}

/*
 * Prints usage of program.
 */
void
usage(void)
{
    printf("Options entered on the command line must be in this order:\n");
    printf("\t\tNumber of disks\n");
    printf("\t\tNumber of worker threads\n");
    printf("\t\tNumber of iterations\n");
    printf("\ne.g.\t./simulation 16 16 1\n");
}

/* 
 *  Will create disc threads, the circular buffer for each disc
 *  and the read/write lock for each queue.
 */
int
create_disk_threads(int num_threads) 
{
    int i;

    discs = emalloc(sizeof(disc_container) * num_threads);
    for (i=0; i < num_threads; i++) {

        /* Initialise cbuf */
        discs[i].read_cbuf.start = 0;
        discs[i].read_cbuf.end = 0;

        /* Initialise cbuf */
        discs[i].write_cbuf.start = 0;
        discs[i].write_cbuf.end = 0;

        /* Initilaise read/write locks */
        if (pthread_mutex_init(&(discs[i].read_lock), &attribute) != 0) {
            perror("Cannot initiliase lock");
            return -1;
        }
        if (pthread_mutex_init(&(discs[i].write_lock), &attribute) != 0) {
            perror("Cannot initiliase lock");
            return -1;
        }

        /* Initialise disc thread */
        if (pthread_create(&(discs[i].thread_id), NULL, disk_listen, &discs[i]) != 0) {
            perror("Cannot create thread.");
            return -1; 
        }   
    }
    fprintf(stdout,"\nDisc Threads Created: %d\n",i);

    return 0;
}

/* Creates and initialises all worker threads */
int create_worker_threads(int num_threads, int num_disks, int num_iterations) {
    int j;


    workers = emalloc(sizeof(worker) * num_threads);
    for(j=0; j < num_threads; j++) {
        workers[j].time = j;
        workers[j].number_of_discs = num_disks;
        // TODO don't pass these every time ovbiously..
        workers[j].repetition = num_iterations;
        workers[j].all_discs = discs;
        workers[j].file_locks = file_locks;
        if (pthread_create(&(workers[j].thread_id), NULL, worker_listen, &workers[j]) != 0) {
            perror("Cannot create thread.");
            return -1; 
        }   
    }   
    fprintf(stdout,"\nWorkers Created: %d\n", j);
    return 0;
}

/* 
 * Executes main thread. 
 * Currently performs no sanity checks on inputs
 */
int 
main(int argc, char **argv) 
{
    int num_disks = atoi(argv[1]);
    int num_worker_threads = atoi(argv[2]);
    int num_iterations = atoi(argv[3]);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    job quit_job;
    quit_job.message = QUIT;
    quit_job.communication_monitor = NULL; // for completeness. if i cared could use it to catch mistaken job
    int j;

    // make disc threads
    create_disk_threads(num_disks);

    file_locks = emalloc(sizeof(pthread_mutex_t) * NUMBER_OF_FILES); 
    
    for (j = 0; j < NUMBER_OF_FILES; j++) {
        if (pthread_mutex_init(&(file_locks[j]), &attribute) != 0) {
            perror("Cannot initiliase lock");
            return -1;
        }
    }

    // create worker threads
    create_worker_threads(num_worker_threads,num_disks, num_iterations);

    // catch all worker threads
    for(j=0; j < num_worker_threads; j++) {
        pthread_join(workers[j].thread_id, NULL); 
    }
    fprintf(stdout,"\nWorker Threads Finished: %d\n",j);

    // now workers are finished generating requests, send quit message to discs
    for(j=0; j < num_disks; j++) {
        cbuffer_add(&quit_job,&discs[j].write_cbuf); 
    }

    // catch disc threads
    for(j=0; j < num_disks; j++) {
        pthread_join(discs[j].thread_id, NULL); 
    }
    printf("Disc Threads Finished: %d\n",j);

    printf("Num disks: %d\nNum worker threads: %d\nNum iterations: %d\n"
            , num_disks, num_worker_threads, num_iterations);
    exit(0);
}
