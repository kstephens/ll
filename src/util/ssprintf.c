#ifndef __rcs_id__
#ifndef __rcs_id_util_ssprintf_c__
#define __rcs_id_util_ssprintf_c__
static const char __rcs_id_util_path_c[] = "$Id: ssprintf.c,v 1.2 2000/01/04 06:22:24 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "ssprintf.h"


char *ssprintf(char *str, const char *format, ...)
{
  size_t len;
  size_t flen;

  len = str ? strlen(str) : 0;
  
  /* Calc append length */
  flen = strlen(format);
  {
    const char *f;
    va_list vap;

    va_start(vap, format);
    f = format;

    while ( *f ) {
      if ( *f == '%' ) {
	int l = 0;
	int w = 0;
	int more = 0;

	f ++;
	do {
	  int fw = 0;

	  more = 0;
	  switch ( *(f ++) ) {
	  case 's':
	    fw = strlen(va_arg(vap, char*));
	    break;

	  case '*':
	    fw = va_arg(vap, int);
	    break;

	  case 'l':
	    l ++;
	    more = 1;
	    break;

	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	    w *= 10;
	    w += f[-1] - '0';
	    more = 1;
	    break;

	  case '-': case '+': case '.':
	    more = 1;
	    break;

          case 'f': case 'g': case 'e':
	    (void) va_arg(vap, double);
	    fw = 32;
	    break;

          case 'd': case 'u': case 'o': case 'x': case 'c':
	    if ( l ) {
	      (void) va_arg(vap, int);
	    } else {
	      (void) va_arg(vap, long);
	    }
	    flen += 32;
	    break;
	  
	  case 'p':
	    (void) va_arg(vap, void*);
	    fw = 32;
	    break;

	  case '\0':
	    break;
	    
	  case '%':
	    break;

	  default:
	    abort();
	    break;
	  }

	  if ( fw < w ) {
	    fw = w;
	  }
	  flen += fw;

	} while ( more && *f );
      } else {
	f ++;
      }
    }

    va_end(vap);
  }

  /* re/alloc buffer */
  if ( str ) {
    str = realloc(str, len + flen + 1);
  } else {
    str = malloc(flen + 1);
  }

  /* append to buffer */
  {
    va_list vap;

    va_start(vap, format);
    vsprintf(str + len, format, vap);
    va_end(vap);
  }

  return str;
}

