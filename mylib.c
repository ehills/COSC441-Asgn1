#include <stdlib.h>
#include "mylib.h"
#include <stdio.h>

#define READ 0
#define WRITE 1
#define QUIT 2
#define NUMBER_OF_FILES = 1000
#define BLOCK_SIZE_IN_KIB 4
#define BLOCK_SIZE (BLOCK_IN_KIB * 1024)
#define BLOCKS_PER_FILE (1000 * 1000 / BLOCK_SIZE_IN_KIB)

void compute_physical_address(
    int file_number,        // f
    int block_number,       // b
    int *which_disc,        // r
    int *which_block,        // q
    int const number_discs
) {
/*    long x;
    if((file_number >= 0) && (file_number < NUMBER_OF_FILES)) {
        if (0 <= block_number && block_number < BLOCKS_PER_FILE) {
            x = (long)file_number * BLOCKS_PER_FILE + block_number;
            *which_disc  = x % number_of_discs;
            *which_block = x / number_of_discs;
        }
    } */
}

/* emalloc with error checking */
void *emalloc(size_t s){
    void *result = malloc(s);
    if (NULL == result){
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

/* erealloc with error checking */
void *erealloc(void *p, size_t s){
    void *result = realloc(p, s);
    if (NULL == result){
        perror("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return result;
}


