#ifndef __rcs_id__
#ifndef __rcs_id_ll_ccont_c__
#define __rcs_id_ll_ccont_c__
static const char __rcs_id_ll_ccont_c[] = "$Id: ccont.c,v 1.1 2007/09/16 16:19:37 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "ccont/ccont.h"
#define CCONT_MALLOC ll_malloc
#define CCONT_FREE   ll_free
#include "ccont/ccont.c"



