/**
 *
 * File:        disc.c
 * Author:      Edward Hills
 * Date:        7/8/12
 * Description: Deals with the creation and mangement of disc threads.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mylib.h"
#include "master.h"
#include "string.h"

/* Reads the monitor, will 'read' the file and then update its clock */
int
disk_read(monitor *read_monitor, disc_container *this)
{
    long block_number;
    char *buffer_address;

    block_number = read_monitor->block_number;

    buffer_address = read_monitor->buffer;
    memset(buffer_address, 5, BLOCK_SIZE);

    /* update disc thread's time */
    this->disc_time = max(this->disc_time, read_monitor->request_time);

    /* update monitors receipt time */
    read_monitor->receipt_time = this->disc_time;

    /* get the block and data, spin the head */
    this->disc_time += 10 + (12*drand48());

    /* store the completion time */
    read_monitor->completion_time = this->disc_time;

//    fprintf(stderr,"\nREAD\tdisc: %ld\tblock: %ld\tbuffer: %p\trequest time: %ld\tcompletion time: %ld\n\n", (unsigned long)pthread_self(), block_number, buffer_address, read_monitor->request_time, this->disc_time);
    return 0;
}

/* Reads the monitor, will 'write' the file and then update its clock */
int
disk_write(monitor *write_monitor, disc_container *this)
{
    long block_number;
    char *buffer_address;

    block_number = write_monitor->block_number;

    buffer_address = write_monitor->buffer;

    /* update disc thread's time */
    this->disc_time = max(this->disc_time, write_monitor->request_time);

    /* update monitors receipt time */
    write_monitor->receipt_time = this->disc_time;

    /* get the block and data, spin the head */
    this->disc_time += 10 + (12*drand48());

    /* store the completion time */
    write_monitor->completion_time = this->disc_time;

  //  fprintf(stderr,"\nWRITE\tdisc: %ld\tblock: %ld\tbuffer: %p\trequest time: %ld\tcompletion time: %ld\n\n", (unsigned long)pthread_self(),block_number, buffer_address, write_monitor->request_time, this->disc_time);

    return 0;
}

/* listen method which will poll the relevant queue for any jobs and process them accordingly*/
/* Unfairly favours reading due to re-write */
void *
disk_listen(disc_container *disc)
{
    job *disc_job;

    for (;;) {
        /* Check the read cbuf for requests */
        if (!is_cb_empty(&disc->read_cbuf)) {
            pthread_mutex_lock(&(disc->read_lock)); 
            disc_job = cbuffer_get_job(&disc->read_cbuf);
            if (disc_job->message == READ) {
                disk_read(disc_job->communication_monitor, disc);
            }
            pthread_mutex_unlock(&(disc->read_lock)); 
        } else {
  //          printf("Nothing to read\n");
        }

        /* Check the write cbuf for requests */
        if (!is_cb_empty(&disc->write_cbuf)) {
            pthread_mutex_lock(&(disc->write_lock)); 
            disc_job = cbuffer_get_job(&disc->write_cbuf);
            if (disc_job->message == WRITE) {
                disk_write(disc_job->communication_monitor, disc);
            } else if (disc_job->message == QUIT) {
                pthread_mutex_unlock(&(disc->write_lock)); 
                printf("QUITTING\n");
                return 0;
            }
            pthread_mutex_unlock(&(disc->write_lock)); 
        } else {

    //        printf("Nothing to write\n");
        }
    }
}
