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

/* Reads the monitor, will 'read' the file and then update its clock */
int
disk_read(monitor *read_monitor, disc_container *this)
{

    this->disc_time = max(this->disc_time, read_monitor->request_time);
    read_monitor->receipt_time = this->disc_time;
    this->disc_time += 10 + (12*drand48());
    read_monitor->completion_time = this->disc_time;

    fprintf(stderr,"\nREAD\trequest time: %ld\treceipt time: %ld\tcompletion time: %ld\n\n", read_monitor->request_time, read_monitor->receipt_time, this->disc_time);
    return 0;
}

/* Reads the monitor, will 'write' the file and then update its clock */
int
disk_write(monitor *write_monitor, disc_container *this)
{
    this->disc_time = max(this->disc_time, write_monitor->request_time);
    write_monitor->receipt_time = this->disc_time;
    this->disc_time += 10 + (12*drand48());
    write_monitor->completion_time = this->disc_time;

    fprintf(stderr,"\nWRITE\trequest time: %ld\treceipt time: %ld\tcompletion time: %ld\n\n", write_monitor->request_time, write_monitor->receipt_time, this->disc_time);

    return 0;
}

/* listen method which will poll the relevant queue for any jobs and process them accordingly*/
void *
disk_listen(disc_container *disc)
{
    job *disc_job;

    for (;;) {
        if (!is_cb_empty(disc->cbuf)) {
            disc_job = cbuffer_get_job(disc);
            if (disc_job->message == READ) {
//                fprintf(stderr, "Disc Thread %ld just read the READ command\n", disc->thread_id);
                disk_read(disc_job->communication_monitor, disc);
            } else if (disc_job->message == WRITE) {
  //              fprintf(stderr, "Disc Thread %ld just read the WRITE command\n", disc->thread_id);
                disk_write(disc_job->communication_monitor, disc);
            } else if (disc_job->message == QUIT) {
//                fprintf(stderr, "Disc Thread %ld just read the QUIT command\n", disc->thread_id);
                return 0;
            }
        } else {
            //fprintf(stderr, "Thread %ld has no messages\n", disc->thread_id);
        }
    }
}

