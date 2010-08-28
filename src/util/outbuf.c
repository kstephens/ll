#include "outbuf.h"
#include <stdlib.h>
#ifdef MEMDEBUG
#include "memdebug/memdebug.h"
#endif
#include <string.h> /* memmove */

int outbuf_write(outbuf *ob, const void *buf, size_t size)
{
  int rtn = 0;

  if ( ! ob )
    return -1;

  if ( ! size )
    return 0;

  if ( ! buf )
    return -1;

  /* allocate buffer */
  if ( ! ob->buf ) {
    if ( ob->size < size )
      ob->size = size;
    ob->buf = malloc(ob->size + 1);
    /* resize buffer? */
  } else if ( ob->size < ob->len + size ) {
    ob->size = ob->len + size;
    ob->buf = realloc(ob->buf, ob->size + 1);
  }

  /* Append to buffer */
  memcpy(ob->buf + ob->len, buf, size);
  ob->len += size;
  ob->buf[ob->len] = '\0';

  return rtn;
}

int outbuf_flush(outbuf *ob)
{
  int rtn = 0;

  if ( ! ob )
    return 0; /* nothing to flush */

  if ( ! ob->len )
    return 0; /* nothing to flush */

  /* Don't call flush recursively */
  if ( ! ob->flushing ) {
    ob->flushing ++;

    if ( ob->flush ) {
      /* Make a copy of the buffer to allow re-entry into ob_write(), during ob->flush() */
      size_t oblen = ob->len;
      char *buf_copy = malloc(ob->len + 1);
      memcpy(buf_copy, ob->buf, ob->len + 1);

      if ( ! (rtn = (ob->flush)(ob, buf_copy, ob->len)) ) {
	/* Remove flushed portion from front */
	size_t l = ob->len - oblen;
	memmove(ob->buf, ob->buf + oblen, l);
	ob->len = l;
	ob->buf[ob->len] = '\0';
	ob->failed_flush = 0;
      } else {
	ob->failed_flush ++;
      }

      free(buf_copy);
    }

    ob->flushing --;
  }

  return rtn;
}


int outbuf_destroy(outbuf *ob)
{
  int rtn = 0;

  if ( ! ob )
    return 0;

  if ( ob->destroy ) {
    if ( ! (rtn = ob->destroy(ob)) )
      return rtn;
  }

  if ( ob->buf ) {
    free(ob->buf);
    ob->buf = 0;
  }

  return rtn;
}
