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
#include "cbuf.h"
#include "master.h"
#include "string.h"

/* Reads the monitor, will 'read' the file and then update its clock */
int
disk_process(monitor *mon, disc_container *this)
{
    long block_number;

   // fprintf(stderr,"TRYING TO READ");
    block_number = mon->block_number;

    memset(mon->buffer, block_number, BLOCK_SIZE);

    /* update disc thread's time */
    this->disc_time = max(this->disc_time, mon->request_time);

    /* update monitors receipt time */
    mon->receipt_time = this->disc_time;

    /* get the block and data, spin the head */
    this->disc_time += 10 + 12*drand48();

    /* store the completion time */
    mon->completion_time = this->disc_time;

    /* set completed flag */
    mon->processed = 0;
    return 0;
}

/* listen method which will poll the relevant queue for any jobs and process them accordingly*/
/* Unfairly favours reading */
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
                disk_process(disc_job->communication_monitor, disc);
            }
            pthread_mutex_unlock(&(disc->read_lock)); 
        }

        /* Check the write cbuf for requests */
        if (!is_cb_empty(&disc->write_cbuf)) {
            pthread_mutex_lock(&(disc->write_lock)); 
            disc_job = cbuffer_get_job(&disc->write_cbuf);
            if (disc_job->message == WRITE) {
                disk_process(disc_job->communication_monitor, disc);
                /* Quit message is stored here so should only be read once all other write messages have been dealt with */
                /* In saying that, current implementation only sends quit once all workers have sent their requests anyway. */
            } else if (disc_job->message == QUIT) {
                printf("Disc processed: %d messages\n", count);
                pthread_mutex_unlock(&(disc->write_lock)); 
                return 0;
            }
            pthread_mutex_unlock(&(disc->write_lock)); 
        }
        /* nothing to do, yield the cpu and come back */
        sched_yield();
    }
}
