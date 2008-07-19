
#ifndef __rcs_id__
#ifndef __rcs_id_test_test_c__
#define __rcs_id_test_test_c__
static const char __rcs_id_test_test_c[] = "$Id: test.c,v 1.5 1999/04/22 05:05:00 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include "ct/ct.h"
#include "ct/ct_sym.h"

#include <stdlib.h>
#include <stdio.h>

static int testid = 0;
#define BT(S) printf("\nTest #%d: %s\n", ++ testid, S);

#define P(F,X) printf("%s = " F "\n", #X, (X))
 
static int _PT_errors = 0;
static void _PT(const char *a, const char *b, const char *c, const char *d, int ok, int expect)
{
  printf("%-15s: %s\t%s\t%s:\t%s\n", a, b, c, d, ok ? "TRUE" : "FALSE");
  if ( ok != expect ) {
    _PT_errors ++;
  }
}
 
#define PT(T,X,OP,Y) _PT(#T, #X, #OP, #Y, ((X) OP (Y)), ! 0);


int main(int argc, char **argv)
{
  const char *argv0 = argv[0];

  BT("ct_t_sizeof()");
#define CT(N,T) PT(N, ct_t_sizeof(ct_t_##N), ==, sizeof(T))
#define CT_NO_VOID
#include "ct/ct.def"

  BT("ct_t_alignof()");
#define CT(N,T) PT(N, ct_t_alignof(ct_t_##N), ==, ct_alignof(T))
#define CT_NO_VOID
#include "ct/ct.def"

  BT("ct_t_named()");
#define CT(N,T) PT(N, ct_t_named(#T), ==, ct_t_##N)
#include "ct/ct.def"

  BT("ct_t_named(*)");
#define CT(N,T) PT(N, ct_t_named(#T " *"), ==, ct_t_##N##P)
#include "ct/ct.def"

  BT("ct_t_named(**)");
#define CT(N,T) PT(N, ct_t_named(#T " **"), ==, ct_t_##N##PP)
#include "ct/ct.def"

  BT("ct_t_named(***)");
#define CT(N,T) PT(N, ct_t_named(#T " ***"), ==, ct_t_##N##PPP)
#include "ct/ct.def"

  BT("ct_t_struct()");
  {
    struct test1 {
      double a;
      char   b;
      int    c;
      void   *d;
    } t1;

    ct_t ct_t_test1;
    ct_t_test1 = ct_t_struct(0, "test1",
				 ct_t_double, "a",
				 ct_t_char,   "b",
				 ct_t_int,    "c",
				 ct_t_voidP,  "d",
				 0);

#define OFFSETOF(T,X) ((size_t)&(((T *)0)->X))*8

#define PO(N) \
    P("%d", OFFSETOF(struct test1, N));
    PO(a);
    PO(b);
    PO(c);
    PO(d);
#undef PO

    P("%lu", sizeof(struct test1));
    P("%lu", ct_t_sizeof(ct_t_test1));
    PT(ct_t_struct, ct_t_sizeof(ct_t_test1), ==, sizeof(struct test1));

#define PO(N) \
    PT(N, ct_t_element_offset(ct_t_test1, ct_t_name_index(ct_t_test1, #N)), ==, OFFSETOF(struct test1, N));
    PO(a);
    PO(b);
    PO(c);
    PO(d);
#undef P0

    P("'%s'", ct_t_declaration(ct_t_test1));
    P("'%s'", ct_t_definition(ct_t_test1));
  }

  BT("ct_sym_load_for_exe()");
  if ( argc > 1 ) {
    argv0 = argv[1];
  }
  P("%s", argv0);
  P("%d", ct_sym_load_for_exe(argv0));
  {
    ct_sym *s;

    P("%p", s = ct_sym_for_name("ct_t_name"));
    if ( s ) {
      PT(ct_t_named, s->addr, ==, (void*) &ct_t_name);
      P("%s", ct_t_name(s->type));
      P("%s", ct_t_definition(ct_t_element_type(s->type, 0)));
      P("%s", ct_t_definition(ct_t_named("struct ct_tdef")));
    }
  }

  BT("DONE");

  return _PT_errors ? 1 : 0;
}








