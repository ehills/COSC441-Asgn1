/**
 *
 * File:        worker.c
 * Author:      Edward Hills
 * Date:        7/8/12
 * Description: Deals with the creation and mangement of disc threads.
 *              Procedurally sends requests to respective disc
 *              Can be moddelled as two input / output threads, possible TODO 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mylib.h"
#include "cbuf.h"
#include "string.h"
#include "master.h"
#include "disc.h"


/* Different Seed for each thread */
void seed_rand(struct drand48_data *buffer)
{
    unsigned long seed;
    seed = (unsigned long)pthread_self();
    srand48_r(seed, buffer);
}

/* Will check the monitor and see if the 'done' flag has been set */
/* This is not ideal and needs to be reimplemented */
/* This now does not allow the worker to send multiple requests */
/* FIXME use signalling with conditions */
int check_reply(monitor *request, worker *this) {
    //pthread_mutex_lock(&request->processed_lock);
//    pthread_cond_wait(&request->processed_cond, &request->processed_lock);
//    this->time = max(this->time, request->completion_time);      
//    pthread_mutex_unlock(&request->processed_lock);

    while (request->processed != 0) {
      sched_yield();
    }
    this->time = max(this->time, request->completion_time);

    return 0;
}

/* live free or die hard */
int free_messages(job *jobby, monitor *mon) {

    free(mon);
    free(jobby);

    return 0;
}

/* send request to disc */
int send_request(pthread_mutex_t *lock, job *job, circular_buffer *cbuf) {
    pthread_mutex_lock(lock);

    while (cbuffer_add(job,cbuf) == -1) {
        pthread_mutex_unlock(lock);
        sched_yield();
        pthread_mutex_lock(lock);
    }

    pthread_mutex_unlock(lock);
    return 0;
}

/* Threads will start here */
int worker_listen(worker *worker) {

    int i, b;
    int total_messages = 0;
    monitor *read_mon;              /* temp monitor storage */
    monitor *write_mon;
    int in_file = -1;
    int out_file = -1;              /* the files */
    char *in_buffer, *out_buffer;   /* i/o buffers */
    job *read_msg, *write_msg;      /* temp job storage */

    int what_disc = 0;
    int what_block = 0;

    double rand_result;
    struct drand48_data work_space;
    pthread_mutexattr_t attribute; 
    pthread_mutexattr_settype(&attribute, PTHREAD_MUTEX_ERRORCHECK);
#if 0
    /* TODO: have this in an array ovbiously */
    char in_buff1[BLOCK_SIZE];  /* read-ahead buffers */
    char in_buff2[BLOCK_SIZE];
    char in_buff3[BLOCK_SIZE];
    char in_buff4[BLOCK_SIZE];

    char out_buff1[BLOCK_SIZE]; /* write-behind buffers */
    char out_buff2[BLOCK_SIZE];
    char out_buff3[BLOCK_SIZE];
    char out_buff4[BLOCK_SIZE];
#endif

    seed_rand(&work_space);
    /* Get in/out file for each repetition desired */
    for (i = 0; i < worker->repetition; i++) {
        in_file = -1;
        out_file= -1;
        while (out_file == in_file) {
            drand48_r(&work_space, &rand_result);
            in_file = (int)(rand_result * 1000);
            drand48_r(&work_space, &rand_result);
            out_file = (int)(rand_result * 1000);
        }

        // make made up buffers
        in_buffer = emalloc(BLOCK_SIZE * sizeof(char));
        memset(in_buffer, 0, BLOCK_SIZE);

        out_buffer = emalloc(BLOCK_SIZE * sizeof (char));
        memset(out_buffer, 0, BLOCK_SIZE);

        // lock infile
        // lock outfile
        for (b=0; b < BLOCKS_PER_FILE; b++) {

            /* Create read monitor */
            read_mon = emalloc(sizeof(monitor)); 
            read_mon->buffer = in_buffer;
            read_mon->request_time = worker->time;
            read_mon->processed = -1;
            pthread_mutex_init(&(read_mon->processed_lock), &attribute);
            pthread_cond_init(&read_mon->processed_cond, NULL);

            /* Create read job. mainly used for original implementation */
            read_msg = emalloc(sizeof(job));
            read_msg->message = READ;
            read_msg->communication_monitor = read_mon;

            /* Create write monitor */
            write_mon = emalloc(sizeof(monitor)); 
            write_mon->buffer = out_buffer;
            write_mon->request_time = worker->time;
            write_mon->processed = -1;
            pthread_mutex_init(&(write_mon->processed_lock), &attribute);
            pthread_cond_init(&write_mon->processed_cond, NULL);

            /* Create write job. mainly used for original implementation */
            write_msg = emalloc(sizeof(job));
            write_msg->message = WRITE;
            write_msg->communication_monitor = write_mon;

            /* Find what disc and block to read from */
            compute_physical_address(in_file, b, &what_disc, &what_block, worker->number_of_discs);
            read_mon->block_number = what_block;

            compute_physical_address(out_file, b, &what_disc, &what_block, worker->number_of_discs);
            write_mon->block_number = what_block;

            /* will check the 'fd' of the in and out files then grab the read/write lock 
             * check if the disc is full, if it is, unlock and schedule out. otherwise 
             * add message and check reply */
            if (in_file < out_file) {
                // read
                pthread_mutex_lock(&(worker->file_locks[in_file])); 
                send_request(&worker->all_discs[what_disc].read_lock, 
                            read_msg, &worker->all_discs[what_disc].read_cbuf);
                check_reply(read_mon, worker);
                pthread_mutex_unlock(&(worker->file_locks[in_file])); 

                worker->time++;

                // write
                pthread_mutex_lock(&(worker->file_locks[out_file])); 
                send_request(&worker->all_discs[what_disc].write_lock,
                        write_msg, &worker->all_discs[what_disc].write_cbuf);
                check_reply(write_mon, worker);
                pthread_mutex_unlock(&(worker->file_locks[out_file])); 

            } else {

                // write
                pthread_mutex_lock(&(worker->file_locks[out_file])); 
                send_request(&worker->all_discs[what_disc].write_lock,
                        write_msg, &worker->all_discs[what_disc].write_cbuf);
                check_reply(write_mon, worker);
                pthread_mutex_unlock(&(worker->file_locks[out_file])); 

                worker->time++;

                // read 
                pthread_mutex_lock(&(worker->file_locks[in_file])); 
                send_request(&worker->all_discs[what_disc].read_lock, 
                            read_msg, &worker->all_discs[what_disc].read_cbuf);
                check_reply(read_mon, worker);
                pthread_mutex_unlock(&(worker->file_locks[in_file])); 
            }
           // fprintf(stderr,"Sent message\n");
            // deal with one file at a time
            total_messages++;
            free_messages(write_msg, write_mon);
            free_messages(read_msg, read_mon);
        }
        free(in_buffer);
        free(out_buffer);

        // TODO TODO TODO - maybe not the below solution. may have to do it for each block 
        // checks replies after all blocks for the file have been queued
        // Figured its better to send the requests to disc first as it is the choke point
        // check reply queue and read replies here 

    }
    printf("Worker took: %ld\nnum messages: %d\n", worker->time,total_messages);
    return 0;
}
