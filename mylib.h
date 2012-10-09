#ifndef MYLIB_H_
#define MYLIB_H_

#ifndef max
    #define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef min
    #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

#define CBUF_SIZE 30
#define READ 0
#define WRITE 1
#define QUIT 2

extern  void    compute_physical_address(int, int, int *, int *, int);
extern  void    *emalloc(size_t);
extern  void    *erealloc(void *, size_t);

/**
 *  Monitor storing information for disc.
 */
typedef struct monitor_t {

    long block_number;
    char *buffer;
    long request_time;
    long receipt_time;
    long completion_time;

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
    job *jobs[CBUF_SIZE];
} circular_buffer;

/**
 *  Stores all things relating to a disc.
 */
typedef struct disc_container_t {
    pthread_t thread_id;
    pthread_mutex_t read_lock;
    int read_count;
    int write_count;
    pthread_mutex_t write_lock;
    circular_buffer cbuf;
    long disc_time;
} disc_container;

extern int   is_cb_full(circular_buffer);
extern int   is_cb_empty(circular_buffer);
extern int   cbuffer_add(job *, disc_container *);
extern job * cbuffer_get_job(disc_container *);
#endif
