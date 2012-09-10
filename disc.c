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

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t *disk_thread_id;  /* Stores ids of all disc threads */

int read_counter = 0;
int write_counter = 0;
unsigned long block_number;
char *buffer;
unsigned long request_time;
unsigned long receipt_time;
unsigned long completion_time;

#define READ 0
#define WRITE 1

struct per_disk_file_system {

   unsigned long F;
   unsigned long f;
   unsigned long b;
   unsigned long q;
   unsigned long r;

};

int
read_monitor()
{
    printf("Buffer address is %p\n", buffer);
    printf("Block number is %ld\n", block_number);
    printf("request time is %ld\n", request_time);

    receipt_time = max(request_time, receipt_time);
    completion_time += 10 + (12*drand48());
    /*TODO add completion time its own clock time */

    printf("New receipt time is %ld\n", receipt_time);
    printf("Completed at %ld\n", completion_time);

    return 0;
}

int
write_monitor()
{
    printf("Buffer address is %p\n", buffer);
    printf("Block number is %ld\n", block_number);
    printf("request time is %ld\n", request_time);
    receipt_time = max(request_time, receipt_time);
    printf("New receipt time is %ld\n", receipt_time);
    completion_time += 10 + (12*drand48());
    printf("Completed at %ld\n", completion_time);

    return 0;
}

int rando = 1;

/* Example mutexed function */
void *
listen(void *command)
{

    // Create filesystem for disk thread
    
    // TODO: read queue for disk, depending on message, read or write.
    // while true: ?
    if (get_job(job_queue) == READ) {
        pthread_mutex_lock(&read_mutex);
        printf("i: %d\n",rando);
        printf("Thread %ld has the read lock.\n", pthread_self());
        read_monitor();
        read_counter++;
        rando++;
        pthread_mutex_unlock(&read_mutex);
    } else if (get_job(job_queue) == WRITE) {
        pthread_mutex_lock(&read_mutex);
        printf("i: %d\n",rando);
        printf("Thread %ld has the write lock.\n", pthread_self());
        write_monitor();
        write_counter++;
        rando++;
        pthread_mutex_unlock(&read_mutex);
    } else { // quit
        return 0;
    }

    return 0;
}

