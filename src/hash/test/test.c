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


