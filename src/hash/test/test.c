#ifndef __rcs_id__
#ifndef __rcs_id_test_test_c__
#define __rcs_id_test_test_c__
static const char __rcs_id_test_test_c[] = "$Id: test.c,v 1.3 1999/10/12 07:27:08 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "charP_int_Table.h"

int main(int argc, char **argv)
{
  charP_int_Table _t, *t = &_t;

  charP_int_TableInit(t, 5);
  charP_int_TableAdd(t, "foo", 4);
  assert(*charP_int_TableGet(t, "foo") == 4);

  charP_int_TableRemove(t, "foo");
  assert(charP_int_TableGet(t, "foo") == 0);

  return 0;
}


