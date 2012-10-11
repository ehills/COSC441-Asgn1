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
#include "string.h"
#include "mylib.h"
#include "master.h"
#include "disc.h"

void compute_physical_address(
    int file_number,        // f
    int block_number,       // b
    int *which_disc,        // r
    int *which_block,        // q
    int const number_of_discs
) {
    long x;
    if ((file_number >= 0) && (file_number < NUMBER_OF_FILES)) {
        if (0 <= block_number && block_number < BLOCKS_PER_FILE) {
            x = (long)(file_number * BLOCKS_PER_FILE + block_number);
            *which_disc  = x % number_of_discs;
            *which_block = x / number_of_discs;
        }
    } 
}

/* Different Seed for each thread */
void seed_rand(struct drand48_data *buffer)
{
    unsigned long seed;
    seed = (unsigned long)pthread_self();
    srand48_r(seed, buffer);
}

/* Threads will start here */
int worker_listen(worker *worker) {

    int i, b;
    monitor *read_mon;              /* temp monitor storage */
    monitor *write_mon;
    int in_file = -1;
    int out_file = -1;     /* the files */
    char *in_buffer, *out_buffer;   /* i/o buffers */
    job *read_msg, *write_msg;       /* temp job storage */

    int res = -1;
    int what_disc = 0;
    int what_block = 0;

    double rand_result;
    struct drand48_data work_space;

#if 0
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

        for (b=0; b < BLOCKS_PER_FILE; b++) {

            /* Create read monitor */
            read_mon = emalloc(sizeof(monitor)); 
            read_mon->buffer = in_buffer;
            read_mon->request_time = worker->time;

            /* Create read job. mainly used for original implementation */
            read_msg = emalloc(sizeof(job));
            read_msg->message = READ;
            read_msg->communication_monitor = read_mon;

            /* Create write monitor */
            write_mon = emalloc(sizeof(monitor)); 
            write_mon->buffer = out_buffer;
            write_mon->request_time = worker->time;

            /* Create write job. mainly used for original implementation */
            write_msg = emalloc(sizeof(job));
            write_msg->message = WRITE;
            write_msg->communication_monitor = write_mon;

            compute_physical_address(in_file, b, &what_disc, &what_block, worker->number_of_discs);
            read_mon->block_number = what_block;

            compute_physical_address(out_file, b, &what_disc, &what_block, worker->number_of_discs);
            write_mon->block_number = what_block;

            if (in_file < out_file) {
                // read
                pthread_mutex_lock(&worker->all_discs[what_disc].read_lock);

                cbuffer_add(read_msg,&worker->all_discs[what_disc].read_cbuf);

                pthread_mutex_unlock(&worker->all_discs[what_disc].read_lock);

                // write
                pthread_mutex_lock(&worker->all_discs[what_disc].write_lock);

                cbuffer_add(write_msg,&worker->all_discs[what_disc].write_cbuf);

                pthread_mutex_unlock(&worker->all_discs[what_disc].write_lock);

            } else {

                // write
                pthread_mutex_lock(&worker->all_discs[what_disc].write_lock);

    //            while (is_cb_full(&worker->all_discs[what_disc].write_cbuf) == 0) {
  //                  sched_yield();
//                }
                cbuffer_add(write_msg,&worker->all_discs[what_disc].write_cbuf);

                pthread_mutex_unlock(&worker->all_discs[what_disc].write_lock);

                // read 
                pthread_mutex_lock(&worker->all_discs[what_disc].read_lock);

                cbuffer_add(read_msg,&worker->all_discs[what_disc].read_cbuf);

                pthread_mutex_unlock(&worker->all_discs[what_disc].read_lock);
            }

        }

        printf("Sent write requests for file: %d\n",out_file);
        printf("Sent read requests for file: %d\n",in_file);

        // TODO TODO TODO - maybe not the below solution. may have to do it for each block 
        // checks replies after all blocks for the file have been queued
        // Figured its better to send the requests to disc first as it is the choke point
        // check reply queue and read replies here 

    }
    printf("Worker took: %ld\n", worker->time);
    //    printf("Worker took %ld\n", );
    return 0;
}
