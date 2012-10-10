#include <stdlib.h>
#include "mylib.h"
#include <stdio.h>



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


