#ifndef __rcs_id__
#ifndef __rcs_id_ll_assert_c__
#define __rcs_id_ll_assert_c__
static const char __rcs_id_ll_assert_c[] = "$Id: assert.c,v 1.2 2001/01/15 03:44:46 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "assert.h"
#include <stdarg.h>


static int _abort_code = 0;


void __ll_assert_1(const char *name, const char *expr, int code, const char *file, int line, const char *func)
{
  _abort_code = code;
  fprintf(stderr, "\nll: %s, assertion: \"%s\", file: \"%s\", line: %d, func: \"%s\"",
	  name,
	  expr,
	  file,
	  line,
	  func ? func : "<unknown>"
	  );
}


void __ll_assert_2(const char *format, ...)
{
  va_list(vap);

  va_start(vap,format);
  if ( format && format[0] ) {
    fprintf(stderr, ", msg: ");
    vfprintf(stderr, format, vap);
  }
  fprintf(stderr, "\n");
  va_end(vap);

  if ( _abort_code )
    ll_abort();

  _abort_code = 0;
}

