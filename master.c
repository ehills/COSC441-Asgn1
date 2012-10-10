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
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "mylib.h"
#include "disc.h"
#include "worker.h"

disc_container *discs;
pthread_mutexattr_t attribute;

/**
 *  * Check circular buffer is full
 *   */
int is_cb_full(circular_buffer cbuf) {
    return cbuf.count == CBUF_SIZE;
}

/*
 * Check whether circular buffer is empty.
 * Doesn't maintain a lock, is not crucial to have accurate reading.
 */
int is_cb_empty(circular_buffer cbuf) {
    return cbuf.count == 0;
}

/**
 *  Add job into circular buffer.
 */
int cbuffer_add(job *entry, disc_container *disc) {
    int end;
    pthread_mutex_lock(&disc->write_lock);
    fprintf(stderr,"WRITE LOCK pointer: %ld\n",(long)&(disc->write_lock));

    fprintf(stderr, "Adding job %d to disc: %ld\n",entry->message,disc->thread_id); 

    if (is_cb_full(disc->cbuf)) {
        pthread_mutex_unlock(&disc->write_lock);
        return -1;
    }

    end = (disc->cbuf.start + disc->cbuf.count) % CBUF_SIZE;

    disc->cbuf.jobs[end] = entry;
    disc->cbuf.count++;

    fprintf(stderr,"WRITE UNLOCK pointer: %ld\n",(long)&(disc->write_lock));
    pthread_mutex_unlock(&(disc->write_lock));
    return 0;
}

/**
 *  Remove job from circular buffer.
 */
job *cbuffer_get_job(disc_container *disc) {
    job *entry;

    pthread_mutex_lock(&disc->read_lock);

    fprintf(stderr,"READ LOCK pointer: %ld\n",(long)&(disc->read_lock));

    entry = disc->cbuf.jobs[disc->cbuf.start];
    disc->cbuf.start = (disc->cbuf.start + 1) % CBUF_SIZE;
    disc->cbuf.count--;

    fprintf(stderr, "Reading job %d from disc: %ld\n",entry->message,disc->thread_id); 


    fprintf(stderr,"READ UNLOCK pointer: %ld\n",(long)&(disc->read_lock));

    pthread_mutex_unlock(&disc->read_lock);
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
    printf("\ne.g.\t./simulation 16 16 125000\n");
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
        discs[i].cbuf.start = 0;
        discs[i].cbuf.count = 0;

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
        fprintf(stderr, "Created disk thread:\t%ld\n", discs[i].thread_id);
    }
    fprintf(stdout,"\nDisc Threads Created: %d\n\n",i);

    return 0;
}


/* 
 * Executes main thread. 
 */
    int 
main(int argc, char **argv) 
{
    int num_disks = atoi(argv[1]);
    int num_worker_threads = atoi(argv[2]);
    pthread_t worker_thread_id[num_worker_threads]; 
    int num_iterations = atoi(argv[3]);
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
    job quit_job;
    quit_job.message = QUIT;
    int j;

    /* Will not perform any sanity checks */
    if (argc < 2) {
        fprintf(stderr, "You must enter at least one argument.\n");
        usage();
        exit(1);
    } 

    fprintf(stderr,"making disk threads\n");
    create_disk_threads(num_disks);

    for(j=0; j < num_worker_threads; j++) {
        if (pthread_create(&worker_thread_id[j], NULL, worker_listen, discs) != 0) {
            perror("Cannot create thread.");
            return -1; 
        }   
        fprintf(stderr, "Created worker thread:\t%ld\n", worker_thread_id[j]);
    }   
    fprintf(stdout,"\nWorkers Created: %d\n\n", j);

    for(j=0; j < num_worker_threads; j++) {
        pthread_join(worker_thread_id[j], NULL); 
    }
    fprintf(stdout,"\nWorker threads finished: %d\n",j);

    for(j=0; j < num_disks; j++) {
        cbuffer_add(&quit_job,&discs[j]); 
        fprintf(stdout,"Sending QUIT message to disc %ld\n", discs[j].thread_id);
    }

    for(j=0; j < num_disks; j++) {
        pthread_join(discs[j].thread_id, NULL); 
        fprintf(stdout,"\nDisc Thread %ld quit. Total: %d\n", discs[j].thread_id,j);
    }

    printf("Num disks: %d\nNum worker threads: %d\nNum iterations: %d\n"
            , num_disks, num_worker_threads, num_iterations);
    exit(0);
}
