#ifndef __rcs_id__
#ifndef __rcs_id_ll_debug_c__
#define __rcs_id_ll_debug_c__
static const char __rcs_id_ll_debug_c[] = "$Id: debugs.c,v 1.5 2008/05/25 22:29:08 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* strchr() */
#include <ctype.h>


ll_INIT(debug, 20, "C level debugging")
{
  static struct {
    int gi;
    const char *name;
  } x[] = {
#ifndef ll_DEBUG_def
#define ll_DEBUG_def(X) { ll_gi(_ll_DEBUG_SYM(X)), #X },
#include "ll/debugs.h"
#endif
    { -1, 0 }
  };
  int i;

  for ( i = 0; x[i].gi != -1; i ++ ) {
    char buf[32];
    char *s;
    int v;

    snprintf(buf, sizeof(buf) - 1, "ll_debug_%s", x[i].name);
    {
      char *p = buf;
      while ( *p ) {
	*p = isupper(*p) ? *p : toupper(*p);
	++ p;
      }
    }
    
    s = getenv(buf);
    v = s && *s ? atoi(s) : 0;

    if ( getenv("LL_DEBUG_INIT_DEBUG") ) {
      fprintf(stderr, "\n  %d %s %s %d", 
	      x[i].gi, 
	      x[i].name,
	      buf,
	      v);
    }

    _ll_set_gi(x[i].gi, ll_make_fixnum(v));
  }

  return 0;
}

/***************************************************************************/
