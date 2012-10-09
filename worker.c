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
#define BUFFER_SIZE 4096

int worker_listen(disc_container *all_discs) {

    int repetition = 3; 
    int iteration; 
    monitor *read_mon;
    monitor *write_mon;
    int in_file = -1;
    int out_file = -1;
    char *in_buffer;
    char *out_buffer;
    job *read_msg;
    job *write_msg;

    printf("Worker starting\n");

    long thread_time = 0;

    for (iteration = 0; iteration < repetition; iteration++) {
        while (out_file == in_file) {
            in_file = (int)(drand48() * 1000);
            out_file = (int)(drand48() * 1000);
        }

        // initiliase monitors
        read_mon = emalloc(sizeof(monitor)); 
        write_mon = emalloc(sizeof(monitor)); 

        // make made up buffers
        // using CBUF_SIZE for convienience. name should be changed
        in_buffer = emalloc(BUFFER_SIZE);
        memset(in_buffer, 0, BUFFER_SIZE);

        out_buffer = emalloc(BUFFER_SIZE);
        memset(out_buffer, 0, BUFFER_SIZE);


        if (in_file < out_file) {
       
        // for disc i in all_disks {
        
          //  for block j in all blocks {   

            // write stuff in monitor
            // TODO need to get actual values
            read_mon->block_number = 1;
            read_mon->buffer = in_buffer;
            read_mon->request_time = thread_time;

            read_msg = emalloc(sizeof(job));
            read_msg->message = READ;
            read_msg->communication_monitor = read_mon;

            write_mon->block_number = 2;
            write_mon->buffer = out_buffer;
            write_mon->request_time = thread_time;

            write_msg = emalloc(sizeof(job));
            write_msg->message = WRITE;
            write_msg->communication_monitor = write_mon;

            // send requests to disc
            // TODO --- get file location and then send it to proper disk. hard coded for now
            cbuffer_add(read_msg,&all_discs[0]);
            cbuffer_add(write_msg,&all_discs[0]);

         //}
       //}
        }
    }
    return 0;
}
