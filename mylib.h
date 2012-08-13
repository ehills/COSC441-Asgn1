#ifndef MYLIB_H_
#define MYLIB_H_

#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

extern void *emalloc(size_t);
extern void *erealloc(void *, size_t);

#endif
