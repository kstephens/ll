#ifndef _util_outbuf_h
#define _util_outbuf_h

#include <stddef.h> /* size_t */

typedef struct outbuf {
  int (*flush)(struct outbuf *ob, void *buf, size_t size); /* flush method.  Can modify buf. */
  int (*destroy)(struct outbuf *ob);  /* destroy method. */

  void *data[4]; /* user data. */

  char *buf;    /* the buffered data. */
  size_t len;   /* length of current buf contents. */
  size_t size;  /* size of buf. */

  int failed_flush; /* no. of failed flush ops. */

  int flushing; /* currently flushing? */
} outbuf;

int outbuf_write(outbuf *ob, const void *buf, size_t size);
int outbuf_flush(outbuf *ob);
int outbuf_destroy(outbuf *ob);

#endif
