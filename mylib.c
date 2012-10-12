#include <stdlib.h>
#include "mylib.h"
#include <stdio.h>

/* Calculate address given the file and block */
/* Courtesy of Richard O'Keefe - has been modified */
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

