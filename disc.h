#ifndef DISK_H_
#define DISK_H_

extern void *listen(void *);
extern int write_monitor(int);
extern int read_monitor(int);
extern int create_disk_threads(int);

#endif
