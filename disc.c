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

pthread_t *disk_thread_id;

/* 
 * Will create disk threads
 */
int
create_disk_threads(int num_threads) 
{
    int i;

    disk_thread_id = emalloc(sizeof (pthread_t) * num_threads);

    for(i=0; i < num_threads; i++) {
        if (pthread_create(&disk_thread_id[i], NULL, example_function, NULL) != 0) {
            perror("Cannot create thread.");
            return -1;
        }   
        fprintf(stderr, "Created disk thread:\t%ld\n", disk_thread_id[i]);
    } 
    return 0;
}
