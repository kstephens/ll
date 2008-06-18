#ifndef __rcs_id__
#ifndef __rcs_id_ll_named_c__
#define __rcs_id_ll_named_c__
static const char __rcs_id_ll_named_c[] = "$Id: named.c,v 1.15 2008/05/26 08:58:38 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "named.h"

#include <assert.h>
#include <string.h>


static struct {
  const char *n;
  ll_v v;
} map[128];
static int map_n;


#define BUF_SIZE 32

static char *get_buf()
{
  static char buf[16][BUF_SIZE];
  static int bufi;
  bufi = (bufi + 1) % (sizeof(buf) / sizeof(buf[0]));
  memset(buf[bufi], 0, sizeof(BUF_SIZE));
  return buf[bufi];
}


const char *ll_po_(ll_v x)
{
   int i;

   for ( i = 0; i < map_n; i ++ ) {
     if ( ll_EQ(map[i].v, x) )
       return map[i].n;
   }

   if ( ll_initialized ) {
     for ( i = 0; i < _ll_env_bindings_n; i ++ ) {
       ll_v binding = _ll_env_bindings[i];
       ll_v loc = ll_THIS_ISA(binding, binding)->_locative;
       if ( ll_ISA_locative(loc) && ll_EQ(*ll_UNBOX_locative(loc), x) ) {
	 ll_v sym = ll_THIS_ISA(binding, binding)->_symbol;
	 return ll_ptr_string(ll_THIS_ISA(symbol, sym)->_name);
       }
     }
   }

   return 0;
}


const char *ll_po(ll_v x)
{
  char *n, *n2;
  ll_v isa = ll_TYPE(x);

  if ( ll_EQ(x, ll_t) ) {
    return "#t";
  } else if ( ll_EQ(x, ll_f) ) {
    return "#f";
  } else if ( ll_EQ(x, ll_undef) ) {
    return "#u";
  } else if ( ll_EQ(x, ll_unspec) ) {
    return "#s";
  } else if ( ll_EQ(x, ll_nil) ) {
    return "()";
  } else if ( ll_EQ(isa, ll_type(fixnum)) ) {
    snprintf(n = get_buf(), BUF_SIZE, "%ld", (long) ll_unbox_fixnum(x));
  } else if ( ll_EQ(isa, ll_type(char)) ) {
    snprintf(n = get_buf(), BUF_SIZE, "#\\%c", (int) ll_unbox_char(x));
  } else if ( ll_EQ(isa, ll_type(string)) || ll_EQ(isa, ll_type(mutable_string)) ) {
    n2 = ll_ptr_string(x);
    snprintf(n = get_buf(), BUF_SIZE, "\"%s\"", n2 ? n2 : "");
    if ( n[BUF_SIZE - 1] ) {
      strcpy(n + BUF_SIZE - 1 - 4, "\"...");
    }
  } else if ( ll_EQ(isa, ll_type(symbol)) ) {
    n2 = (char*) _ll_name_symbol(x);
    if ( n2 ) {
      snprintf(n = get_buf(), BUF_SIZE, "'%s", n2);
    } else {
      goto other;
    }
  } else
  if ( ! (n = (char*) ll_po_(x)) ) {
     const char *tn;

  other:
     tn = ll_po_(isa);

     if ( tn ) {
       snprintf(n = get_buf(), BUF_SIZE, "#<%s #p%lu>", tn, (unsigned long) ll_UNBOX_ref(x));
       return n;
     }

     snprintf(n = get_buf(), BUF_SIZE, "#<#<type #p%lu> #p%lu>", (unsigned long) ll_UNBOX_ref(isa), (unsigned long) ll_UNBOX_ref(x));
   }

   return n;
}


void _ll_set_object_nameE(ll_v x, const char *name)
{
  assert(map_n < sizeof(map) / sizeof(map[0]));
  map[map_n].n = name;
  map[map_n].v = x;
  map_n ++;
}


