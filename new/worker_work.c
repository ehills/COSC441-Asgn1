/**
 *
 * File:        worker.c
 * Author:      Edward Hills
 * Date:        7/8/12
 * Description: Deals with the creation and mangement of disc threads.
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
            x = (long)file_number * BLOCKS_PER_FILE + block_number;
            *which_disc  = x % number_of_discs;
            *which_block = x / number_of_discs;
        }
    } 
}

int worker_listen(disc_container *all_discs) {

    int repetition = 125000;  
    int i, b;
    int number_discs= 16;
    monitor *read_mon;          /* temp monitor storage */
    monitor *write_mon;
    int in_file = -1;           /* the files */
    int out_file = -1;
    char *in_buffer;            /* location for monitors */
    char *out_buffer;
    job *read_msg;              /* temp job storage */
    job *write_msg;

    int what_disc = 0;
    int what_block = 0;

    char in_buff1[BLOCK_SIZE];  /* read-ahead buffers */
    char in_buff2[BLOCK_SIZE];
    char in_buff3[BLOCK_SIZE];
    char in_buff4[BLOCK_SIZE];

    char out_buff1[BLOCK_SIZE]; /* write-behind buffers */
    char out_buff2[BLOCK_SIZE];
    char out_buff3[BLOCK_SIZE];
    char out_buff4[BLOCK_SIZE];

    long thread_time = 0;

    for (i = 0; i < repetition; i++) {
        while (out_file == in_file) {
            in_file = (int)(drand48() * 1000);
            printf("in: %d\n", in_file);
            out_file = (int)(drand48() * 1000);
            printf("out: %d\n", out_file);
        }

        // initiliase monitors
        read_mon = emalloc(sizeof(monitor)); 
        write_mon = emalloc(sizeof(monitor)); 

        // make made up buffers
        in_buffer = emalloc(BLOCK_SIZE * sizeof(char));
        memset(in_buffer, 0, BLOCK_SIZE);

        out_buffer = emalloc(BLOCK_SIZE * sizeof (char));
        memset(out_buffer, 0, BLOCK_SIZE);


        // TODO need to get actual values
        /* Create read monitor */
        read_mon->block_number = 1;
        read_mon->buffer = in_buffer;
        read_mon->request_time = thread_time;

        /* Create read job. mainly used for original implementation */
        read_msg = emalloc(sizeof(job));
        read_msg->message = READ;
        read_msg->communication_monitor = read_mon;

        /* Create write monitor */
        write_mon->block_number = 2;
        write_mon->buffer = out_buffer;
        write_mon->request_time = thread_time;

        /* Create write job. mainly used for original implementation */
        write_msg = emalloc(sizeof(job));
        write_msg->message = WRITE;
        write_msg->communication_monitor = write_mon;

        // TODO --- get file location and then send it to proper disk. hard coded for now

        /* Procedurally sends requests to respective disc */
        /* Can be moddelled as two input / output threads, possible TODO */
        if (in_file < out_file) {

            for (b=0; b < BLOCKS_PER_FILE; b++) {
                compute_physical_address(in_file, b, &what_disc, &what_block, number_discs);

                pthread_mutex_lock(&all_discs[what_disc].read_lock);

                cbuffer_add(read_msg,&all_discs[what_disc].read_cbuf);

                pthread_mutex_unlock(&all_discs[what_disc].read_lock);
            }



            for (b=0; b < BLOCKS_PER_FILE; b++) {
                compute_physical_address(out_file, b, &what_disc, &what_block, number_discs);

                pthread_mutex_lock(&all_discs[what_disc].write_lock);

                cbuffer_add(write_msg,&all_discs[what_disc].write_cbuf);

                pthread_mutex_unlock(&all_discs[what_disc].write_lock);
            }

        } else {

            for (b=0; b < BLOCKS_PER_FILE; b++) {
                compute_physical_address(out_file, b, &what_disc, &what_block, number_discs);

                pthread_mutex_lock(&all_discs[what_disc].write_lock);

                cbuffer_add(write_msg,&all_discs[what_disc].write_cbuf);

                pthread_mutex_unlock(&all_discs[what_disc].write_lock);
            }



            for (b=0; b < BLOCKS_PER_FILE; b++) {
                compute_physical_address(in_file, b, &what_disc, &what_block, number_discs);

                pthread_mutex_lock(&all_discs[what_disc].read_lock);

                cbuffer_add(read_msg,&all_discs[what_disc].read_cbuf);

                pthread_mutex_unlock(&all_discs[what_disc].read_lock);
        }

    }

}
/* TODO so can either send it the discs pointer like i am now, and add a worker struct into that... or.... just send this method a worker struct that also has a pointer to the discs thing. that way it can access the locks and whatn ot but being a little less obvious */
//    printf("Worker took %ld\n", );
return 0;
}
