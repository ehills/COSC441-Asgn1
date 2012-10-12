#ifndef CBUF_H_
#define CBUF_H_

extern int  is_cb_full      (circular_buffer *);
extern int  is_cb_empty     (circular_buffer *);
extern int  cbuffer_add     (job *, circular_buffer *cbuf);
extern job *cbuffer_get_job (circular_buffer *);

#endif
