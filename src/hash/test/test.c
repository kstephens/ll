#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "charP_int_Table.h"

static
int table_count(charP_int_Table *t)
{
  int count = 0;
  charP_int_TableIterator i;
  charP_int_TableIteratorInit(t, &i);
  while ( charP_int_TableIteratorNext(t, &i) ) {
    ++ count;
  }
  return count;
}

int main(int argc, char **argv)
{
  charP_int_Table _t, *t = &_t;

  charP_int_TableInit(t, 5);
  assert(charP_int_TableNEntries(t) == 0);
  assert(table_count(t) == 0);
  assert(charP_int_TableSize(t) == 5);

  charP_int_TableAdd(t, "foo", 4);
  assert(*charP_int_TableGet(t, "foo") == 4);
  assert(charP_int_TableNEntries(t) == 1);
  assert(table_count(t) == 1);
  assert(charP_int_TableSize(t) == 5);

  charP_int_TableAdd(t, "bar", 5);
  assert(*charP_int_TableGet(t, "foo") == 4);
  assert(*charP_int_TableGet(t, "bar") == 5);
  assert(charP_int_TableNEntries(t) == 2);
  assert(table_count(t) == 2);
  assert(charP_int_TableSize(t) == 5);

  charP_int_TableStats(t, stdout);
  assert(charP_int_TableRemove(t, "foo") == 1);
  assert(charP_int_TableGet(t, "foo") == 0);
  assert(*charP_int_TableGet(t, "bar") == 5);
  assert(charP_int_TableNEntries(t) == 1);
  assert(table_count(t) == 1);
  assert(charP_int_TableSize(t) == 5);

  assert(charP_int_TableRemove(t, "bar") == 1);
  assert(charP_int_TableGet(t, "foo") == 0);
  assert(charP_int_TableGet(t, "bar") == 0);
  assert(charP_int_TableNEntries(t) == 0);
  assert(table_count(t) == 0);
  assert(charP_int_TableSize(t) == 5);

  assert(charP_int_TableRemove(t, "foo") == 0);
  assert(charP_int_TableRemove(t, "bar") == 0);
  assert(charP_int_TableGet(t, "foo") == 0);
  assert(charP_int_TableGet(t, "bar") == 0);
  assert(charP_int_TableNEntries(t) == 0);
  assert(table_count(t) == 0);
  assert(charP_int_TableSize(t) == 5);

  printf("OK\n");
  return 0;
}


