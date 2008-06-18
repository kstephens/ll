#ifndef __rcs_id__
#ifndef __rcs_id_ll_debug_c__
#define __rcs_id_ll_debug_c__
static const char __rcs_id_ll_debug_c[] = "$Id: debug.c,v 1.8 2007/12/23 20:09:45 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <stdio.h>
#include <string.h> /* strchr() */
#include <stdarg.h>

void _ll_DEBUG_PRINT(const char *format, ...)
{
  static int newline = 1;
  va_list vap;
  va_start(vap, format);
  if ( newline ) {
    fprintf(stderr, "ll: debug: ");
  }
  newline = (strchr(format, '\0')[-1]) == '\n';
  vfprintf(stderr, format, vap);
  fflush(stderr);
  va_end(vap);
}


