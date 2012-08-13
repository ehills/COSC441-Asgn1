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

int
read_monitor()
{
    printf("Buffer address is %p\n", buffer);
    printf("Block number is %ld\n", block_number);
    printf("request time is %ld\n", request_time);
    receipt_time = max(request_time, receipt_time);
    printf("New receipt time is %ld\n", receipt_time);
    completion_time = 10 + (12*drand48());

    return 0;
}

int
write_monitor()
{
    printf("Buffer address is %p\n", buffer);
    printf("request time is %ld\n", request_time);
    printf("receipt time is %ld\n", receipt_time);
    printf("completion time is %ld\n", completion_time);

    return 0;
}

int rando = 1;

/* Example mutexed function */
void *
listen(void *command)
{
    
    if (rando%2 == READ) {
        pthread_mutex_lock(&read_mutex);
        printf("i: %d\n",rando);
        printf("Thread %ld has the read lock.\n", pthread_self());
        read_monitor();
        read_counter++;
        rando++;
        pthread_mutex_unlock(&read_mutex);
    } else if (rando%2 == WRITE) {
        pthread_mutex_lock(&read_mutex);
        printf("i: %d\n",rando);
        printf("Thread %ld has the write lock.\n", pthread_self());
        write_monitor();
        write_counter++;
        rando++;
        pthread_mutex_unlock(&read_mutex);
    }
    /* why do i have to return? I think its because a threaded function must 
     *      * return as its the same as the thread exiting? 
     *          */
    return 0;
}

/* 
 * Will create disk threads
 */
int
create_disk_threads(int num_threads) 
{
    int i;

    disk_thread_id = emalloc((sizeof(pthread_t)) * num_threads);

    for(i=0; i < num_threads; i++) {
        if (pthread_create(&disk_thread_id[i], NULL, listen, NULL) != 0) {
            perror("Cannot create thread.");
            return -1;
        }   
        fprintf(stderr, "Created disk thread:\t%ld\n", disk_thread_id[i]);
    } 
    return 0;
}
