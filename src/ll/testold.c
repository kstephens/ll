
#ifndef __rcs_id__
#ifndef __rcs_id_ll_testold_c__
#define __rcs_id_ll_testold_c__
static const char __rcs_id_ll_testold_c[] = "$Id: testold.c,v 1.6 2008/05/25 22:27:39 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

static void print_v(int argc, ll_v *argv)
{
  int i = 0;

  printf("#%d(", argc);
  if ( i < argc ) {
    do {
      printf("%d", (int) ll_unbox_fixnum(argv[i]));
      if ( ++ i < argc ) {
	printf(" ");
      } else {
	break;
      }
    } while ( 1 );
  }
  printf(")\n");
}

ll_define_primitive(object,x, __0(),_0())
  printf("x: ");
  print_v(ll_ARGC, ll_ARGV);
  printf("ar_sp = %p, val_sp = %p, c_sp = %p\n", _ll_ar_sp, _ll_val_sp, alloca(0));
  ll_return(0);
ll_define_primitive_end

ll_define_primitive(object,y, __0(),_0())
  printf("y: ");
  print_v(ll_ARGC, ll_ARGV);
  printf("ar_sp = %p, val_sp = %p, c_sp = %p\n", _ll_ar_sp, _ll_val_sp, alloca(0));
  ll_return(1);
ll_define_primitive_end

static ll_v
  a = ll_make_fixnum(1),
  b = ll_make_fixnum(2),
  c = ll_make_fixnum(3),
  d = ll_make_fixnum(4);

ll_declare_primitive(object,foo);
ll_declare_primitive(object,bar);
ll_declare_primitive(object,baz);

ll_define_primitive(object,foo, __0(),_0())
{
  ll_call(ll_prim(object,x), _1(ll_call(ll_prim(object,y), _2(a, b))));
  ll_call_tail(ll_prim(object,baz), _2(b, c));
}
ll_define_primitive_end

ll_define_primitive(object,bar, __0(),_0())
{
  ll_call(ll_prim(object,y), _3(a, b, c));
  ll_call_tail(ll_prim(object,baz), _1(c));
}
ll_define_primitive_end

ll_define_primitive(object,baz, __0(),_0())
{
  ll_call(ll_prim(object,y), _2(b, c));
  ll_call_tail(ll_prim(object,foo), _1(d));
}
ll_define_primitive_end


int main(int argc, char **argv)
{
  ll_init(&argc, &argv);

  ll_call(ll_prim(object,foo), _1(0));

  return 0;
}

