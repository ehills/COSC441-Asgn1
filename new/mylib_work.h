#ifndef MYLIB_H_
#define MYLIB_H_

#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#define CBUF_SIZE 1024
#define READ 0
#define WRITE 1
#define QUIT 2
#define NUMBER_OF_FILES 1000
#define BLOCK_SIZE_IN_KIB 4
#define BLOCK_SIZE (4 * 1024)
#define BLOCKS_PER_FILE (1000 * 1000 / BLOCK_SIZE_IN_KIB)

extern  void    compute_physical_address(int, int, int *, int *, int);
extern  void    *emalloc(size_t);
extern  void    *erealloc(void *, size_t);

/**
 *  Monitor storing information for disc.
 */
typedef struct monitor_t {

    long block_number;
    long request_time;
    long receipt_time;
    long completion_time;
    char *buffer;
} monitor;

/**
 *  Job containng message and monitor.
 */
typedef struct job_t {
    int message;
    monitor *communication_monitor;
} job;

/**
 *  Circular Buffer to store disc jobs
 */
typedef struct circular_buffer_t {
    int start;
    int count;
    int end;
    job *jobs[CBUF_SIZE];
} circular_buffer;

/**
 *  Stores all things relating to a disc.
 */
typedef struct disc_container_t {
    long disc_time;
    pthread_t thread_id;
    pthread_mutex_t read_lock;
    pthread_mutex_t write_lock;
    circular_buffer read_cbuf;
    circular_buffer write_cbuf;
} disc_container;

extern int   is_cb_full(circular_buffer *);
extern int   is_cb_empty(circular_buffer *);
extern int   cbuffer_add(job *, circular_buffer *);
extern job * cbuffer_get_job(circular_buffer *);
#endif
